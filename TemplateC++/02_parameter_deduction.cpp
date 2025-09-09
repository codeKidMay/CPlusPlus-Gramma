/**
 * 第2章：参数推导的算法原理深度解析
 * 
 * 核心概念：
 * 1. 推导规则 - 函数模板参数推导的核心算法
 * 2. 引用折叠 - T&& 在不同上下文中的含义
 * 3. 完美转发 - 通用引用和std::forward的机制
 * 4. CTAD机制 - 类模板参数推导的工作原理
 * 5. 推导限制 - 非推导上下文和推导失败处理
 */

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <memory>
#include <functional>
#include <tuple>

// ===== 1. 函数模板参数推导规则 =====
void demonstrate_basic_deduction_rules() {
    std::cout << "=== 函数模板参数推导规则 ===\n";
    
    // 基础推导规则演示
    template<typename T>
    void basic_deduction(T param) {
        std::cout << "basic_deduction: T = " << typeid(T).name() 
                  << ", param type = " << typeid(param).name() << "\n";
    }
    
    template<typename T>
    void reference_deduction(T& param) {
        std::cout << "reference_deduction: T = " << typeid(T).name() 
                  << ", param type = " << typeid(param).name() << "\n";
    }
    
    template<typename T>
    void const_reference_deduction(const T& param) {
        std::cout << "const_reference_deduction: T = " << typeid(T).name() 
                  << ", param type = " << typeid(param).name() << "\n";
    }
    
    template<typename T>
    void universal_reference_deduction(T&& param) {
        std::cout << "universal_reference_deduction: T = " << typeid(T).name() 
                  << ", param type = " << typeid(param).name() << "\n";
        std::cout << "  is_lvalue_reference: " << std::is_lvalue_reference_v<T> << "\n";
        std::cout << "  is_rvalue_reference: " << std::is_rvalue_reference_v<T> << "\n";
    }
    
    // 测试不同类型的推导
    int x = 42;
    const int cx = x;
    const int& rx = x;
    
    std::cout << "\n--- 传值推导 (T param) ---\n";
    basic_deduction(x);    // T = int
    basic_deduction(cx);   // T = int (const被忽略)
    basic_deduction(rx);   // T = int (const和&都被忽略)
    
    std::cout << "\n--- 引用推导 (T& param) ---\n";
    reference_deduction(x);    // T = int, param = int&
    reference_deduction(cx);   // T = const int, param = const int&
    reference_deduction(rx);   // T = const int, param = const int&
    
    std::cout << "\n--- const引用推导 (const T& param) ---\n";
    const_reference_deduction(x);    // T = int, param = const int&
    const_reference_deduction(cx);   // T = int, param = const int&
    const_reference_deduction(rx);   // T = int, param = const int&
    const_reference_deduction(27);   // T = int, param = const int& (右值也可以)
    
    std::cout << "\n--- 通用引用推导 (T&& param) ---\n";
    universal_reference_deduction(x);     // T = int&, param = int& (引用折叠)
    universal_reference_deduction(cx);    // T = const int&, param = const int&
    universal_reference_deduction(std::move(x)); // T = int, param = int&&
    universal_reference_deduction(27);    // T = int, param = int&&
    
    std::cout << "\n";
}

// ===== 2. 引用折叠规则深度解析 =====
void demonstrate_reference_collapsing() {
    std::cout << "=== 引用折叠规则深度解析 ===\n";
    
    // 引用折叠规则：
    // T& + & -> T&
    // T& + && -> T&  
    // T&& + & -> T&
    // T&& + && -> T&&
    
    template<typename T>
    struct ReferenceCollapsingDemo {
        using lvalue_ref = T&;
        using rvalue_ref = T&&;
        
        static void show_types() {
            std::cout << "Original type T: " << typeid(T).name() << "\n";
            std::cout << "T& type: " << typeid(lvalue_ref).name() << "\n";
            std::cout << "T&& type: " << typeid(rvalue_ref).name() << "\n";
            
            std::cout << "T& is_lvalue_reference: " << std::is_lvalue_reference_v<lvalue_ref> << "\n";
            std::cout << "T&& is_rvalue_reference: " << std::is_rvalue_reference_v<rvalue_ref> << "\n";
        }
    };
    
    std::cout << "\n--- T = int 的情况 ---\n";
    ReferenceCollapsingDemo<int>::show_types();
    
    std::cout << "\n--- T = int& 的情况 (引用折叠) ---\n";
    ReferenceCollapsingDemo<int&>::show_types();
    
    std::cout << "\n--- T = int&& 的情况 (引用折叠) ---\n";
    ReferenceCollapsingDemo<int&&>::show_types();
    
    // 实际应用：完美转发的实现原理
    template<typename T>
    constexpr T&& my_forward(std::remove_reference_t<T>& param) noexcept {
        return static_cast<T&&>(param);
    }
    
    template<typename T>
    constexpr T&& my_forward(std::remove_reference_t<T>&& param) noexcept {
        static_assert(!std::is_lvalue_reference_v<T>, 
                     "Cannot forward an rvalue as an lvalue");
        return static_cast<T&&>(param);
    }
    
    std::cout << "\n--- 完美转发机制演示 ---\n";
    
    auto test_forwarding = [](auto&& param) {
        std::cout << "Forwarding type: " << typeid(decltype(param)).name() << "\n";
        return my_forward<decltype(param)>(param);
    };
    
    int value = 42;
    auto&& result1 = test_forwarding(value);        // 左值转发
    auto&& result2 = test_forwarding(std::move(value)); // 右值转发
    
    std::cout << "\n";
}

