/**
 * C++14 函数返回类型推导深度解析
 * 
 * 核心概念：
 * 1. auto返回类型推导规则和限制条件
 * 2. 递归函数的类型推导延迟机制
 * 3. 模板函数中的返回类型推导优化
 * 4. decltype(auto)的精确类型保持
 * 5. SFINAE与auto返回类型的交互
 */

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <memory>
#include <functional>
#include <chrono>
#include <map>

// ===== 1. C++11尾置返回类型的限制回顾 =====

// C++11方式：繁琐的尾置返回类型
template<typename Container>
auto get_element_cpp11(Container& container, size_t index) 
    -> decltype(container[index]) {
    return container[index];
}

// C++11递归函数必须显式指定返回类型
template<typename T>
constexpr auto factorial_cpp11(T n) -> T {
    return n <= 1 ? 1 : n * factorial_cpp11(n - 1);
}

void demonstrate_cpp11_limitations() {
    std::cout << "=== C++11尾置返回类型限制回顾 ===\n";
    
    std::vector<int> vec{10, 20, 30, 40};
    auto& elem = get_element_cpp11(vec, 2);
    
    std::cout << "C++11方式获取元素: " << elem << std::endl;
    std::cout << "C++11递归阶乘: " << factorial_cpp11(5) << std::endl;
    
    std::cout << "C++11的问题:\n";
    std::cout << "1. 需要复杂的decltype表达式\n";
    std::cout << "2. 递归函数必须显式指定返回类型\n";
    std::cout << "3. 代码冗长，可读性差\n";
    std::cout << "4. 容易出错的decltype推导\n";
    
    std::cout << "\n";
}

// ===== 2. C++14 auto返回类型推导机制 =====

// C++14简化语法
template<typename Container>
auto get_element_cpp14(Container& container, size_t index) {
    return container[index];  // 编译器自动推导返回类型
}

// 多个返回语句的类型一致性要求
auto consistent_returns(bool flag) {
    if (flag) {
        return 42;        // 推导为int
    } else {
        return 24;        // 也是int，类型一致
    }
}

// 编译错误示例：类型不一致
/*
auto inconsistent_returns(bool flag) {
    if (flag) {
        return 42;        // int
    } else {
        return 3.14;      // double - 编译错误！
    }
}
*/

// 正确处理方式：统一类型
auto consistent_mixed_returns(bool flag) -> double {
    if (flag) {
        return 42.0;      // 显式double
    } else {
        return 3.14;      // double
    }
}

// 复杂表达式的类型推导
template<typename T, typename U>
auto complex_operation(T&& t, U&& u) {
    // 编译器推导复杂表达式的类型
    return std::forward<T>(t) * std::forward<U>(u) + 1;
}

void demonstrate_auto_return_deduction() {
    std::cout << "=== C++14 auto返回类型推导机制 ===\n";
    
    // 容器元素访问
    std::vector<int> vec{100, 200, 300};
    std::map<std::string, double> map{{"pi", 3.14159}, {"e", 2.71828}};
    
    auto vec_elem = get_element_cpp14(vec, 1);
    auto map_elem = get_element_cpp14(map, std::string("pi"));
    
    std::cout << "vector元素: " << vec_elem << " (类型: " << typeid(vec_elem).name() << ")\n";
    std::cout << "map元素: " << map_elem << " (类型: " << typeid(map_elem).name() << ")\n";
    
    // 一致性返回类型
    auto consistent_result = consistent_returns(true);
    std::cout << "一致性返回: " << consistent_result << std::endl;
    
    // 混合类型的正确处理
    auto mixed_result = consistent_mixed_returns(false);
    std::cout << "混合类型返回: " << mixed_result << std::endl;
    
    // 复杂表达式推导
    auto complex_result = complex_operation(3, 4.5);
    std::cout << "复杂表达式结果: " << complex_result 
              << " (类型: " << typeid(complex_result).name() << ")\n";
    
    std::cout << "C++14的优势:\n";
    std::cout << "1. 语法简洁，无需decltype\n";
    std::cout << "2. 编译器自动推导复杂类型\n";
    std::cout << "3. 减少代码重复和错误\n";
    std::cout << "4. 提高代码可维护性\n";
    
    std::cout << "\n";
}

