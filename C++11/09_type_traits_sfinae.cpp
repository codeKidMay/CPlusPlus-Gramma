/**
 * C++11 类型特征与SFINAE深度解析
 * 
 * 核心概念：
 * 1. SFINAE (Substitution Failure Is Not An Error) 原理
 * 2. 标准库type_traits的实现机制
 * 3. enable_if的工作原理和应用模式
 * 4. 自定义类型检测器的设计
 * 5. 模板特化与SFINAE的结合使用
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include <utility>

// ===== 1. SFINAE基本原理演示 =====

// SFINAE的基本工作原理：替换失败不是错误
template<typename T>
class SFINAEDemo {
public:
    // 这个函数只有当T有size()方法时才存在
    template<typename U = T>
    static auto has_size_method(U* u) -> decltype(u->size(), std::true_type{});
    
    // 备选方案：当上面的函数不可用时使用这个
    static std::false_type has_size_method(...);
    
    using has_size = decltype(has_size_method(static_cast<T*>(nullptr)));
    
public:
    static constexpr bool value = has_size::value;
};

// 演示不同类型的SFINAE检测
void demonstrate_sfinae_basics() {
    std::cout << "=== SFINAE基本原理演示 ===\n";
    
    // 测试各种类型
    std::cout << "std::vector<int> 有size方法: " << SFINAEDemo<std::vector<int>>::value << std::endl;
    std::cout << "std::string 有size方法: " << SFINAEDemo<std::string>::value << std::endl;
    std::cout << "int 有size方法: " << SFINAEDemo<int>::value << std::endl;
    std::cout << "double 有size方法: " << SFINAEDemo<double>::value << std::endl;
    
    // 展示SFINAE的编译期特性
    if constexpr (SFINAEDemo<std::vector<int>>::value) {
        std::cout << "编译期确定vector有size方法" << std::endl;
    }
    
    std::cout << "\n";
}

// ===== 2. 标准库type_traits深入分析 =====

// 自定义实现一些基础type_traits来理解原理

// 实现is_same
template<typename T, typename U>
struct my_is_same : std::false_type {};

template<typename T>
struct my_is_same<T, T> : std::true_type {};

// 实现remove_const
template<typename T>
struct my_remove_const {
    using type = T;
};

template<typename T>
struct my_remove_const<const T> {
    using type = T;
};

// 实现remove_reference
template<typename T>
struct my_remove_reference {
    using type = T;
};

template<typename T>
struct my_remove_reference<T&> {
    using type = T;
};

template<typename T>
struct my_remove_reference<T&&> {
    using type = T;
};

// 实现is_pointer
template<typename T>
struct my_is_pointer : std::false_type {};

template<typename T>
struct my_is_pointer<T*> : std::true_type {};

// 实现is_array
template<typename T>
struct my_is_array : std::false_type {};

template<typename T>
struct my_is_array<T[]> : std::true_type {};

template<typename T, size_t N>
struct my_is_array<T[N]> : std::true_type {};

// 复合类型特征：decay的简化实现
template<typename T>
struct my_decay {
private:
    using U = typename my_remove_reference<T>::type;
public:
    using type = typename std::conditional<
        my_is_array<U>::value,
        typename std::remove_extent<U>::type*,
        typename std::conditional<
            std::is_function<U>::value,
            typename std::add_pointer<U>::type,
            typename my_remove_const<U>::type
        >::type
    >::type;
};

void demonstrate_type_traits_implementation() {
    std::cout << "=== 类型特征实现原理演示 ===\n";
    
    // 测试自定义type_traits
    std::cout << "my_is_same<int, int>: " << my_is_same<int, int>::value << std::endl;
    std::cout << "my_is_same<int, double>: " << my_is_same<int, double>::value << std::endl;
    
    std::cout << "my_is_pointer<int*>: " << my_is_pointer<int*>::value << std::endl;
    std::cout << "my_is_pointer<int>: " << my_is_pointer<int>::value << std::endl;
    
    std::cout << "my_is_array<int[]>: " << my_is_array<int[]>::value << std::endl;
    std::cout << "my_is_array<int[10]>: " << my_is_array<int[10]>::value << std::endl;
    std::cout << "my_is_array<int>: " << my_is_array<int>::value << std::endl;
    
    // 类型转换特征
    using const_int = const int;
    using int_ref = int&;
    using int_rref = int&&;
    
    std::cout << "\n类型转换特征:\n";
    std::cout << "remove_const<const int> == int: " << 
        my_is_same<typename my_remove_const<const_int>::type, int>::value << std::endl;
    std::cout << "remove_reference<int&> == int: " << 
        my_is_same<typename my_remove_reference<int_ref>::type, int>::value << std::endl;
    std::cout << "remove_reference<int&&> == int: " << 
        my_is_same<typename my_remove_reference<int_rref>::type, int>::value << std::endl;
    
    // 对比标准库实现
    std::cout << "\n与标准库对比:\n";
    std::cout << "std::is_same<int, int>: " << std::is_same<int, int>::value << std::endl;
    std::cout << "std::is_pointer<int*>: " << std::is_pointer<int*>::value << std::endl;
    std::cout << "std::is_array<int[10]>: " << std::is_array<int[10]>::value << std::endl;
    
    std::cout << "\n";
}

// ===== 3. enable_if的深度应用 =====

// enable_if的基本实现原理
template<bool Condition, typename T = void>
struct my_enable_if {};

template<typename T>
struct my_enable_if<true, T> {
    using type = T;
};

// 使用enable_if进行函数重载
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
process_number(T value) {
    std::cout << "处理整数: " << value << " (类型大小: " << sizeof(T) << ")" << std::endl;
    return value * 2;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
process_number(T value) {
    std::cout << "处理浮点数: " << value << " (类型大小: " << sizeof(T) << ")" << std::endl;
    return value * 1.5;
}

template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value, T>::type
process_number(T value) {
    std::cout << "处理字符串: " << value << std::endl;
    return value + "_processed";
}

// enable_if作为模板参数
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
class ArithmeticWrapper {
private:
    T value_;
    
public:
    explicit ArithmeticWrapper(T val) : value_(val) {
        std::cout << "创建算术类型包装器，值: " << val << std::endl;
    }
    
    T get() const { return value_; }
    
    ArithmeticWrapper operator+(const ArithmeticWrapper& other) const {
        return ArithmeticWrapper(value_ + other.value_);
    }
};

// enable_if作为返回类型
template<typename Container>
auto get_size_if_has_size(const Container& container) 
    -> typename std::enable_if<
        std::is_same<decltype(container.size()), size_t>::value, 
        size_t
    >::type {
    return container.size();
}

template<typename T>
auto get_size_if_has_size(const T&) 
    -> typename std::enable_if<
        !std::is_same<decltype(std::declval<T>().size()), size_t>::value, 
        size_t
    >::type {
    return 0;  // 默认大小
}

void demonstrate_enable_if_applications() {
    std::cout << "=== enable_if深度应用演示 ===\n";
    
    std::cout << "1. 基于类型的函数重载:\n";
    
    auto int_result = process_number(42);
    auto double_result = process_number(3.14);
    auto string_result = process_number(std::string("hello"));
    
    std::cout << "结果: " << int_result << ", " << double_result << ", " << string_result << std::endl;
    
    std::cout << "\n2. enable_if约束的模板类:\n";
    
    ArithmeticWrapper<int> int_wrapper(100);
    ArithmeticWrapper<double> double_wrapper(2.5);
    // ArithmeticWrapper<std::string> string_wrapper("test");  // 编译错误！
    
    auto sum_wrapper = int_wrapper + ArithmeticWrapper<int>(200);
    std::cout << "包装器相加结果: " << sum_wrapper.get() << std::endl;
    
    std::cout << "\n3. 条件性函数调用:\n";
    
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::string str = "hello";
    int number = 42;
    
    std::cout << "vector大小: " << get_size_if_has_size(vec) << std::endl;
    std::cout << "string大小: " << get_size_if_has_size(str) << std::endl;
    std::cout << "int大小: " << get_size_if_has_size(number) << std::endl;
    
    std::cout << "\n";
}

// ===== 4. 高级SFINAE技巧 =====

// 检测是否有特定成员函数
#define DEFINE_HAS_MEMBER(member_name) \
template<typename T> \
class has_##member_name { \
private: \
    template<typename U> \
    static auto test(U* u) -> decltype(u->member_name(), std::true_type{}); \
    template<typename> \
    static std::false_type test(...); \
public: \
    static constexpr bool value = decltype(test<T>(nullptr))::value; \
}

// 生成各种成员检测器
DEFINE_HAS_MEMBER(size);
DEFINE_HAS_MEMBER(push_back);
DEFINE_HAS_MEMBER(begin);
DEFINE_HAS_MEMBER(end);
DEFINE_HAS_MEMBER(clear);

// 检测是否可以进行特定操作
template<typename T>
class is_addable {
private:
    template<typename U>
    static auto test(U* u) -> decltype(*u + *u, std::true_type{});
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

template<typename T>
class is_comparable {
private:
    template<typename U>
    static auto test(U* u) -> decltype(*u < *u, *u == *u, std::true_type{});
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

// 检测是否是容器类型
template<typename T>
class is_container {
private:
    template<typename U>
    static auto test(U* u) -> decltype(
        u->begin(),
        u->end(),
        u->size(),
        typename U::value_type{},
        std::true_type{}
    );
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

// 基于检测结果的条件编译
template<typename T>
void print_container_info(const T& container) {
    std::cout << "分析类型: " << typeid(T).name() << std::endl;
    
    if constexpr (has_size<T>::value) {
        std::cout << "  有size方法，大小: " << container.size() << std::endl;
    } else {
        std::cout << "  没有size方法" << std::endl;
    }
    
    if constexpr (has_push_back<T>::value) {
        std::cout << "  有push_back方法，是可扩展容器" << std::endl;
    } else {
        std::cout << "  没有push_back方法" << std::endl;
    }
    
    if constexpr (has_begin<T>::value && has_end<T>::value) {
        std::cout << "  支持迭代器" << std::endl;
    } else {
        std::cout << "  不支持迭代器" << std::endl;
    }
    
    if constexpr (is_container<T>::value) {
        std::cout << "  被识别为标准容器类型" << std::endl;
    } else {
        std::cout << "  不是标准容器类型" << std::endl;
    }
}

void demonstrate_advanced_sfinae() {
    std::cout << "=== 高级SFINAE技巧演示 ===\n";
    
    std::cout << "1. 成员函数检测结果:\n";
    std::cout << "vector有size: " << has_size<std::vector<int>>::value << std::endl;
    std::cout << "vector有push_back: " << has_push_back<std::vector<int>>::value << std::endl;
    std::cout << "string有size: " << has_size<std::string>::value << std::endl;
    std::cout << "string有push_back: " << has_push_back<std::string>::value << std::endl;
    std::cout << "int有size: " << has_size<int>::value << std::endl;
    
    std::cout << "\n2. 操作能力检测:\n";
    std::cout << "int可加法: " << is_addable<int>::value << std::endl;
    std::cout << "string可加法: " << is_addable<std::string>::value << std::endl;
    std::cout << "int可比较: " << is_comparable<int>::value << std::endl;
    std::cout << "vector可比较: " << is_comparable<std::vector<int>>::value << std::endl;
    
    std::cout << "\n3. 容器类型检测:\n";
    std::cout << "vector是容器: " << is_container<std::vector<int>>::value << std::endl;
    std::cout << "string是容器: " << is_container<std::string>::value << std::endl;
    std::cout << "int是容器: " << is_container<int>::value << std::endl;
    
    std::cout << "\n4. 综合分析:\n";
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::string str = "hello";
    int num = 42;
    
    print_container_info(vec);
    std::cout << std::endl;
    print_container_info(str);
    std::cout << std::endl;
    print_container_info(num);
    
    std::cout << "\n";
}

// ===== 5. 模板特化与SFINAE结合 =====

// 通用序列化器基础模板
template<typename T, typename Enable = void>
struct Serializer {
    static std::string serialize(const T& value) {
        return "不支持序列化的类型: " + std::string(typeid(T).name());
    }
};

// 算术类型特化
template<typename T>
struct Serializer<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    static std::string serialize(const T& value) {
        return std::to_string(value);
    }
};

// 字符串类型特化
template<>
struct Serializer<std::string> {
    static std::string serialize(const std::string& value) {
        return "\"" + value + "\"";
    }
};

// 容器类型特化
template<typename T>
struct Serializer<T, typename std::enable_if<is_container<T>::value>::type> {
    static std::string serialize(const T& container) {
        std::string result = "[";
        bool first = true;
        for (const auto& item : container) {
            if (!first) result += ", ";
            result += Serializer<typename T::value_type>::serialize(item);
            first = false;
        }
        result += "]";
        return result;
    }
};

// 智能指针特化
template<typename T>
struct Serializer<std::shared_ptr<T>> {
    static std::string serialize(const std::shared_ptr<T>& ptr) {
        if (ptr) {
            return "shared_ptr{" + Serializer<T>::serialize(*ptr) + "}";
        } else {
            return "shared_ptr{null}";
        }
    }
};

template<typename T>
struct Serializer<std::unique_ptr<T>> {
    static std::string serialize(const std::unique_ptr<T>& ptr) {
        if (ptr) {
            return "unique_ptr{" + Serializer<T>::serialize(*ptr) + "}";
        } else {
            return "unique_ptr{null}";
        }
    }
};

void demonstrate_specialization_with_sfinae() {
    std::cout << "=== 模板特化与SFINAE结合演示 ===\n";
    
    // 测试各种类型的序列化
    std::cout << "算术类型序列化:\n";
    std::cout << "int 42: " << Serializer<int>::serialize(42) << std::endl;
    std::cout << "double 3.14: " << Serializer<double>::serialize(3.14) << std::endl;
    std::cout << "bool true: " << Serializer<bool>::serialize(true) << std::endl;
    
    std::cout << "\n字符串序列化:\n";
    std::cout << "string: " << Serializer<std::string>::serialize("hello world") << std::endl;
    
    std::cout << "\n容器序列化:\n";
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::cout << "vector<int>: " << Serializer<std::vector<int>>::serialize(vec) << std::endl;
    
    std::vector<std::string> str_vec{"hello", "world", "test"};
    std::cout << "vector<string>: " << Serializer<std::vector<std::string>>::serialize(str_vec) << std::endl;
    
    std::cout << "\n智能指针序列化:\n";
    auto shared_ptr = std::make_shared<int>(100);
    std::cout << "shared_ptr<int>: " << Serializer<std::shared_ptr<int>>::serialize(shared_ptr) << std::endl;
    
    std::shared_ptr<int> null_shared;
    std::cout << "null shared_ptr: " << Serializer<std::shared_ptr<int>>::serialize(null_shared) << std::endl;
    
    auto unique_ptr = std::make_unique<std::string>("unique data");
    std::cout << "unique_ptr<string>: " << Serializer<std::unique_ptr<std::string>>::serialize(unique_ptr) << std::endl;
    
    std::cout << "\n不支持的类型:\n";
    struct CustomType { int x; };
    CustomType custom{42};
    std::cout << "CustomType: " << Serializer<CustomType>::serialize(custom) << std::endl;
    
    std::cout << "\n";
}

// ===== 6. 实际应用：通用算法库 =====

// 通用查找算法
template<typename Container, typename Value>
auto find_in_container(const Container& container, const Value& value)
    -> typename std::enable_if<
        has_begin<Container>::value && has_end<Container>::value,
        decltype(container.begin())
    >::type {
    
    return std::find(container.begin(), container.end(), value);
}

// 通用大小获取
template<typename T>
auto get_container_size(const T& container)
    -> typename std::enable_if<has_size<T>::value, size_t>::type {
    return container.size();
}

template<typename T>
auto get_container_size(const T& container)
    -> typename std::enable_if<!has_size<T>::value, size_t>::type {
    return std::distance(container.begin(), container.end());
}

// 条件性清空操作
template<typename T>
auto clear_if_possible(T& container)
    -> typename std::enable_if<has_clear<T>::value>::type {
    container.clear();
    std::cout << "容器已清空" << std::endl;
}

template<typename T>
auto clear_if_possible(T& container)
    -> typename std::enable_if<!has_clear<T>::value>::type {
    std::cout << "容器不支持清空操作" << std::endl;
}

// 通用输出函数
template<typename T>
void universal_print(const T& value) {
    if constexpr (std::is_arithmetic<T>::value) {
        std::cout << "数值: " << value;
    } else if constexpr (std::is_same<T, std::string>::value) {
        std::cout << "字符串: \"" << value << "\"";
    } else if constexpr (is_container<T>::value) {
        std::cout << "容器 [";
        bool first = true;
        for (const auto& item : value) {
            if (!first) std::cout << ", ";
            universal_print(item);
            first = false;
        }
        std::cout << "]";
    } else {
        std::cout << "未知类型对象";
    }
}

void demonstrate_generic_algorithms() {
    std::cout << "=== 通用算法库演示 ===\n";
    
    std::vector<int> vec{10, 20, 30, 40, 50};
    std::string str = "hello";
    
    std::cout << "1. 通用查找:\n";
    auto vec_it = find_in_container(vec, 30);
    if (vec_it != vec.end()) {
        std::cout << "在vector中找到30，位置: " << std::distance(vec.begin(), vec_it) << std::endl;
    }
    
    auto str_it = find_in_container(str, 'l');
    if (str_it != str.end()) {
        std::cout << "在string中找到'l'，位置: " << std::distance(str.begin(), str_it) << std::endl;
    }
    
    std::cout << "\n2. 通用大小获取:\n";
    std::cout << "vector大小: " << get_container_size(vec) << std::endl;
    std::cout << "string大小: " << get_container_size(str) << std::endl;
    
    std::cout << "\n3. 条件性操作:\n";
    std::vector<int> clearable_vec{1, 2, 3};
    std::cout << "清空前vector大小: " << clearable_vec.size() << std::endl;
    clear_if_possible(clearable_vec);
    std::cout << "清空后vector大小: " << clearable_vec.size() << std::endl;
    
    int not_clearable = 42;
    clear_if_possible(not_clearable);
    
    std::cout << "\n4. 通用输出:\n";
    universal_print(42);
    std::cout << std::endl;
    universal_print(std::string("hello world"));
    std::cout << std::endl;
    universal_print(vec);
    std::cout << std::endl;
    
    std::vector<std::string> str_vec{"a", "b", "c"};
    universal_print(str_vec);
    std::cout << std::endl;
    
    std::cout << "\n";
}

// ===== 7. 最佳实践和性能考虑 =====

void demonstrate_best_practices() {
    std::cout << "=== SFINAE最佳实践和性能考虑 ===\n";
    
    std::cout << "1. SFINAE最佳实践:\n";
    std::cout << "✓ 使用现代C++习惯用法(if constexpr, concepts)\n";
    std::cout << "✓ 优先使用标准库type_traits而不是自定义\n";
    std::cout << "✓ 保持SFINAE表达式简洁明了\n";
    std::cout << "✓ 使用别名模板简化复杂的enable_if表达式\n";
    std::cout << "✓ 文档化复杂的SFINAE约束\n";
    
    std::cout << "\n2. 性能考虑:\n";
    std::cout << "• type_traits检查是零运行时开销的\n";
    std::cout << "• SFINAE在编译期完成，不影响运行时性能\n";
    std::cout << "• 过度复杂的SFINAE表达式会延长编译时间\n";
    std::cout << "• 使用constexpr if替代SFINAE可以减少模板实例化\n";
    
    std::cout << "\n3. 常见陷阱:\n";
    std::cout << "⚠ 注意SFINAE的求值顺序\n";
    std::cout << "⚠ 避免循环依赖的type_traits\n";
    std::cout << "⚠ 小心ADL(Argument Dependent Lookup)的影响\n";
    std::cout << "⚠ 模板特化的优先级规则\n";
    
    std::cout << "\n4. 现代替代方案:\n";
    std::cout << "• C++14: enable_if_t别名模板\n";
    std::cout << "• C++17: if constexpr替代部分SFINAE\n";
    std::cout << "• C++20: Concepts提供更好的约束语法\n";
    
    // 演示一些最佳实践
    std::cout << "\n5. 代码风格示例:\n";
    
    // C++11风格
    std::cout << "C++11风格:\n";
    std::cout << "template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>\n";
    std::cout << "void func(T value);\n";
    
    // C++14风格（如果可用）
    std::cout << "\nC++14风格:\n";
    std::cout << "template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>\n";
    std::cout << "void func(T value);\n";
    
    // C++17风格（如果可用）
    std::cout << "\nC++17风格:\n";
    std::cout << "template<typename T>\n";
    std::cout << "void func(T value) {\n";
    std::cout << "    if constexpr (std::is_integral_v<T>) {\n";
    std::cout << "        // 处理整数\n";
    std::cout << "    }\n";
    std::cout << "}\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 类型特征与SFINAE深度解析\n";
    std::cout << "===============================\n";
    
    // SFINAE基础
    demonstrate_sfinae_basics();
    
    // 类型特征实现
    demonstrate_type_traits_implementation();
    
    // enable_if应用
    demonstrate_enable_if_applications();
    
    // 高级SFINAE技巧
    demonstrate_advanced_sfinae();
    
    // 模板特化结合
    demonstrate_specialization_with_sfinae();
    
    // 通用算法应用
    demonstrate_generic_algorithms();
    
    // 最佳实践
    demonstrate_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 09_type_traits_sfinae.cpp -o sfinae_demo
./sfinae_demo

关键学习点:
1. 深入理解SFINAE的工作原理和应用场景
2. 掌握标准库type_traits的实现机制
3. 熟练使用enable_if进行条件编译和函数重载
4. 学会设计自定义的类型检测器和约束
5. 理解模板特化与SFINAE的结合使用
6. 掌握SFINAE在泛型编程中的最佳实践
7. 了解SFINAE的性能特性和现代替代方案
8. 学会构建基于SFINAE的通用算法库

注意事项:
- SFINAE是编译期技术，不会影响运行时性能
- 过度复杂的SFINAE表达式会增加编译时间
- C++17的if constexpr和C++20的Concepts提供了更好的替代方案
- 在实际项目中要平衡代码复杂度和功能需求
*/