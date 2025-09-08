/**
 * C++11 右值引用与移动语义深度解析
 * 
 * 核心概念：
 * 1. 值类别系统重构（lvalue, xvalue, prvalue）
 * 2. 移动构造函数和移动赋值运算符
 * 3. 完美转发机制
 * 4. 引用折叠规则
 */

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <type_traits>
#include <memory>
#include <chrono>

// ===== 1. 值类别系统演示 =====

void demonstrate_value_categories() {
    std::cout << "=== 值类别系统演示 ===\n";
    
    int x = 10;
    int& lref = x;           // lvalue reference
    int&& rref = 20;         // rvalue reference
    
    // lvalue: 有身份，不可移动
    std::cout << "lvalue x: " << x << std::endl;
    std::cout << "lvalue reference: " << lref << std::endl;
    
    // prvalue: 无身份，可移动（纯右值）
    std::cout << "prvalue: " << 10 + 20 << std::endl;
    
    // xvalue: 有身份，可移动（即将过期的值）
    std::cout << "xvalue (std::move): " << std::move(x) << std::endl;
    
    // 函数返回值的分类
    auto get_int = []() -> int { return 42; };        // 返回prvalue
    auto get_ref = [&]() -> int& { return x; };       // 返回lvalue
    auto get_rref = []() -> int&& { return 42; };     // 返回xvalue
    
    std::cout << "Function return prvalue: " << get_int() << std::endl;
    std::cout << "Function return lvalue: " << get_ref() << std::endl;
    std::cout << "Function return xvalue: " << get_rref() << std::endl;
}

// ===== 2. 移动语义的深入实现 =====

class Resource {
private:
    int* data;
    size_t size;
    std::string name;
    
public:
    // 构造函数
    explicit Resource(size_t size = 1000, const std::string& name = "Resource") 
        : size(size), name(name) {
        data = new int[size];
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<int>(i);
        }
        std::cout << "构造 " << name << " (size: " << size << ")\n";
    }
    
    // 拷贝构造函数（深拷贝，性能开销大）
    Resource(const Resource& other) 
        : size(other.size), name(other.name + "_copy") {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
        std::cout << "拷贝构造 " << name << " (深拷贝开销大)\n";
    }
    
    // 移动构造函数（窃取资源，高性能）
    Resource(Resource&& other) noexcept 
        : data(other.data), size(other.size), name(std::move(other.name)) {
        // 窃取资源，将源对象置为安全状态
        other.data = nullptr;
        other.size = 0;
        name += "_moved";
        std::cout << "移动构造 " << name << " (高性能资源窃取)\n";
    }
    
    // 拷贝赋值运算符
    Resource& operator=(const Resource& other) {
        if (this != &other) {
            std::cout << "拷贝赋值 " << other.name << " -> " << name << "\n";
            
            // 释放当前资源
            delete[] data;
            
            // 深拷贝新资源
            size = other.size;
            name = other.name + "_assigned";
            data = new int[size];
            std::copy(other.data, other.data + size, data);
        }
        return *this;
    }
    
    // 移动赋值运算符
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            std::cout << "移动赋值 " << other.name << " -> " << name << "\n";
            
            // 释放当前资源
            delete[] data;
            
            // 窃取资源
            data = other.data;
            size = other.size;
            name = std::move(other.name) + "_move_assigned";
            
            // 将源对象置为安全状态
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    // 析构函数
    ~Resource() {
        if (data) {
            std::cout << "析构 " << name << "\n";
            delete[] data;
        } else {
            std::cout << "析构空对象 " << name << "\n";
        }
    }
    
    // 访问接口
    size_t get_size() const { return size; }
    const std::string& get_name() const { return name; }
    int& operator[](size_t index) { return data[index]; }
    const int& operator[](size_t index) const { return data[index]; }
    
    // 检查对象是否有效
    bool is_valid() const { return data != nullptr; }
};

