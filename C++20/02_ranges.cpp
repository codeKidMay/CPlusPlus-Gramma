/**
 * C++20 Ranges函数式编程深度解析
 * 
 * 核心概念：
 * 1. 视图惰性求值 - Range视图的延迟计算机制和性能优化
 * 2. 组合式编程 - 管道操作符和函数组合的设计哲学
 * 3. 自定义视图实现 - 深入理解view_interface和适配器模式
 * 4. Range算法增强 - 投影和约束的算法设计革新
 * 5. constexpr Ranges - 编译期范围计算的实现原理
 */

#include <iostream>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <concepts>

// ===== 1. 视图惰性求值演示 =====
// 对比传统STL与Ranges的性能差异
class PerformanceComparison {
public:
    // 传统STL多步操作
    static std::vector<int> traditional_approach(const std::vector<int>& input) {
        // 第一步：过滤偶数
        std::vector<int> filtered;
        std::copy_if(input.begin(), input.end(), std::back_inserter(filtered),
                     [](int n) { return n % 2 == 0; });
        
        // 第二步：平方
        std::vector<int> squared;
        std::transform(filtered.begin(), filtered.end(), std::back_inserter(squared),
                      [](int n) { return n * n; });
        
        // 第三步：取前5个
        squared.resize(std::min(squared.size(), size_t{5}));
        
        return squared;
    }
    
    // Ranges惰性求值方法
    static auto ranges_approach(const std::vector<int>& input) {
        return input 
            | std::views::filter([](int n) { return n % 2 == 0; })
            | std::views::transform([](int n) { return n * n; })
            | std::views::take(5);
        // 注意：此时还没有实际计算，只是创建了视图
    }
};

void demonstrate_lazy_evaluation() {
    std::cout << "=== 视图惰性求值演示 ===\n";
    
    std::vector<int> numbers;
    for (int i = 1; i <= 1000000; ++i) {
        numbers.push_back(i);
    }
    
    std::cout << "输入数据大小: " << numbers.size() << "\n";
    
    // 传统方法：立即执行所有操作
    std::cout << "\n传统STL方法:\n";
    auto start = std::chrono::high_resolution_clock::now();
    auto traditional_result = PerformanceComparison::traditional_approach(numbers);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "传统方法耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() 
              << " 微秒\n";
    std::cout << "结果: ";
    for (int n : traditional_result) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    
    // Ranges方法：惰性求值
    std::cout << "\nRanges惰性求值方法:\n";
    start = std::chrono::high_resolution_clock::now();
    auto ranges_view = PerformanceComparison::ranges_approach(numbers);
    auto creation_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "视图创建耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(creation_time - start).count() 
              << " 微秒\n";
    
    // 实际计算发生在遍历时
    std::cout << "结果: ";
    for (int n : ranges_view) {
        std::cout << n << " ";
    }
    end = std::chrono::high_resolution_clock::now();
    
    std::cout << "\n总耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() 
              << " 微秒\n";
    
    std::cout << "\n";
}

