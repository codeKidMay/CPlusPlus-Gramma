/**
 * C++23 std::expected函数式错误处理深度解析
 * 
 * 核心概念：
 * 1. 函数式错误处理 - 替代异常的现代错误处理范式
 * 2. 错误传播链 - and_then、or_else、transform的组合式处理
 * 3. 性能优化 - 零开销的错误处理机制
 * 4. 类型安全 - 编译期错误类型检查和约束
 * 5. 组合式设计 - 可串联的错误处理操作符
 */

#include <iostream>
#include <expected>
#include <string>
#include <vector>
#include <fstream>
#include <system_error>
#include <chrono>
#include <format>
#include <source_location>
#include <variant>
#include <functional>
#include <numeric>

using namespace std::string_literals;

// ===== 1. expected基础概念和语法 =====
void demonstrate_expected_basics() {
    std::cout << "=== std::expected基础概念和语法 ===\n";
    
    // 传统错误处理方式的问题演示
    std::cout << "传统错误处理方式对比:\n";
    
    // 方式1: 返回码 + 引用参数
    auto divide_traditional = [](int a, int b, int& result) -> int {
        if (b == 0) return -1;  // 错误码含义不清晰
        result = a / b;
        return 0;  // 成功码
    };
    
    int result;
    if (divide_traditional(10, 2, result) == 0) {
        std::cout << "传统方式结果: " << result << "\n";
    }
    
    // 方式2: std::optional（丢失错误信息）
    auto divide_optional = [](int a, int b) -> std::optional<int> {
        if (b == 0) return std::nullopt;  // 无法知道具体错误
        return a / b;
    };
    
    if (auto opt_result = divide_optional(10, 2)) {
        std::cout << "optional方式结果: " << *opt_result << "\n";
    }
    
    // 方式3: C++23 std::expected（最佳方案）
    auto divide_expected = [](int a, int b) -> std::expected<int, std::string> {
        if (b == 0) return std::unexpected("除零错误");
        return a / b;
    };
    
    if (auto exp_result = divide_expected(10, 2)) {
        std::cout << "expected方式结果: " << *exp_result << "\n";
    } else {
        std::cout << "expected错误: " << exp_result.error() << "\n";
    }
    
    // expected的状态检查
    auto bad_result = divide_expected(10, 0);
    std::cout << "\n状态检查:\n";
    std::cout << "has_value(): " << bad_result.has_value() << "\n";
    std::cout << "错误信息: " << bad_result.error() << "\n";
    
    std::cout << "\n";
}

// ===== 2. 结构化错误类型设计 =====
enum class MathError {
    DivisionByZero,
    NegativeSquareRoot,
    Overflow,
    InvalidInput
};

struct DetailedError {
    MathError type;
    std::string message;
    std::source_location location;
    
    DetailedError(MathError t, std::string msg, 
                  std::source_location loc = std::source_location::current())
        : type(t), message(std::move(msg)), location(loc) {}
    
    std::string to_string() const {
        return std::format("错误类型: {}, 消息: {}, 位置: {}:{}",
                          static_cast<int>(type), message,
                          location.file_name(), location.line());
    }
};

void demonstrate_structured_errors() {
    std::cout << "=== 结构化错误类型设计 ===\n";
    
    // 使用枚举类型的错误
    auto safe_sqrt = [](double x) -> std::expected<double, MathError> {
        if (x < 0) return std::unexpected(MathError::NegativeSquareRoot);
        return std::sqrt(x);
    };
    
    auto safe_divide = [](double a, double b) -> std::expected<double, MathError> {
        if (b == 0.0) return std::unexpected(MathError::DivisionByZero);
        if (std::abs(a) > 1e100 || std::abs(b) < 1e-100) {
            return std::unexpected(MathError::Overflow);
        }
        return a / b;
    };
    
    // 测试不同的错误情况
    auto test_cases = std::vector<std::pair<double, double>>{
        {16.0, 4.0},   // 正常情况
        {-9.0, 3.0},   // 负数开方
        {10.0, 0.0},   // 除零
        {1e200, 1e-200} // 溢出
    };
    
    for (const auto& [a, b] : test_cases) {
        auto div_result = safe_divide(a, b);
        if (div_result) {
            auto sqrt_result = safe_sqrt(*div_result);
            if (sqrt_result) {
                std::cout << std::format("sqrt({}/{}) = {:.6f}\n", a, b, *sqrt_result);
            } else {
                std::cout << std::format("开方错误: {}\n", static_cast<int>(sqrt_result.error()));
            }
        } else {
            std::cout << std::format("除法错误: {}\n", static_cast<int>(div_result.error()));
        }
    }
    
    // 使用详细错误结构
    auto detailed_operation = [](double x) -> std::expected<double, DetailedError> {
        if (x < 0) {
            return std::unexpected(DetailedError{
                MathError::NegativeSquareRoot, 
                "输入值必须非负"
            });
        }
        return std::sqrt(x) * 2.0;
    };
    
    std::cout << "\n详细错误信息:\n";
    auto detailed_result = detailed_operation(-5.0);
    if (!detailed_result) {
        std::cout << detailed_result.error().to_string() << "\n";
    }
    
    std::cout << "\n";
}

