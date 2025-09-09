/**
 * C++23 Ranges改进和新视图深度解析
 * 
 * 核心概念：
 * 1. 视图组合模式 - zip、adjacent、chunk等新视图的组合使用
 * 2. 惰性求值优化 - 避免不必要的中间容器和计算
 * 3. 算法增强 - ranges::to、contains、find_last等新算法
 * 4. 自定义适配器 - 扩展ranges生态系统的能力
 * 5. 性能优化 - 编译期优化和零开销抽象
 */

#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <string>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <format>
#include <tuple>
#include <utility>
#include <complex>
#include <cmath>

// ===== 1. 新增视图：zip和zip_transform =====
void demonstrate_zip_views() {
    std::cout << "=== 新增视图：zip和zip_transform ===\n";
    
    std::vector<int> numbers{1, 2, 3, 4, 5};
    std::vector<std::string> names{"Alice", "Bob", "Charlie", "David", "Eve"};
    std::vector<double> scores{85.5, 92.0, 78.5, 88.0, 95.5};
    
    // std::views::zip - 并行遍历多个范围
    std::cout << "zip视图 - 并行遍历:\n";
    for (auto [num, name, score] : std::views::zip(numbers, names, scores)) {
        std::println("ID: {}, Name: {}, Score: {:.1f}", num, name, score);
    }
    
    // std::views::zip_transform - 变换后的zip
    std::cout << "\nzip_transform - 计算平方和立方:\n";
    auto squares_cubes = std::views::zip_transform(
        [](int n) { return std::make_pair(n * n, n * n * n); },
        numbers
    );
    
    for (auto [square, cube] : squares_cubes) {
        std::println("平方: {}, 立方: {}", square, cube);
    }
    
    // 实际应用：矩阵转置
    std::cout << "\n矩阵转置应用:\n";
    std::vector<std::vector<int>> matrix{
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    // 使用zip进行矩阵转置
    auto transposed = std::views::zip(matrix[0], matrix[1], matrix[2]);
    std::println("转置后的矩阵:");
    for (auto row : transposed) {
        std::print("  ");
        for (auto val : row) {
            std::print("{} ", val);
        }
        std::println("");
    }
    
    std::cout << "\n";
}

// ===== 2. 相邻元素处理：adjacent和adjacent_transform =====
void demonstrate_adjacent_views() {
    std::cout << "=== 相邻元素处理：adjacent和adjacent_transform ===\n";
    
    std::vector<int> data{1, 3, 6, 10, 15, 21, 28};
    
    // std::views::adjacent<2> - 获取相邻元素对
    std::cout << "adjacent<2> - 相邻元素对:\n";
    auto adjacent_pairs = data | std::views::adjacent<2>;
    for (auto [a, b] : adjacent_pairs) {
        std::println("({}, {}) 差值: {}", a, b, b - a);
    }
    
    // std::views::adjacent<3> - 三元组
    std::cout << "\nadjacent<3> - 三元组:\n";
    auto adjacent_triples = data | std::views::adjacent<3>;
    for (auto [a, b, c] : adjacent_triples) {
        std::println("({}, {}, {}) 和: {}", a, b, c, a + b + c);
    }
    
    // std::views::adjacent_transform - 直接变换相邻元素
    std::cout << "\nadjacent_transform - 相邻元素运算:\n";
    
    // 计算相邻元素的差值
    auto differences = data | std::views::adjacent_transform<2>(std::minus{});
    std::println("相邻差值: {}", std::format("{}", std::format("{}\n", differences)));
    
    // 计算滑动平均
    auto moving_avg = data | std::views::adjacent_transform<3>(
        [](auto... vals) { return (vals + ...) / 3.0; }
    );
    
    std::println("滑动平均:");
    for (double avg : moving_avg) {
        std::println("  {:.2f}", avg);
    }
    
    // 实际应用：数值积分
    std::cout << "\n数值积分应用:\n";
    std::vector<double> x_values{0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<double> y_values{0.0, 1.0, 4.0, 9.0, 16.0}; // y = x^2
    
    auto integral = std::views::zip_transform(
        [](double x1, double x2, double y1, double y2) {
            return 0.5 * (x2 - x1) * (y1 + y2); // 梯形法则
        },
        x_values | std::views::take(4),
        x_values | std::views::drop(1),
        y_values | std::views::take(4),
        y_values | std::views::drop(1)
    );
    
    double total_integral = std::accumulate(integral.begin(), integral.end(), 0.0);
    std::println("积分结果 (y=x^2 从 0 到 4): {:.2f} (理论值: 21.33)", total_integral);
    
    std::cout << "\n";
}

// ===== 3. 分块和滑动：chunk和slide =====
void demonstrate_chunk_slide_views() {
    std::cout << "=== 分块和滑动：chunk和slide ===\n";
    
    std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    
    // std::views::chunk - 分块处理
    std::cout << "chunk视图 - 分块处理:\n";
    for (auto chunk : data | std::views::chunk(3)) {
        std::print("块: ");
        for (int val : chunk) {
            std::print("{} ", val);
        }
        std::println("");
    }
    
    // std::views::slide - 滑动窗口
    std::cout << "\nslide视图 - 滑动窗口:\n";
    auto sliding_windows = data | std::views::slide(4);
    for (auto window : sliding_windows) {
        std::print("窗口: ");
        for (int val : window) {
            std::print("{} ", val);
        }
        std::println(" (和: {})", std::accumulate(window.begin(), window.end(), 0));
    }
    
    // 实际应用：信号处理
    std::cout << "\n信号处理应用 - 移动平均滤波:\n";
    std::vector<double> signal{1.0, 2.0, 10.0, 3.0, 4.0, 15.0, 5.0, 6.0};
    int window_size = 3;
    
    auto smoothed_signal = signal | std::views::slide(window_size) |
                         std::views::transform([window_size](auto window) {
                             return std::accumulate(window.begin(), window.end(), 0.0) / window_size;
                         });
    
    std::println("原始信号 vs 平滑信号:");
    std::println("原始: {}", std::format("{}", std::format("{}\n", signal)));
    std::println("平滑: {}", std::format("{}", std::format("{}\n", smoothed_signal)));
    
    // 批量处理
    std::cout << "\n批量处理应用:\n";
    std::vector<std::string> items{"item1", "item2", "item3", "item4", "item5", "item6"};
    
    auto process_batch = [](auto batch) {
        std::println("处理批次: {}", std::format("{}", std::format("{}\n", batch)));
        return batch.size(); // 返回处理的项目数
    };
    
    auto batch_sizes = items | std::views::chunk(2) |
                       std::views::transform(process_batch);
    
    std::println("各批次处理的项目数:");
    for (size_t count : batch_sizes) {
        std::println("  {} 个项目", count);
    }
    
    std::cout << "\n";
}

// ===== 4. 算法增强：ranges::to、contains、find_last =====
void demonstrate_enhanced_algorithms() {
    std::cout << "=== 算法增强：ranges::to、contains、find_last ===\n";
    
    std::vector<int> numbers{1, 2, 3, 4, 5, 4, 3, 2, 1};
    
    // std::ranges::to - 范围到容器的转换
    std::cout << "ranges::to - 容器转换:\n";
    
    // 过滤并转换为不同容器
    auto filtered = numbers | std::views::filter([](int x) { return x > 2; }) |
                  std::ranges::to<std::vector>();
    std::println("过滤后(vector): {}", std::format("{}", std::format("{}\n", filtered)));
    
    // 转换为set去重
    auto unique = numbers | std::ranges::to<std::set>();
    std::println("去重后(set): {}", std::format("{}", std::format("{}\n", unique)));
    
    // 转换为deque
    auto deque_result = numbers | std::views::transform([](int x) { return x * 2; }) |
                        std::ranges::to<std::deque>();
    std::println("变换后(deque): {}", std::format("{}", std::format("{}\n", deque_result)));
    
    // std::ranges::contains - 包含检查
    std::cout << "\nranges::contains - 包含检查:\n";
    std::println("包含3: {}", std::ranges::contains(numbers, 3));
    std::println("包含10: {}", std::ranges::contains(numbers, 10));
    
    // 在子范围中查找
    auto subrange = numbers | std::views::take(5);
    std::println("前5个元素包含4: {}", std::ranges::contains(subrange, 4));
    
    // std::ranges::starts_with / ends_with
    std::cout << "\nstarts_with / ends_with - 前缀后缀检查:\n";
    std::vector<int> prefix{1, 2, 3};
    std::vector<int> suffix{2, 1};
    
    std::println("以[1,2,3]开头: {}", std::ranges::starts_with(numbers, prefix));
    std::println("以[2,1]结尾: {}", std::ranges::ends_with(numbers, suffix));
    
    // std::ranges::find_last - 从后往前查找
    std::cout << "\nfind_last - 从后查找:\n";
    auto last_3 = std::ranges::find_last(numbers, 3);
    if (last_3 != numbers.end()) {
        auto pos = std::distance(numbers.begin(), last_3);
        std::println("最后一个3的位置: {}", pos);
    }
    
    auto last_even = std::ranges::find_last(numbers, [](int x) { return x % 2 == 0; });
    if (last_even != numbers.end()) {
        auto pos = std::distance(numbers.begin(), last_even);
        std::println("最后一个偶数的位置: {} (值: {})", pos, *last_even);
    }
    
    // 实际应用：数据验证
    std::cout << "\n数据验证应用:\n";
    std::vector<std::string> data_lines{"header", "data1", "data2", "footer"};
    std::vector<std::string> expected_header{"header"};
    std::vector<std::string> expected_footer{"footer"};
    
    bool is_valid = std::ranges::starts_with(data_lines, expected_header) &&
                   std::ranges::ends_with(data_lines, expected_footer);
    
    std::println("数据格式验证: {}", is_valid ? "通过" : "失败");
    
    std::cout << "\n";
}

// ===== 5. 自定义范围适配器 =====
// 自定义repeat视图适配器
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
        
        iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return iter_ == other.iter_ && current_repeat_ == other.current_repeat_;
        }
    };
    
    iterator begin() { return iterator{std::ranges::begin(base_), std::ranges::end(base_), count_}; }
    iterator end() { 
        return iterator{std::ranges::end(base_), std::ranges::end(base_), count_};
    }
    
    size_t size() const { return std::ranges::size(base_) * count_; }
};

