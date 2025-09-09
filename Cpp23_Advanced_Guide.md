# C++23 深度语法手册

## 设计哲学：实用主义的成熟期

C++23是一个相对稳健的版本，专注于实用性改进和标准库的完善，体现了C++语言发展的成熟期特征：
1. **错误处理现代化**：std::expected提供函数式错误处理范式
2. **多维计算支持**：std::mdspan原生支持多维数组和科学计算
3. **输出系统完善**：std::print系列提供现代化的格式化输出
4. **Ranges生态完善**：新增大量实用的视图和算法
5. **开发体验优化**：众多小而美的改进提升日常开发效率

## 1. std::expected：函数式错误处理

### 错误处理范式的演进
```cpp
#include <expected>
#include <string>
#include <system_error>

// 传统错误处理方式的问题
int divide_traditional(int a, int b, int& result) {
    if (b == 0) return -1;  // 错误代码含义不清晰
    result = a / b;
    return 0;  // 成功
}

// C++17 std::optional的局限性
std::optional<int> divide_optional(int a, int b) {
    if (b == 0) return std::nullopt;  // 丢失了错误信息
    return a / b;
}

// C++23 std::expected的优雅方案
std::expected<int, std::string> divide_expected(int a, int b) {
    if (b == 0) 
        return std::unexpected("Division by zero");
    return a / b;
}
```

### expected的组合式错误处理
```cpp
enum class MathError {
    DivisionByZero,
    NegativeSquareRoot,
    Overflow
};

std::expected<double, MathError> safe_sqrt(double x) {
    if (x < 0) return std::unexpected(MathError::NegativeSquareRoot);
    return std::sqrt(x);
}

std::expected<double, MathError> safe_divide(double a, double b) {
    if (b == 0.0) return std::unexpected(MathError::DivisionByZero);
    return a / b;
}

// 错误传播的组合式处理
std::expected<double, MathError> complex_calculation(double a, double b, double c) {
    return safe_divide(a, b)
        .and_then([c](double result) { return safe_sqrt(result + c); })
        .or_else([](MathError error) -> std::expected<double, MathError> {
            // 错误恢复逻辑
            switch (error) {
                case MathError::DivisionByZero:
                    return 0.0;  // 默认值
                default:
                    return std::unexpected(error);  // 传播错误
            }
        });
}
```

### expected与异常的性能对比
```cpp
// 基于异常的错误处理
class DivisionByZeroException : public std::exception {
    const char* what() const noexcept override {
        return "Division by zero";
    }
};

double divide_exception(double a, double b) {
    if (b == 0.0) throw DivisionByZeroException{};
    return a / b;
}

// 基于expected的错误处理
std::expected<double, std::string> divide_expected_perf(double a, double b) {
    if (b == 0.0) return std::unexpected("Division by zero");
    return a / b;
}

// 性能测试对比
void performance_comparison() {
    // expected的优势：
    // 1. 零开销的正常路径执行
    // 2. 无栈展开开销
    // 3. 编译器优化友好
    // 4. 缓存友好的内存布局
    
    // 异常的劣势：
    // 1. 栈展开的昂贵开销
    // 2. 编译器优化困难
    // 3. 代码大小膨胀
    // 4. 异常安全代码的复杂性
}
```

