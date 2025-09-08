/**
 * C++11 统一初始化列表深度解析
 * 
 * 核心概念：
 * 1. 大括号初始化的语法统一和优先级规则
 * 2. initializer_list的实现机制和性能考量
 * 3. 窄化转换的预防和类型安全
 * 4. 与构造函数重载决议的交互
 * 5. 自定义类型的initializer_list支持
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <initializer_list>
#include <algorithm>
#include <memory>
#include <complex>

// ===== 1. 统一初始化语法基础 =====

void demonstrate_basic_uniform_initialization() {
    std::cout << "=== 统一初始化语法基础演示 ===\n";
    
    // 1. 内置类型的统一初始化
    int a{42};              // 等价于 int a = 42;
    double b{3.14};         // 等价于 double b = 3.14;
    char c{'A'};            // 等价于 char c = 'A';
    bool d{true};           // 等价于 bool d = true;
    
    std::cout << "基本类型初始化: a=" << a << ", b=" << b 
              << ", c=" << c << ", d=" << d << std::endl;
    
    // 2. 数组的统一初始化
    int arr1[5]{1, 2, 3, 4, 5};                    // C++11新语法
    int arr2[]{10, 20, 30};                        // 自动推导大小
    int arr3[5]{1, 2};                             // 剩余元素零初始化
    
    std::cout << "数组初始化:\n";
    std::cout << "arr1: ";
    for (int i : arr1) std::cout << i << " ";
    std::cout << "\narr2: ";
    for (int i : arr2) std::cout << i << " ";
    std::cout << "\narr3: ";
    for (int i : arr3) std::cout << i << " ";
    std::cout << std::endl;
    
    // 3. 指针和引用的初始化
    int value = 100;
    int* ptr{&value};       // 指针初始化
    int& ref{value};        // 引用初始化
    
    std::cout << "指针和引用: *ptr=" << *ptr << ", ref=" << ref << std::endl;
    
    // 4. 聚合类型的初始化
    struct Point {
        int x, y;
    };
    
    Point p1{10, 20};       // 聚合初始化
    Point p2{.x = 30, .y = 40};  // C++20指定初始化器（这里仅演示概念）
    
    std::cout << "结构体初始化: p1(" << p1.x << "," << p1.y << ")" << std::endl;
    
    // 5. 复杂类型的初始化
    std::string str{"Hello, C++11!"};
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::map<std::string, int> map{{"one", 1}, {"two", 2}, {"three", 3}};
    
    std::cout << "复杂类型初始化:\n";
    std::cout << "string: " << str << std::endl;
    std::cout << "vector: ";
    for (int i : vec) std::cout << i << " ";
    std::cout << "\nmap: ";
    for (const auto& pair : map) {
        std::cout << "{" << pair.first << ":" << pair.second << "} ";
    }
    std::cout << std::endl;
    
    std::cout << "\n";
}

// ===== 2. 窄化转换的预防 =====

void demonstrate_narrowing_conversion_prevention() {
    std::cout << "=== 窄化转换预防演示 ===\n";
    
    // 传统初始化允许窄化转换
    int traditional1 = 3.14;         // 允许，但有精度丢失
    int traditional2 = 300;
    char traditional_char = traditional2;  // 允许，但可能溢出
    
    std::cout << "传统初始化结果: int from double=" << traditional1 
              << ", char from int=" << static_cast<int>(traditional_char) << std::endl;
    
    // 大括号初始化阻止窄化转换
    // int narrowing1{3.14};         // 编译错误！浮点到整数的窄化
    // char narrowing2{300};         // 编译错误！整数值超出char范围
    
    // 安全的初始化方式
    int safe1{3};                    // OK，没有窄化
    char safe2{100};                 // OK，在char范围内
    double safe3{3};                 // OK，扩展转换
    
    std::cout << "安全初始化: " << safe1 << ", " << static_cast<int>(safe2) 
              << ", " << safe3 << std::endl;
    
    // 显示窄化检查的实际应用
    auto check_narrowing = [](auto value, const char* type_name) {
        std::cout << "检查 " << type_name << " 的窄化转换:\n";
        
        // 这些会编译成功
        try {
            // 使用static_cast进行显式转换
            if constexpr (std::is_same_v<decltype(value), double>) {
                int converted = static_cast<int>(value);
                std::cout << "  显式转换: " << value << " -> " << converted << std::endl;
            }
        } catch (...) {
            std::cout << "  转换异常" << std::endl;
        }
    };
    
    check_narrowing(3.14159, "double");
    check_narrowing(1000, "int");
    
    // 模板中的窄化检查
    auto safe_convert = [](auto from) {
        using FromType = std::decay_t<decltype(from)>;
        
        if constexpr (std::is_floating_point_v<FromType>) {
            // 浮点数转换需要显式处理
            return static_cast<int>(from);
        } else {
            // 整数类型可以直接返回
            return from;
        }
    };
    
    std::cout << "模板安全转换: " << safe_convert(3.14) << ", " << safe_convert(42) << std::endl;
    
    std::cout << "\n";
}

// ===== 3. initializer_list的实现机制 =====

// 自定义支持initializer_list的类
template<typename T>
class SimpleVector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;
    
    void reallocate(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
    
public:
    // 默认构造函数
    SimpleVector() : data_(nullptr), size_(0), capacity_(0) {}
    
    // initializer_list构造函数
    SimpleVector(std::initializer_list<T> init) 
        : size_(init.size()), capacity_(init.size()) {
        data_ = new T[capacity_];
        std::copy(init.begin(), init.end(), data_);
        std::cout << "使用initializer_list构造，大小: " << size_ << std::endl;
    }
    
    // 传统构造函数
    explicit SimpleVector(size_t count, const T& value = T{}) 
        : size_(count), capacity_(count) {
        data_ = new T[capacity_];
        std::fill_n(data_, size_, value);
        std::cout << "使用传统构造函数，大小: " << size_ << std::endl;
    }
    
    // 拷贝构造函数
    SimpleVector(const SimpleVector& other) 
        : size_(other.size_), capacity_(other.capacity_) {
        data_ = new T[capacity_];
        std::copy(other.data_, other.data_ + size_, data_);
    }
    
    // 移动构造函数
    SimpleVector(SimpleVector&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    ~SimpleVector() {
        delete[] data_;
    }
    
    // initializer_list赋值
    SimpleVector& operator=(std::initializer_list<T> init) {
        if (init.size() > capacity_) {
            delete[] data_;
            capacity_ = init.size();
            data_ = new T[capacity_];
        }
        size_ = init.size();
        std::copy(init.begin(), init.end(), data_);
        return *this;
    }
    
    // 访问接口
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    size_t size() const { return size_; }
    
    // 迭代器支持
    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }
    
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_++] = value;
    }
};

void demonstrate_initializer_list_mechanism() {
    std::cout << "=== initializer_list实现机制演示 ===\n";
    
    // 1. 基本使用
    SimpleVector<int> vec1{1, 2, 3, 4, 5};
    SimpleVector<int> vec2(5, 42);  // 使用传统构造函数
    
    std::cout << "vec1内容: ";
    for (const auto& item : vec1) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    std::cout << "vec2内容: ";
    for (const auto& item : vec2) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    // 2. initializer_list赋值
    vec1 = {10, 20, 30};
    std::cout << "重新赋值后vec1: ";
    for (const auto& item : vec1) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    // 3. 嵌套初始化列表
    SimpleVector<SimpleVector<int>> nested{
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    std::cout << "嵌套初始化列表:\n";
    for (const auto& row : nested) {
        for (const auto& item : row) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
    
    // 4. initializer_list的性能特性
    std::cout << "\ninitializer_list性能特性:\n";
    
    auto process_list = [](std::initializer_list<int> list) {
        std::cout << "处理列表，大小: " << list.size() 
                  << "，存储地址: " << static_cast<const void*>(list.begin()) << std::endl;
        
        // initializer_list是轻量级的，只包含指针和大小
        std::cout << "initializer_list对象大小: " << sizeof(list) << " bytes" << std::endl;
        
        // 数据存储在静态存储区，不可修改
        for (auto item : list) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    };
    
    process_list({100, 200, 300, 400});
    
    std::cout << "\n";
}

// ===== 4. 构造函数重载决议 =====

class ConstructorOverloadDemo {
private:
    std::vector<int> data_;
    std::string name_;
    
public:
    // 1. 默认构造函数
    ConstructorOverloadDemo() : name_("default") {
        std::cout << "默认构造函数" << std::endl;
    }
    
    // 2. initializer_list构造函数
    ConstructorOverloadDemo(std::initializer_list<int> init) 
        : data_(init), name_("initializer_list") {
        std::cout << "initializer_list构造函数，元素个数: " << init.size() << std::endl;
    }
    
    // 3. 传统参数构造函数
    ConstructorOverloadDemo(size_t count, int value) 
        : data_(count, value), name_("count_value") {
        std::cout << "count+value构造函数，count=" << count << ", value=" << value << std::endl;
    }
    
    // 4. 单参数构造函数
    explicit ConstructorOverloadDemo(int single_value) 
        : data_{single_value}, name_("single_value") {
        std::cout << "单参数构造函数，value=" << single_value << std::endl;
    }
    
    // 5. 字符串构造函数
    explicit ConstructorOverloadDemo(const std::string& str) 
        : name_(str) {
        std::cout << "字符串构造函数，name=" << str << std::endl;
    }
    
    // 访问接口
    const std::vector<int>& get_data() const { return data_; }
    const std::string& get_name() const { return name_; }
    
    void print() const {
        std::cout << "对象: " << name_ << ", 数据: [";
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i];
            if (i < data_.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

void demonstrate_constructor_overload_resolution() {
    std::cout << "=== 构造函数重载决议演示 ===\n";
    
    std::cout << "1. 不同初始化方式的构造函数选择:\n";
    
    // 空初始化 - 调用默认构造函数
    ConstructorOverloadDemo obj1;
    ConstructorOverloadDemo obj2{};  // 也是调用默认构造函数
    
    // initializer_list优先
    ConstructorOverloadDemo obj3{1, 2, 3, 4, 5};  // initializer_list构造函数
    
    // 传统构造函数调用（避免大括号）
    ConstructorOverloadDemo obj4(3, 42);  // count+value构造函数
    
    // 单参数构造函数
    ConstructorOverloadDemo obj5{100};    // initializer_list构造函数（单元素）
    ConstructorOverloadDemo obj6(200);    // 单参数构造函数
    
    // 字符串构造函数
    ConstructorOverloadDemo obj7{"hello"};  // 字符串构造函数
    
    std::cout << "\n2. 各对象的状态:\n";
    obj1.print();
    obj2.print();
    obj3.print();
    obj4.print();
    obj5.print();
    obj6.print();
    obj7.print();
    
    // 特殊情况：空的初始化列表
    std::cout << "\n3. 特殊情况测试:\n";
    
    // 空初始化列表仍然调用initializer_list构造函数
    ConstructorOverloadDemo empty_list({});
    empty_list.print();
    
    // 显式避免initializer_list构造函数
    ConstructorOverloadDemo explicit_count(static_cast<size_t>(5), 10);
    explicit_count.print();
    
    std::cout << "\n";
}

// ===== 5. Most Vexing Parse问题的解决 =====

class Timer {
public:
    Timer() {
        std::cout << "Timer构造" << std::endl;
    }
    
    void start() {
        std::cout << "Timer开始" << std::endl;
    }
};

class Widget {
public:
    explicit Widget(int value) {
        std::cout << "Widget构造，value=" << value << std::endl;
    }
    
    void process() {
        std::cout << "Widget处理中..." << std::endl;
    }
};

int get_value() {
    std::cout << "get_value()调用" << std::endl;
    return 42;
}

void demonstrate_most_vexing_parse_solution() {
    std::cout << "=== Most Vexing Parse问题解决演示 ===\n";
    
    // 传统问题：这些看起来像对象声明，实际是函数声明
    
    // 问题1: 看起来像默认构造对象，实际是函数声明
    // Timer timer();  // 这是函数声明，不是对象构造！
    
    // C++11解决方案：大括号初始化
    Timer timer{};     // 明确的对象构造
    timer.start();
    
    // 问题2: 看起来像参数化构造，实际可能是函数声明  
    // Widget widget(int());  // 函数声明：Widget widget(int (*)())
    
    // C++11解决方案
    Widget widget1{get_value()};  // 明确的对象构造
    widget1.process();
    
    Widget widget2{42};           // 直接用字面值
    widget2.process();
    
    // 问题3: 临时对象的明确性
    std::cout << "\n临时对象构造:\n";
    
    // 传统方式可能有歧义
    auto process_widget = [](const Widget& w) {
        std::cout << "处理widget对象" << std::endl;
    };
    
    // 明确的临时对象创建
    process_widget(Widget{100});  // 清晰的临时对象构造
    
    // 容器中的对象构造
    std::vector<Widget> widgets;
    widgets.emplace_back(200);    // 原地构造
    widgets.push_back(Widget{300}); // 临时对象构造后移动
    
    std::cout << "\n解决方案总结:\n";
    std::cout << "1. 使用{}而不是()进行对象初始化\n";
    std::cout << "2. 避免函数声明的歧义性\n";
    std::cout << "3. 提供更清晰的代码意图\n";
    
    std::cout << "\n";
}

// ===== 6. 返回值的统一初始化 =====

struct Complex {
    double real, imag;
    
    Complex(double r, double i) : real(r), imag(i) {
        std::cout << "Complex(" << real << ", " << imag << ")" << std::endl;
    }
    
    void print() const {
        std::cout << real << " + " << imag << "i" << std::endl;
    }
};

// 使用统一初始化返回对象
Complex create_complex_traditional(double r, double i) {
    return Complex(r, i);  // 传统方式
}

Complex create_complex_uniform(double r, double i) {
    return {r, i};         // 统一初始化，更简洁
}

// 返回初始化列表
std::vector<int> create_vector_traditional() {
    std::vector<int> result;
    result.push_back(1);
    result.push_back(2);
    result.push_back(3);
    return result;
}

std::vector<int> create_vector_uniform() {
    return {1, 2, 3, 4, 5};  // 直接返回初始化列表
}

void demonstrate_return_value_initialization() {
    std::cout << "=== 返回值统一初始化演示 ===\n";
    
    std::cout << "1. 对象返回值初始化:\n";
    
    auto c1 = create_complex_traditional(3.0, 4.0);
    auto c2 = create_complex_uniform(1.0, 2.0);
    
    c1.print();
    c2.print();
    
    std::cout << "\n2. 容器返回值初始化:\n";
    
    auto vec1 = create_vector_traditional();
    auto vec2 = create_vector_uniform();
    
    std::cout << "传统方式创建的vector: ";
    for (auto item : vec1) std::cout << item << " ";
    std::cout << std::endl;
    
    std::cout << "统一初始化创建的vector: ";
    for (auto item : vec2) std::cout << item << " ";
    std::cout << std::endl;
    
    // 3. 函数参数中的统一初始化
    auto print_vector = [](const std::vector<int>& vec) {
        std::cout << "传入的vector: ";
        for (auto item : vec) std::cout << item << " ";
        std::cout << std::endl;
    };
    
    print_vector({10, 20, 30, 40});  // 直接传递初始化列表
    
    // 4. map的统一初始化
    auto create_map = []() {
        return std::map<std::string, int>{
            {"apple", 5},
            {"banana", 3},
            {"orange", 8}
        };
    };
    
    auto fruit_map = create_map();
    std::cout << "\n水果数量映射:\n";
    for (const auto& [fruit, count] : fruit_map) {  // C++17结构化绑定
        std::cout << fruit << ": " << count << std::endl;
    }
    
    std::cout << "\n";
}

// ===== 7. 性能考量和最佳实践 =====

void demonstrate_performance_and_best_practices() {
    std::cout << "=== 性能考量和最佳实践演示 ===\n";
    
    std::cout << "1. initializer_list的性能特性:\n";
    
    // initializer_list是轻量级的引用语义
    auto measure_performance = [](const std::string& method) {
        auto start = std::chrono::high_resolution_clock::now();
        
        if (method == "initializer_list") {
            // 使用initializer_list创建
            std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            volatile int sum = 0;
            for (auto item : vec) sum += item;
        } else if (method == "push_back") {
            // 使用push_back创建
            std::vector<int> vec;
            for (int i = 1; i <= 10; ++i) {
                vec.push_back(i);
            }
            volatile int sum = 0;
            for (auto item : vec) sum += item;
        } else if (method == "reserve_push_back") {
            // 使用reserve+push_back
            std::vector<int> vec;
            vec.reserve(10);
            for (int i = 1; i <= 10; ++i) {
                vec.push_back(i);
            }
            volatile int sum = 0;
            for (auto item : vec) sum += item;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    };
    
    const int iterations = 100000;
    long long total_init = 0, total_push = 0, total_reserve = 0;
    
    for (int i = 0; i < iterations; ++i) {
        total_init += measure_performance("initializer_list").count();
        total_push += measure_performance("push_back").count();
        total_reserve += measure_performance("reserve_push_back").count();
    }
    
    std::cout << "平均性能 (" << iterations << "次测试):\n";
    std::cout << "initializer_list: " << total_init / iterations << " ns\n";
    std::cout << "push_back: " << total_push / iterations << " ns\n";
    std::cout << "reserve+push_back: " << total_reserve / iterations << " ns\n";
    
    std::cout << "\n2. 最佳实践建议:\n";
    std::cout << "✓ 优先使用大括号初始化提高代码安全性\n";
    std::cout << "✓ 利用窄化检查防止意外的类型转换\n";
    std::cout << "✓ 在已知元素数量时使用initializer_list\n";
    std::cout << "✓ 注意initializer_list构造函数的优先级\n";
    std::cout << "✓ 使用空大括号{}进行默认初始化\n";
    std::cout << "✓ 在返回语句中使用大括号初始化简化代码\n";
    
    std::cout << "\n3. 注意事项:\n";
    std::cout << "⚠ initializer_list有最高优先级，可能覆盖其他构造函数\n";
    std::cout << "⚠ 大括号不允许窄化转换，可能需要显式类型转换\n";
    std::cout << "⚠ auto与initializer_list的推导可能出现意外结果\n";
    
    // auto推导的特殊情况
    auto list1 = {1, 2, 3};         // std::initializer_list<int>
    // auto list2 = {1, 2.0};       // 编译错误：类型不一致
    auto vec = std::vector<int>{1, 2, 3};  // std::vector<int>
    
    std::cout << "\nauto推导示例:\n";
    std::cout << "auto list1类型: " << typeid(list1).name() << std::endl;
    std::cout << "auto vec类型: " << typeid(vec).name() << std::endl;
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 统一初始化列表深度解析\n";
    std::cout << "==============================\n";
    
    // 基础语法
    demonstrate_basic_uniform_initialization();
    
    // 窄化转换预防
    demonstrate_narrowing_conversion_prevention();
    
    // initializer_list机制
    demonstrate_initializer_list_mechanism();
    
    // 构造函数重载决议
    demonstrate_constructor_overload_resolution();
    
    // Most Vexing Parse解决
    demonstrate_most_vexing_parse_solution();
    
    // 返回值初始化
    demonstrate_return_value_initialization();
    
    // 性能和最佳实践
    demonstrate_performance_and_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 08_uniform_initialization.cpp -o uniform_init_demo
./uniform_init_demo

关键学习点:
1. 掌握统一初始化语法的各种应用场景
2. 理解窄化转换检查的类型安全价值
3. 深入了解initializer_list的实现原理和性能特性
4. 掌握构造函数重载决议中initializer_list的优先级
5. 学会使用统一初始化解决Most Vexing Parse问题
6. 了解统一初始化在现代C++编程中的最佳实践
7. 注意auto与initializer_list结合时的特殊行为
8. 平衡类型安全和代码灵活性

注意事项:
- 某些C++17/20特性在注释中提及但不在C++11代码中使用
- 性能测试结果可能因编译器和系统而异
- 建议在实际项目中逐步采用统一初始化语法
*/