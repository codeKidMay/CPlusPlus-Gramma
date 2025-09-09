/**
 * C++20 Lambda表达式增强深度解析
 * 
 * 核心概念：
 * 1. 模板Lambda - 显式模板参数和约束的Lambda表达式
 * 2. 参数包捕获 - 完美转发和变参模板的Lambda应用
 * 3. consteval Lambda - 强制编译期执行的Lambda函数
 * 4. 默认可构造Lambda - 无状态Lambda的优化特性
 * 5. Lambda与Concepts - 约束模板Lambda的类型安全
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <concepts>
#include <string>
#include <memory>
#include <utility>

// ===== 1. 模板Lambda演示 =====
// 传统泛型Lambda的局限性
void demonstrate_traditional_generic_lambda() {
    std::cout << "=== 传统泛型Lambda演示 ===\n";
    
    // C++14泛型Lambda
    auto generic_add = [](auto a, auto b) {
        return a + b;
    };
    
    std::cout << "传统泛型Lambda:\n";
    std::cout << "generic_add(1, 2): " << generic_add(1, 2) << "\n";
    std::cout << "generic_add(1.5, 2.5): " << generic_add(1.5, 2.5) << "\n";
    std::cout << "generic_add(std::string(\"Hello\"), std::string(\" World\")): " 
              << generic_add(std::string("Hello"), std::string(" World")) << "\n";
    
    // 但无法对类型进行约束或特化
    std::cout << "\n";
}

// C++20模板Lambda
void demonstrate_template_lambda() {
    std::cout << "=== C++20模板Lambda演示 ===\n";
    
    // 显式模板参数Lambda
    auto template_add = []<typename T>(T a, T b) {
        std::cout << "处理类型: " << typeid(T).name() << "\n";
        return a + b;
    };
    
    std::cout << "模板Lambda:\n";
    std::cout << "template_add(1, 2): " << template_add(1, 2) << "\n";
    std::cout << "template_add(1.5, 2.5): " << template_add(1.5, 2.5) << "\n";
    
    // 带约束的模板Lambda
    auto constrained_add = []<std::integral T>(T a, T b) {
        std::cout << "整数类型加法: ";
        return a + b;
    };
    
    std::cout << "\n带约束的模板Lambda:\n";
    std::cout << "constrained_add(10, 20): " << constrained_add(10, 20) << "\n";
    // constrained_add(1.5, 2.5);  // 编译错误：不满足std::integral约束
    
    // 多模板参数Lambda
    auto multi_template = []<typename T, typename U>(T&& t, U&& u) {
        std::cout << "T类型: " << typeid(T).name() << ", U类型: " << typeid(U).name() << "\n";
        return std::forward<T>(t) + std::forward<U>(u);
    };
    
    std::cout << "\n多模板参数Lambda:\n";
    std::cout << "multi_template(42, 3.14): " << multi_template(42, 3.14) << "\n";
    
    // 可变参数模板Lambda
    auto variadic_sum = []<typename... Args>(Args... args) {
        std::cout << "参数个数: " << sizeof...(args) << "\n";
        return (args + ...);  // C++17折叠表达式
    };
    
    std::cout << "\n可变参数模板Lambda:\n";
    std::cout << "variadic_sum(1, 2, 3, 4, 5): " << variadic_sum(1, 2, 3, 4, 5) << "\n";
    
    std::cout << "\n";
}

// ===== 2. 参数包捕获演示 =====
// 完美转发工厂函数
template<typename F, typename... Args>
auto make_delayed_call(F&& func, Args&&... args) {
    // C++20参数包捕获
    return [func = std::forward<F>(func), args...]() mutable {
        return func(args...);
    };
}

// 更复杂的参数包捕获：完美转发
template<typename F, typename... Args>
auto make_perfect_delayed_call(F&& func, Args&&... args) {
    return [func = std::forward<F>(func), ...captured_args = std::forward<Args>(args)]() mutable {
        return func(std::move(captured_args)...);
    };
}

// 参数包展开的高级应用
template<typename... Functors>
auto compose_functions(Functors... funcs) {
    return [funcs...]<typename T>(T&& input) {
        // 按顺序应用所有函数
        auto apply_function = [&input](auto func) {
            input = func(input);
        };
        (apply_function(funcs), ...);  // 折叠表达式
        return std::forward<T>(input);
    };
}

void demonstrate_pack_capture() {
    std::cout << "=== 参数包捕获演示 ===\n";
    
    // 简单的延迟调用
    auto multiply = [](int a, int b, int c) {
        std::cout << "计算 " << a << " * " << b << " * " << c << " = ";
        return a * b * c;
    };
    
    auto delayed_multiply = make_delayed_call(multiply, 2, 3, 4);
    std::cout << "延迟调用结果: " << delayed_multiply() << "\n";
    
    // 完美转发示例
    auto string_processor = [](const std::string& s) {
        std::cout << "处理字符串: \"" << s << "\"\n";
        return s + "_processed";
    };
    
    std::string original = "test";
    auto delayed_process = make_perfect_delayed_call(string_processor, std::move(original));
    std::cout << "完美转发结果: " << delayed_process() << "\n";
    // 注意：original在移动后可能为空
    
    // 函数组合示例
    auto add_one = [](int x) { return x + 1; };
    auto multiply_two = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };
    
    auto composed = compose_functions(add_one, multiply_two, square);
    std::cout << "函数组合 f(g(h(5))): " << composed(5) << "\n";  // ((5+1)*2)^2 = 144
    
    std::cout << "\n";
}

// ===== 3. consteval Lambda演示 =====
void demonstrate_consteval_lambda() {
    std::cout << "=== consteval Lambda演示 ===\n";
    
    // consteval Lambda - 强制编译期执行
    auto consteval_factorial = []<int N>() consteval {
        if constexpr (N <= 1) {
            return 1;
        } else {
            return N * consteval_factorial.template operator()<N-1>();
        }
    };
    
    // 只能在编译期调用
    constexpr auto fact_5 = consteval_factorial.template operator()<5>();
    constexpr auto fact_10 = consteval_factorial.template operator()<10>();
    
    std::cout << "consteval Lambda计算:\n";
    std::cout << "5! = " << fact_5 << "\n";
    std::cout << "10! = " << fact_10 << "\n";
    
    // 编译期字符串处理Lambda
    auto consteval_string_length = [](const char* str) consteval -> size_t {
        size_t len = 0;
        while (str[len]) ++len;
        return len;
    };
    
    constexpr auto hello_len = consteval_string_length("Hello, World!");
    std::cout << "编译期字符串长度: " << hello_len << "\n";
    
    // 编译期算法Lambda
    auto consteval_is_palindrome = [](const char* str) consteval -> bool {
        size_t len = 0;
        while (str[len]) ++len;  // 计算长度
        
        for (size_t i = 0; i < len / 2; ++i) {
            if (str[i] != str[len - 1 - i]) {
                return false;
            }
        }
        return true;
    };
    
    constexpr bool is_radar_palindrome = consteval_is_palindrome("radar");
    constexpr bool is_hello_palindrome = consteval_is_palindrome("hello");
    
    std::cout << "\"radar\"是回文: " << is_radar_palindrome << "\n";
    std::cout << "\"hello\"是回文: " << is_hello_palindrome << "\n";
    
    std::cout << "\n";
}

// ===== 4. 默认可构造Lambda演示 =====
void demonstrate_default_constructible_lambda() {
    std::cout << "=== 默认可构造Lambda演示 ===\n";
    
    // 无状态Lambda是默认可构造的
    auto stateless_lambda = [](int x) { return x * 2; };
    
    // 可以创建多个实例
    decltype(stateless_lambda) lambda1;  // 默认构造
    decltype(stateless_lambda) lambda2 = stateless_lambda;  // 拷贝构造
    
    std::cout << "无状态Lambda测试:\n";
    std::cout << "lambda1(5): " << lambda1(5) << "\n";
    std::cout << "lambda2(5): " << lambda2(5) << "\n";
    
    // 可以存储在容器中
    std::vector<decltype(stateless_lambda)> lambda_vector(3);  // 创建3个默认构造的lambda
    
    std::cout << "容器中的Lambda:\n";
    for (size_t i = 0; i < lambda_vector.size(); ++i) {
        std::cout << "lambda_vector[" << i << "](10): " << lambda_vector[i](10) << "\n";
    }
    
    // 带状态的Lambda不是默认可构造的
    int multiplier = 3;
    auto stateful_lambda = [multiplier](int x) { return x * multiplier; };
    
    std::cout << "带状态Lambda测试:\n";
    std::cout << "stateful_lambda(5): " << stateful_lambda(5) << "\n";
    
    // decltype(stateful_lambda) lambda3;  // 编译错误：不能默认构造
    
    // Lambda的赋值操作
    auto assignable_lambda = [](int x) { return x + 1; };
    auto another_lambda = assignable_lambda;  // 可以赋值
    
    std::cout << "Lambda赋值:\n";
    std::cout << "assignable_lambda(10): " << assignable_lambda(10) << "\n";
    std::cout << "another_lambda(10): " << another_lambda(10) << "\n";
    
    std::cout << "\n";
}

// ===== 5. Lambda与Concepts演示 =====
// 定义一些有用的概念
template<typename F, typename T>
concept UnaryPredicate = requires(F f, T t) {
    { f(t) } -> std::convertible_to<bool>;
};

template<typename F, typename T>
concept UnaryFunction = requires(F f, T t) {
    f(t);
};

template<typename F, typename T, typename R>
concept UnaryTransform = requires(F f, T t) {
    { f(t) } -> std::convertible_to<R>;
};

// 约束的Lambda工厂函数
template<typename T>
auto make_predicate_lambda() {
    return []<UnaryPredicate<T> Pred>(Pred&& pred) {
        return [pred = std::forward<Pred>(pred)](const T& value) {
            std::cout << "应用谓词到值: " << value << "\n";
            return pred(value);
        };
    };
}

// 高阶函数：接受Lambda并返回Lambda
template<typename Container>
auto make_container_processor() {
    return []<UnaryPredicate<typename Container::value_type> Pred>(Pred&& predicate) {
        return [predicate = std::forward<Pred>(predicate)](const Container& container) {
            std::cout << "处理容器，元素数量: " << container.size() << "\n";
            
            size_t count = 0;
            for (const auto& element : container) {
                if (predicate(element)) {
                    ++count;
                }
            }
            
            return count;
        };
    };
}

// 带约束的模板Lambda
void demonstrate_lambda_with_concepts() {
    std::cout << "=== Lambda与Concepts演示 ===\n";
    
    // 创建约束的Lambda
    auto int_predicate_factory = make_predicate_lambda<int>();
    
    auto is_even = [](int x) { return x % 2 == 0; };
    auto enhanced_is_even = int_predicate_factory(is_even);
    
    std::cout << "约束谓词Lambda:\n";
    std::cout << "enhanced_is_even(4): " << enhanced_is_even(4) << "\n";
    std::cout << "enhanced_is_even(3): " << enhanced_is_even(3) << "\n";
    
    // 容器处理器
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    auto vector_processor = make_container_processor<std::vector<int>>();
    auto count_even = vector_processor(is_even);
    
    std::cout << "\n容器处理器:\n";
    auto even_count = count_even(numbers);
    std::cout << "偶数个数: " << even_count << "\n";
    
    // 组合约束Lambda
    auto combined_processor = []<std::integral T>(const std::vector<T>& vec, UnaryPredicate<T> auto pred) {
        std::cout << "处理整数向量，大小: " << vec.size() << "\n";
        
        std::vector<T> filtered;
        std::copy_if(vec.begin(), vec.end(), std::back_inserter(filtered), pred);
        
        return filtered;
    };
    
    auto filtered_numbers = combined_processor(numbers, [](int x) { return x > 5; });
    std::cout << "过滤结果 (>5): ";
    for (int n : filtered_numbers) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    
    // 递归Lambda与concepts
    std::function<bool(int)> recursive_is_even;
    recursive_is_even = [&recursive_is_even](int n) -> bool {
        if (n == 0) return true;
        if (n == 1) return false;
        return recursive_is_even(std::abs(n) - 2);
    };
    
    static_assert(UnaryPredicate<decltype(recursive_is_even), int>);
    
    std::cout << "\n递归Lambda测试:\n";
    std::cout << "recursive_is_even(8): " << recursive_is_even(8) << "\n";
    std::cout << "recursive_is_even(7): " << recursive_is_even(7) << "\n";
    
    std::cout << "\n";
}

// 高级应用：Lambda元编程
template<auto... Values>
constexpr auto make_lookup_lambda() {
    return []<size_t Index>() constexpr {
        constexpr std::array values{Values...};
        static_assert(Index < values.size(), "Index out of bounds");
        return values[Index];
    };
}

// Lambda工厂模式
class LambdaFactory {
public:
    template<typename T>
    static auto make_validator() {
        return []<std::predicate<T> auto... Predicates>(Predicates... preds) {
            return [preds...](const T& value) {
                return (preds(value) && ...);  // 所有谓词都必须为真
            };
        };
    }
    
    template<typename T>
    static auto make_transformer() {
        return []<typename... Transformers>(Transformers... transformers) {
            return [transformers...](T value) {
                ((value = transformers(value)), ...);  // 依次应用所有变换
                return value;
            };
        };
    }
};

void demonstrate_advanced_lambda_patterns() {
    std::cout << "=== 高级Lambda模式演示 ===\n";
    
    // Lambda元编程
    constexpr auto lookup = make_lookup_lambda<10, 20, 30, 40, 50>();
    constexpr auto value_2 = lookup.template operator()<2>();
    constexpr auto value_4 = lookup.template operator()<4>();
    
    std::cout << "Lambda元编程查找表:\n";
    std::cout << "lookup<2>(): " << value_2 << "\n";
    std::cout << "lookup<4>(): " << value_4 << "\n";
    
    // Lambda工厂模式
    auto int_validator_factory = LambdaFactory::make_validator<int>();
    
    auto is_positive = [](int x) { return x > 0; };
    auto is_less_than_100 = [](int x) { return x < 100; };
    auto is_even = [](int x) { return x % 2 == 0; };
    
    auto comprehensive_validator = int_validator_factory(is_positive, is_less_than_100, is_even);
    
    std::cout << "\nLambda工厂验证器:\n";
    std::cout << "comprehensive_validator(50): " << comprehensive_validator(50) << "\n";  // true
    std::cout << "comprehensive_validator(51): " << comprehensive_validator(51) << "\n";  // false (奇数)
    std::cout << "comprehensive_validator(-2): " << comprehensive_validator(-2) << "\n";  // false (负数)
    
    // Lambda变换器
    auto int_transformer_factory = LambdaFactory::make_transformer<int>();
    
    auto add_ten = [](int x) { return x + 10; };
    auto multiply_two = [](int x) { return x * 2; };
    auto subtract_five = [](int x) { return x - 5; };
    
    auto comprehensive_transformer = int_transformer_factory(add_ten, multiply_two, subtract_five);
    
    std::cout << "\nLambda变换器:\n";
    std::cout << "comprehensive_transformer(5): " << comprehensive_transformer(5) << "\n";  // ((5+10)*2)-5 = 25
    
    std::cout << "\n";
}

// 性能对比：Lambda vs 函数指针 vs 函数对象
void performance_comparison() {
    std::cout << "=== 性能对比演示 ===\n";
    
    const size_t iterations = 10000000;
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 1);
    
    // Lambda
    auto start = std::chrono::high_resolution_clock::now();
    
    volatile int lambda_sum = 0;
    auto lambda_square = [](int x) { return x * x; };
    
    for (size_t i = 0; i < iterations; ++i) {
        lambda_sum += lambda_square(data[i % data.size()]);
    }
    
    auto lambda_end = std::chrono::high_resolution_clock::now();
    
    // 函数指针
    auto func_ptr_square = [](int x) { return x * x; };
    int (*fp)(int) = func_ptr_square;
    
    volatile int fp_sum = 0;
    
    for (size_t i = 0; i < iterations; ++i) {
        fp_sum += fp(data[i % data.size()]);
    }
    
    auto fp_end = std::chrono::high_resolution_clock::now();
    
    // std::function
    std::function<int(int)> func_obj = [](int x) { return x * x; };
    
    volatile int func_obj_sum = 0;
    
    for (size_t i = 0; i < iterations; ++i) {
        func_obj_sum += func_obj(data[i % data.size()]);
    }
    
    auto func_obj_end = std::chrono::high_resolution_clock::now();
    
    auto lambda_time = std::chrono::duration_cast<std::chrono::microseconds>(lambda_end - start);
    auto fp_time = std::chrono::duration_cast<std::chrono::microseconds>(fp_end - lambda_end);
    auto func_obj_time = std::chrono::duration_cast<std::chrono::microseconds>(func_obj_end - fp_end);
    
    std::cout << "性能测试结果 (" << iterations << " 次迭代):\n";
    std::cout << "Lambda: " << lambda_time.count() << " 微秒\n";
    std::cout << "函数指针: " << fp_time.count() << " 微秒\n";
    std::cout << "std::function: " << func_obj_time.count() << " 微秒\n";
    
    std::cout << "结果验证: ";
    std::cout << "Lambda=" << (lambda_sum % 1000) << ", ";
    std::cout << "FP=" << (fp_sum % 1000) << ", ";
    std::cout << "FuncObj=" << (func_obj_sum % 1000) << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 Lambda表达式增强深度解析\n";
    std::cout << "===============================\n";
    
    demonstrate_traditional_generic_lambda();
    demonstrate_template_lambda();
    demonstrate_pack_capture();
    demonstrate_consteval_lambda();
    demonstrate_default_constructible_lambda();
    demonstrate_lambda_with_concepts();
    demonstrate_advanced_lambda_patterns();
    performance_comparison();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 06_lambda_enhancements.cpp -o lambda_enhancements
./lambda_enhancements

关键学习点:
1. 模板Lambda提供了显式模板参数和约束能力，比auto更强大
2. 参数包捕获支持完美转发，可以构建强大的函数组合器
3. consteval Lambda强制编译期执行，实现零运行时开销
4. 无状态Lambda是默认可构造的，可以存储在容器中
5. Lambda与Concepts结合提供了强大的类型安全和约束能力

注意事项:
- 模板Lambda的语法比较复杂，需要谨慎使用
- 参数包捕获要注意对象生命周期和移动语义
- consteval Lambda只能处理编译期已知的值
- 默认可构造性只对无状态Lambda有效
- 过度使用Lambda可能影响代码可读性和编译速度
*/