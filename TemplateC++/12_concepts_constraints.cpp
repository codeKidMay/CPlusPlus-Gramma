/**
 * C++20 Concepts与约束系统深度解析
 * 
 * 核心概念：
 * 1. Concepts基础语法 - 概念定义与约束表达式
 * 2. 标准库Concepts - 语言内置概念库应用
 * 3. 自定义Concepts设计 - 语义约束与组合约束
 * 4. 约束模板特化 - 条件编译与重载决议
 * 5. Concepts最佳实践 - 错误诊断与性能优化
 */

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <algorithm>
#include <memory>
#include <chrono>
#include <ranges>

// ===== 1. Concepts基础语法演示 =====
void demonstrate_concepts_basics() {
    std::cout << "=== Concepts基础语法演示 ===\n";
    
    // 1.1 基础概念定义
    template<typename T>
    concept Integral = std::is_integral_v<T>;
    
    template<typename T>
    concept Floating = std::is_floating_point_v<T>;
    
    template<typename T>
    concept Numeric = Integral<T> || Floating<T>;
    
    // 1.2 复杂约束表达式
    template<typename T>
    concept Addable = requires(T a, T b) {
        { a + b } -> std::same_as<T>;
    };
    
    template<typename T>
    concept Printable = requires(T t) {
        std::cout << t;
    };
    
    template<typename T>
    concept Container = requires(T c) {
        typename T::value_type;
        typename T::iterator;
        c.begin();
        c.end();
        c.size();
    };
    
    // 1.3 嵌套要求
    template<typename T>
    concept AdvancedContainer = Container<T> && requires(T c) {
        { c.empty() } -> std::convertible_to<bool>;
        { c.clear() } -> std::same_as<void>;
        requires std::default_initializable<typename T::value_type>;
    };
    
    // 测试函数
    auto test_numeric = []<Numeric T>(T value) {
        std::cout << "处理数值类型: " << value << " (类型: " << typeid(T).name() << ")\n";
        return value * 2;
    };
    
    auto test_printable = []<Printable T>(const T& item) {
        std::cout << "可打印对象: ";
        std::cout << item << "\n";
    };
    
    auto test_container = []<Container T>(const T& container) {
        std::cout << "容器大小: " << container.size() << ", 类型: " << typeid(T).name() << "\n";
    };
    
    // 概念应用测试
    std::cout << "1. 数值概念测试:\n";
    std::cout << "int满足Integral: " << Integral<int> << "\n";
    std::cout << "double满足Floating: " << Floating<double> << "\n";
    std::cout << "string满足Numeric: " << Numeric<std::string> << "\n";
    
    test_numeric(42);
    test_numeric(3.14);
    
    std::cout << "\n2. 可打印概念测试:\n";
    test_printable(123);
    test_printable(std::string("Hello"));
    test_printable(2.718);
    
    std::cout << "\n3. 容器概念测试:\n";
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::string str = "hello";
    
    test_container(vec);
    test_container(str);
    
    std::cout << "vector满足AdvancedContainer: " << AdvancedContainer<std::vector<int>> << "\n";
    std::cout << "string满足AdvancedContainer: " << AdvancedContainer<std::string> << "\n";
    
    std::cout << "\n";
}

