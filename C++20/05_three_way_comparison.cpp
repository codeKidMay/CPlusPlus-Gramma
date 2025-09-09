/**
 * C++20 三向比较运算符深度解析
 * 
 * 核心概念：
 * 1. 宇宙飞船操作符 - <=> 的语法和语义设计
 * 2. 比较类别体系 - strong_ordering、weak_ordering、partial_ordering
 * 3. 自动比较生成 - 编译器自动生成所有比较运算符
 * 4. 自定义比较逻辑 - 复杂数据结构的比较策略设计
 * 5. 性能优化分析 - 单一比较函数的效率提升
 */

#include <iostream>
#include <compare>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <cmath>

// ===== 1. 宇宙飞船操作符基础演示 =====
// 简单的Point类展示默认比较
struct Point {
    int x, y;
    
    // 默认三向比较 - 自动生成所有六个比较运算符
    auto operator<=>(const Point& other) const = default;
    bool operator==(const Point& other) const = default;
};

// 手动实现三向比较的示例
struct ManualPoint {
    int x, y;
    
    std::strong_ordering operator<=>(const ManualPoint& other) const {
        if (auto cmp = x <=> other.x; cmp != 0) {
            return cmp;
        }
        return y <=> other.y;
    }
    
    bool operator==(const ManualPoint& other) const {
        return x == other.x && y == other.y;
    }
};

void demonstrate_spaceship_basics() {
    std::cout << "=== 宇宙飞船操作符基础演示 ===\n";
    
    Point p1{1, 2};
    Point p2{3, 4};
    Point p3{1, 2};
    
    std::cout << "Point比较演示:\n";
    std::cout << "p1(1,2) == p3(1,2): " << (p1 == p3) << "\n";
    std::cout << "p1(1,2) != p2(3,4): " << (p1 != p2) << "\n";
    std::cout << "p1(1,2) < p2(3,4): " << (p1 < p2) << "\n";
    std::cout << "p1(1,2) <= p3(1,2): " << (p1 <= p3) << "\n";
    std::cout << "p2(3,4) > p1(1,2): " << (p2 > p1) << "\n";
    std::cout << "p2(3,4) >= p1(1,2): " << (p2 >= p1) << "\n";
    
    // 直接使用三向比较运算符
    auto cmp_result = p1 <=> p2;
    std::cout << "\n三向比较结果分析:\n";
    std::cout << "p1 <=> p2 结果类型: std::strong_ordering\n";
    std::cout << "p1 < p2: " << (cmp_result < 0) << "\n";
    std::cout << "p1 == p2: " << (cmp_result == 0) << "\n";
    std::cout << "p1 > p2: " << (cmp_result > 0) << "\n";
    
    // 手动实现的比较
    ManualPoint mp1{1, 2};
    ManualPoint mp2{3, 4};
    
    std::cout << "\n手动实现的比较:\n";
    std::cout << "mp1 == mp2: " << (mp1 == mp2) << "\n";
    std::cout << "mp1 < mp2: " << (mp1 < mp2) << "\n";
    
    std::cout << "\n";
}

// ===== 2. 比较类别体系演示 =====
// Strong Ordering - 完全有序，相等意味着可替换
struct StrongOrderingExample {
    int value;
    
    std::strong_ordering operator<=>(const StrongOrderingExample& other) const {
        return value <=> other.value;
    }
    
    bool operator==(const StrongOrderingExample& other) const {
        return value == other.value;
    }
};

// Weak Ordering - 部分等价，相等的对象可能不完全相同
struct Person {
    std::string first_name;
    std::string last_name;
    int age;
    
    // 按姓名排序，年龄相同的人被认为是等价的（但不是相同的）
    std::weak_ordering operator<=>(const Person& other) const {
        if (auto cmp = last_name <=> other.last_name; cmp != 0) {
            return cmp;
        }
        if (auto cmp = first_name <=> other.first_name; cmp != 0) {
            return cmp;
        }
        // 年龄相同时返回equivalent而不是equal
        return std::weak_ordering::equivalent;
    }
    
    bool operator==(const Person& other) const {
        return first_name == other.first_name && 
               last_name == other.last_name;
               // 注意：这里不比较age，因为我们认为同名的人是"等价"的
    }
};

