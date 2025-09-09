/**
 * 第4章：从SFINAE到Concepts的演进深度解析
 * 
 * 核心概念：
 * 1. SFINAE原理 - Substitution Failure Is Not An Error的深层机制
 * 2. 表达式SFINAE - 基于表达式有效性的模板选择
 * 3. 检测惯用法 - Detection Idiom的设计模式
 * 4. Concepts系统 - C++20约束编程的革命性改进
 * 5. 迁移策略 - 从SFINAE平滑迁移到Concepts
 */

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <iterator>
#include <memory>
#include <concepts>
#include <ranges>

// ===== 1. SFINAE基础原理深度解析 =====
void demonstrate_sfinae_basics() {
    std::cout << "=== SFINAE基础原理深度解析 ===\n";
    
    // SFINAE的核心：替换失败不是错误
    // 当模板参数替换导致无效代码时，编译器会静默忽略该特化，而不是报错
    
    // 示例1：基于类型存在性的SFINAE
    template<typename T, typename = void>
    struct has_value_type : std::false_type {};
    
    template<typename T>
    struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};
    
    // 示例2：基于成员函数存在性的SFINAE
    template<typename T, typename = void>
    struct has_size_method : std::false_type {};
    
    template<typename T>
    struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
    
    // 示例3：基于表达式有效性的SFINAE
    template<typename T, typename = void>
    struct is_incrementable : std::false_type {};
    
    template<typename T>
    struct is_incrementable<T, std::void_t<decltype(++std::declval<T&>())>> : std::true_type {};
    
    // 示例4：基于运算符存在性的SFINAE
    template<typename T, typename U, typename = void>
    struct is_addable : std::false_type {};
    
    template<typename T, typename U>
    struct is_addable<T, U, std::void_t<decltype(std::declval<T>() + std::declval<U>())>> : std::true_type {};
    
    // 测试SFINAE检测
    std::cout << "std::vector has value_type: " << has_value_type<std::vector<int>>::value << "\n";
    std::cout << "int has value_type: " << has_value_type<int>::value << "\n";
    
    std::cout << "std::vector has size(): " << has_size_method<std::vector<int>>::value << "\n";
    std::cout << "int has size(): " << has_size_method<int>::value << "\n";
    
    std::cout << "int is incrementable: " << is_incrementable<int>::value << "\n";
    std::cout << "std::string is incrementable: " << is_incrementable<std::string>::value << "\n";
    
    std::cout << "int + int is valid: " << is_addable<int, int>::value << "\n";
    std::cout << "int + std::string is valid: " << is_addable<int, std::string>::value << "\n";
    
    std::cout << "\n";
}

// ===== 2. 函数SFINAE和重载解析 =====
void demonstrate_function_sfinae() {
    std::cout << "=== 函数SFINAE和重载解析 ===\n";
    
    // 函数模板的SFINAE应用
    
    // 方法1：返回类型SFINAE
    template<typename T>
    auto get_size_v1(const T& container) -> decltype(container.size()) {
        std::cout << "Container size (return type SFINAE): " << container.size() << "\n";
        return container.size();
    }
    
    template<typename T>
    auto get_size_v1(const T& non_container) -> std::enable_if_t<!std::is_same_v<decltype(non_container.size()), void>, int> {
        // 这个重载永远不会被选中，因为上面的版本更匹配
        return -1;
    }
    
    // 默认情况处理
    template<typename T>
    int get_size_v1(...) {
        std::cout << "Non-container type, default size: 1\n";
        return 1;
    }
    
    // 方法2：模板参数SFINAE
    template<typename T>
    std::enable_if_t<std::is_arithmetic_v<T>, void>
    process_value(T value) {
        std::cout << "Processing arithmetic value: " << value << "\n";
    }
    
    template<typename T>
    std::enable_if_t<std::is_same_v<T, std::string>, void>
    process_value(const T& value) {
        std::cout << "Processing string value: \"" << value << "\"\n";
    }
    
    template<typename T>
    std::enable_if_t<!std::is_arithmetic_v<T> && !std::is_same_v<T, std::string>, void>
    process_value(const T& value) {
        std::cout << "Processing other type (size: " << sizeof(T) << " bytes)\n";
    }
    
    // 方法3：requires表达式（C++20前的模拟）
    template<typename T>
    auto serialize_container(const T& container) 
    -> std::void_t<decltype(container.begin()), decltype(container.end())> {
        std::cout << "Serializing container with " << std::distance(container.begin(), container.end()) << " elements\n";
    }
    
    // 测试函数SFINAE
    std::vector<int> vec{1, 2, 3, 4, 5};
    get_size_v1(vec);
    get_size_v1(42);
    
    process_value(42);
    process_value(3.14);
    process_value(std::string{"hello"});
    process_value(vec);
    
    serialize_container(vec);
    serialize_container(std::string{"test"});
    
    std::cout << "\n";
}