// ===== 3. 组合式错误处理（Monadic Operations） =====
void demonstrate_monadic_operations() {
    std::cout << "=== 组合式错误处理 ===\n";
    
    // 链式操作的构建块
    auto parse_int = [](const std::string& s) -> std::expected<int, std::string> {
        try {
            return std::stoi(s);
        } catch (const std::exception& e) {
            return std::unexpected("解析整数失败: "s + s);
        }
    };
    
    auto validate_positive = [](int x) -> std::expected<int, std::string> {
        if (x <= 0) return std::unexpected("数值必须为正数");
        return x;
    };
    
    auto calculate_factorial = [](int n) -> std::expected<long long, std::string> {
        if (n > 20) return std::unexpected("阶乘输入过大");
        
        long long result = 1;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    };
    
    // 使用and_then进行链式操作
    auto process_input = [&](const std::string& input) {
        return parse_int(input)
            .and_then(validate_positive)
            .and_then(calculate_factorial);
    };
    
    std::cout << "链式操作示例:\n";
    auto test_inputs = std::vector<std::string>{"5", "-3", "abc", "25", "10"};
    
    for (const auto& input : test_inputs) {
        auto result = process_input(input);
        if (result) {
            std::cout << std::format("输入 '{}' 的阶乘: {}\n", input, *result);
        } else {
            std::cout << std::format("输入 '{}' 处理失败: {}\n", input, result.error());
        }
    }
    
    // transform操作（值变换）
    std::cout << "\ntransform操作示例:\n";
    auto double_if_valid = parse_int("42")
        .transform([](int x) { return x * 2; });
    
    if (double_if_valid) {
        std::cout << "变换后的值: " << *double_if_valid << "\n";
    }
    
    // or_else操作（错误恢复）
    std::cout << "\nor_else错误恢复示例:\n";
    auto with_fallback = parse_int("invalid")
        .or_else([](const std::string& error) -> std::expected<int, std::string> {
            std::cout << "解析失败，使用默认值: " << error << "\n";
            return 42;  // 默认值
        });
    
    if (with_fallback) {
        std::cout << "最终值: " << *with_fallback << "\n";
    }
    
    std::cout << "\n";
}

// ===== 4. 复杂业务场景的错误处理 =====
class ConfigLoader {
public:
    struct Config {
        std::string host;
        int port;
        std::string database;
        bool ssl_enabled;
        int timeout_ms;
    };
    
    enum class ConfigError {
        FileNotFound,
        ParseError,
        ValidationError,
        NetworkError
    };
    
    static std::expected<std::string, ConfigError> read_config_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::unexpected(ConfigError::FileNotFound);
        }
        
        // 模拟文件内容
        return std::string{"host=localhost\nport=5432\ndb=myapp\nssl=true\ntimeout=5000"};
    }
    
    static std::expected<Config, ConfigError> parse_config(const std::string& content) {
        // 简化的解析逻辑
        if (content.empty()) {
            return std::unexpected(ConfigError::ParseError);
        }
        
        Config config{
            .host = "localhost",
            .port = 5432,
            .database = "myapp",
            .ssl_enabled = true,
            .timeout_ms = 5000
        };
        
        return config;
    }
    
    static std::expected<Config, ConfigError> validate_config(const Config& config) {
        if (config.port <= 0 || config.port > 65535) {
            return std::unexpected(ConfigError::ValidationError);
        }
        if (config.timeout_ms <= 0) {
            return std::unexpected(ConfigError::ValidationError);
        }
        return config;
    }
    
    static std::expected<Config, ConfigError> test_connection(const Config& config) {
        // 模拟网络连接测试
        if (config.host == "unreachable.example.com") {
            return std::unexpected(ConfigError::NetworkError);
        }
        return config;
    }
};