// ===== 3. 递归函数的延迟推导机制 =====

// C++14递归函数：编译器延迟推导
auto factorial_cpp14(int n) {
    if (n <= 1) {
        return 1;  // 编译器首次遇到return，推导为int
    }
    return n * factorial_cpp14(n - 1);  // 递归调用在类型确定后才检查
}

// 更复杂的递归：斐波那契数列
auto fibonacci_cpp14(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;    // 推导为int
    return fibonacci_cpp14(n-1) + fibonacci_cpp14(n-2);
}

// 相互递归函数的处理
auto is_even(int n) -> bool;  // 前向声明必须指定返回类型
auto is_odd(int n) -> bool;   // 前向声明必须指定返回类型

auto is_even(int n) -> bool {
    return n == 0 ? true : is_odd(n - 1);
}

auto is_odd(int n) -> bool {
    return n == 0 ? false : is_even(n - 1);
}

// 模板递归函数
template<typename T>
auto gcd(T a, T b) {
    return b == 0 ? a : gcd(b, a % b);
}

// 递归类型推导的性能分析
class RecursionTimer {
public:
    template<typename Func>
    static auto time_function(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return std::make_pair(result, duration.count());
    }
};

void demonstrate_recursive_deduction() {
    std::cout << "=== 递归函数延迟推导机制 ===\n";
    
    // 基本递归函数
    auto fact_result = factorial_cpp14(8);
    std::cout << "递归阶乘 8!: " << fact_result << std::endl;
    
    auto fib_result = fibonacci_cpp14(10);
    std::cout << "斐波那契数列第10项: " << fib_result << std::endl;
    
    // 相互递归
    std::cout << "15是偶数: " << (is_even(15) ? "是" : "否") << std::endl;
    std::cout << "16是偶数: " << (is_even(16) ? "是" : "否") << std::endl;
    
    // 模板递归
    auto gcd_result = gcd(48, 18);
    std::cout << "gcd(48, 18): " << gcd_result << std::endl;
    
    // 性能对比：递归 vs 迭代
    std::cout << "\n递归vs迭代性能对比:\n";
    
    auto recursive_test = []() { return factorial_cpp14(10); };
    auto iterative_test = []() {
        int result = 1;
        for (int i = 2; i <= 10; ++i) {
            result *= i;
        }
        return result;
    };
    
    auto [rec_result, rec_time] = RecursionTimer::time_function(recursive_test);
    auto [iter_result, iter_time] = RecursionTimer::time_function(iterative_test);
    
    std::cout << "递归结果: " << rec_result << ", 时间: " << rec_time << "μs\n";
    std::cout << "迭代结果: " << iter_result << ", 时间: " << iter_time << "μs\n";
    
    std::cout << "递归推导机制:\n";
    std::cout << "1. 延迟推导直到遇到第一个return语句\n";
    std::cout << "2. 所有return语句必须推导出相同类型\n";
    std::cout << "3. 相互递归需要前向声明\n";
    std::cout << "4. 模板递归支持类型推导\n";
    
    std::cout << "\n";
}

// ===== 4. decltype(auto) 精确类型保持 =====

// auto vs decltype(auto) 对比
template<typename Container>
auto get_element_auto(Container& container, size_t index) {
    return container[index];  // auto：可能丢失引用性
}

template<typename Container>
decltype(auto) get_element_decltype_auto(Container& container, size_t index) {
    return container[index];  // decltype(auto)：保持精确类型
}

// 完美转发中的应用
template<typename Func, typename... Args>
decltype(auto) perfect_forward_call(Func&& func, Args&&... args) {
    std::cout << "完美转发调用\n";
    return func(std::forward<Args>(args)...);  // 保持返回值的精确类型
}

// 复杂表达式的类型保持
decltype(auto) complex_expression_return() {
    static int x = 42;
    return (x);  // 返回 int&，不是 int
}