// ===== 3. 检测惯用法(Detection Idiom)模式 =====
void demonstrate_detection_idiom() {
    std::cout << "=== 检测惯用法(Detection Idiom)模式 ===\n";
    
    // Walter Brown's Detection Idiom - 现代SFINAE的标准模式
    
    namespace detail {
        template<typename Default, typename AlwaysVoid, template<typename...> class Op, typename... Args>
        struct detector {
            using value_t = std::false_type;
            using type = Default;
        };
        
        template<typename Default, template<typename...> class Op, typename... Args>
        struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
            using value_t = std::true_type;
            using type = Op<Args...>;
        };
    }
    
    // 检测器基础模板
    template<template<typename...> class Op, typename... Args>
    using is_detected = typename detail::detector<void, void, Op, Args...>::value_t;
    
    template<template<typename...> class Op, typename... Args>
    using detected_t = typename detail::detector<void, void, Op, Args...>::type;
    
    template<typename Default, template<typename...> class Op, typename... Args>
    using detected_or = detail::detector<Default, void, Op, Args...>;
    
    template<typename Default, template<typename...> class Op, typename... Args>
    using detected_or_t = typename detected_or<Default, Op, Args...>::type;
    
    template<typename Expected, template<typename...> class Op, typename... Args>
    using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;
    
    template<typename To, template<typename...> class Op, typename... Args>
    using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;
    
    // 定义检测操作
    template<typename T>
    using size_member_t = decltype(std::declval<T>().size());
    
    template<typename T>
    using begin_member_t = decltype(std::declval<T>().begin());
    
    template<typename T>
    using value_type_t = typename T::value_type;
    
    template<typename T>
    using iterator_t = typename T::iterator;
    
    template<typename T, typename U>
    using plus_operator_t = decltype(std::declval<T>() + std::declval<U>());
    
    // 使用检测惯用法
    template<typename Container>
    void analyze_container(const Container& c) {
        std::cout << "\n--- Container Analysis ---\n";
        std::cout << "Type: " << typeid(Container).name() << "\n";
        
        constexpr bool has_size = is_detected<size_member_t, Container>::value;
        constexpr bool has_begin = is_detected<begin_member_t, Container>::value;
        constexpr bool has_value_type = is_detected<value_type_t, Container>::value;
        constexpr bool has_iterator = is_detected<iterator_t, Container>::value;
        
        std::cout << "Has size(): " << has_size << "\n";
        std::cout << "Has begin(): " << has_begin << "\n";
        std::cout << "Has value_type: " << has_value_type << "\n";
        std::cout << "Has iterator: " << has_iterator << "\n";
        
        if constexpr (has_size) {
            std::cout << "Size: " << c.size() << "\n";
        }
        
        if constexpr (has_value_type) {
            using ValueType = detected_t<value_type_t, Container>;
            std::cout << "Value type size: " << sizeof(ValueType) << " bytes\n";
        }
    }
    
    // 测试检测惯用法
    std::vector<int> vec{1, 2, 3};
    std::array<double, 5> arr{1.1, 2.2, 3.3, 4.4, 5.5};
    int plain_int = 42;
    
    analyze_container(vec);
    analyze_container(arr);
    // analyze_container(plain_int); // 这会显示所有检测都为false
    
    // 检测运算符
    constexpr bool int_addable = is_detected<plus_operator_t, int, int>::value;
    constexpr bool string_int_addable = is_detected<plus_operator_t, std::string, int>::value;
    
    std::cout << "\nint + int is valid: " << int_addable << "\n";
    std::cout << "string + int is valid: " << string_int_addable << "\n";
    
    std::cout << "\n";
}