// ===== 2. 组合式编程演示 =====
// 复杂的数据处理流水线
void demonstrate_pipeline_programming() {
    std::cout << "=== 组合式编程演示 ===\n";
    
    // 模拟一个学生数据集
    struct Student {
        std::string name;
        int age;
        double gpa;
        std::string major;
        
        friend std::ostream& operator<<(std::ostream& os, const Student& s) {
            return os << s.name << "(" << s.age << ", " << s.gpa << ", " << s.major << ")";
        }
    };
    
    std::vector<Student> students = {
        {"Alice", 20, 3.8, "CS"},
        {"Bob", 19, 3.2, "Math"},
        {"Charlie", 21, 3.9, "CS"},
        {"Diana", 20, 3.6, "Physics"},
        {"Eve", 22, 3.5, "CS"},
        {"Frank", 19, 3.1, "Math"},
        {"Grace", 21, 4.0, "Physics"}
    };
    
    std::cout << "原始数据:\n";
    for (const auto& student : students) {
        std::cout << "  " << student << "\n";
    }
    
    // 复杂的查询：找出CS专业、年龄>=20、GPA>3.5的学生，按GPA排序
    std::cout << "\n使用Ranges流水线处理:\n";
    auto cs_honors = students 
        | std::views::filter([](const Student& s) { 
            return s.major == "CS"; 
        })
        | std::views::filter([](const Student& s) { 
            return s.age >= 20; 
        })
        | std::views::filter([](const Student& s) { 
            return s.gpa > 3.5; 
        });
    
    std::cout << "CS优秀学生:\n";
    for (const auto& student : cs_honors) {
        std::cout << "  " << student << "\n";
    }
    
    // 更复杂的组合：按专业分组处理
    std::cout << "\n按专业统计平均GPA:\n";
    std::vector<std::string> majors = {"CS", "Math", "Physics"};
    
    for (const auto& major : majors) {
        auto major_students = students 
            | std::views::filter([major](const Student& s) { 
                return s.major == major; 
            });
        
        // 计算平均GPA
        double total_gpa = 0.0;
        int count = 0;
        for (const auto& student : major_students) {
            total_gpa += student.gpa;
            ++count;
        }
        
        if (count > 0) {
            std::cout << "  " << major << ": 平均GPA = " 
                      << (total_gpa / count) << " (" << count << "人)\n";
        }
    }
    
    // 嵌套流水线：每个专业的前N名学生
    std::cout << "\n每个专业的优秀学生(GPA>=3.5):\n";
    for (const auto& major : majors) {
        std::cout << "  " << major << "专业:\n";
        
        auto top_students = students 
            | std::views::filter([major](const Student& s) { 
                return s.major == major && s.gpa >= 3.5; 
            })
            | std::views::take(2);  // 取前2名
        
        for (const auto& student : top_students) {
            std::cout << "    " << student << "\n";
        }
    }
    
    std::cout << "\n";
}

// ===== 3. 自定义视图实现演示 =====
// 自定义斐波那契数列视图
class FibonacciView : public std::ranges::view_interface<FibonacciView> {
private:
    size_t count_;
    
public:
    explicit FibonacciView(size_t count) : count_(count) {}
    
    class Iterator {
    private:
        size_t pos_;
        size_t current_;
        size_t next_;
        
    public:
        using value_type = size_t;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        
        explicit Iterator(size_t pos = 0) 
            : pos_(pos), current_(pos == 0 ? 0 : 1), next_(1) {}
        
        size_t operator*() const { 
            return current_; 
        }
        
        Iterator& operator++() {
            auto temp = current_ + next_;
            current_ = next_;
            next_ = temp;
            ++pos_;
            return *this;
        }
        
        Iterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return pos_ == other.pos_;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };
    
    Iterator begin() const { return Iterator(0); }
    Iterator end() const { return Iterator(count_); }
};

// 自定义素数生成视图
class PrimeView : public std::ranges::view_interface<PrimeView> {
private:
    size_t max_value_;
    
