/**
 * C++11/14/17 完美转发深度解析
 * 
 * 核心概念：
 * 1. 完美转发的设计动机 - 为什么需要完美转发
 * 2. 万能引用与引用折叠 - T&&的双重身份
 * 3. std::forward实现原理 - 条件转发机制
 * 4. 完美转发的应用场景 - 工厂函数与包装器
 * 5. 完美转发的局限性 - 不完美的完美转发
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <type_traits>
#include <utility>

// ===== 1. 完美转发的设计动机演示 =====
void demonstrate_forwarding_motivation() {
    std::cout << "=== 完美转发的设计动机演示 ===\n";
    
    // 传统方法的问题：需要大量重载
    class TraditionalWrapper {
    public:
        // 需要为每种参数组合提供重载
        template<typename T>
        void call_func(T& arg) {
            std::cout << "调用左值版本: " << arg << "\n";
            target_function(arg);
        }
        
        template<typename T>
        void call_func(const T& arg) {
            std::cout << "调用常量左值版本: " << arg << "\n";
            target_function(arg);
        }
        
        template<typename T>
        void call_func(T&& arg) {
            std::cout << "调用右值版本: " << arg << "\n";
            target_function(std::move(arg));
        }
        
    private:
        template<typename T>
        void target_function(T&& arg) {
            std::cout << "目标函数接收到参数\n";
        }
    };
    
    // 完美转发方法：单一模板即可
    class PerfectForwardingWrapper {
    public:
        template<typename T>
        void call_func(T&& arg) {
            std::cout << "完美转发版本处理: ";
            if constexpr (std::is_lvalue_reference_v<T>) {
                std::cout << "左值引用\n";
            } else {
                std::cout << "右值引用\n";
            }
            target_function(std::forward<T>(arg));
        }
        
    private:
        template<typename T>
        void target_function(T&& arg) {
            std::cout << "目标函数接收到参数\n";
        }
    };
    
    PerfectForwardingWrapper wrapper;
    std::string str = "test";
    const std::string const_str = "const_test";
    
    wrapper.call_func(str);                    // 左值
    wrapper.call_func(const_str);              // 常量左值  
    wrapper.call_func(std::string("temp"));    // 右值
    
    std::cout << "\n";
}

// ===== 2. 万能引用与引用折叠演示 =====
void demonstrate_universal_reference() {
    std::cout << "=== 万能引用与引用折叠演示 ===\n";
    
    // 引用折叠规则演示
    auto show_reference_collapsing = []<typename T>() {
        std::cout << "类型T = " << typeid(T).name() << "\n";
        std::cout << "T& & 折叠为: " << std::is_same_v<T& &, T&> << " (T&)\n";
        std::cout << "T& && 折叠为: " << std::is_same_v<T& &&, T&> << " (T&)\n";  
        std::cout << "T&& & 折叠为: " << std::is_same_v<T&& &, T&> << " (T&)\n";
        std::cout << "T&& && 折叠为: " << std::is_same_v<T&& &&, T&&> << " (T&&)\n";
    };
    
    // 万能引用的类型推导
    auto universal_ref_demo = []<typename T>(T&& param) {
        std::cout << "万能引用接收的类型: ";
        if constexpr (std::is_lvalue_reference_v<T>) {
            std::cout << "左值引用 T = " << typeid(T).name() << "\n";
        } else {
            std::cout << "右值引用 T = " << typeid(T).name() << "\n";
        }
        
        // 演示引用折叠的实际应用
        using ForwardedType = T&&;
        std::cout << "T&& 实际类型: " << typeid(ForwardedType).name() << "\n";
    };
    
    int x = 42;
    const int cx = 42;
    
    std::cout << "传入左值 x:\n";
    universal_ref_demo(x);        // T推导为int&，T&&折叠为int&
    
    std::cout << "\n传入常量左值 cx:\n";
    universal_ref_demo(cx);       // T推导为const int&，T&&折叠为const int&
    
    std::cout << "\n传入右值:\n";
    universal_ref_demo(42);       // T推导为int，T&&保持为int&&
    
    std::cout << "\n";
}

// ===== 3. std::forward实现原理演示 =====
void demonstrate_forward_implementation() {
    std::cout << "=== std::forward实现原理演示 ===\n";
    
    // 自定义forward实现
    template<typename T>
    constexpr T&& my_forward(std::remove_reference_t<T>& param) noexcept {
        std::cout << "forward左值版本被调用\n";
        return static_cast<T&&>(param);
    }
    
    template<typename T>
    constexpr T&& my_forward(std::remove_reference_t<T>&& param) noexcept {
        static_assert(!std::is_lvalue_reference_v<T>, "不能将右值转发为左值引用");
        std::cout << "forward右值版本被调用\n";
        return static_cast<T&&>(param);
    }
    
    // 条件转发机制演示
    auto conditional_forward_demo = []<typename T>(T&& param) {
        std::cout << "原始参数类型: " << typeid(T).name() << "\n";
        
        // 使用自定义forward
        if constexpr (std::is_lvalue_reference_v<T>) {
            std::cout << "转发左值引用\n";
            auto&& forwarded = my_forward<T>(param);
            std::cout << "转发后类型: " << typeid(decltype(forwarded)).name() << "\n";
        } else {
            std::cout << "转发右值引用\n";
            auto&& forwarded = my_forward<T>(param);
            std::cout << "转发后类型: " << typeid(decltype(forwarded)).name() << "\n";
        }
    };
    
    std::string str = "test";
    conditional_forward_demo(str);                    // 左值
    conditional_forward_demo(std::string("temp"));    // 右值
    
    std::cout << "\n";
}

// ===== 4. 完美转发的应用场景演示 =====
void demonstrate_forwarding_applications() {
    std::cout << "=== 完美转发的应用场景演示 ===\n";
    
    // 场景1：工厂函数
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique_perfect(Args&&... args) {
        std::cout << "工厂函数创建对象，参数个数: " << sizeof...(args) << "\n";
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
    
    // 场景2：包装器函数
    template<typename Func, typename... Args>
    decltype(auto) wrapper_function(Func&& func, Args&&... args) {
        std::cout << "包装器调用函数，参数个数: " << sizeof...(args) << "\n";
        
        // 完美转发函数和参数
        return std::forward<Func>(func)(std::forward<Args>(args)...);
    }
    
    // 场景3：容器的emplace操作模拟
    template<typename T>
    class Container {
    private:
        std::vector<T> data;
        
    public:
        template<typename... Args>
        void emplace_back_perfect(Args&&... args) {
            std::cout << "就地构造对象，参数个数: " << sizeof...(args) << "\n";
            data.emplace_back(std::forward<Args>(args)...);
        }
        
        void show() const {
            for (const auto& item : data) {
                std::cout << item << " ";
            }
            std::cout << "\n";
        }
    };
    
    // 测试对象
    struct TestObject {
        std::string name;
        int value;
        
        TestObject(std::string n, int v) : name(std::move(n)), value(v) {
            std::cout << "TestObject构造: " << name << ", " << value << "\n";
        }
    };
    
    // 工厂函数测试
    auto obj = make_unique_perfect<TestObject>("factory", 42);
    
    // 包装器函数测试
    auto lambda = [](const std::string& s, int i) -> std::string {
        return s + "_" + std::to_string(i);
    };
    
    std::string str = "wrapper";
    auto result = wrapper_function(lambda, str, 100);
    std::cout << "包装器结果: " << result << "\n";
    
    // 容器emplace测试
    Container<std::string> container;
    container.emplace_back_perfect("direct");
    container.emplace_back_perfect(std::string("moved"));
    container.show();
    
    std::cout << "\n";
}

// ===== 5. 完美转发的局限性演示 =====
void demonstrate_forwarding_limitations() {
    std::cout << "=== 完美转发的局限性演示 ===\n";
    
    // 局限性1：大括号初始化
    std::cout << "1. 大括号初始化问题:\n";
    
    auto target_func = [](std::vector<int> vec) {
        std::cout << "接收到vector，大小: " << vec.size() << "\n";
    };
    
    template<typename Func, typename... Args>
    void forwarder(Func&& func, Args&&... args) {
        std::forward<Func>(func)(std::forward<Args>(args)...);
    }
    
    // 直接调用可以工作
    target_func({1, 2, 3, 4});
    
    // 通过完美转发失败 - 编译错误
    // forwarder(target_func, {1, 2, 3, 4});  // 错误！
    
    // 解决方案：显式类型
    forwarder(target_func, std::vector<int>{1, 2, 3, 4});
    
    // 局限性2：0或NULL作为空指针
    std::cout << "\n2. 空指针问题:\n";
    
    auto null_func = [](int* ptr) {
        if (ptr) {
            std::cout << "接收到非空指针: " << *ptr << "\n";
        } else {
            std::cout << "接收到空指针\n";
        }
    };
    
    // 直接调用
    null_func(0);        // 0被转换为空指针
    null_func(nullptr);  // 明确的空指针
    
    // 通过完美转发
    forwarder(null_func, 0);        // 0被推导为int而非指针！
    forwarder(null_func, nullptr);  // 正确工作
    
    // 局限性3：静态const成员变量
    std::cout << "\n3. 静态const成员问题:\n";
    
    class TestClass {
    public:
        static const int value = 42;  // 只有声明，未定义
    };
    
    auto const_func = [](int val) {
        std::cout << "接收到值: " << val << "\n";
    };
    
    // 直接调用通常可以工作（编译器优化）
    const_func(TestClass::value);
    
    // 通过完美转发可能失败，因为需要TestClass::value的地址
    try {
        forwarder(const_func, TestClass::value);
    } catch (...) {
        std::cout << "可能的链接错误\n";
    }
    
    // 局限性4：重载函数名
    std::cout << "\n4. 重载函数问题:\n";
    
    void overloaded_func(int) { std::cout << "int版本\n"; }
    void overloaded_func(double) { std::cout << "double版本\n"; }
    
    // 无法直接转发重载函数名
    // forwarder(overloaded_func, 42);  // 错误：不知道选择哪个重载
    
    // 解决方案：使用lambda或函数指针
    auto int_version = [](int x) { overloaded_func(x); };
    forwarder(int_version, 42);
    
    std::cout << "\n";
}

// ===== 性能基准测试 =====
void benchmark_perfect_forwarding() {
    std::cout << "=== 完美转发性能基准测试 ===\n";
    
    const int iterations = 1000000;
    
    // 测试对象
    struct TestObj {
        std::string data;
        TestObj(std::string s) : data(std::move(s)) {}
    };
    
    // 传统拷贝方式
    auto copy_approach = [](std::string s) {
        return TestObj(s);
    };
    
    // 完美转发方式
    template<typename T>
    auto forward_approach(T&& s) {
        return TestObj(std::forward<T>(s));
    }
    
    std::string test_str = "benchmark_test_string";
    
    // 基准测试：拷贝方式
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto obj = copy_approach(test_str);
        (void)obj;  // 防止优化
    }
    auto copy_time = std::chrono::high_resolution_clock::now() - start;
    
    // 基准测试：完美转发方式
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto obj = forward_approach(test_str);
        (void)obj;  // 防止优化
    }
    auto forward_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "拷贝方式耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(copy_time).count() 
              << " 微秒\n";
    std::cout << "完美转发耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(forward_time).count() 
              << " 微秒\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++11/14/17 完美转发深度解析\n";
    std::cout << "============================\n";
    
    demonstrate_forwarding_motivation();
    demonstrate_universal_reference();
    demonstrate_forward_implementation();
    demonstrate_forwarding_applications();
    demonstrate_forwarding_limitations();
    benchmark_perfect_forwarding();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 07_perfect_forwarding.cpp -o perfect_forwarding
./perfect_forwarding

关键学习点:
1. 完美转发解决了参数传递中的效率和类型完整性问题
2. 万能引用T&&的双重语义取决于类型推导结果
3. 引用折叠规则是完美转发的理论基础
4. std::forward实现了条件转发，保持参数的值类别
5. 完美转发有局限性，需要了解并规避这些陷阱

注意事项:
- 完美转发只在模板参数推导时有效
- 需要理解左值、右值和值类别的概念
- std::forward必须接收正确的模板参数类型
- 某些语言特性无法被完美转发
*/