### 自定义错误类型的最佳实践
```cpp
// 结构化错误信息
struct ValidationError {
    std::string field_name;
    std::string error_message;
    std::source_location location;
    
    ValidationError(std::string field, std::string msg, 
                   std::source_location loc = std::source_location::current())
        : field_name(std::move(field)), error_message(std::move(msg)), location(loc) {}
};

// 可组合的错误处理器
template<typename T, typename E>
class ResultChain {
    std::expected<T, E> value_;
    
public:
    explicit ResultChain(std::expected<T, E> value) : value_(std::move(value)) {}
    
    template<typename F>
    auto map(F&& func) -> ResultChain<decltype(func(std::declval<T>())), E> {
        if (value_) {
            return ResultChain<decltype(func(*value_)), E>{func(*value_)};
        }
        return ResultChain<decltype(func(std::declval<T>())), E>{std::unexpected(value_.error())};
    }
    
    template<typename F>
    auto flat_map(F&& func) -> decltype(func(std::declval<T>())) {
        if (value_) {
            return func(*value_);
        }
        using ResultType = decltype(func(std::declval<T>()));
        return ResultType{std::unexpected(value_.error())};
    }
    
    T value_or(T default_value) const {
        return value_.value_or(std::move(default_value));
    }
};
```

## 2. std::mdspan：多维数组的标准化

### 科学计算的现代化支持
```cpp
#include <mdspan>
#include <vector>
#include <array>

// 传统多维数组的问题
void traditional_2d_array() {
    // 1. C风格数组：大小固定，不安全
    int matrix[3][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
    
    // 2. vector<vector>：性能差，内存不连续
    std::vector<std::vector<int>> matrix_vec(3, std::vector<int>(4));
    
    // 3. 手动索引计算：容易出错
    std::vector<int> flat_matrix(12);
    auto get_element = [&](size_t row, size_t col) -> int& {
        return flat_matrix[row * 4 + col];  // 手动计算索引
    };
}

// C++23 std::mdspan的解决方案
void modern_mdspan_approach() {
    // 连续内存存储
    std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    
    // 创建2D视图
    std::mdspan<int, std::dextents<size_t, 2>> matrix(data.data(), 3, 4);
    
    // 自然的多维索引
    for (size_t i = 0; i < matrix.extent(0); ++i) {
        for (size_t j = 0; j < matrix.extent(1); ++j) {
            std::cout << matrix[i, j] << " ";  // 直观的多维索引
        }
        std::cout << "\n";
    }
}
```

### 自定义布局策略
```cpp
// 行主序布局（C/C++默认）
using row_major_2d = std::mdspan<double, std::dextents<size_t, 2>, 
                                 std::layout_right>;

// 列主序布局（Fortran风格）
using col_major_2d = std::mdspan<double, std::dextents<size_t, 2>, 
                                 std::layout_left>;

// 跨步布局（自定义步长）
using strided_2d = std::mdspan<double, std::dextents<size_t, 2>, 
                               std::layout_stride>;

void layout_comparison() {
    std::vector<double> data(12);
    std::iota(data.begin(), data.end(), 1.0);
    
    // 行主序：data[row * cols + col]
    row_major_2d row_matrix(data.data(), 3, 4);
    
    // 列主序：data[col * rows + row]
    col_major_2d col_matrix(data.data(), 3, 4);
    
    // 跨步布局：自定义访问模式
    std::array<size_t, 2> extents{3, 4};
    std::array<size_t, 2> strides{8, 2};  // 自定义步长
    strided_2d strided_matrix(data.data(), 
                              std::layout_stride::mapping{extents, strides});
}
```