// 创建自定义视图的便利函数
template<std::ranges::view V>
repeat_view<V> repeat(V&& view, size_t count) {
    return repeat_view<V>{std::forward<V>(view), count};
}

// 自定义统计视图
template<std::ranges::view V>
class stats_view : public std::ranges::view_interface<stats_view<V>> {
private:
    V base_;
    
public:
    stats_view(V base) : base_(std::move(base)) {}
    
    struct stats_result {
        double min;
        double max;
        double mean;
        double sum;
        size_t count;
        
        std::string to_string() const {
            return std::format("min={:.2f}, max={:.2f}, mean={:.2f}, sum={:.2f}, count={}", 
                             min, max, mean, sum, count);
        }
    };
    
    class iterator {
        std::ranges::iterator_t<V> iter_;
        std::ranges::iterator_t<V> end_;
        stats_result current_stats_;
        std::vector<double> window_data_;
        size_t window_size_;
        
    public:
        using value_type = stats_result;
        using difference_type = std::ptrdiff_t;
        
        iterator(std::ranges::iterator_t<V> begin, std::ranges::iterator_t<V> end, 
                size_t window_size)
            : iter_(begin), end_(end), window_size_(window_size) {
            if (iter_ != end_) {
                update_stats();
            }
        }
        
        void update_stats() {
            window_data_.clear();
            auto temp_iter = iter_;
            for (size_t i = 0; i < window_size_ && temp_iter != end_; ++i, ++temp_iter) {
                window_data_.push_back(static_cast<double>(*temp_iter));
            }
            
            if (!window_data_.empty()) {
                current_stats_.min = *std::ranges::min_element(window_data_);
                current_stats_.max = *std::ranges::max_element(window_data_);
                current_stats_.sum = std::accumulate(window_data_.begin(), window_data_.end(), 0.0);
                current_stats_.mean = current_stats_.sum / window_data_.size();
                current_stats_.count = window_data_.size();
            }
        }
        