// ===== 4. C++20 Concepts基础 =====
void demonstrate_concepts_basics() {
    std::cout << "=== C++20 Concepts基础 ===\n";
    
    // 基础概念定义
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;
    
    template<typename T>
    concept Incrementable = requires(T t) {
        ++t;
        t++;
    };
    
    template<typename T>
    concept HasSize = requires(T t) {
        t.size();
    };
    
    template<typename T>
    concept Container = requires(T t) {
        t.begin();
        t.end();
        t.size();
        typename T::value_type;
    };
    
    template<typename T, typename U>
    concept Addable = requires(T t, U u) {
        t + u;
    };
    
    // 复合概念
    template<typename T>
    concept ArithmeticContainer = Container<T> && Arithmetic<typename T::value_type>;
    
    // 使用concepts的函数
    template<Arithmetic T>
    void process_arithmetic(T value) {
        std::cout << "Processing arithmetic: " << value << "\n";
    }
    
    template<Container C>
    void process_container(const C& container) {
        std::cout << "Processing container with " << container.size() << " elements\n";
    }
    
    template<typename T>
    requires HasSize<T>
    auto get_size(const T& obj) {
        return obj.size();
    }
    
    template<ArithmeticContainer C>
    auto sum_container(const C& container) {
        typename C::value_type sum{};
        for (const auto& elem : container) {
            sum += elem;
        }
        return sum;
    }
    
    // 概念的条件使用
    template<typename T>
    void flexible_process(const T& value) {
        if constexpr (Arithmetic<T>) {
            std::cout << "Arithmetic value: " << value << "\n";
        } else if constexpr (Container<T>) {
            std::cout << "Container with size: " << value.size() << "\n";
        } else {
            std::cout << "Unknown type\n";
        }
    }
    
    // 测试concepts
    process_arithmetic(42);
    process_arithmetic(3.14);
    
    std::vector<int> vec{1, 2, 3, 4, 5};
    process_container(vec);
    
    std::cout << "Vector size: " << get_size(vec) << "\n";
    std::cout << "Vector sum: " << sum_container(vec) << "\n";
    
    flexible_process(42);
    flexible_process(vec);
    flexible_process(std::string{"hello"});
    
    std::cout << "\n";
}

// ===== 5. 高级Concepts技术 =====
void demonstrate_advanced_concepts() {
    std::cout << "=== 高级Concepts技术 ===\n";
    
    // 嵌套要求和类型要求
    template<typename T>
    concept Iterator = requires(T it) {
        *it;                    // 解引用要求
        ++it;                   // 递增要求
        typename T::value_type; // 类型要求
        
        // 嵌套要求
        requires std::is_copy_constructible_v<T>;
        requires std::is_copy_assignable_v<T>;
    };
    
    template<typename T>
    concept ForwardIterator = Iterator<T> && requires(T it) {
        it++;
        requires std::is_default_constructible_v<T>;
    };
    
    template<typename T>
    concept RandomAccessIterator = ForwardIterator<T> && requires(T it, std::ptrdiff_t n) {
        it += n;
        it -= n;
        it + n;
        it - n;
        it[n];
        it - it;
    };
    
    // 函数概念
    template<typename F, typename... Args>
    concept Callable = std::invocable<F, Args...>;
    
    template<typename F, typename R, typename... Args>
    concept CallableReturning = Callable<F, Args...> && 
                               std::is_same_v<std::invoke_result_t<F, Args...>, R>;
    
    // 约束模板的多种方式
    // 方式1：requires子句
    template<typename T>
    requires std::integral<T>
    auto factorial_v1(T n) {
        if (n <= 1) return T{1};
        return n * factorial_v1(n - 1);
    }
    
    // 方式2：概念约束
    template<std::integral T>
    auto factorial_v2(T n) {
        if (n <= 1) return T{1};
        return n * factorial_v2(n - 1);
    }
    
    // 方式3：函数模板后置约束
    template<typename T>
    auto factorial_v3(T n) requires std::integral<T> {
        if (n <= 1) return T{1};
        return n * factorial_v3(n - 1);
    }
    
    // 重载解析中的概念
    template<std::integral T>
    void process_number(T value) {
        std::cout << "Processing integral: " << value << "\n";
    }
    
    template<std::floating_point T>
    void process_number(T value) {
        std::cout << "Processing floating point: " << value << "\n";
    }
    
    // 概念的短路求值
    template<typename T>
    concept SafeIncrement = requires(T t) {
        ++t;
    } && !std::is_same_v<T, bool>; // bool的++已被弃用
    
    template<typename T>
    requires SafeIncrement<T>
    void safe_increment(T& value) {
        ++value;
        std::cout << "Safely incremented to: " << value << "\n";
    }
    
    // 测试高级concepts
    std::vector<int> vec{1, 2, 3};
    auto it = vec.begin();
    
    std::cout << "Iterator concepts:\n";
    std::cout << "Is Iterator: " << Iterator<decltype(it)> << "\n";
    std::cout << "Is ForwardIterator: " << ForwardIterator<decltype(it)> << "\n";
    std::cout << "Is RandomAccessIterator: " << RandomAccessIterator<decltype(it)> << "\n";
    
    // 测试factorial重载
    std::cout << "Factorial 5: " << factorial_v1(5) << "\n";
    std::cout << "Factorial 6: " << factorial_v2(6) << "\n";
    std::cout << "Factorial 4: " << factorial_v3(4) << "\n";
    
    // 测试重载解析
    process_number(42);
    process_number(3.14);
    
    // 测试安全递增
    int x = 10;
    safe_increment(x);
    
    std::cout << "\n";
}

