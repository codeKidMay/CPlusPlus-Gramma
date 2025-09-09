/**
 * C++11/14/17/20 模板策略模式深度解析
 * 
 * 核心概念：
 * 1. 策略模式的模板化实现 - 编译期策略选择
 * 2. 策略特化与策略组合 - 多维度策略空间
 * 3. 策略的自动推导机制 - SFINAE与Concepts约束
 * 4. 性能优化策略模式 - 零开销抽象
 * 5. 现代C++策略设计 - 函数对象与Lambda策略
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <random>
#include <concepts>

// ===== 1. 策略模式的模板化实现演示 =====
void demonstrate_template_strategy() {
    std::cout << "=== 策略模式的模板化实现演示 ===\n";
    
    // 传统策略模式（运行时多态）
    class SortStrategy {
    public:
        virtual ~SortStrategy() = default;
        virtual void sort(std::vector<int>& data) = 0;
        virtual std::string name() const = 0;
    };
    
    class BubbleSort : public SortStrategy {
    public:
        void sort(std::vector<int>& data) override {
            // 简化的冒泡排序实现
            for (size_t i = 0; i < data.size(); ++i) {
                for (size_t j = 0; j < data.size() - i - 1; ++j) {
                    if (data[j] > data[j + 1]) {
                        std::swap(data[j], data[j + 1]);
                    }
                }
            }
        }
        std::string name() const override { return "BubbleSort"; }
    };
    
    // 模板策略模式（编译期多态）
    template<typename SortStrategy>
    class TemplateSorter {
    private:
        SortStrategy strategy_;
        
    public:
        template<typename... Args>
        TemplateSorter(Args&&... args) : strategy_(std::forward<Args>(args)...) {}
        
        void sort(std::vector<int>& data) {
            std::cout << "使用策略: " << strategy_.name() << "\n";
            strategy_.sort(data);
        }
        
        const SortStrategy& get_strategy() const { return strategy_; }
    };
    
    // 模板策略实现
    struct QuickSortStrategy {
        std::string name() const { return "QuickSort"; }
        
        void sort(std::vector<int>& data) {
            if (!data.empty()) {
                quick_sort_impl(data, 0, data.size() - 1);
            }
        }
        
    private:
        void quick_sort_impl(std::vector<int>& data, int low, int high) {
            if (low < high) {
                int pivot = partition(data, low, high);
                quick_sort_impl(data, low, pivot - 1);
                quick_sort_impl(data, pivot + 1, high);
            }
        }
        
        int partition(std::vector<int>& data, int low, int high) {
            int pivot = data[high];
            int i = low - 1;
            
            for (int j = low; j < high; ++j) {
                if (data[j] <= pivot) {
                    ++i;
                    std::swap(data[i], data[j]);
                }
            }
            std::swap(data[i + 1], data[high]);
            return i + 1;
        }
    };
    
    struct MergeSortStrategy {
        std::string name() const { return "MergeSort"; }
        
        void sort(std::vector<int>& data) {
            if (data.size() > 1) {
                merge_sort_impl(data, 0, data.size() - 1);
            }
        }
        
    private:
        void merge_sort_impl(std::vector<int>& data, size_t left, size_t right) {
            if (left < right) {
                size_t mid = left + (right - left) / 2;
                merge_sort_impl(data, left, mid);
                merge_sort_impl(data, mid + 1, right);
                merge(data, left, mid, right);
            }
        }
        
        void merge(std::vector<int>& data, size_t left, size_t mid, size_t right) {
            std::vector<int> temp(right - left + 1);
            size_t i = left, j = mid + 1, k = 0;
            
            while (i <= mid && j <= right) {
                if (data[i] <= data[j]) {
                    temp[k++] = data[i++];
                } else {
                    temp[k++] = data[j++];
                }
            }
            
            while (i <= mid) temp[k++] = data[i++];
            while (j <= right) temp[k++] = data[j++];
            
            for (i = left, k = 0; i <= right; ++i, ++k) {
                data[i] = temp[k];
            }
        }
    };
    
    // 测试模板策略
    std::vector<int> data1 = {64, 34, 25, 12, 22, 11, 90};
    std::vector<int> data2 = data1;
    
    TemplateSorter<QuickSortStrategy> quick_sorter;
    TemplateSorter<MergeSortStrategy> merge_sorter;
    
    quick_sorter.sort(data1);
    merge_sorter.sort(data2);
    
    std::cout << "QuickSort结果: ";
    for (int x : data1) std::cout << x << " ";
    std::cout << "\n";
    
    std::cout << "MergeSort结果: ";
    for (int x : data2) std::cout << x << " ";
    std::cout << "\n\n";
}

// ===== 2. 策略特化与策略组合演示 =====
void demonstrate_strategy_specialization() {
    std::cout << "=== 策略特化与策略组合演示 ===\n";
    
    // 多维度策略特化
    template<typename DataType, typename CompareStrategy, typename ContainerStrategy>
    class MultiDimensionalSorter {
    private:
        CompareStrategy compare_;
        ContainerStrategy container_;
        
    public:
        using value_type = DataType;
        
        template<typename Container>
        void sort(Container& data) {
            std::cout << "使用比较策略: " << compare_.name() << "\n";
            std::cout << "使用容器策略: " << container_.name() << "\n";
            
            // 策略组合应用
            container_.prepare(data);
            std::sort(data.begin(), data.end(), compare_);
            container_.finalize(data);
        }
    };
    
    // 比较策略
    struct AscendingCompare {
        std::string name() const { return "Ascending"; }
        
        template<typename T>
        bool operator()(const T& a, const T& b) const {
            return a < b;
        }
    };
    
    struct DescendingCompare {
        std::string name() const { return "Descending"; }
        
        template<typename T>
        bool operator()(const T& a, const T& b) const {
            return a > b;
        }
    };
    
    // 容器策略
    struct VectorStrategy {
        std::string name() const { return "Vector"; }
        
        template<typename Container>
        void prepare(Container& c) {
            std::cout << "准备Vector容器，大小: " << c.size() << "\n";
        }
        
        template<typename Container>
        void finalize(Container& c) {
            std::cout << "Vector排序完成，容量: " << c.capacity() << "\n";
        }
    };
    
    struct OptimizedStrategy {
        std::string name() const { return "Optimized"; }
        
        template<typename Container>
        void prepare(Container& c) {
            std::cout << "优化容器准备，预留空间\n";
            c.reserve(c.size() * 2);
        }
        
        template<typename Container>
        void finalize(Container& c) {
            std::cout << "优化完成，收缩容器\n";
            c.shrink_to_fit();
        }
    };
    
    // 策略特化示例
    template<typename T>
    using AscendingSorter = MultiDimensionalSorter<T, AscendingCompare, VectorStrategy>;
    
    template<typename T>
    using OptimizedDescendingSorter = MultiDimensionalSorter<T, DescendingCompare, OptimizedStrategy>;
    
    // 测试策略组合
    std::vector<int> data = {5, 2, 8, 1, 9, 3};
    
    AscendingSorter<int> asc_sorter;
    asc_sorter.sort(data);
    
    std::cout << "升序结果: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n";
    
    OptimizedDescendingSorter<int> opt_desc_sorter;
    opt_desc_sorter.sort(data);
    
    std::cout << "优化降序结果: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n\n";
}

// ===== 3. 策略的自动推导机制演示 =====
void demonstrate_strategy_deduction() {
    std::cout << "=== 策略的自动推导机制演示 ===\n";
    
    // 基于SFINAE的策略推导
    template<typename T, typename = void>
    struct DefaultProcessingStrategy {
        std::string name() const { return "Generic"; }
        
        void process(const T& item) {
            std::cout << "通用处理: " << item << "\n";
        }
    };
    
    // 针对数值类型的特化
    template<typename T>
    struct DefaultProcessingStrategy<T, std::enable_if_t<std::is_arithmetic_v<T>>> {
        std::string name() const { return "Arithmetic"; }
        
        void process(const T& item) {
            std::cout << "数值处理: " << item << " (平方: " << item * item << ")\n";
        }
    };
    
    // 针对字符串类型的特化
    template<>
    struct DefaultProcessingStrategy<std::string> {
        std::string name() const { return "String"; }
        
        void process(const std::string& item) {
            std::cout << "字符串处理: '" << item << "' (长度: " << item.length() << ")\n";
        }
    };
    
    // 自动推导处理器
    template<typename T, typename Strategy = DefaultProcessingStrategy<T>>
    class AutoProcessor {
    private:
        Strategy strategy_;
        
    public:
        void process(const T& item) {
            std::cout << "自动选择策略: " << strategy_.name() << " -> ";
            strategy_.process(item);
        }
    };
    
    // C++20 Concepts版本的策略推导
#if __cplusplus >= 202002L
    template<typename T>
    concept Processable = requires(T t) {
        { t.process() } -> std::convertible_to<void>;
    };
    
    template<typename T>
    concept Comparable = requires(T a, T b) {
        { a < b } -> std::convertible_to<bool>;
    };
    
    template<typename T>
    class ConceptProcessor {
    public:
        void process(const T& item) requires Processable<T> {
            std::cout << "使用Processable concept策略\n";
            item.process();
        }
        
        void process(const T& item) requires Comparable<T> && (!Processable<T>) {
            std::cout << "使用Comparable concept策略: " << item << "\n";
        }
    };
#endif
    
    // 测试自动推导
    AutoProcessor<int> int_processor;
    AutoProcessor<double> double_processor;
    AutoProcessor<std::string> string_processor;
    
    int_processor.process(42);
    double_processor.process(3.14);
    string_processor.process("hello");
    
    std::cout << "\n";
}

// ===== 4. 性能优化策略模式演示 =====
void demonstrate_performance_strategies() {
    std::cout << "=== 性能优化策略模式演示 ===\n";
    
    // 编译期策略选择
    template<bool UseOptimized>
    struct ProcessingStrategy;
    
    template<>
    struct ProcessingStrategy<true> {
        static std::string name() { return "Optimized"; }
        
        template<typename Container>
        static void process(Container& data) {
            // 优化版本：使用并行算法
            std::sort(std::execution::par, data.begin(), data.end());
        }
        
        static constexpr bool is_parallel = true;
    };
    
    template<>
    struct ProcessingStrategy<false> {
        static std::string name() { return "Sequential"; }
        
        template<typename Container>
        static void process(Container& data) {
            // 标准版本：顺序算法
            std::sort(data.begin(), data.end());
        }
        
        static constexpr bool is_parallel = false;
    };
    
    // 零开销策略模板
    template<typename Strategy>
    class ZeroOverheadProcessor {
    public:
        template<typename Container>
        void process(Container& data) {
            std::cout << "使用零开销策略: " << Strategy::name() << "\n";
            std::cout << "并行处理: " << (Strategy::is_parallel ? "是" : "否") << "\n";
            
            Strategy::process(data);
        }
    };
    
    // 条件编译策略选择
    constexpr bool should_use_optimized = true;  // 可以基于编译配置
    
    using SelectedStrategy = ProcessingStrategy<should_use_optimized>;
    ZeroOverheadProcessor<SelectedStrategy> processor;
    
    std::vector<int> data = {9, 5, 2, 7, 1, 8, 3, 6, 4};
    processor.process(data);
    
    std::cout << "处理结果: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n\n";
}

// ===== 5. 现代C++策略设计演示 =====
void demonstrate_modern_strategy() {
    std::cout << "=== 现代C++策略设计演示 ===\n";
    
    // Lambda策略
    auto create_processor = [](auto strategy) {
        return [strategy](auto& data) {
            std::cout << "Lambda策略处理数据\n";
            strategy(data);
        };
    };
    
    // 函数对象策略
    struct LoggingStrategy {
        std::string prefix;
        
        LoggingStrategy(std::string p) : prefix(std::move(p)) {}
        
        template<typename Container>
        void operator()(const Container& data) {
            std::cout << prefix << "数据大小: " << data.size() << "\n";
            std::cout << prefix << "首元素: " << *data.begin() << "\n";
        }
    };
    
    // 现代策略容器
    template<typename DataType>
    class ModernStrategyContainer {
    private:
        std::vector<DataType> data_;
        
    public:
        template<typename... Args>
        ModernStrategyContainer(Args&&... args) : data_{std::forward<Args>(args)...} {}
        
        // 接受任何可调用对象作为策略
        template<typename Strategy>
        auto apply_strategy(Strategy&& strategy) -> decltype(strategy(data_)) {
            std::cout << "应用现代策略\n";
            return std::forward<Strategy>(strategy)(data_);
        }
        
        // 链式策略应用
        template<typename... Strategies>
        void apply_chain(Strategies&&... strategies) {
            std::cout << "应用策略链，数量: " << sizeof...(strategies) << "\n";
            (std::forward<Strategies>(strategies)(data_), ...);  // C++17折叠表达式
        }
        
        const std::vector<DataType>& get_data() const { return data_; }
    };
    
    // 测试现代策略
    ModernStrategyContainer<int> container{5, 2, 8, 1, 9};
    
    // Lambda策略
    auto sort_strategy = [](auto& data) {
        std::sort(data.begin(), data.end());
        std::cout << "Lambda排序完成\n";
    };
    
    // 函数对象策略
    LoggingStrategy logger("INFO: ");
    
    // 统计策略
    auto stats_strategy = [](const auto& data) {
        auto [min, max] = std::minmax_element(data.begin(), data.end());
        std::cout << "统计 - 最小值: " << *min << ", 最大值: " << *max << "\n";
    };
    
    // 应用单个策略
    container.apply_strategy(sort_strategy);
    container.apply_strategy(logger);
    
    // 应用策略链
    container.apply_chain(
        [](const auto& data) { std::cout << "策略1: 验证数据完整性\n"; },
        [](const auto& data) { std::cout << "策略2: 数据压缩\n"; },
        stats_strategy
    );
    
    std::cout << "最终数据: ";
    for (int x : container.get_data()) std::cout << x << " ";
    std::cout << "\n\n";
}

// ===== 策略模式性能基准测试 =====
void benchmark_strategy_patterns() {
    std::cout << "=== 策略模式性能基准测试 ===\n";
    
    const int data_size = 10000;
    const int iterations = 100;
    
    // 生成测试数据
    auto generate_data = []() {
        std::vector<int> data(data_size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::iota(data.begin(), data.end(), 1);
        std::shuffle(data.begin(), data.end(), gen);
        return data;
    };
    
    // 传统多态策略（虚函数）
    class VirtualStrategy {
    public:
        virtual ~VirtualStrategy() = default;
        virtual void sort(std::vector<int>& data) = 0;
    };
    
    class VirtualQuickSort : public VirtualStrategy {
    public:
        void sort(std::vector<int>& data) override {
            std::sort(data.begin(), data.end());
        }
    };
    
    // 模板策略
    struct TemplateQuickSort {
        void sort(std::vector<int>& data) {
            std::sort(data.begin(), data.end());
        }
    };
    
    // 基准测试：虚函数策略
    auto virtual_strategy = std::make_unique<VirtualQuickSort>();
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto data = generate_data();
        virtual_strategy->sort(data);
    }
    
    auto virtual_time = std::chrono::high_resolution_clock::now() - start;
    
    // 基准测试：模板策略
    TemplateQuickSort template_strategy;
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto data = generate_data();
        template_strategy.sort(data);
    }
    
    auto template_time = std::chrono::high_resolution_clock::now() - start;
    
    // 基准测试：Lambda策略
    auto lambda_strategy = [](std::vector<int>& data) {
        std::sort(data.begin(), data.end());
    };
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto data = generate_data();
        lambda_strategy(data);
    }
    
    auto lambda_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "虚函数策略耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(virtual_time).count() 
              << " 毫秒\n";
    std::cout << "模板策略耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(template_time).count() 
              << " 毫秒\n";
    std::cout << "Lambda策略耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(lambda_time).count() 
              << " 毫秒\n";
    
    double speedup_template = static_cast<double>(virtual_time.count()) / template_time.count();
    double speedup_lambda = static_cast<double>(virtual_time.count()) / lambda_time.count();
    
    std::cout << "模板策略加速比: " << speedup_template << "x\n";
    std::cout << "Lambda策略加速比: " << speedup_lambda << "x\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++11/14/17/20 模板策略模式深度解析\n";
    std::cout << "==================================\n";
    
    demonstrate_template_strategy();
    demonstrate_strategy_specialization();
    demonstrate_strategy_deduction();
    demonstrate_performance_strategies();
    demonstrate_modern_strategy();
    benchmark_strategy_patterns();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 08_strategy_patterns.cpp -o strategy_patterns
./strategy_patterns

C++20版本:
g++ -std=c++20 -O2 -Wall 08_strategy_patterns.cpp -o strategy_patterns

关键学习点:
1. 模板策略模式实现编译期多态，避免虚函数开销
2. 策略特化和组合支持多维度的策略空间设计
3. SFINAE和Concepts可以实现策略的自动推导和约束
4. 零开销抽象是现代C++策略模式的核心优势
5. Lambda和函数对象提供了灵活的现代策略实现

注意事项:
- 模板策略在编译期确定，无法运行时动态切换
- 策略组合可能导致模板实例膨胀
- 需要在灵活性和性能之间找到平衡
- 合理使用策略特化避免代码重复
*/