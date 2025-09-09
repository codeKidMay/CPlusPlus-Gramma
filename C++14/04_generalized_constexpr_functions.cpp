/*
 * C++14 广义constexpr函数 - 深入解析
 * 
 * C++14对constexpr函数进行了重大扩展，从C++11的单表达式限制扩展到支持多语句、
 * 局部变量、循环和条件语句。这一改进使得编译时计算能力大幅提升。
 * 
 * 核心改进点：
 * 1. 多语句支持 - 不再限制为单一return语句
 * 2. 局部变量 - 支持局部对象创建和修改
 * 3. 控制流 - 支持if/switch/for/while等控制结构
 * 4. 成员函数 - constexpr成员函数可以修改对象状态
 * 5. 更复杂的算法 - 支持递归以外的算法实现方式
 */

#include <iostream>
#include <array>
#include <string_view>
#include <chrono>
#include <cmath>

namespace cpp14_constexpr {

// ===== C++11 vs C++14 constexpr 对比 =====

// C++11限制：只能是单一表达式
constexpr int cpp11_factorial(int n) {
    return (n <= 1) ? 1 : n * cpp11_factorial(n - 1);
}

// C++14扩展：支持多语句、局部变量、循环
constexpr int cpp14_factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// ===== 局部变量和修改操作 =====

constexpr int fibonacci_iterative(int n) {
    if (n <= 1) return n;
    
    int prev2 = 0;
    int prev1 = 1;
    int current = 0;
    
    for (int i = 2; i <= n; ++i) {
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }
    
    return current;
}

// 字符串处理的constexpr实现
constexpr size_t constexpr_strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    return len;
}

constexpr bool constexpr_strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        ++str1;
        ++str2;
    }
    return (*str1 == *str2);
}

// ===== constexpr成员函数和对象修改 =====

class Point {
private:
    double x_, y_;

public:
    constexpr Point(double x = 0.0, double y = 0.0) : x_(x), y_(y) {}
    
    constexpr double x() const { return x_; }
    constexpr double y() const { return y_; }
    
    // C++14: constexpr成员函数可以修改对象状态
    constexpr void set_x(double x) { x_ = x; }
    constexpr void set_y(double y) { y_ = y; }
    constexpr void translate(double dx, double dy) {
        x_ += dx;
        y_ += dy;
    }
    
    constexpr double distance_from_origin() const {
        return x_ * x_ + y_ * y_;  // 避免使用sqrt，保持constexpr
    }
    
    constexpr Point operator+(const Point& other) const {
        return Point(x_ + other.x_, y_ + other.y_);
    }
    
    constexpr Point& operator+=(const Point& other) {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }
};

// 编译时几何计算
constexpr Point calculate_centroid() {
    Point centroid(0.0, 0.0);
    std::array<Point, 4> points = {{
        {1.0, 1.0}, {3.0, 1.0}, {3.0, 3.0}, {1.0, 3.0}
    }};
    
    for (const auto& point : points) {
        centroid += point;
    }
    
    // 手动除法避免浮点运算精度问题
    constexpr double inv_size = 1.0 / 4.0;
    centroid.set_x(centroid.x() * inv_size);
    centroid.set_y(centroid.y() * inv_size);
    
    return centroid;
}

// ===== 复杂算法：编译时排序 =====

template<size_t N>
constexpr std::array<int, N> constexpr_bubble_sort(std::array<int, N> arr) {
    // 冒泡排序的constexpr实现
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                // C++14允许在constexpr中进行交换操作
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    return arr;
}

// 编译时快速排序实现
template<size_t N>
constexpr std::array<int, N> constexpr_quick_sort(std::array<int, N> arr) {
    if (N <= 1) return arr;
    
    // 简化的快排实现，选择第一个元素作为基准
    constexpr auto quick_sort_impl = [](auto arr, size_t low, size_t high) -> std::array<int, N> {
        if (low < high) {
            // 分区操作
            int pivot = arr[high];
            size_t i = low;
            
            for (size_t j = low; j < high; ++j) {
                if (arr[j] <= pivot) {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                    ++i;
                }
            }
            
            int temp = arr[i];
            arr[i] = arr[high];
            arr[high] = temp;
            
            // 递归排序（在constexpr中必须小心处理递归终止条件）
            if (i > 0) {
                arr = quick_sort_impl(arr, low, i - 1);
            }
            arr = quick_sort_impl(arr, i + 1, high);
        }
        return arr;
    };
    
    return quick_sort_impl(arr, 0, N - 1);
}

// ===== 编译时哈希表实现 =====

template<size_t Size>
class ConstexprHashMap {
private:
    struct Entry {
        const char* key;
        int value;
        bool used;
        
