/**
 * C++11 智能指针与RAII深度解析
 * 
 * 核心概念：
 * 1. RAII (Resource Acquisition Is Initialization) 设计模式
 * 2. unique_ptr的零开销抽象实现
 * 3. shared_ptr的引用计数机制和控制块设计
 * 4. weak_ptr解决循环引用问题
 * 5. 自定义删除器的应用
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <fstream>

// ===== 1. RAII原理演示 =====

// 传统C风格资源管理的问题
void demonstrate_raw_pointer_problems() {
    std::cout << "=== 原始指针资源管理问题演示 ===\n";
    
    // 问题1: 忘记释放内存
    int* raw_ptr = new int(42);
    std::cout << "分配内存: " << *raw_ptr << std::endl;
    
    // 如果这里有异常抛出，内存泄露！
    try {
        if (*raw_ptr > 40) {
            throw std::runtime_error("模拟异常");
        }
        delete raw_ptr;  // 正常情况下的释放
    } catch (...) {
        std::cout << "异常发生，可能导致内存泄露！\n";
        delete raw_ptr;  // 需要在每个异常处理分支都记得释放
    }
    
    // 问题2: 双重释放
    int* another_ptr = new int(100);
    int* copy_ptr = another_ptr;  // 浅拷贝
    
    delete another_ptr;  // 第一次释放
    // delete copy_ptr;  // 双重释放，未定义行为！
    
    std::cout << "原始指针管理复杂且容易出错\n\n";
}

// RAII资源管理包装器
template<typename T>
class SimpleRAII {
private:
    T* resource;
    
public:
    explicit SimpleRAII(T* ptr) : resource(ptr) {
        std::cout << "RAII获取资源: " << (void*)resource << std::endl;
    }
    
    ~SimpleRAII() {
        if (resource) {
            delete resource;
            std::cout << "RAII自动释放资源: " << (void*)resource << std::endl;
        }
    }
    
    // 禁止拷贝构造和拷贝赋值
    SimpleRAII(const SimpleRAII&) = delete;
    SimpleRAII& operator=(const SimpleRAII&) = delete;
    
    // 支持移动语义
    SimpleRAII(SimpleRAII&& other) noexcept : resource(other.resource) {
        other.resource = nullptr;
        std::cout << "RAII移动构造\n";
    }
    
    SimpleRAII& operator=(SimpleRAII&& other) noexcept {
        if (this != &other) {
            delete resource;
            resource = other.resource;
            other.resource = nullptr;
            std::cout << "RAII移动赋值\n";
        }
        return *this;
    }
    
    T& operator*() { return *resource; }
    T* operator->() { return resource; }
    T* get() const { return resource; }
    
    T* release() {
        T* temp = resource;
        resource = nullptr;
        return temp;
    }
};

void demonstrate_raii_principle() {
    std::cout << "=== RAII原理演示 ===\n";
    
    try {
        SimpleRAII<int> raii_ptr(new int(42));
        std::cout << "使用RAII管理的资源: " << *raii_ptr << std::endl;
        
        if (*raii_ptr > 40) {
            throw std::runtime_error("模拟异常");
        }
        
    } catch (...) {
        std::cout << "异常发生，RAII自动清理资源！\n";
    }
    
    std::cout << "RAII确保异常安全和自动资源管理\n\n";
}

// ===== 2. unique_ptr深度解析 =====

class Resource {
public:
    int id;
    std::string name;
    
    Resource(int id, const std::string& name) : id(id), name(name) {
        std::cout << "Resource " << id << " (" << name << ") 构造\n";
    }
    
    ~Resource() {
        std::cout << "Resource " << id << " (" << name << ") 析构\n";
    }
    
    void do_work() {
        std::cout << "Resource " << id << " 正在工作...\n";
    }
};

// 自定义删除器演示
struct ArrayDeleter {
    void operator()(Resource* ptr) {
        std::cout << "使用自定义数组删除器\n";
        delete[] ptr;
    }
};

struct FileDeleter {
    void operator()(std::FILE* file) {
        if (file) {
            std::cout << "自动关闭文件\n";
            std::fclose(file);
        }
    }
};

void demonstrate_unique_ptr() {
    std::cout << "=== unique_ptr深度解析 ===\n";
    
    // 基础用法
    std::unique_ptr<Resource> ptr1(new Resource(1, "BasicUnique"));
    ptr1->do_work();
    
    // make_unique (C++14特性，这里手动实现演示)
    auto make_unique_resource = [](int id, const std::string& name) {
        return std::unique_ptr<Resource>(new Resource(id, name));
    };
    
    auto ptr2 = make_unique_resource(2, "MakeUnique");
    
    // 移动语义
    std::cout << "\n--- unique_ptr移动语义 ---\n";
    std::unique_ptr<Resource> ptr3 = std::move(ptr1);  // ptr1变为nullptr
    std::cout << "ptr1 after move: " << (ptr1.get() ? "有效" : "nullptr") << std::endl;
    std::cout << "ptr3 after move: " << (ptr3.get() ? "有效" : "nullptr") << std::endl;
    
    // 自定义删除器
    std::cout << "\n--- 自定义删除器 ---\n";
    
    // 数组删除器
    std::unique_ptr<Resource[], ArrayDeleter> array_ptr(new Resource[2]{
        Resource(3, "Array1"),
        Resource(4, "Array2")
    });
    
    // 文件删除器（lambda版本）
    auto file_deleter = [](std::FILE* file) {
        if (file) {
            std::cout << "Lambda文件删除器\n";
            std::fclose(file);
        }
    };
    
    std::unique_ptr<std::FILE, decltype(file_deleter)> file_ptr(
        std::fopen("temp.txt", "w"), file_deleter);
    
    if (file_ptr) {
        std::fprintf(file_ptr.get(), "Hello RAII!\n");
    }
    
    // 工厂模式应用
    std::cout << "\n--- 工厂模式应用 ---\n";
    auto resource_factory = [](int id, const std::string& type) -> std::unique_ptr<Resource> {
        if (type == "standard") {
            return std::unique_ptr<Resource>(new Resource(id, "Standard_" + std::to_string(id)));
        } else {
            return std::unique_ptr<Resource>(new Resource(id, "Special_" + std::to_string(id)));
        }
    };
    
    auto factory_ptr = resource_factory(5, "standard");
    factory_ptr->do_work();
    
    std::cout << "\n";
}

// ===== 3. shared_ptr引用计数机制 =====

// 简化版shared_ptr实现（仅演示原理）
template<typename T>
class SimpleSharedPtr {
private:
    T* ptr;
    std::atomic<int>* ref_count;
    
    void increment() {
        if (ref_count) {
            ++(*ref_count);
        }
    }
    
    void decrement() {
        if (ref_count && --(*ref_count) == 0) {
            delete ptr;
            delete ref_count;
            std::cout << "SimpleSharedPtr: 引用计数归零，释放资源\n";
        }
    }
    
public:
    explicit SimpleSharedPtr(T* p = nullptr) 
        : ptr(p), ref_count(p ? new std::atomic<int>(1) : nullptr) {
        std::cout << "SimpleSharedPtr构造，引用计数: " << use_count() << std::endl;
    }
    
    SimpleSharedPtr(const SimpleSharedPtr& other) 
        : ptr(other.ptr), ref_count(other.ref_count) {
        increment();
        std::cout << "SimpleSharedPtr拷贝构造，引用计数: " << use_count() << std::endl;
    }
    
    SimpleSharedPtr& operator=(const SimpleSharedPtr& other) {
        if (this != &other) {
            decrement();  // 减少当前的引用计数
            ptr = other.ptr;
            ref_count = other.ref_count;
            increment();  // 增加新的引用计数
            std::cout << "SimpleSharedPtr赋值，引用计数: " << use_count() << std::endl;
        }
        return *this;
    }
    
    ~SimpleSharedPtr() {
        decrement();
    }
    
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
    T* get() const { return ptr; }
    
    int use_count() const {
        return ref_count ? ref_count->load() : 0;
    }
    
    bool unique() const {
        return use_count() == 1;
    }
};

void demonstrate_shared_ptr_mechanism() {
    std::cout << "=== shared_ptr引用计数机制演示 ===\n";
    
    // 简化版演示
    std::cout << "--- 简化版shared_ptr ---\n";
    {
        SimpleSharedPtr<Resource> sptr1(new Resource(6, "SharedSimple"));
        {
            SimpleSharedPtr<Resource> sptr2 = sptr1;  // 引用计数增加
            SimpleSharedPtr<Resource> sptr3;
            sptr3 = sptr1;  // 引用计数再增加
            
            std::cout << "当前引用计数: " << sptr1.use_count() << std::endl;
        }  // sptr2和sptr3销毁，引用计数减少
        std::cout << "退出内层作用域后引用计数: " << sptr1.use_count() << std::endl;
    }  // sptr1销毁，引用计数归零，资源释放
    
    // 标准shared_ptr演示
    std::cout << "\n--- 标准shared_ptr ---\n";
    std::shared_ptr<Resource> std_ptr1 = std::make_shared<Resource>(7, "StdShared");
    std::cout << "make_shared后引用计数: " << std_ptr1.use_count() << std::endl;
    
    {
        std::shared_ptr<Resource> std_ptr2 = std_ptr1;
        std::shared_ptr<Resource> std_ptr3(std_ptr1);
        
        std::cout << "多个共享后引用计数: " << std_ptr1.use_count() << std::endl;
        std::cout << "是否唯一: " << (std_ptr1.unique() ? "是" : "否") << std::endl;
        
        // reset操作
        std_ptr2.reset();
        std::cout << "reset后引用计数: " << std_ptr1.use_count() << std::endl;
        
        // 重新赋值
        std_ptr3 = std::make_shared<Resource>(8, "NewShared");
        std::cout << "重新赋值后引用计数: " << std_ptr1.use_count() << std::endl;
    }
    
    std::cout << "退出作用域后引用计数: " << std_ptr1.use_count() << std::endl;
    std::cout << "\n";
}

// ===== 4. weak_ptr解决循环引用 =====

struct Node {
    int data;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> parent;  // 使用weak_ptr避免循环引用
    
    Node(int d) : data(d) {
        std::cout << "Node " << data << " 构造\n";
    }
    
    ~Node() {
        std::cout << "Node " << data << " 析构\n";
    }
    
    void set_parent(std::shared_ptr<Node> p) {
        parent = p;  // weak_ptr不增加引用计数
    }
    
    std::shared_ptr<Node> get_parent() {
        return parent.lock();  // 安全地获取shared_ptr
    }
};

void demonstrate_circular_reference_problem() {
    std::cout << "=== 循环引用问题演示 ===\n";
    
    // 创建循环引用的问题场景
    {
        std::shared_ptr<Node> node1 = std::make_shared<Node>(1);
        std::shared_ptr<Node> node2 = std::make_shared<Node>(2);
        
        // 创建双向链接（如果都用shared_ptr会造成循环引用）
        node1->next = node2;
        node2->set_parent(node1);  // 使用weak_ptr，避免循环引用
        
        std::cout << "node1 引用计数: " << node1.use_count() << std::endl;  // 应该是2
        std::cout << "node2 引用计数: " << node2.use_count() << std::endl;  // 应该是1
        
        // 测试weak_ptr的有效性
        if (auto parent = node2->get_parent()) {
            std::cout << "node2的父节点数据: " << parent->data << std::endl;
        } else {
            std::cout << "node2的父节点已失效\n";
        }
    }  // 所有节点应该正确析构
    
    // 演示weak_ptr的失效检测
    std::cout << "\n--- weak_ptr失效检测 ---\n";
    std::weak_ptr<Resource> weak_res;
    
    {
        auto shared_res = std::make_shared<Resource>(9, "WeakTest");
        weak_res = shared_res;
        
        std::cout << "weak_ptr有效性: " << (!weak_res.expired() ? "有效" : "失效") << std::endl;
        std::cout << "weak_ptr引用计数: " << weak_res.use_count() << std::endl;
        
        if (auto locked = weak_res.lock()) {
            locked->do_work();
        }
    }  // shared_res销毁
    
    std::cout << "共享对象销毁后:\n";
    std::cout << "weak_ptr有效性: " << (!weak_res.expired() ? "有效" : "失效") << std::endl;
    std::cout << "weak_ptr引用计数: " << weak_res.use_count() << std::endl;
    
    if (auto locked = weak_res.lock()) {
        std::cout << "意外：weak_ptr仍然有效\n";
    } else {
        std::cout << "正确：weak_ptr已失效，无法锁定\n";
    }
    
    std::cout << "\n";
}

// ===== 5. 性能对比分析 =====

void performance_comparison() {
    std::cout << "=== 智能指针性能对比 ===\n";
    
    const int iterations = 1000000;
    
    // 测试原始指针性能
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::vector<Resource*> raw_ptrs;
        for (int i = 0; i < iterations; ++i) {
            raw_ptrs.push_back(new Resource(i, "Raw"));
        }
        for (auto ptr : raw_ptrs) {
            delete ptr;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto raw_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 测试unique_ptr性能
    start = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::unique_ptr<Resource>> unique_ptrs;
        for (int i = 0; i < iterations; ++i) {
            unique_ptrs.push_back(std::unique_ptr<Resource>(new Resource(i, "Unique")));
        }
    }  // 自动析构
    end = std::chrono::high_resolution_clock::now();
    auto unique_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 测试shared_ptr性能
    start = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::shared_ptr<Resource>> shared_ptrs;
        for (int i = 0; i < iterations; ++i) {
            shared_ptrs.push_back(std::make_shared<Resource>(i, "Shared"));
        }
    }  // 自动析构
    end = std::chrono::high_resolution_clock::now();
    auto shared_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "性能测试结果 (" << iterations << " 次操作):\n";
    std::cout << "原始指针时间: " << raw_time.count() << "ms\n";
    std::cout << "unique_ptr时间: " << unique_time.count() << "ms (开销: " 
              << (static_cast<double>(unique_time.count()) / raw_time.count()) << "x)\n";
    std::cout << "shared_ptr时间: " << shared_time.count() << "ms (开销: "
              << (static_cast<double>(shared_time.count()) / raw_time.count()) << "x)\n";
    
    // 内存占用对比
    std::cout << "\n内存占用对比:\n";
    std::cout << "原始指针大小: " << sizeof(Resource*) << " bytes\n";
    std::cout << "unique_ptr大小: " << sizeof(std::unique_ptr<Resource>) << " bytes\n";
    std::cout << "shared_ptr大小: " << sizeof(std::shared_ptr<Resource>) << " bytes\n";
    
    std::cout << "\n";
}

// ===== 6. 线程安全性演示 =====

void demonstrate_thread_safety() {
    std::cout << "=== 智能指针线程安全性演示 ===\n";
    
    auto shared_resource = std::make_shared<Resource>(10, "ThreadSafe");
    
    // shared_ptr的拷贝和析构是线程安全的
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([shared_resource, i]() {
            // 每个线程都拷贝shared_ptr（线程安全）
            auto local_copy = shared_resource;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            local_copy->do_work();
            std::cout << "线程 " << i << " 引用计数: " << local_copy.use_count() << std::endl;
            
            // local_copy在线程结束时自动销毁（线程安全）
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "所有线程完成后引用计数: " << shared_resource.use_count() << std::endl;
    std::cout << "\n";
}

// ===== 7. 实际应用场景 =====

// 观察者模式实现
class Subject {
private:
    std::vector<std::weak_ptr<class Observer>> observers;
    
public:
    void attach(std::weak_ptr<Observer> observer) {
        observers.push_back(observer);
    }
    
    void notify(const std::string& message) {
        std::cout << "主题通知: " << message << std::endl;
        
        // 清理失效的观察者
        auto it = observers.begin();
        while (it != observers.end()) {
            if (auto obs = it->lock()) {
                obs->update(message);
                ++it;
            } else {
                it = observers.erase(it);  // 移除失效的观察者
            }
        }
    }
    
    size_t observer_count() const { return observers.size(); }
};

class Observer {
private:
    std::string name;
    
public:
    Observer(const std::string& n) : name(n) {
        std::cout << "观察者 " << name << " 创建\n";
    }
    
    ~Observer() {
        std::cout << "观察者 " << name << " 销毁\n";
    }
    
    void update(const std::string& message) {
        std::cout << "观察者 " << name << " 收到消息: " << message << std::endl;
    }
};

void demonstrate_observer_pattern() {
    std::cout << "=== 观察者模式应用 ===\n";
    
    Subject subject;
    
    {
        auto obs1 = std::make_shared<Observer>("观察者1");
        auto obs2 = std::make_shared<Observer>("观察者2");
        
        subject.attach(obs1);
        subject.attach(obs2);
        
        subject.notify("第一条消息");
        std::cout << "当前观察者数量: " << subject.observer_count() << std::endl;
        
        // obs2销毁
        obs2.reset();
        
        subject.notify("第二条消息");  // 自动清理失效的观察者
        std::cout << "清理后观察者数量: " << subject.observer_count() << std::endl;
    }
    
    subject.notify("第三条消息");  // 所有观察者都已失效
    std::cout << "最终观察者数量: " << subject.observer_count() << std::endl;
    
    std::cout << "\n";
}

// ===== 8. 最佳实践总结 =====

void demonstrate_best_practices() {
    std::cout << "=== 智能指针最佳实践 ===\n";
    
    std::cout << "1. 优先使用 make_shared 和 make_unique\n";
    std::cout << "   - 异常安全\n";
    std::cout << "   - 更好的内存布局（make_shared）\n";
    std::cout << "   - 避免显式new\n\n";
    
    std::cout << "2. 选择合适的智能指针类型:\n";
    std::cout << "   - unique_ptr: 独占所有权，零开销\n";
    std::cout << "   - shared_ptr: 共享所有权，有引用计数开销\n";
    std::cout << "   - weak_ptr: 观察但不拥有，解决循环引用\n\n";
    
    std::cout << "3. 避免常见陷阱:\n";
    std::cout << "   - 不要用同一个原始指针创建多个智能指针\n";
    std::cout << "   - 注意循环引用问题\n";
    std::cout << "   - 小心this指针的使用\n\n";
    
    std::cout << "4. 性能考虑:\n";
    std::cout << "   - unique_ptr几乎零开销\n";
    std::cout << "   - shared_ptr有原子操作开销\n";
    std::cout << "   - 优先传递引用而非智能指针\n\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 智能指针与RAII深度解析\n";
    std::cout << "===============================\n";
    
    // 原始指针问题
    demonstrate_raw_pointer_problems();
    
    // RAII原理
    demonstrate_raii_principle();
    
    // unique_ptr详解
    demonstrate_unique_ptr();
    
    // shared_ptr机制
    demonstrate_shared_ptr_mechanism();
    
    // 循环引用问题
    demonstrate_circular_reference_problem();
    
    // 性能对比
    performance_comparison();
    
    // 线程安全
    demonstrate_thread_safety();
    
    // 观察者模式应用
    demonstrate_observer_pattern();
    
    // 最佳实践
    demonstrate_best_practices();
    
    std::cout << "程序结束 - 观察智能指针的自动清理\n";
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -pthread -O2 -Wall 03_smart_pointers_raii.cpp -o smart_pointers
./smart_pointers

关键学习点:
1. 理解RAII设计模式的核心价值
2. 掌握unique_ptr的零开销抽象特性
3. 深入理解shared_ptr的引用计数机制
4. 学会使用weak_ptr解决循环引用
5. 了解不同智能指针的性能特征
6. 掌握智能指针在设计模式中的应用
7. 养成使用智能指针的最佳实践习惯
*/