// 右值引用的保持
template<typename T>
decltype(auto) move_and_return(T&& obj) {
    return std::move(obj);  // 保持右值引用特性
}

void demonstrate_decltype_auto() {
    std::cout << "=== decltype(auto) 精确类型保持 ===\n";
    
    std::vector<int> vec{100, 200, 300, 400};
    
    // 对比auto和decltype(auto)
    auto elem_auto = get_element_auto(vec, 2);           // int (值拷贝)
    decltype(auto) elem_decltype = get_element_decltype_auto(vec, 2); // int& (引用)
    
    std::cout << "原始元素: " << vec[2] << std::endl;
    
    // 修改通过auto获取的值（不会影响原容器）
    elem_auto = 999;
    std::cout << "修改auto元素后，原始元素: " << vec[2] << std::endl;
    
    // 修改通过decltype(auto)获取的引用（会影响原容器）
    elem_decltype = 888;
    std::cout << "修改decltype(auto)元素后，原始元素: " << vec[2] << std::endl;
    
    // 类型信息对比
    std::cout << "\n类型信息对比:\n";
    std::cout << "auto返回类型: " << typeid(decltype(get_element_auto(vec, 0))).name() << std::endl;
    std::cout << "decltype(auto)返回类型: " << typeid(decltype(get_element_decltype_auto(vec, 0))).name() << std::endl;
    
    // 完美转发应用
    auto lambda = [](int& x) -> int& { 
        x *= 2; 
        return x; 
    };
    
    int test_value = 10;
    decltype(auto) forward_result = perfect_forward_call(lambda, test_value);
    std::cout << "完美转发结果: " << forward_result << ", 原值: " << test_value << std::endl;
    
    // 复杂表达式
    decltype(auto) complex_ref = complex_expression_return();
    std::cout << "复杂表达式返回引用: " << complex_ref << std::endl;
    complex_ref = 100;  // 可以修改，因为是引用
    std::cout << "修改后: " << complex_expression_return() << std::endl;
    
    std::cout << "decltype(auto)的优势:\n";
    std::cout << "1. 精确保持表达式的类型和值类别\n";
    std::cout << "2. 适用于完美转发场景\n";
    std::cout << "3. 避免意外的类型转换\n";
    std::cout << "4. 在泛型编程中保持类型精确性\n";
    
    std::cout << "\n";
}

// ===== 5. SFINAE与auto返回类型的交互 =====

// 使用auto返回类型的SFINAE检测
template<typename T>
auto has_size_method_auto(T& obj) -> decltype(obj.size(), bool{}) {
    return true;
}

template<typename T>
bool has_size_method_auto(...) {
    return false;
}

// 条件返回类型
template<typename T>
auto process_container_auto(T& container) {
    if constexpr (std::is_same_v<decltype(has_size_method_auto(container)), bool>) {
        if (has_size_method_auto(container)) {
            return container.size();  // 返回size_t
        }
    }
    return 0;  // 返回int - 编译错误：类型不一致！
}

// 正确的条件返回类型处理
template<typename T>
auto process_container_correct(T& container) -> size_t {
    if constexpr (std::is_same_v<decltype(has_size_method_auto(container)), bool>) {
        if (has_size_method_auto(container)) {
            return container.size();
        }
    }
    return 0;  // 显式转换为size_t
}

// 基于auto的重载决议
struct HasSize { size_t size() const { return 42; } };
struct NoSize { int value = 10; };

template<typename T>
auto get_info(T& obj) -> std::enable_if_t<
    std::is_same_v<decltype(obj.size()), size_t>, 
    size_t
> {
    std::cout << "有size方法的类型\n";
    return obj.size();
}

template<typename T>
auto get_info(T& obj) -> std::enable_if_t<
    !std::is_same_v<decltype(obj.value), void>, 
    int
> {
    std::cout << "有value成员的类型\n";
    return obj.value;
}