        const stats_result& operator*() const { return current_stats_; }
        
        iterator& operator++() {
            if (iter_ != end_) {
                ++iter_;
                update_stats();
            }
            return *this;
        }
        
        iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return iter_ == other.iter_;
        }
    };
    
    iterator begin() { return iterator{std::ranges::begin(base_), std::ranges::end(base_), 3}; }
    iterator end() { return iterator{std::ranges::end(base_), std::ranges::end(base_), 3}; }
};

template<std::ranges::view V>
stats_view<V> stats(V&& view) {
    return stats_view<V>{std::forward<V>(view)};
}

void demonstrate_custom_adapters() {
    std::cout << "=== 自定义范围适配器 ===\n";
    
    std::vector<int> data{1, 2, 3, 4, 5};
    
    // 使用自定义repeat视图
    std::cout << "repeat视图 - 重复数据:\n";
    auto repeated = repeat(std::views::all(data), 3);
    std::println("重复3次: {}", std::format("{}", std::format("{}\n", repeated)));
    
    // 使用自定义统计视图
    std::cout << "stats视图 - 滑动统计:\n";
    std::vector<double> sensor_data{10.5, 12.3, 11.8, 13.2, 14.1, 12.9, 15.3, 16.7};
    
    auto sliding_stats = stats(std::views::all(sensor_data));
    std::println("滑动窗口统计:");
    for (const auto& stat : sliding_stats) {
        std::println("  {}", stat.to_string());
    }
    
    // 组合使用多个适配器
    std::cout << "\n组合适配器应用:\n";
    std::vector<int> large_data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    auto complex_pipeline = large_data | 
                           std::views::filter([](int x) { return x % 2 == 0; }) |
                           std::views::transform([](int x) { return x * x; }) |
                           repeat(2) |
                           std::views::chunk(3);
    
    std::println("复杂管道处理结果:");
    for (auto chunk : complex_pipeline) {
        std::println("  块: {}", std::format("{}", std::format("{}\n", chunk)));
    }
    
    std::cout << "\n";
}

