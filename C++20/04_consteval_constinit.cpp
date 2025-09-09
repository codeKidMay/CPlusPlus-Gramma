/**
 * C++20 编译期增强特性深度解析
 * 
 * 核心概念：
 * 1. consteval立即函数 - 强制编译期执行的函数机制
 * 2. constinit静态初始化 - 保证静态变量编译期初始化
 * 3. 编译期计算优化 - 零运行时开销的计算模式
 * 4. constexpr扩展增强 - 更强大的编译期编程能力
 * 5. 模板元编程革新 - 编译期算法和数据结构设计
 */

#include <iostream>
#include <array>
#include <string_view>
#include <type_traits>
#include <chrono>

// ===== 1. consteval立即函数演示 =====
// constexpr vs consteval对比
constexpr int constexpr_factorial(int n) {
    return n <= 1 ? 1 : n * constexpr_factorial(n - 1);
}

consteval int consteval_factorial(int n) {
    return n <= 1 ? 1 : n * consteval_factorial(n - 1);
}

// consteval强制编译期字符串处理
consteval size_t consteval_string_length(const char* str) {
    const char* end = str;
    while (*end) ++end;
    return end - str;
}

consteval bool is_power_of_two(size_t value) {
    return value > 0 && (value & (value - 1)) == 0;
}

// 编译期配置检查
template<size_t BufferSize>
class SafeBuffer {
    static_assert(is_power_of_two(BufferSize), "Buffer size must be power of two");
    static_assert(BufferSize >= 64, "Buffer size must be at least 64 bytes");
    static_assert(BufferSize <= 65536, "Buffer size must not exceed 64KB");
    
    std::array<char, BufferSize> buffer_;
    
public:
    constexpr size_t size() const noexcept { return BufferSize; }
    constexpr size_t capacity() const noexcept { return BufferSize; }
    
    constexpr char* data() noexcept { return buffer_.data(); }
    constexpr const char* data() const noexcept { return buffer_.data(); }
    
    // 编译期验证的索引访问
    constexpr char& at(size_t index) {
        if (index >= BufferSize) {
            throw std::out_of_range("Index out of range");
        }
        return buffer_[index];
    }
};

void demonstrate_consteval_functions() {
    std::cout << "=== consteval立即函数演示 ===\n";
    
    // constexpr可以在运行时调用
    constexpr int compile_time = 5;
    int runtime = 5;
    
    std::cout << "constexpr函数:\n";
    std::cout << "  编译期调用 factorial(5): " << constexpr_factorial(compile_time) << "\n";
    std::cout << "  运行时调用 factorial(5): " << constexpr_factorial(runtime) << "\n";
    
    // consteval只能在编译期调用
    std::cout << "\nconsteval函数:\n";
    std::cout << "  编译期调用 factorial(5): " << consteval_factorial(5) << "\n";
    // std::cout << "  运行时调用会编译错误: " << consteval_factorial(runtime) << "\n";  // 编译错误
    
    // 编译期字符串处理
    constexpr auto hello_len = consteval_string_length("Hello World");
    std::cout << "  编译期字符串长度: " << hello_len << "\n";
    
    // 编译期配置验证
    SafeBuffer<1024> buffer;  // 通过：1024是2的幂
    std::cout << "  安全缓冲区大小: " << buffer.size() << "\n";
    
    // SafeBuffer<1000> bad_buffer;  // 编译错误：不是2的幂
    // SafeBuffer<32> small_buffer;   // 编译错误：小于64字节
    
    std::cout << "\n";
}

// ===== 2. constinit静态初始化演示 =====
// 传统静态初始化的问题
int expensive_computation(int x) {
    std::cout << "运行时计算: " << x << "\n";
    return x * x + x + 1;
}

// 可能在main之前或之后初始化，存在初始化顺序问题
static int problematic_global = expensive_computation(42);

// constinit保证编译期初始化
constinit static int safe_global_1 = 100;  // 简单常量
constinit static double safe_global_2 = 3.14159;  // 编译期可计算的值

// 与consteval结合使用
consteval int compute_config_value() {
    return 256 * 1024;  // 256KB
}

constinit static int buffer_size = compute_config_value();

// constinit与thread_local
constinit thread_local int thread_counter = 0;

// 复杂的编译期初始化
consteval std::array<int, 10> generate_lookup_table() {
    std::array<int, 10> table{};
    for (size_t i = 0; i < table.size(); ++i) {
        table[i] = static_cast<int>(i * i);
    }
    return table;
}

constinit static auto lookup_table = generate_lookup_table();

