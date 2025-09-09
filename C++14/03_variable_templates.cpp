/**
 * C++14 变量模板深度解析
 * 
 * 核心概念：
 * 1. 变量模板的语法机制和编译器实例化过程
 * 2. 类型特征简化的设计模式和性能优化
 * 3. 数学常量模板的精度控制和类型安全
 * 4. 模板特化与偏特化在变量模板中的应用
 * 5. 编译期计算和运行时性能的平衡策略
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <complex>
#include <limits>

// ===== 1. C++11类型特征的冗长语法回顾 =====

void demonstrate_cpp11_type_traits_verbosity() {
    std::cout << "=== C++11类型特征冗长语法回顾 ===\n";
    
    // C++11需要::value后缀
    std::cout << "C++11类型检查语法:\n";
    std::cout << "is_integral<int>::value: " << std::is_integral<int>::value << std::endl;
    std::cout << "is_floating_point<double>::value: " << std::is_floating_point<double>::value << std::endl;
    std::cout << "is_pointer<int*>::value: " << std::is_pointer<int*>::value << std::endl;
    std::cout << "is_same<int, int>::value: " << std::is_same<int, int>::value << std::endl;
    
    // 复杂的SFINAE表达式
    template<typename T>
    auto complex_sfinae_cpp11(T t) -> 
        typename std::enable_if<
            std::is_arithmetic<T>::value && 
            !std::is_same<T, bool>::value,
            T
        >::type 
    {
        return t * 2;
    }
    
    auto result = complex_sfinae_cpp11(42);
    std::cout << "复杂SFINAE结果: " << result << std::endl;
    
    std::cout << "\nC++11的问题:\n";
    std::cout << "1. 每次都需要写::value，代码冗长\n";
    std::cout << "2. SFINAE表达式难以阅读和维护\n";
    std::cout << "3. 类型特征嵌套层次深，易出错\n";
    std::cout << "4. 模板元编程代码可读性差\n";
    
    std::cout << "\n";
}

// ===== 2. C++14变量模板基础语法 =====

// 基本变量模板定义
template<typename T>
constexpr bool is_integral_v = std::is_integral<T>::value;

template<typename T>
constexpr bool is_floating_point_v = std::is_floating_point<T>::value;

template<typename T>
constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;

template<typename T, typename U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// 复合类型特征变量模板
template<typename T>
constexpr bool is_numeric_v = is_arithmetic_v<T> && !is_same_v<T, bool>;

template<typename T>
constexpr size_t type_size_v = sizeof(T);

template<typename T>
constexpr size_t type_alignment_v = alignof(T);

void demonstrate_variable_template_basics() {
    std::cout << "=== C++14变量模板基础语法 ===\n";
    
    // 简化的类型检查
    std::cout << "C++14简化语法:\n";
    std::cout << "is_integral_v<int>: " << is_integral_v<int> << std::endl;
    std::cout << "is_floating_point_v<double>: " << is_floating_point_v<double> << std::endl;
    std::cout << "is_same_v<int, int>: " << is_same_v<int, int> << std::endl;
    std::cout << "is_numeric_v<bool>: " << is_numeric_v<bool> << std::endl;
    std::cout << "is_numeric_v<int>: " << is_numeric_v<int> << std::endl;
    
    // 类型信息查询
    std::cout << "\n类型信息查询:\n";
    std::cout << "sizeof(double): " << type_size_v<double> << " bytes\n";
    std::cout << "alignof(double): " << type_alignment_v<double> << " bytes\n";
    std::cout << "sizeof(std::string): " << type_size_v<std::string> << " bytes\n";
    
    // 简化的SFINAE
    auto simple_sfinae = [](auto t) {
        if constexpr (is_numeric_v<decltype(t)>) {
            return t * 2;
        } else {
            return t;
        }
    };
    
    std::cout << "\n简化SFINAE结果:\n";
    std::cout << "numeric: " << simple_sfinae(42) << std::endl;
    std::cout << "non-numeric: " << simple_sfinae(std::string("hello")) << std::endl;
    
    std::cout << "\n变量模板的优势:\n";
    std::cout << "1. 语法简洁，去掉了::value后缀\n";
    std::cout << "2. 提高代码可读性和可维护性\n";
    std::cout << "3. 减少模板元编程的复杂度\n";
    std::cout << "4. 支持constexpr，编译期求值\n";
    
    std::cout << "\n";
}

// ===== 3. 数学常量模板的高精度实现 =====

// 数学常量变量模板
template<typename T>
constexpr T pi_v = T(3.14159265358979323846264338327950288419716939937510L);

template<typename T>
constexpr T e_v = T(2.71828182845904523536028747135266249775724709369995L);

template<typename T>
constexpr T sqrt2_v = T(1.41421356237309504880168872420969807856967187537694L);

template<typename T>
constexpr T golden_ratio_v = T(1.61803398874989484820458683436563811772030917980576L);

// 物理常量模板
template<typename T>
constexpr T light_speed_v = T(299792458.0);  // m/s

template<typename T>
constexpr T planck_constant_v = T(6.62607015e-34);  // J⋅s

template<typename T>
constexpr T avogadro_number_v = T(6.02214076e23);  // mol⁻¹

// 特化：为复数类型提供特殊处理
template<typename T>
constexpr std::complex<T> i_v = std::complex<T>(T(0), T(1));

// 精度验证函数
template<typename T>
void verify_precision() {
    std::cout << "类型 " << typeid(T).name() << " 的精度验证:\n";
    std::cout << "  π = " << std::fixed << pi_v<T> << std::endl;
    std::cout << "  e = " << e_v<T> << std::endl;
    std::cout << "  √2 = " << sqrt2_v<T> << std::endl;
    std::cout << "  φ = " << golden_ratio_v<T> << std::endl;
    std::cout << "  精度位数: " << std::numeric_limits<T>::digits10 << std::endl;
}

void demonstrate_mathematical_constants() {
    std::cout << "=== 数学常量模板的高精度实现 ===\n";
    
    // 不同精度类型的常量
    std::cout << "不同精度类型的数学常量:\n";
    verify_precision<float>();
    std::cout << std::endl;
    verify_precision<double>();
    std::cout << std::endl;
    verify_precision<long double>();
    
    // 数学计算应用
    std::cout << "\n数学计算应用:\n";
    
    auto circle_area = [](auto radius) {
        return pi_v<decltype(radius)> * radius * radius;
    };
    
    auto sphere_volume = [](auto radius) {
        return (4.0L / 3.0L) * pi_v<decltype(radius)> * radius * radius * radius;
    };
    
    double radius = 5.0;
    std::cout << "圆面积 (r=5): " << circle_area(radius) << std::endl;
    std::cout << "球体积 (r=5): " << sphere_volume(radius) << std::endl;
    
    // 复数计算
    std::cout << "\n复数计算:\n";
    auto complex_exp = i_v<double> * pi_v<double>;
    std::cout << "e^(iπ) = " << std::exp(complex_exp) << std::endl;
    std::cout << "欧拉公式验证: e^(iπ) + 1 = " << std::exp(complex_exp) + 1.0 << std::endl;
    
    // 物理计算
    std::cout << "\n物理常量应用:\n";
    double energy_joules = 1e-10;  // 10^-10 焦耳
    auto photon_frequency = energy_joules / planck_constant_v<double>;
    std::cout << "光子频率: " << photon_frequency << " Hz\n";
    
    auto wavelength = light_speed_v<double> / photon_frequency;
    std::cout << "对应波长: " << wavelength << " m\n";
    
    std::cout << "\n";
}

// ===== 4. 模板特化和偏特化 =====

// 基础模板：默认对齐值
template<typename T>
constexpr size_t preferred_alignment_v = alignof(T);

// 特化：为特定类型提供优化的对齐值
template<>
constexpr size_t preferred_alignment_v<char> = 1;

template<>
constexpr size_t preferred_alignment_v<double> = 8;

// 偏特化：指针类型的通用处理
template<typename T>
constexpr size_t preferred_alignment_v<T*> = sizeof(void*);

// 数组类型的特化
template<typename T, size_t N>
constexpr size_t array_size_v = N;

template<typename T>
constexpr size_t array_size_v<T[]> = 0;  // 不定长数组

// 函数类型识别模板
template<typename T>
constexpr bool is_function_v = std::is_function<T>::value;

template<typename R, typename... Args>
constexpr bool is_function_v<R(Args...)> = true;

template<typename R, typename... Args>
constexpr size_t function_arity_v = sizeof...(Args);

// 容器特征检测
template<typename T>
constexpr bool has_value_type_v = false;

template<typename T>
constexpr bool has_value_type_v<std::vector<T>> = true;

template<typename K, typename V>
constexpr bool has_value_type_v<std::pair<K, V>> = true;

void demonstrate_template_specialization() {
    std::cout << "=== 模板特化和偏特化 ===\n";
    
    // 对齐值特化测试
    std::cout << "对齐值特化:\n";
    std::cout << "char对齐: " << preferred_alignment_v<char> << " bytes\n";
    std::cout << "int对齐: " << preferred_alignment_v<int> << " bytes\n";
    std::cout << "double对齐: " << preferred_alignment_v<double> << " bytes\n";
    std::cout << "int*对齐: " << preferred_alignment_v<int*> << " bytes\n";
    std::cout << "double*对齐: " << preferred_alignment_v<double*> << " bytes\n";
    
    // 数组大小检测
    std::cout << "\n数组大小检测:\n";
    int fixed_array[10];
    std::cout << "固定数组大小: " << array_size_v<int[10]> << std::endl;
    
    // 函数类型分析
    std::cout << "\n函数类型分析:\n";
    using func_type = int(double, char);
    std::cout << "函数类型检测: " << is_function_v<func_type> << std::endl;
    std::cout << "函数参数个数: " << function_arity_v<func_type> << std::endl;
    
    // 容器特征检测
    std::cout << "\n容器特征检测:\n";
    std::cout << "vector有value_type: " << has_value_type_v<std::vector<int>> << std::endl;
    std::cout << "int有value_type: " << has_value_type_v<int> << std::endl;
    std::cout << "pair有value_type: " << has_value_type_v<std::pair<int, double>> << std::endl;
    
    std::cout << "\n特化的应用价值:\n";
    std::cout << "1. 为特定类型提供优化的实现\n";
    std::cout << "2. 实现类型特征的精确检测\n";
    std::cout << "3. 支持复杂的模板元编程模式\n";
    std::cout << "4. 提供编译期的类型信息查询\n";
    
    std::cout << "\n";
}

// ===== 5. 编译期计算优化 =====

// 编译期算法变量模板
template<size_t N>
constexpr size_t factorial_v = N * factorial_v<N - 1>;

template<>
constexpr size_t factorial_v<0> = 1;

template<>
constexpr size_t factorial_v<1> = 1;

template<size_t N>
constexpr size_t fibonacci_v = fibonacci_v<N - 1> + fibonacci_v<N - 2>;

template<>
constexpr size_t fibonacci_v<0> = 0;

template<>
constexpr size_t fibonacci_v<1> = 1;

// 质数检测
template<size_t N, size_t D = 2>
constexpr bool is_prime_helper_v = (D * D > N) ? true : 
                                  (N % D == 0) ? false : 
                                  is_prime_helper_v<N, D + 1>;

template<size_t N>
constexpr bool is_prime_v = (N < 2) ? false : is_prime_helper_v<N>;

// 编译期字符串哈希
template<size_t N>
constexpr size_t string_hash_v = 0;

constexpr size_t fnv1a_hash(const char* str, size_t hash = 2166136261u) {
    return *str ? fnv1a_hash(str + 1, (hash ^ *str) * 16777619u) : hash;
}

#define HASH(str) std::integral_constant<size_t, fnv1a_hash(str)>::value

// 编译期数学运算
template<typename T, size_t N>
constexpr T power_v = T(N) * power_v<T, N - 1>;

template<typename T>
constexpr T power_v<T, 0> = T(1);

template<typename T>
constexpr T power_v<T, 1> = T(1);

void demonstrate_compile_time_optimization() {
    std::cout << "=== 编译期计算优化 ===\n";
    
    // 编译期数学计算
    std::cout << "编译期数学计算:\n";
    std::cout << "10! = " << factorial_v<10> << std::endl;
    std::cout << "fibonacci(15) = " << fibonacci_v<15> << std::endl;
    
    // 质数检测
    std::cout << "\n编译期质数检测:\n";
    std::cout << "17是质数: " << is_prime_v<17> << std::endl;
    std::cout << "18是质数: " << is_prime_v<18> << std::endl;
    std::cout << "97是质数: " << is_prime_v<97> << std::endl;
    
    // 编译期字符串哈希
    std::cout << "\n编译期字符串哈希:\n";
    constexpr size_t hash1 = HASH("hello");
    constexpr size_t hash2 = HASH("world");
    constexpr size_t hash3 = HASH("hello");
    
    std::cout << "\"hello\"哈希: " << hash1 << std::endl;
    std::cout << "\"world\"哈希: " << hash2 << std::endl;
    std::cout << "\"hello\"哈希(重复): " << hash3 << std::endl;
    std::cout << "哈希冲突检测: " << (hash1 == hash3 ? "相同" : "不同") << std::endl;
    
    // 编译期幂运算
    std::cout << "\n编译期幂运算:\n";
    std::cout << "2^10 = " << power_v<int, 10> << std::endl;
    std::cout << "1.5^3 = " << power_v<double, 3> << std::endl;
    
    // 性能对比：编译期 vs 运行时
    std::cout << "\n性能对比分析:\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    volatile size_t runtime_result = 1;
    for (size_t i = 1; i <= 10; ++i) {
        runtime_result *= i;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto runtime_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    constexpr size_t compile_time_result = factorial_v<10>;
    
    std::cout << "运行时计算10!: " << runtime_result << " (用时: " << runtime_duration.count() << "ns)\n";
    std::cout << "编译期计算10!: " << compile_time_result << " (用时: 0ns)\n";
    
    std::cout << "\n编译期计算优势:\n";
    std::cout << "1. 零运行时开销，结果直接嵌入代码\n";
    std::cout << "2. 编译期错误检测和验证\n";
    std::cout << "3. 模板实例化时确定所有值\n";
    std::cout << "4. 支持复杂的编译期算法\n";
    
    std::cout << "\n";
}

// ===== 6. 实际应用：配置系统 =====

// 编译期配置系统
template<typename T>
constexpr T default_buffer_size_v = T(4096);

template<>
constexpr size_t default_buffer_size_v<size_t> = 8192;

template<typename T>
constexpr T max_connections_v = T(100);

template<typename T>
constexpr bool enable_logging_v = true;

template<>
constexpr bool enable_logging_v<void> = false;  // 特殊用途

// 平台相关配置
#ifdef _WIN32
    template<typename T>
    constexpr T path_separator_v = T('\\');
#else
    template<typename T>
    constexpr T path_separator_v = T('/');
#endif

// 类型安全的配置访问
template<typename ConfigType, typename ValueType>
struct Config {
    static constexpr ValueType value = ValueType{};
};

// 配置特化
template<>
struct Config<struct BufferSize, size_t> {
    static constexpr size_t value = 16384;
};

template<>
struct Config<struct ThreadCount, int> {
    static constexpr int value = 8;
};

template<>
struct Config<struct EnableDebug, bool> {
    static constexpr bool value = 
#ifdef NDEBUG
        false;
#else
        true;
#endif
};

void demonstrate_configuration_system() {
    std::cout << "=== 实际应用：配置系统 ===\n";
    
    // 基础配置查询
    std::cout << "基础配置:\n";
    std::cout << "默认缓冲区大小: " << default_buffer_size_v<size_t> << " bytes\n";
    std::cout << "最大连接数: " << max_connections_v<int> << std::endl;
    std::cout << "启用日志: " << (enable_logging_v<bool> ? "是" : "否") << std::endl;
    std::cout << "路径分隔符: '" << path_separator_v<char> << "'\n";
    
    // 类型安全配置
    std::cout << "\n类型安全配置:\n";
    std::cout << "缓冲区大小配置: " << Config<struct BufferSize, size_t>::value << std::endl;
    std::cout << "线程数配置: " << Config<struct ThreadCount, int>::value << std::endl;
    std::cout << "调试模式: " << (Config<struct EnableDebug, bool>::value ? "开启" : "关闭") << std::endl;
    
    // 配置的编译期验证
    static_assert(default_buffer_size_v<size_t> > 0, "缓冲区大小必须为正数");
    static_assert(max_connections_v<int> <= 10000, "连接数不能超过10000");
    
    std::cout << "\n配置系统的优势:\n";
    std::cout << "1. 编译期确定所有配置值\n";
    std::cout << "2. 类型安全的配置访问\n";
    std::cout << "3. 零运行时配置查询开销\n";
    std::cout << "4. 编译期配置验证和错误检测\n";
    std::cout << "5. 平台和编译条件相关的自适应配置\n";
    
    std::cout << "\n";
}

// ===== 7. 性能分析和最佳实践 =====

// 性能测试模板
template<size_t Iterations>
class PerformanceTester {
public:
    template<typename Func>
    static auto measure(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < Iterations; ++i) {
            volatile auto result = func();
            (void)result;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }
};

// 编译期 vs 运行时对比
constexpr double runtime_pi() {
    return 3.14159265358979323846;
}

void demonstrate_performance_best_practices() {
    std::cout << "=== 性能分析和最佳实践 ===\n";
    
    constexpr size_t test_iterations = 10000000;
    
    // 常量访问性能对比
    std::cout << "常量访问性能测试 (" << test_iterations << "次):\n";
    
    auto compile_time_test = []() { return pi_v<double>; };
    auto runtime_test = []() { return runtime_pi(); };
    auto literal_test = []() { return 3.14159265358979323846; };
    
    auto compile_time_result = PerformanceTester<test_iterations>::measure(compile_time_test);
    auto runtime_time_result = PerformanceTester<test_iterations>::measure(runtime_test);
    auto literal_time_result = PerformanceTester<test_iterations>::measure(literal_test);
    
    std::cout << "编译期常量: " << compile_time_result.count() << " ns\n";
    std::cout << "运行时函数: " << runtime_time_result.count() << " ns\n";
    std::cout << "字面量: " << literal_time_result.count() << " ns\n";
    
    // 类型特征性能对比
    std::cout << "\n类型特征性能测试:\n";
    
    auto old_trait_test = []() { 
        return std::is_integral<int>::value && std::is_arithmetic<double>::value; 
    };
    
    auto new_trait_test = []() { 
        return is_integral_v<int> && is_arithmetic_v<double>; 
    };
    
    auto old_time = PerformanceTester<test_iterations>::measure(old_trait_test);
    auto new_time = PerformanceTester<test_iterations>::measure(new_trait_test);
    
    std::cout << "传统类型特征: " << old_time.count() << " ns\n";
    std::cout << "变量模板: " << new_time.count() << " ns\n";
    
    std::cout << "\n编译时间影响分析:\n";
    std::cout << "• 变量模板减少了模板实例化复杂度\n";
    std::cout << "• 编译期计算可能增加编译时间但提升运行时性能\n";
    std::cout << "• 合理的特化可以减少不必要的模板展开\n";
    
    std::cout << "\n最佳实践总结:\n";
    std::cout << "✓ 为常用类型特征定义_v变量模板\n";
    std::cout << "✓ 使用constexpr确保编译期求值\n";
    std::cout << "✓ 合理使用特化避免代码重复\n";
    std::cout << "✓ 在配置系统中应用变量模板\n";
    std::cout << "✓ 注意编译时间和运行时性能的平衡\n";
    
    std::cout << "\n注意事项:\n";
    std::cout << "⚠ 过度的编译期计算可能增加编译时间\n";
    std::cout << "⚠ 复杂的递归模板可能导致编译器栈溢出\n";
    std::cout << "⚠ 模板特化的优先级规则需要仔细考虑\n";
    std::cout << "⚠ 跨平台兼容性需要特别注意\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++14 变量模板深度解析\n";
    std::cout << "=======================\n";
    
    // C++11冗长语法回顾
    demonstrate_cpp11_type_traits_verbosity();
    
    // 变量模板基础
    demonstrate_variable_template_basics();
    
    // 数学常量应用
    demonstrate_mathematical_constants();
    
    // 模板特化
    demonstrate_template_specialization();
    
    // 编译期计算
    demonstrate_compile_time_optimization();
    
    // 配置系统应用
    demonstrate_configuration_system();
    
    // 性能和最佳实践
    demonstrate_performance_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall 03_variable_templates.cpp -o variable_templates_demo
./variable_templates_demo

关键学习点:
1. 理解变量模板如何简化C++11类型特征的冗长语法
2. 掌握数学常量模板的精度控制和类型安全设计
3. 学会使用模板特化和偏特化提供定制化实现
4. 了解编译期计算在变量模板中的优化效果
5. 掌握变量模板在实际配置系统中的应用模式
6. 理解变量模板对编译时间和运行时性能的影响
7. 学会在模板元编程中合理应用变量模板
8. 避免过度复杂的编译期计算导致的编译问题

注意事项:
- 变量模板提供了更简洁的语法但本质上仍是模板实例化
- 编译期计算虽然高效但可能增加编译时间
- 合理使用特化可以为特定类型提供优化实现
- 在配置系统中应用变量模板可以实现零开销的配置访问
*/