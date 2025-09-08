/**
 * C++11 变长模板参数深度解析
 * 
 * 核心概念：
 * 1. 参数包(Parameter Pack)的展开机制
 * 2. 递归模板的终止条件设计
 * 3. 完美转发与参数包的结合
 * 4. 元编程技巧：类型操作和编译期计算
 * 5. 实际应用：通用工厂函数、tuple实现等
 */

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <type_traits>
#include <memory>
#include <functional>
#include <utility>

// ===== 1. 基础语法和参数包展开 =====

// 最简单的变长模板函数
template<typename... Args>
void basic_print(Args... args) {
    std::cout << "参数个数: " << sizeof...(args) << std::endl;
    std::cout << "类型个数: " << sizeof...(Args) << std::endl;
}

// 递归展开参数包 - 经典方法
template<typename First>
void recursive_print(First&& first) {
    std::cout << first << std::endl;  // 递归终止条件
}

template<typename First, typename... Rest>
void recursive_print(First&& first, Rest&&... rest) {
    std::cout << first << ", ";
    recursive_print(rest...);  // 递归调用，参数包逐步缩小
}

// 使用逗号操作符的技巧（C++11可用）
template<typename... Args>
void comma_trick_print(Args... args) {
    std::cout << "逗号技巧输出: ";
    int dummy[] = {0, (std::cout << args << " ", 0)...};
    (void)dummy;  // 避免未使用变量警告
    std::cout << std::endl;
}

void demonstrate_basic_variadic_templates() {
    std::cout << "=== 基础变长模板演示 ===\n";
    
    basic_print(1, "hello", 3.14, 'c');
    
    std::cout << "递归展开:\n";
    recursive_print(1, "hello", 3.14, 'c');
    
    std::cout << "逗号技巧:\n";
    comma_trick_print(1, "hello", 3.14, 'c');
    
    std::cout << "\n";
}

// ===== 2. 类型操作与编译期计算 =====

// 获取第N个类型的元编程
template<size_t N, typename... Types>
struct type_at;

template<size_t N, typename First, typename... Rest>
struct type_at<N, First, Rest...> {
    using type = typename type_at<N-1, Rest...>::type;
};

template<typename First, typename... Rest>
struct type_at<0, First, Rest...> {
    using type = First;
};

// 类型包的长度
template<typename... Types>
struct pack_size {
    static constexpr size_t value = sizeof...(Types);
};

// 检查类型是否在参数包中
template<typename T, typename... Types>
struct contains;

template<typename T>
struct contains<T> : std::false_type {};

template<typename T, typename First, typename... Rest>
struct contains<T, First, Rest...> 
    : std::conditional<
        std::is_same<T, First>::value,
        std::true_type,
        contains<T, Rest...>
    >::type {};

// 获取类型在参数包中的索引
template<typename T, typename... Types>
struct index_of;

template<typename T, typename First, typename... Rest>
struct index_of<T, First, Rest...> {
    static constexpr size_t value = 
        std::is_same<T, First>::value ? 0 : 1 + index_of<T, Rest...>::value;
};

template<typename T>
struct index_of<T> {
    static constexpr size_t value = 0;
};

void demonstrate_type_operations() {
    std::cout << "=== 类型操作元编程演示 ===\n";
    
    // 测试type_at
    using Types = std::tuple<int, float, double, char>;
    using SecondType = typename type_at<1, int, float, double, char>::type;
    static_assert(std::is_same<SecondType, float>::value, "type_at错误");
    std::cout << "第1个类型是float: " << std::is_same<SecondType, float>::value << std::endl;
    
    // 测试pack_size
    std::cout << "参数包大小: " << pack_size<int, float, double, char>::value << std::endl;
    
    // 测试contains
    std::cout << "包含double: " << contains<double, int, float, double, char>::value << std::endl;
    std::cout << "包含string: " << contains<std::string, int, float, double, char>::value << std::endl;
    
    // 测试index_of
    std::cout << "double的索引: " << index_of<double, int, float, double, char>::value << std::endl;
    
    std::cout << "\n";
}

// ===== 3. 完美转发与参数包 =====