void demonstrate_move_semantics() {
    std::cout << "\n=== 移动语义演示 ===\n";
    
    // 1. 构造和拷贝
    Resource r1(100, "Original");
    Resource r2 = r1;  // 拷贝构造
    
    std::cout << "\n--- 移动构造测试 ---\n";
    Resource r3 = std::move(r1);  // 移动构造
    std::cout << "r1 after move: valid=" << r1.is_valid() << std::endl;
    std::cout << "r3 after move: valid=" << r3.is_valid() << ", name=" << r3.get_name() << std::endl;
    
    std::cout << "\n--- 移动赋值测试 ---\n";
    Resource r4(50, "Target");
    r4 = std::move(r2);  // 移动赋值
    std::cout << "r2 after move: valid=" << r2.is_valid() << std::endl;
    std::cout << "r4 after move: valid=" << r4.is_valid() << ", name=" << r4.get_name() << std::endl;
}

// ===== 3. 完美转发机制 =====

// 模拟工厂函数，需要完美转发参数
template<typename T, typename... Args>
std::unique_ptr<T> make_unique_perfect(Args&&... args) {
    std::cout << "完美转发构造参数...\n";
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 测试完美转发的函数
template<typename T>
void process_value(T&& param) {
    using pure_type = typename std::remove_reference<T>::type;
    
    std::cout << "接收到参数类型: ";
    if (std::is_lvalue_reference<T>::value) {
        std::cout << "左值引用";
    } else if (std::is_rvalue_reference<T>::value) {
        std::cout << "右值引用";
    } else {
        std::cout << "值类型";
    }
    std::cout << std::endl;
    
    // 完美转发给另一个函数
    auto resource = make_unique_perfect<Resource>(std::forward<T>(param));
    std::cout << "创建的资源名称: " << resource->get_name() << std::endl;
}

// 引用折叠规则演示
template<typename T>
void demonstrate_reference_collapsing() {
    std::cout << "\n=== 引用折叠规则演示 ===\n";
    
    // T& + & = T&
    // T& + && = T&  
    // T&& + & = T&
    // T&& + && = T&&
    
    std::cout << "引用折叠规则:\n";
    std::cout << "T& + & = T&\n";
    std::cout << "T& + && = T&\n";  
    std::cout << "T&& + & = T&\n";
    std::cout << "T&& + && = T&&\n";
    
    int x = 42;
    
    std::cout << "\n完美转发测试:\n";
    process_value(x);                    // 传递左值
    process_value(std::move(x));         // 传递右值
    process_value(Resource(10, "temp")); // 传递临时对象
}

// ===== 4. std::move的实现原理 =====

// 简化版std::move实现
template<typename T>
constexpr typename std::remove_reference<T>::type&& my_move(T&& param) noexcept {
    using return_type = typename std::remove_reference<T>::type&&;
    return static_cast<return_type>(param);
}

// std::forward的实现原理
template<typename T>
constexpr T&& my_forward(typename std::remove_reference<T>::type& param) noexcept {
    return static_cast<T&&>(param);
}

template<typename T>
constexpr T&& my_forward(typename std::remove_reference<T>::type&& param) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "Cannot forward rvalue as lvalue");
    return static_cast<T&&>(param);
}

void demonstrate_move_forward_implementation() {
    std::cout << "\n=== std::move和std::forward实现原理 ===\n";
    
    Resource r1(50, "ForMoveTest");
    
    std::cout << "使用自定义my_move:\n";
    Resource r2 = my_move(r1);
    std::cout << "r1 after my_move: valid=" << r1.is_valid() << std::endl;
    
    // 演示完美转发的必要性
    auto wrapper = [](auto&& param) {
        std::cout << "使用my_forward转发:\n";
        return Resource(my_forward<decltype(param)>(param));
    };
    
    Resource r3(30, "ForwardTest");
    auto r4 = wrapper(std::move(r3));
    std::cout << "r3 after forward: valid=" << r3.is_valid() << std::endl;
}