void demonstrate_complex_error_handling() {
    std::cout << "=== 复杂业务场景的错误处理 ===\n";
    
    auto load_and_validate_config = [](const std::string& config_path) {
        return ConfigLoader::read_config_file(config_path)
            .and_then(ConfigLoader::parse_config)
            .and_then(ConfigLoader::validate_config)
            .and_then(ConfigLoader::test_connection);
    };
    
    // 测试不同的配置加载场景
    auto test_scenarios = std::vector<std::string>{
        "config.json",           // 正常情况（模拟）
        "missing.json",          // 文件不存在
        "invalid.json"           // 文件存在但内容无效（模拟）
    };
    
    for (const auto& config_path : test_scenarios) {
        std::cout << std::format("\n加载配置文件: {}\n", config_path);
        
        auto config_result = load_and_validate_config(config_path);
        if (config_result) {
            const auto& config = *config_result;
            std::cout << std::format("配置加载成功:\n");
            std::cout << std::format("  主机: {}\n", config.host);
            std::cout << std::format("  端口: {}\n", config.port);
            std::cout << std::format("  数据库: {}\n", config.database);
            std::cout << std::format("  SSL: {}\n", config.ssl_enabled ? "启用" : "禁用");
            std::cout << std::format("  超时: {}ms\n", config.timeout_ms);
        } else {
            auto error_msg = [](ConfigLoader::ConfigError error) {
                switch (error) {
                    case ConfigLoader::ConfigError::FileNotFound:
                        return "配置文件未找到";
                    case ConfigLoader::ConfigError::ParseError:
                        return "配置文件解析错误";
                    case ConfigLoader::ConfigError::ValidationError:
                        return "配置验证失败";
                    case ConfigLoader::ConfigError::NetworkError:
                        return "网络连接测试失败";
                }
                return "未知错误";
            };
            
            std::cout << std::format("配置加载失败: {}\n", error_msg(config_result.error()));
        }
    }
    
    std::cout << "\n";
}

// ===== 5. 性能对比：expected vs 异常 =====
class PerformanceTest {
private:
    static std::expected<double, std::string> divide_expected(double a, double b) {
        if (b == 0.0) return std::unexpected("Division by zero");
        return a / b;
    }
    