class ConfigManager {
public:
    // 编译期初始化的配置常量
    constinit static inline int max_connections = 100;
    constinit static inline double timeout_seconds = 30.0;
    constinit static inline bool debug_mode = false;
    
    // 编译期计算的派生配置
    constinit static inline int connection_pool_size = max_connections / 4;
    constinit static inline int timeout_ms = static_cast<int>(timeout_seconds * 1000);
};

void demonstrate_constinit() {
    std::cout << "=== constinit静态初始化演示 ===\n";
    
    std::cout << "constinit全局变量:\n";
    std::cout << "  safe_global_1: " << safe_global_1 << "\n";
    std::cout << "  safe_global_2: " << safe_global_2 << "\n";
    std::cout << "  buffer_size: " << buffer_size << "\n";
    
    std::cout << "\n编译期生成的查找表:\n";
    std::cout << "  查找表内容: ";
    for (size_t i = 0; i < lookup_table.size(); ++i) {
        std::cout << lookup_table[i] << " ";
    }
    std::cout << "\n";
    
    std::cout << "\n配置管理器:\n";
    std::cout << "  最大连接数: " << ConfigManager::max_connections << "\n";
    std::cout << "  连接池大小: " << ConfigManager::connection_pool_size << "\n";
    std::cout << "  超时时间: " << ConfigManager::timeout_ms << "ms\n";
    std::cout << "  调试模式: " << (ConfigManager::debug_mode ? "开启" : "关闭") << "\n";
    
    // thread_local变量测试
    ++thread_counter;
    std::cout << "\nthread_local计数器: " << thread_counter << "\n";
    
    std::cout << "\n";
}

// ===== 3. 编译期计算优化演示 =====
// 编译期数学函数
consteval double consteval_sin_approx(double x) {
    // 使用泰勒级数近似sin(x)
    double result = x;
    double term = x;
    
    for (int i = 1; i < 10; ++i) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        result += term;
    }
    
    return result;
}

consteval double consteval_sqrt(double x) {
    if (x < 0) throw std::domain_error("sqrt of negative number");
    if (x == 0) return 0;
    
    // 牛顿法求平方根
    double guess = x / 2.0;
    for (int i = 0; i < 20; ++i) {
        double new_guess = 0.5 * (guess + x / guess);
        if (new_guess == guess) break;  // 收敛
        guess = new_guess;
    }
    
    return guess;
}

// 编译期复数运算
struct ComplexNumber {
    double real, imag;
    
    consteval ComplexNumber(double r = 0, double i = 0) : real(r), imag(i) {}
    
    consteval ComplexNumber operator+(const ComplexNumber& other) const {
        return {real + other.real, imag + other.imag};
    }
    
    consteval ComplexNumber operator*(const ComplexNumber& other) const {
        return {
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
        };
    }
    
    consteval double magnitude() const {
        return consteval_sqrt(real * real + imag * imag);
    }
};

// 编译期矩阵运算
template<size_t Rows, size_t Cols>
struct Matrix {
    std::array<std::array<double, Cols>, Rows> data{};
    
    consteval Matrix() = default;
    
    consteval Matrix(std::initializer_list<std::initializer_list<double>> init) {
        size_t row = 0;
        for (auto& row_init : init) {
            size_t col = 0;
            for (auto& val : row_init) {
                if (row < Rows && col < Cols) {
                    data[row][col] = val;
                }
                ++col;
            }
            ++row;
        }
    }
    
    consteval double& operator()(size_t row, size_t col) {
        return data[row][col];
    }
    
    consteval const double& operator()(size_t row, size_t col) const {
        return data[row][col];
    }
    
