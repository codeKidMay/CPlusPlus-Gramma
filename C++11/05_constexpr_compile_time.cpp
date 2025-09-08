/**
 * C++11 constexpr编译期计算深度解析
 * 
 * 核心概念：
 * 1. constexpr函数的限制和设计原则
 * 2. 字面值类型(Literal Types)的要求
 * 3. 编译期与运行时的双重性质
 * 4. 模板元编程与constexpr的结合
 * 5. 实际应用：编译期算法和数据结构
 */

#include <iostream>
#include <array>
#include <string>
#include <type_traits>
#include <limits>
#include <cmath>

// ===== 1. constexpr函数基础 =====

// 简单的constexpr函数（C++11只能有一个return语句）
constexpr int square(int x) {
    return x * x;
}

// 递归实现阶乘
constexpr long factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 递归实现斐波那契数列
constexpr long fibonacci(int n) {
    return n <= 1 ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

// 条件表达式的使用
constexpr int absolute(int x) {
    return x < 0 ? -x : x;
}

// 编译期字符串长度计算
constexpr size_t string_length(const char* str) {
    return *str ? 1 + string_length(str + 1) : 0;
}

void demonstrate_basic_constexpr() {
    std::cout << "=== constexpr函数基础演示 ===\n";
    
    // 编译期计算
    constexpr int compile_square = square(10);
    constexpr long compile_factorial = factorial(5);
    constexpr long compile_fib = fibonacci(10);
    constexpr size_t compile_length = string_length("Hello World");
    
    std::cout << "编译期计算结果:\n";
    std::cout << "square(10) = " << compile_square << std::endl;
    std::cout << "factorial(5) = " << compile_factorial << std::endl;
    std::cout << "fibonacci(10) = " << compile_fib << std::endl;
    std::cout << "string_length(\"Hello World\") = " << compile_length << std::endl;
    
    // 运行时也可以调用
    int runtime_input = 7;
    std::cout << "\n运行时调用:\n";
    std::cout << "factorial(" << runtime_input << ") = " << factorial(runtime_input) << std::endl;
    
    std::cout << "\n";
}

// ===== 2. 字面值类型和constexpr构造函数 =====

// 字面值类型的要求：
// 1. 标量类型、引用类型、特定的数组类型
// 2. 有constexpr构造函数且析构函数不是用户定义的类类型

class Point {
private:
    int x_, y_;
    
public:
    // constexpr构造函数
    constexpr Point(int x = 0, int y = 0) : x_(x), y_(y) {}
    
    // constexpr成员函数
    constexpr int x() const { return x_; }
    constexpr int y() const { return y_; }
    constexpr int distance_squared() const { return x_ * x_ + y_ * y_; }
    
    // constexpr静态成员函数
    constexpr static Point origin() { return Point(0, 0); }
};

// 更复杂的字面值类型
struct Rectangle {
    Point top_left;
    Point bottom_right;
    
    constexpr Rectangle(Point tl, Point br) : top_left(tl), bottom_right(br) {}
    
    constexpr int width() const { 
        return bottom_right.x() - top_left.x(); 
    }
    
    constexpr int height() const { 
        return bottom_right.y() - top_left.y(); 
    }
    
    constexpr int area() const { 
        return width() * height(); 
    }
};

void demonstrate_literal_types() {
    std::cout << "=== 字面值类型演示 ===\n";
    
    // 编译期创建对象
    constexpr Point p1(3, 4);
    constexpr Point p2 = Point::origin();
    constexpr Rectangle rect(Point(0, 0), Point(10, 5));
    
    // 编译期计算
    constexpr int distance = p1.distance_squared();
    constexpr int rect_area = rect.area();
    
    std::cout << "点p1距离平方: " << distance << std::endl;
    std::cout << "矩形面积: " << rect_area << std::endl;
    
    // 验证这些确实是编译期常量
    static_assert(distance == 25, "编译期计算错误");
    static_assert(rect_area == 50, "编译期计算错误");
    std::cout << "编译期断言通过！\n";
    
    std::cout << "\n";
}

// ===== 3. 编译期数组和容器操作 =====

// 编译期数组初始化
template<size_t N>
constexpr std::array<int, N> generate_squares() {
    std::array<int, N> result = {};
    for (size_t i = 0; i < N; ++i) {
        // C++11的constexpr函数限制，这里需要用递归或其他技巧
        // 这个例子展示概念，实际C++11实现会更复杂
    }
    return result;
}

// C++11兼容的编译期数组生成
template<size_t N, size_t I = 0>
struct ArrayGenerator {
    static constexpr void fill_squares(std::array<int, N>& arr) {
        arr[I] = I * I;
        ArrayGenerator<N, I + 1>::fill_squares(arr);
    }
};

template<size_t N>
struct ArrayGenerator<N, N> {
    static constexpr void fill_squares(std::array<int, N>&) {
        // 递归终止
    }
};

// 编译期查找
constexpr int find_in_array(const int* arr, size_t size, int target, size_t index = 0) {
    return index >= size ? -1 : 
           (arr[index] == target ? static_cast<int>(index) : 
            find_in_array(arr, size, target, index + 1));
}

// 编译期排序（冒泡排序）
constexpr void bubble_sort_step(int* arr, size_t size, size_t i, size_t j) {
    // C++11 constexpr限制，实际实现会使用条件表达式
}

void demonstrate_compile_time_arrays() {
    std::cout << "=== 编译期数组操作演示 ===\n";
    
    // 编译期数组操作
    constexpr int numbers[] = {5, 2, 8, 1, 9, 3};
    constexpr size_t array_size = sizeof(numbers) / sizeof(numbers[0]);
    
    // 编译期查找
    constexpr int index_of_8 = find_in_array(numbers, array_size, 8);
    constexpr int index_of_7 = find_in_array(numbers, array_size, 7);
    
    std::cout << "在数组中查找8的索引: " << index_of_8 << std::endl;
    std::cout << "在数组中查找7的索引: " << index_of_7 << std::endl;
    
    std::cout << "\n";
}

// ===== 4. 数学计算库 =====

// 编译期数学常量
constexpr double PI = 3.14159265358979323846;
constexpr double E = 2.71828182845904523536;

// 编译期幂运算
constexpr double power(double base, int exp) {
    return exp == 0 ? 1.0 :
           exp > 0 ? base * power(base, exp - 1) :
           1.0 / power(base, -exp);
}

// 编译期平方根（牛顿法）
constexpr double sqrt_helper(double x, double guess, double prev_guess) {
    return guess == prev_guess ? guess :
           sqrt_helper(x, (guess + x / guess) / 2.0, guess);
}

constexpr double sqrt_constexpr(double x) {
    return x >= 0 ? sqrt_helper(x, x / 2.0, x) : 0.0;
}

// 编译期三角函数近似（泰勒级数）
constexpr double sin_taylor_term(double x, int n) {
    return power(-1, n) * power(x, 2 * n + 1) / factorial(2 * n + 1);
}

constexpr double sin_taylor(double x, int terms, int current = 0, double result = 0.0) {
    return current >= terms ? result :
           sin_taylor(x, terms, current + 1, result + sin_taylor_term(x, current));
}

constexpr double sin_constexpr(double x) {
    return sin_taylor(x, 10);  // 使用10项泰勒展开
}

void demonstrate_math_library() {
    std::cout << "=== 编译期数学库演示 ===\n";
    
    // 编译期数学计算
    constexpr double circle_area = PI * power(5.0, 2);
    constexpr double sqrt_25 = sqrt_constexpr(25.0);
    constexpr double sin_pi_6 = sin_constexpr(PI / 6.0);
    
    std::cout << "圆的面积 (r=5): " << circle_area << std::endl;
    std::cout << "sqrt(25): " << sqrt_25 << std::endl;
    std::cout << "sin(π/6): " << sin_pi_6 << " (应该约等于0.5)" << std::endl;
    
    // 验证精度
    std::cout << "sin(π/6)误差: " << std::abs(sin_pi_6 - 0.5) << std::endl;
    
    std::cout << "\n";
}

// ===== 5. 编译期字符串处理 =====

// 编译期字符串比较
constexpr bool string_equal(const char* str1, const char* str2) {
    return *str1 == *str2 && (*str1 == '\0' || string_equal(str1 + 1, str2 + 1));
}

// 编译期字符串查找
constexpr const char* string_find(const char* haystack, const char* needle) {
    return *needle == '\0' ? haystack :
           *haystack == '\0' ? nullptr :
           string_equal(haystack, needle) ? haystack :
           string_find(haystack + 1, needle);
}

// 编译期字符串哈希（简单版本）
constexpr size_t string_hash(const char* str, size_t hash = 5381) {
    return *str ? string_hash(str + 1, hash * 33 + *str) : hash;
}

// constexpr字符串类（简化版）
class constexpr_string {
private:
    const char* data_;
    size_t size_;
    
public:
    constexpr constexpr_string(const char* str) 
        : data_(str), size_(string_length(str)) {}
    
    constexpr size_t length() const { return size_; }
    constexpr const char* data() const { return data_; }
    constexpr char operator[](size_t index) const { return data_[index]; }
    
    constexpr bool operator==(const constexpr_string& other) const {
        return size_ == other.size_ && string_equal(data_, other.data_);
    }
    
    constexpr size_t hash() const {
        return string_hash(data_);
    }
};

void demonstrate_string_processing() {
    std::cout << "=== 编译期字符串处理演示 ===\n";
    
    // 编译期字符串操作
    constexpr const char* text = "Hello World";
    constexpr const char* pattern = "World";
    constexpr bool are_equal = string_equal("test", "test");
    constexpr const char* found = string_find(text, pattern);
    constexpr size_t text_hash = string_hash(text);
    
    std::cout << "字符串相等性测试: " << are_equal << std::endl;
    std::cout << "字符串查找结果: " << (found ? found : "未找到") << std::endl;
    std::cout << "字符串哈希值: " << text_hash << std::endl;
    
    // constexpr字符串类
    constexpr constexpr_string str1("Hello");
    constexpr constexpr_string str2("World");
    constexpr constexpr_string str3("Hello");
    
    std::cout << "str1长度: " << str1.length() << std::endl;
    std::cout << "str1 == str3: " << (str1 == str3) << std::endl;
    std::cout << "str1 == str2: " << (str1 == str2) << std::endl;
    std::cout << "str1哈希: " << str1.hash() << std::endl;
    
    std::cout << "\n";
}

// ===== 6. 模板元编程与constexpr结合 =====

// 编译期类型列表
template<typename... Types>
struct type_list {
    static constexpr size_t size = sizeof...(Types);
};

// 编译期条件
template<bool Condition, typename TrueType, typename FalseType>
struct conditional_constexpr {
    using type = TrueType;
    static constexpr bool value = true;
};

template<typename TrueType, typename FalseType>
struct conditional_constexpr<false, TrueType, FalseType> {
    using type = FalseType;
    static constexpr bool value = false;
};

// 编译期整数序列（C++14的std::integer_sequence的简化版）
template<int... Ints>
struct integer_sequence {
    static constexpr size_t size() { return sizeof...(Ints); }
};

// 编译期范围生成
template<int N, int... Ints>
struct make_integer_sequence : make_integer_sequence<N-1, N-1, Ints...> {};

template<int... Ints>
struct make_integer_sequence<0, Ints...> {
    using type = integer_sequence<Ints...>;
};

// 编译期累加器
template<int... Ints>
constexpr int sum_sequence(integer_sequence<Ints...>) {
    return (Ints + ...);  // C++17语法，C++11需要递归实现
}

// C++11兼容版本
template<int First, int... Rest>
constexpr int sum_integers() {
    return First + sum_integers<Rest...>();
}

template<>
constexpr int sum_integers<>() {
    return 0;
}

void demonstrate_metaprogramming_constexpr() {
    std::cout << "=== 模板元编程与constexpr结合演示 ===\n";
    
    // 编译期类型操作
    using types = type_list<int, double, char, std::string>;
    constexpr size_t type_count = types::size;
    
    std::cout << "类型列表大小: " << type_count << std::endl;
    
    // 编译期条件判断
    using result_type = conditional_constexpr<true, int, double>::type;
    constexpr bool condition_result = conditional_constexpr<sizeof(int) == 4, int, double>::value;
    
    std::cout << "条件结果: " << condition_result << std::endl;
    std::cout << "结果类型是int: " << std::is_same<result_type, int>::value << std::endl;
    
    // 编译期序列计算
    constexpr int sum_1_to_5 = sum_integers<1, 2, 3, 4, 5>();
    std::cout << "1+2+3+4+5 = " << sum_1_to_5 << std::endl;
    
    std::cout << "\n";
}

// ===== 7. 实际应用：编译期配置系统 =====

// 编译期配置
struct CompileTimeConfig {
    static constexpr bool DEBUG_MODE = true;
    static constexpr int MAX_CONNECTIONS = 1000;
    static constexpr double TIMEOUT_SECONDS = 30.0;
    static constexpr const char* DEFAULT_HOST = "localhost";
    static constexpr int DEFAULT_PORT = 8080;
    
    // 编译期验证
    static_assert(MAX_CONNECTIONS > 0, "最大连接数必须为正数");
    static_assert(TIMEOUT_SECONDS > 0.0, "超时时间必须为正数");
    static_assert(string_length(DEFAULT_HOST) > 0, "默认主机名不能为空");
};

// 基于配置的编译期优化
template<bool DebugMode>
class Logger {
public:
    static void log(const char* message) {
        if (DebugMode) {
            std::cout << "[DEBUG] " << message << std::endl;
        }
        // 在非调试模式下，编译器会优化掉整个函数体
    }
};

using ProductionLogger = Logger<false>;
using DebugLogger = Logger<CompileTimeConfig::DEBUG_MODE>;

// 编译期网络地址计算
constexpr uint32_t make_ipv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(b) << 16) |
           (static_cast<uint32_t>(c) << 8) |
           static_cast<uint32_t>(d);
}