### 高维张量和科学计算
```cpp
// 3D张量表示
template<typename T>
class Tensor3D {
    std::vector<T> data_;
    std::mdspan<T, std::dextents<size_t, 3>> view_;
    
public:
    Tensor3D(size_t depth, size_t height, size_t width) 
        : data_(depth * height * width), view_(data_.data(), depth, height, width) {}
    
    // 自然的3D索引访问
    T& operator()(size_t d, size_t h, size_t w) {
        return view_[d, h, w];
    }
    
    const T& operator()(size_t d, size_t h, size_t w) const {
        return view_[d, h, w];
    }
    
    // 切片操作
    auto slice(size_t depth_idx) {
        return std::submdspan(view_, depth_idx, std::full_extent, std::full_extent);
    }
    
    // 张量运算
    Tensor3D& operator+=(const Tensor3D& other) {
        for (size_t d = 0; d < view_.extent(0); ++d) {
            for (size_t h = 0; h < view_.extent(1); ++h) {
                for (size_t w = 0; w < view_.extent(2); ++w) {
                    view_[d, h, w] += other.view_[d, h, w];
                }
            }
        }
        return *this;
    }
};

// 矩阵乘法的mdspan实现
void matrix_multiply_mdspan(
    std::mdspan<const double, std::dextents<size_t, 2>> A,
    std::mdspan<const double, std::dextents<size_t, 2>> B,
    std::mdspan<double, std::dextents<size_t, 2>> C
) {
    assert(A.extent(1) == B.extent(0));
    assert(C.extent(0) == A.extent(0));
    assert(C.extent(1) == B.extent(1));
    
    for (size_t i = 0; i < C.extent(0); ++i) {
        for (size_t j = 0; j < C.extent(1); ++j) {
            C[i, j] = 0;
            for (size_t k = 0; k < A.extent(1); ++k) {
                C[i, j] += A[i, k] * B[k, j];
            }
        }
    }
}
```

## 3. std::print：现代化输出系统

### 格式化输出的统一化
```cpp
#include <print>
#include <format>

void output_system_evolution() {
    // C语言时代：printf系列
    printf("Hello %s, you are %d years old\n", "Alice", 25);
    
    // C++早期：iostream
    std::cout << "Hello " << "Alice" << ", you are " << 25 << " years old" << std::endl;
    
    // C++20：std::format
    std::cout << std::format("Hello {}, you are {} years old\n", "Alice", 25);
    
    // C++23：std::print（最简洁）
    std::print("Hello {}, you are {} years old\n", "Alice", 25);
    std::println("Hello {}, you are {} years old", "Alice", 25);  // 自动换行
}
```

### Unicode和国际化支持
```cpp
void unicode_output_demo() {
    // 自动UTF-8输出
    std::print("中文输出：{}\n", "你好世界");
    std::print("Emoji: {} {}\n", "🌍", "🚀");
    std::print("数学符号: ∑ ∫ ∞ π\n");
    
    // 控制台兼容性
    std::print("Console width: {}\n", 80);
    
    // 错误输出到stderr
    std::print(stderr, "Error: {}\n", "Something went wrong");
}
```

### 性能优化的输出
```cpp
void performance_optimized_output() {
    // std::print的性能优势：
    // 1. 直接系统调用，避免iostream的同步开销
    // 2. 编译期格式字符串验证
    // 3. 更少的虚函数调用
    // 4. 更好的编译器优化机会
    
    std::vector<int> large_data(1000000);
    std::iota(large_data.begin(), large_data.end(), 1);
    
    // 高效的批量输出
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int value : large_data) {
        std::print("{} ", value);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::println("\nOutput time: {}ms", 
                 std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}
```

## 4. Ranges改进和新视图

### 新增的实用视图
```cpp
#include <ranges>
#include <algorithm>

void new_views_demo() {
    std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // std::views::zip - 并行遍历多个范围
    std::vector<char> letters{'a', 'b', 'c', 'd', 'e'};
    for (auto [num, letter] : std::views::zip(data, letters)) {
        std::print("{}: {}\n", num, letter);
    }
    
    // std::views::zip_transform - 变换后的zip
    auto products = std::views::zip_transform(std::multiplies{}, data, data);
    for (int square : products | std::views::take(5)) {
        std::print("{} ", square);  // 1 4 9 16 25
    }
    std::println("");
    
    // std::views::adjacent - 相邻元素处理
    auto differences = data | std::views::adjacent<2> 
                          | std::views::transform([](auto pair) {
                                auto [a, b] = pair;
                                return b - a;
                            });
    
    // std::views::adjacent_transform - 直接变换相邻元素
    auto adjacent_sums = data | std::views::adjacent_transform<2>(std::plus{});
    
    // std::views::chunk - 分块处理
    for (auto chunk : data | std::views::chunk(3)) {
        std::print("Chunk: ");
        for (int val : chunk) {
            std::print("{} ", val);
        }
        std::println("");
    }
    
    // std::views::slide - 滑动窗口
    auto sliding_windows = data | std::views::slide(3);
    for (auto window : sliding_windows) {
        std::print("Window: ");
        for (int val : window) {
            std::print("{} ", val);
        }
        std::println("");
    }
}
```

