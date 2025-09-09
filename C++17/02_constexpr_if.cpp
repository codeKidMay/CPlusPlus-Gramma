/**
 * C++17 constexpr if编译期分支深度解析
 * 
 * 核心概念：
 * 1. 编译期条件分支 - 模板实例化时的代码生成控制
 * 2. SFINAE替代方案 - 简化复杂的模板元编程技巧
 * 3. 类型特征应用 - 与type_traits的深度结合
 * 4. 性能零开销 - 编译期优化和运行时效率
 * 5. 模板递归优化 - 简化递归模板的终止条件
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <utility>

// ===== 1. 编译期条件分支演示 =====
template<typename T>
void demonstrate_basic_constexpr_if(T value) {
    std::cout << "=== 编译期条件分支演示 ===\n";
    
    // 根据类型在编译期选择不同的处理逻辑
    if constexpr (std::is_integral_v<T>) {
        std::cout << "处理整数类型: " << value << " (二进制位数: " << sizeof(T) * 8 << ")\n";
        std::cout << "最大值: " << std::numeric_limits<T>::max() << "\n";
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "处理浮点类型: " << value << " (精度: " << std::numeric_limits<T>::digits10 << " 位)\n";
        std::cout << "epsilon: " << std::numeric_limits<T>::epsilon() << "\n";
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "处理字符串类型: \"" << value << "\" (长度: " << value.length() << ")\n";
        std::cout << "容量: " << value.capacity() << "\n";
    }
    else {
        std::cout << "处理其他类型: " << typeid(T).name() << "\n";
    }
    
    std::cout << "\n";
}

// ===== 2. SFINAE替代方案演示 =====
// 传统SFINAE方法
template<typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
old_style_clamp(T value, T min_val, T max_val) {
    return std::max(min_val, std::min(value, max_val));
}

// constexpr if简化版本
template<typename T>
auto modern_clamp(T value, T min_val, T max_val) {
    if constexpr (std::is_arithmetic_v<T>) {
        return std::max(min_val, std::min(value, max_val));
    } else {
        static_assert(std::is_arithmetic_v<T>, "clamp只支持算术类型");
    }
}

// 复杂的类型处理示例
template<typename Container>
auto process_container(const Container& container) {
    if constexpr (std::is_same_v<Container, std::vector<typename Container::value_type>>) {
        std::cout << "优化的vector处理，容量: " << container.capacity() << "\n";
        return container.size();
    }
    else if constexpr (requires { container.size(); }) {
        std::cout << "通用容器处理\n";
        return container.size();
    }
    else {
        std::cout << "非容器类型处理\n";
        return size_t{0};
    }
}

void demonstrate_sfinae_replacement() {
    std::cout << "=== SFINAE替代方案演示 ===\n";
    
    // 比较传统和现代方法
    int clamped_old = old_style_clamp(15, 10, 20);
    int clamped_new = modern_clamp(15, 10, 20);
    std::cout << "传统clamp: " << clamped_old << ", 现代clamp: " << clamped_new << "\n";
    
    // 容器处理
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::string str = "hello";
    int plain_value = 42;
    
    std::cout << "Vector大小: " << process_container(vec) << "\n";
    std::cout << "String大小: " << process_container(str) << "\n";
    // std::cout << "Plain值处理: " << process_container(plain_value) << "\n";  // 编译错误
    
    std::cout << "\n";
}

// ===== 3. 类型特征应用演示 =====
template<typename T>
class SmartContainer {
private:
    T data;
    
public:
    explicit SmartContainer(T value) : data(std::move(value)) {}
    
    // 根据类型特征选择最优的访问方式
    auto get() const {
        if constexpr (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(void*)) {
            // 小型可简单复制的类型：按值返回
            return data;
        } else {
            // 大型或复杂类型：按引用返回
            return std::cref(data);
        }
    }
    
    // 智能序列化
    void serialize() const {
        if constexpr (std::is_arithmetic_v<T>) {
            std::cout << "二进制序列化算术类型: " << data << "\n";
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "文本序列化字符串: \"" << data << "\"\n";
        }
        else if constexpr (requires { data.begin(); data.end(); }) {
            std::cout << "容器序列化，元素数量: " << std::distance(data.begin(), data.end()) << "\n";
        }
        else {
            std::cout << "自定义序列化复杂类型\n";
        }
    }
    
    // 类型适配的比较操作
    template<typename U>
    bool compare(const U& other) const {
        if constexpr (std::is_same_v<T, U>) {
            return data == other;
        }
        else if constexpr (std::is_convertible_v<U, T>) {
            return data == static_cast<T>(other);
        }
        else {
            static_assert(std::is_same_v<T, U> || std::is_convertible_v<U, T>, 
                         "无法比较不兼容的类型");
            return false;
        }
    }
};

void demonstrate_type_traits_application() {
    std::cout << "=== 类型特征应用演示 ===\n";
    
    // 不同类型的智能容器
    SmartContainer<int> int_container(42);
    SmartContainer<std::string> str_container(std::string("hello world"));
    SmartContainer<std::vector<int>> vec_container(std::vector<int>{1, 2, 3});
    
    // 智能访问
    auto int_val = int_container.get();  // 按值返回
    auto str_ref = str_container.get();  // 按引用返回
    
    std::cout << "整数值: " << int_val << "\n";
    std::cout << "字符串引用: " << str_ref.get() << "\n";
    
    // 智能序列化
    int_container.serialize();
    str_container.serialize();
    vec_container.serialize();
    
    // 类型适配比较
    std::cout << "比较结果: " << int_container.compare(42) << "\n";
    std::cout << "转换比较: " << int_container.compare(42.0) << "\n";
    
    std::cout << "\n";
}

// ===== 4. 性能零开销演示 =====
template<int N>
constexpr int fibonacci() {
    if constexpr (N <= 1) {
        return N;
    } else {
        return fibonacci<N-1>() + fibonacci<N-2>();
    }
}

// 运行时vs编译时性能比较
int runtime_fibonacci(int n) {
    if (n <= 1) return n;
    return runtime_fibonacci(n-1) + runtime_fibonacci(n-2);
}

template<typename T>
T optimized_abs(T value) {
    if constexpr (std::is_unsigned_v<T>) {
        return value;  // 无符号类型无需处理
    } else {
        return value < 0 ? -value : value;
    }
}

void demonstrate_zero_overhead() {
    std::cout << "=== 性能零开销演示 ===\n";
    
    // 编译时斐波那契数列计算
    constexpr int fib_10 = fibonacci<10>();
    std::cout << "编译时计算 fibonacci(10): " << fib_10 << "\n";
    
    // 性能测试
    const int test_value = 35;
    
    auto start = std::chrono::high_resolution_clock::now();
    int runtime_result = runtime_fibonacci(test_value);
    auto mid = std::chrono::high_resolution_clock::now();
    
    // 注意：编译时版本无法用变量，这里展示概念
    constexpr int compile_result = fibonacci<35>();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto runtime_duration = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    
    std::cout << "运行时 fibonacci(" << test_value << "): " << runtime_result 
              << " (耗时: " << runtime_duration.count() << "ms)\n";
    std::cout << "编译时 fibonacci(" << test_value << "): " << compile_result 
              << " (耗时: 0ms)\n";
    
    // 类型优化示例
    unsigned int u_val = 10;
    int s_val = -10;
    
    std::cout << "无符号绝对值: " << optimized_abs(u_val) << " (无操作优化)\n";
    std::cout << "有符号绝对值: " << optimized_abs(s_val) << "\n";
    
    std::cout << "\n";
}

// ===== 5. 模板递归优化演示 =====
template<typename... Args>
void variadic_print(Args&&... args) {
    ((std::cout << args << " "), ...);  // C++17折叠表达式
    std::cout << "\n";
}

// 传统递归模板
template<typename T>
void old_style_print(T&& value) {
    std::cout << value << "\n";
}

template<typename T, typename... Args>
void old_style_print(T&& first, Args&&... rest) {
    std::cout << first << " ";
    old_style_print(rest...);
}

// constexpr if简化递归
template<typename T, typename... Args>
void modern_recursive_print(T&& first, Args&&... rest) {
    std::cout << first;
    
    if constexpr (sizeof...(rest) > 0) {
        std::cout << " ";
        modern_recursive_print(rest...);
    } else {
        std::cout << "\n";
    }
}

// 高级递归模板：类型列表处理
template<typename... Types>
struct TypeList {};

template<typename List>
struct type_count;

template<typename... Types>
struct type_count<TypeList<Types...>> {
    static constexpr size_t value = sizeof...(Types);
};

template<typename List, template<typename> class Predicate>
struct count_if;

template<typename... Types, template<typename> class Predicate>
struct count_if<TypeList<Types...>, Predicate> {
    static constexpr size_t value = (Predicate<Types>::value + ...);
};

void demonstrate_template_recursion() {
    std::cout << "=== 模板递归优化演示 ===\n";
    
    // 比较不同的打印方式
    std::cout << "变参折叠表达式: ";
    variadic_print(1, 2.5, "hello", 'c');
    
    std::cout << "传统递归模板: ";
    old_style_print(1, 2.5, "hello", 'c');
    
    std::cout << "constexpr if递归: ";
    modern_recursive_print(1, 2.5, "hello", 'c');
    
    // 类型列表处理
    using MyTypes = TypeList<int, double, std::string, char, float>;
    constexpr size_t total_types = type_count<MyTypes>::value;
    constexpr size_t arithmetic_types = count_if<MyTypes, std::is_arithmetic>::value;
    
    std::cout << "类型列表总数: " << total_types << "\n";
    std::cout << "算术类型数量: " << arithmetic_types << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 constexpr if编译期分支深度解析\n";
    std::cout << "======================================\n";
    
    // 演示基本用法
    demonstrate_basic_constexpr_if(42);
    demonstrate_basic_constexpr_if(3.14);
    demonstrate_basic_constexpr_if(std::string("test"));
    
    demonstrate_sfinae_replacement();
    demonstrate_type_traits_application();
    demonstrate_zero_overhead();
    demonstrate_template_recursion();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 02_constexpr_if.cpp -o constexpr_if
./constexpr_if

关键学习点:
1. constexpr if在编译期确定分支，未选择的分支不会被实例化
2. 完全替代复杂的SFINAE技巧，代码更清晰易读
3. 与type_traits结合实现类型特化的高级功能
4. 编译期计算实现真正的零运行时开销
5. 简化递归模板的终止条件处理

注意事项:
- constexpr if的条件必须是编译期常量表达式
- 未选择的分支仍需语法正确，但不会被语义检查
- 在模板外使用时，条件必须能在编译期确定
- 配合requires表达式能实现更强大的约束检查
*/