// ===== 3. auto推导 vs 模板推导 =====
void demonstrate_auto_vs_template_deduction() {
    std::cout << "=== auto推导 vs 模板推导 ===\n";
    
    // auto推导基本等同于模板推导，但有一些特殊情况
    
    template<typename T>
    void template_func(T param) {
        std::cout << "Template: T = " << typeid(T).name() << "\n";
    }
    
    // 对比auto和模板推导
    auto auto_var1 = 27;           // auto = int
    auto auto_var2 = "hello";      // auto = const char*
    auto auto_var3 = {1, 2, 3};    // auto = std::initializer_list<int>
    
    template_func(27);             // T = int
    template_func("hello");        // T = const char*
    // template_func({1, 2, 3});   // 编译错误！模板无法推导initializer_list
    
    std::cout << "auto_var1 type: " << typeid(auto_var1).name() << "\n";
    std::cout << "auto_var2 type: " << typeid(auto_var2).name() << "\n";
    std::cout << "auto_var3 type: " << typeid(auto_var3).name() << "\n";
    
    // auto和引用的组合
    int x = 42;
    const int cx = x;
    const int& rx = x;
    
    auto auto_x = x;      // auto = int
    auto auto_cx = cx;    // auto = int (const被忽略)
    auto auto_rx = rx;    // auto = int (const和&都被忽略)
    
    auto& auto_ref_x = x;   // auto = int, auto_ref_x = int&
    auto& auto_ref_cx = cx; // auto = const int, auto_ref_cx = const int&
    auto& auto_ref_rx = rx; // auto = const int, auto_ref_rx = const int&
    
    const auto& auto_cref_x = x; // auto = int, auto_cref_x = const int&
    
    auto&& auto_uref_x = x;           // auto = int&, auto_uref_x = int&
    auto&& auto_uref_move = std::move(x); // auto = int, auto_uref_move = int&&
    
    std::cout << "\n--- auto推导结果 ---\n";
    std::cout << "auto_x is const: " << std::is_const_v<decltype(auto_x)> << "\n";
    std::cout << "auto_cx is const: " << std::is_const_v<decltype(auto_cx)> << "\n";
    std::cout << "auto_ref_cx is const ref: " << std::is_const_v<std::remove_reference_t<decltype(auto_ref_cx)>> << "\n";
    
    std::cout << "\n";
}

// ===== 4. 类模板参数推导(CTAD) =====
void demonstrate_ctad() {
    std::cout << "=== 类模板参数推导(CTAD) ===\n";
    
    // 自定义类演示CTAD
    template<typename T, typename U>
    class MyPair {
        T first;
        U second;
        
    public:
        MyPair(const T& f, const U& s) : first(f), second(s) {}
        
        template<typename T2, typename U2>
        MyPair(T2&& f, U2&& s) : first(std::forward<T2>(f)), second(std::forward<U2>(s)) {}
        
        void print() const {
            std::cout << "MyPair(" << first << ", " << second << ")\n";
        }
    };
    
    // CTAD工作示例
    MyPair p1{1, 2.5};           // MyPair<int, double>
    MyPair p2{"hello", 42};      // MyPair<const char*, int>
    MyPair p3{std::string{"world"}, std::vector<int>{1,2,3}}; // 复杂类型推导
    
    p1.print();
    p2.print();
    
    // 推导指引(Deduction Guides)
    template<typename T>
    class MyContainer {
        std::vector<T> data;
        
    public:
        MyContainer(std::initializer_list<T> il) : data(il) {}
        
        template<typename Iter>
        MyContainer(Iter first, Iter last) : data(first, last) {}
        
        void print() const {
            std::cout << "MyContainer: ";
            for (const auto& item : data) {
                std::cout << item << " ";
            }
            std::cout << "\n";
        }
    };
    
    // 自定义推导指引
    template<typename Iter>
    MyContainer(Iter, Iter) -> MyContainer<typename std::iterator_traits<Iter>::value_type>;
    
    MyContainer c1{1, 2, 3, 4, 5};  // 通过initializer_list推导
    c1.print();
    
    std::vector<double> vec{1.1, 2.2, 3.3};
    MyContainer c2{vec.begin(), vec.end()};  // 通过推导指引推导
    c2.print();
    
    // 标准库中的CTAD示例
    std::vector v1{1, 2, 3, 4};      // std::vector<int>
    std::pair p4{42, 3.14};          // std::pair<int, double>
    std::tuple t1{1, "hello", 3.14}; // std::tuple<int, const char*, double>
    
    std::cout << "Standard library CTAD works perfectly!\n";
    
    std::cout << "\n";
}

