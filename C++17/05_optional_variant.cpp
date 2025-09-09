/**
 * C++17 std::optional和std::variant类型安全容器深度解析
 * 
 * 核心概念：
 * 1. 类型安全的空值处理 - optional的设计哲学和实现机制
 * 2. 类型安全的联合体 - variant的多态存储和访问策略
 * 3. 异常安全保证 - 强异常安全和基本异常安全的实现
 * 4. 函数式编程范式 - monadic操作和链式调用
 * 5. 性能和内存优化 - 零开销抽象的实际应用
 */

#include <iostream>
#include <optional>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <type_traits>
#include <chrono>
#include <exception>

// ===== 1. std::optional类型安全的空值处理 =====
class DatabaseRecord {
private:
    std::string name;
    std::optional<int> age;
    std::optional<std::string> email;
    
public:
    DatabaseRecord(const std::string& n) : name(n) {}
    
    void set_age(int a) { age = a; }
    void set_email(const std::string& e) { email = e; }
    
    const std::string& get_name() const { return name; }
    
    std::optional<int> get_age() const { return age; }
    std::optional<std::string> get_email() const { return email; }
    
    // 安全的年龄访问
    std::string get_age_display() const {
        return age.has_value() ? std::to_string(age.value()) : "未知";
    }
    
    // 安全的邮箱访问
    std::string get_email_display() const {
        return email.value_or("无邮箱");
    }
};

// 安全的除法函数
std::optional<double> safe_divide(double numerator, double denominator) {
    if (denominator == 0.0) {
        return std::nullopt;  // 返回空optional
    }
    return numerator / denominator;
}

// 链式调用示例
std::optional<std::string> parse_and_format_number(const std::string& str) {
    try {
        int num = std::stoi(str);
        if (num < 0) return std::nullopt;
        return "格式化数字: " + std::to_string(num);
    } catch (...) {
        return std::nullopt;
    }
}

void demonstrate_optional_basics() {
    std::cout << "=== std::optional基础演示 ===\n";
    
    // 基本用法
    std::optional<int> opt1;                    // 空optional
    std::optional<int> opt2 = 42;               // 有值的optional
    std::optional<int> opt3 = std::nullopt;     // 显式空optional
    
    std::cout << "opt1有值: " << opt1.has_value() << "\n";
    std::cout << "opt2有值: " << opt2.has_value() << ", 值: " << opt2.value() << "\n";
    std::cout << "opt3有值: " << opt3.has_value() << "\n";
    
    // 安全访问
    std::cout << "opt2安全值: " << opt2.value_or(0) << "\n";
    std::cout << "opt3安全值: " << opt3.value_or(-1) << "\n";
    
    // 数据库记录示例
    DatabaseRecord record{"Alice"};
    record.set_age(25);
    
    std::cout << "记录: " << record.get_name() 
              << ", 年龄: " << record.get_age_display()
              << ", 邮箱: " << record.get_email_display() << "\n";
    
    // 安全计算
    auto result1 = safe_divide(10.0, 2.0);
    auto result2 = safe_divide(10.0, 0.0);
    
    if (result1) {
        std::cout << "除法结果: " << *result1 << "\n";
    }
    if (!result2) {
        std::cout << "除法失败：除数为零\n";
    }
    
    std::cout << "\n";
}

// ===== 2. std::variant类型安全的联合体 =====
using Value = std::variant<int, double, std::string, std::vector<int>>;

class ConfigValue {
private:
    std::string key;
    Value value;
    
public:
    template<typename T>
    ConfigValue(const std::string& k, T&& v) 
        : key(k), value(std::forward<T>(v)) {}
    
    const std::string& get_key() const { return key; }
    const Value& get_value() const { return value; }
    
    // 类型安全的访问
    template<typename T>
    std::optional<T> get_as() const {
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
        return std::nullopt;
    }
    