void demonstrate_sfinae_auto_interaction() {
    std::cout << "=== SFINAE与auto返回类型交互 ===\n";
    
    // 类型检测
    std::vector<int> vec{1, 2, 3, 4, 5};
    int plain_value = 42;
    
    std::cout << "vector有size方法: " << has_size_method_auto(vec) << std::endl;
    std::cout << "int有size方法: " << has_size_method_auto(plain_value) << std::endl;
    
    // 条件处理
    auto vec_result = process_container_correct(vec);
    auto int_result = process_container_correct(plain_value);
    
    std::cout << "vector处理结果: " << vec_result << std::endl;
    std::cout << "int处理结果: " << int_result << std::endl;
    
    // 基于auto的重载决议
    HasSize has_size_obj;
    NoSize no_size_obj;
    
    auto size_result = get_info(has_size_obj);
    auto value_result = get_info(no_size_obj);
    
    std::cout << "HasSize结果: " << size_result << std::endl;
    std::cout << "NoSize结果: " << value_result << std::endl;
    
    std::cout << "SFINAE与auto交互要点:\n";
    std::cout << "1. auto推导必须在SFINAE检查之后\n";
    std::cout << "2. 条件返回需要保证类型一致性\n";
    std::cout << "3. 可以结合enable_if进行重载决议\n";
    std::cout << "4. 注意推导失败的错误处理\n";
    
    std::cout << "\n";
}

// ===== 6. 模板函数的返回类型推导优化 =====

// 复杂的模板函数族
template<typename Container>
auto begin_iterator(Container& container) {
    return container.begin();
}

template<typename Container>
auto end_iterator(Container& container) {
    return container.end();  
}

template<typename Iterator>
auto iterator_distance(Iterator first, Iterator last) {
    return std::distance(first, last);
}

// 模板特化中的auto返回类型
template<typename T>
auto process_arithmetic(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value * 2;        // 整数处理
    } else if constexpr (std::is_floating_point_v<T>) {
        return value * 1.5;      // 浮点数处理
    } else {
        return value;            // 其他类型
    }
}

// 链式操作的类型推导
template<typename T>
auto chain_operations(T&& input) {
    return std::forward<T>(input);
}

template<typename T, typename Func, typename... Funcs>
auto chain_operations(T&& input, Func&& func, Funcs&&... funcs) {
    return chain_operations(func(std::forward<T>(input)), std::forward<Funcs>(funcs)...);
}

// 类型推导的编译时优化检查
template<typename T>
constexpr bool is_auto_deducible_v = true;  // 简化检查

void demonstrate_template_deduction_optimization() {
    std::cout << "=== 模板函数返回类型推导优化 ===\n";
    
    // 迭代器处理
    std::vector<int> vec{10, 20, 30, 40, 50};
    std::string str = "hello world";
    
    auto vec_begin = begin_iterator(vec);
    auto vec_end = end_iterator(vec);
    auto str_begin = begin_iterator(str);
    auto str_end = end_iterator(str);
    
    std::cout << "vector距离: " << iterator_distance(vec_begin, vec_end) << std::endl;
    std::cout << "string距离: " << iterator_distance(str_begin, str_end) << std::endl;
    
    // 算术类型处理
    auto int_result = process_arithmetic(10);
    auto double_result = process_arithmetic(3.14);
    auto char_result = process_arithmetic('A');
    
    std::cout << "整数处理: " << int_result << std::endl;
    std::cout << "浮点处理: " << double_result << std::endl;
    std::cout << "字符处理: " << static_cast<int>(char_result) << std::endl;
    
    // 链式操作
    auto square = [](int x) { return x * x; };
    auto add_one = [](int x) { return x + 1; };
    auto to_string = [](int x) { return std::to_string(x); };
    
    auto chain_result = chain_operations(5, square, add_one, to_string);
    std::cout << "链式操作结果: " << chain_result << std::endl;
    
    std::cout << "模板推导优化效果:\n";
    std::cout << "1. 减少模板实例化的复杂度\n";
    std::cout << "2. 提高编译时性能\n";
    std::cout << "3. 简化复杂模板的使用\n";
    std::cout << "4. 增强代码的通用性\n";
    
    std::cout << "\n";
}

// ===== 7. 性能分析和最佳实践 =====

