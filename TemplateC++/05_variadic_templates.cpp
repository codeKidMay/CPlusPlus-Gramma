/**
 * 第5章：变长模板的威力深度解析
 * 
 * 核心概念：
 * 1. 参数包展开 - 变长参数的语法机制和展开规则
 * 2. 递归实例化 - 经典的递归处理模式和终止条件
 * 3. 折叠表达式 - C++17的四种折叠形式和应用
 * 4. 索引序列 - 编译期索引生成和高级技巧
 * 5. 完美转发包 - 参数包的完美转发机制
 */

#include <iostream>
#include <tuple>
#include <array>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <functional>
#include <chrono>
#include <algorithm>

// ===== 1. 参数包展开的语法机制深度解析 =====
void demonstrate_parameter_pack_expansion() {
    std::cout << "=== 参数包展开的语法机制深度解析 ===\n";
    
    // 基础参数包展开语法
    auto basic_expansion_demo = []<typename... Types>(Types... args) {
        std::cout << "参数个数: " << sizeof...(Types) << "\n";
        std::cout << "参数总大小: " << (sizeof(Types) + ...) << " bytes\n";
        
        // 各种展开形式
        std::tuple<Types...> tuple_expansion{args...};              // 构造函数参数包展开
        std::array<std::common_type_t<Types...>, sizeof...(Types)> array_expansion{args...}; // 数组初始化
        
        // 函数调用中的参数包展开
        auto print_each = [](auto&& arg) {
            std::cout << "  Value: " << arg << " (type: " << typeid(arg).name() << ")\n";
        };
        (print_each(args), ...);  // C++17折叠表达式
        
        return tuple_expansion;
    };
    
    auto result = basic_expansion_demo(42, 3.14, "hello", 'c');
    std::cout << "Tuple size: " << std::tuple_size_v<decltype(result)> << "\n";
    
    // 参数包在模板特化中的应用
    template<typename... Types>
    struct ParameterPackAnalyzer {
        static constexpr size_t count = sizeof...(Types);
        static constexpr size_t total_size = (sizeof(Types) + ...);
        
        // 类型包的第一个和最后一个类型
        using first_type = std::tuple_element_t<0, std::tuple<Types...>>;
        using last_type = std::tuple_element_t<sizeof...(Types) - 1, std::tuple<Types...>>;
        
        static void analyze() {
            std::cout << "\n--- 参数包分析 ---\n";
            std::cout << "类型数量: " << count << "\n";
            std::cout << "总大小: " << total_size << " bytes\n";
            std::cout << "第一个类型大小: " << sizeof(first_type) << " bytes\n";
            std::cout << "最后一个类型大小: " << sizeof(last_type) << " bytes\n";
        }
    };
    
    ParameterPackAnalyzer<int, double, std::string, char>::analyze();
    
    std::cout << "\n";
}

// ===== 2. 递归模板实例化的设计模式 =====
void demonstrate_recursive_instantiation() {
    std::cout << "=== 递归模板实例化的设计模式 ===\n";
    
    // 经典递归模式：可变参数打印
    template<typename T>
    void recursive_print(T&& value) {
        std::cout << value << "\n";  // 递归基础情况
    }
    
    template<typename T, typename... Args>
    void recursive_print(T&& value, Args&&... args) {
        std::cout << value << " ";
        recursive_print(std::forward<Args>(args)...);  // 递归调用
    }
    
    std::cout << "递归打印示例: ";
    recursive_print(1, 2.5, "hello", 'x', true);
    
    // 递归计算：编译期最大值计算
    template<typename T>
    constexpr T recursive_max(T value) {
        return value;  // 单个值的情况
    }
    
    template<typename T, typename... Args>
    constexpr T recursive_max(T first, Args... args) {
        T rest_max = recursive_max(args...);
        return (first > rest_max) ? first : rest_max;
    }
    
    constexpr auto max_result = recursive_max(3, 7, 2, 9, 4);
    std::cout << "递归最大值: " << max_result << "\n";
    
    // 递归类型操作：类型列表处理
    template<typename... Types>
    struct TypeList {
        static constexpr size_t size = sizeof...(Types);
    };
    
    template<typename T, typename... Rest>
    struct Head {
        using type = T;
    };
    
    template<typename T, typename... Rest>
    struct Tail {
        using type = TypeList<Rest...>;
    };
    
    // 递归类型查找
    template<typename Target, typename... Types>
    struct Contains : std::false_type {};
    
    template<typename Target, typename Head, typename... Tail>
    struct Contains<Target, Head, Tail...> : 
        std::conditional_t<std::is_same_v<Target, Head>, 
                          std::true_type, 
                          Contains<Target, Tail...>> {};
    
    using TestList = TypeList<int, double, std::string>;
    static_assert(Contains<double, int, double, std::string>::value);
    static_assert(!Contains<char, int, double, std::string>::value);
    
    std::cout << "递归类型查找测试通过\n";
    
    // 递归深度控制和优化
    template<int N>
    struct RecursionDepthTest {
        static constexpr int value = RecursionDepthTest<N-1>::value + 1;
    };
    
    template<>
    struct RecursionDepthTest<0> {
        static constexpr int value = 0;
    };
    
    // 大多数编译器支持的递归深度
    constexpr int depth_500 = RecursionDepthTest<500>::value;
    std::cout << "递归深度500测试: " << depth_500 << "\n";
    
    std::cout << "\n";
}