void demonstrate_compile_time_config() {
    std::cout << "=== 编译期配置系统演示 ===\n";
    
    // 编译期配置使用
    constexpr int max_conn = CompileTimeConfig::MAX_CONNECTIONS;
    constexpr double timeout = CompileTimeConfig::TIMEOUT_SECONDS;
    
    std::cout << "最大连接数: " << max_conn << std::endl;
    std::cout << "超时时间: " << timeout << "秒" << std::endl;
    std::cout << "调试模式: " << (CompileTimeConfig::DEBUG_MODE ? "开启" : "关闭") << std::endl;
    
    // 条件编译的日志
    DebugLogger::log("这是一条调试信息");
    ProductionLogger::log("这条信息不会显示");
    
    // 编译期IP地址计算
    constexpr uint32_t localhost_ip = make_ipv4(127, 0, 0, 1);
    constexpr uint32_t broadcast_ip = make_ipv4(255, 255, 255, 255);
    
    std::cout << "本地主机IP: " << std::hex << localhost_ip << std::dec << std::endl;
    std::cout << "广播IP: " << std::hex << broadcast_ip << std::dec << std::endl;
    
    std::cout << "\n";
}

// ===== 8. 性能分析和最佳实践 =====

// 编译期 vs 运行时性能对比
constexpr long compile_time_factorial_10 = factorial(10);