// ===== 5. 推导上下文和非推导上下文 =====
void demonstrate_deduction_contexts() {
    std::cout << "=== 推导上下文和非推导上下文 ===\n";
    
    // 推导上下文：编译器能够从参数推导模板参数的情况
    template<typename T>
    void deducible_context(T param) {
        std::cout << "Deduced T: " << typeid(T).name() << "\n";
    }
    
    template<typename T>
    void deducible_context_ptr(T* param) {
        std::cout << "Deduced T from pointer: " << typeid(T).name() << "\n";
    }
    
    template<typename T>
    void deducible_context_ref(const T& param) {
        std::cout << "Deduced T from const ref: " << typeid(T).name() << "\n";
    }
    
    // 非推导上下文：编译器无法推导的情况
    template<typename T>
    struct NonDeducibleDemo {
        using type = T;
    };
    
    template<typename T>
    void non_deducible_context(typename NonDeducibleDemo<T>::type param) {
        std::cout << "Non-deducible context, T must be explicitly specified\n";
    }
    
    template<typename T>
    void non_deducible_return() -> typename NonDeducibleDemo<T>::type {
        std::cout << "Non-deducible return type\n";
        return T{};
    }
    
    // 推导成功的例子
    deducible_context(42);           // T = int
    deducible_context_ptr(&main);    // T = int()
    deducible_context_ref(std::string{"hello"}); // T = std::string
    
    // 推导失败，需要显式指定
    // non_deducible_context(42);      // 编译错误
    non_deducible_context<int>(42);    // 正确：显式指定T = int
    
    // 使用auto处理非推导上下文
    auto result = non_deducible_return<std::string>();
    
    std::cout << "\n--- 复杂推导情况 ---\n";
    
    // 函数指针的推导
    template<typename Ret, typename... Args>
    void function_ptr_deduction(Ret(*func)(Args...)) {
        std::cout << "Function pointer deduced: " 
                  << "return type size = " << sizeof(Ret)
                  << ", arg count = " << sizeof...(Args) << "\n";
    }
    
    function_ptr_deduction(static_cast<int(*)(int, double)>(nullptr));
    
    // 成员函数指针推导
    template<typename Class, typename Ret, typename... Args>
    void member_func_deduction(Ret(Class::*)(Args...)) {
        std::cout << "Member function deduced for class size: " << sizeof(Class) << "\n";
    }
    
    struct TestClass {
        int member_func(double) { return 0; }
    };
    
    member_func_deduction(&TestClass::member_func);
    
    std::cout << "\n";
}

// ===== 6. 推导失败和SFINAE的协作 =====
void demonstrate_deduction_failure_and_sfinae() {
    std::cout << "=== 推导失败和SFINAE的协作 ===\n";
    
    // 推导失败不是错误，可以用于SFINAE
    template<typename T>
    auto has_size_method(T&& t) -> decltype(t.size(), std::true_type{}) {
        std::cout << "Type has size() method\n";
        return {};
    }
    
    template<typename T>
    std::false_type has_size_method(...) {
        std::cout << "Type does not have size() method\n";
        return {};
    }
    
    // 更现代的方法：使用void_t
    template<typename T, typename = void>
    struct has_size : std::false_type {};
    
    template<typename T>
    struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
    
    // 测试不同类型
    std::vector<int> vec{1, 2, 3};
    int number = 42;
    
    std::cout << "\n--- 检测size()方法 ---\n";
    has_size_method(vec);     // 有size方法
    has_size_method(number);  // 没有size方法
    
    std::cout << "vector has_size: " << has_size<std::vector<int>>::value << "\n";
    std::cout << "int has_size: " << has_size<int>::value << "\n";
    
    // 基于推导结果的条件编译
    template<typename Container>
    auto get_size_if_available(Container&& c) {
        if constexpr (has_size<std::decay_t<Container>>::value) {
            return c.size();
        } else {
            return size_t{1}; // 默认大小
        }
    }
    
    std::cout << "\n--- 条件性大小获取 ---\n";
    std::cout << "Vector size: " << get_size_if_available(vec) << "\n";
    std::cout << "Int 'size': " << get_size_if_available(number) << "\n";
    
    std::cout << "\n";
}

