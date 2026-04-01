# 网络基础高密度总结

## 一个请求到底发生了什么

```
浏览器 → DNS查询(域名→IP) → TCP三次握手 → HTTP请求 → 服务器处理 → HTTP响应 → TCP关闭
```

**RTT（Round Trip Time）**：数据从发出到收到确认的时间。是衡量网络延迟的基本单位。一次HTTP请求至少需要 1 RTT握手 + 1 RTT请求响应。

**packet如何路由**：数据被分割为packet，每个packet独立经过路由器逐跳转发（基于目标IP查路由表），到达目的地后重组。

---

## TCP三次握手（为什么要三次）

```
Client  →  SYN(seq=x)          →  Server   # Client告诉Server：我想连，我的序号是x
Client  ←  SYN-ACK(seq=y,ack=x+1) ←  Server  # Server确认：好的，我的序号是y
Client  →  ACK(ack=y+1)        →  Server   # Client确认：收到
```

**为什么三次**：两次握手Server无法确认Client收到了Server的SYN-ACK（防止旧的重复连接请求造成混乱）。三次是建立双向可靠信道的最少次数。

**SYN**：同步序列号，发起连接请求。**ACK**：确认号，确认收到数据。

---

## TCP vs UDP

| | TCP | UDP |
|---|---|---|
| 连接 | 有连接（握手） | 无连接 |
| 可靠性 | 保证送达、有序 | 不保证 |
| 延迟 | 高（握手+重传+拥塞控制） | 低 |
| 适用 | HTTP、文件传输 | HFT行情、DNS、视频流 |

**TCP本质**：可靠 + 有序，代价是延迟。

**HFT/market data用UDP**：行情数据时效性极强，旧数据没价值，丢了就丢了，绝不能为等重传而延迟后续数据。

---

## TCP核心机制

**滑动窗口**：发送方不等每个ACK就连续发多个packet，窗口大小决定"在途未确认数据量"。窗口越大，吞吐越高，但内存占用增加。

**重传机制**：超时未收到ACK则重传。重传导致延迟尖刺（为什么TCP会"卡"）。

**拥塞控制 / Slow Start**：
- 连接建立初期，拥塞窗口(cwnd)从1开始，每个RTT翻倍（指数增长）
- 遇到丢包则大幅降窗，再缓慢增长
- 目的：探测网络容量，避免把网络打爆

**四次挥手**：
```
Client → FIN → Server
Client ← ACK ← Server
Client ← FIN ← Server
Client → ACK → Server  （等2MSL后才真正关闭，进入TIME_WAIT）
```

---

## Socket与系统接口

**socket是什么**：socket是文件描述符（fd），内核为其分配发送/接收缓冲区。网络通信本质是读写这个fd。

**send/recv实际做了什么**：
- `send()`：把数据从用户空间**拷贝**到内核发送缓冲区，由内核决定何时发出
- `recv()`：把内核接收缓冲区的数据**拷贝**到用户空间

**关键认知：网络IO ≠ 直接发数据，而是写入kernel buffer，内核异步处理实际发送。**

**blocking vs non-blocking**：
- blocking：`recv()`没数据时线程挂起，等待。简单但一个线程只能服务一个连接。
- non-blocking：`recv()`没数据时立即返回`EAGAIN`。需要配合epoll使用。

---

## I/O模型

**select/poll**：遍历所有fd检查是否ready，O(n)复杂度，连接多时性能差。

**epoll**（重点）：
- 内核维护就绪列表，只通知ready的fd
- `epoll_wait()`返回的直接是有事件的fd集合，O(1)
- **epoll = "告诉你谁ready"，而不是你去轮询**

**event-driven模型**：单线程 + epoll 可处理数万并发连接（Nginx、Redis的核心）。

**epoll vs thread-per-connection**：
- thread-per-connection：每连接一个线程，线程切换开销大，内存占用高，连接数上去就崩
- epoll：单线程处理所有连接，无切换开销，scalable

---

## 延迟从哪里来