// ===== 3. C++17折叠表达式的四种形式 =====
void demonstrate_fold_expressions() {
    std::cout << "=== C++17折叠表达式的四种形式 ===\n";
    
    // 一元右折叠：(pack op ...)
    template<typename... Args>
    auto unary_right_fold_sum(Args... args) {
        return (args + ...);  // 展开为：a1 + (a2 + (a3 + a4))
    }
    
    // 一元左折叠：(... op pack)
    template<typename... Args>
    auto unary_left_fold_sum(Args... args) {
        return (... + args);  // 展开为：((a1 + a2) + a3) + a4
    }
    
    // 二元右折叠：(pack op ... op init)
    template<typename... Args>
    auto binary_right_fold_sum(Args... args) {
        return (args + ... + 0);  // 展开为：a1 + (a2 + (a3 + (a4 + 0)))
    }
    
    // 二元左折叠：(init op ... op pack)
    template<typename... Args>
    auto binary_left_fold_sum(Args... args) {
        return (0 + ... + args);  // 展开为：(((0 + a1) + a2) + a3) + a4
    }
    
    std::cout << "右折叠求和: " << unary_right_fold_sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "左折叠求和: " << unary_left_fold_sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "二元右折叠: " << binary_right_fold_sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "二元左折叠: " << binary_left_fold_sum(1, 2, 3, 4, 5) << "\n";
    
    // 折叠表达式的高级应用
    
    // 逻辑折叠
    template<typename... Args>
    constexpr bool all_true(Args... args) {
        return (args && ...);  // 所有参数都为true
    }
    
    template<typename... Args>
    constexpr bool any_true(Args... args) {
        return (args || ...);  // 任一参数为true
    }
    
    std::cout << "全为真测试: " << all_true(true, true, true) << "\n";
    std::cout << "任一为真测试: " << any_true(false, true, false) << "\n";
    
    // 类型检查折叠
    template<typename T, typename... Types>
    constexpr bool all_same_type() {
        return (std::is_same_v<T, Types> && ...);
    }
    
    static_assert(all_same_type<int, int, int, int>());
    static_assert(!all_same_type<int, int, double, int>());
    
    // 函数调用折叠
    template<typename F, typename... Args>
    void call_with_each(F func, Args&&... args) {
        (func(std::forward<Args>(args)), ...);
    }
    
    auto printer = [](auto&& value) {
        std::cout << "调用: " << value << " ";
    };
    
    std::cout << "函数调用折叠: ";
    call_with_each(printer, 1, 2.5, "hello");
    std::cout << "\n";
    
    // 容器插入折叠
    template<typename Container, typename... Elements>
    void insert_all(Container& container, Elements&&... elements) {
        (container.push_back(std::forward<Elements>(elements)), ...);
    }
    
    std::vector<int> vec;
    insert_all(vec, 1, 2, 3, 4, 5);
    std::cout << "容器插入结果: ";
    for (int x : vec) std::cout << x << " ";
    std::cout << "\n";
    
    std::cout << "\n";
}

