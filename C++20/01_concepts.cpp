/**
 * C++20 Concepts类型约束深度解析
 * 
 * 核心概念：
 * 1. SFINAE的终结者 - 从复杂的模板元编程到清晰的类型约束
 * 2. Concept定义机制 - requires表达式的四种形式和组合规则
 * 3. 约束求值和短路 - 编译期类型检查的优化策略
 * 4. 重载解析增强 - Concept如何影响模板特化和函数重载
 * 5. 标准库Concepts - 深入理解预定义概念的设计思想
 */

#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <ranges>

// ===== 1. SFINAE的终结者演示 =====
// C++17复杂的SFINAE写法
namespace old_style {
    template<typename T>
    std::enable_if_t<
        std::is_arithmetic_v<T> && 
        !std::is_same_v<T, bool> &&
        std::is_copy_constructible_v<T>,
        T
    > 
    complex_math_operation(T value) {
        return value * value + static_cast<T>(1);
    }
    
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, void>
    print_as_integer(T value) {
        std::cout << "整数: " << value << "\n";
    }
    
    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, void>
    print_as_float(T value) {
        std::cout << "浮点: " << value << "\n";
    }
}

// C++20优雅的Concepts写法
namespace modern_style {
    template<std::integral T>
        requires (!std::same_as<T, bool>)
    T complex_math_operation(T value) {
        return value * value + T{1};
    }
    
    template<std::floating_point T>
    T complex_math_operation(T value) {
        return value * value + T{1.0};
    }
    
    // 统一的打印函数，使用Concepts重载
    template<std::integral T>
    void smart_print(T value) {
        std::cout << "整数: " << value << "\n";
    }
    
    template<std::floating_point T>
    void smart_print(T value) {
        std::cout << "浮点: " << value << "\n";
    }
    
    template<typename T>
        requires std::is_same_v<T, std::string>
    void smart_print(const T& value) {
        std::cout << "字符串: \"" << value << "\"\n";
    }
}

void demonstrate_sfinae_replacement() {
    std::cout << "=== SFINAE的终结者演示 ===\n";
    
    std::cout << "C++17复杂写法:\n";
    std::cout << "整数运算: " << old_style::complex_math_operation(5) << "\n";
    old_style::print_as_integer(42);
    old_style::print_as_float(3.14);
    
    std::cout << "\nC++20优雅写法:\n";
    std::cout << "整数运算: " << modern_style::complex_math_operation(5) << "\n";
    std::cout << "浮点运算: " << modern_style::complex_math_operation(3.14) << "\n";
    
    modern_style::smart_print(42);
    modern_style::smart_print(3.14);
    modern_style::smart_print(std::string{"Hello Concepts"});
    
    std::cout << "\n";
}

// ===== 2. Concept定义机制演示 =====
// 基础概念定义
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept NonBoolIntegral = std::integral<T> && !std::same_as<T, bool>;

// 复合概念：容器概念
template<typename T>
concept Container = requires(T container) {
    // 类型要求
    typename T::value_type;
    typename T::iterator;
    
    // 表达式要求
    container.begin();
    container.end();
    container.size();
    container.empty();
    
    // 复合要求：指定返回类型约束
    { container.size() } -> std::convertible_to<size_t>;
    { container.empty() } -> std::convertible_to<bool>;
    { *container.begin() } -> std::convertible_to<typename T::value_type>;
};

// 高级容器概念：可排序容器
template<typename T>
concept SortableContainer = Container<T> && requires(T container) {
    // 嵌套要求：需要满足其他概念
    requires std::random_access_iterator<typename T::iterator>;
    requires std::sortable<typename T::iterator>;
    
    // 算法要求
    std::sort(container.begin(), container.end());
};

// 函数概念
template<typename F, typename... Args>
concept Callable = requires(F func, Args... args) {
    func(args...);
};

template<typename F, typename T, typename R = std::invoke_result_t<F, T>>
concept UnaryFunction = requires(F func, T arg) {
    { func(arg) } -> std::convertible_to<R>;
};

// 复杂概念组合
template<typename T>
concept ArithmeticContainer = Container<T> && 
    Numeric<typename T::value_type> &&
    requires(T container) {
        // 容器算术操作
        { std::accumulate(container.begin(), container.end(), typename T::value_type{}) } 
            -> std::convertible_to<typename T::value_type>;
    };

