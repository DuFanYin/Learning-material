// Build:
//   mkdir -p build
//   clang++ -std=c++20 -O2 codes/src/oop/dynamic_virtual_example.cpp -o build/dynamic_virtual_example
// Run:
//   ./build/virtual_example
//
// 一个简单的虚函数/多态示例：基类 Shape，有 Circle/Rectangle 两个派生类，
// 通过基类指针调用虚函数 draw()/area()，展示动态绑定（dynamic dispatch）。

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;

    virtual void draw() const = 0;
    virtual double area() const = 0;
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}

    void draw() const override {
        std::cout << "Drawing Circle, r = " << r_ << "\n";
    }

    double area() const override {
        return M_PI * r_ * r_;
    }

private:
    double r_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}

    void draw() const override {
        std::cout << "Drawing Rectangle, w = " << w_ << ", h = " << h_ << "\n";
    }

    double area() const override {
        return w_ * h_;
    }

private:
    double w_;
    double h_;
};

int main() {
    std::vector<std::unique_ptr<Shape>> shape_list;
    shape_list.emplace_back(std::make_unique<Circle>(1.0));
    shape_list.emplace_back(std::make_unique<Rectangle>(2.0, 3.0));
    shape_list.emplace_back(std::make_unique<Circle>(2.5));

    double total_area = 0.0;
    for (const auto& shape_ptr : shape_list) {
        shape_ptr->draw();               // 动态绑定到具体类型的 draw()
        total_area += shape_ptr->area(); // 动态绑定到具体类型的 area()
    }

    std::cout << "Total area = " << total_area << "\n";
    return 0;
}

