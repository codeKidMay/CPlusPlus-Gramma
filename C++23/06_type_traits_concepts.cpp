/**
 * C++23 新的类型特征和概念深度解析
 * 
 * 核心概念：
 * 1. 类型检测增强 - is_scoped_enum、is_implicit_lifetime等新特征
 * 2. 引用构造检测 - reference_constructs_from_temporary防止悬垂引用
 * 3. 概念约束系统 - 更精确的类型约束和编译期检查
 * 4. 元编程工具 - 增强的编译期计算和类型推导
 * 5. 标准库概念 - 新增的实用概念和类型约束
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>
#include <variant>
#include <optional>
#include <concepts>
#include <type_traits>
#include <memory>
#include <utility>
#include <complex>
#include <ranges>
#include <iterator>

// ===== 1. 新的类型特征 =====
void demonstrate_new_type_traits() {
    std::cout << "=== 新的类型特征 ===\n";
    
    // std::is_scoped_enum - 检测作用域枚举
    enum class Color { Red, Green, Blue };
    enum OldColor { RED, GREEN, BLUE };
    
    std::cout << "is_scoped_enum检测:\n";
    std::cout << "Color (enum class): " << std::is_scoped_enum_v<Color> << "\n";
    std::cout << "OldColor (传统enum): " << std::is_scoped_enum_v<OldColor> << "\n\n";
    
    // std::is_implicit_lifetime - 检测隐式生命周期类型
    std::cout << "is_implicit_lifetime检测:\n";
    std::cout << "int: " << std::is_implicit_lifetime_v<int> << "\n";
    std::cout << "std::string: " << std::is_implicit_lifetime_v<std::string> << "\n";
    std::cout << "std::vector<int>: " << std::is_implicit_lifetime_v<std::vector<int>> << "\n\n";
    
    // std::reference_constructs_from_temporary - 防止悬垂引用
    std::cout << "reference_constructs_from_temporary检测:\n";
    std::cout << "const std::string& from const char*: " 
              << std::reference_constructs_from_temporary_v<const std::string&, const char*> << "\n";
    std::cout << "const std::string& from std::string: " 
              << std::reference_constructs_from_temporary_v<const std::string&, std::string> << "\n";
    std::cout << "std::string_view from const char*: " 
              << std::reference_constructs_from_temporary_v<std::string_view, const char*> << "\n\n";
    
    // std::convertible_to_ref - 转换为引用的检测
    std::cout << "convertible_to_ref检测:\n";
    std::cout << "int -> int&: " << std::convertible_to_ref_v<int, int&> << "\n";
    std::cout << "int -> const int&: " << std::convertible_to_ref_v<int, const int&> << "\n";
    std::cout << "const int -> int&: " << std::convertible_to_ref_v<const int, int&> << "\n\n";
}

// ===== 2. 增强的概念系统 =====
// 基础数值概念
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 可迭代概念
template<typename T>
concept Iterable = requires(T t) {
    { std::begin(t) } -> std::input_iterator;
    { std::end(t) } -> std::sentinel_for<decltype(std::begin(t))>;
};

// 可索引概念
template<typename T>
concept Indexable = Iterable<T> && requires(T t, size_t index) {
    { t[index] };
};

// 数值范围概念
template<typename R>
concept NumericRange = std::ranges::range<R> && 
                       Numeric<std::ranges::range_value_t<R>>;

// 容器概念
template<typename C>
concept Container = requires(C c) {
    typename C::value_type;
    typename C::size_type;
    { c.size() } -> std::convertible_to<typename C::size_type>;
    { c.empty() } -> std::convertible_to<bool>;
    { c.begin() } -> std::input_iterator;
    { c.end() } -> std::sentinel_for<decltype(c.begin())>;
};

void demonstrate_enhanced_concepts() {
    std::cout << "=== 增强的概念系统 ===\n";
    
    // 测试数值概念
    std::cout << "Numeric概念测试:\n";
    std::cout << "int: " << Numeric<int> << "\n";
    std::cout << "double: " << Numeric<double> << "\n";
    std::cout << "std::string: " << Numeric<std::string> << "\n\n";
    
    // 测试可迭代概念
    std::cout << "Iterable概念测试:\n";
    std::cout << "std::vector<int>: " << Iterable<std::vector<int>> << "\n";
    std::cout << "std::array<int, 5>: " << Iterable<std::array<int, 5>> << "\n";
    std::cout << "int: " << Iterable<int> << "\n\n";
    
    // 测试可索引概念
    std::cout << "Indexable概念测试:\n";
    std::cout << "std::vector<int>: " << Indexable<std::vector<int>> << "\n";
    std::cout << "std::list<int>: " << Indexable<std::list<int>> << "\n";
    std::cout << "std::map<int, int>: " << Indexable<std::map<int, int>> << "\n\n";
    
    // 测试数值范围概念
    std::cout << "NumericRange概念测试:\n";
    std::vector<int> int_vec{1, 2, 3, 4, 5};
    std::vector<std::string> str_vec{"a", "b", "c"};
    
    std::cout << "vector<int>: " << NumericRange<decltype(int_vec)> << "\n";
    std::cout << "vector<string>: " << NumericRange<decltype(str_vec)> << "\n\n";
}

// ===== 3. 实用概念模板 =====
// 可哈希概念
template<typename T>
concept Hashable = requires(T a, T b) {
    { std::hash<T>{}(a) } -> std::convertible_to<size_t>;
    { a == b } -> std::convertible_to<bool>;
};

// 可比较概念
template<typename T>
concept Comparable = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
    { a < b } -> std::convertible_to<bool>;
    { a <= b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
    { a >= b } -> std::convertible_to<bool>;
};

// 可流输出概念
template<typename T>
concept Streamable = requires(std::ostream& os, T value) {
    { os << value } -> std::convertible_to<std::ostream&>;
};

// 可构造概念
template<typename T, typename... Args>
concept ConstructibleFrom = std::constructible_from<T, Args...>;

// 可转换概念
template<typename From, typename To>
concept ConvertibleTo = std::convertible_to<From, To>;

void demonstrate_practical_concepts() {
    std::cout << "=== 实用概念模板 ===\n";
    
    // 测试可哈希概念
    std::cout << "Hashable概念测试:\n";
    std::cout << "int: " << Hashable<int> << "\n";
    std::cout << "std::string: " << Hashable<std::string> << "\n";
    std::cout << "std::vector<int>: " << Hashable<std::vector<int>> << "\n\n";
    
    // 测试可比较概念
    std::cout << "Comparable概念测试:\n";
    std::cout << "int: " << Comparable<int> << "\n";
    std::cout << "std::string: " << Comparable<std::string> << "\n";
    std::cout << "std::complex<double>: " << Comparable<std::complex<double>> << "\n\n";
    
    // 测试可流输出概念
    std::cout << "Streamable概念测试:\n";
    std::cout << "int: " << Streamable<int> << "\n";
    std::cout << "std::string: " << Streamable<std::string> << "\n";
    std::cout << "std::vector<int>: " << Streamable<std::vector<int>> << "\n\n";
}

// ===== 4. 约束模板示例 =====
// 约束的数值函数模板
template<Numeric T>
T constrained_add(T a, T b) {
    return a + b;
}

// 约束的范围处理模板
template<NumericRange R>
auto constrained_sum(const R& range) {
    using ValueType = std::ranges::range_value_t<R>;
    return std::accumulate(std::ranges::begin(range), 
                          std::ranges::end(range), 
                          ValueType{});
}

// 约束的容器操作模板
template<Container C>
auto constrained_find(const C& container, const typename C::value_type& value) {
    return std::find(container.begin(), container.end(), value);
}

// 约束的算法模板
template<Iterable I, typename T>
requires requires(I iter, T value) {
    { *iter } -> std::convertible_to<T>;
}
auto constrained_find_if(I begin, I end, T value) {
    for (auto it = begin; it != end; ++it) {
        if (*it == value) {
            return it;
        }
    }
    return end;
}

void demonstrate_constrained_templates() {
    std::cout << "=== 约束模板示例 ===\n";
    
    // 测试约束的数值函数
    std::cout << "约束的数值函数:\n";
    std::cout << "constrained_add(3, 4): " << constrained_add(3, 4) << "\n";
    std::cout << "constrained_add(3.5, 2.5): " << constrained_add(3.5, 2.5) << "\n";
    
    // 下面这行会导致编译错误，因为std::string不是Numeric类型
    // constrained_add(std::string("a"), std::string("b"));
    
    // 测试约束的范围处理
    std::cout << "\n约束的范围处理:\n";
    std::vector<int> numbers{1, 2, 3, 4, 5};
    std::cout << "sum of {1,2,3,4,5}: " << constrained_sum(numbers) << "\n";
    
    std::vector<double> doubles{1.1, 2.2, 3.3};
    std::cout << "sum of {1.1,2.2,3.3}: " << constrained_sum(doubles) << "\n";
    
    // 测试约束的容器操作
    std::cout << "\n约束的容器操作:\n";
    std::vector<std::string> names{"Alice", "Bob", "Charlie"};
    auto it = constrained_find(names, std::string("Bob"));
    if (it != names.end()) {
        std::cout << "Found 'Bob' at position: " << std::distance(names.begin(), it) << "\n";
    }
    
    std::cout << "\n";
}

// ===== 5. 高级概念组合 =====
// 可排序概念
template<typename T>
concept Sortable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
    { a <= b } -> std::convertible_to<bool>;
    { a >= b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
} && std::swappable<T>;

// 可排序范围概念
template<typename R>
concept SortableRange = std::ranges::range<R> && 
                        Sortable<std::ranges::range_value_t<R>>;

// 可哈希范围概念
template<typename R>
concept HashableRange = std::ranges::range<R> && 
                        Hashable<std::ranges::range_value_t<R>>;

// 约束的排序算法
template<SortableRange R>
void constrained_sort(R& range) {
    std::sort(std::ranges::begin(range), std::ranges::end(range));
}

// 约束的哈希计算
template<HashableRange R>
size_t constrained_hash_range(const R& range) {
    size_t seed = 0;
    for (const auto& value : range) {
        seed ^= std::hash<std::ranges::range_value_t<R>>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

void demonstrate_advanced_concept_combinations() {
    std::cout << "=== 高级概念组合 ===\n";
    
    // 测试可排序概念
    std::cout << "Sortable概念测试:\n";
    std::cout << "int: " << Sortable<int> << "\n";
    std::cout << "std::string: " << Sortable<std::string> << "\n";
    std::cout << "std::complex<double>: " << Sortable<std::complex<double>> << "\n\n";
    
    // 测试约束的排序算法
    std::cout << "约束的排序算法:\n";
    std::vector<int> unsorted{5, 2, 8, 1, 9};
    std::cout << "排序前: ";
    for (int x : unsorted) std::cout << x << " ";
    std::cout << "\n";
    
    constrained_sort(unsorted);
    std::cout << "排序后: ";
    for (int x : unsorted) std::cout << x << " ";
    std::cout << "\n\n";
    
    // 测试约束的哈希计算
    std::cout << "约束的哈希计算:\n";
    std::vector<std::string> words{"hello", "world", "cpp23"};
    size_t hash1 = constrained_hash_range(words);
    std::cout << "Hash of ['hello','world','cpp23']: " << hash1 << "\n";
    
    std::vector<std::string> words2{"hello", "world", "cpp23"};
    size_t hash2 = constrained_hash_range(words2);
    std::cout << "Hash of ['hello','world','cpp23']: " << hash2 << "\n";
    std::cout << "Hash equality: " << (hash1 == hash2 ? "true" : "false") << "\n\n";
}

// ===== 6. 自定义类型的概念支持 =====
// 支持概念的自定义类型
class Point {
public:
    double x, y;
    
    Point(double x, double y) : x(x), y(y) {}
    
    // 支持相等比较
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    // 支持流输出
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << "Point(" << p.x << ", " << p.y << ")";
        return os;
    }
    
    // 支持哈希
    size_t hash() const {
        return std::hash<double>{}(x) ^ std::hash<double>{}(y);
    }
};

// 为Point类型特化hash
template<>
struct std::hash<Point> {
    size_t operator()(const Point& p) const {
        return p.hash();
    }
};

// 支持概念的容器
class SafeVector {
private:
    std::vector<int> data_;
    
public:
    SafeVector() = default;
    explicit SafeVector(size_t size) : data_(size) {}
    
    // 容器接口
    using value_type = int;
    using size_type = size_t;
    using iterator = std::vector<int>::iterator;
    using const_iterator = std::vector<int>::const_iterator;
    
    auto size() const { return data_.size(); }
    auto empty() const { return data_.empty(); }
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // 支持索引访问
    auto operator[](size_t index) { return data_[index]; }
    auto operator[](size_t index) const { return data_[index]; }
    
    // 边界检查
    auto at(size_t index) { return data_.at(index); }
    auto at(size_t index) const { return data_.at(index); }
};

void demonstrate_custom_type_concepts() {
    std::cout << "=== 自定义类型的概念支持 ===\n";
    
    // 测试Point类型的概念支持
    std::cout << "Point类型概念测试:\n";
    std::cout << "Comparable: " << Comparable<Point> << "\n";
    std::cout << "Hashable: " << Hashable<Point> << "\n";
    std::cout << "Streamable: " << Streamable<Point> << "\n\n";
    
    // 测试SafeVector的概念支持
    std::cout << "SafeVector概念测试:\n";
    std::cout << "Container: " << Container<SafeVector> << "\n";
    std::cout << "Iterable: " << Iterable<SafeVector> << "\n";
    std::cout << "Indexable: " << Indexable<SafeVector> << "\n\n";
    
    // 使用支持概念的自定义类型
    std::vector<Point> points{Point(1.0, 2.0), Point(3.0, 4.0), Point(5.0, 6.0)};
    std::cout << "Point vector:\n";
    for (const auto& p : points) {
        std::cout << "  " << p << "\n";
    }
    
    // 测试Point的哈希
    std::cout << "\nPoint哈希测试:\n";
    std::cout << "Hash of Point(1,2): " << std::hash<Point>{}(Point(1.0, 2.0)) << "\n";
    std::cout << "Hash of Point(1,2): " << std::hash<Point>{}(Point(1.0, 2.0)) << "\n";
    
    // 测试SafeVector
    SafeVector safe_vec(5);
    std::cout << "\nSafeVector测试:\n";
    std::cout << "Size: " << safe_vec.size() << "\n";
    std::cout << "Empty: " << safe_vec.empty() << "\n";
    std::cout << "Elements: ";
    for (int x : safe_vec) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++23 新的类型特征和概念深度解析\n";
    std::cout << "====================================\n";
    
    demonstrate_new_type_traits();
    demonstrate_enhanced_concepts();
    demonstrate_practical_concepts();
    demonstrate_constrained_templates();
    demonstrate_advanced_concept_combinations();
    demonstrate_custom_type_concepts();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall 06_type_traits_concepts.cpp -o concepts_demo
./concepts_demo

注意：C++23的类型特征和概念增强需要编译器支持：
- GCC 12+: 大部分支持
- Clang 15+: 大部分支持
- MSVC 19.32+: 大部分支持

某些特性可能需要特定的编译器版本或实验性标志。

关键学习点:
1. 新的类型特征提供了更精确的类型检测能力
2. 增强的概念系统支持更复杂的类型约束
3. 约束模板提供了更好的类型安全和编译期检查
4. 概念组合可以构建强大的类型系统
5. 自定义类型可以很好地支持现代概念系统

注意事项:
- 概念约束应该简洁明了，避免过度复杂
- 类型特征主要用于模板元编程和编译期检查
- 概念约束可以提高代码的可读性和错误信息质量
- 在设计API时优先使用概念而非enable_if
- 概念支持是C++20/23的重要特性，应该充分利用
*/