// 展示四种requires表达式形式
template<typename T>
concept ComprehensiveConcept = requires(T obj, const T const_obj) {
    // 1. 简单要求：表达式必须有效
    obj.method();
    obj + obj;
    ++obj;
    
    // 2. 类型要求：类型必须存在
    typename T::value_type;
    typename T::size_type;
    
    // 3. 复合要求：表达式有效且满足类型约束
    { obj.size() } -> std::convertible_to<size_t>;
    { obj[0] } -> std::same_as<typename T::value_type&>;
    { const_obj[0] } -> std::same_as<const typename T::value_type&>;
    
    // 4. 嵌套要求：满足其他概念
    requires std::default_initializable<T>;
    requires std::copy_constructible<T>;
    requires Numeric<typename T::value_type>;
};

void demonstrate_concept_definitions() {
    std::cout << "=== Concept定义机制演示 ===\n";
    
    // 测试基础概念
    std::cout << "基础概念测试:\n";
    std::cout << "int是Numeric: " << Numeric<int> << "\n";
    std::cout << "std::string是Numeric: " << Numeric<std::string> << "\n";
    std::cout << "bool是NonBoolIntegral: " << NonBoolIntegral<bool> << "\n";
    std::cout << "int是NonBoolIntegral: " << NonBoolIntegral<int> << "\n";
    
    // 测试容器概念
    std::cout << "\n容器概念测试:\n";
    std::cout << "std::vector<int>是Container: " << Container<std::vector<int>> << "\n";
    std::cout << "std::vector<int>是SortableContainer: " << SortableContainer<std::vector<int>> << "\n";
    std::cout << "std::vector<int>是ArithmeticContainer: " << ArithmeticContainer<std::vector<int>> << "\n";
    
    // 测试函数概念
    auto lambda = [](int x) { return x * 2; };
    std::cout << "\n函数概念测试:\n";
    std::cout << "lambda是Callable: " << Callable<decltype(lambda), int> << "\n";
    std::cout << "lambda是UnaryFunction: " << UnaryFunction<decltype(lambda), int> << "\n";
    
    std::cout << "\n";
}

// ===== 3. 约束求值和短路演示 =====
// 短路求值概念
template<typename T>
concept SafeArithmetic = requires {
    // 短路求值：如果第一个条件失败，后续不会检查
    requires std::is_arithmetic_v<T>;
    requires !std::is_same_v<T, bool>;
    requires std::is_copy_constructible_v<T>;
    requires sizeof(T) >= 4;  // 只有前面条件都通过才检查
};

// 复杂的约束求值示例
template<typename T>
concept ComplexContainer = requires(T container) {
    // 基础检查
    requires Container<T>;
    
    // 只有是容器时才进行后续检查
    requires requires {
        requires std::is_same_v<typename T::value_type, int> ||
                 std::is_same_v<typename T::value_type, double>;
    };
    
    // 算法检查（依赖前面的条件）
    requires requires {
        std::transform(container.begin(), container.end(), container.begin(),
                      [](const auto& x) { return x * 2; });
    };
};

// 递归概念定义
template<typename T>
concept RecursivelyPrintable = requires(const T& obj) {
    std::cout << obj;
} || requires(const T& obj) {
    // 如果不能直接打印，检查是否是容器
    requires Container<T>;
    requires RecursivelyPrintable<typename T::value_type>;
};

void demonstrate_constraint_evaluation() {
    std::cout << "=== 约束求值和短路演示 ===\n";
    
    // 短路求值测试
    std::cout << "短路求值测试:\n";
    std::cout << "int是SafeArithmetic: " << SafeArithmetic<int> << "\n";
    std::cout << "char是SafeArithmetic: " << SafeArithmetic<char> << "\n";  // sizeof(char)可能<4
    std::cout << "bool是SafeArithmetic: " << SafeArithmetic<bool> << "\n"; // 第二个条件失败
    std::cout << "std::string是SafeArithmetic: " << SafeArithmetic<std::string> << "\n"; // 第一个条件失败
    
    // 复杂约束测试
    std::cout << "\n复杂约束测试:\n";
    std::cout << "std::vector<int>是ComplexContainer: " << ComplexContainer<std::vector<int>> << "\n";
    std::cout << "std::vector<double>是ComplexContainer: " << ComplexContainer<std::vector<double>> << "\n";
    std::cout << "std::vector<std::string>是ComplexContainer: " << ComplexContainer<std::vector<std::string>> << "\n";
    
    // 递归概念测试
    std::cout << "\n递归概念测试:\n";
    std::cout << "int是RecursivelyPrintable: " << RecursivelyPrintable<int> << "\n";
    std::cout << "std::vector<int>是RecursivelyPrintable: " << RecursivelyPrintable<std::vector<int>> << "\n";
    
    std::cout << "\n";
}