// ===== 2. 标准库Concepts演示 =====
void demonstrate_standard_concepts() {
    std::cout << "=== 标准库Concepts演示 ===\n";
    
    // 2.1 基础概念
    std::cout << "1. 基础类型概念:\n";
    std::cout << "int可默认构造: " << std::default_initializable<int> << "\n";
    std::cout << "int可移动构造: " << std::move_constructible<int> << "\n";
    std::cout << "int可拷贝构造: " << std::copy_constructible<int> << "\n";
    std::cout << "unique_ptr可移动但不可拷贝: " << std::movable<std::unique_ptr<int>> 
              << "/" << std::copyable<std::unique_ptr<int>> << "\n";
    
    // 2.2 比较概念
    template<std::equality_comparable T>
    bool are_equal(const T& a, const T& b) {
        std::cout << "比较两个可相等比较的对象\n";
        return a == b;
    }
    
    template<std::totally_ordered T>
    T find_max(const T& a, const T& b, const T& c) {
        std::cout << "在完全有序类型中找最大值\n";
        return std::max({a, b, c});
    }
    
    std::cout << "\n2. 比较概念测试:\n";
    std::cout << "相等比较结果: " << are_equal(10, 10) << "\n";
    std::cout << "最大值: " << find_max(5, 2, 8) << "\n";
    
    // 2.3 可调用概念
    template<std::invocable<int> F>
    void test_invocable(F func) {
        std::cout << "调用可调用对象，参数为int: " << func(42) << "\n";
    }
    
    template<std::predicate<int> P>
    void test_predicate(P pred, int value) {
        std::cout << "谓词测试结果: " << pred(value) << "\n";
    }
    
    std::cout << "\n3. 可调用概念测试:\n";
    auto square = [](int x) { return x * x; };
    auto is_even = [](int x) { return x % 2 == 0; };
    
    test_invocable(square);
    test_predicate(is_even, 10);
    test_predicate(is_even, 7);
    
    // 2.4 迭代器概念
    template<std::input_iterator I>
    void process_input_range(I first, I last) {
        std::cout << "处理输入迭代器范围，元素数量: " << std::distance(first, last) << "\n";
    }
    
    template<std::random_access_iterator I>
    void random_access_demo(I first, I last) {
        if (first != last) {
            std::cout << "随机访问迭代器演示，第一个元素: " << *first;
            if (std::distance(first, last) > 1) {
                std::cout << "，最后一个元素: " << *(last - 1);
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\n4. 迭代器概念测试:\n";
    std::vector<int> data{10, 20, 30, 40, 50};
    
    process_input_range(data.begin(), data.end());
    random_access_demo(data.begin(), data.end());
    
    std::cout << "vector迭代器是随机访问: " 
              << std::random_access_iterator<std::vector<int>::iterator> << "\n";
    
    // 2.5 范围概念
    template<std::ranges::range R>
    void process_range(R&& range) {
        std::cout << "处理范围，大小: " << std::ranges::size(range) << "\n";
        std::cout << "前3个元素: ";
        auto it = std::ranges::begin(range);
        for (int i = 0; i < 3 && it != std::ranges::end(range); ++i, ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";
    }
    
    template<std::ranges::input_range R>
    requires std::integral<std::ranges::range_value_t<R>>
    auto sum_range(R&& range) {
        std::cout << "计算整数范围的和\n";
        std::ranges::range_value_t<R> sum{};
        for (auto&& value : range) {
            sum += value;
        }
        return sum;
    }
    
    std::cout << "\n5. 范围概念测试:\n";
    process_range(data);
    std::cout << "范围元素和: " << sum_range(data) << "\n";
    
    std::cout << "\n";
}

// ===== 3. 自定义Concepts设计演示 =====
void demonstrate_custom_concepts() {
    std::cout << "=== 自定义Concepts设计演示 ===\n";
    
    // 3.1 语义约束概念
    template<typename T>
    concept Drawable = requires(T obj) {
        obj.draw();
        { obj.get_area() } -> std::convertible_to<double>;
        { obj.get_name() } -> std::convertible_to<std::string>;
    };
    
    template<typename T>
    concept Resizable = Drawable<T> && requires(T obj, double factor) {
        obj.resize(factor);
    };
    
    template<typename T>
    concept Colorable = requires(T obj, std::string color) {
        obj.set_color(color);
        { obj.get_color() } -> std::convertible_to<std::string>;
    };
    
    template<typename T>
    concept Shape = Drawable<T> && Colorable<T>;
    
    // 具体形状实现
    class Circle {
    private:
        double radius_;
        std::string color_;
        
    public:
        Circle(double r) : radius_(r), color_("black") {}
        
        void draw() const {
            std::cout << "绘制" << color_ << "圆形，半径: " << radius_;
        }
        
        double get_area() const {
            return 3.14159 * radius_ * radius_;
        }
        
        std::string get_name() const {
            return "Circle";
        }
        
        void resize(double factor) {
            radius_ *= factor;
            std::cout << "圆形缩放因子: " << factor << "，新半径: " << radius_ << "\n";
        }
        
        void set_color(const std::string& color) {
            color_ = color;
        }
        
        std::string get_color() const {
            return color_;
        }
    };
    
    class Rectangle {
    private:
        double width_, height_;
        std::string color_;
        
    public:
        Rectangle(double w, double h) : width_(w), height_(h), color_("black") {}
        
        void draw() const {
            std::cout << "绘制" << color_ << "矩形，尺寸: " << width_ << "x" << height_;
        }
        
        double get_area() const {
            return width_ * height_;
        }
        
        std::string get_name() const {
            return "Rectangle";
        }
        
        void resize(double factor) {
            width_ *= factor;
            height_ *= factor;
            std::cout << "矩形缩放因子: " << factor << "，新尺寸: " << width_ << "x" << height_ << "\n";
        }
        
        void set_color(const std::string& color) {
            color_ = color;
        }
        
        std::string get_color() const {
            return color_;
        }
    };
    
    // 3.2 组合约束概念
    template<typename T>
    concept ComplexShape = Shape<T> && Resizable<T> && requires {
        T::dimension_count;
        requires T::dimension_count >= 2;
    };
    
    // 增强的形状类
    class AdvancedCircle : public Circle {
    public:
        static constexpr int dimension_count = 2;
        
        AdvancedCircle(double r) : Circle(r) {}
        
        void advanced_operation() {
            std::cout << "执行高级圆形操作\n";
        }
    };
    
    // 泛型算法
    template<Drawable T>
    void draw_shape(const T& shape) {
        std::cout << "绘制形状: " << shape.get_name() << " - ";
        shape.draw();
        std::cout << " (面积: " << shape.get_area() << ")\n";
    }
    
    template<Shape T>
    void customize_shape(T& shape, const std::string& color) {
        std::cout << "定制形状颜色\n";
        shape.set_color(color);
        draw_shape(shape);
    }
    
    template<Resizable T>
    void resize_shape(T& shape, double factor) {
        std::cout << "调整形状大小\n";
        double old_area = shape.get_area();
        shape.resize(factor);
        double new_area = shape.get_area();
        std::cout << "面积从 " << old_area << " 变为 " << new_area << "\n";
    }
    
    template<ComplexShape T>
    void process_complex_shape(T& shape) {
        std::cout << "处理复杂形状 (维度: " << T::dimension_count << ")\n";
        customize_shape(shape, "red");
        resize_shape(shape, 1.5);
    }
    
    // 测试自定义概念
    std::cout << "1. 概念匹配测试:\n";
    std::cout << "Circle满足Drawable: " << Drawable<Circle> << "\n";
    std::cout << "Circle满足Shape: " << Shape<Circle> << "\n";
    std::cout << "Circle满足Resizable: " << Resizable<Circle> << "\n";
    std::cout << "AdvancedCircle满足ComplexShape: " << ComplexShape<AdvancedCircle> << "\n";
    
    std::cout << "\n2. 概念约束函数测试:\n";
    Circle circle(5.0);
    Rectangle rect(4.0, 6.0);
    AdvancedCircle adv_circle(3.0);
    
    draw_shape(circle);
    draw_shape(rect);
    
    customize_shape(circle, "blue");
    customize_shape(rect, "green");
    
    resize_shape(circle, 2.0);
    
    process_complex_shape(adv_circle);
    
    std::cout << "\n";
}

// ===== 4. 约束模板特化演示 =====
void demonstrate_constrained_specialization() {
    std::cout << "=== 约束模板特化演示 ===\n";
    
    // 4.1 条件编译约束
    template<typename T>
    struct TypeHandler {
        static void handle(const T& value) {
            std::cout << "通用类型处理: " << typeid(T).name() << "\n";
        }
    };
    
    // 整数类型特化
    template<std::integral T>
    struct TypeHandler<T> {
        static void handle(const T& value) {
            std::cout << "整数类型处理: " << value;
            if (value % 2 == 0) {
                std::cout << " (偶数)";
            } else {
                std::cout << " (奇数)";
            }
            std::cout << "\n";
        }
    };
    
    // 浮点类型特化
    template<std::floating_point T>
    struct TypeHandler<T> {
        static void handle(const T& value) {
            std::cout << "浮点类型处理: " << value << " (精度: " << sizeof(T) << " bytes)\n";
        }
    };
    
    // 4.2 重载决议约束
    template<typename T>
    void process_data(const T& data) {
        std::cout << "默认数据处理\n";
    }
    
    template<std::ranges::range T>
    void process_data(const T& data) {
        std::cout << "范围数据处理，元素数量: " << std::ranges::size(data) << "\n";
    }
    
    template<std::integral T>
    void process_data(const T& data) {
        std::cout << "整数数据处理: " << data << "\n";
    }
    
    // 复杂约束重载
    template<typename T>
    requires std::integral<T> && (sizeof(T) >= 4)
    void optimized_compute(T value) {
        std::cout << "大整数优化计算: " << value << "\n";
    }
    
    template<typename T>
    requires std::integral<T> && (sizeof(T) < 4)
    void optimized_compute(T value) {
        std::cout << "小整数标准计算: " << static_cast<int>(value) << "\n";
    }
    
    template<std::floating_point T>
    void optimized_compute(T value) {
        std::cout << "浮点数高精度计算: " << value << "\n";
    }
    
    // 4.3 SFINAE替代方案
    // 传统SFINAE方法
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    void legacy_function(T value) {
        std::cout << "SFINAE方法处理算术类型: " << value << "\n";
    }
    
    // Concepts方法
    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    void modern_function(T value) {
        std::cout << "Concepts方法处理算术类型: " << value << "\n";
    }
    
    // 测试约束特化
    std::cout << "1. 类型特化测试:\n";
    TypeHandler<int>::handle(42);
    TypeHandler<double>::handle(3.14159);
    TypeHandler<std::string>::handle(std::string("hello"));
    
    std::cout << "\n2. 重载决议测试:\n";
    process_data(100);
    process_data(std::vector<int>{1, 2, 3});
    process_data(std::string("test"));
    
    std::cout << "\n3. 复杂约束重载测试:\n";
    optimized_compute(static_cast<short>(10));    // 小整数
    optimized_compute(1000);                      // 大整数
    optimized_compute(2.718);                     // 浮点数
    
    std::cout << "\n4. SFINAE vs Concepts对比:\n";
    legacy_function(42);
    legacy_function(3.14);
    modern_function(42);
    modern_function(3.14);
    
    std::cout << "\n";
}

// ===== 5. Concepts最佳实践演示 =====
void demonstrate_concepts_best_practices() {
    std::cout << "=== Concepts最佳实践演示 ===\n";
    
    // 5.1 清晰的错误诊断
    template<typename T>
    concept SerializableObject = requires(T obj) {
        { obj.serialize() } -> std::convertible_to<std::string>;
        { obj.deserialize(std::declval<std::string>()) } -> std::same_as<void>;
        typename T::version_type;
        requires std::integral<typename T::version_type>;
    };
    
    class GoodSerializable {
    public:
        using version_type = int;
        
        std::string serialize() const {
            return "serialized_data";
        }
        
        void deserialize(const std::string& data) {
            // 反序列化逻辑
        }
    };
    
    class BadSerializable {
    public:
        // 缺少version_type
        std::string serialize() const {
            return "bad_serialized_data";
        }
        // 缺少deserialize方法
    };
    
    template<SerializableObject T>
    void safe_serialize(const T& obj) {
        std::cout << "安全序列化: " << obj.serialize() << "\n";
    }
    
    // 5.2 组合式概念设计
    template<typename T>
    concept Streamable = requires(T obj, std::ostream& os) {
        os << obj;
    };
    
    template<typename T>
    concept Hashable = requires(T obj) {
        { std::hash<T>{}(obj) } -> std::convertible_to<std::size_t>;
    };
    
    template<typename T>
    concept CacheKey = Streamable<T> && Hashable<T> && std::equality_comparable<T>;
    
    // 5.3 性能友好的约束
    template<typename T>
    concept LightweightCopyable = std::copyable<T> && (sizeof(T) <= 64) && std::is_trivially_copyable_v<T>;
    
    template<typename T>
    concept HeavyObject = sizeof(T) > 64 || !std::is_trivially_copyable_v<T>;
    
    // 根据对象大小选择不同的传递方式
    template<LightweightCopyable T>
    void efficient_process(T obj) {  // 值传递
        std::cout << "轻量对象值传递处理 (大小: " << sizeof(T) << " bytes)\n";
    }
    
    template<HeavyObject T>
    void efficient_process(const T& obj) {  // 引用传递
        std::cout << "重量对象引用传递处理 (大小: " << sizeof(T) << " bytes)\n";
    }
    
    // 5.4 概念层次结构
    template<typename T>
    concept BasicIterable = requires(T container) {
        container.begin();
        container.end();
    };
    
    template<typename T>
    concept SizedIterable = BasicIterable<T> && requires(T container) {
        { container.size() } -> std::convertible_to<std::size_t>;
    };
    
    template<typename T>
    concept ModifiableContainer = SizedIterable<T> && requires(T container) {
        container.clear();
        container.push_back(std::declval<typename T::value_type>());
    };
    
    template<BasicIterable T>
    void basic_iteration(const T& container) {
        std::cout << "基础迭代处理\n";
        for (const auto& item : container) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
    
    template<SizedIterable T>
    void sized_iteration(const T& container) {
        std::cout << "带大小迭代处理 (大小: " << container.size() << ")\n";
        basic_iteration(container);
    }
    
    template<ModifiableContainer T>
    void modify_container(T& container) {
        std::cout << "修改容器\n";
        if constexpr (requires { container.push_back(typename T::value_type{}); }) {
            container.push_back(typename T::value_type{});
            std::cout << "添加元素后大小: " << container.size() << "\n";
        }
    }
    
    // 5.5 编译时性能优化
    template<typename T>
    concept TriviallySerializable = std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;
    
    template<TriviallySerializable T>
    void fast_serialize(const T& obj) {
        std::cout << "快速序列化 (memcpy) - 大小: " << sizeof(T) << " bytes\n";
        // 使用memcpy进行快速序列化
    }
    
    template<typename T>
    requires (!TriviallySerializable<T>) && SerializableObject<T>
    void safe_serialize_alternative(const T& obj) {
        std::cout << "安全序列化 (对象方法)\n";
        obj.serialize();
    }
    
    // 测试最佳实践
    std::cout << "1. 错误诊断测试:\n";
    GoodSerializable good_obj;
    safe_serialize(good_obj);
    
    // BadSerializable bad_obj;
    // safe_serialize(bad_obj);  // 编译错误，清晰的错误信息
    
    std::cout << "SerializableObject<GoodSerializable>: " << SerializableObject<GoodSerializable> << "\n";
    std::cout << "SerializableObject<BadSerializable>: " << SerializableObject<BadSerializable> << "\n";
    
    std::cout << "\n2. 性能优化测试:\n";
    struct LightObject { int x, y; };
    struct HeavyObject { double data[100]; };
    
    LightObject light{1, 2};
    HeavyObject heavy{};
    
    efficient_process(light);  // 值传递
    efficient_process(heavy);  // 引用传递
    
    std::cout << "\n3. 概念层次结构测试:\n";
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::string str = "hello";
    
    std::cout << "Vector测试:\n";
    sized_iteration(vec);
    modify_container(vec);
    
    std::cout << "String测试:\n";
    basic_iteration(str);
    
    std::cout << "\n4. 编译时优化测试:\n";
    int trivial_data = 42;
    fast_serialize(trivial_data);
    safe_serialize_alternative(good_obj);
    
    std::cout << "\n";
}

// ===== 性能基准测试 =====
void benchmark_concepts_vs_sfinae() {
    std::cout << "=== Concepts vs SFINAE性能基准测试 ===\n";
    
    const int iterations = 1000000;
    
    // SFINAE版本
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    T sfinae_compute(T value) {
        return value * value + value;
    }
    
    // Concepts版本
    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    T concepts_compute(T value) {
        return value * value + value;
    }
    
    // 编译时间测试（运行时展示）
    auto start = std::chrono::high_resolution_clock::now();
    
    int sfinae_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        sfinae_sum += sfinae_compute(i % 1000);
    }
    
    auto sfinae_time = std::chrono::high_resolution_clock::now() - start;
    
    start = std::chrono::high_resolution_clock::now();
    
    int concepts_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        concepts_sum += concepts_compute(i % 1000);
    }
    
    auto concepts_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "运行时性能对比 (" << iterations << " 次迭代):\n";
    std::cout << "SFINAE版本耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(sfinae_time).count() 
              << " 微秒 (结果: " << sfinae_sum << ")\n";
    std::cout << "Concepts版本耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(concepts_time).count() 
              << " 微秒 (结果: " << concepts_sum << ")\n";
    
    double ratio = static_cast<double>(sfinae_time.count()) / concepts_time.count();
    std::cout << "性能比率 (SFINAE/Concepts): " << ratio << "\n";
    
    std::cout << "\n编译时优势:\n";
    std::cout << "- Concepts提供更清晰的错误消息\n";
    std::cout << "- Concepts减少模板实例化开销\n";
    std::cout << "- Concepts提高代码可读性和维护性\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 Concepts与约束系统深度解析\n";
    std::cout << "=================================\n";
    
    demonstrate_concepts_basics();
    demonstrate_standard_concepts();
    demonstrate_custom_concepts();
    demonstrate_constrained_specialization();
    demonstrate_concepts_best_practices();
    benchmark_concepts_vs_sfinae();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 12_concepts_constraints.cpp -o concepts_constraints
./concepts_constraints

关键学习点:
1. Concepts提供编译期类型约束，替代复杂的SFINAE技术
2. 标准库提供丰富的预定义概念，覆盖常见使用场景
3. 自定义概念支持语义约束和复杂组合约束表达式
4. 约束模板特化实现条件编译和精确的重载决议
5. 正确使用Concepts可以显著改善错误诊断和代码可读性

注意事项:
- Concepts需要C++20编译器支持
- 过度约束可能降低模板的通用性
- 概念设计应该反映语义而非仅仅语法要求
- 合理组织概念层次结构避免重复和冲突
*/