    // 访问者模式
    void print_value() const {
        std::visit([this](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            std::cout << key << " = ";
            if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::cout << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    std::cout << v[i];
                    if (i < v.size() - 1) std::cout << ", ";
                }
                std::cout << "]";
            } else {
                std::cout << v;
            }
            std::cout << " (" << typeid(T).name() << ")\n";
        }, value);
    }
};

// 复杂的variant处理：表达式求值器
using Expression = std::variant<int, double, std::string>;

class ExpressionEvaluator {
public:
    static double evaluate(const Expression& expr, const std::map<std::string, double>& variables = {}) {
        return std::visit([&variables](const auto& value) -> double {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                return static_cast<double>(value);
            } else if constexpr (std::is_same_v<T, double>) {
                return value;
            } else if constexpr (std::is_same_v<T, std::string>) {
                auto it = variables.find(value);
                if (it != variables.end()) {
                    return it->second;
                }
                throw std::runtime_error("未知变量: " + value);
            }
        }, expr);
    }
};

void demonstrate_variant_basics() {
    std::cout << "=== std::variant基础演示 ===\n";
    
    // 基本用法
    std::variant<int, std::string> var1 = 42;
    std::variant<int, std::string> var2 = std::string{"hello"};
    
    std::cout << "var1类型索引: " << var1.index() << "\n";
    std::cout << "var2类型索引: " << var2.index() << "\n";
    
    // 类型检查和访问
    if (std::holds_alternative<int>(var1)) {
        std::cout << "var1是int: " << std::get<int>(var1) << "\n";
    }
    
    if (std::holds_alternative<std::string>(var2)) {
        std::cout << "var2是string: " << std::get<std::string>(var2) << "\n";
    }
    
    // 配置值系统
    std::vector<ConfigValue> config = {
        {"timeout", 30},
        {"rate", 2.5},
        {"name", std::string{"MyApp"}},
        {"ports", std::vector<int>{80, 443, 8080}}
    };
    
    std::cout << "配置项:\n";
    for (const auto& item : config) {
        item.print_value();
    }
    
    // 表达式求值
    std::map<std::string, double> variables = {{"x", 10.0}, {"y", 20.0}};
    std::vector<Expression> expressions = {42, 3.14, std::string{"x"}, std::string{"y"}};
    
    std::cout << "表达式求值:\n";
    for (const auto& expr : expressions) {
        try {
            double result = ExpressionEvaluator::evaluate(expr, variables);
            std::cout << "结果: " << result << "\n";
        } catch (const std::exception& e) {
            std::cout << "错误: " << e.what() << "\n";
        }
    }
    
    std::cout << "\n";
}

// ===== 3. 异常安全保证演示 =====
class ExceptionSafeProcessor {
private:
    std::optional<std::string> last_error;
    
public:
    // 强异常安全：要么完全成功，要么完全失败
    std::optional<std::vector<int>> process_numbers(const std::vector<std::string>& input) noexcept {
        try {
            std::vector<int> result;
            result.reserve(input.size());  // 预分配避免异常
            
            for (const auto& str : input) {
                int num = std::stoi(str);
                if (num < 0) {
                    last_error = "负数不被支持: " + str;
                    return std::nullopt;
                }
                result.push_back(num);
            }
            
            last_error.reset();
            return result;
        } catch (const std::exception& e) {
            last_error = "转换错误: " + std::string{e.what()};
            return std::nullopt;
        }
    }
    
    // 使用variant进行错误处理
    std::variant<std::vector<int>, std::string> process_with_error_info(const std::vector<std::string>& input) {
        try {
            std::vector<int> result;
            for (const auto& str : input) {
                result.push_back(std::stoi(str));
            }
            return result;
        } catch (const std::exception& e) {
            return std::string{"处理失败: "} + e.what();
        }
    }
    
    std::optional<std::string> get_last_error() const { return last_error; }
};

