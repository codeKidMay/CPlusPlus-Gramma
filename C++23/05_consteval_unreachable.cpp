/**
 * C++23 if consteval和std::unreachable编译期优化深度解析
 * 
 * 核心概念：
 * 1. 编译期执行检测 - if consteval区分编译期和运行时路径
 * 2. 编译器优化提示 - std::unreachable标记不可达代码
 * 3. 性能优化 - 消除死代码和分支优化
 * 4. 元编程增强 - 编译期和运行时的统一接口
 * 5. 安全性保证 - 编译期检查和运行时验证
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <utility>
#include <limits>
#include <cmath>
#include <complex>
#include <source_location>
#include <type_traits>
#include <concepts>
#include <cassert>

// ===== 1. if consteval基础概念 =====
void demonstrate_if_consteval_basics() {
    std::cout << "=== if consteval基础概念 ===\n";
    
    // 基本的if consteval使用
    constexpr auto flexible_function = [](int x) {
        if consteval {
            // 编译期执行的代码路径
            std::cout << "编译期执行: x = " << x << "\n";
            return x * x;  // 简单计算
        } else {
            // 运行时执行的代码路径
            std::cout << "运行时执行: x = " << x << "\n";
            return expensive_runtime_calculation(x);  // 复杂计算
        }
    };
    
    std::cout << "编译期调用:\n";
    constexpr int compile_time_result = flexible_function(5);
    std::cout << "编译期结果: " << compile_time_result << "\n\n";
    
    std::cout << "运行时调用:\n";
    int runtime_value = 10;
    int runtime_result = flexible_function(runtime_value);
    std::cout << "运行时结果: " << runtime_result << "\n\n";
    
    // 类型检查示例
    auto type_checker = []<typename T>(T value) {
        if consteval {
            // 编译期：更严格的类型检查
            static_assert(std::is_arithmetic_v<T>, "必须是算术类型");
            return value;
        } else {
            // 运行时：动态类型检查
            if constexpr (!std::is_arithmetic_v<T>) {
                throw std::runtime_error("运行时类型错误");
            }
            return value;
        }
    };
    
    std::cout << "类型检查示例:\n";
    constexpr int valid_compile = type_checker(42);
    std::cout << "编译期类型检查通过: " << valid_compile << "\n";
    
    try {
        int valid_runtime = type_checker(3.14);
        std::cout << "运行时类型检查通过: " << valid_runtime << "\n";
    } catch (const std::exception& e) {
        std::cout << "类型检查失败: " << e.what() << "\n";
    }
    
    std::cout << "\n";
}

// ===== 2. 编译期优化和性能增强 =====
int expensive_runtime_calculation(int x) {
    // 模拟昂贵的运行时计算
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return x * x + 2 * x + 1;
}

void demonstrate_compilation_optimization() {
    std::cout << "=== 编译期优化和性能增强 ===\n";
    
    // 优化的数学函数
    template<typename T>
    constexpr auto optimized_sqrt(T x) {
        if consteval {
            // 编译期：使用精确但可能较慢的算法
            if (x < 0) {
                throw std::runtime_error("负数无法开方");
            }
            // 使用牛顿迭代法计算平方根
            T guess = x / 2;
            for (int i = 0; i < 20; ++i) {
                guess = (guess + x / guess) / 2;
            }
            return guess;
        } else {
            // 运行时：使用硬件优化的函数
            return std::sqrt(static_cast<double>(x));
        }
    }
    
    // 编译期计算
    constexpr double compile_time_sqrt = optimized_sqrt(16.0);
    std::cout << "编译期平方根: sqrt(16) = " << compile_time_sqrt << "\n";
    
    // 运行时计算
    auto start = std::chrono::high_resolution_clock::now();
    double runtime_sqrt = optimized_sqrt(25.0);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "运行时平方根: sqrt(25) = " << runtime_sqrt << "\n";
    std::cout << "计算耗时: " << duration.count() << "μs\n\n";
    
    // 字符串处理优化
    auto string_processor = [](const std::string& str) {
        if consteval {
            // 编译期：简单的字符串处理
            return str.length();
        } else {
            // 运行时：复杂的字符串分析
            size_t vowels = 0;
            for (char c : str) {
                if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
                    vowels++;
                }
            }
            return vowels;
        }
    };
    
    std::cout << "字符串处理示例:\n";
    // 编译期字符串处理
    constexpr size_t compile_len = string_processor("hello");
    std::cout << "编译期字符串长度: " << compile_len << "\n";
    
    // 运行时字符串处理
    std::string runtime_str = "beautiful world";
    size_t vowel_count = string_processor(runtime_str);
    std::cout << "运行时元音计数: " << vowel_count << "\n\n";
}

// ===== 3. std::unreachable优化提示 =====
void demonstrate_std_unreachable() {
    std::cout << "=== std::unreachable优化提示 ===\n";
    
    // 优化的switch语句
    auto optimized_switch = [](int value) -> int {
        switch (value) {
            case 1: return 10;
            case 2: return 20;
            case 3: return 30;
            case 4: return 40;
            default:
                // 告诉编译器这里永远不会执行
                std::unreachable();
        }
        // 编译器可以优化掉后续的死代码
    };
    
    std::cout << "优化的switch语句:\n";
    for (int i = 1; i <= 4; ++i) {
        std::cout << "输入 " << i << " -> 输出 " << optimized_switch(i) << "\n";
    }
    
    // 编译期优化的函数
    template<int N>
    constexpr int factorial() {
        if constexpr (N == 0 || N == 1) {
            return 1;
        } else if constexpr (N > 1 && N <= 20) {
            return N * factorial<N-1>();
        } else {
            // 编译期就能确定这种情况不会发生
            std::unreachable();
        }
    }
    
    std::cout << "\n编译期阶乘计算:\n";
    std::cout << "5! = " << factorial<5>() << "\n";
    std::cout << "10! = " << factorial<10>() << "\n";
    
    // 类型安全转换
    auto safe_cast = []<typename T>(auto value) -> T {
        if consteval {
            // 编译期：直接转换，编译器会检查安全性
            return static_cast<T>(value);
        } else {
            // 运行时：添加额外检查
            if (value < std::numeric_limits<T>::min() || 
                value > std::numeric_limits<T>::max()) {
                std::unreachable(); // 理论上不会到达这里
            }
            return static_cast<T>(value);
        }
    };
    
    std::cout << "\n类型安全转换:\n";
    constexpr int8_t compile_cast = safe_cast<int8_t>(42);
    std::cout << "编译期转换: " << static_cast<int>(compile_cast) << "\n";
    
    int8_t runtime_cast = safe_cast<int8_t>(100);
    std::cout << "运行时转换: " << static_cast<int>(runtime_cast) << "\n\n";
}

// ===== 4. 高级编译期编程模式 =====
void demonstrate_advanced_compile_time_patterns() {
    std::cout << "=== 高级编译期编程模式 ===\n";
    
    // 编译期和运行时的统一接口
    class UniversalProcessor {
    public:
        template<typename T>
        static auto process(T data) {
            if consteval {
                return compile_time_process(data);
            } else {
                return runtime_process(data);
            }
        }
        
    private:
        template<typename T>
        static constexpr auto compile_time_process(T data) {
            if constexpr (std::is_integral_v<T>) {
                return data * 2;
            } else if constexpr (std::is_floating_point_v<T>) {
                return data * 1.5;
            } else {
                return data; // 默认处理
            }
        }
        
        template<typename T>
        static auto runtime_process(T data) {
            // 运行时可以进行更复杂的处理
            if constexpr (std::is_integral_v<T>) {
                return data * 2 + data % 10;
            } else if constexpr (std::is_floating_point_v<T>) {
                return std::round(data * 1.5 * 100) / 100;
            } else {
                return data;
            }
        }
    };
    
    std::cout << "通用处理器示例:\n";
    constexpr int compile_int = UniversalProcessor::process(21);
    constexpr double compile_double = UniversalProcessor::process(3.14);
    
    std::cout << "编译期整数处理: " << compile_int << "\n";
    std::cout << "编译期浮点处理: " << compile_double << "\n";
    
    int runtime_int = UniversalProcessor::process(17);
    double runtime_double = UniversalProcessor::process(2.718);
    
    std::cout << "运行时整数处理: " << runtime_int << "\n";
    std::cout << "运行时浮点处理: " << runtime_double << "\n\n";
    
    // 编译期缓存优化
    template<typename F>
    class CompileTimeCache {
    public:
        template<typename... Args>
        static constexpr auto get(Args&&... args) {
            if consteval {
                // 编译期：计算并缓存结果
                return F{}(std::forward<Args>(args)...);
            } else {
                // 运行时：使用缓存或重新计算
                return runtime_get(std::forward<Args>(args)...);
            }
        }
        
    private:
        template<typename... Args>
        static auto runtime_get(Args&&... args) {
            // 简化的运行时缓存逻辑
            return F{}(std::forward<Args>(args)...);
        }
    };
    
    struct Fibonacci {
        constexpr auto operator()(int n) const {
            if (n <= 1) return n;
            int a = 0, b = 1;
            for (int i = 2; i <= n; ++i) {
                int temp = a + b;
                a = b;
                b = temp;
            }
            return b;
        }
    };
    
    std::cout << "编译期缓存示例:\n";
    constexpr int fib10 = CompileTimeCache<Fibonacci>::get(10);
    std::cout << "Fibonacci(10) = " << fib10 << "\n";
    
    int fib15 = CompileTimeCache<Fibonacci>::get(15);
    std::cout << "Fibonacci(15) = " << fib15 << "\n\n";
}

// ===== 5. 错误处理和调试支持 =====
void demonstrate_error_handling_and_debugging() {
    std::cout << "=== 错误处理和调试支持 ===\n";
    
    // 增强的错误处理
    auto safe_division = [](int a, int b) {
        if consteval {
            // 编译期：静态检查
            if (b == 0) {
                throw std::runtime_error("编译期除零错误");
            }
            return a / b;
        } else {
            // 运行时：动态检查
            if (b == 0) {
                return std::unexpected("运行时除零错误");
            }
            return a / b;
        }
    };
    
    std::cout << "安全除法示例:\n";
    try {
        constexpr int compile_div = safe_division(10, 2);
        std::cout << "编译期除法: " << compile_div << "\n";
    } catch (const std::exception& e) {
        std::cout << "编译期错误: " << e.what() << "\n";
    }
    
    auto runtime_div = safe_division(15, 3);
    if (runtime_div) {
        std::cout << "运行时除法: " << *runtime_div << "\n";
    } else {
        std::cout << "运行时错误: " << runtime_div.error() << "\n";
    }
    
    // 调试信息输出
    auto debug_function = [](int x, std::source_location loc = std::source_location::current()) {
        if consteval {
            std::cout << "[编译期] " << loc.function_name() << " at " 
                      << loc.file_name() << ":" << loc.line() << "\n";
            return x * x;
        } else {
            std::cout << "[运行时] " << loc.function_name() << " at " 
                      << loc.file_name() << ":" << loc.line() << "\n";
            return x * x + 1; // 运行时版本略有不同
        }
    };
    
    std::cout << "\n调试信息示例:\n";
    constexpr int debug_compile = debug_function(5);
    std::cout << "编译期结果: " << debug_compile << "\n";
    
    int debug_runtime = debug_function(7);
    std::cout << "运行时结果: " << debug_runtime << "\n\n";
}

// ===== 6. 实际应用场景 =====
class ApplicationExamples {
public:
    // 矩阵运算优化
    template<size_t N>
    class Matrix {
        std::array<std::array<double, N>, N> data_;
        
    public:
        constexpr Matrix() : data_{} {
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    data_[i][j] = (i == j) ? 1.0 : 0.0; // 单位矩阵
                }
            }
        }
        
        constexpr double determinant() const {
            if consteval {
                // 编译期：使用递归算法
                return compute_determinant(data_);
            } else {
                // 运行时：使用优化的LU分解
                return runtime_determinant();
            }
        }
        
    private:
        static constexpr double compute_determinant(const std::array<std::array<double, N>, N>& matrix) {
            if constexpr (N == 1) {
                return matrix[0][0];
            } else if constexpr (N == 2) {
                return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
            } else {
                // 简化的行列式计算
                double det = 0;
                for (size_t j = 0; j < N; ++j) {
                    det += matrix[0][j] * cofactor(matrix, 0, j);
                }
                return det;
            }
        }
        
        static constexpr double cofactor(const std::array<std::array<double, N>, N>& matrix, 
                                        size_t row, size_t col) {
            // 简化的余子式计算
            return (row + col) % 2 == 0 ? 1.0 : -1.0;
        }
        
        double runtime_determinant() const {
            // 简化的运行时实现
            double det = 1.0;
            for (size_t i = 0; i < N; ++i) {
                det *= data_[i][i]; // 对角矩阵
            }
            return det;
        }
    };
    
    // 编译期验证器
    template<typename T>
    class Validator {
    public:
        static constexpr bool validate(T value) {
            if consteval {
                // 编译期：严格验证
                return compile_time_validate(value);
            } else {
                // 运行时：宽松验证
                return runtime_validate(value);
            }
        }
        
    private:
        static constexpr bool compile_time_validate(T value) {
            if constexpr (std::is_integral_v<T>) {
                return value >= 0 && value <= 100;
            } else if constexpr (std::is_floating_point_v<T>) {
                return value >= 0.0 && value <= 1.0;
            }
            return true;
        }
        
        static bool runtime_validate(T value) {
            if constexpr (std::is_integral_v<T>) {
                return value >= -10 && value <= 200;
            } else if constexpr (std::is_floating_point_v<T>) {
                return value >= -0.1 && value <= 1.1;
            }
            return true;
        }
    };
    
    static void demonstrate_applications() {
        std::cout << "=== 实际应用场景 ===\n";
        
        // 矩阵运算示例
        constexpr Matrix<3> mat3;
        constexpr double det3 = mat3.determinant();
        std::cout << "3x3矩阵行列式(编译期): " << det3 << "\n";
        
        Matrix<4> mat4;
        double det4 = mat4.determinant();
        std::cout << "4x4矩阵行列式(运行时): " << det4 << "\n\n";
        
        // 验证器示例
        std::cout << "验证器示例:\n";
        constexpr int valid_compile = 50;
        constexpr bool compile_valid = Validator<int>::validate(valid_compile);
        std::cout << "编译期验证(50): " << (compile_valid ? "通过" : "失败") << "\n";
        
        int runtime_value = 150;
        bool runtime_valid = Validator<int>::validate(runtime_value);
        std::cout << "运行时验证(150): " << (runtime_valid ? "通过" : "失败") << "\n";
        
        double double_value = 0.5;
        bool double_valid = Validator<double>::validate(double_value);
        std::cout << "运行时验证(0.5): " << (double_valid ? "通过" : "失败") << "\n\n";
    }
};

// ===== 主函数 =====
int main() {
    std::cout << "C++23 if consteval和std::unreachable编译期优化深度解析\n";
    std::cout << "===================================================\n";
    
    demonstrate_if_consteval_basics();
    demonstrate_compilation_optimization();
    demonstrate_std_unreachable();
    demonstrate_advanced_compile_time_patterns();
    demonstrate_error_handling_and_debugging();
    ApplicationExamples::demonstrate_applications();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall 05_consteval_unreachable.cpp -o consteval_demo
./consteval_demo

注意：if consteval和std::unreachable需要C++23支持：
- GCC 12+: 完整支持
- Clang 15+: 完整支持
- MSVC 19.32+: 完整支持

关键学习点:
1. if consteval可以区分编译期和运行时执行路径
2. std::unreachable帮助编译器优化，标记不可达代码
3. 这两个特性结合可以实现更高效的编译期优化
4. 统一了编译期和运行时的编程模型
5. 提供了更好的错误处理和调试支持

注意事项:
- std::unreachable必须确保代码确实不可达，否则会导致未定义行为
- if consteval主要用于模板和constexpr函数中
- 过度使用可能增加编译时间
- 在性能关键路径上使用可以获得显著优化效果
*/