// Partial Ordering - 部分有序，某些元素可能无法比较
struct FloatingPoint {
    double value;
    
    std::partial_ordering operator<=>(const FloatingPoint& other) const {
        if (std::isnan(value) || std::isnan(other.value)) {
            return std::partial_ordering::unordered;  // NaN与任何数都无法比较
        }
        
        if (value < other.value) return std::partial_ordering::less;
        if (value > other.value) return std::partial_ordering::greater;
        return std::partial_ordering::equivalent;
    }
    
    bool operator==(const FloatingPoint& other) const {
        // NaN不等于任何值，包括自己
        if (std::isnan(value) || std::isnan(other.value)) {
            return false;
        }
        return value == other.value;
    }
};

void demonstrate_comparison_categories() {
    std::cout << "=== 比较类别体系演示 ===\n";
    
    // Strong Ordering演示
    std::cout << "Strong Ordering演示:\n";
    StrongOrderingExample s1{10};
    StrongOrderingExample s2{20};
    StrongOrderingExample s3{10};
    
    std::cout << "s1(10) == s3(10): " << (s1 == s3) << "\n";
    std::cout << "s1(10) < s2(20): " << (s1 < s2) << "\n";
    
    auto strong_cmp = s1 <=> s2;
    std::cout << "s1 <=> s2 是 strong_ordering::less: " << (strong_cmp == std::strong_ordering::less) << "\n";
    
    // Weak Ordering演示
    std::cout << "\nWeak Ordering演示:\n";
    Person p1{"John", "Doe", 25};
    Person p2{"John", "Doe", 30};  // 同名不同年龄
    Person p3{"Jane", "Smith", 25};
    
    std::cout << "同名不同年龄的人:\n";
    std::cout << "p1(John Doe, 25) == p2(John Doe, 30): " << (p1 == p2) << "\n";
    std::cout << "p1 <=> p2: ";
    auto weak_cmp = p1 <=> p2;
    if (weak_cmp == std::weak_ordering::equivalent) {
        std::cout << "equivalent (等价但不完全相同)\n";
    }
    
    std::cout << "不同姓名:\n";
    std::cout << "p1(John Doe) < p3(Jane Smith): " << (p1 < p3) << "\n";
    
    // Partial Ordering演示
    std::cout << "\nPartial Ordering演示:\n";
    FloatingPoint f1{3.14};
    FloatingPoint f2{2.71};
    FloatingPoint f3{std::numeric_limits<double>::quiet_NaN()};
    
    std::cout << "正常数值比较:\n";
    std::cout << "f1(3.14) > f2(2.71): " << (f1 > f2) << "\n";
    
    std::cout << "NaN的特殊情况:\n";
    std::cout << "f3(NaN) == f3(NaN): " << (f3 == f3) << " (NaN不等于自己)\n";
    
    auto partial_cmp = f1 <=> f3;
    std::cout << "f1 <=> f3(NaN): ";
    if (partial_cmp == std::partial_ordering::unordered) {
        std::cout << "unordered (无法比较)\n";
    }
    
    std::cout << "\n";
}

// ===== 3. 自动比较生成演示 =====
// 复杂结构的默认比较
struct ComplexData {
    std::string name;
    std::vector<int> values;
    double priority;
    
    // 默认比较会按成员声明顺序逐一比较
    auto operator<=>(const ComplexData& other) const = default;
    bool operator==(const ComplexData& other) const = default;
};

// 继承层次中的比较
struct Base {
    int base_value;
    
    auto operator<=>(const Base& other) const = default;
    bool operator==(const Base& other) const = default;
};

struct Derived : Base {
    std::string derived_value;
    
    // 自动比较会先比较基类，再比较派生类成员
    auto operator<=>(const Derived& other) const = default;
    bool operator==(const Derived& other) const = default;
};

