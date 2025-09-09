/**
 * C++20 Coroutines异步编程深度解析
 * 
 * 核心概念：
 * 1. 协程三关键字 - co_await、co_yield、co_return的底层机制
 * 2. Promise类型设计 - 协程状态机的控制接口和生命周期管理
 * 3. Awaitable对象 - 自定义等待逻辑和异步操作封装
 * 4. 协程帧管理 - 栈帧转换和内存布局优化
 * 5. 异步编程模式 - Generator、Task、异步I/O的实际应用
 */

#include <iostream>
#include <coroutine>
#include <exception>
#include <thread>
#include <chrono>
#include <future>
#include <queue>
#include <functional>
#include <optional>
#include <vector>
#include <memory>

// ===== 1. 协程三关键字演示 =====
// 简单的Generator实现
template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        
        void unhandled_exception() {
            std::rethrow_exception(std::current_exception());
        }
        
        void return_void() {}
    };
    
private:
    std::coroutine_handle<promise_type> handle_;
    
public:
    explicit Generator(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    
    ~Generator() {
        if (handle_) {
            handle_.destroy();
        }
    }
    
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    
    Generator(Generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = {};
    }
    
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = {};
        }
        return *this;
    }
    
    bool move_next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        return !handle_.done();
    }
    
    T current_value() const {
        return handle_.promise().current_value;
    }
    
    bool done() const {
        return !handle_ || handle_.done();
    }
};

// co_yield示例：斐波那契数列生成器
Generator<int> fibonacci_generator(int count) {
    if (count <= 0) co_return;
    
    int a = 0, b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;  // 暂停并返回当前值
        int temp = a + b;
        a = b;
        b = temp;
    }
}

// co_yield示例：范围生成器
Generator<int> range_generator(int start, int end, int step = 1) {
    for (int i = start; i < end; i += step) {
        co_yield i;
    }
}

void demonstrate_coroutine_keywords() {
    std::cout << "=== 协程三关键字演示 ===\n";
    
    // co_yield演示
    std::cout << "斐波那契数列前10项:\n";
    auto fib_gen = fibonacci_generator(10);
    
    while (fib_gen.move_next()) {
        std::cout << fib_gen.current_value() << " ";
    }
    std::cout << "\n\n";
    
    // 范围生成器
    std::cout << "范围生成器 [0, 20) step=3:\n";
    auto range_gen = range_generator(0, 20, 3);
    
    while (range_gen.move_next()) {
        std::cout << range_gen.current_value() << " ";
    }
    std::cout << "\n\n";
}

// ===== 2. Promise类型设计演示 =====
// 完整的Task实现，展示Promise类型的设计
template<typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> result_;
        std::exception_ptr exception_;
        
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }  // 立即开始执行
        std::suspend_always final_suspend() noexcept { return {}; }  // 在结束时暂停
        
        void return_value(T value) {
            result_ = std::move(value);
        }
        
        void unhandled_exception() {
            exception_ = std::current_exception();
        }
    };
    
private:
    std::coroutine_handle<promise_type> handle_;
    
