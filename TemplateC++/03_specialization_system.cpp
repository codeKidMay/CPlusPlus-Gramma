/**
 * 第3章：特化体系的设计哲学深度解析
 * 
 * 核心概念：
 * 1. 特化层次 - 全特化、偏特化和函数重载的关系
 * 2. 匹配优先级 - 最特化匹配原则和歧义解决
 * 3. 标签分发 - 编译期分支的经典技术
 * 4. 特化vs重载 - 选择正确的技术方案
 * 5. 高级特化 - 模式匹配和条件特化技术
 */

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <iterator>
#include <memory>
#include <chrono>
#include <algorithm>
#include <numeric>

// ===== 1. 基础特化概念和语法 =====
void demonstrate_basic_specialization() {
    std::cout << "=== 基础特化概念和语法 ===\n";
    
    // 主模板
    template<typename T>
    struct TypeTraits {
        static constexpr const char* name = "unknown type";
        static constexpr bool is_builtin = false;
        static constexpr size_t typical_size = sizeof(T);
        
        static void print_info() {
            std::cout << "Generic type: " << name 
                      << ", size: " << typical_size 
                      << ", builtin: " << is_builtin << "\n";
        }
    };
    
    // 全特化（完全特化）
    template<>
    struct TypeTraits<int> {
        static constexpr const char* name = "integer";
        static constexpr bool is_builtin = true;
        static constexpr size_t typical_size = sizeof(int);
        
        static void print_info() {
            std::cout << "Specialized for int: " << name 
                      << ", size: " << typical_size 
                      << ", builtin: " << is_builtin << "\n";
        }
    };
    
    template<>
    struct TypeTraits<std::string> {
        static constexpr const char* name = "string";
        static constexpr bool is_builtin = false;
        static constexpr size_t typical_size = sizeof(std::string);
        
        static void print_info() {
            std::cout << "Specialized for string: " << name 
                      << ", size: " << typical_size 
                      << ", builtin: " << is_builtin << "\n";
        }
    };
    
    // 偏特化（部分特化）- 只适用于类模板
    template<typename T>
    struct TypeTraits<T*> {
        static constexpr const char* name = "pointer type";
        static constexpr bool is_builtin = true;
        static constexpr size_t typical_size = sizeof(T*);
        
        static void print_info() {
            std::cout << "Specialized for pointer: " << name 
                      << ", size: " << typical_size 
                      << ", builtin: " << is_builtin << "\n";
        }
    };
    
    template<typename T>
    struct TypeTraits<std::vector<T>> {
        static constexpr const char* name = "vector type";
        static constexpr bool is_builtin = false;
        static constexpr size_t typical_size = sizeof(std::vector<T>);
        
        static void print_info() {
            std::cout << "Specialized for vector: " << name 
                      << ", size: " << typical_size 
                      << ", builtin: " << is_builtin << "\n";
        }
    };
    
    // 测试各种特化
    TypeTraits<double>::print_info();          // 主模板
    TypeTraits<int>::print_info();             // 全特化
    TypeTraits<std::string>::print_info();     // 全特化
    TypeTraits<int*>::print_info();            // 偏特化（指针）
    TypeTraits<std::vector<int>>::print_info(); // 偏特化（vector）
    
    std::cout << "\n";
}