// ===== 7. 高级推导技巧和性能考虑 =====
void demonstrate_advanced_deduction_techniques() {
    std::cout << "=== 高级推导技巧和性能考虑 ===\n";
    
    // 推导技巧1：利用推导避免显式类型声明
    template<typename Func, typename... Args>
    decltype(auto) invoke_and_time(Func&& func, Args&&... args) {
        auto start = std::chrono::steady_clock::now();
        
        // 完美转发并保持返回类型
        if constexpr (std::is_void_v<std::invoke_result_t<Func, Args...>>) {
            std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
            auto end = std::chrono::steady_clock::now();
            std::cout << "Void function executed in: " 
                      << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
                      << " microseconds\n";
        } else {
            auto result = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
            auto end = std::chrono::steady_clock::now();
            std::cout << "Function executed in: " 
                      << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
                      << " microseconds\n";
            return result;
        }
    }
    
    // 推导技巧2：类型擦除与推导结合
    class AnyCallable {
        struct CallableBase {
            virtual ~CallableBase() = default;
            virtual void call() = 0;
        };
        
        template<typename F>
        struct CallableImpl : CallableBase {
            F func;
            CallableImpl(F&& f) : func(std::forward<F>(f)) {}
            void call() override { func(); }
        };
        
        std::unique_ptr<CallableBase> impl;
        
    public:
        template<typename F>
        AnyCallable(F&& f) : impl(std::make_unique<CallableImpl<std::decay_t<F>>>(std::forward<F>(f))) {}
        
        void operator()() { if (impl) impl->call(); }
    };
    
    // 使用示例
    auto lambda = []() { std::cout << "Lambda called through type erasure\n"; };
    auto add_func = [](int a, int b) { return a + b; };
    
    // 测试推导和性能
    auto result = invoke_and_time(add_func, 10, 20);
    std::cout << "Addition result: " << result << "\n";
    
    invoke_and_time(lambda);
    
    AnyCallable any_call{lambda};
    any_call();
    
    // 推导技巧3：编译期类型信息收集
    template<typename... Types>
    struct TypeInfo {
        static constexpr size_t count = sizeof...(Types);
        static constexpr size_t total_size = (sizeof(Types) + ...);
        
        static void print_info() {
            std::cout << "Type pack info: " << count << " types, total size: " << total_size << " bytes\n";
            ((std::cout << "  " << typeid(Types).name() << " (" << sizeof(Types) << " bytes)\n"), ...);
        }
    };
    
    template<typename... Args>
    auto make_type_info(Args&&...) {
        return TypeInfo<std::decay_t<Args>...>{};
    }
    
    auto type_info = make_type_info(42, 3.14, std::string{"hello"}, std::vector<int>{});
    type_info.print_info();
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "第2章：参数推导的算法原理深度解析\n";
    std::cout << "=====================================\n";
    
    demonstrate_basic_deduction_rules();
    demonstrate_reference_collapsing();
    demonstrate_auto_vs_template_deduction();
    demonstrate_ctad();
    demonstrate_deduction_contexts();
    demonstrate_deduction_failure_and_sfinae();
    demonstrate_advanced_deduction_techniques();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall -Wextra 02_parameter_deduction.cpp -o parameter_deduction
./parameter_deduction

关键学习点:
1. 理解模板参数推导的三种形式：T、T&、T&&
2. 掌握引用折叠规则是理解完美转发的基础
3. auto推导与模板推导基本相同，但有特殊情况(如initializer_list)
4. CTAD使类模板使用更简洁，推导指引提供额外控制
5. 区分推导上下文和非推导上下文，合理设计接口

高级技巧:
- 利用推导失败实现SFINAE
- 结合if constexpr进行条件编译
- 使用decltype(auto)保持返回类型
- 通过推导实现零开销抽象

注意事项:
- 推导不总是完美的，有时需要显式指定
- 推导会影响重载解析的结果
- 某些情况下推导可能产生意外的类型
- 合理使用推导提升代码可读性和维护性
*/