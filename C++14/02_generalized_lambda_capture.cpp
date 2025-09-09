/**
 * C++14 广义Lambda捕获深度解析
 * 
 * 核心概念：
 * 1. 初始化捕获的语法机制和编译器实现
 * 2. 移动捕获解决C++11的unique_ptr限制
 * 3. 表达式捕获的高级应用和性能优化
 * 4. 捕获this指针的安全模式和陷阱
 * 5. 闭包类型的内存布局和生命周期管理
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <atomic>

// ===== 1. C++11 Lambda捕获的限制回顾 =====

void demonstrate_cpp11_capture_limitations() {
    std::cout << "=== C++11 Lambda捕获限制回顾 ===\n";
    
    // C++11只支持按值和按引用捕获
    int x = 42;
    std::string str = "hello";
    
    // 基本捕获方式
    auto lambda1 = [x](){ return x * 2; };              // 按值捕获
    auto lambda2 = [&str](){ return str + " world"; };  // 按引用捕获
    auto lambda3 = [=](){ return x + str.length(); };   // 全部按值
    auto lambda4 = [&](){ str += "!"; return str; };    // 全部按引用
    
    std::cout << "按值捕获结果: " << lambda1() << std::endl;
    std::cout << "按引用捕获结果: " << lambda2() << std::endl;
    std::cout << "全部按值捕获: " << lambda3() << std::endl;
    std::cout << "全部按引用捕获: " << lambda4() << std::endl;
    
    // C++11的主要限制
    std::cout << "\nC++11 Lambda捕获的限制:\n";
    std::cout << "1. 无法移动捕获unique_ptr等只移动类型\n";
    std::cout << "2. 无法捕获表达式的结果\n";
    std::cout << "3. 无法进行计算性捕获\n";
    std::cout << "4. 无法在捕获时进行类型转换\n";
    
    // 展示C++11的workaround
    auto unique_ptr = std::make_unique<int>(100);
    std::cout << "unique_ptr原始值: " << *unique_ptr << std::endl;
    
    // C++11无法直接捕获unique_ptr，需要通过引用
    auto lambda_ref = [&unique_ptr]() { 
        return unique_ptr ? *unique_ptr * 2 : 0; 
    };
    std::cout << "通过引用捕获unique_ptr: " << lambda_ref() << std::endl;
    
    std::cout << "\n";
}

// ===== 2. C++14 初始化捕获机制 =====

void demonstrate_initialization_capture() {
    std::cout << "=== C++14 初始化捕获机制 ===\n";
    
    // 基本初始化捕获语法：[变量名 = 初始化表达式]
    int x = 10;
    auto lambda1 = [y = x * 2](){ 
        return y + 5; 
    };
    std::cout << "初始化捕获: " << lambda1() << std::endl;
    
    // 复杂表达式捕获
    std::vector<int> vec{1, 2, 3, 4, 5};
    auto lambda2 = [sum = std::accumulate(vec.begin(), vec.end(), 0)](int multiplier){
        return sum * multiplier;
    };
    std::cout << "表达式捕获结果: " << lambda2(3) << std::endl;
    
    // 类型转换捕获
    double pi = 3.14159;
    auto lambda3 = [int_pi = static_cast<int>(pi)](){ 
        return int_pi; 
    };
    std::cout << "类型转换捕获: " << lambda3() << std::endl;
    
    // 函数调用结果捕获
    auto get_current_time = []() {
        return std::chrono::high_resolution_clock::now();
    };
    
    auto lambda4 = [start_time = get_current_time()](){ 
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            now - start_time);
        return duration.count();
    };
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "时间间隔捕获: " << lambda4() << "μs" << std::endl;
    
    // 编译器生成的闭包类等价实现
    class EquivalentClosure {
    private:
        int y_;  // 初始化捕获的成员
        
    public:
        EquivalentClosure(int init_y) : y_(init_y) {}
        
        auto operator()() const {
            return y_ + 5;
        }
    };
    
    EquivalentClosure equivalent_closure(x * 2);
    std::cout << "等价闭包类结果: " << equivalent_closure() << std::endl;
    
    std::cout << "\n初始化捕获的特点:\n";
    std::cout << "1. 在闭包对象构造时执行初始化表达式\n";
    std::cout << "2. 捕获的变量成为闭包类的成员\n";
    std::cout << "3. 支持任意复杂的初始化表达式\n";
    std::cout << "4. 可以进行类型推导和转换\n";
    
    std::cout << "\n";
}

// ===== 3. 移动捕获的深度应用 =====

// 资源管理类用于演示移动语义
class ExpensiveResource {
private:
    std::vector<int> data_;
    std::string name_;
    
public:
    explicit ExpensiveResource(const std::string& name, size_t size = 1000) 
        : name_(name), data_(size) {
        std::iota(data_.begin(), data_.end(), 1);
        std::cout << "构造资源: " << name_ << " (大小: " << size << ")\n";
    }
    
    // 移动构造函数
    ExpensiveResource(ExpensiveResource&& other) noexcept
        : data_(std::move(other.data_)), name_(std::move(other.name_)) {
        std::cout << "移动构造资源: " << name_ << "\n";
    }
    
    // 移动赋值运算符
    ExpensiveResource& operator=(ExpensiveResource&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            name_ = std::move(other.name_);
            std::cout << "移动赋值资源: " << name_ << "\n";
        }
        return *this;
    }
    
    // 禁用拷贝
    ExpensiveResource(const ExpensiveResource&) = delete;
    ExpensiveResource& operator=(const ExpensiveResource&) = delete;
    
    ~ExpensiveResource() {
        std::cout << "析构资源: " << name_ << "\n";
    }
    
    size_t size() const { return data_.size(); }
    const std::string& name() const { return name_; }
    int sum() const { return std::accumulate(data_.begin(), data_.end(), 0); }
};

void demonstrate_move_capture() {
    std::cout << "=== 移动捕获深度应用 ===\n";
    
    // unique_ptr的移动捕获
    auto unique_resource = std::make_unique<ExpensiveResource>("UniqueResource", 500);
    std::cout << "unique_ptr初始值: " << unique_resource->sum() << std::endl;
    
    // C++14移动捕获语法
    auto lambda_move_unique = [ptr = std::move(unique_resource)](int multiplier) {
        return ptr ? ptr->sum() * multiplier : 0;
    };
    
    // 原指针现在为空
    std::cout << "移动后unique_ptr状态: " << (unique_resource ? "非空" : "空") << std::endl;
    std::cout << "移动捕获Lambda结果: " << lambda_move_unique(2) << std::endl;
    
    // 直接移动对象捕获
    ExpensiveResource resource("DirectMove", 300);
    std::cout << "移动前资源名: " << resource.name() << std::endl;
    
    auto lambda_move_object = [res = std::move(resource)](){ 
        return res.name() + "_processed";
    };
    
    std::cout << "移动后原对象名: " << resource.name() << " (可能为空)" << std::endl;
    std::cout << "Lambda中的对象: " << lambda_move_object() << std::endl;
    
    // shared_ptr的高效传递
    auto shared_resource = std::make_shared<ExpensiveResource>("SharedResource", 200);
    std::cout << "shared_ptr引用计数: " << shared_resource.use_count() << std::endl;
    
    auto lambda_shared = [shared = shared_resource](){ 
        return shared->name() + "_shared";
    };
    
    std::cout << "Lambda创建后引用计数: " << shared_resource.use_count() << std::endl;
    std::cout << "共享捕获结果: " << lambda_shared() << std::endl;
    
    // 移动捕获在异步编程中的应用
    auto async_task = [data = std::make_unique<std::vector<int>>(1000, 42)]() -> std::future<int> {
        return std::async(std::launch::async, [data = std::move(data)](){
            return std::accumulate(data->begin(), data->end(), 0);
        });
    };
    
    auto future_result = async_task();
    std::cout << "异步移动捕获结果: " << future_result.get() << std::endl;
    
    std::cout << "\n移动捕获的优势:\n";
    std::cout << "1. 解决了unique_ptr等只移动类型的捕获问题\n";
    std::cout << "2. 避免不必要的拷贝，提高性能\n";
    std::cout << "3. 支持资源的转移和生命周期管理\n";
    std::cout << "4. 在异步编程中特别有用\n";
    
    std::cout << "\n";
}

// ===== 4. 表达式捕获的高级模式 =====

void demonstrate_expression_capture_patterns() {
    std::cout << "=== 表达式捕获的高级模式 ===\n";
    
    // 计算性捕获
    std::vector<double> prices{10.5, 25.3, 8.7, 15.2, 30.1};
    auto calculator = [
        total = std::accumulate(prices.begin(), prices.end(), 0.0),
        count = prices.size(),
        max_price = *std::max_element(prices.begin(), prices.end()),
        min_price = *std::min_element(prices.begin(), prices.end())
    ](const std::string& operation) {
        if (operation == "average") return total / count;
        if (operation == "max") return max_price;
        if (operation == "min") return min_price;
        if (operation == "total") return total;
        return 0.0;
    };
    
    std::cout << "价格统计:\n";
    std::cout << "  平均价格: " << calculator("average") << std::endl;
    std::cout << "  最高价格: " << calculator("max") << std::endl;
    std::cout << "  最低价格: " << calculator("min") << std::endl;
    std::cout << "  总价: " << calculator("total") << std::endl;
    
    // 函数对象的捕获
    auto custom_comparator = [](const std::string& a, const std::string& b) {
        return a.length() < b.length();
    };
    
    std::vector<std::string> words{"hello", "world", "cpp", "lambda", "capture"};
    auto sorter = [
        comp = custom_comparator,
        original_size = words.size()
    ](std::vector<std::string> input) mutable {
        std::sort(input.begin(), input.end(), comp);
        std::cout << "排序了" << original_size << "个单词\n";
        return input;
    };
    
    auto sorted_words = sorter(words);
    std::cout << "按长度排序结果: ";
    for (const auto& word : sorted_words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    
    // 复杂对象的部分捕获
    struct Configuration {
        std::string host = "localhost";
        int port = 8080;
        bool ssl_enabled = false;
        std::vector<std::string> allowed_ips{"127.0.0.1", "192.168.1.1"};
    };
    
    Configuration config;
    auto connection_checker = [
        endpoint = config.host + ":" + std::to_string(config.port),
        secure = config.ssl_enabled,
        ip_count = config.allowed_ips.size()
    ](const std::string& client_ip) {
        std::cout << "检查连接 " << client_ip << " -> " << endpoint;
        std::cout << (secure ? " (SSL)" : " (Plain)");
        std::cout << ", 允许IP数: " << ip_count << std::endl;
        return true;  // 简化实现
    };
    
    connection_checker("192.168.1.100");
    
    // 状态机的实现
    enum class State { Idle, Running, Paused, Stopped };
    
    auto state_machine = [
        current_state = State::Idle,
        transition_count = 0
    ](State new_state) mutable {
        auto state_to_string = [](State s) {
            switch (s) {
                case State::Idle: return "Idle";
                case State::Running: return "Running";
                case State::Paused: return "Paused";
                case State::Stopped: return "Stopped";
            }
            return "Unknown";
        };
        
        std::cout << "状态转换: " << state_to_string(current_state) 
                  << " -> " << state_to_string(new_state)
                  << " (第" << ++transition_count << "次)" << std::endl;
        current_state = new_state;
    };
    
    state_machine(State::Running);
    state_machine(State::Paused);
    state_machine(State::Stopped);
    
    std::cout << "\n表达式捕获模式:\n";
    std::cout << "1. 计算性捕获：预计算结果存储在闭包中\n";
    std::cout << "2. 函数对象捕获：捕获复杂的行为逻辑\n";
    std::cout << "3. 部分对象捕获：只捕获对象的部分成员\n";
    std::cout << "4. 状态捕获：实现有状态的函数对象\n";
    
    std::cout << "\n";
}

// ===== 5. this指针捕获的安全模式 =====

class DataProcessor {
private:
    std::string name_;
    std::vector<int> data_;
    mutable std::atomic<int> process_count_{0};
    
public:
    explicit DataProcessor(const std::string& name) : name_(name) {
        data_.resize(1000);
        std::iota(data_.begin(), data_.end(), 1);
    }
    
    // 危险的this捕获
    std::function<int()> create_unsafe_processor() {
        return [this]() {  // 危险：捕获this指针
            ++process_count_;
            return std::accumulate(data_.begin(), data_.end(), 0);
        };
    }
    
    // 安全的按值捕获成员
    std::function<int()> create_safe_processor() {
        return [name = name_, data = data_]() {  // 安全：拷贝成员数据
            std::cout << "安全处理器 " << name << " 处理数据\n";
            return std::accumulate(data.begin(), data.end(), 0);
        };
    }
    
    // 混合捕获：部分按值，部分按引用
    std::function<int()> create_hybrid_processor() {
        return [name = name_, &counter = process_count_](const std::vector<int>& external_data) {
            ++counter;
            std::cout << "混合处理器 " << name << " 处理外部数据\n";
            return std::accumulate(external_data.begin(), external_data.end(), 0);
        };
    }
    
    // 智能指针捕获this
    std::function<int()> create_shared_processor(std::shared_ptr<DataProcessor> self) {
        return [self, name = name_]() {
            self->process_count_++;
            return std::accumulate(self->data_.begin(), self->data_.end(), 0);
        };
    }
    
    int get_process_count() const { return process_count_; }
    const std::string& name() const { return name_; }
};

void demonstrate_this_capture_safety() {
    std::cout << "=== this指针捕获的安全模式 ===\n";
    
    std::function<int()> unsafe_func;
    std::function<int()> safe_func;
    
    {
        DataProcessor processor("TestProcessor");
        
        // 创建不同类型的处理器
        unsafe_func = processor.create_unsafe_processor();  // 危险
        safe_func = processor.create_safe_processor();      // 安全
        
        std::cout << "对象存在时调用:\n";
        std::cout << "不安全处理器结果: " << unsafe_func() << std::endl;
        std::cout << "安全处理器结果: " << safe_func() << std::endl;
        
    }  // processor在这里被销毁
    
    std::cout << "\n对象销毁后调用:\n";
    // unsafe_func();  // 未定义行为！悬空this指针
    std::cout << "安全处理器仍可调用: " << safe_func() << std::endl;
    
    // 使用shared_ptr的安全模式
    auto shared_processor = std::make_shared<DataProcessor>("SharedProcessor");
    auto shared_func = shared_processor->create_shared_processor(shared_processor);
    
    std::cout << "\n使用shared_ptr的安全模式:\n";
    std::cout << "shared_ptr引用计数: " << shared_processor.use_count() << std::endl;
    std::cout << "共享处理器结果: " << shared_func() << std::endl;
    
    shared_processor.reset();  // 释放原始引用
    std::cout << "原始shared_ptr释放后，Lambda仍可安全调用: " << shared_func() << std::endl;
    
    // 成员函数中的安全捕获模式
    class SafeCapture {
    private:
        std::string data_;
        
    public:
        explicit SafeCapture(const std::string& data) : data_(data) {}
        
        auto create_lambda() {
            // 好的做法：显式捕获需要的成员
            return [data = data_](const std::string& suffix) {
                return data + suffix;
            };
        }
        
        auto create_lambda_with_validation() {
            // 更好的做法：添加验证和错误处理
            return [data = data_, validation = !data_.empty()](const std::string& suffix) {
                if (!validation) {
                    return std::string("Invalid data");
                }
                return data + suffix;
            };
        }
    };
    
    SafeCapture safe_obj("Hello");
    auto safe_lambda = safe_obj.create_lambda();
    auto validated_lambda = safe_obj.create_lambda_with_validation();
    
    std::cout << "\n成员安全捕获:\n";
    std::cout << "安全Lambda: " << safe_lambda(" World") << std::endl;
    std::cout << "验证Lambda: " << validated_lambda(" C++14") << std::endl;
    
    std::cout << "\nthis捕获安全指南:\n";
    std::cout << "✓ 优先使用按值捕获具体成员\n";
    std::cout << "✓ 使用shared_ptr延长对象生命周期\n";
    std::cout << "✓ 避免在成员函数中捕获裸this指针\n";
    std::cout << "✓ 添加生命周期验证和错误处理\n";
    std::cout << "✗ 不要在对象可能提前销毁时捕获this\n";
    
    std::cout << "\n";
}

// ===== 6. 闭包类型的内存布局分析 =====

void demonstrate_closure_memory_layout() {
    std::cout << "=== 闭包类型的内存布局分析 ===\n";
    
    // 空Lambda（无捕获）
    auto empty_lambda = [](){};
    std::cout << "空Lambda大小: " << sizeof(empty_lambda) << " bytes\n";
    
    // 单个int捕获
    int x = 42;
    auto int_capture = [x](){};
    std::cout << "捕获int的Lambda大小: " << sizeof(int_capture) << " bytes\n";
    
    // 多个基本类型捕获
    double y = 3.14;
    char z = 'A';
    auto multi_capture = [x, y, z](){};
    std::cout << "捕获int+double+char的Lambda大小: " << sizeof(multi_capture) << " bytes\n";
    
    // 捕获大对象
    std::vector<int> large_vec(1000, 1);
    auto large_capture = [large_vec](){};
    std::cout << "捕获大vector的Lambda大小: " << sizeof(large_capture) << " bytes\n";
    
    // 引用捕获（只存储引用）
    auto ref_capture = [&large_vec](){};
    std::cout << "引用捕获大vector的Lambda大小: " << sizeof(ref_capture) << " bytes\n";
    
    // 移动捕获unique_ptr
    auto unique_ptr = std::make_unique<std::vector<int>>(1000, 2);
    auto move_capture = [ptr = std::move(unique_ptr)](){};
    std::cout << "移动捕获unique_ptr的Lambda大小: " << sizeof(move_capture) << " bytes\n";
    
    // 复杂初始化捕获
    auto complex_capture = [
        computed_value = x * y,
        string_result = std::to_string(x) + std::to_string(y),
        current_time = std::chrono::high_resolution_clock::now()
    ](){};
    std::cout << "复杂初始化捕获的Lambda大小: " << sizeof(complex_capture) << " bytes\n";
    
    // 内存对齐的影响
    struct AlignedData {
        char a;
        double b;
        char c;
    };
    
    AlignedData aligned{};
    auto aligned_capture = [aligned](){};
    std::cout << "对齐结构体大小: " << sizeof(AlignedData) << " bytes\n";
    std::cout << "捕获对齐结构的Lambda大小: " << sizeof(aligned_capture) << " bytes\n";
    
    // 分析内存布局优化
    std::cout << "\n内存布局优化分析:\n";
    
    // 按大小优化捕获顺序
    char small = 'S';
    int medium = 42;
    double large = 3.14159;
    
    auto unoptimized = [small, medium, large](){};  // 可能有内存对齐空洞
    auto optimized = [large, medium, small](){};    // 更好的对齐
    
    std::cout << "未优化顺序Lambda大小: " << sizeof(unoptimized) << " bytes\n";
    std::cout << "优化顺序Lambda大小: " << sizeof(optimized) << " bytes\n";
    
    // 智能指针vs原始指针的内存影响
    int* raw_ptr = &x;
    std::shared_ptr<int> shared_ptr = std::make_shared<int>(42);
    std::unique_ptr<int> unique_ptr_local = std::make_unique<int>(42);
    
    auto raw_capture = [raw_ptr](){};
    auto shared_capture = [shared_ptr](){};
    auto unique_capture = [ptr = std::move(unique_ptr_local)](){};
    
    std::cout << "\n指针类型对内存的影响:\n";
    std::cout << "原始指针捕获大小: " << sizeof(raw_capture) << " bytes\n";
    std::cout << "shared_ptr捕获大小: " << sizeof(shared_capture) << " bytes\n";
    std::cout << "unique_ptr捕获大小: " << sizeof(unique_capture) << " bytes\n";
    
    std::cout << "\n内存优化建议:\n";
    std::cout << "1. 避免捕获大对象，优先使用引用或智能指针\n";
    std::cout << "2. 注意成员对齐对Lambda大小的影响\n";
    std::cout << "3. 移动捕获比拷贝捕获更节省内存\n";
    std::cout << "4. 空Lambda占用最小空间（通常1字节）\n";
    std::cout << "5. 引用捕获只存储引用，不受对象大小影响\n";
    
    std::cout << "\n";
}

// ===== 7. 性能分析和最佳实践 =====

class PerformanceBenchmark {
public:
    template<typename Func>
    static auto measure_lambda_creation(Func&& factory, int iterations = 1000000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            volatile auto lambda = factory();
            (void)lambda;  // 防止优化掉
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }
    
    template<typename Lambda>
    static auto measure_lambda_execution(Lambda&& lambda, int iterations = 1000000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            volatile auto result = lambda();
            (void)result;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }
};

void demonstrate_performance_best_practices() {
    std::cout << "=== 性能分析和最佳实践 ===\n";
    
    std::vector<int> data(1000, 42);
    
    // 对比不同捕获方式的性能
    std::cout << "Lambda创建性能测试 (1M次):\n";
    
    // 按值捕获大对象
    auto value_capture_factory = [&data]() {
        return [data](){ return data.size(); };  // 每次都拷贝vector
    };
    
    // 引用捕获
    auto ref_capture_factory = [&data]() {
        return [&data](){ return data.size(); };  // 只存储引用
    };
    
    // 移动捕获（模拟）
    auto move_capture_factory = [&data]() {
        return [size = data.size()](){ return size; };  // 只捕获需要的值
    };
    
    auto value_time = PerformanceBenchmark::measure_lambda_creation(value_capture_factory);
    auto ref_time = PerformanceBenchmark::measure_lambda_creation(ref_capture_factory);
    auto move_time = PerformanceBenchmark::measure_lambda_creation(move_capture_factory);
    
    std::cout << "按值捕获创建时间: " << value_time.count() << " ns\n";
    std::cout << "引用捕获创建时间: " << ref_time.count() << " ns\n";
    std::cout << "优化捕获创建时间: " << move_time.count() << " ns\n";
    
    // Lambda执行性能测试
    std::cout << "\nLambda执行性能测试 (1M次):\n";
    
    auto value_lambda = [data](){ return std::accumulate(data.begin(), data.end(), 0); };
    auto ref_lambda = [&data](){ return std::accumulate(data.begin(), data.end(), 0); };
    
    auto value_exec_time = PerformanceBenchmark::measure_lambda_execution(value_lambda);
    auto ref_exec_time = PerformanceBenchmark::measure_lambda_execution(ref_lambda);
    
    std::cout << "按值捕获执行时间: " << value_exec_time.count() << " ns\n";
    std::cout << "引用捕获执行时间: " << ref_exec_time.count() << " ns\n";
    
    // 复杂初始化捕获的开销
    std::cout << "\n初始化捕获开销分析:\n";
    
    auto expensive_init_factory = []() {
        return [
            expensive_calc = std::accumulate(std::vector<int>(1000, 1).begin(), 
                                           std::vector<int>(1000, 1).end(), 0),
            timestamp = std::chrono::high_resolution_clock::now()
        ](){
            return expensive_calc;
        };
    };
    
    auto simple_factory = []() {
        return [](){ return 42000; };
    };
    
    auto expensive_time = PerformanceBenchmark::measure_lambda_creation(expensive_init_factory, 10000);
    auto simple_time = PerformanceBenchmark::measure_lambda_creation(simple_factory, 10000);
    
    std::cout << "复杂初始化捕获时间: " << expensive_time.count() << " ns\n";
    std::cout << "简单Lambda创建时间: " << simple_time.count() << " ns\n";
    
    std::cout << "\n最佳实践总结:\n";
    std::cout << "✓ 优先使用移动捕获减少拷贝开销\n";
    std::cout << "✓ 大对象使用引用捕获或智能指针\n";
    std::cout << "✓ 避免在初始化捕获中进行昂贵计算\n";
    std::cout << "✓ 只捕获Lambda中实际使用的数据\n";
    std::cout << "✓ 考虑Lambda的生命周期和调用频率\n";
    
    std::cout << "\n性能优化策略:\n";
    std::cout << "1. 捕获策略：移动 > 引用 > 按值\n";
    std::cout << "2. 减少闭包对象大小以提高缓存局部性\n";
    std::cout << "3. 避免不必要的智能指针开销\n";
    std::cout << "4. 在热路径中缓存Lambda对象\n";
    std::cout << "5. 使用编译器优化标志提高性能\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++14 广义Lambda捕获深度解析\n";
    std::cout << "=============================\n";
    
    // C++11限制回顾
    demonstrate_cpp11_capture_limitations();
    
    // 初始化捕获机制
    demonstrate_initialization_capture();
    
    // 移动捕获应用
    demonstrate_move_capture();
    
    // 表达式捕获模式
    demonstrate_expression_capture_patterns();
    
    // this捕获安全
    demonstrate_this_capture_safety();
    
    // 内存布局分析
    demonstrate_closure_memory_layout();
    
    // 性能最佳实践
    demonstrate_performance_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall 02_generalized_lambda_capture.cpp -o lambda_capture_demo
./lambda_capture_demo

关键学习点:
1. 掌握C++14初始化捕获的语法和编译器实现机制
2. 理解移动捕获如何解决C++11的unique_ptr捕获限制
3. 学会使用表达式捕获实现复杂的计算和状态管理
4. 掌握this指针捕获的安全模式和常见陷阱
5. 了解闭包类型的内存布局对性能的影响
6. 学会分析和优化Lambda捕获的性能开销
7. 掌握不同捕获策略在实际项目中的选择标准
8. 理解广义捕获在现代C++异步编程中的重要作用

注意事项:
- 移动捕获会改变原对象状态，需要谨慎处理生命周期
- 表达式捕获在Lambda创建时执行，注意计算开销
- this指针捕获容易产生悬空引用，优先使用成员值捕获
- 大对象捕获会影响Lambda的内存占用和创建性能
*/