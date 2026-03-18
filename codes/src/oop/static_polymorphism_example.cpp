// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 codes/src/oop/static_polymorphism_example.cpp -o build/static_polymorphism_example
// Run:
//   ./build/static_polymorphism_example
//
// 一个“静态多态”（template/CRTP）示例：在编译期根据类型选择实现，
// 不使用 virtual，调用在编译期静态解析（static dispatch）。

#include <cmath>
#include <iostream>
#include <vector>

// 约定接口：所有形状类型都提供：
//   - void draw() const;
//   - double area() const;

struct CircleStatic {
    explicit CircleStatic(double radius) : radius_(radius) {}

    void draw() const {
        std::cout << "Drawing CircleStatic, r = " << radius_ << "\n";
    }

    double area() const {
        return M_PI * radius_ * radius_;
    }

private:
    double radius_;
};

struct RectangleStatic {
    RectangleStatic(double width, double height) : width_(width), height_(height) {}

    void draw() const {
        std::cout << "Drawing RectangleStatic, w = " << width_ << ", h = " << height_ << "\n";
    }

    double area() const {
        return width_ * height_;
    }

private:
    double width_;
    double height_;
};

// 一个模板函数，接受任何“看起来像 Shape”的类型（duck typing），
// 调用在编译期静态解析，不需要虚表。
template <typename Shape>
void render_and_accumulate(const Shape& shape, double& total_area) {
    shape.draw();               // 静态解析到具体类型的 draw()
    total_area += shape.area(); // 静态解析到具体类型的 area()
}

int main() {
    std::vector<CircleStatic> circle_list;
    std::vector<RectangleStatic> rectangle_list;

    circle_list.emplace_back(1.0);
    circle_list.emplace_back(2.5);
    rectangle_list.emplace_back(2.0, 3.0);

    double total_area = 0.0;

    for (const auto& circle : circle_list) {
        render_and_accumulate(circle, total_area);
    }
    for (const auto& rectangle : rectangle_list) {
        render_and_accumulate(rectangle, total_area);
    }

    std::cout << "Total area (static polymorphism) = " << total_area << "\n";
    return 0;
}