    template<size_t OtherCols>
    consteval Matrix<Rows, OtherCols> operator*(const Matrix<Cols, OtherCols>& other) const {
        Matrix<Rows, OtherCols> result;
        
        for (size_t i = 0; i < Rows; ++i) {
            for (size_t j = 0; j < OtherCols; ++j) {
                double sum = 0;
                for (size_t k = 0; k < Cols; ++k) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }
        
        return result;
    }
};

void demonstrate_compile_time_optimization() {
    std::cout << "=== 编译期计算优化演示 ===\n";
    
    // 编译期数学计算
    constexpr double pi_4 = 3.14159265359 / 4.0;
    constexpr auto sin_result = consteval_sin_approx(pi_4);
    constexpr auto sqrt_result = consteval_sqrt(16.0);
    
    std::cout << "编译期数学计算:\n";
    std::cout << "  sin(π/4) ≈ " << sin_result << "\n";
    std::cout << "  sqrt(16) = " << sqrt_result << "\n";
    
    // 编译期复数运算
    constexpr ComplexNumber c1{3, 4};
    constexpr ComplexNumber c2{1, 2};
    constexpr auto c_sum = c1 + c2;
    constexpr auto c_product = c1 * c2;
    constexpr auto c1_magnitude = c1.magnitude();
    
    std::cout << "\n编译期复数运算:\n";
    std::cout << "  (3+4i) + (1+2i) = " << c_sum.real << "+" << c_sum.imag << "i\n";
    std::cout << "  (3+4i) * (1+2i) = " << c_product.real << "+" << c_product.imag << "i\n";
    std::cout << "  |3+4i| = " << c1_magnitude << "\n";
    
    // 编译期矩阵运算
    constexpr Matrix<2, 2> m1{{
        {1, 2},
        {3, 4}
    }};
    
    constexpr Matrix<2, 2> m2{{
        {5, 6},
        {7, 8}
    }};
    
    constexpr auto m_product = m1 * m2;
    
    std::cout << "\n编译期矩阵乘法:\n";
    std::cout << "  结果矩阵:\n";
    std::cout << "    [" << m_product(0, 0) << ", " << m_product(0, 1) << "]\n";
    std::cout << "    [" << m_product(1, 0) << ", " << m_product(1, 1) << "]\n";
    
    std::cout << "\n";
}

// ===== 4. constexpr扩展增强演示 =====
// C++20中constexpr的新能力
constexpr std::string_view constexpr_string_processing() {
    constexpr std::string_view text = "Hello World C++20";
    
    // 编译期字符串查找
    constexpr auto pos = text.find("World");
    if (pos != std::string_view::npos) {
        return text.substr(pos);
    }
    
    return text;
}

// 编译期容器操作
constexpr std::array<int, 10> constexpr_array_operations() {
    std::array<int, 10> arr{};
    
    // 编译期填充
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = static_cast<int>(i * i);
    }
    
    // 编译期排序（简单冒泡排序）
    for (size_t i = 0; i < arr.size() - 1; ++i) {
        for (size_t j = 0; j < arr.size() - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                auto temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    
    return arr;
}

// 编译期算法实现
template<typename Iterator, typename Predicate>
constexpr Iterator constexpr_find_if(Iterator first, Iterator last, Predicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return first;
        }
        ++first;
    }
    return last;
}

template<typename Iterator, typename T>
constexpr Iterator constexpr_find(Iterator first, Iterator last, const T& value) {
    return constexpr_find_if(first, last, [&](const auto& element) {
        return element == value;
    });
}

void demonstrate_constexpr_extensions() {
    std::cout << "=== constexpr扩展增强演示 ===\n";
    
    // 编译期字符串处理
    constexpr auto processed_string = constexpr_string_processing();
    std::cout << "编译期字符串处理结果: " << processed_string << "\n";
    
    // 编译期容器操作
    constexpr auto sorted_array = constexpr_array_operations();
    std::cout << "编译期排序结果: ";
    for (const auto& element : sorted_array) {
        std::cout << element << " ";
    }
    std::cout << "\n";
    
    // 编译期算法
    constexpr std::array<int, 5> search_array{10, 20, 30, 40, 50};
    constexpr auto found = constexpr_find(search_array.begin(), search_array.end(), 30);
    constexpr bool found_30 = (found != search_array.end());
    
    std::cout << "编译期查找30: " << (found_30 ? "找到" : "未找到") << "\n";
    
    // 编译期谓词查找
    constexpr auto found_even = constexpr_find_if(search_array.begin(), search_array.end(),
        [](int x) { return x % 2 == 0; });
    constexpr bool has_even = (found_even != search_array.end());
    
    std::cout << "编译期查找偶数: " << (has_even ? "找到" : "未找到") << "\n";
    
    std::cout << "\n";
}

// ===== 5. 模板元编程革新演示 =====
// 编译期类型列表处理
template<typename... Types>
struct TypeList {
    static constexpr size_t size = sizeof...(Types);
};

template<typename List>
struct TypeListSize;

template<typename... Types>
struct TypeListSize<TypeList<Types...>> {
    static constexpr size_t value = sizeof...(Types);
};

// 编译期类型过滤
template<template<typename> class Predicate, typename... Types>
consteval auto filter_types(TypeList<Types...>) {
    return []<typename T>(T) {
        if constexpr (Predicate<T>::value) {
            return TypeList<T>{};
        } else {
            return TypeList<>{};
        }
    };
}