public:
    explicit Task(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    
    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }
    
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = {};
    }
    
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = {};
        }
        return *this;
    }
    
    bool is_ready() const {
        return !handle_ || handle_.done();
    }
    
    T get_result() {
        if (!handle_) {
            throw std::runtime_error("Task has no coroutine handle");
        }
        
        if (!handle_.done()) {
            throw std::runtime_error("Task is not completed");
        }
        
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }
        
        if (!handle_.promise().result_) {
            throw std::runtime_error("Task has no result");
        }
        
        return std::move(*handle_.promise().result_);
    }
    
    void wait() {
        if (!handle_) return;
        
        while (!handle_.done()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

// co_return示例：异步计算任务
Task<int> compute_async(int x, int y) {
    std::cout << "开始异步计算: " << x << " + " << y << "\n";
    
    // 模拟异步工作
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "异步计算完成\n";
    co_return x + y;  // 返回结果并结束协程
}

// 链式异步操作
Task<std::string> format_result_async(int value) {
    std::cout << "开始格式化结果: " << value << "\n";
    
    // 模拟异步格式化
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    co_return "结果是: " + std::to_string(value);
}

void demonstrate_promise_design() {
    std::cout << "=== Promise类型设计演示 ===\n";
    
    // 创建异步任务
    auto compute_task = compute_async(15, 27);
    
    std::cout << "任务已启动，等待完成...\n";
    compute_task.wait();
    
    try {
        int result = compute_task.get_result();
        std::cout << "计算结果: " << result << "\n";
        
        // 链式处理
        auto format_task = format_result_async(result);
        format_task.wait();
        
        std::string formatted = format_task.get_result();
        std::cout << formatted << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "任务执行失败: " << e.what() << "\n";
    }
    
    std::cout << "\n";
}

// ===== 3. Awaitable对象演示 =====
// 自定义Timer Awaitable
class TimerAwaitable {
private:
    std::chrono::milliseconds duration_;
    
public:
    explicit TimerAwaitable(std::chrono::milliseconds duration) 
        : duration_(duration) {}
    
    bool await_ready() const noexcept {
        return duration_.count() <= 0;  // 如果延迟为0，立即准备好
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        // 在后台线程中等待
        std::thread([duration = duration_, handle]() {
            std::this_thread::sleep_for(duration);
            handle.resume();  // 时间到后恢复协程
        }).detach();
    }
    
    void await_resume() noexcept {
        // 恢复时无需返回值
    }
};

// 便利函数
TimerAwaitable sleep_for(std::chrono::milliseconds duration) {
    return TimerAwaitable{duration};
}

// 自定义Future Awaitable
template<typename T>
class FutureAwaitable {
private:
    std::future<T> future_;
    
public:
    explicit FutureAwaitable(std::future<T> future) 
        : future_(std::move(future)) {}
    
    bool await_ready() const {
        return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        std::thread([future = std::move(future_), handle]() mutable {
            future.wait();  // 等待future完成
            handle.resume();  // 完成后恢复协程
        }).detach();
    }
    
    T await_resume() {
        return future_.get();
    }
};

// 便利函数
template<typename T>
FutureAwaitable<T> await_future(std::future<T> future) {
    return FutureAwaitable<T>{std::move(future)};
}

// Void Task for co_await demonstrations
class VoidTask {
public:
    struct promise_type {
        std::exception_ptr exception_;
        
        VoidTask get_return_object() {
            return VoidTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_void() {}
        
        void unhandled_exception() {
            exception_ = std::current_exception();
        }
    };
    
private:
    std::coroutine_handle<promise_type> handle_;
    
public:
    explicit VoidTask(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    
    ~VoidTask() {
        if (handle_) handle_.destroy();
    }
    
    VoidTask(const VoidTask&) = delete;
    VoidTask& operator=(const VoidTask&) = delete;
    
    VoidTask(VoidTask&& other) noexcept : handle_(other.handle_) {
        other.handle_ = {};
    }
    
    VoidTask& operator=(VoidTask&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = {};
        }
        return *this;
    }
    
    void wait() {
        if (!handle_) return;
        while (!handle_.done()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    bool is_ready() const {
        return !handle_ || handle_.done();
    }
};

// co_await示例：定时任务
VoidTask timed_task() {
    std::cout << "任务开始...\n";
    
    co_await sleep_for(std::chrono::milliseconds(500));
    std::cout << "500ms延迟后继续...\n";
    
    co_await sleep_for(std::chrono::milliseconds(300));
    std::cout << "再300ms延迟后完成\n";
    
    std::cout << "定时任务完成!\n";
}

// co_await示例：等待异步操作
VoidTask async_operation_task() {
    std::cout << "启动异步操作...\n";
    
    // 创建一个异步任务
    auto future = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        return 42;
    });
    
    std::cout << "等待异步操作完成...\n";
    int result = co_await await_future(std::move(future));
    
    std::cout << "异步操作结果: " << result << "\n";
}

void demonstrate_awaitable_objects() {
    std::cout << "=== Awaitable对象演示 ===\n";
    
    // 定时任务演示
    std::cout << "定时任务演示:\n";
    auto timer_task = timed_task();
    timer_task.wait();
    
    std::cout << "\n异步操作任务演示:\n";
    auto async_task = async_operation_task();
    async_task.wait();
    
    std::cout << "\n";
}

// ===== 4. 协程帧管理演示 =====
// 协程状态检查和调试工具
class CoroutineInspector {
public:
    template<typename Promise>
    static void inspect_coroutine(const std::coroutine_handle<Promise>& handle, const std::string& name) {
        std::cout << "协程 '" << name << "' 状态:\n";
        std::cout << "  有效: " << (handle ? "是" : "否") << "\n";
        
        if (handle) {
            std::cout << "  已完成: " << (handle.done() ? "是" : "否") << "\n";
            std::cout << "  地址: " << handle.address() << "\n";
            
            // 协程帧大小估算（仅用于演示）
            std::cout << "  Promise大小: " << sizeof(Promise) << " 字节\n";
        }
        std::cout << "\n";
    }
};

// 带状态跟踪的Generator
template<typename T>
class TrackedGenerator {
public:
    struct promise_type {
        T current_value;
        int yield_count = 0;
        std::string debug_info;
        
        TrackedGenerator get_return_object() {
            return TrackedGenerator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() { 
            debug_info = "协程已初始化";
            return {}; 
        }
        
        std::suspend_always final_suspend() noexcept { 
            debug_info = "协程已完成";
            return {}; 
        }
        
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            ++yield_count;
            debug_info = "第" + std::to_string(yield_count) + "次yield";
            return {};
        }
        
        void unhandled_exception() {
            debug_info = "协程发生异常";
            std::rethrow_exception(std::current_exception());
        }
        
        void return_void() {
            debug_info = "协程正常返回";
        }
    };
    
private:
    std::coroutine_handle<promise_type> handle_;
    
public:
    explicit TrackedGenerator(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    
    ~TrackedGenerator() {
        if (handle_) {
            std::cout << "销毁协程，最终状态: " << handle_.promise().debug_info << "\n";
            handle_.destroy();
        }
    }
    
    TrackedGenerator(const TrackedGenerator&) = delete;
    TrackedGenerator& operator=(const TrackedGenerator&) = delete;
    
    TrackedGenerator(TrackedGenerator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = {};
    }
    
    TrackedGenerator& operator=(TrackedGenerator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = {};
        }
        return *this;
    }
    
    bool move_next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        return !handle_.done();
    }
    
    T current_value() const {
        return handle_.promise().current_value;
    }
    
    std::string debug_info() const {
        return handle_ ? handle_.promise().debug_info : "无效协程";
    }
    
    int yield_count() const {
        return handle_ ? handle_.promise().yield_count : 0;
    }
    
    std::coroutine_handle<promise_type> get_handle() const {
        return handle_;
    }
};

// 状态跟踪的生成器函数
TrackedGenerator<int> tracked_sequence(int start, int count) {
    for (int i = 0; i < count; ++i) {
        co_yield start + i;
    }
}

void demonstrate_coroutine_frame_management() {
    std::cout << "=== 协程帧管理演示 ===\n";
    
    auto gen = tracked_sequence(100, 5);
    
    // 初始状态检查
    CoroutineInspector::inspect_coroutine(gen.get_handle(), "tracked_sequence");
    std::cout << "调试信息: " << gen.debug_info() << "\n\n";
    
    // 逐步执行并检查状态
    int step = 1;
    while (gen.move_next()) {
        std::cout << "步骤 " << step++ << ":\n";
        std::cout << "  当前值: " << gen.current_value() << "\n";
        std::cout << "  调试信息: " << gen.debug_info() << "\n";
        std::cout << "  yield次数: " << gen.yield_count() << "\n";
        
        CoroutineInspector::inspect_coroutine(gen.get_handle(), "tracked_sequence");
    }
    
    std::cout << "最终状态:\n";
    std::cout << "  调试信息: " << gen.debug_info() << "\n";
    CoroutineInspector::inspect_coroutine(gen.get_handle(), "tracked_sequence");
}

// ===== 5. 异步编程模式演示 =====
// 异步文件处理模拟
class AsyncFileProcessor {
public:
    struct FileData {
        std::string filename;
        std::string content;
        size_t size;
    };
    
    // 模拟异步文件读取
    static Task<FileData> read_file_async(const std::string& filename) {
        std::cout << "开始读取文件: " << filename << "\n";
        
        // 模拟I/O延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // 模拟文件内容
        std::string content = "这是文件 " + filename + " 的内容\n包含一些重要数据";
        
        std::cout << "文件读取完成: " << filename << "\n";
        
        co_return FileData{filename, content, content.size()};
    }
    
    // 模拟异步文件处理
    static Task<std::string> process_file_async(const FileData& data) {
        std::cout << "开始处理文件: " << data.filename << "\n";
        
        // 模拟处理延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        
        std::string result = "处理结果: " + data.filename + " (大小: " + std::to_string(data.size) + " 字节)";
        
        std::cout << "文件处理完成: " << data.filename << "\n";
        
        co_return result;
    }
};

// 异步管道处理
VoidTask process_multiple_files() {
    std::vector<std::string> filenames = {"file1.txt", "file2.txt", "file3.txt"};
    
    std::cout << "开始处理多个文件...\n";
    
    for (const auto& filename : filenames) {
        // 异步读取文件
        auto read_task = AsyncFileProcessor::read_file_async(filename);
        read_task.wait();
        
        try {
            auto file_data = read_task.get_result();
            
            // 异步处理文件
            auto process_task = AsyncFileProcessor::process_file_async(file_data);
            process_task.wait();
            
            auto result = process_task.get_result();
            std::cout << "最终结果: " << result << "\n\n";
            
        } catch (const std::exception& e) {
            std::cout << "处理文件 " << filename << " 时出错: " << e.what() << "\n";
        }
    }
    
    std::cout << "所有文件处理完成\n";
}

// 生产者-消费者模式的协程实现
template<typename T>
class AsyncQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;
    
public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!closed_) {
            queue_.push(std::move(item));
            cv_.notify_one();
        }
    }
    
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty() || closed_; });
        
        if (queue_.empty()) {
            return std::nullopt;  // 队列已关闭且为空
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
    
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        cv_.notify_all();
    }
    
    bool is_closed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }
};