void demonstrate_automatic_generation() {
    std::cout << "=== 自动比较生成演示 ===\n";
    
    // 复杂数据结构比较
    ComplexData cd1{"alpha", {1, 2, 3}, 1.5};
    ComplexData cd2{"alpha", {1, 2, 3}, 1.5};
    ComplexData cd3{"beta", {1, 2, 3}, 1.5};
    ComplexData cd4{"alpha", {1, 2, 4}, 1.5};  // values不同
    
    std::cout << "复杂数据结构比较:\n";
    std::cout << "cd1 == cd2 (完全相同): " << (cd1 == cd2) << "\n";
    std::cout << "cd1 == cd3 (name不同): " << (cd1 == cd3) << "\n";
    std::cout << "cd1 == cd4 (values不同): " << (cd1 == cd4) << "\n";
    std::cout << "cd1 < cd3 (按字典序): " << (cd1 < cd3) << "\n";
    std::cout << "cd1 < cd4 (vector比较): " << (cd1 < cd4) << "\n";
    
    // 继承层次比较
    Derived d1{{10}, "hello"};
    Derived d2{{10}, "hello"};
    Derived d3{{20}, "hello"};
    Derived d4{{10}, "world"};
    
    std::cout << "\n继承层次比较:\n";
    std::cout << "d1 == d2 (完全相同): " << (d1 == d2) << "\n";
    std::cout << "d1 == d3 (base不同): " << (d1 == d3) << "\n";
    std::cout << "d1 == d4 (derived不同): " << (d1 == d4) << "\n";
    std::cout << "d1 < d3 (base值更小): " << (d1 < d3) << "\n";
    std::cout << "d1 < d4 (base相同，derived字典序更小): " << (d1 < d4) << "\n";
    
    std::cout << "\n";
}

// ===== 4. 自定义比较逻辑演示 =====
// 版本号比较 - 语义化版本控制
struct Version {
    int major, minor, patch;
    std::string prerelease;  // 如 "alpha", "beta", "rc1"
    
    std::strong_ordering operator<=>(const Version& other) const {
        // 先比较主版本号
        if (auto cmp = major <=> other.major; cmp != 0) {
            return cmp;
        }
        
        // 再比较次版本号
        if (auto cmp = minor <=> other.minor; cmp != 0) {
            return cmp;
        }
        
        // 再比较补丁版本号
        if (auto cmp = patch <=> other.patch; cmp != 0) {
            return cmp;
        }
        
        // 预发布版本的特殊逻辑
        if (prerelease.empty() && !other.prerelease.empty()) {
            return std::strong_ordering::greater;  // 正式版本 > 预发布版本
        }
        if (!prerelease.empty() && other.prerelease.empty()) {
            return std::strong_ordering::less;     // 预发布版本 < 正式版本
        }
        
        // 都是预发布版本或都是正式版本
        return prerelease <=> other.prerelease;
    }
    
    bool operator==(const Version& other) const {
        return major == other.major && 
               minor == other.minor && 
               patch == other.patch && 
               prerelease == other.prerelease;
    }
    
    std::string to_string() const {
        std::string result = std::to_string(major) + "." + 
                           std::to_string(minor) + "." + 
                           std::to_string(patch);
        if (!prerelease.empty()) {
            result += "-" + prerelease;
        }
        return result;
    }
};

// 优先级队列元素 - 多条件排序
struct Task {
    std::string name;
    int priority;      // 数值越高优先级越高
    std::chrono::system_clock::time_point deadline;
    bool is_urgent;
    
    // 复杂的比较逻辑：紧急任务优先，然后按优先级，最后按截止时间
    std::strong_ordering operator<=>(const Task& other) const {
        // 紧急任务总是优先
        if (auto cmp = is_urgent <=> other.is_urgent; cmp != 0) {
            return cmp;
        }
        
        // 优先级高的优先（注意这里是反向比较）
        if (auto cmp = other.priority <=> priority; cmp != 0) {
            return cmp;
        }
        
        // 截止时间早的优先
        if (auto cmp = deadline <=> other.deadline; cmp != 0) {
            return cmp;
        }
        
        // 最后按名称排序
        return name <=> other.name;
    }
    
    bool operator==(const Task& other) const {
        return name == other.name && 
               priority == other.priority && 
               deadline == other.deadline && 
               is_urgent == other.is_urgent;
    }
};

// 地理坐标 - 基于距离的比较
struct GeoPoint {
    double latitude, longitude;
    
    // 到原点的距离比较
    std::weak_ordering operator<=>(const GeoPoint& other) const {
        double dist1 = std::sqrt(latitude * latitude + longitude * longitude);
        double dist2 = std::sqrt(other.latitude * other.latitude + other.longitude * other.longitude);
        
        const double epsilon = 1e-9;
        if (std::abs(dist1 - dist2) < epsilon) {
            return std::weak_ordering::equivalent;
        }
        
        return dist1 < dist2 ? std::weak_ordering::less : std::weak_ordering::greater;
    }
    
