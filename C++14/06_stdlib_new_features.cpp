/*
 * C++14 标准库新特性深度解析
 * 
 * C++14虽然主要是对C++11的完善，但标准库方面也有重要改进：
 * 1. <chrono>库的用户定义字面值 - 时间单位的直观表示
 * 2. <string>库的std::make_unique - 统一智能指针创建
 * 3. <memory>库的std::exchange - 原子性值交换与状态管理
 * 4. <memory>库的std::get_temporary_buffer改进 - 临时缓冲区管理
 * 5. <type_traits>库的新类型特征 - 编译期类型检查增强
 * 6. <tuple>库的std::get按类型访问 - 增强元组操作
 * 7. <algorithm>库的新算法 - 交换操作和比较算法
 * 8. <iterator>库的std::make_reverse_iterator - 迭代器适配器
 * 
 * 这些改进体现了C++14对库的完善和对实际编程问题的解决。
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <complex>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace cpp14_stdlib {

// ===== 1. <chrono>库用户定义字面值 =====

namespace ChronoLiterals {
    using namespace std::chrono_literals;
    
    // 时间间隔计算和转换
    class TimeIntervalCalculator {
    private:
        std::chrono::system_clock::duration duration_;
        
    public:
        constexpr TimeIntervalCalculator(std::chrono::system_clock::duration duration) 
            : duration_(duration) {}
        
        // 转换为不同时间单位
        template<typename Duration>
        constexpr Duration as() const {
            return std::chrono::duration_cast<Duration>(duration_);
        }
        
        // 格式化输出
        std::string format() const {
            auto hours = as<std::chrono::hours>();
            auto minutes = as<std::chrono::minutes>() % 1h;
            auto seconds = as<std::chrono::seconds>() % 1min;
            auto milliseconds = as<std::chrono::milliseconds>() % 1s;
            
            std::ostringstream oss;
            oss << hours.count() << "h " << minutes.count() << "m " 
                << seconds.count() << "s " << milliseconds.count() << "ms";
            return oss.str();
        }
        
        // 时间间隔运算
        constexpr TimeIntervalCalculator operator+(const TimeIntervalCalculator& other) const {
            return TimeIntervalCalculator(duration_ + other.duration_);
        }
        
        constexpr TimeIntervalCalculator operator-(const TimeIntervalCalculator& other) const {
            return TimeIntervalCalculator(duration_ - other.duration_);
        }
        
        constexpr TimeIntervalCalculator operator*(int factor) const {
            return TimeIntervalCalculator(duration_ * factor);
        }
    };
    
    // 性能基准测试器
    class PerformanceBenchmark {
    private:
        std::string name_;
        std::chrono::high_resolution_clock::time_point start_;
        
    public:
        explicit PerformanceBenchmark(const std::string& name) : name_(name) {
            start_ = std::chrono::high_resolution_clock::now();
        }
        
        ~PerformanceBenchmark() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = end - start_;
            
            std::cout << "性能测试 [" << name_ << "]: ";
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << " μs";
            std::cout << " (" << std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() << " ns)\n";
        }
    };
    
    // 定时器类
    class Timer {
    private:
        std::chrono::milliseconds interval_;
        std::atomic<bool> running_{false};
        std::thread timer_thread_;
        std::function<void()> callback_;
        
    public:
        Timer(std::chrono::milliseconds interval, std::function<void()> callback)
            : interval_(interval), callback_(callback) {}
        
        void start() {
            running_ = true;
            timer_thread_ = std::thread([this]() {
                while (running_) {
                    auto start = std::chrono::steady_clock::now();
                    callback_();
                    auto end = std::chrono::steady_clock::now();
                    auto elapsed = end - start;
                    
                    if (elapsed < interval_) {
                        std::this_thread::sleep_for(interval_ - elapsed);
                    }
                }
            });
        }
        
        void stop() {
            running_ = false;
            if (timer_thread_.joinable()) {
                timer_thread_.join();
            }
        }
        
        ~Timer() {
            stop();
        }
    };
}

// ===== 2. <memory>库的std::make_unique和std::exchange =====

namespace MemoryUtilities {
    // 资源管理器类
    template<typename T>
    class ResourceManager {
    private:
        std::unique_ptr<T> resource_;
        std::string resource_name_;
        
    public:
        explicit ResourceManager(const std::string& name) : resource_name_(name) {}
        
        // 使用std::make_unique创建资源
        template<typename... Args>
        void create_resource(Args&&... args) {
            resource_ = std::make_unique<T>(std::forward<Args>(args)...);
            std::cout << "创建资源 [" << resource_name_ << "]\n";
        }
        
        // 使用std::exchange转移资源所有权
        std::unique_ptr<T> release_resource() {
            std::cout << "释放资源 [" << resource_name_ << "]\n";
            return std::exchange(resource_, nullptr);
        }
        
        // 检查资源状态
        bool has_resource() const {
            return resource_ != nullptr;
        }
        
        // 获取资源
        T* get() const {
            return resource_.get();
        }
        
        // 使用std::exchange更新资源
        template<typename... Args>
        void replace_resource(Args&&... args) {
            auto old_resource = std::exchange(
                resource_, 
                std::make_unique<T>(std::forward<Args>(args)...)
            );
            if (old_resource) {
                std::cout << "替换资源 [" << resource_name_ << "]\n";
            }
        }
    };
    
    // 状态机示例
    class ConnectionState {
    private:
        enum class State { DISCONNECTED, CONNECTING, CONNECTED, ERROR };
        State current_state_{State::DISCONNECTED};
        std::string last_error_;
        
    public:
        // 使用std::exchange进行状态转换
        bool connect() {
            if (current_state_ == State::DISCONNECTED) {
                auto old_state = std::exchange(current_state_, State::CONNECTING);
                std::cout << "状态转换: " << static_cast<int>(old_state) 
                         << " -> " << static_cast<int>(current_state_) << "\n";
                
                // 模拟连接过程
                std::this_thread::sleep_for(100ms);
                
                old_state = std::exchange(current_state_, State::CONNECTED);
                std::cout << "状态转换: " << static_cast<int>(old_state) 
                         << " -> " << static_cast<int>(current_state_) << "\n";
                return true;
            }
            return false;
        }
        
        bool disconnect() {
            if (current_state_ == State::CONNECTED) {
                auto old_state = std::exchange(current_state_, State::DISCONNECTED);
                std::cout << "状态转换: " << static_cast<int>(old_state) 
                         << " -> " << static_cast<int>(current_state_) << "\n";
                return true;
            }
            return false;
        }
        
        void set_error(const std::string& error) {
            auto old_state = std::exchange(current_state_, State::ERROR);
            auto old_error = std::exchange(last_error_, error);
            std::cout << "错误状态: " << error 
                     << " (之前错误: " << (old_error.empty() ? "无" : old_error) << ")\n";
        }
        
        State get_state() const { return current_state_; }
    };
    
    // 原子操作使用std::exchange
    template<typename T>
    class AtomicSwapBuffer {
    private:
        std::atomic<T> buffer_;
        T default_value_;
        
    public:
        AtomicSwapBuffer(T default_value) : default_value_(default_value) {
            buffer_.store(default_value);
        }
        
        // 原子性地交换并获取旧值
        T swap(T new_value) {
            return std::exchange(buffer_.load(), new_value);
        }
        
        // 原子性地获取并重置为默认值
        T get_and_reset() {
            return std::exchange(buffer_.load(), default_value_);
        }
        
        T get() const {
            return buffer_.load();
        }
    };
}

// ===== 3. <tuple>库的std::get按类型访问 =====

namespace TupleUtilities {
    // 配置系统示例
    class ApplicationConfig {
    private:
        std::tuple<int, std::string, bool, double> config_data_;
        
    public:
        ApplicationConfig(int port, const std::string& host, bool debug, double timeout)
            : config_data_(port, host, debug, timeout) {}
        
        // 使用std::get按类型访问
        template<typename T>
        T get_config() const {
            return std::get<T>(config_data_);
        }
        
        // 设置配置值
        template<typename T>
        void set_config(T value) {
            std::get<T>(config_data_) = value;
        }
        
        // 打印配置
        void print_config() const {
            std::cout << "配置信息:\n";
            std::cout << "  端口: " << std::get<int>(config_data_) << "\n";
            std::cout << "  主机: " << std::get<std::string>(config_data_) << "\n";
            std::cout << "  调试: " << (std::get<bool>(config_data_) ? "是" : "否") << "\n";
            std::cout << "  超时: " << std::get<double>(config_data_) << "s\n";
        }
    };
    
    // 多类型容器
    template<typename... Types>
    class MultiTypeContainer {
    private:
        std::tuple<Types...> data_;
        
    public:
        MultiTypeContainer(Types... args) : data_(args...) {}
        
        template<typename T>
        T get() const {
            return std::get<T>(data_);
        }
        
        template<typename T>
        void set(T value) {
            std::get<T>(data_) = value;
        }
        
        // 访问所有元素
        template<typename Func>
        void for_each(Func&& func) const {
            std::apply([&func](const auto&... args) {
                (func(args), ...);
            }, data_);
        }
    };
    
    // 元组操作工具
    namespace TupleOps {
        // 元组元素类型检查
        template<typename T, typename Tuple>
        struct tuple_contains_type;
        
        template<typename T, typename... Types>
        struct tuple_contains_type<T, std::tuple<Types...>> 
            : std::disjunction<std::is_same<T, Types>...> {};
        
        // 获取元组中指定类型的索引
        template<typename T, typename Tuple>
        struct tuple_type_index;
        
        template<typename T, typename... Types>
        struct tuple_type_index<T, std::tuple<Types...>> {
            static constexpr size_t value = std::index_of<T, Types...>::value;
        };
        
        // 安全的按类型获取
        template<typename T, typename... Types>
        constexpr T safe_get(const std::tuple<Types...>& tuple) {
            static_assert(tuple_contains_type<T, std::tuple<Types...>>::value,
                         "类型不在元组中");
            return std::get<T>(tuple);
        }
    }
}

// ===== 4. <type_traits>库的新类型特征 =====

namespace TypeTraits {
    // C++14引入的新类型特征
    template<typename T>
    using add_const_t = typename std::add_const<T>::type;
    
    template<typename T>
    using add_volatile_t = typename std::add_volatile<T>::type;
    
    template<typename T>
    using add_cv_t = typename std::add_cv<T>::type;
    
    template<typename T>
    using remove_const_t = typename std::remove_const<T>::type;
    
    template<typename T>
    using remove_volatile_t = typename std::remove_volatile<T>::type;
    
    template<typename T>
    using remove_cv_t = typename std::remove_cv<T>::type;
    
    template<typename T>
    using add_lvalue_reference_t = typename std::add_lvalue_reference<T>::type;
    
    template<typename T>
    using add_rvalue_reference_t = typename std::add_rvalue_reference<T>::type;
    
    template<typename T>
    using remove_reference_t = typename std::remove_reference<T>::type;
    
    template<typename T>
    using add_pointer_t = typename std::add_pointer<T>::type;
    
    template<typename T>
    using remove_pointer_t = typename std::remove_pointer<T>::type;
    
    template<typename T>
    using make_signed_t = typename std::make_signed<T>::type;
    
    template<typename T>
    using make_unsigned_t = typename std::make_unsigned<T>::type;
    
    template<typename T>
    using remove_extent_t = typename std::remove_extent<T>::type;
    
    template<typename T>
    using remove_all_extents_t = typename std::remove_all_extents<T>::type;
    
    template<typename T>
    using decay_t = typename std::decay<T>::type;
    
    template<bool B, typename T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;
    
    template<bool B, typename T, typename F>
    using conditional_t = typename std::conditional<B, T, F>::type;
    
    template<typename... T>
    using common_type_t = typename std::common_type<T...>::type;
    
    template<typename T>
    using underlying_type_t = typename std::underlying_type<T>::type;
    
    // 类型特征应用示例
    template<typename T>
    class TypeAnalyzer {
    public:
        static void analyze() {
            std::cout << "类型分析: " << typeid(T).name() << "\n";
            std::cout << "  是否为const: " << std::is_const_v<T> << "\n";
            std::cout << "  是否为volatile: " << std::is_volatile_v<T> << "\n";
            std::cout << "  是否为引用: " << std::is_reference_v<T> << "\n";
            std::cout << "  是否为指针: " << std::is_pointer_v<T> << "\n";
            std::cout << "  是否为数组: " << std::is_array_v<T> << "\n";
            std::cout << "  是否为函数: " << std::is_function_v<T> << "\n";
            std::cout << "  是否为类: " << std::is_class_v<T> << "\n";
            std::cout << "  是否为枚举: " << std::is_enum_v<T> << "\n";
            std::cout << "  是否为联合: " << std::is_union_v<T> << "\n";
            std::cout << "  是否为算术类型: " << std::is_arithmetic_v<T> << "\n";
            std::cout << "  是否为有符号: " << std::is_signed_v<T> << "\n";
            std::cout << "  是否为无符号: " << std::is_unsigned_v<T> << "\n";
            std::cout << "  大小: " << sizeof(T) << " 字节\n";
        }
    };
    
    // SFINAE应用示例
    template<typename T, enable_if_t<std::is_integral_v<T>, int> = 0>
    constexpr T safe_integral_add(T a, T b) {
        if (a > 0 && b > std::numeric_limits<T>::max() - a) {
            throw std::overflow_error("整数溢出");
        }
        if (a < 0 && b < std::numeric_limits<T>::min() - a) {
            throw std::underflow_error("整数下溢");
        }
        return a + b;
    }
    
    template<typename T, enable_if_t<std::is_floating_point_v<T>, int> = 0>
    constexpr T safe_floating_add(T a, T b) {
        return a + b;  // 浮点数有内置的溢出处理
    }
}

// ===== 5. <algorithm>库的新算法 =====

namespace AlgorithmExtensions {
    // std::exchange的应用示例
    template<typename T>
    class ValueHistory {
    private:
        std::vector<T> history_;
        T current_;
        
    public:
        explicit ValueHistory(T initial) : current_(initial) {
            history_.push_back(initial);
        }
        
        // 记录历史值并更新当前值
        T update(T new_value) {
            auto old_value = std::exchange(current_, new_value);
            history_.push_back(old_value);
            return old_value;
        }
        
        const std::vector<T>& get_history() const {
            return history_;
        }
        
        T get_current() const {
            return current_;
        }
    };
    
    // std::make_reverse_iterator应用
    template<typename Container>
    class ReversedContainer {
    private:
        Container& container_;
        
    public:
        explicit ReversedContainer(Container& container) : container_(container) {}
        
        auto begin() {
            return std::make_reverse_iterator(container_.end());
        }
        
        auto end() {
            return std::make_reverse_iterator(container_.begin());
        }
        
        auto begin() const {
            return std::make_reverse_iterator(container_.end());
        }
        
        auto end() const {
            return std::make_reverse_iterator(container_.begin());
        }
    };
    
    // 算法性能测试
    void benchmark_algorithms() {
        std::cout << "\n===== 算法性能测试 =====\n";
        
        std::vector<int> large_vector(1000000);
        std::iota(large_vector.begin(), large_vector.end(), 0);
        
        // 测试std::reverse
        {
            ChronoLiterals::PerformanceBenchmark bench("std::reverse");
            std::vector<int> copy = large_vector;
            std::reverse(copy.begin(), copy.end());
        }
        
        // 测试反向迭代器
        {
            ChronoLiterals::PerformanceBenchmark bench("反向迭代器");
            std::vector<int> copy = large_vector;
            std::vector<int> reversed;
            reversed.reserve(copy.size());
            std::copy(copy.rbegin(), copy.rend(), std::back_inserter(reversed));
        }
    }
}

// ===== 6. <iterator>库的std::make_reverse_iterator =====

namespace IteratorUtilities {
    // 自定义容器示例
    template<typename T>
    class CircularBuffer {
    private:
        std::vector<T> buffer_;
        size_t head_{0};
        size_t tail_{0};
        size_t size_{0};
        
    public:
        explicit CircularBuffer(size_t capacity) : buffer_(capacity) {}
        
        void push_back(const T& value) {
            if (size_ == buffer_.size()) {
                throw std::runtime_error("缓冲区已满");
            }
            
            buffer_[tail_] = value;
            tail_ = (tail_ + 1) % buffer_.size();
            ++size_;
        }
        
        T pop_front() {
            if (size_ == 0) {
                throw std::runtime_error("缓冲区为空");
            }
            
            T value = buffer_[head_];
            head_ = (head_ + 1) % buffer_.size();
            --size_;
            return value;
        }
        
        // 正向迭代器
        auto begin() { return buffer_.begin() + head_; }
        auto end() { return buffer_.begin() + (head_ + size_); }
        
        // 反向迭代器
        auto rbegin() { 
            return std::make_reverse_iterator(buffer_.begin() + (head_ + size_)); 
        }
        auto rend() { 
            return std::make_reverse_iterator(buffer_.begin() + head_); 
        }
        
        size_t size() const { return size_; }
        bool empty() const { return size_ == 0; }
    };
    
    // 迭代器适配器示例
    template<typename Iterator>
    class StepIterator {
    private:
        Iterator current_;
        Iterator end_;
        size_t step_;
        
    public:
        StepIterator(Iterator current, Iterator end, size_t step)
            : current_(current), end_(end), step_(step) {}
        
        Iterator operator*() const { return current_; }
        
        StepIterator& operator++() {
            for (size_t i = 0; i < step_ && current_ != end_; ++i) {
                ++current_;
            }
            return *this;
        }
        
        bool operator!=(const StepIterator& other) const {
            return current_ != other.current_;
        }
    };
    
    template<typename Iterator>
    StepIterator<Iterator> make_step_iterator(Iterator current, Iterator end, size_t step) {
        return StepIterator<Iterator>(current, end, step);
    }
}

// ===== 7. 综合应用示例 =====

namespace ComprehensiveExamples {
    // 任务调度器
    class TaskScheduler {
    private:
        using Task = std::function<void()>;
        using TimePoint = std::chrono::system_clock::time_point;
        
        struct ScheduledTask {
            TimePoint execute_time;
            Task task;
            std::string name;
            
            bool operator<(const ScheduledTask& other) const {
                return execute_time > other.execute_time;  // 优先队列需要
            }
        };
        
        std::priority_queue<ScheduledTask> task_queue_;
        std::atomic<bool> running_{false};
        std::thread scheduler_thread_;
        std::mutex queue_mutex_;
        std::condition_variable cv_;
        
    public:
        TaskScheduler() = default;
        
        ~TaskScheduler() {
            stop();
        }
        
        void start() {
            running_ = true;
            scheduler_thread_ = std::thread([this]() {
                while (running_) {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    
                    if (task_queue_.empty()) {
                        cv_.wait(lock);
                        continue;
                    }
                    
                    auto now = std::chrono::system_clock::now();
                    auto& next_task = task_queue_.top();
                    
                    if (now >= next_task.execute_time) {
                        auto task = std::move(next_task.task);
                        auto name = next_task.name;
                        task_queue_.pop();
                        lock.unlock();
                        
                        try {
                            std::cout << "执行任务: " << name << "\n";
                            task();
                        } catch (const std::exception& e) {
                            std::cerr << "任务执行失败: " << name << " - " << e.what() << "\n";
                        }
                    } else {
                        cv_.wait_until(lock, next_task.execute_time);
                    }
                }
            });
        }
        
        void stop() {
            running_ = false;
            cv_.notify_all();
            if (scheduler_thread_.joinable()) {
                scheduler_thread_.join();
            }
        }
        
        template<typename Rep, typename Period>
        void schedule_after(const std::chrono::duration<Rep, Period>& delay, 
                           Task task, const std::string& name) {
            auto execute_time = std::chrono::system_clock::now() + delay;
            schedule_at(execute_time, std::move(task), name);
        }
        
        void schedule_at(TimePoint execute_time, Task task, const std::string& name) {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            task_queue_.push({execute_time, std::move(task), name});
            cv_.notify_one();
        }
    };
    
    // 配置管理系统
    class ConfigurationManager {
    private:
        using ConfigTuple = std::tuple<int, std::string, bool, double, std::vector<std::string>>;
        ConfigTuple config_;
        std::string config_file_;
        std::mutex config_mutex_;
        
    public:
        ConfigurationManager(const std::string& file) : config_file_(file) {
            // 默认配置
            config_ = std::make_tuple(8080, "localhost", false, 30.0, std::vector<std::string>{"user", "admin"});
        }
        
        // 获取配置项
        template<typename T>
        T get_config() const {
            std::lock_guard<std::mutex> lock(config_mutex_);
            return std::get<T>(config_);
        }
        
        // 设置配置项
        template<typename T>
        void set_config(T value) {
            std::lock_guard<std::mutex> lock(config_mutex_);
            auto old_value = std::exchange(std::get<T>(config_), value);
            std::cout << "配置更新: " << typeid(T).name() 
                     << " = " << value << " (旧值: " << old_value << ")\n";
        }
        
        // 打印配置
        void print_config() const {
            std::lock_guard<std::mutex> lock(config_mutex_);
            std::cout << "当前配置:\n";
            std::cout << "  端口: " << std::get<int>(config_) << "\n";
            std::cout << "  主机: " << std::get<std::string>(config_) << "\n";
            std::cout << "  调试: " << (std::get<bool>(config_) ? "是" : "否") << "\n";
            std::cout << "  超时: " << std::get<double>(config_) << "s\n";
            
            const auto& roles = std::get<std::vector<std::string>>(config_);
            std::cout << "  角色: ";
            for (const auto& role : roles) {
                std::cout << role << " ";
            }
            std::cout << "\n";
        }
    };
}

} // namespace cpp14_stdlib

// ===== 主函数 =====

int main() {
    std::cout << "=== C++14 标准库新特性深度解析 ===\n";
    
    using namespace cpp14_stdlib;
    
    // 1. Chrono字面值演示
    std::cout << "\n===== 1. Chrono字面值演示 =====\n";
    using namespace ChronoLiterals;
    
    auto work_duration = 8h + 30min + 45s + 500ms;
    TimeIntervalCalculator work_time(work_duration);
    std::cout << "工作时间: " << work_time.format() << "\n";
    
    auto meeting_duration = 2h + 15min;
    TimeIntervalCalculator meeting_time(meeting_duration);
    std::cout << "会议时间: " << meeting_time.format() << "\n";
    
    auto total_time = work_time + meeting_time;
    std::cout << "总时间: " << total_time.format() << "\n";
    
    // 2. 内存管理工具演示
    std::cout << "\n===== 2. 内存管理工具演示 =====\n";
    using namespace MemoryUtilities;
    
    ResourceManager<std::string> db_connection("数据库连接");
    db_connection.create_resource("postgresql://localhost:5432/mydb");
    std::cout << "连接状态: " << (db_connection.has_resource() ? "已连接" : "未连接") << "\n";
    
    auto connection = db_connection.release_resource();
    std::cout << "连接状态: " << (db_connection.has_resource() ? "已连接" : "未连接") << "\n";
    
    // 状态机演示
    ConnectionState conn_state;
    conn_state.connect();
    conn_state.disconnect();
    
    // 原子交换演示
    AtomicSwapBuffer<int> counter(0);
    std::cout << "交换前: " << counter.get() << "\n";
    std::cout << "旧值: " << counter.swap(42) << "\n";
    std::cout << "交换后: " << counter.get() << "\n";
    
    // 3. 元组工具演示
    std::cout << "\n===== 3. 元组工具演示 =====\n";
    using namespace TupleUtilities;
    
    ApplicationConfig config(8080, "localhost", true, 30.0);
    config.print_config();
    
    // 按类型访问
    std::cout << "获取端口: " << config.get_config<int>() << "\n";
    std::cout << "获取主机: " << config.get_config<std::string>() << "\n";
    std::cout << "获取调试: " << config.get_config<bool>() << "\n";
    
    // 多类型容器
    MultiTypeContainer<int, std::string, bool> container(42, "测试", true);
    std::cout << "多类型容器: ";
    container.for_each([](const auto& item) {
        std::cout << item << " ";
    });
    std::cout << "\n";
    
    // 4. 类型特征演示
    std::cout << "\n===== 4. 类型特征演示 =====\n";
    using namespace TypeTraits;
    
    TypeAnalyzer<int>::analyze();
    std::cout << "\n";
    TypeAnalyzer<const std::string&>::analyze();
    std::cout << "\n";
    TypeAnalyzer<std::vector<double>>::analyze();
    
    // 5. 算法扩展演示
    std::cout << "\n===== 5. 算法扩展演示 =====\n";
    using namespace AlgorithmExtensions;
    
    ValueHistory<int> version_history(1);
    version_history.update(2);
    version_history.update(3);
    version_history.update(4);
    
    std::cout << "版本历史: ";
    for (const auto& version : version_history.get_history()) {
        std::cout << version << " -> ";
    }
    std::cout << version_history.get_current() << "\n";
    
    // 6. 迭代器工具演示
    std::cout << "\n===== 6. 迭代器工具演示 =====\n";
    using namespace IteratorUtilities;
    
    CircularBuffer<int> buffer(5);
    for (int i = 1; i <= 5; ++i) {
        buffer.push_back(i);
    }
    
    std::cout << "正向遍历: ";
    for (const auto& item : buffer) {
        std::cout << item << " ";
    }
    std::cout << "\n";
    
    std::cout << "反向遍历: ";
    for (auto it = buffer.rbegin(); it != buffer.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    // 7. 综合应用演示
    std::cout << "\n===== 7. 综合应用演示 =====\n";
    using namespace ComprehensiveExamples;
    
    ConfigurationManager config_manager("app.config");
    config_manager.print_config();
    
    // 更新配置
    config_manager.set_config<int>(9090);
    config_manager.set_config<std::string>("example.com");
    config_manager.set_config<bool>(true);
    
    config_manager.print_config();
    
    // 性能测试
    AlgorithmExtensions::benchmark_algorithms();
    
    // 定时器演示
    std::cout << "\n===== 定时器演示 =====\n";
    Timer periodic_timer(500ms, []() {
        std::cout << "定时器触发: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch()).count() 
                  << "ms\n";
    });
    
    periodic_timer.start();
    std::this_thread::sleep_for(2s);
    periodic_timer.stop();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall -pthread 06_stdlib_new_features.cpp -o stdlib_features
./stdlib_features

关键学习点:
1. Chrono字面值提供了直观的时间单位表示，使时间相关的代码更加清晰
2. std::make_unique完善了智能指针创建的一致性，避免了直接使用new
3. std::exchange提供了原子性的值交换，在状态管理和资源管理中非常有用
4. std::get按类型访问增强了元组的易用性，使配置系统更加类型安全
5. 类型特征别名模板(_t后缀)简化了模板编程的代码
6. std::make_reverse_iterator提供了更灵活的迭代器适配器
7. 这些库改进体现了C++14对实际编程问题的关注和解决

注意事项:
- Chrono字面值需要using namespace std::chrono_literals;或using声明
- std::make_unique比直接使用new更安全，避免了内存泄漏
- std::exchange在多线程环境中需要配合原子类型使用
- std::get按类型访问要求类型在元组中唯一，否则会编译错误
- C++14的库改进主要是完善C++11，没有引入重大的概念性变化
*/