// 生产者协程
Generator<int> producer_coroutine(int start, int count) {
    std::cout << "生产者开始生产数据...\n";
    
    for (int i = 0; i < count; ++i) {
        int value = start + i;
        std::cout << "生产: " << value << "\n";
        
        // 模拟生产延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        co_yield value;
    }
    
    std::cout << "生产者完成\n";
}

void demonstrate_async_patterns() {
    std::cout << "=== 异步编程模式演示 ===\n";
    
    // 异步文件处理演示
    std::cout << "异步文件处理演示:\n";
    auto file_task = process_multiple_files();
    file_task.wait();
    
    std::cout << "\n生产者-消费者模式演示:\n";
    
    // 创建异步队列
    AsyncQueue<int> queue;
    
    // 启动生产者
    std::thread producer_thread([&queue]() {
        auto producer = producer_coroutine(1, 5);
        
        while (producer.move_next()) {
            queue.push(producer.current_value());
        }
        
        queue.close();  // 生产完成后关闭队列
    });
    
    // 消费者
    std::thread consumer_thread([&queue]() {
        std::cout << "消费者开始消费数据...\n";
        
        while (true) {
            auto item = queue.pop();
            if (!item) {
                std::cout << "队列已关闭，消费者退出\n";
                break;
            }
            
            std::cout << "消费: " << *item << "\n";
            
            // 模拟消费处理时间
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    });
    
    // 等待线程完成
    producer_thread.join();
    consumer_thread.join();
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 Coroutines异步编程深度解析\n";
    std::cout << "==================================\n";
    
    demonstrate_coroutine_keywords();
    demonstrate_promise_design();
    demonstrate_awaitable_objects();
    demonstrate_coroutine_frame_management();
    demonstrate_async_patterns();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall -pthread 03_coroutines.cpp -o coroutines
./coroutines

关键学习点:
1. 协程三关键字各有不同用途：co_yield暂停返回值，co_await等待异步操作，co_return结束协程
2. promise_type定义了协程的行为和状态管理，是协程机制的核心
3. Awaitable对象封装异步等待逻辑，可以自定义各种异步操作
4. 协程帧在堆上分配，支持挂起和恢复，实现了栈到堆的转换
5. 协程非常适合实现Generator、异步Task、生产者-消费者等模式

注意事项:
- 协程是C++20的实验性特性，需要编译器支持
- 协程帧的生命周期管理很重要，避免悬空引用
- 异常处理需要在promise_type中正确实现
- 协程的性能开销主要在首次创建和销毁时
- 多线程环境下需要注意协程的线程安全性
*/