// ===== 6. SFINAE到Concepts的迁移 =====
void demonstrate_migration_strategy() {
    std::cout << "=== SFINAE到Concepts的迁移 ===\n";
    
    // 迁移前：复杂的SFINAE代码
    namespace sfinae_version {
        template<typename T, typename = void>
        struct is_container : std::false_type {};
        
        template<typename T>
        struct is_container<T, std::void_t<
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()),
            typename T::value_type
        >> : std::true_type {};
        
        template<typename T>
        std::enable_if_t<std::is_arithmetic_v<T>, void>
        print_info(const T& value) {
            std::cout << "SFINAE: Arithmetic value: " << value << "\n";
        }
        
        template<typename T>
        std::enable_if_t<is_container<T>::value && !std::is_arithmetic_v<T>, void>
        print_info(const T& container) {
            std::cout << "SFINAE: Container with " << container.size() << " elements\n";
        }
        
        template<typename T>
        std::enable_if_t<!std::is_arithmetic_v<T> && !is_container<T>::value, void>
        print_info(const T& other) {
            std::cout << "SFINAE: Other type (size: " << sizeof(T) << " bytes)\n";
        }
    }
    
    // 迁移后：清晰的Concepts代码
    namespace concepts_version {
        template<typename T>
        concept Container = requires(T t) {
            t.begin();
            t.end();
            t.size();
            typename T::value_type;
        };
        
        template<std::integral T>
        void print_info(const T& value) {
            std::cout << "Concepts: Integral value: " << value << "\n";
        }
        
        template<std::floating_point T>
        void print_info(const T& value) {
            std::cout << "Concepts: Floating point value: " << value << "\n";
        }
        
        template<Container T>
        void print_info(const T& container) {
            std::cout << "Concepts: Container with " << container.size() << " elements\n";
        }
        
        template<typename T>
        void print_info(const T& other) {
            std::cout << "Concepts: Other type (size: " << sizeof(T) << " bytes)\n";
        }
    }
    
    // 渐进式迁移策略
    namespace hybrid_version {
        // 保持SFINAE检测，但使用concepts约束
        template<typename T, typename = void>
        struct has_legacy_interface : std::false_type {};
        
        template<typename T>
        struct has_legacy_interface<T, std::void_t<decltype(std::declval<T>().legacy_method())>> 
            : std::true_type {};
        
        template<typename T>
        concept ModernInterface = requires(T t) {
            t.modern_method();
        };
        
        template<typename T>
        void handle_object(const T& obj) {
            if constexpr (ModernInterface<T>) {
                std::cout << "Using modern interface\n";
                // obj.modern_method();
            } else if constexpr (has_legacy_interface<T>::value) {
                std::cout << "Using legacy interface\n";
                // obj.legacy_method();
            } else {
                std::cout << "Fallback handling\n";
            }
        }
    }
    
    // 测试迁移效果
    std::cout << "\n--- SFINAE版本 ---\n";
    sfinae_version::print_info(42);
    sfinae_version::print_info(std::vector<int>{1, 2, 3});
    sfinae_version::print_info(std::string{"hello"});
    
    std::cout << "\n--- Concepts版本 ---\n";
    concepts_version::print_info(42);
    concepts_version::print_info(3.14);
    concepts_version::print_info(std::vector<int>{1, 2, 3});
    concepts_version::print_info(std::string{"hello"});
    
    std::cout << "\n--- 混合版本 ---\n";
    hybrid_version::handle_object(42);
    hybrid_version::handle_object(std::vector<int>{});
    
    std::cout << "\n";
}