// ===== 4. 重载解析增强演示 =====
// Concept约束的重载解析
template<typename T>
void process_data(T value) {
    std::cout << "通用处理: " << typeid(T).name() << "\n";
}

template<std::integral T>
void process_data(T value) {
    std::cout << "整数特化处理: " << value << "\n";
}

template<std::floating_point T>
void process_data(T value) {
    std::cout << "浮点特化处理: " << value << "\n";
}

template<typename T>
    requires Container<T> && std::integral<typename T::value_type>
void process_data(const T& container) {
    std::cout << "整数容器处理，大小: " << container.size() << "\n";
}

// 概念的部分排序
template<typename T>
    requires Numeric<T>
void algorithm_dispatch(T value) {
    std::cout << "数值算法: " << value << "\n";
}

template<typename T>
    requires std::integral<T>  // 更具体的约束
void algorithm_dispatch(T value) {
    std::cout << "整数算法: " << value << "\n";
}

template<typename T>
    requires NonBoolIntegral<T>  // 最具体的约束
void algorithm_dispatch(T value) {
    std::cout << "非布尔整数算法: " << value << "\n";
}

// 模板类的Concept特化
template<typename T>
class DataProcessor {
public:
    void process() {
        std::cout << "通用数据处理器\n";
    }
};

template<std::integral T>
class DataProcessor<T> {
public:
    void process() {
        std::cout << "整数数据处理器\n";
    }
};

template<Container T>
class DataProcessor<T> {
public:
    void process() {
        std::cout << "容器数据处理器，元素类型: " << typeid(typename T::value_type).name() << "\n";
    }
};

void demonstrate_overload_resolution() {
    std::cout << "=== 重载解析增强演示 ===\n";
    
    // 函数重载解析
    std::cout << "函数重载解析:\n";
    process_data(42);           // 整数特化
    process_data(3.14);         // 浮点特化
    process_data(std::string{"hello"});  // 通用版本
    
    std::vector<int> int_vec{1, 2, 3};
    process_data(int_vec);      // 整数容器特化
    
    // 概念的部分排序
    std::cout << "\n概念部分排序:\n";
    algorithm_dispatch(3.14);   // 数值算法
    algorithm_dispatch(true);   // 整数算法（bool是integral）
    algorithm_dispatch(42);     // 非布尔整数算法（最具体）
    
    // 模板类特化
    std::cout << "\n模板类特化:\n";
    DataProcessor<std::string> generic_processor;
    DataProcessor<int> int_processor;
    DataProcessor<std::vector<double>> container_processor;
    
    generic_processor.process();
    int_processor.process();
    container_processor.process();
    
    std::cout << "\n";
}

// ===== 5. 标准库Concepts深度解析 =====
// 展示标准库concepts的使用
void demonstrate_standard_concepts() {
    std::cout << "=== 标准库Concepts演示 ===\n";
    
    // 基础类型概念
    std::cout << "基础类型概念:\n";
    std::cout << "std::same_as<int, int>: " << std::same_as<int, int> << "\n";
    std::cout << "std::derived_from<std::string, std::string>: " << std::derived_from<std::string, std::string> << "\n";
    std::cout << "std::convertible_to<int, double>: " << std::convertible_to<int, double> << "\n";
    
    // 构造和析构概念
    std::cout << "\n构造和析构概念:\n";
    std::cout << "std::default_initializable<std::vector<int>>: " << std::default_initializable<std::vector<int>> << "\n";
    std::cout << "std::copy_constructible<std::string>: " << std::copy_constructible<std::string> << "\n";
    std::cout << "std::move_constructible<std::unique_ptr<int>>: " << std::move_constructible<std::unique_ptr<int>> << "\n";
    
    // 比较概念
    std::cout << "\n比较概念:\n";
    std::cout << "std::equality_comparable<int>: " << std::equality_comparable<int> << "\n";
    std::cout << "std::totally_ordered<double>: " << std::totally_ordered<double> << "\n";
    
    // 对象概念
    std::cout << "\n对象概念:\n";
    std::cout << "std::semiregular<std::string>: " << std::semiregular<std::string> << "\n";
    std::cout << "std::regular<int>: " << std::regular<int> << "\n";
    
    // 可调用概念
    auto lambda = [](int x) { return x * 2; };
    std::cout << "\n可调用概念:\n";
    std::cout << "std::invocable<decltype(lambda), int>: " << std::invocable<decltype(lambda), int> << "\n";
    std::cout << "std::predicate<decltype([](int x){ return x > 0; }), int>: " 
              << std::predicate<decltype([](int x){ return x > 0; }), int> << "\n";
    
    // 迭代器概念
    std::cout << "\n迭代器概念:\n";
    std::cout << "std::input_iterator<std::vector<int>::iterator>: " 
              << std::input_iterator<std::vector<int>::iterator> << "\n";
    std::cout << "std::random_access_iterator<std::vector<int>::iterator>: " 
              << std::random_access_iterator<std::vector<int>::iterator> << "\n";
    
    // Range概念
    std::cout << "\nRange概念:\n";
    std::cout << "std::ranges::range<std::vector<int>>: " << std::ranges::range<std::vector<int>> << "\n";
    std::cout << "std::ranges::view<std::ranges::filter_view<std::vector<int>, std::function<bool(int)>>>: " 
              << std::ranges::view<decltype(std::vector<int>{} | std::views::filter([](int){return true;}))> << "\n";
    
    std::cout << "\n";
}

