/**
 * C++11 线程库与并发编程深度解析
 * 
 * 核心概念：
 * 1. std::thread的RAII设计和生命周期管理
 * 2. 原子操作和内存模型的深入理解
 * 3. 互斥量、锁和条件变量的正确使用
 * 4. 线程局部存储和异步任务
 * 5. 线程安全的数据结构设计
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <memory>
#include <functional>

// ===== 1. std::thread基础和RAII设计 =====

void simple_worker_function(int id, int work_amount) {
    std::cout << "线程 " << id << " 开始工作，工作量: " << work_amount << std::endl;
    
    // 模拟工作
    std::this_thread::sleep_for(std::chrono::milliseconds(work_amount * 100));
    
    std::cout << "线程 " << id << " 完成工作" << std::endl;
}

class WorkerWithState {
private:
    int worker_id_;
    std::atomic<bool> should_stop_{false};
    
public:
    explicit WorkerWithState(int id) : worker_id_(id) {}
    
    void operator()() {
        std::cout << "Worker " << worker_id_ << " 启动" << std::endl;
        
        while (!should_stop_.load()) {
            // 模拟工作
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "Worker " << worker_id_ << " 正在工作..." << std::endl;
        }
        
        std::cout << "Worker " << worker_id_ << " 停止" << std::endl;
    }
    
    void stop() {
        should_stop_.store(true);
    }
};

void demonstrate_thread_basics() {
    std::cout << "=== std::thread基础演示 ===\n";
    
    // 1. 使用函数创建线程
    std::thread t1(simple_worker_function, 1, 3);
    
    // 2. 使用lambda创建线程
    std::thread t2([](int id) {
        std::cout << "Lambda线程 " << id << " 执行中..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Lambda线程 " << id << " 完成" << std::endl;
    }, 2);
    
    // 3. 使用函数对象创建线程
    WorkerWithState worker(3);
    std::thread t3(std::ref(worker));  // 注意使用std::ref传递引用
    
    // 让worker运行一会儿然后停止
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    worker.stop();
    
    // 4. 线程管理 - 必须join或detach
    if (t1.joinable()) {
        t1.join();
        std::cout << "线程t1已join" << std::endl;
    }
    
    if (t2.joinable()) {
        t2.join();
        std::cout << "线程t2已join" << std::endl;
    }
    
    if (t3.joinable()) {
        t3.join();
        std::cout << "线程t3已join" << std::endl;
    }
    
    // 5. 获取线程信息
    std::cout << "硬件线程数: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "当前线程ID: " << std::this_thread::get_id() << std::endl;
    
    std::cout << "\n";
}

// ===== 2. 原子操作和内存模型 =====

// 演示不同内存序的效果
class AtomicCounter {
private:
    std::atomic<int> count_{0};
    
public:
    // 宽松内存序 - 最快但最少保证
    void increment_relaxed() {
        count_.fetch_add(1, std::memory_order_relaxed);
    }
    
    // 获取-释放内存序 - 同步相关操作
    void increment_acq_rel() {
        count_.fetch_add(1, std::memory_order_acq_rel);
    }
    
    // 顺序一致内存序 - 最强保证（默认）
    void increment_seq_cst() {
        count_.fetch_add(1);  // 等价于memory_order_seq_cst
    }
    
    int get_relaxed() const {
        return count_.load(std::memory_order_relaxed);
    }
    
    int get_seq_cst() const {
        return count_.load();
    }
    
    void reset() {
        count_.store(0);
    }
};

// 演示ABA问题和compare_exchange
class LockFreeStack {
private:
    struct Node {
        int data;
        Node* next;
        Node(int val) : data(val), next(nullptr) {}
    };
    
    std::atomic<Node*> head_{nullptr};
    
public:
    ~LockFreeStack() {
        while (Node* old_head = head_.load()) {
            head_ = old_head->next;
            delete old_head;
        }
    }
    
    void push(int data) {
        Node* new_node = new Node(data);
        new_node->next = head_.load();
        
        // 使用compare_exchange_weak防止ABA问题
        while (!head_.compare_exchange_weak(new_node->next, new_node)) {
            // 如果失败，new_node->next已被更新为当前head值
            // 继续尝试
        }
    }
    
    bool pop(int& result) {
        Node* old_head = head_.load();
        
        while (old_head && !head_.compare_exchange_weak(old_head, old_head->next)) {
            // 继续尝试，old_head会被更新
        }
        
        if (old_head) {
            result = old_head->data;
            delete old_head;
            return true;
        }
        return false;
    }
    
    bool empty() const {
        return head_.load() == nullptr;
    }
};

void demonstrate_atomic_operations() {
    std::cout << "=== 原子操作和内存模型演示 ===\n";
    
    AtomicCounter counter;
    const int num_threads = 4;
    const int increments_per_thread = 10000;
    
    // 测试不同内存序的性能和正确性
    std::vector<std::thread> threads;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 使用宽松内存序
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&counter, increments_per_thread]() {
            for (int j = 0; j < increments_per_thread; ++j) {
                counter.increment_relaxed();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto relaxed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "宽松内存序结果: " << counter.get_relaxed() 
              << " (预期: " << num_threads * increments_per_thread 
              << "), 时间: " << relaxed_time.count() << "μs" << std::endl;
    
    // 测试无锁栈
    std::cout << "\n无锁栈测试:\n";
    LockFreeStack stack;
    
    // 生产者线程
    std::thread producer([&stack]() {
        for (int i = 0; i < 100; ++i) {
            stack.push(i);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    });
    
    // 消费者线程
    std::thread consumer([&stack]() {
        int value;
        int count = 0;
        while (count < 100) {
            if (stack.pop(value)) {
                std::cout << "弹出: " << value << " ";
                ++count;
                if (count % 20 == 0) std::cout << std::endl;
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    std::cout << "\n栈是否为空: " << (stack.empty() ? "是" : "否") << std::endl;
    
    std::cout << "\n";
}

// ===== 3. 互斥量和锁机制 =====

class ThreadSafeCounter {
private:
    mutable std::mutex mtx_;
    int count_ = 0;
    
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx_);  // RAII锁管理
        ++count_;
    }
    
    void increment_by(int value) {
        std::unique_lock<std::mutex> lock(mtx_);  // 可以提前解锁
        count_ += value;
        // lock在作用域结束时自动解锁
    }
    
    int get() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }
    
    // 尝试锁定，避免阻塞
    bool try_increment() {
        std::unique_lock<std::mutex> lock(mtx_, std::try_to_lock);
        if (lock.owns_lock()) {
            ++count_;
            return true;
        }
        return false;
    }
    
    // 超时锁定
    bool timed_increment(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx_, timeout);
        if (lock.owns_lock()) {
            ++count_;
            return true;
        }
        return false;
    }
};

// 读写锁的简单实现
class ReaderWriterCounter {
private:
    mutable std::shared_mutex rw_mutex_;  // C++17特性，这里用mutex模拟
    mutable std::mutex mtx_;
    int count_ = 0;
    int readers_ = 0;
    
public:
    void write_increment() {
        std::lock_guard<std::mutex> lock(mtx_);
        ++count_;
    }
    
    int read_count() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }
    
    // 模拟读写分离的复杂操作
    void complex_read_operation() const {
        std::lock_guard<std::mutex> lock(mtx_);
        ++const_cast<ReaderWriterCounter*>(this)->readers_;
        
        // 模拟读操作
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        --const_cast<ReaderWriterCounter*>(this)->readers_;
    }
    
    int get_readers() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return readers_;
    }
};

void demonstrate_mutex_and_locks() {
    std::cout << "=== 互斥量和锁机制演示 ===\n";
    
    ThreadSafeCounter safe_counter;
    const int num_threads = 8;
    const int operations_per_thread = 1000;
    
    std::vector<std::thread> threads;
    
    // 测试基本锁定
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&safe_counter, operations_per_thread]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                safe_counter.increment();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "线程安全计数器结果: " << safe_counter.get()
              << " (预期: " << num_threads * operations_per_thread
              << "), 时间: " << duration.count() << "ms" << std::endl;
    
    // 测试尝试锁定
    threads.clear();
    int successful_tries = 0;
    std::mutex try_count_mutex;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&safe_counter, &successful_tries, &try_count_mutex]() {
            for (int j = 0; j < 100; ++j) {
                if (safe_counter.try_increment()) {
                    std::lock_guard<std::mutex> lock(try_count_mutex);
                    ++successful_tries;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "尝试锁定成功次数: " << successful_tries << " / " << num_threads * 100 << std::endl;
    
    // 测试读写分离
    ReaderWriterCounter rw_counter;
    threads.clear();
    
    // 启动多个读线程
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&rw_counter]() {
            for (int j = 0; j < 10; ++j) {
                rw_counter.complex_read_operation();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    // 启动写线程
    threads.emplace_back([&rw_counter]() {
        for (int j = 0; j < 50; ++j) {
            rw_counter.write_increment();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "读写锁测试完成，最终计数: " << rw_counter.read_count() << std::endl;
    
    std::cout << "\n";
}

// ===== 4. 条件变量和同步 =====

class ProducerConsumerQueue {
private:
    std::queue<int> queue_;
    std::mutex mtx_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    const size_t max_size_;
    bool finished_ = false;
    
public:
    explicit ProducerConsumerQueue(size_t max_size) : max_size_(max_size) {}
    
    void produce(int item) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        // 等待直到队列不满
        not_full_.wait(lock, [this] { return queue_.size() < max_size_ || finished_; });
        
        if (!finished_) {
            queue_.push(item);
            std::cout << "生产: " << item << " (队列大小: " << queue_.size() << ")" << std::endl;
        }
        
        not_empty_.notify_one();  // 通知消费者
    }
    
    bool consume(int& item) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        // 等待直到队列不空或者完成
        not_empty_.wait(lock, [this] { return !queue_.empty() || finished_; });
        
        if (!queue_.empty()) {
            item = queue_.front();
            queue_.pop();
            std::cout << "消费: " << item << " (队列大小: " << queue_.size() << ")" << std::endl;
            not_full_.notify_one();  // 通知生产者
            return true;
        }
        
        return false;  // 队列空且已完成
    }
    
    void finish() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            finished_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }
};

// 线程池的简单实现
class SimpleThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
    
public:
    explicit SimpleThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(mtx_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    ~SimpleThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            stop_ = true;
        }
        
        cv_.notify_all();
        
        for (auto& worker : workers_) {
            worker.join();
        }
    }
    
    template<typename F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            if (stop_) {
                throw std::runtime_error("线程池已停止");
            }
            tasks_.emplace(std::forward<F>(task));
        }
        cv_.notify_one();
    }
    
    size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return tasks_.size();
    }
};

void demonstrate_condition_variables() {
    std::cout << "=== 条件变量和同步演示 ===\n";
    
    // 生产者-消费者模式
    ProducerConsumerQueue queue(5);  // 最大容量为5
    
    std::vector<std::thread> threads;
    
    // 启动生产者
    for (int producer_id = 0; producer_id < 2; ++producer_id) {
        threads.emplace_back([&queue, producer_id]() {
            for (int i = 0; i < 10; ++i) {
                int item = producer_id * 100 + i;
                queue.produce(item);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    // 启动消费者
    for (int consumer_id = 0; consumer_id < 3; ++consumer_id) {
        threads.emplace_back([&queue, consumer_id]() {
            int item;
            while (queue.consume(item)) {
                // 模拟处理时间
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::cout << "消费者 " << consumer_id << " 退出" << std::endl;
        });
    }
    
    // 等待生产者完成
    threads[0].join();
    threads[1].join();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    queue.finish();
    
    // 等待消费者完成
    for (size_t i = 2; i < threads.size(); ++i) {
        threads[i].join();
    }
    
    std::cout << "生产者-消费者演示完成\n";
    
    // 线程池演示
    std::cout << "\n线程池演示:\n";
    {
        SimpleThreadPool pool(4);
        
        // 提交一些任务
        for (int i = 0; i < 20; ++i) {
            pool.enqueue([i]() {
                std::cout << "执行任务 " << i << " 在线程 " << std::this_thread::get_id() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            });
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "剩余任务数: " << pool.pending_tasks() << std::endl;
        
        // 线程池析构时会等待所有任务完成
    }
    
    std::cout << "线程池演示完成\n\n";
}

// ===== 5. 异步任务和future =====

// 计算密集型任务
long fibonacci_slow(int n) {
    if (n <= 1) return n;
    return fibonacci_slow(n - 1) + fibonacci_slow(n - 2);
}

// 可取消的异步任务
class CancellableTask {
private:
    std::atomic<bool> cancelled_{false};
    
public:
    void cancel() {
        cancelled_.store(true);
    }
    
    long compute_sum(int start, int end) {
        long sum = 0;
        for (int i = start; i <= end; ++i) {
            if (cancelled_.load()) {
                std::cout << "任务被取消在 i=" << i << std::endl;
                break;
            }
            
            sum += i;
            
            // 模拟一些工作
            if (i % 1000000 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        return sum;
    }
};

void demonstrate_async_and_future() {
    std::cout << "=== 异步任务和future演示 ===\n";
    
    // 1. 基本async使用
    std::cout << "1. 基本async使用:\n";
    
    auto future1 = std::async(std::launch::async, fibonacci_slow, 35);
    auto future2 = std::async(std::launch::async, fibonacci_slow, 36);
    
    std::cout << "异步计算fibonacci(35)和fibonacci(36)...\n";
    
    // 在等待的同时做其他工作
    for (int i = 0; i < 5; ++i) {
        std::cout << "主线程工作中..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    // 获取结果
    std::cout << "fibonacci(35) = " << future1.get() << std::endl;
    std::cout << "fibonacci(36) = " << future2.get() << std::endl;
    
    // 2. 使用promise和future进行线程间通信
    std::cout << "\n2. promise和future通信:\n";
    
    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    
    std::thread worker([&promise]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "工作线程计算完成\n";
        promise.set_value(42);
    });
    
    std::cout << "等待工作线程结果...\n";
    int result = future.get();
    std::cout << "收到结果: " << result << std::endl;
    
    worker.join();
    
    // 3. 超时和状态检查
    std::cout << "\n3. 超时和状态检查:\n";
    
    auto slow_future = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return std::string("延迟的结果");
    });
    
    // 检查状态
    auto status = slow_future.wait_for(std::chrono::milliseconds(100));
    if (status == std::future_status::ready) {
        std::cout << "任务已完成: " << slow_future.get() << std::endl;
    } else if (status == std::future_status::timeout) {
        std::cout << "任务未完成，继续等待...\n";
        std::cout << "最终结果: " << slow_future.get() << std::endl;
    }
    
    // 4. 可取消的任务
    std::cout << "\n4. 可取消的任务:\n";
    
    CancellableTask task;
    auto cancellable_future = std::async(std::launch::async, 
        [&task]() { return task.compute_sum(1, 100000000); });
    
    // 让任务运行一会儿然后取消
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    task.cancel();
    
    long partial_sum = cancellable_future.get();
    std::cout << "部分求和结果: " << partial_sum << std::endl;
    
    std::cout << "\n";
}

// ===== 6. 线程局部存储 =====

thread_local int tls_counter = 0;
thread_local std::unique_ptr<std::string> tls_string;

void initialize_tls() {
    if (!tls_string) {
        tls_string = std::make_unique<std::string>("线程" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())));
    }
}

void demonstrate_thread_local_storage() {
    std::cout << "=== 线程局部存储演示 ===\n";
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i]() {
            initialize_tls();
            
            for (int j = 0; j < 5; ++j) {
                ++tls_counter;
                std::cout << *tls_string << " 计数器: " << tls_counter << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "主线程的tls_counter: " << tls_counter << std::endl;
    
    std::cout << "\n";
}

// ===== 7. 最佳实践和陷阱避免 =====

void demonstrate_best_practices() {
    std::cout << "=== 最佳实践和陷阱避免 ===\n";
    
    std::cout << "并发编程最佳实践:\n";
    std::cout << "1. 优先使用高级同步原语(future, async)而非低级原语\n";
    std::cout << "2. 使用RAII管理锁，避免死锁\n";
    std::cout << "3. 最小化共享数据，优先使用消息传递\n";
    std::cout << "4. 使用原子操作替代简单的互斥量场景\n";
    std::cout << "5. 注意线程的生命周期管理，必须join或detach\n";
    
    std::cout << "\n常见陷阱:\n";
    
    // 陷阱1: 忘记join线程
    std::cout << "陷阱1 - 线程生命周期管理:\n";
    {
        std::thread t([]() {
            std::cout << "短暂的工作线程\n";
        });
        // 如果忘记join或detach，程序会在析构时调用std::terminate
        t.join();  // 正确做法
    }
    
    // 陷阱2: 数据竞争
    std::cout << "\n陷阱2 - 数据竞争检测:\n";
    int unsafe_counter = 0;
    std::atomic<int> safe_counter{0};
    
    std::vector<std::thread> race_threads;
    
    // 不安全的计数器（数据竞争）
    for (int i = 0; i < 2; ++i) {
        race_threads.emplace_back([&unsafe_counter, &safe_counter]() {
            for (int j = 0; j < 1000; ++j) {
                ++unsafe_counter;  // 数据竞争！
                ++safe_counter;    // 原子操作，安全
            }
        });
    }
    
    for (auto& t : race_threads) {
        t.join();
    }
    
    std::cout << "不安全计数器: " << unsafe_counter << " (可能不是2000)" << std::endl;
    std::cout << "安全计数器: " << safe_counter << " (总是2000)" << std::endl;
    
    // 陷阱3: 死锁预防
    std::cout << "\n陷阱3 - 死锁预防示例:\n";
    std::mutex mtx1, mtx2;
    
    auto safe_double_lock = [&mtx1, &mtx2](int id) {
        // 使用std::lock避免死锁
        std::lock(mtx1, mtx2);
        std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
        
        std::cout << "线程 " << id << " 安全获取双锁" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };
    
    std::thread t1(safe_double_lock, 1);
    std::thread t2(safe_double_lock, 2);
    
    t1.join();
    t2.join();
    
    std::cout << "\n性能考虑:\n";
    std::cout << "- 原子操作 > 无锁数据结构 > 细粒度锁 > 粗粒度锁\n";
    std::cout << "- 减少上下文切换和缓存失效\n";
    std::cout << "- 合理选择线程数量（通常等于CPU核心数）\n";
    std::cout << "- 使用线程池避免频繁创建销毁线程\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 线程库与并发编程深度解析\n";
    std::cout << "=================================\n";
    
    // 线程基础
    demonstrate_thread_basics();
    
    // 原子操作
    demonstrate_atomic_operations();
    
    // 互斥量和锁
    demonstrate_mutex_and_locks();
    
    // 条件变量
    demonstrate_condition_variables();
    
    // 异步任务
    demonstrate_async_and_future();
    
    // 线程局部存储
    demonstrate_thread_local_storage();
    
    // 最佳实践
    demonstrate_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -pthread -O2 -Wall 07_threading_concurrency.cpp -o threading_demo
./threading_demo

关键学习点:
1. 理解std::thread的RAII设计和生命周期管理
2. 掌握原子操作和不同内存序的含义
3. 学会正确使用各种锁机制避免竞争条件
4. 掌握条件变量实现线程间同步和通信
5. 理解async/future异步编程模型
6. 了解线程局部存储的使用场景
7. 学会识别和避免常见的并发编程陷阱
8. 掌握现代C++并发编程的最佳实践

注意事项:
- 编译时需要链接pthread库 (-pthread)
- 在多核系统上运行效果更明显
- 输出顺序可能因系统调度而不同
- 某些示例可能需要根据系统调整参数
*/