    static bool is_prime(size_t n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        
        for (size_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
    
public:
    explicit PrimeView(size_t max_value) : max_value_(max_value) {}
    
    class Iterator {
    private:
        size_t current_;
        size_t max_value_;
        
        void advance_to_next_prime() {
            do {
                ++current_;
            } while (current_ <= max_value_ && !is_prime(current_));
        }
        
    public:
        using value_type = size_t;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        Iterator(size_t start, size_t max_value) 
            : current_(start), max_value_(max_value) {
            if (current_ <= max_value_ && !is_prime(current_)) {
                advance_to_next_prime();
            }
        }
        
        size_t operator*() const { return current_; }
        
        Iterator& operator++() {
            advance_to_next_prime();
            return *this;
        }
        
        Iterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };
    
    Iterator begin() const { return Iterator(2, max_value_); }
    Iterator end() const { return Iterator(max_value_ + 1, max_value_); }
};

// 自定义窗口视图（滑动窗口）
template<std::ranges::view R>
class SlidingWindowView : public std::ranges::view_interface<SlidingWindowView<R>> {
private:
    R base_;
    size_t window_size_;
    
public:
    SlidingWindowView(R base, size_t window_size) 
        : base_(std::move(base)), window_size_(window_size) {}
    
    class Iterator {
    private:
        std::ranges::iterator_t<R> current_;
        std::ranges::iterator_t<R> end_;
        size_t window_size_;
        
    public:
        using value_type = std::vector<std::ranges::range_value_t<R>>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        Iterator(std::ranges::iterator_t<R> current, 
                std::ranges::iterator_t<R> end, 
                size_t window_size)
            : current_(current), end_(end), window_size_(window_size) {}
        
        value_type operator*() const {
            value_type window;
            auto it = current_;
            for (size_t i = 0; i < window_size_ && it != end_; ++i, ++it) {
                window.push_back(*it);
            }
            return window;
        }
        
        Iterator& operator++() {
            if (current_ != end_) ++current_;
            return *this;
        }
        
        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };
    
    Iterator begin() const { 
        return Iterator(std::ranges::begin(base_), std::ranges::end(base_), window_size_); 
    }
    
    Iterator end() const { 
        auto base_end = std::ranges::end(base_);
        return Iterator(base_end, base_end, window_size_); 
    }
};

// 便利函数
auto fibonacci(size_t count) { return FibonacciView(count); }
auto primes(size_t max_value) { return PrimeView(max_value); }

template<std::ranges::view R>
auto sliding_window(R&& r, size_t window_size) {
    return SlidingWindowView(std::forward<R>(r), window_size);
}

void demonstrate_custom_views() {
    std::cout << "=== 自定义视图实现演示 ===\n";
    
    // 斐波那契数列视图
    std::cout << "斐波那契数列前10项:\n";
    for (auto fib : fibonacci(10)) {
        std::cout << fib << " ";
    }
    std::cout << "\n\n";
    
    // 素数视图
    std::cout << "100以内的素数:\n";
    for (auto prime : primes(100)) {
        std::cout << prime << " ";
    }
    std::cout << "\n\n";
    
    // 组合自定义视图
    std::cout << "斐波那契数列中的素数:\n";
    auto fib_primes = fibonacci(20) 
        | std::views::filter([](size_t n) {
            if (n < 2) return false;
            if (n == 2) return true;
            if (n % 2 == 0) return false;
            for (size_t i = 3; i * i <= n; i += 2) {
                if (n % i == 0) return false;
            }
            return true;
        });
    
    for (auto prime : fib_primes) {
        std::cout << prime << " ";
    }
    std::cout << "\n\n";
    
    // 滑动窗口视图
    std::cout << "滑动窗口视图演示:\n";
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    std::cout << "原始数据: ";
    for (int n : data) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    
    std::cout << "大小为3的滑动窗口:\n";
    for (auto window : sliding_window(data | std::views::all, 3)) {
        std::cout << "[";
        for (size_t i = 0; i < window.size(); ++i) {
            std::cout << window[i];
            if (i < window.size() - 1) std::cout << ", ";
        }
        std::cout << "] ";
    }
    std::cout << "\n\n";
}

// ===== 4. Range算法增强演示 =====
void demonstrate_enhanced_algorithms() {
    std::cout << "=== Range算法增强演示 ===\n";
    
    // 测试数据
    struct Employee {
        std::string name;
        int age;
        double salary;
        std::string department;
        
        friend std::ostream& operator<<(std::ostream& os, const Employee& e) {
            return os << e.name << "(" << e.age << ", $" << e.salary << ", " << e.department << ")";
        }
    };
    
    std::vector<Employee> employees = {
        {"Alice", 28, 75000, "Engineering"},
        {"Bob", 32, 85000, "Engineering"}, 
        {"Carol", 25, 60000, "Marketing"},
        {"David", 35, 95000, "Engineering"},
        {"Eve", 29, 70000, "Marketing"},
        {"Frank", 31, 80000, "Sales"}
    };
    
    std::cout << "原始员工数据:\n";
    for (const auto& emp : employees) {
        std::cout << "  " << emp << "\n";
    }
    
    // 1. 投影(Projection)的强大功能
    std::cout << "\n使用投影排序（按工资降序）:\n";
    std::vector<Employee> salary_sorted = employees;
    std::ranges::sort(salary_sorted, std::greater{}, &Employee::salary);  // 投影到salary字段
    
    for (const auto& emp : salary_sorted) {
        std::cout << "  " << emp << "\n";
    }
    
    // 2. 多重投影
    std::cout << "\n按部门排序，同部门内按年龄排序:\n";
    std::vector<Employee> dept_age_sorted = employees;
    std::ranges::sort(dept_age_sorted, {}, [](const Employee& e) {
        return std::make_tuple(e.department, e.age);  // 复合投影
    });
    
    for (const auto& emp : dept_age_sorted) {
        std::cout << "  " << emp << "\n";
    }
    
    // 3. 算法与视图的结合
    std::cout << "\n查找工程部最高薪水:\n";
    auto engineering_max_salary = std::ranges::max(
        employees 
            | std::views::filter([](const Employee& e) { 
                return e.department == "Engineering"; 
            }),
        {},  // 默认比较
        &Employee::salary  // 投影到salary
    );
    std::cout << "工程部最高薪水: " << engineering_max_salary << "\n";
    
    // 4. 范围算法的约束检查
    std::cout << "\n使用ranges::find_if查找:\n";
    auto high_earner = std::ranges::find_if(employees, 
        [](double salary) { return salary > 90000; },
        &Employee::salary);  // 投影
    
    if (high_earner != employees.end()) {
        std::cout << "找到高收入员工: " << *high_earner << "\n";
    }
    
    // 5. 范围算法的返回类型增强
    std::cout << "\n使用ranges::equal_range:\n";
    std::vector<Employee> age_sorted = employees;
    std::ranges::sort(age_sorted, {}, &Employee::age);
    
    auto [lower, upper] = std::ranges::equal_range(age_sorted, 29, {}, &Employee::age);
    std::cout << "29岁的员工:\n";
    for (auto it = lower; it != upper; ++it) {
        std::cout << "  " << *it << "\n";
    }
    
    std::cout << "\n";
}

// ===== 5. constexpr Ranges演示 =====
constexpr auto compile_time_range_operations() {
    std::array data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // 编译期范围操作
    auto filtered_transformed = data 
        | std::views::filter([](int x) { return x % 2 == 0; })
        | std::views::transform([](int x) { return x * x; });
    
    // 编译期聚合
    int sum = 0;
    for (int value : filtered_transformed) {
        sum += value;
    }
    
    return sum;
}

// 编译期字符串处理
constexpr auto process_compile_time_string() {
    constexpr std::string_view text = "Hello World C++20 Ranges";
    
    // 编译期字符计数
    int letter_count = 0;
    int space_count = 0;
    
    for (char c : text) {
        if (std::isalpha(c)) {
            ++letter_count;
        } else if (std::isspace(c)) {
            ++space_count;
        }
    }
    
    return std::make_pair(letter_count, space_count);
}

void demonstrate_constexpr_ranges() {
    std::cout << "=== constexpr Ranges演示 ===\n";
    
    // 编译期计算的结果
    constexpr int compile_time_sum = compile_time_range_operations();
    std::cout << "编译期范围操作结果: " << compile_time_sum << "\n";
    
    constexpr auto [letters, spaces] = process_compile_time_string();
    std::cout << "编译期字符串分析: " << letters << "个字母, " << spaces << "个空格\n";
    
    // 编译期斐波那契计算
    constexpr auto compile_time_fibonacci = []() {
        std::array<int, 10> fib{};
        fib[0] = 0;
        fib[1] = 1;
        
        for (size_t i = 2; i < fib.size(); ++i) {
            fib[i] = fib[i-1] + fib[i-2];
        }
        
        return fib;
    }();
    
    std::cout << "编译期斐波那契数列: ";
    for (int n : compile_time_fibonacci) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    
    // 编译期质数筛选
    constexpr auto compile_time_primes = []() {
        std::array candidates{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        std::array<int, 8> primes{};  // 20以内有8个质数
        
        auto is_prime = [](int n) {
            if (n < 2) return false;
            for (int i = 2; i * i <= n; ++i) {
                if (n % i == 0) return false;
            }
            return true;
        };
        
        int prime_count = 0;
        for (int candidate : candidates) {
            if (is_prime(candidate)) {
                primes[prime_count++] = candidate;
            }
        }
        
        return primes;
    }();
    
    std::cout << "编译期质数列表: ";
    for (int prime : compile_time_primes) {
        if (prime > 0) {  // 跳过未填充的元素
            std::cout << prime << " ";
        }
    }
    std::cout << "\n\n";
}

// 高级应用：数据流处理管道
void demonstrate_advanced_pipeline() {
    std::cout << "=== 高级数据流处理管道 ===\n";
    
    // 模拟传感器数据
    struct SensorReading {
        int sensor_id;
        double value;
        std::string timestamp;
        bool is_valid;
    };
    
    std::vector<SensorReading> sensor_data = {
        {1, 23.5, "2023-01-01T10:00:00", true},
        {1, 24.1, "2023-01-01T10:01:00", true},
        {2, 18.7, "2023-01-01T10:00:00", false},  // 无效数据
        {1, 25.3, "2023-01-01T10:02:00", true},
        {2, 19.2, "2023-01-01T10:01:00", true},
        {3, 30.1, "2023-01-01T10:00:00", true},
        {1, -999.0, "2023-01-01T10:03:00", false}, // 异常数据
        {2, 20.1, "2023-01-01T10:02:00", true},
        {3, 31.5, "2023-01-01T10:01:00", true}
    };
    
    std::cout << "传感器数据处理管道:\n";
    
    // 复杂的数据处理流水线
    auto processed_data = sensor_data
        | std::views::filter([](const SensorReading& reading) {
            return reading.is_valid && reading.value > -100;  // 过滤有效数据
        })
        | std::views::transform([](const SensorReading& reading) {
            // 数据标准化：转换为摄氏度并四舍五入
            return std::make_tuple(reading.sensor_id, 
                                 std::round(reading.value * 10) / 10.0,
                                 reading.timestamp);
        })
        | std::views::filter([](const auto& processed) {
            auto [id, temp, time] = processed;
            return temp >= 15.0 && temp <= 35.0;  // 合理温度范围
        });
    
    std::cout << "处理后的有效数据:\n";
    for (const auto& [sensor_id, temperature, timestamp] : processed_data) {
        std::cout << "  传感器" << sensor_id << ": " << temperature 
                  << "°C @ " << timestamp << "\n";
    }
    
    // 按传感器ID分组统计
    std::cout << "\n按传感器统计平均温度:\n";
    std::vector<int> sensor_ids = {1, 2, 3};
    
    for (int id : sensor_ids) {
        auto sensor_readings = sensor_data
            | std::views::filter([id](const SensorReading& r) {
                return r.sensor_id == id && r.is_valid && r.value > -100;
            })
            | std::views::transform([](const SensorReading& r) {
                return r.value;
            });
        
        double sum = 0.0;
        int count = 0;
        for (double temp : sensor_readings) {
            sum += temp;
            ++count;
        }
        
        if (count > 0) {
            std::cout << "  传感器" << id << ": 平均温度 " 
                      << (sum / count) << "°C (" << count << "个有效读数)\n";
        }
    }
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 Ranges函数式编程深度解析\n";
    std::cout << "===============================\n";
    
    demonstrate_lazy_evaluation();
    demonstrate_pipeline_programming();
    demonstrate_custom_views();
    demonstrate_enhanced_algorithms();
    demonstrate_constexpr_ranges();
    demonstrate_advanced_pipeline();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 02_ranges.cpp -o ranges
./ranges

关键学习点:
1. Ranges视图采用惰性求值，只在实际遍历时才执行计算
2. 管道操作符|提供了清晰的函数组合语法，支持流式编程
3. 自定义视图通过继承view_interface可以实现复杂的数据处理逻辑
4. Range算法支持投影，简化了对复杂数据结构的操作
5. constexpr ranges支持编译期范围计算，提高运行时性能

注意事项:
- 视图是轻量级对象，但要注意底层数据的生命周期
- 惰性求值可能导致多次遍历时重复计算，适当时可以转为容器
- 自定义视图的迭代器需要满足相应的迭代器概念要求
- 复杂的管道可能影响编译速度，需要权衡可读性和性能
*/