// ===== 2. 函数模板的重载解析 =====
void demonstrate_function_template_overloading() {
    std::cout << "=== 函数模板的重载解析 ===\n";
    
    // 函数模板不能偏特化，但可以重载
    
    // 主函数模板
    template<typename T>
    void process(T value) {
        std::cout << "Generic process: " << value << " (type: " << typeid(T).name() << ")\n";
    }
    
    // 函数模板重载
    template<typename T>
    void process(T* ptr) {
        std::cout << "Pointer process: " << (ptr ? *ptr : T{}) << " (pointer type)\n";
    }
    
    template<typename T>
    void process(const T& value) {
        std::cout << "Const reference process: " << value << " (const ref)\n";
    }
    
    // 更特化的重载
    template<typename T, typename U>
    void process(const std::pair<T, U>& p) {
        std::cout << "Pair process: (" << p.first << ", " << p.second << ")\n";
    }
    
    // 函数模板全特化
    template<>
    void process<std::string>(std::string value) {
        std::cout << "Specialized for string: \"" << value << "\"\n";
    }
    
    // 普通函数（优先级最高）
    void process(int value) {
        std::cout << "Non-template function for int: " << value << "\n";
    }
    
    // 测试重载解析
    process(42);                    // 普通函数
    process(3.14);                  // 主模板
    process(std::string{"hello"});  // 特化版本
    
    int x = 100;
    process(&x);                    // 指针重载
    process(std::make_pair(1, 2.5)); // pair重载
    
    const int cx = 200;
    process(cx);                    // const引用重载
    
    std::cout << "\n";
}

// ===== 3. 最特化匹配原则深度解析 =====
void demonstrate_most_specialized_matching() {
    std::cout << "=== 最特化匹配原则深度解析 ===\n";
    
    // 演示特化匹配的复杂情况
    template<typename T>
    struct Matcher {
        static void match() {
            std::cout << "Primary template: T\n";
        }
    };
    
    // 偏特化1：指针类型
    template<typename T>
    struct Matcher<T*> {
        static void match() {
            std::cout << "Partial specialization: T*\n";
        }
    };
    
    // 偏特化2：const类型
    template<typename T>
    struct Matcher<const T> {
        static void match() {
            std::cout << "Partial specialization: const T\n";
        }
    };
    
    // 偏特化3：const指针类型（更特化）
    template<typename T>
    struct Matcher<const T*> {
        static void match() {
            std::cout << "Partial specialization: const T* (most specialized)\n";
        }
    };
    
    // 偏特化4：引用类型
    template<typename T>
    struct Matcher<T&> {
        static void match() {
            std::cout << "Partial specialization: T&\n";
        }
    };
    
    // 全特化
    template<>
    struct Matcher<int> {
        static void match() {
            std::cout << "Full specialization: int\n";
        }
    };
    
    std::cout << "\n--- 匹配优先级测试 ---\n";
    Matcher<double>::match();        // 主模板
    Matcher<int>::match();           // 全特化
    Matcher<int*>::match();          // 偏特化 T*
    Matcher<const int>::match();     // 偏特化 const T
    Matcher<const int*>::match();    // 偏特化 const T*（最特化）
    Matcher<int&>::match();          // 偏特化 T&
    
    // 复杂匹配示例：多参数模板
    template<typename T, typename U>
    struct ComplexMatcher {
        static void match() {
            std::cout << "Primary: T, U\n";
        }
    };
    
    template<typename T>
    struct ComplexMatcher<T, T> {
        static void match() {
            std::cout << "Partial: T, T (same type)\n";
        }
    };
    
    template<typename T>
    struct ComplexMatcher<T*, int> {
        static void match() {
            std::cout << "Partial: T*, int\n";
        }
    };
    
    template<>
    struct ComplexMatcher<int, int> {
        static void match() {
            std::cout << "Full: int, int\n";
        }
    };
    
    std::cout << "\n--- 复杂匹配测试 ---\n";
    ComplexMatcher<double, std::string>::match(); // 主模板
    ComplexMatcher<float, float>::match();        // 偏特化：相同类型
    ComplexMatcher<double*, int>::match();        // 偏特化：T*, int
    ComplexMatcher<int, int>::match();            // 全特化
    
    std::cout << "\n";
}