void demonstrate_exception_safety() {
    std::cout << "=== 异常安全保证演示 ===\n";
    
    ExceptionSafeProcessor processor;
    
    // 成功案例
    std::vector<std::string> good_input = {"1", "2", "3", "4", "5"};
    auto result1 = processor.process_numbers(good_input);
    
    if (result1) {
        std::cout << "处理成功，结果大小: " << result1->size() << "\n";
    }
    
    // 失败案例1：负数
    std::vector<std::string> bad_input1 = {"1", "2", "-3", "4"};
    auto result2 = processor.process_numbers(bad_input1);
    
    if (!result2) {
        std::cout << "处理失败: " << processor.get_last_error().value_or("未知错误") << "\n";
    }
    
    // 失败案例2：格式错误
    std::vector<std::string> bad_input2 = {"1", "2", "abc", "4"};
    auto result3 = processor.process_numbers(bad_input2);
    
    if (!result3) {
        std::cout << "处理失败: " << processor.get_last_error().value_or("未知错误") << "\n";
    }
    
    // 使用variant的错误处理
    auto variant_result = processor.process_with_error_info(bad_input2);
    std::visit([](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::cout << "variant处理成功，大小: " << value.size() << "\n";
        } else {
            std::cout << "variant处理失败: " << value << "\n";
        }
    }, variant_result);
    
    std::cout << "\n";
}

// ===== 4. 函数式编程范式演示 =====
template<typename T>
class Maybe {
private:
    std::optional<T> value;
    
public:
    Maybe() = default;
    Maybe(T v) : value(std::move(v)) {}
    Maybe(std::nullopt_t) : value(std::nullopt) {}
    
    bool has_value() const { return value.has_value(); }
    const T& operator*() const { return *value; }
    
    // Functor: map操作
    template<typename F>
    auto map(F&& func) -> Maybe<std::decay_t<decltype(func(*value))>> {
        if (value) {
            return Maybe<std::decay_t<decltype(func(*value))>>{func(*value)};
        }
        return std::nullopt;
    }
    
    // Monad: flatMap操作
    template<typename F>
    auto flat_map(F&& func) -> decltype(func(*value)) {
        if (value) {
            return func(*value);
        }
        return std::nullopt;
    }
    
    // 过滤操作
    template<typename Predicate>
    Maybe filter(Predicate&& pred) {
        if (value && pred(*value)) {
            return *this;
        }
        return std::nullopt;
    }
};

// 链式操作示例
Maybe<std::string> process_string_chain(const std::string& input) {
    return Maybe<std::string>{input}
        .filter([](const std::string& s) { return !s.empty(); })
        .map([](const std::string& s) { return s + "_processed"; })
        .map([](const std::string& s) { return s + "_validated"; });
}

// variant的函数式处理
template<typename... Types, typename F>
auto visit_with_index(const std::variant<Types...>& var, F&& func) {
    return std::visit([&func, index = var.index()](const auto& value) {
        return func(value, index);
    }, var);
}

void demonstrate_functional_programming() {
    std::cout << "=== 函数式编程范式演示 ===\n";
    
    // Maybe monad使用
    Maybe<int> m1{42};
    Maybe<int> m2{std::nullopt};
    
    auto result1 = m1.map([](int x) { return x * 2; })
                     .map([](int x) { return std::to_string(x); });
    
    auto result2 = m2.map([](int x) { return x * 2; })
                     .map([](int x) { return std::to_string(x); });
    
    if (result1.has_value()) {
        std::cout << "链式操作结果1: " << *result1 << "\n";
    }
    
    std::cout << "链式操作结果2有值: " << result2.has_value() << "\n";
    
    // 字符串处理链
    auto str_result1 = process_string_chain("hello");
    auto str_result2 = process_string_chain("");
    
    if (str_result1.has_value()) {
        std::cout << "字符串处理结果: " << *str_result1 << "\n";
    }
    std::cout << "空字符串处理有值: " << str_result2.has_value() << "\n";
    
    // variant的索引访问
    std::variant<int, std::string, double> var = std::string{"test"};
    auto index_result = visit_with_index(var, [](const auto& value, size_t index) {
        std::cout << "类型索引 " << index << ", 值: " << value << "\n";
        return index;
    });
    
    std::cout << "\n";
}