### 范围算法的增强
```cpp
void enhanced_range_algorithms() {
    std::vector<int> data{3, 1, 4, 1, 5, 9, 2, 6};
    
    // std::ranges::to - 范围到容器的转换
    auto filtered = data 
                  | std::views::filter([](int x) { return x > 3; })
                  | std::ranges::to<std::vector>();
    
    auto set_result = data 
                    | std::views::filter([](int x) { return x % 2 == 0; })
                    | std::ranges::to<std::set>();
    
    // 链式转换
    auto processed = data 
                   | std::views::transform([](int x) { return x * 2; })
                   | std::views::filter([](int x) { return x > 5; })
                   | std::ranges::to<std::deque>();
    
    // std::ranges::contains - 包含检查
    bool has_five = std::ranges::contains(data, 5);
    std::println("Contains 5: {}", has_five);
    
    // std::ranges::starts_with / ends_with
    std::vector<int> prefix{3, 1};
    bool starts_with_prefix = std::ranges::starts_with(data, prefix);
    
    // std::ranges::find_last - 从后往前查找
    auto last_1 = std::ranges::find_last(data, 1);
    if (last_1 != data.end()) {
        std::println("Last 1 found at position: {}", 
                     std::distance(data.begin(), last_1));
    }
}
```

### 自定义范围适配器
```cpp
// 自定义一个"重复"视图适配器
template<std::ranges::view V>
class repeat_view : public std::ranges::view_interface<repeat_view<V>> {
private:
    V base_;
    size_t count_;
    
public:
    repeat_view(V base, size_t count) : base_(std::move(base)), count_(count) {}
    
    class iterator {
        std::ranges::iterator_t<V> iter_;
        std::ranges::iterator_t<V> begin_;
        std::ranges::iterator_t<V> end_;
        size_t current_repeat_;
        size_t max_repeats_;
        
    public:
        using value_type = std::ranges::range_value_t<V>;
        using difference_type = std::ptrdiff_t;
        
        iterator(std::ranges::iterator_t<V> begin, std::ranges::iterator_t<V> end, 
                size_t max_repeats)
            : iter_(begin), begin_(begin), end_(end), 
              current_repeat_(0), max_repeats_(max_repeats) {}
        
        auto operator*() const { return *iter_; }
        
        iterator& operator++() {
            ++iter_;
            if (iter_ == end_ && current_repeat_ < max_repeats_ - 1) {
                iter_ = begin_;
                ++current_repeat_;
            }
            return *this;
        }
        
        bool operator==(const iterator& other) const {
            return iter_ == other.iter_ && current_repeat_ == other.current_repeat_;
        }
    };
    
    iterator begin() { return iterator{std::ranges::begin(base_), std::ranges::end(base_), count_}; }
    iterator end() { 
        auto end_iter = iterator{std::ranges::end(base_), std::ranges::end(base_), count_};
        return end_iter;
    }
};

// 创建自定义视图的便利函数
template<std::ranges::view V>
repeat_view<V> repeat(V&& view, size_t count) {
    return repeat_view<V>{std::forward<V>(view), count};
}
```

## 5. 其他重要改进