// 返回类型推导的编译时间测试
template<int N>
struct CompileTimeTest {
    static auto generate_sequence() {
        if constexpr (N <= 0) {
            return std::vector<int>{};
        } else {
            auto vec = CompileTimeTest<N-1>::generate_sequence();
            vec.push_back(N);
            return vec;
        }
    }
};

// 运行时性能对比
class PerformanceAnalyzer {
public:
    template<typename Func>
    static auto measure_execution(Func&& func, int iterations = 1000000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            volatile auto result = func();
            (void)result;  // 防止优化
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }
};

void demonstrate_performance_analysis() {
    std::cout << "=== 性能分析和最佳实践 ===\n";
    
    // 编译时序列生成测试
    auto sequence = CompileTimeTest<10>::generate_sequence();
    std::cout << "编译期生成序列大小: " << sequence.size() << std::endl;
    
    // 运行时性能对比
    auto explicit_return = []() -> int { return 42; };
    auto auto_return = []() { return 42; };
    auto decltype_auto_return = []() -> decltype(auto) { return 42; };
    
    auto explicit_time = PerformanceAnalyzer::measure_execution(explicit_return);
    auto auto_time = PerformanceAnalyzer::measure_execution(auto_return);
    auto decltype_auto_time = PerformanceAnalyzer::measure_execution(decltype_auto_return);
    
    std::cout << "性能测试结果 (1M次调用):\n";
    std::cout << "显式返回类型: " << explicit_time.count() << "μs\n";
    std::cout << "auto返回类型: " << auto_time.count() << "μs\n";
    std::cout << "decltype(auto): " << decltype_auto_time.count() << "μs\n";
    
    std::cout << "\n最佳实践总结:\n";
    std::cout << "✓ 简单情况优先使用auto\n";
    std::cout << "✓ 需要保持精确类型时使用decltype(auto)\n";
    std::cout << "✓ 递归函数注意返回类型一致性\n";
    std::cout << "✓ 复杂模板中考虑推导失败的处理\n";
    std::cout << "✓ 性能敏感场景测试推导开销\n";
    
    std::cout << "\n注意事项:\n";
    std::cout << "⚠ 多返回路径必须类型一致\n";
    std::cout << "⚠ 递归函数的推导延迟特性\n";
    std::cout << "⚠ 模板特化中的推导限制\n";
    std::cout << "⚠ 与SFINAE结合时的复杂性\n";
    
    std::cout << "\n编译器优化效果:\n";
    std::cout << "• 返回类型推导通常零运行时开销\n";
    std::cout << "• 编译期完成所有类型确定\n";
    std::cout << "• 内联优化消除推导开销\n";
    std::cout << "• 模板实例化可能影响编译时间\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++14 函数返回类型推导深度解析\n";
    std::cout << "===============================\n";
    
    // C++11限制回顾
    demonstrate_cpp11_limitations();
    
    // auto返回类型推导
    demonstrate_auto_return_deduction();
    
    // 递归函数推导
    demonstrate_recursive_deduction();
    
    // decltype(auto)应用
    demonstrate_decltype_auto();
    
    // SFINAE交互
    demonstrate_sfinae_auto_interaction();
    
    // 模板推导优化
    demonstrate_template_deduction_optimization();
    
    // 性能分析
    demonstrate_performance_analysis();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall 01_function_return_type_deduction.cpp -o return_deduction_demo
./return_deduction_demo

关键学习点:
1. 理解C++14返回类型推导相比C++11的重大简化
2. 掌握递归函数中延迟推导的工作机制
3. 学会区分auto和decltype(auto)的适用场景
4. 理解类型一致性要求对函数设计的影响
5. 掌握SFINAE与auto返回类型的交互模式
6. 了解模板函数中返回类型推导的优化效果
7. 学会在性能敏感场景中合理使用推导功能
8. 避免推导失败和类型不一致的常见陷阱

注意事项:
- 所有return语句必须推导出相同的类型
- 递归函数的类型推导存在延迟，需要谨慎处理
- decltype(auto)保持表达式的精确类型和值类别
- 与模板和SFINAE结合时需要考虑推导失败的情况
*/