        constexpr Entry() : key(nullptr), value(0), used(false) {}
        constexpr Entry(const char* k, int v) : key(k), value(v), used(true) {}
    };
    
    std::array<Entry, Size> buckets_;
    
    constexpr size_t hash(const char* key) const {
        size_t hash_val = 5381;
        while (*key) {
            hash_val = ((hash_val << 5) + hash_val) + static_cast<unsigned char>(*key);
            ++key;
        }
        return hash_val % Size;
    }
    
public:
    constexpr ConstexprHashMap() : buckets_{} {}
    
    constexpr void insert(const char* key, int value) {
        size_t index = hash(key);
        size_t original_index = index;
        
        // 线性探测处理冲突
        while (buckets_[index].used) {
            if (constexpr_strcmp(buckets_[index].key, key)) {
                buckets_[index].value = value;  // 更新现有键
                return;
            }
            index = (index + 1) % Size;
            if (index == original_index) {
                // 哈希表已满，在实际应用中应该处理这种情况
                return;
            }
        }
        
        buckets_[index] = Entry(key, value);
    }
    
    constexpr int get(const char* key) const {
        size_t index = hash(key);
        size_t original_index = index;
        
        while (buckets_[index].used) {
            if (constexpr_strcmp(buckets_[index].key, key)) {
                return buckets_[index].value;
            }
            index = (index + 1) % Size;
            if (index == original_index) break;
        }
        
        return -1;  // 未找到
    }
};

// 编译时配置系统
constexpr auto create_config_map() {
    ConstexprHashMap<16> config;
    config.insert("max_connections", 1000);
    config.insert("timeout_ms", 5000);
    config.insert("buffer_size", 8192);
    config.insert("thread_pool_size", 8);
    return config;
}

// ===== 编译时数学计算 =====

// 牛顿法求平方根（编译时版本）
constexpr double constexpr_sqrt(double x, double guess = 1.0, int iterations = 10) {
    if (iterations == 0 || (guess * guess - x) * (guess * guess - x) < 1e-10) {
        return guess;
    }
    return constexpr_sqrt(x, (guess + x / guess) / 2.0, iterations - 1);
}

// 编译时三角函数近似（泰勒级数）
constexpr double constexpr_sin(double x, int terms = 10) {
    double result = 0.0;
    double term = x;
    double x_squared = x * x;
    
    for (int i = 0; i < terms; ++i) {
        if (i % 2 == 0) {
            result += term;
        } else {
            result -= term;
        }
        
        term *= x_squared / ((2 * i + 2) * (2 * i + 3));
    }
    
    return result;
}

// ===== 编译时字符串处理和验证 =====

constexpr bool is_valid_email(const char* email) {
    // 简化的邮箱验证逻辑
    size_t len = constexpr_strlen(email);
    if (len < 3) return false;
    
    bool has_at = false;
    bool has_dot_after_at = false;
    size_t at_pos = 0;
    
    for (size_t i = 0; i < len; ++i) {
        char c = email[i];
        
        if (c == '@') {
            if (has_at || i == 0 || i == len - 1) return false;
            has_at = true;
            at_pos = i;
        } else if (c == '.' && has_at && i > at_pos + 1 && i < len - 1) {
            has_dot_after_at = true;
        }
    }
    
    return has_at && has_dot_after_at;
}

// 编译时正则表达式匹配（简化版）
constexpr bool simple_wildcard_match(const char* pattern, const char* text) {
    while (*pattern && *text) {
        if (*pattern == '*') {
            ++pattern;
            if (*pattern == '\0') return true;
            
            while (*text) {
                if (simple_wildcard_match(pattern, text)) {
                    return true;
                }
                ++text;
            }
            return false;
        } else if (*pattern == '?' || *pattern == *text) {
            ++pattern;
            ++text;
        } else {
            return false;
        }
    }
    
    while (*pattern == '*') ++pattern;
    return (*pattern == '\0' && *text == '\0');
}

// ===== 性能测试和基准测试 =====

template<typename Func>
void benchmark_constexpr(const char* name, Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    
    constexpr int iterations = 1000000;
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    std::cout << name << ": " << duration.count() / iterations << " ns per call\n";
}

