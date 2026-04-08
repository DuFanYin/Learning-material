# 时间轮（Timing Wheel）：高密度实现与工程取舍

面向“做过定时器/调度/IO 多路复用”的读者：本文不讲玩具思路，而是把时间轮当作一种**在固定精度下用数组环替代堆的事件映射器**来拆。

---

## 0. 先给结论：什么时候时间轮更划算

- 如果定时器数量很大、到期时间分布在某个合理范围内、且你接受“到期误差 <= tick”，时间轮能把大部分操作降到**摊还 O(1)**。
- 如果你需要强精度（tick 要极小）或者超大超时范围占比高，通常需要：
  - 分层时间轮（hierarchical timing wheel），或
  - 直接上最小堆/日历队列/分段结构。
- 关键比较维度不是 big-O 口号，而是：
  - 插入路径是否分配/加锁；
  - 过期处理是否形成热点 bucket；
  - 取消（cancel）是否导致链表抖动或内存碎片；
  - tick 推进是否由单线程/多线程驱动。

---

## 1. 定义：tick、槽（slot）、轮长与“旋转次数”

时间轮用一个环形数组 `buckets[0..N-1]` 存放**待触发定时器**。

参数：
- `tick`：时间精度（通常用 `1ms/10ms` 之类）
- `N`：槽数（建议取 2 的幂，方便用掩码 `idx & (N-1)`）
- `wheel_interval = N * tick`：一整圈覆盖的时间长度

核心映射：
- 以当前时间 `now` 为基准，令：
  - `delta = deadline - now`
  - `ticks = floor(delta / tick)` 或者用你定义的“向上取整策略”（见后文边界）
- 令当前槽指针为 `cur`（表示“下一次 tick 将处理哪个槽”），则目标槽：
  - `slot = (cur + ticks) % N`
- 但“slot 相同”不代表到期时刻相同，因为会绕圈：
  - 旋转次数 `rounds = ticks / N`（整数）
- 定时器条目里保存 `rounds`，当 wheel推进处理到该槽时：
  - `rounds > 0`：先减一（表示少绕一圈），不触发
  - `rounds == 0`：触发回调/投递任务

不变量（强烈建议写在代码注释里）：
- 对任意条目 `t`：它所在槽一定是“目标 tick 的槽”，且 `t.rounds` 表示还需跨过多少次完整 wheel 周期才会到期。
- 只有当 `cur` 指针推进并处理到该槽时，`rounds` 才会减少；因此 `rounds` 只会被该槽处理的单线程更新（在合理实现中）。

---

## 2. 数据结构：bucket 里放什么

一个高效的 bucket 通常是链表（或动态数组）：

- `TimerEntry`：每个定时器一个条目（最好可复用/池化）
  - `deadline_ticks`（可选：存相对 now 的 ticks）
  - `rounds`
  - `callback` 或 `task` 句柄
  - 取消标记（见下一节）
  - 链表指针（intrusive list）或索引
- `Bucket`：
  - `head` 指向 intrusive list 的头
  - 或者用 `std::vector<TimerEntry*>` 但要避免频繁扩容

工程要点：
- **避免每个 timer 都做一次 `new`**：allocator 抖动会吞掉轮的优势。
- **intrusive list** 能减少额外节点分配与缓存缺失；但要管理生命周期与并发访问。
- bucket 热点：tick 小、N 小、超时集中时会形成热点槽；链表遍历成本必须可控（摊还 O(1) 在这里体现为“每个条目最多被触发一次 + 被取消条目可能被扫到一次”。）

---

## 3. 插入（schedule）算法：把 deadline 映射成 slot + rounds

以条目 `t` 的 deadline 为例，假设使用单调时钟 `mono_now`（不要用 wall-clock）。

建议统一成“以 tick 为单位的整数时间轴”：
- `now_tick = mono_now / tick`
- `deadline_tick = deadline / tick`（向下/向上取整策略一致即可）

则：
- `ticks = deadline_tick - now_tick`
- 若 `ticks <= 0`：通常直接触发或放入“当前处理批次”（实现选择，但要一致）
- 否则：
  - `slot = (cur + ticks) & (N-1)`（如果 N 是 2 的幂）
  - `rounds = ticks >> log2(N)`

插入时把条目挂到 `buckets[slot]`。

为什么 rounds 必须存在：
- slot 只决定“绕圈后的相对位置”，不是绝对时刻；
- rounds 把“绕了几圈才到期”编码进条目里，避免全局重排或多结构索引。

复杂度：
- 插入：O(1)（不考虑取消清理）

---

## 4. 取消（cancel）与生命周期：懒删除 vs 物理删除

取消是时间轮工程里最容易“慢回堆”的地方。

两种主流策略：

### 4.1 懒删除（lazy cancel）
- 取消只做：
  - `entry->cancelled = true`
- 到 bucket 被处理时：
  - 发现 `cancelled` 就跳过回调并释放条目（或转回池）

优点：
- 取消是 O(1)，不会遍历链表删除。