    bool operator==(const GeoPoint& other) const {
        const double epsilon = 1e-9;
        return std::abs(latitude - other.latitude) < epsilon && 
               std::abs(longitude - other.longitude) < epsilon;
    }
};

void demonstrate_custom_comparison_logic() {
    std::cout << "=== 自定义比较逻辑演示 ===\n";
    
    // 版本号比较
    std::cout << "版本号比较:\n";
    std::vector<Version> versions = {
        {2, 1, 0, ""},
        {2, 0, 1, ""},
        {2, 1, 0, "beta"},
        {2, 1, 0, "alpha"},
        {1, 9, 9, ""},
        {2, 1, 1, ""}
    };
    
    std::cout << "排序前:\n";
    for (const auto& v : versions) {
        std::cout << "  " << v.to_string() << "\n";
    }
    
    std::sort(versions.begin(), versions.end());
    
    std::cout << "排序后:\n";
    for (const auto& v : versions) {
        std::cout << "  " << v.to_string() << "\n";
    }
    
    // 任务优先级比较
    std::cout << "\n任务优先级比较:\n";
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto next_week = now + std::chrono::hours(168);
    
    std::vector<Task> tasks = {
        {"普通任务A", 3, next_week, false},
        {"紧急任务", 1, tomorrow, true},
        {"高优先级任务", 5, next_week, false},
        {"普通任务B", 3, tomorrow, false}
    };
    
    std::cout << "任务排序前:\n";
    for (const auto& task : tasks) {
        std::cout << "  " << task.name << " (优先级:" << task.priority 
                  << ", 紧急:" << (task.is_urgent ? "是" : "否") << ")\n";
    }
    
    std::sort(tasks.begin(), tasks.end(), std::greater<Task>{});  // 使用greater进行降序排列
    
    std::cout << "任务排序后(按优先级):\n";
    for (const auto& task : tasks) {
        std::cout << "  " << task.name << " (优先级:" << task.priority 
                  << ", 紧急:" << (task.is_urgent ? "是" : "否") << ")\n";
    }
    
    // 地理坐标比较
    std::cout << "\n地理坐标比较(按到原点距离):\n";
    std::vector<GeoPoint> points = {
        {3.0, 4.0},    // 距离 5
        {1.0, 1.0},    // 距离 √2 ≈ 1.414
        {0.0, 5.0},    // 距离 5 (与第一个点等距离)
        {2.0, 2.0}     // 距离 2√2 ≈ 2.828
    };
    
    std::sort(points.begin(), points.end());
    
    std::cout << "按距离排序后:\n";
    for (const auto& p : points) {
        double dist = std::sqrt(p.latitude * p.latitude + p.longitude * p.longitude);
        std::cout << "  (" << p.latitude << ", " << p.longitude 
                  << ") 距离: " << dist << "\n";
    }
    
    std::cout << "\n";
}

// ===== 5. 性能优化分析演示 =====
// 传统比较方式
struct TraditionalComparison {
    std::string data;
    
    bool operator==(const TraditionalComparison& other) const {
        return data == other.data;
    }
    
    bool operator!=(const TraditionalComparison& other) const {
        return !(*this == other);
    }
    
    bool operator<(const TraditionalComparison& other) const {
        return data < other.data;
    }
    
    bool operator<=(const TraditionalComparison& other) const {
        return *this < other || *this == other;
    }
    
    bool operator>(const TraditionalComparison& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const TraditionalComparison& other) const {
        return !(*this < other);
    }
};

// 三向比较方式
struct ModernComparison {
    std::string data;
    
    auto operator<=>(const ModernComparison& other) const {
        return data <=> other.data;
    }
    
    bool operator==(const ModernComparison& other) const {
        return data == other.data;
    }
};

