/*
 * C++14 标准用户定义字面值深度解析
 * 
 * C++14在C++11基础上引入了标准库的用户定义字面值，这些字面值提供了：
 * 1. std::chrono_literals - 时间单位的直观表示
 * 2. std::string_literals - 字符串字面值的std::string对象
 * 3. std::complex_literals - 复数字面值的便捷创建
 * 
 * 用户定义字面值的核心价值：
 * - 提高代码的可读性和表达力
 * - 减少类型转换的显式代码
 * - 在编译期提供类型安全
 * - 使特定领域的代码更加直观
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <complex>
#include <ratio>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include <limits>

namespace cpp14_udl {

// ===== 1. std::chrono_literals 深度解析 =====

namespace ChronoUDL {
    using namespace std::chrono_literals;
    
    // 时间间隔计算器
    class TimeCalculator {
    private:
        std::chrono::nanoseconds duration_;
        
    public:
        constexpr TimeCalculator(std::chrono::nanoseconds duration) : duration_(duration) {}
        
        // 转换为各种时间单位
        template<typename Duration>
        constexpr Duration as() const {
            return std::chrono::duration_cast<Duration>(duration_);
        }
        
        // 获取原始持续时间
        constexpr std::chrono::nanoseconds raw_duration() const {
            return duration_;
        }
        
        // 时间运算
        constexpr TimeCalculator operator+(const TimeCalculator& other) const {
            return TimeCalculator(duration_ + other.duration_);
        }
        
        constexpr TimeCalculator operator-(const TimeCalculator& other) const {
            return TimeCalculator(duration_ - other.duration_);
        }
        
        constexpr TimeCalculator operator*(int factor) const {
            return TimeCalculator(duration_ * factor);
        }
        
        constexpr TimeCalculator operator/(int divisor) const {
            return TimeCalculator(duration_ / divisor);
        }
        
        // 格式化输出
        std::string to_string() const {
            auto total_ns = duration_.count();
            
            if (total_ns >= 1'000'000'000) {  // 秒
                auto seconds = as<std::chrono::duration<double>>();
                return std::to_string(seconds.count()) + "s";
            } else if (total_ns >= 1'000'000) {  // 毫秒
                auto ms = as<std::chrono::milliseconds>();
                return std::to_string(ms.count()) + "ms";
            } else if (total_ns >= 1'000) {  // 微秒
                auto us = as<std::chrono::microseconds>();
                return std::to_string(us.count()) + "μs";
            } else {  // 纳秒
                return std::to_string(total_ns) + "ns";
            }
        }
        
        // 详细分解
        std::string detailed_breakdown() const {
            auto hours = as<std::chrono::hours>();
            auto minutes = as<std::chrono::minutes>() % 1h;
            auto seconds = as<std::chrono::seconds>() % 1min;
            auto milliseconds = as<std::chrono::milliseconds>() % 1s;
            auto microseconds = as<std::chrono::microseconds>() % 1ms;
            auto nanoseconds = duration_ % 1us;
            
            std::ostringstream oss;
            if (hours.count() > 0) {
                oss << hours.count() << "h ";
            }
            if (minutes.count() > 0) {
                oss << minutes.count() << "m ";
            }
            if (seconds.count() > 0) {
                oss << seconds.count() << "s ";
            }
            if (milliseconds.count() > 0) {
                oss << milliseconds.count() << "ms ";
            }
            if (microseconds.count() > 0) {
                oss << microseconds.count() << "μs ";
            }
            if (nanoseconds.count() > 0) {
                oss << nanoseconds.count() << "ns";
            }
            
            return oss.str();
        }
    };
    
    // 时间区间类
    class TimeInterval {
    private:
        std::chrono::system_clock::time_point start_;
        std::chrono::system_clock::time_point end_;
        
    public:
        TimeInterval(std::chrono::system_clock::time_point start, 
                     std::chrono::system_clock::time_point end) 
            : start_(start), end_(end) {}
        
        TimeInterval(std::chrono::system_clock::duration duration) 
            : start_(std::chrono::system_clock::now()), 
              end_(start_ + duration) {}
        
        TimeCalculator duration() const {
            return TimeCalculator(end_ - start_);
        }
        
        bool contains(std::chrono::system_clock::time_point point) const {
            return point >= start_ && point <= end_;
        }
        
        bool overlaps(const TimeInterval& other) const {
            return contains(other.start_) || contains(other.end_) ||
                   other.contains(start_) || other.contains(end_);
        }
        
        std::chrono::system_clock::time_point start() const { return start_; }
        std::chrono::system_clock::time_point end() const { return end_; }
    };
    
    // 任务调度器
    class TaskScheduler {
    private:
        struct Task {
            std::chrono::system_clock::time_point execute_time;
            std::function<void()> func;
            std::string name;
        };
        
        std::vector<Task> tasks_;
        std::mutex tasks_mutex_;
        std::atomic<bool> running_{false};
        std::thread scheduler_thread_;
        
    public:
        template<typename Duration>
        void schedule_after(Duration delay, std::function<void()> func, const std::string& name) {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            tasks_.push_back({
                std::chrono::system_clock::now() + delay,
                func,
                name
            });
        }
        
        void start() {
            running_ = true;
            scheduler_thread_ = std::thread([this]() {
                while (running_) {
                    std::this_thread::sleep_for(10ms);  // 检查间隔
                    
                    std::lock_guard<std::mutex> lock(tasks_mutex_);
                    auto now = std::chrono::system_clock::now();
                    
                    for (auto it = tasks_.begin(); it != tasks_.end();) {
                        if (now >= it->execute_time) {
                            std::cout << "执行任务: " << it->name << "\n";
                            try {
                                it->func();
                            } catch (const std::exception& e) {
                                std::cerr << "任务执行失败: " << e.what() << "\n";
                            }
                            it = tasks_.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
            });
        }
        
        void stop() {
            running_ = false;
            if (scheduler_thread_.joinable()) {
                scheduler_thread_.join();
            }
        }
        
        ~TaskScheduler() {
            stop();
        }
    };
    
    // 性能分析器
    class Profiler {
    private:
        struct ProfileEntry {
            std::string name;
            std::chrono::nanoseconds duration;
            size_t call_count;
        };
        
        std::vector<ProfileEntry> entries_;
        std::mutex entries_mutex_;
        
    public:
        class ScopeTimer {
        private:
            Profiler& profiler_;
            std::string name_;
            std::chrono::high_resolution_clock::time_point start_;
            
        public:
            ScopeTimer(Profiler& profiler, const std::string& name)
                : profiler_(profiler), name_(name), start_(std::chrono::high_resolution_clock::now()) {}
            
            ~ScopeTimer() {
                auto duration = std::chrono::high_resolution_clock::now() - start_;
                profiler_.add_entry(name_, duration);
            }
        };
        
        void add_entry(const std::string& name, std::chrono::nanoseconds duration) {
            std::lock_guard<std::mutex> lock(entries_mutex_);
            
            auto it = std::find_if(entries_.begin(), entries_.end(),
                                 [&name](const ProfileEntry& entry) {
                                     return entry.name == name;
                                 });
            
            if (it != entries_.end()) {
                it->duration += duration;
                it->call_count++;
            } else {
                entries_.push_back({name, duration, 1});
            }
        }
        
        void print_report() const {
            std::lock_guard<std::mutex> lock(entries_mutex_);
            
            std::cout << "\n=== 性能分析报告 ===\n";
            for (const auto& entry : entries_) {
                auto avg_duration = entry.duration / entry.call_count;
                std::cout << entry.name << ":\n";
                std::cout << "  总时间: " << TimeCalculator(entry.duration).to_string() << "\n";
                std::cout << "  调用次数: " << entry.call_count << "\n";
                std::cout << "  平均时间: " << TimeCalculator(avg_duration).to_string() << "\n";
            }
        }
    };
}

// ===== 2. std::string_literals 深度解析 =====

namespace StringUDL {
    using namespace std::string_literals;
    
    // 字符串处理器
    class StringProcessor {
    private:
        std::string data_;
        
    public:
        explicit StringProcessor(const std::string& data) : data_(data) {}
        
        // 字符串操作
        StringProcessor to_upper() const {
            std::string result = data_;
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            return StringProcessor(result);
        }
        
        StringProcessor to_lower() const {
            std::string result = data_;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return StringProcessor(result);
        }
        
        StringProcessor trim() const {
            auto start = data_.find_first_not_of(" \t\n\r");
            if (start == std::string::npos) return StringProcessor("");
            
            auto end = data_.find_last_not_of(" \t\n\r");
            return StringProcessor(data_.substr(start, end - start + 1));
        }
        
        std::vector<std::string> split(char delimiter) const {
            std::vector<std::string> result;
            std::stringstream ss(data_);
            std::string item;
            
            while (std::getline(ss, item, delimiter)) {
                result.push_back(item);
            }
            
            return result;
        }
        
        StringProcessor replace(const std::string& from, const std::string& to) const {
            std::string result = data_;
            size_t pos = 0;
            
            while ((pos = result.find(from, pos)) != std::string::npos) {
                result.replace(pos, from.length(), to);
                pos += to.length();
            }
            
            return StringProcessor(result);
        }
        
        bool contains(const std::string& substr) const {
            return data_.find(substr) != std::string::npos;
        }
        
        bool starts_with(const std::string& prefix) const {
            return data_.substr(0, prefix.length()) == prefix;
        }
        
        bool ends_with(const std::string& suffix) const {
            return data_.substr(data_.length() - suffix.length()) == suffix;
        }
        
        const std::string& str() const { return data_; }
        
        // 链式操作
        StringProcessor process() const {
            return *this;
        }
    };
    
    // 配置解析器
    class ConfigParser {
    private:
        std::map<std::string, std::string> config_;
        
    public:
        void parse_line(const std::string& line) {
            auto trimmed = StringProcessor(line).trim();
            if (trimmed.empty() || trimmed.str()[0] == '#') {
                return;  // 跳过注释和空行
            }
            
            auto parts = trimmed.split('=');
            if (parts.size() == 2) {
                auto key = StringProcessor(parts[0]).trim().str();
                auto value = StringProcessor(parts[1]).trim().str();
                config_[key] = value;
            }
        }
        
        void parse(const std::string& content) {
            auto lines = StringProcessor(content).split('\n');
            for (const auto& line : lines) {
                parse_line(line);
            }
        }
        
        std::string get(const std::string& key, const std::string& default_value = "") const {
            auto it = config_.find(key);
            return it != config_.end() ? it->second : default_value;
        }
        
        int get_int(const std::string& key, int default_value = 0) const {
            try {
                return std::stoi(get(key));
            } catch (...) {
                return default_value;
            }
        }
        
        bool get_bool(const std::string& key, bool default_value = false) const {
            auto value = get(key);
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            return value == "true" || value == "1" || value == "yes";
        }
        
        void print_config() const {
            std::cout << "配置项:\n";
            for (const auto& [key, value] : config_) {
                std::cout << "  " << key << " = " << value << "\n";
            }
        }
    };
    
    // 路径操作器
    class Path {
    private:
        std::string path_;
        
    public:
        explicit Path(const std::string& path) : path_(path) {}
        
        Path join(const std::string& component) const {
            if (path_.empty()) return Path(component);
            if (component.empty()) return *this;
            
            auto ends_with_sep = path_.back() == '/' || path_.back() == '\\';
            auto starts_with_sep = component.front() == '/' || component.front() == '\\';
            
            if (ends_with_sep && starts_with_sep) {
                return Path(path_ + component.substr(1));
            } else if (!ends_with_sep && !starts_with_sep) {
                return Path(path_ + "/" + component);
            } else {
                return Path(path_ + component);
            }
        }
        
        std::string filename() const {
            auto last_sep = path_.find_last_of("/\\");
            return last_sep == std::string::npos ? path_ : path_.substr(last_sep + 1);
        }
        
        std::string extension() const {
            auto name = filename();
            auto last_dot = name.find_last_of('.');
            return last_dot == std::string::npos ? "" : name.substr(last_dot);
        }
        
        std::string stem() const {
            auto name = filename();
            auto last_dot = name.find_last_of('.');
            return last_dot == std::string::npos ? name : name.substr(0, last_dot);
        }
        
        Path parent() const {
            auto last_sep = path_.find_last_of("/\\");
            return last_sep == std::string::npos ? Path("") : Path(path_.substr(0, last_sep));
        }
        
        bool is_absolute() const {
            return !path_.empty() && (path_[0] == '/' || (path_.length() > 1 && path_[1] == ':'));
        }
        
        const std::string& str() const { return path_; }
    };
}

// ===== 3. std::complex_literals 深度解析 =====

namespace ComplexUDL {
    using namespace std::complex_literals;
    
    // 复数计算器
    class ComplexCalculator {
    private:
        std::complex<double> value_;
        
    public:
        constexpr ComplexCalculator(std::complex<double> value) : value_(value) {}
        
        // 基本运算
        ComplexCalculator operator+(const ComplexCalculator& other) const {
            return ComplexCalculator(value_ + other.value_);
        }
        
        ComplexCalculator operator-(const ComplexCalculator& other) const {
            return ComplexCalculator(value_ - other.value_);
        }
        
        ComplexCalculator operator*(const ComplexCalculator& other) const {
            return ComplexCalculator(value_ * other.value_);
        }
        
        ComplexCalculator operator/(const ComplexCalculator& other) const {
            return ComplexCalculator(value_ / other.value_);
        }
        
        // 幂运算
        ComplexCalculator power(int exponent) const {
            if (exponent == 0) return ComplexCalculator(1.0 + 0.0i);
            if (exponent == 1) return *this;
            
            ComplexCalculator result = *this;
            for (int i = 1; i < exponent; ++i) {
                result = result * *this;
            }
            return result;
        }
        
        // 复数函数
        ComplexCalculator conjugate() const {
            return ComplexCalculator(std::conj(value_));
        }
        
        double magnitude() const {
            return std::abs(value_);
        }
        
        double phase() const {
            return std::arg(value_);
        }
        
        ComplexCalculator sqrt() const {
            return ComplexCalculator(std::sqrt(value_));
        }
        
        ComplexCalculator exp() const {
            return ComplexCalculator(std::exp(value_));
        }
        
        // 格式化输出
        std::string to_string() const {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3);
            
            double real = value_.real();
            double imag = value_.imag();
            
            oss << real;
            if (imag >= 0) {
                oss << " + " << imag << "i";
            } else {
                oss << " - " << -imag << "i";
            }
            
            return oss.str();
        }
        
        std::string to_polar() const {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3);
            oss << magnitude() << " ∠ " << phase() << " rad";
            return oss.str();
        }
        
        const std::complex<double>& value() const { return value_; }
    };
    
    // FFT相关计算
    namespace FFT {
        // 生成旋转因子
        std::vector<std::complex<double>> generate_twiddle_factors(int n) {
            std::vector<std::complex<double>> factors;
            for (int k = 0; k < n; ++k) {
                double angle = -2.0 * M_PI * k / n;
                factors.push_back(std::exp(std::complex<double>(0, angle)));
            }
            return factors;
        }
        
        // 简单的DFT实现（用于演示）
        std::vector<std::complex<double>> dft(const std::vector<std::complex<double>>& input) {
            int n = input.size();
            std::vector<std::complex<double>> output(n);
            
            for (int k = 0; k < n; ++k) {
                std::complex<double> sum = 0.0;
                for (int m = 0; m < n; ++m) {
                    double angle = -2.0 * M_PI * k * m / n;
                    std::complex<double> twiddle = std::exp(std::complex<double>(0, angle));
                    sum += input[m] * twiddle;
                }
                output[k] = sum;
            }
            
            return output;
        }
    }
    
    // 信号处理示例
    class SignalProcessor {
    private:
        std::vector<std::complex<double>> signal_;
        
    public:
        SignalProcessor(const std::vector<std::complex<double>>& signal) : signal_(signal) {}
        
        // 添加噪声
        void add_noise(double noise_level) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::normal_distribution<double> dist(0, noise_level);
            
            for (auto& sample : signal_) {
                double noise_real = dist(gen);
                double noise_imag = dist(gen);
                sample += std::complex<double>(noise_real, noise_imag);
            }
        }
        
        // 应用滤波器
        void apply_filter(const std::vector<std::complex<double>>& filter_coeffs) {
            std::vector<std::complex<double>> filtered(signal_.size());
            
            for (size_t i = 0; i < signal_.size(); ++i) {
                std::complex<double> sum = 0.0;
                for (size_t j = 0; j < filter_coeffs.size() && j <= i; ++j) {
                    sum += signal_[i - j] * filter_coeffs[j];
                }
                filtered[i] = sum;
            }
            
            signal_ = filtered;
        }
        
        // 计算频谱
        std::vector<double> compute_spectrum() {
            auto spectrum = FFT::dft(signal_);
            std::vector<double> magnitudes;
            
            for (const auto& freq : spectrum) {
                magnitudes.push_back(std::abs(freq));
            }
            
            return magnitudes;
        }
        
        const std::vector<std::complex<double>>& signal() const { return signal_; }
    };
}

// ===== 4. 自定义用户定义字面值 =====

namespace CustomUDL {
    // 温度单位字面值
    namespace Temperature {
        struct Celsius {
            double value;
            explicit constexpr Celsius(double v) : value(v) {}
        };
        
        struct Fahrenheit {
            double value;
            explicit constexpr Fahrenheit(double v) : value(v) {}
        };
        
        struct Kelvin {
            double value;
            explicit constexpr Kelvin(double v) : value(v) {}
        };
        
        // 温度转换
        constexpr Kelvin to_kelvin(Celsius c) {
            return Kelvin(c.value + 273.15);
        }
        
        constexpr Kelvin to_kelvin(Fahrenheit f) {
            return Kelvin((f.value - 32) * 5.0 / 9.0 + 273.15);
        }
        
        constexpr Celsius to_celsius(Kelvin k) {
            return Celsius(k.value - 273.15);
        }
        
        constexpr Fahrenheit to_fahrenheit(Celsius c) {
            return Fahrenheit(c.value * 9.0 / 5.0 + 32);
        }
        
        // 用户定义字面值
        constexpr Celsius operator"" _C(long double value) {
            return Celsius(static_cast<double>(value));
        }
        
        constexpr Fahrenheit operator"" _F(long double value) {
            return Fahrenheit(static_cast<double>(value));
        }
        
        constexpr Kelvin operator"" _K(long double value) {
            return Kelvin(static_cast<double>(value));
        }
        
        // 温度比较
        constexpr bool operator==(Celsius a, Celsius b) {
            return a.value == b.value;
        }
        
        constexpr bool operator<(Celsius a, Celsius b) {
            return a.value < b.value;
        }
        
        constexpr bool operator==(Kelvin a, Kelvin b) {
            return a.value == b.value;
        }
        
        constexpr bool operator<(Kelvin a, Kelvin b) {
            return a.value < b.value;
        }
    }
    
    // 内存大小字面值
    namespace Memory {
        struct Bytes {
            uint64_t value;
            explicit constexpr Bytes(uint64_t v) : value(v) {}
        };
        
        struct Kilobytes {
            uint64_t value;
            explicit constexpr Kilobytes(uint64_t v) : value(v) {}
        };
        
        struct Megabytes {
            uint64_t value;
            explicit constexpr Megabytes(uint64_t v) : value(v) {}
        };
        
        struct Gigabytes {
            uint64_t value;
            explicit constexpr Gigabytes(uint64_t v) : value(v) {}
        };
        
        // 转换为字节
        constexpr Bytes to_bytes(Kilobytes kb) {
            return Bytes(kb.value * 1024);
        }
        
        constexpr Bytes to_bytes(Megabytes mb) {
            return Bytes(mb.value * 1024 * 1024);
        }
        
        constexpr Bytes to_bytes(Gigabytes gb) {
            return Bytes(gb.value * 1024 * 1024 * 1024);
        }
        
        // 用户定义字面值
        constexpr Bytes operator"" _B(unsigned long long value) {
            return Bytes(value);
        }
        
        constexpr Kilobytes operator"" _KB(unsigned long long value) {
            return Kilobytes(value);
        }
        
        constexpr Megabytes operator"" _MB(unsigned long long value) {
            return Megabytes(value);
        }
        
        constexpr Gigabytes operator"" _GB(unsigned long long value) {
            return Gigabytes(value);
        }
        
        // 内存分配器示例
        template<size_t Size>
        class FixedSizeAllocator {
        private:
            std::array<uint8_t, Size> buffer_;
            size_t offset_{0};
            
        public:
            uint8_t* allocate(size_t size) {
                if (offset_ + size > Size) {
                    throw std::bad_alloc();
                }
                auto ptr = &buffer_[offset_];
                offset_ += size;
                return ptr;
            }
            
            void deallocate(uint8_t* ptr, size_t size) {
                // 简单的分配器，不支持单独释放
            }
            
            void reset() {
                offset_ = 0;
            }
            
            size_t used() const { return offset_; }
            size_t available() const { return Size - offset_; }
        };
    }
}

} // namespace cpp14_udl

// ===== 主函数 =====

int main() {
    std::cout << "=== C++14 标准用户定义字面值深度解析 ===\n";
    
    // 1. Chrono字面值演示
    std::cout << "\n===== 1. Chrono字面值演示 =====\n";
    using namespace cpp14_udl::ChronoUDL;
    
    auto work_time = 8h + 30min + 45s + 500ms;
    auto meeting_time = 2h + 15min;
    auto break_time = 15min + 30s;
    
    TimeCalculator total_time = work_time + meeting_time + break_time;
    std::cout << "总工作时间: " << total_time.to_string() << "\n";
    std::cout << "详细分解: " << total_time.detailed_breakdown() << "\n";
    
    // 时间间隔
    TimeInterval meeting_interval(2h);
    TimeInterval lunch_interval(12h, 13h);
    
    std::cout << "会议间隔包含12:30: " 
              << (meeting_interval.contains(std::chrono::system_clock::now() + 2h) ? "是" : "否") << "\n";
    
    // 任务调度演示
    TaskScheduler scheduler;
    scheduler.schedule_after(1s, []() { std::cout << "任务1: 1秒后执行\n"; }, "任务1");
    scheduler.schedule_after(2s, []() { std::cout << "任务2: 2秒后执行\n"; }, "任务2");
    scheduler.schedule_after(3s, []() { std::cout << "任务3: 3秒后执行\n"; }, "任务3");
    
    scheduler.start();
    std::this_thread::sleep_for(4s);
    scheduler.stop();
    
    // 性能分析演示
    Profiler profiler;
    {
        Profiler::ScopeTimer timer(profiler, "计算密集型任务");
        std::this_thread::sleep_for(100ms);
    }
    {
        Profiler::ScopeTimer timer(profiler, "I/O操作");
        std::this_thread::sleep_for(50ms);
    }
    {
        Profiler::ScopeTimer timer(profiler, "计算密集型任务");
        std::this_thread::sleep_for(120ms);
    }
    
    profiler.print_report();
    
    // 2. String字面值演示
    std::cout << "\n===== 2. String字面值演示 =====\n";
    using namespace cpp14_udl::StringUDL;
    
    auto config_content = 
R"(# 应用配置
server.host = localhost
server.port = 8080
debug.mode = true
log.level = info
database.url = postgresql://localhost:5432/mydb
)"s;
    
    ConfigParser parser;
    parser.parse(config_content);
    parser.print_config();
    
    std::cout << "服务器端口: " << parser.get_int("server.port") << "\n";
    std::cout << "调试模式: " << (parser.get_bool("debug.mode") ? "开启" : "关闭") << "\n";
    
    // 字符串处理演示
    auto text = "  Hello, World!  This is a TEST string.  "s;
    auto processed = StringProcessor(text)
        .trim()
        .to_lower()
        .replace("test", "sample")
        .replace("hello", "hi");
    
    std::cout << "处理结果: " << processed.str() << "\n";
    
    // 路径操作演示
    Path base_path("/home/user");
    Path full_path = base_path.join("projects").join("cpp14").join("src");
    
    std::cout << "完整路径: " << full_path.str() << "\n";
    std::cout << "文件名: " << full_path.filename() << "\n";
    std::cout << "扩展名: " << full_path.extension() << "\n";
    std::cout << "主干名: " << full_path.stem() << "\n";
    std::cout << "父目录: " << full_path.parent().str() << "\n";
    
    // 3. Complex字面值演示
    std::cout << "\n===== 3. Complex字面值演示 =====\n";
    using namespace cpp14_udl::ComplexUDL;
    
    auto z1 = 3.0 + 4.0i;
    auto z2 = 1.0 - 2.0i;
    auto z3 = 2.0i;
    auto z4 = 5.0;
    
    ComplexCalculator calc1(z1);
    ComplexCalculator calc2(z2);
    
    std::cout << "z1 = " << calc1.to_string() << "\n";
    std::cout << "z2 = " << calc2.to_string() << "\n";
    std::cout << "z1 + z2 = " << (calc1 + calc2).to_string() << "\n";
    std::cout << "z1 * z2 = " << (calc1 * calc2).to_string() << "\n";
    std::cout << "z1的极坐标: " << calc1.to_polar() << "\n";
    std::cout << "z1的共轭: " << calc1.conjugate().to_string() << "\n";
    
    // 信号处理演示
    std::vector<std::complex<double>> signal = {
        1.0 + 0.0i, 0.5 + 0.5i, 0.0 + 1.0i, -0.5 + 0.5i,
        -1.0 + 0.0i, -0.5 - 0.5i, 0.0 - 1.0i, 0.5 - 0.5i
    };
    
    SignalProcessor processor(signal);
    auto spectrum = processor.compute_spectrum();
    
    std::cout << "信号频谱:\n";
    for (size_t i = 0; i < spectrum.size(); ++i) {
        std::cout << "  频率[" << i << "]: " << spectrum[i] << "\n";
    }
    
    // 4. 自定义字面值演示
    std::cout << "\n===== 4. 自定义字面值演示 =====\n";
    using namespace cpp14_udl::CustomUDL;
    
    // 温度演示
    using namespace Temperature;
    auto room_temp = 22.0_C;
    auto body_temp = 98.6_F;
    auto freezing = 273.15_K;
    
    std::cout << "室温: " << room_temp.value << "°C\n";
    std::cout << "体温: " << body_temp.value << "°F = " 
              << to_celsius(to_kelvin(body_temp)).value << "°C\n";
    std::cout << "冰点: " << freezing.value << "K = " 
              << to_celsius(freezing).value << "°C\n";
    
    std::cout << "室温 < 体温: " << (to_kelvin(room_temp) < to_kelvin(body_temp) ? "是" : "否") << "\n";
    
    // 内存大小演示
    using namespace Memory;
    auto ram_size = 16_GB;
    auto cache_size = 8_MB;
    auto buffer_size = 4_KB;
    
    auto total_bytes = to_bytes(ram_size) + to_bytes(cache_size) + to_bytes(buffer_size);
    std::cout << "总内存: " << total_bytes.value << " 字节\n";
    std::cout << "RAM: " << ram_size.value << "GB = " << to_bytes(ram_size).value << " 字节\n";
    std::cout << "缓存: " << cache_size.value << "MB = " << to_bytes(cache_size).value << " 字节\n";
    std::cout << "缓冲区: " << buffer_size.value << "KB = " << to_bytes(buffer_size).value << " 字节\n";
    
    // 固定大小分配器演示
    FixedSizeAllocator<1024> allocator;
    auto* ptr1 = allocator.allocate(100);
    auto* ptr2 = allocator.allocate(200);
    
    std::cout << "分配器使用: " << allocator.used() << " / 1024 字节\n";
    std::cout << "可用空间: " << allocator.available() << " 字节\n";
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall -pthread 07_user_defined_literals.cpp -o udl_examples
./udl_examples

关键学习点:
1. Chrono字面值提供了直观的时间单位表示，使时间相关的代码更加清晰
2. String字面值避免了std::string构造函数的显式调用，简化了字符串操作
3. Complex字面值提供了复数的直观表示，简化了科学计算和信号处理代码
4. 用户定义字面值通过operator""定义，可以创建特定领域的直观表示
5. 字面值运算符必须在命名空间作用域或全局作用域中定义
6. 字面值运算符的参数类型限制了可用的字面值形式

注意事项:
- 使用标准库字面值需要相应的using声明或using指令
- 用户定义字面值的参数类型有限制：整数、浮点数、字符、字符串
- 字面值运算符必须是constexpr函数，以确保编译期求值
- 自定义字面值应该遵循直观的命名约定，避免混淆
- 字面值运算符应该返回适当的类型，提供良好的类型安全性
*/