### if consteval的编译期检测
```cpp
#include <type_traits>

// 运行时和编译期的不同行为
constexpr int flexible_function(int x) {
    if consteval {
        // 编译期执行的代码
        return x * x;  // 简单计算
    } else {
        // 运行时执行的代码  
        return expensive_runtime_calculation(x);  // 复杂计算
    }
}

// 编译期优化的实际应用
template<typename T>
constexpr auto safe_cast(auto value) {
    if consteval {
        // 编译期：直接转换，编译器会检查安全性
        return static_cast<T>(value);
    } else {
        // 运行时：添加额外检查
        if (value < std::numeric_limits<T>::min() || 
            value > std::numeric_limits<T>::max()) {
            throw std::overflow_error("Cast overflow");
        }
        return static_cast<T>(value);
    }
}
```

### std::unreachable的优化提示
```cpp
#include <utility>

// 编译器优化提示
int optimized_switch(int value) {
    switch (value) {
        case 1: return 10;
        case 2: return 20;
        case 3: return 30;
        default:
            // 告诉编译器这里永远不会执行
            std::unreachable();
    }
    // 编译器可以优化掉后续的死代码
}

// 性能关键路径的优化
template<int N>
constexpr int factorial() {
    if constexpr (N == 0 || N == 1) {
        return 1;
    } else if constexpr (N > 1 && N <= 20) {
        return N * factorial<N-1>();
    } else {
        // 编译期就能确定这种情况不会发生
        std::unreachable();
    }
}
```

### 新的类型特征和概念
```cpp
#include <type_traits>
#include <concepts>

// 新的类型特征
template<typename T>
void type_traits_demo() {
    // std::is_scoped_enum - 检测作用域枚举
    enum class Color { Red, Green, Blue };
    enum OldColor { RED, GREEN, BLUE };
    
    static_assert(std::is_scoped_enum_v<Color>);      // true
    static_assert(!std::is_scoped_enum_v<OldColor>);  // false
    
    // std::is_implicit_lifetime - 检测隐式生命周期类型
    static_assert(std::is_implicit_lifetime_v<int>);
    
    // std::reference_constructs_from_temporary
    static_assert(std::reference_constructs_from_temporary_v<const std::string&, const char*>);
}

// 新的标准概念
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename R>
concept ContiguousRange = std::ranges::contiguous_range<R> && 
                         std::ranges::sized_range<R>;

// 增强的concept使用
template<ContiguousRange R>
    requires Numeric<std::ranges::range_value_t<R>>
auto sum_contiguous(R&& range) {
    using ValueType = std::ranges::range_value_t<R>;
    return std::accumulate(std::ranges::begin(range), 
                          std::ranges::end(range), 
                          ValueType{});
}
```

### 标准库容器的小改进
```cpp
void container_improvements() {
    // std::flat_map和std::flat_set（提案中）
    // 基于排序vector的关联容器，更好的缓存局部性
    
    // std::stacktrace（部分实现）
    // 程序崩溃时的调用栈追踪
    
    // 改进的hash支持
    std::unordered_map<std::string, int> map;
    map["key"] = 42;
    
    // 透明哈希查找
    auto it = map.find("key");  // 避免不必要的string构造
}
```

## 设计哲学总结

C++23体现了C++语言发展的四个重要方向：

### 1. 实用主义优先
- **std::expected**解决了实际项目中的错误处理痛点
- **std::print**提供了简洁高效的输出方案
- **ranges改进**完善了函数式编程生态

### 2. 性能与安全并重
- **std::mdspan**零开销的多维数组抽象
- **if consteval**编译期运行期分离优化
- **std::unreachable**帮助编译器生成更优代码

### 3. 标准库的持续完善
- **ranges**生态系统的进一步扩展
- **新的类型特征**支持更精确的类型检查
- **容器改进**提升日常使用体验

### 4. 向后兼容的渐进式改进
- 保持与现有代码的兼容性
- 提供更现代的替代方案
- 渐进式迁移路径

C++23虽然没有C++11和C++20那样的革命性变化，但通过大量实用的改进，显著提升了C++的开发体验和代码质量。这些特性的组合使用将进一步巩固C++在系统编程、高性能计算和现代应用开发中的地位。