```
latency ≈ syscall耗时 + 内存拷贝 + 网络传输 + 队列等待
```

**数据完整路径**：
```
应用代码 → syscall(send) → 内核缓冲区 → 驱动 → NIC → 网络 → 对方NIC → 内核缓冲区 → syscall(recv) → 对方应用
```

**关键瓶颈**：
- **syscall**：用户态/内核态上下文切换，约100-1000ns
- **memory copy**：用户空间↔内核空间至少2次拷贝
- **cache miss**：数据不在L1/L2 cache，访问内存延迟~100ns
- **queueing**：NIC队列、内核调度队列堆积

**zero-copy**：`sendfile()`等系统调用跳过用户空间，数据直接从内核缓冲区到NIC，减少2次拷贝，降低CPU开销和延迟。

**DPDK（kernel bypass）**：应用直接控制NIC，完全绕过内核网络栈，消除syscall开销。延迟可从微秒级降至百纳秒级。用于HFT、电信等极低延迟场景。

---

## 真实协议

**HTTP**：无状态请求/响应协议。每次请求独立，简单但短连接有握手开销。HTTP/1.1引入keep-alive复用TCP连接。

**WebSocket**：在HTTP握手后升级为全双工长连接。服务器可主动推送，无需轮询，适合实时数据。

**FIX（Financial Information eXchange）**：金融行业标准消息协议。基于TCP，文本格式（tag=value），低延迟版本用二进制（FAST/SBE）。设计目标：可靠、可审计，而非极致低延迟。

**latency tradeoff**：HTTP每次重建连接（高延迟）→ WebSocket长连接（低延迟）→ UDP裸包（最低延迟，无可靠性）。

---

## 系统观察工具

```bash
ss -ant          # 查看所有TCP连接状态（比netstat快）
ss -s            # 连接统计汇总
tcpdump -i eth0 'tcp port 8080'  # 抓包
top / htop       # CPU使用率，观察软中断(si)比例
```

**为什么连接多了会慢**：每个连接占用内存（socket缓冲区约4-8KB×2），大量连接导致内存压力、cache miss增加；epoll本身O(1)不慢，慢在内存和处理逻辑。

**TIME_WAIT**：主动关闭方在发出最后一个ACK后等待2MSL（约60s）。目的：确保对方收到ACK；防止旧连接的迷途packet干扰新连接。大量TIME_WAIT会耗尽端口（`net.ipv4.tcp_tw_reuse=1`可缓解）。

**backlog**：`listen(fd, backlog)`中的backlog是已完成三次握手但未被`accept()`的连接队列长度。backlog满了，新连接的SYN-ACK会被丢弃，客户端超时重试。高并发服务需调大此值及内核参数`net.core.somaxconn`。

---

## 核心问题答案

**1. TCP为什么不适合market data**
行情数据有时效性，1ms前的价格可能已无效。TCP重传导致后续数据被阻塞（Head-of-Line Blocking），宁可丢包也不能等重传。

**2. UDP为什么快但不可靠**
无握手（省1RTT）、无重传（无等待延迟）、无拥塞控制（不降速）。代价：packet可能丢失、乱序、重复，应用层自己处理。

**3. epoll为什么比thread-per-connection好**
线程切换耗时~1-10μs，且每线程栈占用~8MB内存。1万并发=1万线程=80GB内存。epoll单线程+事件驱动，无切换开销，连接数只受内存限制。

**4. 数据在系统里拷贝了几次**
标准路径：用户buf→内核发送buf（1次）→NIC DMA（不算CPU拷贝）→对方NIC→内核接收buf→用户buf（1次）。共2次CPU拷贝。`sendfile`可减至0次用户空间拷贝。

**5. latency最大头在哪**
局域网内：syscall + 内存拷贝（微秒级）占主导。跨城市/跨国：光速传播延迟（几十毫秒）占主导。高并发时：queueing delay（排队等待）可能成为最大瓶颈。

---

## 一句话

**网络的核心不是协议，而是理解数据如何流动 + 哪里会变慢。**
