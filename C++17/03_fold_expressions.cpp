/**
 * C++17 折叠表达式参数包处理深度解析
 * 
 * 核心概念：
 * 1. 折叠表达式语法 - 一元和二元折叠的四种形式
 * 2. 编译器展开机制 - 参数包的递归展开过程
 * 3. 运算符重载配合 - 自定义类型的折叠操作
 * 4. 性能优化分析 - 与传统递归模板的对比
 * 5. 高级应用模式 - 函数式编程和元编程技巧
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>
#include <chrono>
#include <functional>
#include <memory>

// ===== 1. 折叠表达式语法演示 =====
template<typename... Args>
auto sum_all(Args... args) {
    return (args + ...);  // 一元右折叠
}

template<typename... Args>
auto multiply_all(Args... args) {
    return (args * ...);  // 一元右折叠
}

template<typename... Args>
auto sum_with_init(Args... args) {
    return (0 + ... + args);  // 二元左折叠，初值为0
}

template<typename... Args>
auto logical_and_all(Args... args) {
    return (args && ...);  // 逻辑与折叠
}

template<typename... Args>
void print_all(Args... args) {
    ((std::cout << args << " "), ...);  // 逗号运算符折叠
    std::cout << "\n";
}

// 四种折叠形式完整演示
template<typename... Args>
void demonstrate_fold_syntax(Args... args) {
    std::cout << "=== 折叠表达式语法演示 ===\n";
    
    // 1. 一元右折叠: (pack op ...)
    std::cout << "一元右折叠求和: " << (args + ...) << "\n";
    
    // 2. 一元左折叠: (... op pack)
    std::cout << "一元左折叠求和: " << (... + args) << "\n";
    
    // 3. 二元右折叠: (pack op ... op init)
    std::cout << "二元右折叠(初值100): " << (args + ... + 100) << "\n";
    
    // 4. 二元左折叠: (init op ... op pack)
    std::cout << "二元左折叠(初值100): " << (100 + ... + args) << "\n";
    
    // 逻辑运算折叠
    std::cout << "逻辑与折叠: " << (args && ...) << "\n";
    std::cout << "逻辑或折叠: " << (args || ...) << "\n";
    
    // 比较运算折叠（需要确保所有参数相等）
    auto first = (args, ...);  // 获取最后一个参数
    // std::cout << "全部相等: " << ((args == first) && ...) << "\n";  // 需要更复杂的实现
    
    std::cout << "\n";
}

// ===== 2. 编译器展开机制演示 =====
// 手动展示编译器的展开过程
template<typename T>
T manual_sum_1(T arg) {
    std::cout << "展开到单个参数: " << arg << "\n";
    return arg;
}

template<typename T1, typename T2>
auto manual_sum_2(T1 arg1, T2 arg2) {
    std::cout << "展开到两个参数: " << arg1 << " + " << arg2 << " = " << (arg1 + arg2) << "\n";
    return arg1 + arg2;
}

template<typename T1, typename T2, typename T3>
auto manual_sum_3(T1 arg1, T2 arg2, T3 arg3) {
    std::cout << "展开到三个参数: " << arg1 << " + " << arg2 << " + " << arg3 << " = " << (arg1 + arg2 + arg3) << "\n";
    return arg1 + arg2 + arg3;
}

// 编译期展开追踪
template<int N>
struct FoldTracker {
    static constexpr int value = N;
    FoldTracker() { std::cout << "创建追踪器 " << N << "\n"; }
    int get() const { return value; }
    
    // 自定义运算符用于展开追踪
    FoldTracker operator+(const FoldTracker& other) const {
        std::cout << "折叠操作: " << value << " + " << other.value << " = " << (value + other.value) << "\n";
        return FoldTracker<value + other.value>{};
    }
};

void demonstrate_expansion_mechanism() {
    std::cout << "=== 编译器展开机制演示 ===\n";
    
    // 手动展开示例
    std::cout << "手动展开过程:\n";
    manual_sum_1(5);
    manual_sum_2(1, 2);
    manual_sum_3(1, 2, 3);
    
    // 折叠展开追踪
    std::cout << "\n折叠表达式展开追踪:\n";
    auto result = (FoldTracker<1>{} + FoldTracker<2>{} + FoldTracker<3>{});
    std::cout << "最终结果: " << result.get() << "\n";
    
    std::cout << "\n";
}

// ===== 3. 运算符重载配合演示 =====
class StringBuilder {
private:
    std::string data;
    
public:
    StringBuilder() = default;
    explicit StringBuilder(const std::string& str) : data(str) {}
    
    // 自定义加法运算符用于字符串连接
    StringBuilder operator+(const StringBuilder& other) const {
        return StringBuilder(data + other.data);
    }
    
    // 支持与字符串字面量的操作
    StringBuilder operator+(const std::string& str) const {
        return StringBuilder(data + str);
    }
    
    // 支持与字符的操作
    StringBuilder operator+(char c) const {
        return StringBuilder(data + c);
    }
    
    // 逗号运算符重载用于链式添加
    StringBuilder& operator,(const std::string& str) {
        data += str;
        return *this;
    }
    
    std::string str() const { return data; }
    
    friend std::ostream& operator<<(std::ostream& os, const StringBuilder& sb) {
        return os << sb.data;
    }
};

// 累积器类型
template<typename T>
class Accumulator {
private:
    T value;
    
public:
    explicit Accumulator(T val = T{}) : value(val) {}
    
    Accumulator operator+(const Accumulator& other) const {
        return Accumulator{value + other.value};
    }
    
    Accumulator operator*(const Accumulator& other) const {
        return Accumulator{value * other.value};
    }
    
    T get() const { return value; }
};

template<typename... Args>
StringBuilder concatenate_all(Args... args) {
    return (StringBuilder{} + ... + StringBuilder{args});
}

template<typename T, typename... Args>
Accumulator<T> accumulate_all(Args... args) {
    return (Accumulator<T>{args} + ...);
}

void demonstrate_operator_overloading() {
    std::cout << "=== 运算符重载配合演示 ===\n";
    
    // 字符串构建器折叠
    auto result = concatenate_all("Hello", " ", "World", "!");
    std::cout << "字符串折叠结果: " << result.str() << "\n";
    
    // 数值累积器折叠
    auto sum_acc = accumulate_all<int>(1, 2, 3, 4, 5);
    std::cout << "累积器求和: " << sum_acc.get() << "\n";
    
    // 直接折叠不同类型
    StringBuilder sb1{"开始"};
    StringBuilder sb2{"中间"};
    StringBuilder sb3{"结束"};
    auto combined = (sb1 + sb2 + sb3);
    std::cout << "StringBuilder折叠: " << combined.str() << "\n";
    
    std::cout << "\n";
}

// ===== 4. 性能优化分析演示 =====
// 传统递归求和
template<typename T>
constexpr T recursive_sum(T value) {
    return value;
}

template<typename T, typename... Args>
constexpr T recursive_sum(T first, Args... rest) {
    return first + recursive_sum(rest...);
}

// 折叠表达式求和
template<typename... Args>
constexpr auto fold_sum(Args... args) {
    return (args + ...);
}

// 性能测试辅助函数
template<typename Func>
auto measure_time(Func&& func, const std::string& name) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << name << " 耗时: " << duration.count() << " ns, 结果: " << result << "\n";
    
    return result;
}

void demonstrate_performance_analysis() {
    std::cout << "=== 性能优化分析演示 ===\n";
    
    // 编译期计算比较
    constexpr auto compile_recursive = recursive_sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    constexpr auto compile_fold = fold_sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    
    std::cout << "编译期递归求和: " << compile_recursive << "\n";
    std::cout << "编译期折叠求和: " << compile_fold << "\n";
    
    // 运行时性能比较（模拟）
    const int iterations = 1000000;
    
    auto recursive_test = [&]() {
        int result = 0;
        for (int i = 0; i < iterations; ++i) {
            result += recursive_sum(1, 2, 3, 4, 5);
        }
        return result;
    };
    
    auto fold_test = [&]() {
        int result = 0;
        for (int i = 0; i < iterations; ++i) {
            result += fold_sum(1, 2, 3, 4, 5);
        }
        return result;
    };
    
    measure_time(recursive_test, "递归求和");
    measure_time(fold_test, "折叠求和");
    
    std::cout << "\n";
}

// ===== 5. 高级应用模式演示 =====
// 函数式编程：Map操作
template<typename Func, typename... Args>
auto map_fold(Func func, Args... args) {
    return std::make_tuple(func(args)...);
}

// 函数式编程：Filter操作（使用折叠实现计数）
template<typename Predicate, typename... Args>
constexpr size_t count_if_fold(Predicate pred, Args... args) {
    return (static_cast<size_t>(pred(args)) + ...);
}

// 类型列表操作
template<typename... Types>
struct TypeList {
    static constexpr size_t size = sizeof...(Types);
};

template<typename... Types>
constexpr bool all_arithmetic(TypeList<Types...>) {
    return (std::is_arithmetic_v<Types> && ...);
}

template<typename... Types>
constexpr bool any_pointer(TypeList<Types...>) {
    return (std::is_pointer_v<Types> || ...);
}

template<typename... Types>
constexpr size_t total_size(TypeList<Types...>) {
    return (sizeof(Types) + ...);
}

// 变参函数调用
template<typename Func, typename... Args>
void call_with_each(Func func, Args... args) {
    (func(args), ...);
}

// 智能指针批量操作
template<typename... Ptrs>
void reset_all(Ptrs&... ptrs) {
    (ptrs.reset(), ...);
}

template<typename... Ptrs>
bool all_valid(const Ptrs&... ptrs) {
    return (static_cast<bool>(ptrs) && ...);
}

void demonstrate_advanced_patterns() {
    std::cout << "=== 高级应用模式演示 ===\n";
    
    // 函数式Map操作
    auto squared = map_fold([](int x) { return x * x; }, 1, 2, 3, 4, 5);
    std::cout << "Map操作结果: ";
    std::apply([](auto... args) { ((std::cout << args << " "), ...); }, squared);
    std::cout << "\n";
    
    // 函数式Filter计数
    auto positive_count = count_if_fold([](int x) { return x > 0; }, -2, -1, 0, 1, 2, 3);
    std::cout << "正数计数: " << positive_count << "\n";
    
    // 类型列表操作
    using MixedTypes = TypeList<int, double, char*, std::string, float>;
    std::cout << "类型列表大小: " << MixedTypes::size << "\n";
    std::cout << "全为算术类型: " << all_arithmetic(MixedTypes{}) << "\n";
    std::cout << "包含指针类型: " << any_pointer(MixedTypes{}) << "\n";
    std::cout << "总字节大小: " << total_size(MixedTypes{}) << "\n";
    
    // 批量函数调用
    std::cout << "批量调用输出: ";
    call_with_each([](const auto& x) { std::cout << x << " "; }, 1, 2.5, "hello", 'c');
    std::cout << "\n";
    
    // 智能指针操作
    auto ptr1 = std::make_unique<int>(10);
    auto ptr2 = std::make_unique<std::string>("test");
    auto ptr3 = std::make_unique<double>(3.14);
    
    std::cout << "所有指针有效: " << all_valid(ptr1, ptr2, ptr3) << "\n";
    reset_all(ptr1, ptr2, ptr3);
    std::cout << "重置后有效: " << all_valid(ptr1, ptr2, ptr3) << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 折叠表达式参数包处理深度解析\n";
    std::cout << "=====================================\n";
    
    // 基本语法演示
    demonstrate_fold_syntax(1, 2, 3, 4, 5);
    
    demonstrate_expansion_mechanism();
    demonstrate_operator_overloading();
    demonstrate_performance_analysis();
    demonstrate_advanced_patterns();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 03_fold_expressions.cpp -o fold_expressions
./fold_expressions

关键学习点:
1. 四种折叠形式：一元左/右折叠、二元左/右折叠
2. 编译器自动展开参数包，无需手动递归
3. 自定义运算符重载可实现复杂的折叠操作
4. 折叠表达式通常比递归模板性能更好
5. 在函数式编程和元编程中有广泛应用

注意事项:
- 空参数包的折叠需要注意默认值（&&为true，||为false，,为void()）
- 运算符优先级在复杂折叠中需要用括号明确
- 某些运算符（如赋值运算符）不能用于折叠表达式
- 折叠表达式可与其他C++17特性（如constexpr if）结合使用
*/