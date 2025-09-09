/**
 * C++11/14/17/20 type_traits库深度解析
 * 
 * 核心概念：
 * 1. 类型特性基础机制 - 编译期类型信息查询
 * 2. 类型分类特性 - 基础类型判断与分类体系
 * 3. 类型转换特性 - 类型修饰符操作与转换
 * 4. 类型关系特性 - 类型间关系判断与兼容性
 * 5. 自定义类型特性 - 元编程技巧与最佳实践
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>
#include <functional>
#include <chrono>
#include <array>

// ===== 1. 类型特性基础机制演示 =====
void demonstrate_type_traits_basics() {
    std::cout << "=== 类型特性基础机制演示 ===\n";
    
    // 1.1 基础类型特性
    std::cout << "1. 基础类型特性:\n";
    std::cout << "int是整数类型: " << std::is_integral_v<int> << "\n";
    std::cout << "double是浮点类型: " << std::is_floating_point_v<double> << "\n";
    std::cout << "char是算术类型: " << std::is_arithmetic_v<char> << "\n";
    std::cout << "std::string是算术类型: " << std::is_arithmetic_v<std::string> << "\n";
    
    // 1.2 复合类型特性
    std::cout << "\n2. 复合类型特性:\n";
    std::cout << "int*是指针: " << std::is_pointer_v<int*> << "\n";
    std::cout << "int&是引用: " << std::is_reference_v<int&> << "\n";
    std::cout << "int[]是数组: " << std::is_array_v<int[]> << "\n";
    std::cout << "void()是函数: " << std::is_function_v<void()> << "\n";
    
    // 1.3 类型属性特性
    std::cout << "\n3. 类型属性特性:\n";
    std::cout << "const int是常量: " << std::is_const_v<const int> << "\n";
    std::cout << "volatile int是易变: " << std::is_volatile_v<volatile int> << "\n";
    std::cout << "int是有符号: " << std::is_signed_v<int> << "\n";
    std::cout << "unsigned int是无符号: " << std::is_unsigned_v<unsigned int> << "\n";
    
    // 1.4 类型大小和对齐
    std::cout << "\n4. 类型大小和对齐:\n";
    std::cout << "int大小: " << sizeof(int) << " bytes\n";
    std::cout << "double对齐: " << alignof(double) << " bytes\n";
    std::cout << "指针大小: " << sizeof(void*) << " bytes\n";
    
    // 1.5 编译期条件判断
    std::cout << "\n5. 编译期条件判断:\n";
    
    auto type_info = []<typename T>() {
        std::cout << "类型 " << typeid(T).name() << ": ";
        
        if constexpr (std::is_integral_v<T>) {
            std::cout << "整数类型";
        } else if constexpr (std::is_floating_point_v<T>) {
            std::cout << "浮点类型";
        } else if constexpr (std::is_pointer_v<T>) {
            std::cout << "指针类型";
        } else {
            std::cout << "其他类型";
        }
        
        std::cout << " (大小: " << sizeof(T) << " bytes)\n";
    };
    
    type_info.template operator()<int>();
    type_info.template operator()<double>();
    type_info.template operator()<int*>();
    type_info.template operator()<std::string>();
    
    std::cout << "\n";
}

// ===== 2. 类型分类特性演示 =====
void demonstrate_type_classification() {
    std::cout << "=== 类型分类特性演示 ===\n";
    
    // 2.1 基础类型分类
    template<typename T>
    void analyze_type() {
        std::cout << "类型: " << typeid(T).name() << "\n";
        std::cout << "  是空类型: " << std::is_void_v<T> << "\n";
        std::cout << "  是空指针类型: " << std::is_null_pointer_v<T> << "\n";
        std::cout << "  是整数类型: " << std::is_integral_v<T> << "\n";
        std::cout << "  是浮点类型: " << std::is_floating_point_v<T> << "\n";
        std::cout << "  是数组类型: " << std::is_array_v<T> << "\n";
        std::cout << "  是枚举类型: " << std::is_enum_v<T> << "\n";
        std::cout << "  是联合类型: " << std::is_union_v<T> << "\n";
        std::cout << "  是类类型: " << std::is_class_v<T> << "\n";
        std::cout << "  是函数类型: " << std::is_function_v<T> << "\n";
        std::cout << "  是指针类型: " << std::is_pointer_v<T> << "\n";
        std::cout << "  是左值引用: " << std::is_lvalue_reference_v<T> << "\n";
        std::cout << "  是右值引用: " << std::is_rvalue_reference_v<T> << "\n";
        std::cout << "  是成员对象指针: " << std::is_member_object_pointer_v<T> << "\n";
        std::cout << "  是成员函数指针: " << std::is_member_function_pointer_v<T> << "\n";
        std::cout << "\n";
    }
    
    // 2.2 复合类型分类
    std::cout << "1. 基础类型分析:\n";
    analyze_type<int>();
    
    std::cout << "2. 复合类型分析:\n";
    analyze_type<int*>();
    analyze_type<int&>();
    analyze_type<int&&>();
    
    // 2.3 类和枚举类型
    enum Color { RED, GREEN, BLUE };
    enum class Shape { CIRCLE, SQUARE, TRIANGLE };
    
    class TestClass {
    public:
        int value;
        void method() {}
    };
    
    union TestUnion {
        int i;
        float f;
    };
    
    std::cout << "3. 用户定义类型分析:\n";
    analyze_type<Color>();
    analyze_type<Shape>();
    analyze_type<TestClass>();
    analyze_type<TestUnion>();
    
    // 2.4 成员指针类型
    std::cout << "4. 成员指针类型分析:\n";
    analyze_type<int TestClass::*>();
    analyze_type<void (TestClass::*)()>();
    
    std::cout << "\n";
}

// ===== 3. 类型转换特性演示 =====
void demonstrate_type_transformations() {
    std::cout << "=== 类型转换特性演示 ===\n";
    
    // 3.1 引用操作
    template<typename T>
    void reference_operations() {
        std::cout << "原始类型: " << typeid(T).name() << "\n";
        std::cout << "  移除引用: " << typeid(std::remove_reference_t<T>).name() << "\n";
        std::cout << "  添加左值引用: " << typeid(std::add_lvalue_reference_t<T>).name() << "\n";
        std::cout << "  添加右值引用: " << typeid(std::add_rvalue_reference_t<T>).name() << "\n";
        std::cout << "\n";
    }
    
    std::cout << "1. 引用操作:\n";
    reference_operations<int>();
    reference_operations<int&>();
    reference_operations<int&&>();
    
    // 3.2 指针操作
    template<typename T>
    void pointer_operations() {
        std::cout << "原始类型: " << typeid(T).name() << "\n";
        std::cout << "  移除指针: " << typeid(std::remove_pointer_t<T>).name() << "\n";
        std::cout << "  添加指针: " << typeid(std::add_pointer_t<T>).name() << "\n";
        std::cout << "\n";
    }
    
    std::cout << "2. 指针操作:\n";
    pointer_operations<int>();
    pointer_operations<int*>();
    pointer_operations<int**>();
    
    // 3.3 CV限定符操作
    template<typename T>
    void cv_operations() {
        std::cout << "原始类型: " << typeid(T).name() << "\n";
        std::cout << "  移除const: " << typeid(std::remove_const_t<T>).name() << "\n";
        std::cout << "  移除volatile: " << typeid(std::remove_volatile_t<T>).name() << "\n";
        std::cout << "  移除cv: " << typeid(std::remove_cv_t<T>).name() << "\n";
        std::cout << "  添加const: " << typeid(std::add_const_t<T>).name() << "\n";
        std::cout << "  添加volatile: " << typeid(std::add_volatile_t<T>).name() << "\n";
        std::cout << "  添加cv: " << typeid(std::add_cv_t<T>).name() << "\n";
        std::cout << "\n";
    }
    
    std::cout << "3. CV限定符操作:\n";
    cv_operations<int>();
    cv_operations<const int>();
    cv_operations<volatile int>();
    cv_operations<const volatile int>();
    
    // 3.4 符号操作
    template<typename T>
    void sign_operations() {
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
            std::cout << "原始类型: " << typeid(T).name() << "\n";
            std::cout << "  有符号版本: " << typeid(std::make_signed_t<T>).name() << "\n";
            std::cout << "  无符号版本: " << typeid(std::make_unsigned_t<T>).name() << "\n";
            std::cout << "\n";
        }
    }
    
    std::cout << "4. 符号操作:\n";
    sign_operations<int>();
    sign_operations<unsigned int>();
    sign_operations<char>();
    
    // 3.5 数组操作
    template<typename T>
    void array_operations() {
        std::cout << "原始类型: " << typeid(T).name() << "\n";
        std::cout << "  移除范围: " << typeid(std::remove_extent_t<T>).name() << "\n";
        std::cout << "  移除所有范围: " << typeid(std::remove_all_extents_t<T>).name() << "\n";
        
        if constexpr (std::is_array_v<T>) {
            std::cout << "  数组维度: " << std::rank_v<T> << "\n";
            std::cout << "  第0维大小: " << std::extent_v<T, 0> << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "5. 数组操作:\n";
    array_operations<int[10]>();
    array_operations<int[5][10]>();
    array_operations<int[]>();
    
    // 3.6 decay操作
    template<typename T>
    void decay_operations() {
        std::cout << "原始类型: " << typeid(T).name() << "\n";
        std::cout << "  decay后: " << typeid(std::decay_t<T>).name() << "\n";
        std::cout << "\n";
    }
    
    std::cout << "6. Decay操作:\n";
    decay_operations<int&>();
    decay_operations<const int&>();
    decay_operations<int[10]>();
    decay_operations<void()>();
    
    std::cout << "\n";
}

// ===== 4. 类型关系特性演示 =====
void demonstrate_type_relationships() {
    std::cout << "=== 类型关系特性演示 ===\n";
    
    // 4.1 类型相等性
    std::cout << "1. 类型相等性:\n";
    std::cout << "int == int: " << std::is_same_v<int, int> << "\n";
    std::cout << "int == const int: " << std::is_same_v<int, const int> << "\n";
    std::cout << "int == int&: " << std::is_same_v<int, int&> << "\n";
    std::cout << "int == std::decay_t<int&>: " << std::is_same_v<int, std::decay_t<int&>> << "\n";
    
    // 4.2 继承关系
    class Base {
    public:
        virtual ~Base() = default;
        virtual void method() {}
    };
    
    class Derived : public Base {
    public:
        void method() override {}
    };
    
    class Unrelated {
    public:
        void method() {}
    };
    
    std::cout << "\n2. 继承关系:\n";
    std::cout << "Derived继承自Base: " << std::is_base_of_v<Base, Derived> << "\n";
    std::cout << "Base继承自Derived: " << std::is_base_of_v<Derived, Base> << "\n";
    std::cout << "Base继承自Unrelated: " << std::is_base_of_v<Base, Unrelated> << "\n";
    
    // 4.3 转换关系
    std::cout << "\n3. 转换关系:\n";
    std::cout << "int可转换为double: " << std::is_convertible_v<int, double> << "\n";
    std::cout << "double可转换为int: " << std::is_convertible_v<double, int> << "\n";
    std::cout << "Derived*可转换为Base*: " << std::is_convertible_v<Derived*, Base*> << "\n";
    std::cout << "Base*可转换为Derived*: " << std::is_convertible_v<Base*, Derived*> << "\n";
    std::cout << "std::string可转换为int: " << std::is_convertible_v<std::string, int> << "\n";
    
    // 4.4 可调用性
    auto lambda = [](int x) -> double { return x * 2.0; };
    std::function<double(int)> func = lambda;
    
    std::cout << "\n4. 可调用性:\n";
    std::cout << "lambda可用int调用: " << std::is_invocable_v<decltype(lambda), int> << "\n";
    std::cout << "lambda可用string调用: " << std::is_invocable_v<decltype(lambda), std::string> << "\n";
    std::cout << "lambda调用返回double: " << std::is_invocable_r_v<double, decltype(lambda), int> << "\n";
    std::cout << "lambda调用返回int: " << std::is_invocable_r_v<int, decltype(lambda), int> << "\n";
    
    // 4.5 构造性
    std::cout << "\n5. 构造性:\n";
    std::cout << "int可默认构造: " << std::is_default_constructible_v<int> << "\n";
    std::cout << "std::vector<int>可默认构造: " << std::is_default_constructible_v<std::vector<int>> << "\n";
    std::cout << "int可从double构造: " << std::is_constructible_v<int, double> << "\n";
    std::cout << "std::string可从const char*构造: " << std::is_constructible_v<std::string, const char*> << "\n";
    
    // 4.6 赋值性
    std::cout << "\n6. 赋值性:\n";
    std::cout << "int可拷贝赋值: " << std::is_copy_assignable_v<int> << "\n";
    std::cout << "int可移动赋值: " << std::is_move_assignable_v<int> << "\n";
    std::cout << "unique_ptr可拷贝赋值: " << std::is_copy_assignable_v<std::unique_ptr<int>> << "\n";
    std::cout << "unique_ptr可移动赋值: " << std::is_move_assignable_v<std::unique_ptr<int>> << "\n";
    
    // 4.7 销毁性
    std::cout << "\n7. 销毁性:\n";
    std::cout << "int可销毁: " << std::is_destructible_v<int> << "\n";
    std::cout << "Base可平凡销毁: " << std::is_trivially_destructible_v<Base> << "\n";
    std::cout << "int可nothrow销毁: " << std::is_nothrow_destructible_v<int> << "\n";
    
    std::cout << "\n";
}

// ===== 5. 自定义类型特性演示 =====
void demonstrate_custom_traits() {
    std::cout << "=== 自定义类型特性演示 ===\n";
    
    // 5.1 检测成员函数存在性
    template<typename T, typename = void>
    struct has_size : std::false_type {};
    
    template<typename T>
    struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
    
    template<typename T>
    constexpr bool has_size_v = has_size<T>::value;
    
    // 5.2 检测成员类型存在性
    template<typename T, typename = void>
    struct has_value_type : std::false_type {};
    
    template<typename T>
    struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};
    
    template<typename T>
    constexpr bool has_value_type_v = has_value_type<T>::value;
    
    // 5.3 检测操作符存在性
    template<typename T, typename = void>
    struct has_equality_operator : std::false_type {};
    
    template<typename T>
    struct has_equality_operator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> 
        : std::true_type {};
    
    template<typename T>
    constexpr bool has_equality_operator_v = has_equality_operator<T>::value;
    
    // 5.4 复杂特性：可迭代类型
    template<typename T, typename = void>
    struct is_iterable : std::false_type {};
    
    template<typename T>
    struct is_iterable<T, std::void_t<
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        typename T::iterator
    >> : std::true_type {};
    
    template<typename T>
    constexpr bool is_iterable_v = is_iterable<T>::value;
    
    // 5.5 SFINAE友好的特性
    template<typename T>
    using size_type_t = decltype(std::declval<T>().size());
    
    template<typename T>
    using iterator_type_t = typename T::iterator;
    
    template<typename T>
    using value_type_t = typename T::value_type;
    
    // 5.6 条件类型选择
    template<typename T>
    using optimal_parameter_type_t = std::conditional_t<
        sizeof(T) <= sizeof(void*) && std::is_trivially_copyable_v<T>,
        T,          // 小对象：值传递
        const T&    // 大对象：引用传递
    >;
    
    // 5.7 类型列表操作
    template<typename...>
    struct type_list {};
    
    template<typename List>
    struct list_size;
    
    template<typename... Types>
    struct list_size<type_list<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};
    
    template<typename List>
    constexpr std::size_t list_size_v = list_size<List>::value;
    
    template<typename List, std::size_t Index>
    struct list_at;
    
    template<typename Head, typename... Tail>
    struct list_at<type_list<Head, Tail...>, 0> {
        using type = Head;
    };
    
    template<typename Head, typename... Tail, std::size_t Index>
    struct list_at<type_list<Head, Tail...>, Index> {
        using type = typename list_at<type_list<Tail...>, Index - 1>::type;
    };
    
    template<typename List, std::size_t Index>
    using list_at_t = typename list_at<List, Index>::type;
    
    // 测试自定义特性
    std::cout << "1. 成员函数检测:\n";
    std::cout << "vector有size方法: " << has_size_v<std::vector<int>> << "\n";
    std::cout << "int有size方法: " << has_size_v<int> << "\n";
    std::cout << "string有size方法: " << has_size_v<std::string> << "\n";
    
    std::cout << "\n2. 成员类型检测:\n";
    std::cout << "vector有value_type: " << has_value_type_v<std::vector<int>> << "\n";
    std::cout << "int有value_type: " << has_value_type_v<int> << "\n";
    std::cout << "string有value_type: " << has_value_type_v<std::string> << "\n";
    
    std::cout << "\n3. 操作符检测:\n";
    std::cout << "int有==操作符: " << has_equality_operator_v<int> << "\n";
    std::cout << "string有==操作符: " << has_equality_operator_v<std::string> << "\n";
    
    class NoEquality {
        int x;
    };
    std::cout << "NoEquality有==操作符: " << has_equality_operator_v<NoEquality> << "\n";
    
    std::cout << "\n4. 可迭代检测:\n";
    std::cout << "vector可迭代: " << is_iterable_v<std::vector<int>> << "\n";
    std::cout << "string可迭代: " << is_iterable_v<std::string> << "\n";
    std::cout << "int可迭代: " << is_iterable_v<int> << "\n";
    
    std::cout << "\n5. 参数类型优化:\n";
    std::cout << "int的最优参数类型: " << typeid(optimal_parameter_type_t<int>).name() << "\n";
    std::cout << "string的最优参数类型: " << typeid(optimal_parameter_type_t<std::string>).name() << "\n";
    std::cout << "vector的最优参数类型: " << typeid(optimal_parameter_type_t<std::vector<int>>).name() << "\n";
    
    std::cout << "\n6. 类型列表操作:\n";
    using test_list = type_list<int, double, std::string, std::vector<int>>;
    std::cout << "类型列表大小: " << list_size_v<test_list> << "\n";
    std::cout << "第0个类型: " << typeid(list_at_t<test_list, 0>).name() << "\n";
    std::cout << "第2个类型: " << typeid(list_at_t<test_list, 2>).name() << "\n";
    
    std::cout << "\n";
}

// ===== 实际应用案例演示 =====
void demonstrate_practical_applications() {
    std::cout << "=== 实际应用案例演示 ===\n";
    
    // 案例1：智能序列化系统
    template<typename T>
    void smart_serialize(const T& obj) {
        std::cout << "序列化对象: ";
        
        if constexpr (std::is_arithmetic_v<T>) {
            std::cout << "算术类型，直接序列化: " << obj << "\n";
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "字符串类型，带引号序列化: \"" << obj << "\"\n";
        } else if constexpr (has_size_v<T> && has_value_type_v<T>) {
            std::cout << "容器类型，序列化 " << obj.size() << " 个元素: [";
            bool first = true;
            for (const auto& item : obj) {
                if (!first) std::cout << ", ";
                smart_serialize(item);
                first = false;
            }
            std::cout << "]\n";
        } else {
            std::cout << "未知类型，使用通用序列化\n";
        }
    }
    
    // 案例2：性能优化的拷贝函数
    template<typename T>
    void optimized_copy(T& dest, const T& src) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::cout << "平凡拷贝，使用memcpy (大小: " << sizeof(T) << " bytes)\n";
            std::memcpy(&dest, &src, sizeof(T));
        } else if constexpr (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>) {
            std::cout << "支持移动语义，使用移动赋值\n";
            dest = std::move(const_cast<T&>(src));  // 仅演示，实际中要小心
        } else {
            std::cout << "使用拷贝构造和赋值\n";
            dest = src;
        }
    }
    
    // 案例3：类型安全的variant访问器
    template<typename T, typename Variant>
    decltype(auto) safe_get(Variant&& var) {
        if constexpr (std::is_same_v<T, std::decay_t<decltype(std::get<T>(var))>>) {
            std::cout << "类型匹配，安全获取\n";
            return std::get<T>(std::forward<Variant>(var));
        } else {
            static_assert(std::is_same_v<T, T>, "类型不匹配");
        }
    }
    
    // 案例4：通用比较器
    template<typename T>
    struct universal_comparator {
        bool operator()(const T& a, const T& b) const {
            if constexpr (has_equality_operator_v<T>) {
                std::cout << "使用==操作符比较\n";
                return a == b;
            } else if constexpr (std::is_arithmetic_v<T>) {
                std::cout << "算术类型，直接比较\n";
                return a == b;
            } else {
                std::cout << "使用内存比较\n";
                return std::memcmp(&a, &b, sizeof(T)) == 0;
            }
        }
    };
    
    // 测试实际应用
    std::cout << "1. 智能序列化测试:\n";
    smart_serialize(42);
    smart_serialize(std::string("hello"));
    smart_serialize(std::vector<int>{1, 2, 3, 4});
    
    std::cout << "\n2. 性能优化拷贝测试:\n";
    int a = 10, b = 20;
    optimized_copy(a, b);
    
    std::string str1 = "dest", str2 = "src";
    optimized_copy(str1, str2);
    
    std::cout << "\n3. 通用比较器测试:\n";
    universal_comparator<int> int_cmp;
    std::cout << "比较结果: " << int_cmp(10, 10) << "\n";
    
    universal_comparator<std::string> str_cmp;
    std::cout << "比较结果: " << str_cmp("hello", "hello") << "\n";
    
    std::cout << "\n";
}

// ===== 性能基准测试 =====
void benchmark_type_traits() {
    std::cout << "=== type_traits性能基准测试 ===\n";
    
    const int iterations = 10000000;
    
    // 编译期类型检查 vs 运行期类型检查
    auto compile_time_dispatch = []<typename T>(T value) -> int {
        if constexpr (std::is_integral_v<T>) {
            return static_cast<int>(value * 2);
        } else if constexpr (std::is_floating_point_v<T>) {
            return static_cast<int>(value * 3.0);
        } else {
            return 0;
        }
    };
    
    auto runtime_dispatch = [](auto value) -> int {
        // 模拟运行时类型检查开销
        if (typeid(value) == typeid(int)) {
            return static_cast<int>(value * 2);
        } else if (typeid(value) == typeid(double)) {
            return static_cast<int>(value * 3.0);
        } else {
            return 0;
        }
    };
    
    // 编译期版本基准测试
    auto start = std::chrono::high_resolution_clock::now();
    
    int compile_time_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        compile_time_sum += compile_time_dispatch(i % 1000);
    }
    
    auto compile_time_duration = std::chrono::high_resolution_clock::now() - start;
    
    // 运行期版本基准测试
    start = std::chrono::high_resolution_clock::now();
    
    int runtime_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        runtime_sum += runtime_dispatch(i % 1000);
    }
    
    auto runtime_duration = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "性能对比结果 (" << iterations << " 次迭代):\n";
    std::cout << "编译期类型检查耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(compile_time_duration).count() 
              << " 毫秒 (结果: " << compile_time_sum << ")\n";
    std::cout << "运行期类型检查耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(runtime_duration).count() 
              << " 毫秒 (结果: " << runtime_sum << ")\n";
    
    double speedup = static_cast<double>(runtime_duration.count()) / compile_time_duration.count();
    std::cout << "编译期优势: " << speedup << "x 性能提升\n";
    
    std::cout << "\n编译期优势总结:\n";
    std::cout << "- 零运行时开销的类型检查和分发\n";
    std::cout << "- 编译期错误检测和类型安全\n";
    std::cout << "- 代码优化和死代码消除\n";
    std::cout << "- 模板实例化的精确控制\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++11/14/17/20 type_traits库深度解析\n";
    std::cout << "===================================\n";
    
    demonstrate_type_traits_basics();
    demonstrate_type_classification();
    demonstrate_type_transformations();
    demonstrate_type_relationships();
    demonstrate_custom_traits();
    demonstrate_practical_applications();
    benchmark_type_traits();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 13_type_traits.cpp -o type_traits
./type_traits

C++20版本:
g++ -std=c++20 -O2 -Wall 13_type_traits.cpp -o type_traits

关键学习点:
1. type_traits提供编译期类型信息查询和转换能力
2. 类型分类、转换、关系特性构成完整的类型系统
3. 自定义type_traits支持复杂的元编程技术
4. SFINAE和void_t是实现类型特性的核心技术
5. type_traits在模板编程中实现零开销抽象

注意事项:
- type_traits只在编译期有效，不产生运行时开销
- 需要理解SFINAE原理来实现复杂的类型检测
- C++17的if constexpr大大简化了类型特性的使用
- 自定义特性应该遵循标准库的命名和设计模式
*/