// ===== 4. 索引序列技巧的高级应用 =====
void demonstrate_index_sequence_tricks() {
    std::cout << "=== 索引序列技巧的高级应用 ===\n";
    
    // 基础索引序列应用：tuple转数组
    template<typename Tuple, std::size_t... I>
    constexpr auto tuple_to_array_impl(const Tuple& t, std::index_sequence<I...>) {
        using ElementType = std::tuple_element_t<0, Tuple>;
        return std::array<ElementType, sizeof...(I)>{std::get<I>(t)...};
    }
    
    template<typename... Types>
    constexpr auto tuple_to_array(const std::tuple<Types...>& t) {
        static_assert((std::is_same_v<Types, std::tuple_element_t<0, std::tuple<Types...>>> && ...),
                     "All tuple elements must be of the same type");
        return tuple_to_array_impl(t, std::index_sequence_for<Types...>{});
    }
    
    constexpr auto test_tuple = std::make_tuple(1, 2, 3, 4, 5);
    constexpr auto result_array = tuple_to_array(test_tuple);
    
    std::cout << "Tuple转数组: ";
    for (const auto& elem : result_array) {
        std::cout << elem << " ";
    }
    std::cout << "\n";
    
    // 高级应用：函数参数展开
    template<typename F, typename Tuple, std::size_t... I>
    constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
        return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
    }
    
    template<typename F, typename Tuple>
    constexpr decltype(auto) my_apply(F&& f, Tuple&& t) {
        return apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
                         std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
    }
    
    auto add_function = [](int a, int b, int c) { return a + b + c; };
    auto args = std::make_tuple(10, 20, 30);
    int sum_result = my_apply(add_function, args);
    std::cout << "函数应用结果: " << sum_result << "\n";
    
    // 编译期循环：生成斐波那契数列
    template<std::size_t... I>
    constexpr auto generate_fibonacci_impl(std::index_sequence<I...>) {
        constexpr auto fib = [](std::size_t n) constexpr -> std::size_t {
            if (n <= 1) return n;
            std::size_t a = 0, b = 1;
            for (std::size_t i = 2; i <= n; ++i) {
                std::size_t temp = a + b;
                a = b;
                b = temp;
            }
            return b;
        };
        return std::array<std::size_t, sizeof...(I)>{fib(I)...};
    }
    
    template<std::size_t N>
    constexpr auto generate_fibonacci() {
        return generate_fibonacci_impl(std::make_index_sequence<N>{});
    }
    
    constexpr auto fib_sequence = generate_fibonacci<10>();
    std::cout << "斐波那契数列: ";
    for (const auto& num : fib_sequence) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    // 反向索引序列
    template<std::size_t N, std::size_t... I>
    constexpr auto reverse_index_sequence_impl(std::index_sequence<I...>) {
        return std::index_sequence<(N - 1 - I)...>{};
    }
    
    template<std::size_t N>
    constexpr auto reverse_index_sequence() {
        return reverse_index_sequence_impl<N>(std::make_index_sequence<N>{});
    }
    
    template<typename Array, std::size_t... I>
    constexpr auto reverse_array_impl(const Array& arr, std::index_sequence<I...>) {
        return std::array<typename Array::value_type, sizeof...(I)>{arr[I]...};
    }
    
    template<typename T, std::size_t N>
    constexpr auto reverse_array(const std::array<T, N>& arr) {
        return reverse_array_impl(arr, reverse_index_sequence<N>());
    }
    
    constexpr std::array<int, 5> original{1, 2, 3, 4, 5};
    constexpr auto reversed = reverse_array(original);
    
    std::cout << "原始数组: ";
    for (const auto& elem : original) std::cout << elem << " ";
    std::cout << "\n反转数组: ";
    for (const auto& elem : reversed) std::cout << elem << " ";
    std::cout << "\n";
    
    std::cout << "\n";
}