代价：
- bucket 中残留被取消条目的内存与遍历开销。
- 取消比例极高时，需要重新评估策略（或引入定期清理/更细粒度分摊）。

### 4.2 物理删除（remove from list）
- 需要从 bucket 链表中定位并移除条目。
- 如果你能做到 intrusive list + entry 里存 `prev/next`，取消可以 O(1)（但要保证并发安全与指针一致性）。

代价：
- 并发条件更复杂（尤其当 wheel 线程可能正在遍历 bucket）。
- 更难避免 ABA/悬垂引用。

工程建议：
- 在大多数高性能实现里，单线程推进 wheel + 多线程只负责投递“新增 timer / cancel 请求”到队列，属于最简单可靠的组合：
  - wheel 线程独占操作 bucket
  - cancel 通过将请求入队实现（而不是直接修改 bucket 链表）

---

## 5. 推进（advance / tick）算法：每 tick 处理一个槽

典型结构（单线程 wheel）：
1. 获取当前槽指针 `cur`
2. 取出 `buckets[cur]` 链表
3. 遍历链表条目：
   - 若 `cancelled`：跳过并回收
   - else 若 `rounds > 0`：`rounds--`，并保留在 bucket 或转移到新桶
     - 更常见做法：rounds 不为 0 的条目仍留在原 bucket，等下个“完整圈”再扫（这要求你在遍历后把未到期条目挂回去；但如果 rounds > 0，下一次到期仍在同 slot，因此无需换桶，只需减 rounds）
   - else 执行回调
4. 清空 bucket
5. `cur = (cur + 1) % N`

“减 rounds 后还留在原槽”成立的原因：
- 因为当你处理 slot=cur 的时候，你代表的是“cur 对应的 tick 上界”。同一个 slot 未来再次被处理发生在 `N` 个 tick 之后，即完整一圈，因此 rounds 直接对应未来圈数衰减。

复杂度：
- 每 tick：遍历当槽内所有条目。
- 摊还：每个条目会随着 rounds 从初值减到 0，期间每次轮到该槽都会扫它一次；因此真正触发次数是 1，但“被扫描次数”取决于 rounds 值大小。
- 这也是为什么：
  - tick 粒度小会增加 ticks 数导致较大 rounds；
  - 定时器超时时间远时，最好上分层 wheel。

---

## 6. 边界与精度：deadline、向上/向下取整、漂移

### 6.1 deadline 已经过期
- 若 `deadline <= now`：立即触发（或放到“当前批次尾部”以避免递归栈）。

### 6.2 tick 取整策略
你必须明确：
- `deadline_tick = deadline / tick` 用整数除法是向下取整；
- 如果你要“deadline 在 tick 内只要到了就触发”，通常用向上取整（ceil）；
- 否则会出现系统性误差偏差（总是提前触发或总是延后触发）。

推荐工程做法：
- 使用同一套“单调 tick 时间轴”的向上取整策略，让误差保证在 `[0, tick)` 范围内且偏差单调。

### 6.3 漂移（drift）与 tick 驱动
- 不要用 `sleep(tick)` 累积误差。
- 使用绝对时间驱动：例如基于 `monotonic clock` 计算下一次应处理的目标 tick，或者用 timerfd/高精度时钟唤醒后“追赶到当前 tick”。

“追赶”策略：
- 如果 wheel 线程延迟了若干 tick：
  - 可以连续推进多个 tick，并逐槽处理到当前 tick 对齐；
  - 或者只推进一次并把“剩余 tick 差距”映射到 `cur` + rounds 调整。
- 前者逻辑最清晰，后者更复杂但可能减少回调批量造成的尾延迟。

---

## 7. 分层时间轮（Hierarchical Timing Wheel）：解决大超时

单层时间轮的问题：
- `rounds` 可能很大，导致条目被扫很多次（每圈一次）。

分层解决：
- L0：最细 tick，覆盖短时间段（例如 `N0 * tick0`）
- L1：更粗 tick（`tick1 = N0 * tick0` 或更大倍数），覆盖更长时间段
- 当 L0 完成一圈，把 L1 中“将落到 L0 时间轴”的条目重新下放到 L0（bucket 重分配）

工程实现要点：
- 只在“上层轮发生 wrap”时做一次 cascade 下放；
- 每个条目会被下放有限次数，因此总体扫描开销受控。

复杂度直觉：
- 摊还仍接近 O(1)（更准确地说：每条目被重新分配的次数与层数成正比，而层数是固定常数）

---

## 8. 与堆（min-heap）对比：你到底换了什么瓶颈

最小堆：
- 插入 O(log n)
- 取消：通常需要额外结构（lazy cancel 或可定位堆节点）
- 到期处理：每次 pop 最早到期者，可能导致频繁 log 调整

时间轮：
- 插入 O(1)
- 到期处理受 bucket 内部分布影响
- 如果定时器分布“均匀且在轮长范围内”，轮的常数项更小
- 如果分布高度偏斜（大量定时器集中在某段），bucket 可能形成长链表，尾延迟上升