// ===== 6. 性能优化和编译期优化 =====
class PerformanceBenchmark {
public:
    static void compare_range_vs_traditional() {
        std::cout << "=== Ranges vs 传统算法性能对比 ===\n";
        
        const size_t size = 1000000;
        std::vector<int> data(size);
        std::iota(data.begin(), data.end(), 1);
        
        // 传统算法
        auto traditional_approach = [&]() {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::vector<int> filtered;
            filtered.reserve(size / 2);
            for (int x : data) {
                if (x % 2 == 0) {
                    filtered.push_back(x * x);
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        };
        
        // Ranges算法
        auto ranges_approach = [&]() {
            auto start = std::chrono::high_resolution_clock::now();
            
            auto result = data | std::views::filter([](int x) { return x % 2 == 0; }) |
                         std::views::transform([](int x) { return x * x; }) |
                         std::ranges::to<std::vector>();
            
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        };
        
        // 执行测试
        auto traditional_time = traditional_approach();
        auto ranges_time = ranges_approach();
        
        std::println("数据大小: {}", size);
        std::println("传统算法耗时: {}μs", traditional_time.count());
        std::println("Ranges算法耗时: {}μs", ranges_time.count());
        std::println("性能比率: {:.2f}x", 
                     static_cast<double>(traditional_time.count()) / ranges_time.count());
        
        std::println("\n性能分析:");
        std::println("- Ranges的惰性求值避免了中间容器");
        std::println("- 编译器可以更好地优化管道操作");
        std::println("- 现代CPU缓存友好性更好");
        std::println("- 代码更简洁，可读性更强");
        
        std::cout << "\n";
    }
};

// ===== 主函数 =====
int main() {
    std::cout << "C++23 Ranges改进和新视图深度解析\n";
    std::cout << "==================================\n";
    
    demonstrate_zip_views();
    demonstrate_adjacent_views();
    demonstrate_chunk_slide_views();
    demonstrate_enhanced_algorithms();
    demonstrate_custom_adapters();
    PerformanceBenchmark::compare_range_vs_traditional();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall 04_ranges_enhancements.cpp -o ranges_demo
./ranges_demo

注意：C++23的Ranges增强需要编译器支持：
- GCC 13+: 大部分支持
- Clang 16+: 大部分支持
- MSVC 19.34+: 大部分支持

某些特性可能需要特定的编译器版本或实验性标志。

关键学习点:
1. zip和adjacent视图提供了强大的数据组合能力
2. chunk和slide视图适合批量处理和滑动窗口操作
3. ranges::to等新算法简化了容器转换
4. 自定义适配器可以扩展ranges生态系统
5. Ranges的惰性求值和编译期优化带来性能优势

注意事项:
- Ranges视图是惰性的，只在需要时计算
- 复杂的管道可能影响编译时间
- 自定义适配器需要深入理解ranges概念
- 在性能关键路径上进行基准测试
*/