// ===== 5. 参数包的完美转发和性能优化 =====
void demonstrate_perfect_forwarding_packs() {
    std::cout << "=== 参数包的完美转发和性能优化 ===\n";
    
    // 完美转发的工厂函数
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique_perfect(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    // 测试类
    struct TestObject {
        int value;
        std::string name;
        
        TestObject(int v, const std::string& n) : value(v), name(n) {
            std::cout << "TestObject构造: " << value << ", " << name << "\n";
        }
        
        TestObject(int v, std::string&& n) : value(v), name(std::move(n)) {
            std::cout << "TestObject移动构造: " << value << ", " << name << "\n";
        }
    };
    
    std::string temp_name = "移动测试";
    auto obj1 = make_unique_perfect<TestObject>(42, "常量字符串");
    auto obj2 = make_unique_perfect<TestObject>(43, std::move(temp_name));
    
    // 通用调用包装器
    template<typename F, typename... Args>
    decltype(auto) invoke_perfect(F&& func, Args&&... args) {
        std::cout << "调用函数，参数数量: " << sizeof...(args) << "\n";
        
        if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
            std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
            std::cout << "void函数调用完成\n";
        } else {
            auto result = std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
            std::cout << "函数调用结果: " << result << "\n";
            return result;
        }
    }
    
    auto add_lambda = [](int a, int b, int c) { return a + b + c; };
    auto print_lambda = [](const std::string& msg) { std::cout << "消息: " << msg << "\n"; };
    
    invoke_perfect(add_lambda, 10, 20, 30);
    invoke_perfect(print_lambda, std::string("完美转发测试"));
    
    // 参数包的分组处理
    template<std::size_t N, typename... Args>
    void process_in_groups(Args&&... args) {
        constexpr std::size_t total_args = sizeof...(args);
        constexpr std::size_t full_groups = total_args / N;
        constexpr std::size_t remaining = total_args % N;
        
        std::cout << "参数分组处理: " << total_args << "个参数，每组" << N << "个\n";
        std::cout << "完整组数: " << full_groups << "，剩余: " << remaining << "\n";
        
        auto args_tuple = std::forward_as_tuple(std::forward<Args>(args)...);
        
        // 处理完整的组（这里简化为打印组信息）
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            ([&] {
                std::cout << "处理第" << I + 1 << "组\n";
                // 实际处理逻辑会在这里
            }(), ...);
        }(std::make_index_sequence<full_groups>{});
        
        if constexpr (remaining > 0) {
            std::cout << "处理剩余参数\n";
        }
    }
    
    process_in_groups<3>(1, 2, 3, 4, 5, 6, 7, 8);
    
    // 参数包的类型安全检查
    template<template<typename> class Predicate, typename... Types>
    constexpr bool all_satisfy() {
        return (Predicate<Types>::value && ...);
    }
    
    template<typename T>
    struct is_arithmetic : std::is_arithmetic<T> {};
    
    template<typename T>
    struct is_copyable : std::is_copy_constructible<T> {};
    
    static_assert(all_satisfy<is_arithmetic, int, double, float>());
    static_assert(all_satisfy<is_copyable, int, std::string, std::vector<int>>());
    
    std::cout << "类型安全检查通过\n";
    
    std::cout << "\n";
}