工程结论：
- 时间轮是“换掉 log 的 CPU 成本”，用固定结构+tick 扫描换取可预测性；
- 是否划算取决于：
  - tick 粒度与 N 是否合理；
  - bucket 链长分布；
  - cancel 行为比例。

---

## 9. 并发实现：最常见的可靠组合

建议目标：让 wheel 线程独占结构状态，其他线程通过无锁/低锁方式提交请求。

常见方案：
- wheel 线程：
  - tick 推进
  - 处理 buckets
  - 执行回调（或把到期任务推送到线程池）
  - 负责 cancel 的最终生效（回收条目）
- producer 线程：
  - 调用 `schedule(deadline, cb)` / `cancel(handle)`
  - 把请求推入 MPSC 队列（或带锁队列）
  - wheel 线程在每个 tick 前把新增/cancel 请求批量 apply 到 buckets

为什么批量 apply：
- 减少锁竞争/原子操作；
- 改善缓存局部性（一次性更新多个 bucket）。

---

## 10. 选择参数的工程准则（经验版）

- `tick`：由可接受抖动决定（max_error <= tick），同时受系统调度延迟约束（过小 tick 反而会因为唤醒不准导致“追赶”成本上升）。
- `N`：让 `wheel_interval` 覆盖绝大多数常见超时时间分布，且尽量取 2 的幂。
- 如果超时时间跨越多个数量级：直接用分层，别指望单层 rounds 不大。
- 条目池化：
  - intrusive list + 自定义 allocator（freelist）能显著减少碎片与抖动。
- callback 执行：
  - 不要在 wheel 线程里做长回调；常见做法是把回调封装成任务投递到执行器（线程池/事件循环）。

---

## 11. 伪代码（单层、单线程推进）

```cpp
struct TimerEntry {
  int rounds;
  bool cancelled;
  Callback cb;
  TimerEntry* next; // intrusive
};

class TimingWheel {
public:
  TimingWheel(uint64_t tick_ns, size_t N)
    : tick(tick_ns), n(N), buckets(N), cur(0) {}

  TimerHandle schedule(uint64_t deadline_ns, Callback cb) {
    auto now = monotonic_now_ns();
    if (deadline_ns <= now) {
      // 可选：立即投递到任务队列
      return enqueue_immediate(cb);
    }

    uint64_t now_tick = now / tick;
    uint64_t dl_tick  = deadline_ns / tick;
    uint64_t ticks = dl_tick - now_tick;

    size_t slot = (cur + (ticks % n)) & (n - 1);
    int rounds  = (int)(ticks / n);

    auto* e = pool.allocate();
    e->rounds = rounds;
    e->cancelled = false;
    e->cb = std::move(cb);
    push_front(buckets[slot], e);
    return TimerHandle{e, generation++};
  }

  void tick_once(uint64_t /*target_time_ns*/) {
    auto& head = buckets[cur];
    TimerEntry* it = head;
    head = nullptr;

    while (it) {
      TimerEntry* nxt = it->next;
      if (it->cancelled) {
        pool.free(it);
      } else if (it->rounds > 0) {
        it->rounds -= 1;
        // 仍然在当前 slot 下次被处理，因此放回当前 bucket
        push_front(buckets[cur], it);
      } else {
        // 到期
        enqueue_callback(it->cb);
        pool.free(it);
      }
      it = nxt;
    }

    cur = (cur + 1) & (n - 1);
  }

private:
  uint64_t tick;
  size_t n;
  std::vector<TimerEntry*> buckets;
  size_t cur;
  EntryPool pool;
};
```

注意：真实工程会处理更多细节（cancel handle 的代际校验、MPSC 请求批量 apply、追赶 tick 的多次推进、层级 wheel cascade 等）。

---

## 12. 工程坑清单（高概率踩坑）

- `tick` 过小导致唤醒与追赶成本吞掉收益。
- deadline 使用 wall-clock（NTP/手动调整）导致定时器跳变。
- 取消直接修改链表导致并发遍历崩溃（尤其 wheel 线程正在扫 bucket）。
- 未池化导致频繁分配释放，时间轮的优势被 allocator 成本抵消。
- bucket 使用 `std::vector` 且反复增长收缩导致缓存与分支开销增大。
- callback 在 wheel 线程里同步执行引发 head-of-line blocking，导致到期延迟放大。
- rounds 计算溢出（deadline/ns 和 tick 的乘除要用 128 位/检查，尤其是极大超时时）。

---

## 13. 和你现有笔记的关联（用 OS 视角看）

时间轮是 OS/网络栈/事件循环里典型的：
- 把“按时间排序的动态集合”变成“按时间桶映射的近似集合”
- 用固定结构换取稳定的 CPU 时间分布，从而减少 tail latency 的抖动源（log 操作与不可预测分支）。

它和你前面学过的关键点同构：
- 时间事件处理的主要成本来自 cache/TLB 访问与锁竞争；
- tick 推进与并发模型决定了你到底在竞争什么资源。