// 自定义概念的实际应用示例
template<typename T>
concept Serializable = requires(const T& obj, std::ostream& os, std::istream& is) {
    { os << obj } -> std::same_as<std::ostream&>;
    { is >> const_cast<T&>(obj) } -> std::same_as<std::istream&>;
};

template<Serializable T>
class SerializationManager {
public:
    static void save(const T& obj, std::ostream& os) {
        os << obj;
        std::cout << "对象已序列化\n";
    }
    
    static void load(T& obj, std::istream& is) {
        is >> obj;
        std::cout << "对象已反序列化\n";
    }
};

template<typename T>
concept MathOperations = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    { a / b } -> std::convertible_to<T>;
    { -a } -> std::convertible_to<T>;
    { T{0} } -> std::convertible_to<T>;
    { T{1} } -> std::convertible_to<T>;
};

template<MathOperations T>
T polynomial_evaluate(T x, const std::vector<T>& coefficients) {
    T result = T{0};
    T power = T{1};
    
    for (const auto& coeff : coefficients) {
        result = result + coeff * power;
        power = power * x;
    }
    
    return result;
}

void demonstrate_practical_concepts() {
    std::cout << "=== 实用概念应用演示 ===\n";
    
    // 序列化概念应用
    std::cout << "序列化概念应用:\n";
    int value = 42;
    std::ostringstream oss;
    SerializationManager<int>::save(value, oss);
    
    std::istringstream iss("100");
    int loaded_value;
    SerializationManager<int>::load(loaded_value, iss);
    std::cout << "加载的值: " << loaded_value << "\n";
    
    // 数学运算概念应用
    std::cout << "\n数学运算概念应用:\n";
    std::vector<double> coeffs = {1.0, 2.0, 1.0};  // 1 + 2x + x^2
    double x = 3.0;
    auto result = polynomial_evaluate(x, coeffs);
    std::cout << "多项式在x=" << x << "处的值: " << result << "\n";
    
    // 验证不同类型是否满足概念
    std::cout << "\n概念兼容性检查:\n";
    std::cout << "int满足MathOperations: " << MathOperations<int> << "\n";
    std::cout << "double满足MathOperations: " << MathOperations<double> << "\n";
    std::cout << "std::string满足MathOperations: " << MathOperations<std::string> << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 Concepts类型约束深度解析\n";
    std::cout << "===============================\n";
    
    demonstrate_sfinae_replacement();
    demonstrate_concept_definitions();
    demonstrate_constraint_evaluation();
    demonstrate_overload_resolution();
    demonstrate_standard_concepts();
    demonstrate_practical_concepts();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 01_concepts.cpp -o concepts
./concepts

关键学习点:
1. Concepts彻底替代了复杂的SFINAE模式，提供清晰的类型约束
2. requires表达式支持四种形式：简单、类型、复合和嵌套要求
3. 约束求值支持短路优化，提高编译性能
4. Concepts增强了重载解析，支持更精确的模板特化
5. 标准库提供了丰富的预定义概念，覆盖常见使用场景

注意事项:
- Concepts是编译期特性，不会影响运行时性能
- 约束失败会产生更清晰的错误信息
- requires子句可以组合使用逻辑运算符
- 自定义概念应该语义清晰，避免过度约束
*/