// ===== 6. 变长模板的高级应用和性能分析 =====
void demonstrate_advanced_applications() {
    std::cout << "=== 变长模板的高级应用和性能分析 ===\n";
    
    // 编译期字符串拼接
    template<char... chars>
    struct CompileTimeString {
        static constexpr char data[] = {chars..., '\0'};
        static constexpr std::size_t size() { return sizeof...(chars); }
        
        template<char... other_chars>
        constexpr auto operator+(CompileTimeString<other_chars...>) const {
            return CompileTimeString<chars..., other_chars...>{};
        }
    };
    
    template<char... chars>
    constexpr char CompileTimeString<chars...>::data[];
    
    // 字符串字面量到编译期字符串的转换
    template<typename T, T... chars>
    constexpr auto operator""_cts() {
        return CompileTimeString<chars...>{};
    }
    
    // 变长模板的性能测试框架
    template<typename F, typename... Args>
    auto measure_execution_time(F&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        
        if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
            std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
        } else {
            auto result = std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "执行时间: " << duration.count() << " 微秒\n";
        return duration;
    }
    
    // 变长参数的批量操作
    template<typename Container, typename... Elements>
    void bulk_insert_optimized(Container& container, Elements&&... elements) {
        // 预分配空间优化
        if constexpr (requires { container.reserve(0); }) {
            container.reserve(container.size() + sizeof...(elements));
        }
        
        // 批量插入
        (container.push_back(std::forward<Elements>(elements)), ...);
    }
    
    std::vector<int> test_container;
    auto insert_lambda = [&] {
        bulk_insert_optimized(test_container, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    };
    
    std::cout << "批量插入性能测试:\n";
    measure_execution_time(insert_lambda);
    std::cout << "容器大小: " << test_container.size() << "\n";
    
    // 编译期计算密集型任务
    template<int... values>
    struct CompileTimeCalculations {
        static constexpr int sum = (values + ...);
        static constexpr int product = (values * ...);
        static constexpr int count = sizeof...(values);
        static constexpr double average = static_cast<double>(sum) / count;
        
        static constexpr int max_value() {
            int max_val = std::numeric_limits<int>::min();
            ((max_val = values > max_val ? values : max_val), ...);
            return max_val;
        }
        
        static constexpr int min_value() {
            int min_val = std::numeric_limits<int>::max();
            ((min_val = values < min_val ? values : min_val), ...);
            return min_val;
        }
    };
    
    using Calc = CompileTimeCalculations<3, 1, 4, 1, 5, 9, 2, 6, 5, 3>;
    
    std::cout << "编译期计算结果:\n";
    std::cout << "  求和: " << Calc::sum << "\n";
    std::cout << "  乘积: " << Calc::product << "\n";
    std::cout << "  数量: " << Calc::count << "\n";
    std::cout << "  平均值: " << Calc::average << "\n";
    std::cout << "  最大值: " << Calc::max_value() << "\n";
    std::cout << "  最小值: " << Calc::min_value() << "\n";
    
    // 变长模板的内存使用分析
    template<typename... Types>
    struct MemoryAnalysis {
        static constexpr std::size_t individual_sizes[] = {sizeof(Types)...};
        static constexpr std::size_t total_size = (sizeof(Types) + ...);
        static constexpr std::size_t count = sizeof...(Types);
        static constexpr std::size_t max_alignment = std::max({alignof(Types)...});
        
        static void print_analysis() {
            std::cout << "\n--- 内存使用分析 ---\n";
            std::cout << "类型数量: " << count << "\n";
            std::cout << "总大小: " << total_size << " bytes\n";
            std::cout << "最大对齐: " << max_alignment << " bytes\n";
            
            std::size_t i = 0;
            ((std::cout << "类型" << i++ << "大小: " << sizeof(Types) << " bytes\n"), ...);
        }
    };
    
    MemoryAnalysis<char, int, double, std::string, std::vector<int>>::print_analysis();
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "第5章：变长模板的威力深度解析\n";
    std::cout << "===============================\n";
    
    demonstrate_parameter_pack_expansion();
    demonstrate_recursive_instantiation();
    demonstrate_fold_expressions();
    demonstrate_index_sequence_tricks();
    demonstrate_perfect_forwarding_packs();
    demonstrate_advanced_applications();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall -Wextra 05_variadic_templates.cpp -o variadic_templates
./variadic_templates

关键学习点:
1. 变长模板的参数包展开遵循严格的语法规则
2. 递归实例化是处理参数包的经典技术，需要注意终止条件
3. C++17折叠表达式大大简化了参数包的处理
4. 索引序列是连接编译期索引和运行期操作的桥梁
5. 完美转发在参数包中的应用需要特别注意值类别保持

高级技巧:
- 使用折叠表达式替代复杂的递归模板
- 利用索引序列实现编译期循环
- 通过SFINAE和concepts约束参数包类型
- 合理控制递归深度避免编译器限制
- 使用编译期计算减少运行时开销

性能考虑:
- 参数包展开在编译期完成，运行时零开销
- 递归模板可能导致长编译时间
- 折叠表达式通常比递归模板编译更快
- 大量参数包可能导致代码膨胀

注意事项:
- 参数包只能在特定上下文中展开
- 递归模板需要显式的终止条件
- 编译器对递归深度有限制（通常900-1500层）
- 空参数包的处理需要特别考虑
*/