// ===== 4. 标签分发技术 =====
void demonstrate_tag_dispatch() {
    std::cout << "=== 标签分发技术 ===\n";
    
    // 标签类型定义
    struct input_iterator_tag {};
    struct forward_iterator_tag : input_iterator_tag {};
    struct bidirectional_iterator_tag : forward_iterator_tag {};
    struct random_access_iterator_tag : bidirectional_iterator_tag {};
    
    // 迭代器特征提取
    template<typename Iterator>
    struct iterator_traits {
        using iterator_category = typename Iterator::iterator_category;
    };
    
    // 针对指针的特化
    template<typename T>
    struct iterator_traits<T*> {
        using iterator_category = random_access_iterator_tag;
    };
    
    // advance函数的标签分发实现
    template<typename Iterator, typename Distance>
    void advance_impl(Iterator& it, Distance n, input_iterator_tag) {
        std::cout << "Advancing input iterator step by step\n";
        while (n--) ++it;
    }
    
    template<typename Iterator, typename Distance>
    void advance_impl(Iterator& it, Distance n, bidirectional_iterator_tag) {
        std::cout << "Advancing bidirectional iterator (can go backward)\n";
        if (n >= 0) {
            while (n--) ++it;
        } else {
            while (n++) --it;
        }
    }
    
    template<typename Iterator, typename Distance>
    void advance_impl(Iterator& it, Distance n, random_access_iterator_tag) {
        std::cout << "Advancing random access iterator in one step\n";
        it += n;
    }
    
    // 主函数：自动选择正确的实现
    template<typename Iterator, typename Distance>
    void my_advance(Iterator& it, Distance n) {
        using category = typename iterator_traits<Iterator>::iterator_category;
        advance_impl(it, n, category{});
    }
    
    // 测试标签分发
    std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vec_it = vec.begin();
    my_advance(vec_it, 3);  // 使用random_access版本
    std::cout << "Vector iterator value: " << *vec_it << "\n";
    
    std::list<int> lst{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto lst_it = lst.begin();
    my_advance(lst_it, 3);  // 使用bidirectional版本
    std::cout << "List iterator value: " << *lst_it << "\n";
    
    // 现代版本：if constexpr替代标签分发
    template<typename Iterator, typename Distance>
    void modern_advance(Iterator& it, Distance n) {
        using category = typename iterator_traits<Iterator>::iterator_category;
        
        if constexpr (std::is_same_v<category, random_access_iterator_tag>) {
            std::cout << "Modern: random access advance\n";
            it += n;
        } else if constexpr (std::is_base_of_v<bidirectional_iterator_tag, category>) {
            std::cout << "Modern: bidirectional advance\n";
            if (n >= 0) {
                while (n--) ++it;
            } else {
                while (n++) --it;
            }
        } else {
            std::cout << "Modern: input iterator advance\n";
            while (n--) ++it;
        }
    }
    
    auto modern_it = vec.begin();
    modern_advance(modern_it, 2);
    std::cout << "Modern advance result: " << *modern_it << "\n";
    
    std::cout << "\n";
}

// ===== 5. 特化用于性能优化 =====
void demonstrate_specialization_for_optimization() {
    std::cout << "=== 特化用于性能优化 ===\n";
    
    // 通用copy算法
    template<typename InputIt, typename OutputIt>
    OutputIt my_copy(InputIt first, InputIt last, OutputIt d_first) {
        std::cout << "Generic copy: element by element\n";
        while (first != last) {
            *d_first++ = *first++;
        }
        return d_first;
    }
    
    // 针对trivially copyable类型的优化特化
    template<typename T>
    T* my_copy(const T* first, const T* last, T* d_first) {
        static_assert(std::is_trivially_copyable_v<T>, 
                     "This specialization requires trivially copyable types");
        
        std::cout << "Optimized copy: using memmove for trivial types\n";
        const size_t count = last - first;
        if (count > 0) {
            std::memmove(d_first, first, count * sizeof(T));
        }
        return d_first + count;
    }
    
    // 性能测试
    constexpr size_t SIZE = 1000000;
    std::vector<int> source(SIZE);
    std::iota(source.begin(), source.end(), 1);
    
    std::vector<int> dest1(SIZE);
    std::vector<int> dest2(SIZE);
    
    // 测试通用版本
    auto start = std::chrono::high_resolution_clock::now();
    my_copy(source.begin(), source.end(), dest1.begin());
    auto end = std::chrono::high_resolution_clock::now();
    auto generic_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 测试优化版本
    start = std::chrono::high_resolution_clock::now();
    my_copy(source.data(), source.data() + source.size(), dest2.data());
    end = std::chrono::high_resolution_clock::now();
    auto optimized_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Generic copy time: " << generic_time.count() << " microseconds\n";
    std::cout << "Optimized copy time: " << optimized_time.count() << " microseconds\n";
    std::cout << "Speedup: " << (double)generic_time.count() / optimized_time.count() << "x\n";
    
    std::cout << "\n";
}

// ===== 6. 条件特化和SFINAE集成 =====
void demonstrate_conditional_specialization() {
    std::cout << "=== 条件特化和SFINAE集成 ===\n";
    
    // 基于类型特征的条件特化
    template<typename T, typename Enable = void>
    struct Serializer {
        static void serialize(const T& value) {
            std::cout << "Generic serialization for unknown type\n";
        }
    };
    
    // 针对算术类型的特化
    template<typename T>
    struct Serializer<T, std::enable_if_t<std::is_arithmetic_v<T>>> {
        static void serialize(const T& value) {
            std::cout << "Arithmetic serialization: " << value << "\n";
        }
    };
    
    // 针对有begin/end的容器类型的特化
    template<typename T>
    struct Serializer<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                     decltype(std::end(std::declval<T>()))>> {
        static void serialize(const T& container) {
            std::cout << "Container serialization: [";
            bool first = true;
            for (const auto& item : container) {
                if (!first) std::cout << ", ";
                std::cout << item;
                first = false;
            }
            std::cout << "]\n";
        }
    };
    
    // 针对有first和second成员的类型（如pair）的特化
    template<typename T>
    struct Serializer<T, std::void_t<decltype(std::declval<T>().first),
                                     decltype(std::declval<T>().second)>> {
        static void serialize(const T& pair) {
            std::cout << "Pair serialization: (" << pair.first << ", " << pair.second << ")\n";
        }
    };
    
    // 现代C++20版本：使用concepts
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;
    
    template<typename T>
    concept Container = requires(T t) {
        std::begin(t);
        std::end(t);
    };
    
    template<typename T>
    concept PairLike = requires(T t) {
        t.first;
        t.second;
    };
    
    template<typename T>
    void modern_serialize(const T& value) {
        if constexpr (PairLike<T>) {
            std::cout << "Modern pair: (" << value.first << ", " << value.second << ")\n";
        } else if constexpr (Container<T>) {
            std::cout << "Modern container: [";
            bool first = true;
            for (const auto& item : value) {
                if (!first) std::cout << ", ";
                std::cout << item;
                first = false;
            }
            std::cout << "]\n";
        } else if constexpr (Arithmetic<T>) {
            std::cout << "Modern arithmetic: " << value << "\n";
        } else {
            std::cout << "Modern generic for unknown type\n";
        }
    }
    
    // 测试条件特化
    Serializer<int>::serialize(42);
    Serializer<std::vector<int>>::serialize({1, 2, 3, 4, 5});
    Serializer<std::pair<int, std::string>>::serialize({1, "hello"});
    
    std::cout << "\n--- 现代版本 ---\n";
    modern_serialize(42);
    modern_serialize(std::vector<int>{1, 2, 3, 4, 5});
    modern_serialize(std::make_pair(1, "hello"));
    
    std::cout << "\n";
}

// ===== 7. 高级特化模式 =====
void demonstrate_advanced_specialization_patterns() {
    std::cout << "=== 高级特化模式 ===\n";
    
    // 模式1：递归特化（类型列表处理）
    template<typename... Types>
    struct TypeList {};
    
    template<typename List>
    struct ListSize;
    
    template<typename... Types>
    struct ListSize<TypeList<Types...>> {
        static constexpr size_t value = sizeof...(Types);
    };
    
    template<typename List, size_t Index>
    struct ListAt;
    
    template<typename Head, typename... Tail>
    struct ListAt<TypeList<Head, Tail...>, 0> {
        using type = Head;
    };
    
    template<typename Head, typename... Tail, size_t Index>
    struct ListAt<TypeList<Head, Tail...>, Index> {
        using type = typename ListAt<TypeList<Tail...>, Index - 1>::type;
    };
    
    // 模式2：值特化（编译期计算）
    template<int N>
    struct Fibonacci {
        static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
    };
    
    template<>
    struct Fibonacci<0> {
        static constexpr int value = 0;
    };
    
    template<>
    struct Fibonacci<1> {
        static constexpr int value = 1;
    };
    
    // 模式3：模板模板参数特化
    template<template<typename> class Container, typename T>
    struct ContainerTraits {
        static constexpr const char* name = "unknown container";
    };
    
    template<typename T>
    struct ContainerTraits<std::vector, T> {
        static constexpr const char* name = "vector";
        static constexpr bool is_contiguous = true;
        static constexpr bool is_resizable = true;
    };
    
    // 测试高级模式
    using MyList = TypeList<int, double, std::string>;
    std::cout << "TypeList size: " << ListSize<MyList>::value << "\n";
    std::cout << "Element 0 size: " << sizeof(ListAt<MyList, 0>::type) << "\n";
    std::cout << "Element 2 size: " << sizeof(ListAt<MyList, 2>::type) << "\n";
    
    std::cout << "Fibonacci<10>: " << Fibonacci<10>::value << "\n";
    
    std::cout << "Vector traits: " << ContainerTraits<std::vector, int>::name << "\n";
    
    // 模式4：CRTP与特化结合
    template<typename Derived>
    class Base {
    public:
        void interface() {
            static_cast<Derived*>(this)->implementation();
        }
        
        void common_functionality() {
            std::cout << "Common functionality in base\n";
        }
    };
    
    class ConcreteA : public Base<ConcreteA> {
    public:
        void implementation() {
            std::cout << "ConcreteA implementation\n";
        }
    };
    
    class ConcreteB : public Base<ConcreteB> {
    public:
        void implementation() {
            std::cout << "ConcreteB implementation\n";
        }
    };
    
    // 针对具体派生类的特化
    template<typename Derived>
    struct DerivedTraits {
        static constexpr const char* name = "unknown derived";
    };
    
    template<>
    struct DerivedTraits<ConcreteA> {
        static constexpr const char* name = "ConcreteA";
    };
    
    template<>
    struct DerivedTraits<ConcreteB> {
        static constexpr const char* name = "ConcreteB";
    };
    
    ConcreteA a;
    ConcreteB b;
    
    a.interface();
    b.interface();
    
    std::cout << "DerivedA name: " << DerivedTraits<ConcreteA>::name << "\n";
    std::cout << "DerivedB name: " << DerivedTraits<ConcreteB>::name << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "第3章：特化体系的设计哲学深度解析\n";
    std::cout << "===================================\n";
    
    demonstrate_basic_specialization();
    demonstrate_function_template_overloading();
    demonstrate_most_specialized_matching();
    demonstrate_tag_dispatch();
    demonstrate_specialization_for_optimization();
    demonstrate_conditional_specialization();
    demonstrate_advanced_specialization_patterns();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall -Wextra 03_specialization_system.cpp -o specialization_system
./specialization_system

关键学习点:
1. 理解全特化、偏特化和函数重载的区别和应用场景
2. 掌握最特化匹配原则，能预测编译器的选择
3. 标签分发是实现编译期分支的经典技术
4. 特化是性能优化的重要手段，特别是对trivial类型
5. 结合SFINAE和concepts实现条件特化

高级应用:
- 类型列表和编译期数据结构操作
- 递归模板实例化的控制
- 模板模板参数的特化技巧
- CRTP与特化的结合使用

设计原则:
- 特化应该保持接口一致性
- 使用特化实现零开销抽象
- 避免过度特化导致的代码膨胀
- 优先使用标准库的特化版本

注意事项:
- 特化匹配的歧义问题
- 模板实例化顺序对特化的影响
- ODR规则在特化中的应用
- 编译器对特化的优化能力差异
*/