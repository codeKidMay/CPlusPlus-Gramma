/**
 * C++11 decltype类型推导深度解析
 * 
 * 核心概念：
 * 1. decltype推导规则的精确定义
 * 2. 与auto的区别和应用场景对比
 * 3. 尾置返回类型在模板编程中的应用
 * 4. 完美转发和通用引用的类型推导
 * 5. 表达式值类别对decltype结果的影响
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <type_traits>
#include <functional>

// ===== 1. decltype基础推导规则 =====

void demonstrate_basic_decltype() {
    std::cout << "=== decltype基础推导规则演示 ===\n";
    
    int x = 42;
    const int cx = x;
    int& rx = x;
    const int& crx = cx;
    int* px = &x;
    
    // 规则1: 对于变量名，decltype返回变量的确切类型
    decltype(x) var1 = 10;        // int
    decltype(cx) var2 = 20;       // const int
    decltype(rx) var3 = x;        // int&
    decltype(crx) var4 = cx;      // const int&
    decltype(px) var5 = &x;       // int*
    
    std::cout << "基础变量类型推导:\n";
    std::cout << "decltype(x): " << typeid(decltype(x)).name() << std::endl;
    std::cout << "decltype(cx): " << typeid(decltype(cx)).name() << std::endl;
    std::cout << "decltype(rx): " << typeid(decltype(rx)).name() << std::endl;
    
    // 规则2: 对于函数调用，decltype返回函数返回类型
    auto get_int = []() -> int { return 42; };
    auto get_ref = [&]() -> int& { return x; };
    auto get_cref = [&]() -> const int& { return cx; };
    
    decltype(get_int()) func_result1 = 100;  // int
    decltype(get_ref()) func_result2 = x;    // int&
    decltype(get_cref()) func_result3 = cx;  // const int&
    
    std::cout << "\n函数返回类型推导:\n";
    std::cout << "decltype(get_int()): " << typeid(decltype(get_int())).name() << std::endl;
    std::cout << "decltype(get_ref()): " << typeid(decltype(get_ref())).name() << std::endl;
    
    std::cout << "\n";
}

// ===== 2. 表达式值类别的影响 =====

void demonstrate_expression_categories() {
    std::cout << "=== 表达式值类别对decltype的影响 ===\n";
    
    int x = 10;
    int arr[5] = {1, 2, 3, 4, 5};
    
    // 规则3: 对于左值表达式，decltype返回T&
    decltype((x)) expr1 = x;          // int& (注意双层括号)
    decltype(arr[0]) expr2 = arr[0];  // int& (数组下标是左值)
    decltype(*(&x)) expr3 = x;        // int& (解引用指针是左值)
    
    // 规则4: 对于右值表达式，decltype返回T
    decltype(x + 1) expr4 = 20;       // int (算术表达式是右值)
    decltype(std::move(x)) expr5 = 30; // int&& (std::move返回右值引用)
    
    std::cout << "表达式值类别推导:\n";
    std::cout << "decltype((x)): " << typeid(decltype((x))).name() << " (左值表达式)" << std::endl;
    std::cout << "decltype(arr[0]): " << typeid(decltype(arr[0])).name() << " (左值表达式)" << std::endl;
    std::cout << "decltype(x + 1): " << typeid(decltype(x + 1)).name() << " (右值表达式)" << std::endl;
    
    // 复杂表达式的类型推导
    std::vector<int> vec{1, 2, 3, 4, 5};
    decltype(vec[0]) vec_elem = vec[0];        // int& (vector的operator[]返回引用)
    decltype(vec.size()) vec_size = vec.size(); // size_t
    decltype(vec.begin()) vec_iter = vec.begin(); // vector<int>::iterator
    
    std::cout << "\n容器表达式推导:\n";
    std::cout << "decltype(vec[0]): " << typeid(decltype(vec[0])).name() << std::endl;
    std::cout << "decltype(vec.size()): " << typeid(decltype(vec.size())).name() << std::endl;
    
    std::cout << "\n";
}

// ===== 3. decltype vs auto 对比 =====

void demonstrate_decltype_vs_auto() {
    std::cout << "=== decltype vs auto 对比 ===\n";
    
    const int cx = 42;
    int& rx = const_cast<int&>(cx);
    
    // auto会忽略顶层const和引用
    auto auto_var1 = cx;    // int (忽略const)
    auto auto_var2 = rx;    // int (忽略引用)
    
    // decltype保持完整类型信息
    decltype(cx) decltype_var1 = cx;  // const int
    decltype(rx) decltype_var2 = rx;  // int&
    
    std::cout << "类型推导对比:\n";
    std::cout << "auto from const int: " << typeid(decltype(auto_var1)).name() 
              << " (忽略const)" << std::endl;
    std::cout << "decltype from const int: " << typeid(decltype(decltype_var1)).name() 
              << " (保持const)" << std::endl;
    
    std::cout << "auto from int&: " << typeid(decltype(auto_var2)).name() 
              << " (忽略引用)" << std::endl;
    std::cout << "decltype from int&: " << typeid(decltype(decltype_var2)).name() 
              << " (保持引用)" << std::endl;
    
    // 实际应用场景的选择
    std::vector<int> vec{1, 2, 3};
    
    // 想要拷贝元素值
    auto copied_elem = vec[0];  // int，拷贝
    
    // 想要引用元素
    decltype(vec[0]) ref_elem = vec[0];  // int&，引用
    
    // 或者使用auto&
    auto& auto_ref_elem = vec[0];  // int&，引用
    
    std::cout << "\n实际应用中的选择:\n";
    std::cout << "复制元素用auto: " << typeid(decltype(copied_elem)).name() << std::endl;
    std::cout << "引用元素用decltype或auto&: " << typeid(decltype(ref_elem)).name() << std::endl;
    
    std::cout << "\n";
}

// ===== 4. 尾置返回类型的强大应用 =====

// 传统方式的限制
template<typename Container>
auto get_element_old(Container& c, size_t index) -> decltype(c[index]) {
    return c[index];
}

// 更复杂的尾置返回类型
template<typename T, typename U>
auto multiply(T t, U u) -> decltype(t * u) {
    std::cout << "乘法运算类型: " << typeid(decltype(t * u)).name() << std::endl;
    return t * u;
}

// 条件返回类型
template<typename T>
auto process_value(T value) -> decltype(value > 0 ? value : -value) {
    return value > 0 ? value : -value;
}

// SFINAE与decltype结合
template<typename T>
auto has_size_method(T& obj) -> decltype(obj.size(), std::true_type{}) {
    return std::true_type{};
}

template<typename T>
std::false_type has_size_method(...) {
    return std::false_type{};
}

void demonstrate_trailing_return_types() {
    std::cout << "=== 尾置返回类型应用演示 ===\n";
    
    std::vector<int> vec{10, 20, 30};
    std::map<std::string, double> map{{"pi", 3.14}, {"e", 2.718}};
    
    // 通用元素访问
    auto vec_elem = get_element_old(vec, 1);     // int&
    auto map_elem = get_element_old(map, std::string("pi")); // double&
    
    std::cout << "通用元素访问:\n";
    std::cout << "vec[1] = " << vec_elem << std::endl;
    std::cout << "map[\"pi\"] = " << map_elem << std::endl;
    
    // 类型推导的乘法
    auto int_double = multiply(5, 3.14);     // double
    auto int_int = multiply(10, 20);         // int
    auto float_int = multiply(2.5f, 8);      // float
    
    std::cout << "\n类型推导乘法结果:\n";
    std::cout << "5 * 3.14 = " << int_double << std::endl;
    std::cout << "10 * 20 = " << int_int << std::endl;
    std::cout << "2.5f * 8 = " << float_int << std::endl;
    
    // SFINAE检测
    std::cout << "\nSFINAE方法检测:\n";
    std::cout << "vector有size方法: " << has_size_method(vec).value << std::endl;
    
    int plain_int = 42;
    std::cout << "int有size方法: " << has_size_method(plain_int).value << std::endl;
    
    std::cout << "\n";
}

// ===== 5. 完美转发中的类型推导 =====

// 通用转发函数
template<typename Func, typename... Args>
auto invoke_with_logging(Func&& func, Args&&... args) 
    -> decltype(std::forward<Func>(func)(std::forward<Args>(args)...)) {
    
    std::cout << "调用函数，参数个数: " << sizeof...(args) << std::endl;
    
    // 完美转发调用
    return std::forward<Func>(func)(std::forward<Args>(args)...);
}

// 测试用的函数和函数对象
int add_function(int a, int b) {
    return a + b;
}

struct Multiplier {
    int factor;
    explicit Multiplier(int f) : factor(f) {}
    
    int operator()(int x) const {
        return x * factor;
    }
};

void demonstrate_perfect_forwarding() {
    std::cout << "=== 完美转发中的类型推导演示 ===\n";
    
    // 转发普通函数
    auto result1 = invoke_with_logging(add_function, 10, 20);
    std::cout << "函数调用结果: " << result1 << std::endl;
    
    // 转发lambda
    auto lambda = [](const std::string& s, int n) -> std::string {
        return s + std::to_string(n);
    };
    auto result2 = invoke_with_logging(lambda, std::string("Number: "), 42);
    std::cout << "Lambda调用结果: " << result2 << std::endl;
    
    // 转发函数对象
    Multiplier mult(5);
    auto result3 = invoke_with_logging(mult, 8);
    std::cout << "函数对象调用结果: " << result3 << std::endl;
    
    // 转发成员函数（需要更复杂的实现）
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    auto size_result = invoke_with_logging(&std::vector<int>::size, test_vec);
    std::cout << "成员函数调用结果: " << size_result << std::endl;
    
    std::cout << "\n";
}

// ===== 6. 高级模板技巧 =====

// 类型萃取器
template<typename T>
struct function_traits;

// 特化：普通函数
template<typename R, typename... Args>
struct function_traits<R(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
    static constexpr size_t arity = sizeof...(Args);
};

// 特化：函数指针
template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

// 特化：成员函数指针
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> {
    using return_type = R;
    using class_type = C;
    using args_tuple = std::tuple<Args...>;
    static constexpr size_t arity = sizeof...(Args);
};

// 通用类型检测
template<typename T>
struct is_container {
private:
    template<typename C>
    static auto test(C* c) -> decltype(
        c->begin(),
        c->end(),
        c->size(),
        std::true_type{}
    );
    
    template<typename>
    static std::false_type test(...);
    
public:
    using type = decltype(test<T>(nullptr));
    static constexpr bool value = type::value;
};

// 复杂的类型转换检测
template<typename From, typename To>
struct is_convertible_custom {
private:
    template<typename F, typename T>
    static auto test(F&& from) -> decltype(static_cast<T>(from), std::true_type{});
    
    template<typename, typename>
    static std::false_type test(...);
    
public:
    using type = decltype(test<From, To>(std::declval<From>()));
    static constexpr bool value = type::value;
};

void demonstrate_advanced_techniques() {
    std::cout << "=== 高级模板技巧演示 ===\n";
    
    // 函数类型萃取
    using func_type = int(double, char);
    using func_ptr_type = int(*)(double, char);
    
    std::cout << "函数类型分析:\n";
    std::cout << "普通函数参数个数: " << function_traits<func_type>::arity << std::endl;
    std::cout << "函数指针参数个数: " << function_traits<func_ptr_type>::arity << std::endl;
    
    // 容器检测
    std::cout << "\n容器类型检测:\n";
    std::cout << "vector是容器: " << is_container<std::vector<int>>::value << std::endl;
    std::cout << "string是容器: " << is_container<std::string>::value << std::endl;
    std::cout << "int是容器: " << is_container<int>::value << std::endl;
    
    // 类型转换检测
    std::cout << "\n类型转换检测:\n";
    std::cout << "int可转换为double: " << is_convertible_custom<int, double>::value << std::endl;
    std::cout << "string可转换为int: " << is_convertible_custom<std::string, int>::value << std::endl;
    
    std::cout << "\n";
}

// ===== 7. 实际应用：通用容器操作库 =====

// 通用的容器元素访问
template<typename Container, typename Index>
auto safe_access(Container&& container, Index&& index) 
    -> decltype(std::forward<Container>(container)[std::forward<Index>(index)]) {
    
    if (index >= 0 && static_cast<size_t>(index) < container.size()) {
        return std::forward<Container>(container)[std::forward<Index>(index)];
    }
    throw std::out_of_range("索引越界");
}

// 通用的容器转换
template<typename Container, typename Func>
auto transform_container(const Container& container, Func func) 
    -> std::vector<decltype(func(*container.begin()))> {
    
    using result_type = decltype(func(*container.begin()));
    std::vector<result_type> result;
    result.reserve(container.size());
    
    for (const auto& item : container) {
        result.push_back(func(item));
    }
    
    return result;
}

// 通用的聚合操作
template<typename Container, typename Func, typename InitType>
auto accumulate_custom(const Container& container, InitType init, Func func)
    -> decltype(func(init, *container.begin())) {
    
    auto result = init;
    for (const auto& item : container) {
        result = func(result, item);
    }
    return result;
}

void demonstrate_practical_applications() {
    std::cout << "=== 实际应用：通用容器操作演示 ===\n";
    
    std::vector<int> numbers{1, 2, 3, 4, 5};
    std::vector<double> doubles{1.1, 2.2, 3.3, 4.4, 5.5};
    
    // 安全访问
    try {
        auto elem1 = safe_access(numbers, 2);
        auto elem2 = safe_access(doubles, 1);
        std::cout << "安全访问结果: " << elem1 << ", " << elem2 << std::endl;
        
        // auto bad_elem = safe_access(numbers, 10);  // 会抛出异常
    } catch (const std::exception& e) {
        std::cout << "访问异常: " << e.what() << std::endl;
    }
    
    // 容器转换
    auto squared_numbers = transform_container(numbers, [](int x) { return x * x; });
    auto int_from_double = transform_container(doubles, [](double x) { return static_cast<int>(x); });
    
    std::cout << "\n容器转换结果:\n";
    std::cout << "平方数: ";
    for (auto n : squared_numbers) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    std::cout << "双精度转整数: ";
    for (auto n : int_from_double) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // 聚合操作
    auto sum = accumulate_custom(numbers, 0, [](int a, int b) { return a + b; });
    auto product = accumulate_custom(doubles, 1.0, [](double a, double b) { return a * b; });
    auto concat = accumulate_custom(
        std::vector<std::string>{"Hello", " ", "World", "!"},
        std::string{},
        [](const std::string& a, const std::string& b) { return a + b; }
    );
    
    std::cout << "\n聚合操作结果:\n";
    std::cout << "数字求和: " << sum << std::endl;
    std::cout << "双精度乘积: " << product << std::endl;
    std::cout << "字符串连接: " << concat << std::endl;
    
    std::cout << "\n";
}

// ===== 8. 最佳实践和陷阱避免 =====

void demonstrate_best_practices() {
    std::cout << "=== decltype最佳实践和陷阱避免 ===\n";
    
    std::cout << "最佳实践:\n";
    std::cout << "1. 使用decltype保持精确的类型信息\n";
    std::cout << "2. 在模板中用尾置返回类型处理复杂表达式\n";
    std::cout << "3. 结合SFINAE进行类型检测和约束\n";
    std::cout << "4. 注意表达式的值类别对结果类型的影响\n";
    std::cout << "5. 在完美转发中保持参数的原始类型\n";
    
    std::cout << "\n常见陷阱:\n";
    
    // 陷阱1: 双层括号的影响
    int x = 42;
    decltype(x) var1 = 10;    // int
    decltype((x)) var2 = x;   // int& - 注意双层括号！
    
    std::cout << "陷阱1 - 双层括号:\n";
    std::cout << "decltype(x): " << typeid(decltype(x)).name() << " (变量名)" << std::endl;
    std::cout << "decltype((x)): " << typeid(decltype((x))).name() << " (左值表达式)" << std::endl;
    
    // 陷阱2: 临时对象的生命周期
    std::vector<int> vec{1, 2, 3};
    // decltype(std::move(vec)) moved_vec = std::move(vec);  // 危险！右值引用绑定到临时对象
    
    // 陷阱3: 函数调用 vs 函数指针
    auto func = []() { return 42; };
    decltype(func()) result = 100;   // int (函数调用的返回类型)
    decltype(func) func_copy = func; // lambda类型 (函数对象类型)
    
    std::cout << "\n陷阱3 - 函数调用vs函数对象:\n";
    std::cout << "decltype(func()): " << typeid(decltype(func())).name() << std::endl;
    std::cout << "decltype(func): " << typeid(decltype(func)).name() << std::endl;
    
    std::cout << "\n使用建议:\n";
    std::cout << "- 需要精确类型时使用decltype\n";
    std::cout << "- 需要简化类型时使用auto\n";
    std::cout << "- 模板中需要推导返回类型时使用尾置返回类型\n";
    std::cout << "- 注意表达式的复杂性，避免过度复杂的类型推导\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 decltype类型推导深度解析\n";
    std::cout << "===============================\n";
    
    // 基础推导规则
    demonstrate_basic_decltype();
    
    // 表达式值类别影响
    demonstrate_expression_categories();
    
    // decltype vs auto
    demonstrate_decltype_vs_auto();
    
    // 尾置返回类型
    demonstrate_trailing_return_types();
    
    // 完美转发
    demonstrate_perfect_forwarding();
    
    // 高级技巧
    demonstrate_advanced_techniques();
    
    // 实际应用
    demonstrate_practical_applications();
    
    // 最佳实践
    demonstrate_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 06_decltype_type_deduction.cpp -o decltype_demo
./decltype_demo

关键学习点:
1. 掌握decltype的四条基本推导规则
2. 理解表达式值类别对decltype结果的影响
3. 学会在模板编程中使用尾置返回类型
4. 掌握decltype与auto的区别和选择时机
5. 了解decltype在SFINAE中的应用
6. 学会使用decltype实现类型萃取和检测
7. 避免常见的decltype使用陷阱
8. 理解decltype在现代C++泛型编程中的重要作用
*/