// ===== 5. 性能和内存优化演示 =====
// 性能测试：optional vs 指针
class PerformanceTest {
public:
    static void test_optional_vs_pointer() {
        constexpr size_t iterations = 10000000;
        
        // optional测试
        auto start = std::chrono::high_resolution_clock::now();
        
        size_t optional_hits = 0;
        for (size_t i = 0; i < iterations; ++i) {
            std::optional<int> opt = (i % 2 == 0) ? std::make_optional(static_cast<int>(i)) : std::nullopt;
            if (opt) {
                optional_hits += *opt % 1000;  // 避免编译器优化
            }
        }
        
        auto mid = std::chrono::high_resolution_clock::now();
        
        // 指针测试
        size_t pointer_hits = 0;
        for (size_t i = 0; i < iterations; ++i) {
            std::unique_ptr<int> ptr = (i % 2 == 0) ? std::make_unique<int>(static_cast<int>(i)) : nullptr;
            if (ptr) {
                pointer_hits += *ptr % 1000;  // 避免编译器优化
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        
        auto optional_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
        auto pointer_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
        
        std::cout << "Optional耗时: " << optional_time.count() << "ms (命中: " << optional_hits % 1000 << ")\n";
        std::cout << "指针耗时: " << pointer_time.count() << "ms (命中: " << pointer_hits % 1000 << ")\n";
    }
    
    static void test_variant_vs_inheritance() {
        constexpr size_t iterations = 1000000;
        
        // variant测试
        std::vector<std::variant<int, double, std::string>> variant_data;
        variant_data.reserve(iterations);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            switch (i % 3) {
                case 0: variant_data.emplace_back(static_cast<int>(i)); break;
                case 1: variant_data.emplace_back(static_cast<double>(i)); break;
                case 2: variant_data.emplace_back(std::to_string(i)); break;
            }
        }
        
        size_t variant_sum = 0;
        for (const auto& item : variant_data) {
            std::visit([&variant_sum](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_arithmetic_v<T>) {
                    variant_sum += static_cast<size_t>(value) % 1000;
                }
            }, item);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto variant_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Variant耗时: " << variant_time.count() << "ms (求和: " << variant_sum % 1000 << ")\n";
    }
};

void demonstrate_performance_optimization() {
    std::cout << "=== 性能和内存优化演示 ===\n";
    
    // 内存布局分析
    std::cout << "类型大小分析:\n";
    std::cout << "std::optional<int>: " << sizeof(std::optional<int>) << " 字节\n";
    std::cout << "std::optional<std::string>: " << sizeof(std::optional<std::string>) << " 字节\n";
    std::cout << "std::variant<int, double, std::string>: " << sizeof(std::variant<int, double, std::string>) << " 字节\n";
    std::cout << "std::unique_ptr<int>: " << sizeof(std::unique_ptr<int>) << " 字节\n";
    
    // 性能测试
    PerformanceTest::test_optional_vs_pointer();
    PerformanceTest::test_variant_vs_inheritance();
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 std::optional和std::variant类型安全容器深度解析\n";
    std::cout << "====================================================\n";
    
    demonstrate_optional_basics();
    demonstrate_variant_basics();
    demonstrate_exception_safety();
    demonstrate_functional_programming();
    demonstrate_performance_optimization();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 05_optional_variant.cpp -o optional_variant
./optional_variant

关键学习点:
1. std::optional提供类型安全的空值处理，避免空指针异常
2. std::variant提供类型安全的联合体，支持多种类型的存储
3. 两者都提供强异常安全保证，不会出现未定义行为
4. 支持函数式编程范式，可以实现优雅的链式操作
5. 零开销抽象，性能接近原生实现

注意事项:
- optional的value()在空值时会抛出异常，推荐使用value_or()
- variant访问时要注意类型检查，避免bad_variant_access异常
- 在性能关键的场景中要考虑内存开销
- 与现代C++其他特性（如结构化绑定）结合使用效果更佳
*/