long runtime_factorial(int n) {
    long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

void demonstrate_performance_analysis() {
    std::cout << "=== 性能分析和最佳实践 ===\n";
    
    // 编译期计算结果直接内嵌在代码中
    std::cout << "编译期计算factorial(10): " << compile_time_factorial_10 << std::endl;
    
    // 运行时计算
    long runtime_result = runtime_factorial(10);
    std::cout << "运行时计算factorial(10): " << runtime_result << std::endl;
    
    std::cout << "\nconstexpr最佳实践:\n";
    std::cout << "1. 尽可能使用constexpr减少运行时计算\n";
    std::cout << "2. 注意C++11 constexpr函数的单一返回语句限制\n";
    std::cout << "3. 合理设计字面值类型支持编译期对象创建\n";
    std::cout << "4. 使用static_assert进行编译期验证\n";
    std::cout << "5. 结合模板元编程实现复杂的编译期算法\n";
    std::cout << "6. 注意编译时间和代码复杂度的平衡\n";
    
    std::cout << "\n编译期计算的优势:\n";
    std::cout << "- 零运行时开销\n";
    std::cout << "- 编译期错误检测\n";
    std::cout << "- 更好的优化机会\n";
    std::cout << "- 类型安全的常量表达式\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 constexpr编译期计算深度解析\n";
    std::cout << "==================================\n";
    
    // 基础constexpr
    demonstrate_basic_constexpr();
    
    // 字面值类型
    demonstrate_literal_types();
    
    // 编译期数组
    demonstrate_compile_time_arrays();
    
    // 数学库
    demonstrate_math_library();
    
    // 字符串处理
    demonstrate_string_processing();
    
    // 模板元编程结合
    demonstrate_metaprogramming_constexpr();
    
    // 配置系统
    demonstrate_compile_time_config();
    
    // 性能分析
    demonstrate_performance_analysis();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 05_constexpr_compile_time.cpp -o constexpr_demo
./constexpr_demo

关键学习点:
1. 理解constexpr的编译期/运行时双重性质
2. 掌握C++11 constexpr函数的语法限制
3. 学会设计字面值类型支持编译期对象创建
4. 了解编译期算法的递归实现模式
5. 掌握constexpr与模板元编程的结合技巧
6. 学会使用static_assert进行编译期断言
7. 理解编译期计算对性能和代码质量的影响
8. 掌握实际项目中constexpr的应用场景
*/