// 编译期数值计算模板
template<size_t N>
struct Fibonacci {
    static constexpr size_t value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr size_t value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr size_t value = 1;
};

// 编译期素数判断
template<size_t N, size_t Divisor = 2>
struct IsPrime {
    static constexpr bool value = (N % Divisor != 0) && IsPrime<N, Divisor + 1>::value;
};

template<size_t N>
struct IsPrime<N, N> {
    static constexpr bool value = true;
};

template<>
struct IsPrime<1, 2> {
    static constexpr bool value = false;
};

template<>
struct IsPrime<2, 2> {
    static constexpr bool value = true;
};

// 编译期素数生成
template<size_t N>
consteval std::array<size_t, N> generate_primes() {
    std::array<size_t, N> primes{};
    size_t count = 0;
    size_t candidate = 2;
    
    while (count < N) {
        bool is_prime = true;
        
        // 检查是否为素数
        for (size_t i = 2; i * i <= candidate; ++i) {
            if (candidate % i == 0) {
                is_prime = false;
                break;
            }
        }
        
        if (is_prime) {
            primes[count++] = candidate;
        }
        ++candidate;
    }
    
    return primes;
}

void demonstrate_metaprogramming_revolution() {
    std::cout << "=== 模板元编程革新演示 ===\n";
    
    // 编译期类型列表
    using TestTypes = TypeList<int, double, std::string, char>;
    constexpr size_t type_count = TypeListSize<TestTypes>::value;
    
    std::cout << "类型列表大小: " << type_count << "\n";
    
    // 编译期斐波那契数列
    constexpr auto fib_10 = Fibonacci<10>::value;
    constexpr auto fib_15 = Fibonacci<15>::value;
    constexpr auto fib_20 = Fibonacci<20>::value;
    
    std::cout << "编译期斐波那契数列:\n";
    std::cout << "  F(10) = " << fib_10 << "\n";
    std::cout << "  F(15) = " << fib_15 << "\n";
    std::cout << "  F(20) = " << fib_20 << "\n";
    
    // 编译期素数判断
    std::cout << "\n编译期素数判断:\n";
    std::cout << "  17是素数: " << IsPrime<17>::value << "\n";
    std::cout << "  25是素数: " << IsPrime<25>::value << "\n";
    std::cout << "  97是素数: " << IsPrime<97>::value << "\n";
    
    // 编译期素数生成
    constexpr auto first_10_primes = generate_primes<10>();
    std::cout << "\n前10个素数: ";
    for (const auto& prime : first_10_primes) {
        std::cout << prime << " ";
    }
    std::cout << "\n";
    
    std::cout << "\n";
}

// 性能对比演示
void demonstrate_performance_comparison() {
    std::cout << "=== 性能对比演示 ===\n";
    
    const int iterations = 1000000;
    
    // 运行时计算
    auto start = std::chrono::high_resolution_clock::now();
    
    volatile int runtime_sum = 0;  // volatile防止编译器优化
    for (int i = 0; i < iterations; ++i) {
        runtime_sum += expensive_computation(i % 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto runtime_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 编译期计算（查表）
    start = std::chrono::high_resolution_clock::now();
    
    volatile int compile_time_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        compile_time_sum += lookup_table[i % 10];  // 使用预计算的查找表
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto compile_time_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "性能对比结果:\n";
    std::cout << "  运行时计算: " << runtime_duration.count() << " 微秒\n";
    std::cout << "  编译期查表: " << compile_time_duration.count() << " 微秒\n";
    std::cout << "  性能提升: " << (runtime_duration.count() / double(compile_time_duration.count())) << "倍\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 编译期增强特性深度解析\n";
    std::cout << "=============================\n";
    
    demonstrate_consteval_functions();
    demonstrate_constinit();
    demonstrate_compile_time_optimization();
    demonstrate_constexpr_extensions();
    demonstrate_metaprogramming_revolution();
    demonstrate_performance_comparison();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 04_consteval_constinit.cpp -o consteval_constinit
./consteval_constinit

关键学习点:
1. consteval强制函数在编译期执行，提供完全的编译期计算保证
2. constinit确保静态变量在编译期初始化，解决初始化顺序问题
3. 编译期计算可以实现零运行时开销的复杂算法
4. C++20的constexpr扩展支持更多标准库功能
5. 新的编译期特性革新了模板元编程的可能性

注意事项:
- consteval函数的参数必须是编译期常量
- constinit只保证初始化时机，不保证值本身是常量
- 过度使用编译期计算可能增加编译时间
- 编译期计算的错误信息可能较难理解
- 某些复杂计算在编译期可能有限制
*/