void demonstrate_constexpr_benefits() {
    std::cout << "\n===== C++14 Constexpr 函数性能对比 =====\n";
    
    // 编译时计算的值在程序中是常量
    constexpr int fact_result = cpp14_factorial(10);
    constexpr int fib_result = fibonacci_iterative(20);
    constexpr auto sorted_array = constexpr_bubble_sort(std::array<int, 8>{{8, 3, 1, 4, 5, 9, 2, 6}});
    constexpr auto config = create_config_map();
    constexpr double sqrt_result = constexpr_sqrt(16.0);
    
    std::cout << "编译时计算结果:\n";
    std::cout << "10! = " << fact_result << "\n";
    std::cout << "fibonacci(20) = " << fib_result << "\n";
    std::cout << "sqrt(16) = " << sqrt_result << "\n";
    std::cout << "sorted array: ";
    for (auto val : sorted_array) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    
    // 配置系统测试
    std::cout << "配置值: max_connections = " << config.get("max_connections") << "\n";
    std::cout << "配置值: timeout_ms = " << config.get("timeout_ms") << "\n";
    
    // 字符串验证测试
    constexpr bool email1_valid = is_valid_email("user@example.com");
    constexpr bool email2_valid = is_valid_email("invalid.email");
    std::cout << "邮箱验证: user@example.com -> " << (email1_valid ? "有效" : "无效") << "\n";
    std::cout << "邮箱验证: invalid.email -> " << (email2_valid ? "有效" : "无效") << "\n";
    
    // 通配符匹配测试
    constexpr bool match1 = simple_wildcard_match("*.cpp", "test.cpp");
    constexpr bool match2 = simple_wildcard_match("test*", "testing.txt");
    std::cout << "通配符匹配: *.cpp vs test.cpp -> " << (match1 ? "匹配" : "不匹配") << "\n";
    std::cout << "通配符匹配: test* vs testing.txt -> " << (match2 ? "匹配" : "不匹配") << "\n";
}

// ===== 设计原理和最佳实践 =====

/*
 * C++14 constexpr扩展的设计原理：
 * 
 * 1. 编译时计算能力扩展
 *    - C++11的单表达式限制过于严格
 *    - 实际应用需要更复杂的编译时算法
 *    - 循环和条件语句是算法实现的基础
 * 
 * 2. 性能优化
 *    - 编译时计算消除运行时开销
 *    - 常量折叠和内联优化机会增加
 *    - 减少动态内存分配和函数调用
 * 
 * 3. 类型安全和错误检查
 *    - 编译时验证输入参数的有效性
 *    - 模板元编程的补充和简化
 *    - 更好的错误信息和诊断
 * 
 * 最佳实践：
 * 
 * 1. constexpr函数设计原则
 *    - 保持函数的纯函数特性（除了局部状态修改）
 *    - 避免依赖运行时才能确定的值
 *    - 合理使用递归，注意编译器的递归深度限制
 * 
 * 2. 性能考虑
 *    - constexpr不保证编译时求值，需要constexpr上下文
 *    - 复杂算法可能增加编译时间
 *    - 平衡编译时计算和运行时性能
 * 
 * 3. 可维护性
 *    - constexpr函数也要考虑可读性
 *    - 提供非constexpr版本作为运行时备选
 *    - 适当的单元测试验证正确性
 */

} // namespace cpp14_constexpr

// ===== 编译器实现细节 =====

/*
 * 编译器如何处理C++14 constexpr：
 * 
 * 1. 常量表达式求值器（Constant Expression Evaluator）
 *    - 编译器内置解释器，模拟程序执行
 *    - 跟踪变量状态和内存分配
 *    - 处理控制流和函数调用栈
 * 
 * 2. 编译时内存模型
 *    - 局部变量在编译时栈上分配
 *    - 对象生命周期管理和构造/析构
 *    - 指针和引用的编译时处理
 * 
 * 3. 优化策略
 *    - 常量传播和死代码消除
 *    - 循环展开和内联展开
 *    - 模板实例化时的常量折叠
 * 
 * 4. 限制和约束
 *    - 不能调用非constexpr函数
 *    - 不能使用动态内存分配
 *    - 不能访问全局/静态变量（除非是constexpr）
 *    - 递归深度限制（通常512-1024层）
 */

int main() {
    std::cout << "=== C++14 广义constexpr函数示例 ===\n";
    
    cpp14_constexpr::demonstrate_constexpr_benefits();
    
    std::cout << "\n===== 几何计算示例 =====\n";
    constexpr auto centroid = cpp14_constexpr::calculate_centroid();
    std::cout << "四边形重心: (" << centroid.x() << ", " << centroid.y() << ")\n";
    
    std::cout << "\n===== 编译时字符串处理 =====\n";
    constexpr size_t len = cpp14_constexpr::constexpr_strlen("Hello, World!");
    std::cout << "字符串长度: " << len << "\n";
    
    constexpr bool same = cpp14_constexpr::constexpr_strcmp("test", "test");
    std::cout << "字符串比较: " << (same ? "相等" : "不相等") << "\n";
    
    return 0;
}