// ===== 5. 移动语义的性能测试 =====

void performance_comparison() {
    std::cout << "\n=== 移动语义性能对比 ===\n";
    
    const int iterations = 1000;
    const size_t resource_size = 10000;
    
    // 测试拷贝性能
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::vector<Resource> vec;
        for (int i = 0; i < iterations; ++i) {
            Resource r(resource_size, "PerfTest_" + std::to_string(i));
            vec.push_back(r);  // 拷贝构造
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto copy_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 测试移动性能
    start = std::chrono::high_resolution_clock::now();
    {
        std::vector<Resource> vec;
        for (int i = 0; i < iterations; ++i) {
            Resource r(resource_size, "PerfTest_" + std::to_string(i));
            vec.push_back(std::move(r));  // 移动构造
        }
    }
    end = std::chrono::high_resolution_clock::now();
    auto move_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "拷贝构造时间: " << copy_time.count() << "ms\n";
    std::cout << "移动构造时间: " << move_time.count() << "ms\n";
    std::cout << "性能提升: " << (static_cast<double>(copy_time.count()) / move_time.count()) << "x\n";
}

// ===== 6. 容器中的移动语义应用 =====

void demonstrate_container_move_semantics() {
    std::cout << "\n=== 容器中的移动语义应用 ===\n";
    
    std::vector<Resource> vec;
    
    std::cout << "1. push_back拷贝:\n";
    Resource r1(100, "Container_Copy");
    vec.push_back(r1);  // 拷贝
    
    std::cout << "\n2. push_back移动:\n";
    Resource r2(100, "Container_Move");
    vec.push_back(std::move(r2));  // 移动
    
    std::cout << "\n3. emplace_back原地构造:\n";
    vec.emplace_back(100, "Container_Emplace");  // 原地构造，最高效
    
    std::cout << "\n4. 容器移动:\n";
    std::vector<Resource> vec2 = std::move(vec);  // 整个容器移动
    std::cout << "原容器大小: " << vec.size() << std::endl;
    std::cout << "新容器大小: " << vec2.size() << std::endl;
}

// ===== 7. 返回值优化与移动语义 =====

Resource create_resource_copy() {
    Resource r(50, "ReturnCopy");
    return r;  // 可能触发RVO或移动构造
}

Resource create_resource_move() {
    Resource r(50, "ReturnMove");  
    return std::move(r);  // 强制移动（不推荐，阻碍RVO）
}

void demonstrate_return_value_optimization() {
    std::cout << "\n=== 返回值优化与移动语义 ===\n";
    
    std::cout << "1. 普通返回（可能RVO）:\n";
    auto r1 = create_resource_copy();
    
    std::cout << "\n2. 强制移动返回（阻碍RVO）:\n";
    auto r2 = create_resource_move();
    
    std::cout << "\n3. 临时对象返回:\n";
    auto r3 = Resource(50, "TempReturn");
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 右值引用与移动语义深度解析\n";
    std::cout << "=====================================\n";
    
    // 值类别系统
    demonstrate_value_categories();
    
    // 移动语义基础
    demonstrate_move_semantics();
    
    // 完美转发
    demonstrate_reference_collapsing();
    
    // move和forward实现
    demonstrate_move_forward_implementation();
    
    // 性能对比
    performance_comparison();
    
    // 容器应用
    demonstrate_container_move_semantics();
    
    // 返回值优化
    demonstrate_return_value_optimization();
    
    std::cout << "\n程序结束 - 观察析构函数调用顺序\n";
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 01_rvalue_references_move_semantics.cpp -o move_semantics
./move_semantics

关键学习点:
1. 理解值类别系统的重构对性能的影响
2. 掌握移动构造和移动赋值的实现技巧
3. 深入理解完美转发的必要性和实现原理
4. 学会在实际项目中合理使用移动语义
5. 注意移动语义与RVO的关系和权衡
*/