    static double divide_exception(double a, double b) {
        if (b == 0.0) throw std::runtime_error("Division by zero");
        return a / b;
    }
    
public:
    static void run_performance_comparison() {
        std::cout << "=== 性能对比：expected vs 异常 ===\n";
        
        const int iterations = 1000000;
        std::vector<std::pair<double, double>> test_data;
        test_data.reserve(iterations);
        
        // 生成测试数据（90%正常，10%错误）
        for (int i = 0; i < iterations; ++i) {
            if (i % 10 == 0) {
                test_data.emplace_back(static_cast<double>(i), 0.0);  // 除零错误
            } else {
                test_data.emplace_back(static_cast<double>(i), static_cast<double>(i % 100 + 1));
            }
        }
        
        // 测试expected性能
        auto start = std::chrono::high_resolution_clock::now();
        int expected_success = 0, expected_errors = 0;
        
        for (const auto& [a, b] : test_data) {
            auto result = divide_expected(a, b);
            if (result) {
                expected_success++;
            } else {
                expected_errors++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto expected_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // 测试异常性能
        start = std::chrono::high_resolution_clock::now();
        int exception_success = 0, exception_errors = 0;
        
        for (const auto& [a, b] : test_data) {
            try {
                auto result = divide_exception(a, b);
                exception_success++;
            } catch (const std::exception&) {
                exception_errors++;
            }
        }
        
        end = std::chrono::high_resolution_clock::now();
        auto exception_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // 输出结果
        std::cout << std::format("测试迭代次数: {}\n", iterations);
        std::cout << std::format("错误率: {:.1f}%\n", (expected_errors * 100.0) / iterations);
        std::cout << "\n性能结果:\n";
        std::cout << std::format("expected方式: {}μs (成功: {}, 错误: {})\n", 
                                expected_duration.count(), expected_success, expected_errors);
        std::cout << std::format("异常方式: {}μs (成功: {}, 错误: {})\n", 
                                exception_duration.count(), exception_success, exception_errors);
        
        double speedup = static_cast<double>(exception_duration.count()) / expected_duration.count();
        std::cout << std::format("性能提升: {:.2f}倍\n", speedup);
        
        std::cout << "\n性能分析:\n";
        std::cout << "- expected在错误频繁的场景下性能优势明显\n";
        std::cout << "- 异常处理的栈展开开销很大\n";
        std::cout << "- expected更适合可预期的错误情况\n";
        std::cout << "- 编译器对expected的优化更好\n";
        
        std::cout << "\n";
    }
};

// ===== 6. expected的高级用法和设计模式 =====
template<typename T, typename E>
class ResultBuilder {
private:
    std::expected<T, E> result_;
    
public:
    explicit ResultBuilder(std::expected<T, E> initial) : result_(std::move(initial)) {}
    
    template<typename F>
    ResultBuilder& map(F&& func) {
        if (result_) {
            result_ = func(*result_);
        }
        return *this;
    }
    
    template<typename F>
    ResultBuilder& flat_map(F&& func) {
        if (result_) {
            result_ = func(*result_);
        }
        return *this;
    }
    
    template<typename F>
    ResultBuilder& map_error(F&& func) {
        if (!result_) {
            result_ = std::unexpected(func(result_.error()));
        }
        return *this;
    }
    
    template<typename F>
    ResultBuilder& on_error(F&& func) {
        if (!result_) {
            func(result_.error());
        }
        return *this;
    }
    
    std::expected<T, E> build() && {
        return std::move(result_);
    }
    
    T value_or(T default_value) const {
        return result_.value_or(std::move(default_value));
    }
};

template<typename T, typename E>
ResultBuilder<T, E> make_result(std::expected<T, E> result) {
    return ResultBuilder<T, E>{std::move(result)};
}

void demonstrate_advanced_patterns() {
    std::cout << "=== expected的高级用法和设计模式 ===\n";
    
    // 流式API构建器模式
    auto process_number = [](const std::string& input) {
        return make_result(std::expected<std::string, std::string>{input})
            .flat_map([](const std::string& s) -> std::expected<int, std::string> {
                try {
                    return std::stoi(s);
                } catch (...) {
                    return std::unexpected("解析失败");
                }
            })
            .map([](int x) { return x * 2; })
            .map([](int x) { return x + 10; })
            .map_error([](const std::string& error) {
                return "处理失败: " + error;
            })
            .on_error([](const std::string& error) {
                std::cout << "错误回调: " << error << "\n";
            })
            .build();
    };
    
    auto test_inputs = std::vector<std::string>{"42", "invalid", "100"};
    
    std::cout << "流式API处理结果:\n";
    for (const auto& input : test_inputs) {
        auto result = process_number(input);
        if (result) {
            std::cout << std::format("输入 '{}' -> 结果: {}\n", input, *result);
        } else {
            std::cout << std::format("输入 '{}' -> 错误: {}\n", input, result.error());
        }
    }
    
    // 多个expected的并行处理
    std::cout << "\n多个expected的并行处理:\n";
    
    auto safe_add = [](int a, int b) -> std::expected<int, std::string> {
        if (a > 1000 || b > 1000) return std::unexpected("输入过大");
        return a + b;
    };
    
    auto safe_multiply = [](int a, int b) -> std::expected<int, std::string> {
        if (static_cast<long long>(a) * b > INT_MAX) return std::unexpected("乘法溢出");
        return a * b;
    };
    
    // 组合多个expected结果
    auto combine_results = [&](int x, int y, int z) -> std::expected<int, std::string> {
        auto add_result = safe_add(x, y);
        auto mul_result = safe_multiply(y, z);
        
        if (!add_result) return add_result;
        if (!mul_result) return mul_result;
        
        return safe_add(*add_result, *mul_result);
    };
    
    auto test_combinations = std::vector<std::tuple<int, int, int>>{
        {10, 20, 30},      // 正常
        {500, 600, 700},   // 加法溢出
        {100, 200, 300000} // 乘法溢出
    };
    
    for (const auto& [x, y, z] : test_combinations) {
        auto result = combine_results(x, y, z);
        if (result) {
            std::cout << std::format("组合({}, {}, {}) = {}\n", x, y, z, *result);
        } else {
            std::cout << std::format("组合({}, {}, {}) 失败: {}\n", x, y, z, result.error());
        }
    }
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++23 std::expected函数式错误处理深度解析\n";
    std::cout << "==========================================\n";
    
    demonstrate_expected_basics();
    demonstrate_structured_errors();
    demonstrate_monadic_operations();
    demonstrate_complex_error_handling();
    PerformanceTest::run_performance_comparison();
    demonstrate_advanced_patterns();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall 01_expected_error_handling.cpp -o expected_demo
./expected_demo

注意：std::expected需要C++23支持，目前编译器支持情况：
- GCC 12+: 实验性支持，可能需要 -std=c++2b
- Clang 16+: 部分支持
- MSVC 19.32+: 实验性支持

如果编译器不支持，可以使用第三方实现如tl::expected

关键学习点:
1. expected提供了零开销的函数式错误处理范式
2. 通过and_then、transform、or_else实现链式错误处理
3. 相比异常，expected在错误频繁的场景下性能更好
4. 支持结构化错误类型，提供丰富的错误信息
5. 可以构建复杂的错误处理流水线和恢复机制

注意事项:
- expected适合可预期的错误情况，不适合真正的异常情况
- 错误类型设计要平衡信息丰富性和性能开销
- 链式操作要注意错误类型的一致性
- 在性能关键路径上使用expected可以获得显著优势
*/