// 性能测试函数
template<typename T>
void performance_test(const std::vector<T>& data, const std::string& name) {
    const int iterations = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    volatile int comparison_count = 0;  // 防止编译器优化
    
    // 执行大量比较操作
    for (int i = 0; i < iterations; ++i) {
        for (size_t j = 1; j < data.size(); ++j) {
            if (data[j-1] < data[j]) ++comparison_count;
            if (data[j-1] == data[j]) ++comparison_count;
            if (data[j-1] > data[j]) ++comparison_count;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << name << " 性能测试:\n";
    std::cout << "  比较次数: " << comparison_count << "\n";
    std::cout << "  耗时: " << duration.count() << " 微秒\n";
}

void demonstrate_performance_analysis() {
    std::cout << "=== 性能优化分析演示 ===\n";
    
    // 准备测试数据
    std::vector<std::string> string_data = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon"
    };
    
    std::vector<TraditionalComparison> traditional_data;
    std::vector<ModernComparison> modern_data;
    
    for (const auto& str : string_data) {
        traditional_data.push_back({str});
        modern_data.push_back({str});
    }
    
    // 性能测试
    performance_test(traditional_data, "传统比较方式");
    performance_test(modern_data, "三向比较方式");
    
    // 代码复杂度分析
    std::cout << "\n代码复杂度分析:\n";
    std::cout << "传统方式需要实现的运算符: ==, !=, <, <=, >, >= (6个)\n";
    std::cout << "三向比较方式需要实现: <=>, == (2个)\n";
    std::cout << "代码减少: " << (6.0 - 2.0) / 6.0 * 100 << "%\n";
    
    // 编译器生成的代码分析
    std::cout << "\n编译器优化效果:\n";
    std::cout << "- 传统方式每个运算符都需要单独的函数调用\n";
    std::cout << "- 三向比较方式编译器可以内联和优化单一比较逻辑\n";
    std::cout << "- 减少了代码重复和潜在的逻辑不一致问题\n";
    
    std::cout << "\n";
}

// 综合示例：排序算法效率比较
template<typename Container>
void sort_efficiency_demo(Container& data, const std::string& type_name) {
    auto start = std::chrono::high_resolution_clock::now();
    std::sort(data.begin(), data.end());
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << type_name << " 排序耗时: " << duration.count() << " 微秒\n";
}

void demonstrate_comprehensive_example() {
    std::cout << "=== 综合示例：排序效率比较 ===\n";
    
    const size_t data_size = 10000;
    
    // 生成随机数据
    std::vector<TraditionalComparison> traditional_data;
    std::vector<ModernComparison> modern_data;
    
    for (size_t i = 0; i < data_size; ++i) {
        std::string random_str = "string_" + std::to_string(rand() % 1000);
        traditional_data.push_back({random_str});
        modern_data.push_back({random_str});
    }
    
    // 排序效率测试
    sort_efficiency_demo(traditional_data, "传统比较");
    sort_efficiency_demo(modern_data, "三向比较");
    
    std::cout << "\n总结:\n";
    std::cout << "三向比较运算符的优势:\n";
    std::cout << "1. 代码简洁：只需实现两个运算符而不是六个\n";
    std::cout << "2. 性能优化：编译器可以更好地优化比较逻辑\n";
    std::cout << "3. 一致性：避免了手动实现多个运算符可能产生的逻辑不一致\n";
    std::cout << "4. 表达力：直接表达对象间的排序关系\n";
    std::cout << "5. 标准化：提供了统一的比较框架\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 三向比较运算符深度解析\n";
    std::cout << "=============================\n";
    
    demonstrate_spaceship_basics();
    demonstrate_comparison_categories();
    demonstrate_automatic_generation();
    demonstrate_custom_comparison_logic();
    demonstrate_performance_analysis();
    demonstrate_comprehensive_example();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 05_three_way_comparison.cpp -o three_way_comparison
./three_way_comparison

关键学习点:
1. 三向比较运算符<=>可以自动生成所有六个比较运算符
2. 三种比较类别适用于不同的比较语义和数学特性
3. 编译器可以为聚合类型自动生成比较逻辑
4. 自定义比较逻辑可以实现复杂的业务排序需求
5. 相比传统方式，三向比较在性能和代码简洁性上都有优势

注意事项:
- 三向比较和相等比较需要分别实现
- 默认比较按成员声明顺序进行
- 选择合适的比较类别对算法正确性很重要
- 自定义比较逻辑要确保传递性和一致性
- 浮点数比较需要特别处理NaN等特殊值
*/