// 通用工厂函数模板
template<typename T, typename... Args>
std::unique_ptr<T> make_unique_variadic(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 模拟std::invoke的实现
template<typename Func, typename... Args>
auto invoke_variadic(Func&& func, Args&&... args) 
    -> decltype(std::forward<Func>(func)(std::forward<Args>(args)...)) {
    return std::forward<Func>(func)(std::forward<Args>(args)...);
}

// 测试类
class TestClass {
private:
    int value;
    std::string name;
    double rate;
    
public:
    TestClass(int v, const std::string& n, double r) 
        : value(v), name(n), rate(r) {
        std::cout << "TestClass构造: " << value << ", " << name << ", " << rate << std::endl;
    }
    
    void display() const {
        std::cout << "TestClass: " << value << ", " << name << ", " << rate << std::endl;
    }
};

void demonstrate_perfect_forwarding() {
    std::cout << "=== 完美转发与参数包演示 ===\n";
    
    // 使用通用工厂函数
    auto obj1 = make_unique_variadic<TestClass>(42, "Hello", 3.14);
    obj1->display();
    
    // 测试不同类型的参数转发
    std::string temp_str = "TempString";
    auto obj2 = make_unique_variadic<TestClass>(100, temp_str, 2.718);
    obj2->display();
    
    // 使用invoke_variadic
    auto lambda = [](int a, const std::string& b, double c) {
        std::cout << "Lambda调用: " << a << ", " << b << ", " << c << std::endl;
        return a + c;
    };
    
    auto result = invoke_variadic(lambda, 10, std::string("World"), 1.414);
    std::cout << "调用结果: " << result << std::endl;
    
    std::cout << "\n";
}

// ===== 4. 变长模板类的实现 =====

// 简化版tuple实现
template<typename... Types>
class simple_tuple;

// 空tuple特化
template<>
class simple_tuple<> {
public:
    static constexpr size_t size() { return 0; }
};

// 递归tuple实现
template<typename Head, typename... Tail>
class simple_tuple<Head, Tail...> : private simple_tuple<Tail...> {
private:
    Head head_;
    using base_type = simple_tuple<Tail...>;
    
public:
    template<typename H, typename... T>
    simple_tuple(H&& h, T&&... t) 
        : base_type(std::forward<T>(t)...), head_(std::forward<H>(h)) {}
    
    static constexpr size_t size() { return 1 + base_type::size(); }
    
    Head& head() { return head_; }
    const Head& head() const { return head_; }
    
    base_type& tail() { return static_cast<base_type&>(*this); }
    const base_type& tail() const { return static_cast<const base_type&>(*this); }
};

// tuple的get实现
template<size_t Index, typename... Types>
struct tuple_element;

template<size_t Index, typename Head, typename... Tail>
struct tuple_element<Index, simple_tuple<Head, Tail...>> {
    using type = typename tuple_element<Index-1, simple_tuple<Tail...>>::type;
};

template<typename Head, typename... Tail>
struct tuple_element<0, simple_tuple<Head, Tail...>> {
    using type = Head;
};

template<size_t Index, typename... Types>
auto get(simple_tuple<Types...>& t) -> typename tuple_element<Index, simple_tuple<Types...>>::type& {
    if constexpr (Index == 0) {
        return t.head();
    } else {
        return get<Index-1>(t.tail());
    }
}

void demonstrate_variadic_class() {
    std::cout << "=== 变长模板类演示 ===\n";
    
    simple_tuple<int, std::string, double> my_tuple(42, "Hello", 3.14);
    
    std::cout << "tuple大小: " << my_tuple.size() << std::endl;
    std::cout << "第0个元素: " << get<0>(my_tuple) << std::endl;
    std::cout << "第1个元素: " << get<1>(my_tuple) << std::endl;
    std::cout << "第2个元素: " << get<2>(my_tuple) << std::endl;
    
    std::cout << "\n";
}

// ===== 5. 函数对象和绑定 =====

// 通用函数包装器
template<typename... Args>
class function_wrapper;

template<typename R, typename... Args>
class function_wrapper<R(Args...)> {
private:
    std::function<R(Args...)> func_;
    
public:
    template<typename F>
    function_wrapper(F&& f) : func_(std::forward<F>(f)) {}
    
    R operator()(Args... args) {
        std::cout << "调用包装的函数，参数个数: " << sizeof...(args) << std::endl;
        return func_(args...);
    }
};

// 通用绑定器（简化版）
template<typename Func, typename... BoundArgs>
class simple_bind {
private:
    Func func_;
    std::tuple<BoundArgs...> bound_args_;
    
    template<size_t... Indices, typename... CallArgs>
    auto call_impl(std::index_sequence<Indices...>, CallArgs&&... call_args)
        -> decltype(func_(std::get<Indices>(bound_args_)..., std::forward<CallArgs>(call_args)...)) {
        return func_(std::get<Indices>(bound_args_)..., std::forward<CallArgs>(call_args)...);
    }
    
public:
    simple_bind(Func&& func, BoundArgs&&... bound_args)
        : func_(std::forward<Func>(func)), bound_args_(std::forward<BoundArgs>(bound_args)...) {}
    
    template<typename... CallArgs>
    auto operator()(CallArgs&&... call_args) 
        -> decltype(call_impl(std::make_index_sequence<sizeof...(BoundArgs)>{}, std::forward<CallArgs>(call_args)...)) {
        return call_impl(std::make_index_sequence<sizeof...(BoundArgs)>{}, std::forward<CallArgs>(call_args)...);
    }
};

template<typename Func, typename... Args>
auto make_bind(Func&& func, Args&&... args) {
    return simple_bind<std::decay_t<Func>, std::decay_t<Args>...>(
        std::forward<Func>(func), std::forward<Args>(args)...);
}

void demonstrate_function_binding() {
    std::cout << "=== 函数对象和绑定演示 ===\n";
    
    // 测试函数包装器
    function_wrapper<int(int, int)> wrapped_add([](int a, int b) { return a + b; });
    std::cout << "包装器调用结果: " << wrapped_add(10, 20) << std::endl;
    
    // 测试简单绑定器
    auto multiply = [](int a, int b, int c) { return a * b * c; };
    auto bound_multiply = make_bind(multiply, 2, 3);  // 绑定前两个参数
    std::cout << "绑定器调用结果: " << bound_multiply(4) << std::endl;  // 2 * 3 * 4 = 24
    
    std::cout << "\n";
}

// ===== 6. 编译期递归算法 =====

// 编译期求和
template<int... Values>
struct sum;

template<int First, int... Rest>
struct sum<First, Rest...> {
    static constexpr int value = First + sum<Rest...>::value;
};

template<>
struct sum<> {
    static constexpr int value = 0;
};

// 编译期查找最大值
template<int... Values>
struct max_value;

template<int First>
struct max_value<First> {
    static constexpr int value = First;
};

template<int First, int Second, int... Rest>
struct max_value<First, Second, Rest...> {
    static constexpr int value = max_value<(First > Second ? First : Second), Rest...>::value;
};

// 编译期类型过滤
template<template<typename> class Predicate, typename... Types>
struct filter;

template<template<typename> class Predicate>
struct filter<Predicate> {
    using type = std::tuple<>;
};

template<template<typename> class Predicate, typename First, typename... Rest>
struct filter<Predicate, First, Rest...> {
    using rest_filtered = typename filter<Predicate, Rest...>::type;
    using type = typename std::conditional<
        Predicate<First>::value,
        typename std::decay<decltype(std::tuple_cat(std::tuple<First>(), rest_filtered()))>::type,
        rest_filtered
    >::type;
};

// 谓词：是否为指针类型
template<typename T>
using is_pointer_predicate = std::is_pointer<T>;

void demonstrate_compile_time_algorithms() {
    std::cout << "=== 编译期递归算法演示 ===\n";
    
    // 编译期求和
    constexpr int total = sum<1, 2, 3, 4, 5>::value;
    std::cout << "编译期求和 1+2+3+4+5 = " << total << std::endl;
    
    // 编译期最大值
    constexpr int maximum = max_value<10, 30, 20, 50, 25>::value;
    std::cout << "编译期最大值 max(10,30,20,50,25) = " << maximum << std::endl;
    
    // 类型过滤演示
    using all_types = std::tuple<int, int*, float, char*, double, void*>;
    using pointer_types = typename filter<is_pointer_predicate, int, int*, float, char*, double, void*>::type;
    
    std::cout << "原始类型数量: " << std::tuple_size<all_types>::value << std::endl;
    std::cout << "指针类型数量: " << std::tuple_size<pointer_types>::value << std::endl;
    
    std::cout << "\n";
}

// ===== 7. 实际应用：printf风格的类型安全输出 =====

// 简单的格式化输出实现
template<typename... Args>
void safe_printf(const char* format, Args... args);

// 递归处理格式字符串
void safe_printf_impl(const char* format) {
    while (*format) {
        if (*format == '%' && *(++format) != '%') {
            throw std::runtime_error("参数不匹配：格式说明符过多");
        }
        std::cout << *format++;
    }
}

template<typename T, typename... Args>
void safe_printf_impl(const char* format, T&& value, Args&&... args) {
    while (*format) {
        if (*format == '%' && *(++format) != '%') {
            std::cout << value;
            safe_printf_impl(++format, args...);  // 递归处理剩余参数
            return;
        }
        std::cout << *format++;
    }
    throw std::runtime_error("参数不匹配：参数过多");
}

template<typename... Args>
void safe_printf(const char* format, Args... args) {
    safe_printf_impl(format, args...);
}

// 类型安全的日志系统
enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

template<typename... Args>
void log(LogLevel level, const char* format, Args... args) {
    const char* level_str[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    std::cout << "[" << level_str[static_cast<int>(level)] << "] ";
    safe_printf(format, args...);
    std::cout << std::endl;
}

void demonstrate_practical_applications() {
    std::cout << "=== 实际应用演示 ===\n";
    
    // 类型安全的printf
    std::cout << "类型安全的printf:\n";
    try {
        safe_printf("Hello %, my age is % and my score is %\n", "World", 25, 98.5);
    } catch (const std::exception& e) {
        std::cout << "错误: " << e.what() << std::endl;
    }
    
    // 日志系统
    std::cout << "\n日志系统演示:\n";
    log(LogLevel::INFO, "程序启动，版本 %", "1.0.0");
    log(LogLevel::WARNING, "内存使用率: %%", 85);
    log(LogLevel::ERROR, "连接失败，错误代码: %", 404);
    
    std::cout << "\n";
}

// ===== 8. 性能分析和最佳实践 =====

// 性能测试：递归vs迭代
template<typename... Args>
int recursive_sum_runtime(Args... args) {
    std::vector<int> values = {args...};
    int sum = 0;
    for (int val : values) {
        sum += val;
    }
    return sum;
}

template<int... Values>
constexpr int compile_time_sum() {
    return sum<Values...>::value;
}

void demonstrate_performance_and_best_practices() {
    std::cout << "=== 性能分析和最佳实践 ===\n";
    
    // 编译期vs运行时计算
    constexpr int compile_time_result = compile_time_sum<1, 2, 3, 4, 5>();
    int runtime_result = recursive_sum_runtime(1, 2, 3, 4, 5);
    
    std::cout << "编译期计算结果: " << compile_time_result << std::endl;
    std::cout << "运行时计算结果: " << runtime_result << std::endl;
    
    std::cout << "\n最佳实践总结:\n";
    std::cout << "1. 优先使用编译期计算减少运行时开销\n";
    std::cout << "2. 合理设计递归终止条件避免无限递归\n";
    std::cout << "3. 使用完美转发保持参数的值类别\n";
    std::cout << "4. 利用SFINAE和type_traits进行类型约束\n";
    std::cout << "5. 考虑编译时间和代码复杂度的平衡\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 变长模板参数深度解析\n";
    std::cout << "=============================\n";
    
    // 基础语法
    demonstrate_basic_variadic_templates();
    
    // 类型操作
    demonstrate_type_operations();
    
    // 完美转发
    demonstrate_perfect_forwarding();
    
    // 变长模板类
    demonstrate_variadic_class();
    
    // 函数绑定
    demonstrate_function_binding();
    
    // 编译期算法
    demonstrate_compile_time_algorithms();
    
    // 实际应用
    demonstrate_practical_applications();
    
    // 性能和最佳实践
    demonstrate_performance_and_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 04_variadic_templates.cpp -o variadic_templates
./variadic_templates

注意：部分C++17特性（如if constexpr）在注释中提及但不在代码中使用，
保持C++11兼容性。

关键学习点:
1. 掌握参数包的展开机制和递归模式
2. 理解编译期类型计算和元编程技巧
3. 学会结合完美转发实现通用工具函数
4. 了解变长模板在标准库中的应用（如tuple）
5. 掌握编译期算法的设计和实现
6. 学会在实际项目中合理应用变长模板
7. 理解性能影响和编译时间权衡
*/