// ===== 7. 性能和错误诊断对比 =====
void demonstrate_performance_and_diagnostics() {
    std::cout << "=== 性能和错误诊断对比 ===\n";
    
    // 编译期性能测试结构
    template<int N>
    struct CompileTimeTest {
        // SFINAE版本的递归深度测试
        template<typename T, typename = void>
        struct sfinae_depth : std::integral_constant<int, 0> {};
        
        template<typename T>
        struct sfinae_depth<T, std::enable_if_t<(N > 0)>> 
            : std::integral_constant<int, 1 + CompileTimeTest<N-1>::template sfinae_depth<T>::value> {};
        
        // Concepts版本
        template<typename T>
        static constexpr int concepts_depth() {
            if constexpr (N > 0) {
                return 1 + CompileTimeTest<N-1>::template concepts_depth<T>();
            } else {
                return 0;
            }
        }
    };
    
    // 错误信息质量对比
    namespace error_quality_demo {
        // SFINAE: 错误信息冗长且难理解
        template<typename T>
        std::enable_if_t<
            std::is_arithmetic_v<T> && 
            std::is_copy_constructible_v<T> &&
            std::is_default_constructible_v<T> &&
            sizeof(T) >= 4,
        void> complex_sfinae_function(T value) {
            std::cout << "Complex SFINAE function: " << value << "\n";
        }
        
        // Concepts: 错误信息清晰且有意义
        template<typename T>
        concept ComplexRequirement = std::is_arithmetic_v<T> && 
                                   std::is_copy_constructible_v<T> &&
                                   std::is_default_constructible_v<T> &&
                                   sizeof(T) >= 4;
        
        template<ComplexRequirement T>
        void complex_concepts_function(T value) {
            std::cout << "Complex Concepts function: " << value << "\n";
        }
    }
    
    // 测试编译期性能
    constexpr int sfinae_result = CompileTimeTest<5>::sfinae_depth<int>::value;
    constexpr int concepts_result = CompileTimeTest<5>::concepts_depth<int>();
    
    std::cout << "SFINAE depth result: " << sfinae_result << "\n";
    std::cout << "Concepts depth result: " << concepts_result << "\n";
    
    // 测试错误质量（这些会编译成功）
    error_quality_demo::complex_sfinae_function(42);
    error_quality_demo::complex_concepts_function(42);
    
    // 编译器优化友好性对比
    std::cout << "\nConcepts advantages:\n";
    std::cout << "1. Better compile-time performance\n";
    std::cout << "2. Clearer error messages\n"; 
    std::cout << "3. More intuitive syntax\n";
    std::cout << "4. Better IDE support\n";
    std::cout << "5. Standardized constraint system\n";
    
    std::cout << "\nSFINAE still useful for:\n";
    std::cout << "1. Detection idioms in libraries\n";
    std::cout << "2. Backwards compatibility\n";
    std::cout << "3. Complex metaprogramming\n";
    std::cout << "4. Template specialization scenarios\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "第4章：从SFINAE到Concepts的演进深度解析\n";
    std::cout << "==========================================\n";
    
    demonstrate_sfinae_basics();
    demonstrate_function_sfinae();
    demonstrate_detection_idiom();
    demonstrate_concepts_basics();
    demonstrate_advanced_concepts();
    demonstrate_migration_strategy();
    demonstrate_performance_and_diagnostics();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall -Wextra 04_sfinae_concepts.cpp -o sfinae_concepts
./sfinae_concepts

关键学习点:
1. SFINAE基于"替换失败不是错误"原理，实现编译期条件编译
2. Detection Idiom是现代SFINAE的标准模式，提供统一的检测框架
3. Concepts提供更清晰、更高效的约束系统
4. 从SFINAE迁移到Concepts需要渐进式策略
5. Concepts在编译性能和错误诊断上都有显著优势

技术演进:
- SFINAE (C++98-17): 基于替换失败的隐式约束
- Detection Idiom (C++17): 标准化的SFINAE模式  
- Concepts (C++20+): 显式约束系统

最佳实践:
- 新代码优先使用Concepts
- 库代码可能需要同时支持SFINAE和Concepts
- 复杂的类型检测仍然可能需要SFINAE
- 使用concepts改善模板错误诊断

注意事项:
- Concepts不能完全替代所有SFINAE用法
- 理解两种技术的适用场景
- 注意concepts的编译器支持情况
- 合理平衡代码复杂度和功能需求
*/