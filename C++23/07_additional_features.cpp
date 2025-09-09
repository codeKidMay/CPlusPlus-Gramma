/**
 * C++23 补充特性和标准库改进深度解析
 * 
 * 核心概念：
 * 1. 栈追踪支持 - std::stacktrace提供调试和错误追踪能力
 * 2. 平坦容器 - std::flat_map和std::flat_set提供更好的缓存局部性
 * 3. 协程生成器 - std::generator简化协程使用
 * 4. 模块导入 - import std提供更快的编译
 * 5. 显式对象参数 - deducing this支持更好的成员函数推导
 * 6. 静态调用操作符 - static operator()支持无状态可调用对象
 * 7. 其他标准库改进 - 各种实用的小改进
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <queue>
#include <array>
#include <span>
#include <ranges>
#include <algorithm>
#include <memory>
#include <functional>
#include <chrono>
#include <format>
#include <compare>
#include <concepts>
#include <type_traits>
#include <utility>
#include <tuple>
#include <optional>
#include <variant>
#include <numbers>

// ===== 1. std::stacktrace - 栈追踪支持 =====
void demonstrate_stacktrace() {
    std::cout << "=== std::stacktrace - 栈追踪支持 ===\n";
    
    // 注意：std::stacktrace需要编译器支持
    // 这里展示概念性的使用方法
    
    std::cout << "std::stacktrace的主要用途:\n";
    std::cout << "1. 错误处理和调试\n";
    std::cout << "2. 性能分析\n";
    std::cout << "3. 日志记录\n";
    std::cout << "4. 异常信息增强\n\n";
    
    // 模拟的栈追踪使用
    auto log_stack_trace = []() {
        std::cout << "模拟栈追踪:\n";
        std::cout << "  #0 0x0000000000401234 in main() at main.cpp:42\n";
        std::cout << "  #1 0x0000000000401567 in function_a() at utils.cpp:15\n";
        std::cout << "  #2 0x0000000000401789 in function_b() at utils.cpp:23\n";
        std::cout << "  #3 0x0000000000401abc in error_handler() at error.cpp:8\n";
    };
    
    // 异常中的栈追踪
    class traced_exception : public std::exception {
    private:
        std::string message_;
        std::string stack_trace_;
        
    public:
        traced_exception(const std::string& msg, const std::string& trace)
            : message_(msg), stack_trace_(trace) {}
        
        const char* what() const noexcept override {
            return message_.c_str();
        }
        
        const std::string& get_stack_trace() const {
            return stack_trace_;
        }
    };
    
    // 模拟带栈追踪的异常抛出
    auto throw_with_trace = []() {
        std::string trace = "模拟栈追踪信息...";
        throw traced_exception("发生了错误", trace);
    };
    
    std::cout << "带栈追踪的异常处理:\n";
    try {
        throw_with_trace();
    } catch (const traced_exception& e) {
        std::cout << "错误: " << e.what() << "\n";
        std::cout << "栈追踪: " << e.get_stack_trace() << "\n";
    }
    
    std::cout << "\n";
}

// ===== 2. 平坦容器 - std::flat_map和std::flat_set =====
void demonstrate_flat_containers() {
    std::cout << "=== 平坦容器 - std::flat_map和std::flat_set ===\n";
    
    // 模拟std::flat_map的实现（基于排序的vector）
    template<typename Key, typename Value, typename Compare = std::less<Key>>
    class flat_map {
    private:
        std::vector<std::pair<Key, Value>> data_;
        Compare comp_;
        
    public:
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::pair<Key, Value>;
        using size_type = size_t;
        
        flat_map() = default;
        
        template<typename InputIt>
        flat_map(InputIt first, InputIt last, Compare comp = Compare{})
            : data_(first, last), comp_(comp) {
            std::sort(data_.begin(), data_.end(), 
                     [this](const auto& a, const auto& b) {
                         return comp_(a.first, b.first);
                     });
        }
        
        auto insert(const value_type& value) {
            auto it = std::lower_bound(data_.begin(), data_.end(), value,
                                      [this](const auto& a, const auto& b) {
                                          return comp_(a.first, b.first);
                                      });
            data_.insert(it, value);
        }
        
        auto find(const Key& key) {
            auto it = std::lower_bound(data_.begin(), data_.end(), key,
                                      [this](const auto& a, const auto& b) {
                                          return comp_(a.first, b);
                                      });
            return (it != data_.end() && it->first == key) ? it : data_.end();
        }
        
        auto& operator[](const Key& key) {
            auto it = find(key);
            if (it == data_.end()) {
                data_.insert(std::lower_bound(data_.begin(), data_.end(), key,
                                           [this](const auto& a, const auto& b) {
                                               return comp_(a.first, b.first);
                                           }), 
                           std::make_pair(key, Value{}));
                it = find(key);
            }
            return it->second;
        }
        
        size_t size() const { return data_.size(); }
        bool empty() const { return data_.empty(); }
        
        auto begin() { return data_.begin(); }
        auto end() { return data_.end(); }
        auto begin() const { return data_.begin(); }
        auto end() const { return data_.end(); }
    };
    
    // 性能对比测试
    std::cout << "容器性能对比:\n";
    
    // 准备测试数据
    const int size = 10000;
    std::vector<std::pair<int, std::string>> test_data;
    test_data.reserve(size);
    for (int i = 0; i < size; ++i) {
        test_data.emplace_back(i, "value_" + std::to_string(i));
    }
    
    // 测试std::map
    auto start = std::chrono::high_resolution_clock::now();
    std::map<int, std::string> regular_map(test_data.begin(), test_data.end());
    auto end = std::chrono::high_resolution_clock::now();
    auto map_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 测试flat_map
    start = std::chrono::high_resolution_clock::now();
    flat_map<int, std::string> flat_container(test_data.begin(), test_data.end());
    end = std::chrono::high_resolution_clock::now();
    auto flat_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "std::map构造时间: " << map_time.count() << "μs\n";
    std::cout << "flat_map构造时间: " << flat_time.count() << "μs\n";
    std::cout << "性能比率: " << static_cast<double>(map_time.count()) / flat_time.count() << "x\n\n";
    
    // 遍历性能测试
    start = std::chrono::high_resolution_clock::now();
    long long map_sum = 0;
    for (const auto& [key, value] : regular_map) {
        map_sum += key;
    }
    end = std::chrono::high_resolution_clock::now();
    auto map_traverse = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    start = std::chrono::high_resolution_clock::now();
    long long flat_sum = 0;
    for (const auto& [key, value] : flat_container) {
        flat_sum += key;
    }
    end = std::chrono::high_resolution_clock::now();
    auto flat_traverse = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "std::map遍历时间: " << map_traverse.count() << "μs\n";
    std::cout << "flat_map遍历时间: " << flat_traverse.count() << "μs\n";
    std::cout << "遍历比率: " << static_cast<double>(map_traverse.count()) / flat_traverse.count() << "x\n";
    
    std::cout << "\n平坦容器的优势:\n";
    std::cout << "- 更好的缓存局部性\n";
    std::cout << "- 更少的内存分配\n";
    std::cout << "- 更快的遍历速度\n";
    std::cout << "- 适合读多写少的场景\n\n";
}

// ===== 3. 协程生成器 - std::generator =====
void demonstrate_generator() {
    std::cout << "=== 协程生成器 - std::generator ===\n";
    
    // 模拟std::generator的实现
    template<typename T>
    class generator {
    private:
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;
        
        struct promise_type {
            T current_value;
            auto get_return_object() { return generator{handle_type::from_promise(*this)}; }
            auto initial_suspend() { return std::suspend_always{}; }
            auto final_suspend() noexcept { return std::suspend_always{}; }
            void return_void() {}
            void unhandled_exception() { std::terminate(); }
            
            auto yield_value(T value) {
                current_value = std::move(value);
                return std::suspend_always{};
            }
        };
        
        handle_type coro_;
        
    public:
        generator(handle_type h) : coro_(h) {}
        ~generator() { if (coro_) coro_.destroy(); }
        
        generator(const generator&) = delete;
        generator& operator=(const generator&) = delete;
        
        generator(generator&& other) noexcept : coro_(other.coro_) {
            other.coro_ = nullptr;
        }
        
        generator& operator=(generator&& other) noexcept {
            if (this != &other) {
                if (coro_) coro_.destroy();
                coro_ = other.coro_;
                other.coro_ = nullptr;
            }
            return *this;
        }
        
        class iterator {
            handle_type coro_;
            bool done_;
            
        public:
            iterator(handle_type coro, bool done) : coro_(coro), done_(done) {}
            
            iterator& operator++() {
                coro_.resume();
                done_ = coro_.done();
                return *this;
            }
            
            bool operator==(const iterator& other) const {
                return done_ == other.done_;
            }
            
            T operator*() const {
                return coro_.promise().current_value;
            }
        };
        
        iterator begin() {
            coro_.resume();
            return iterator(coro_, coro_.done());
        }
        
        iterator end() {
            return iterator(coro_, true);
        }
    };
    
    // 生成器的实际应用
    auto fibonacci = []() -> generator<int> {
        int a = 0, b = 1;
        while (true) {
            co_yield a;
            std::tie(a, b) = std::make_tuple(b, a + b);
        }
    };
    
    auto range = [](int start, int end) -> generator<int> {
        for (int i = start; i < end; ++i) {
            co_yield i;
        }
    };
    
    std::cout << "Fibonacci数列生成器:\n";
    int count = 0;
    for (int fib : fibonacci()) {
        if (count++ >= 10) break;
        std::cout << fib << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "范围生成器:\n";
    for (int i : range(5, 15)) {
        std::cout << i << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "生成器的优势:\n";
    std::cout << "- 惰性求值，节省内存\n";
    std::cout << "- 支持无限序列\n";
    std::cout << "- 更清晰的代码结构\n";
    std::cout << "- 适用于复杂的序列生成\n\n";
}

// ===== 4. 显式对象参数 - deducing this =====
void demonstrate_deducing_this() {
    std::cout << "=== 显式对象参数 - deducing this ===\n";
    
    // 传统方式的问题
    struct Traditional {
        void print() {
            std::cout << "Traditional::print()\n";
        }
        
        template<typename T>
        void process(T& other) {
            std::cout << "Processing with traditional method\n";
        }
    };
    
    // 使用显式对象参数的新方式
    struct Modern {
        // 显式对象参数允许推导对象的类型
        void print(this auto&& self) {
            std::cout << "Modern::print() - ";
            if constexpr (std::is_const_v<std::remove_reference_t<decltype(self)>>) {
                std::cout << "const version\n";
            } else {
                std::cout << "non-const version\n";
            }
        }
        
        // 可以根据对象类型进行不同的处理
        template<typename T>
        void process(this T&& self, auto& other) {
            std::cout << "Processing with ";
            if constexpr (std::is_const_v<T>) {
                std::cout << "const ";
            }
            std::cout << "object\n";
        }
        
        // 递归调用示例
        void recursive(this auto&& self, int n) {
            if (n > 0) {
                std::cout << "Recursive call: " << n << "\n";
                self.recursive(n - 1);
            }
        }
    };
    
    std::cout << "显式对象参数演示:\n";
    
    Modern m;
    m.print();           // non-const version
    const Modern cm;
    cm.print();          // const version
    
    int x = 42;
    m.process(x);        // non-const object
    cm.process(x);       // const object
    
    std::cout << "\n递归调用演示:\n";
    m.recursive(3);
    
    std::cout << "\n显式对象参数的优势:\n";
    std::cout << "- 支持完美转发\n";
    std::cout << "- 可以区分const/non-const对象\n";
    std::cout << "- 支持递归调用\n";
    std::cout << "- 更好的模板推导\n";
    std::cout << "- 支持CRTP更简洁的实现\n\n";
}

// ===== 5. 静态调用操作符 - static operator() =====
void demonstrate_static_operator() {
    std::cout << "=== 静态调用操作符 - static operator() ===\n";
    
    // 传统函数对象
    struct TraditionalFunctor {
        int operator()(int x, int y) const {
            return x + y;
        }
    };
    
    // 静态调用操作符
    struct StaticFunctor {
        static int operator()(int x, int y) {
            return x + y;
        }
    };
    
    // 无状态的函数对象
    struct Stateless {
        static int multiply(int x, int y) {
            return x * y;
        }
        
        // 也可以有非静态成员
        int stateful_multiply(int x, int y) const {
            return x * y * factor_;
        }
        
    private:
        int factor_ = 2;
    };
    
    std::cout << "静态调用操作符演示:\n";
    
    TraditionalFunctor traditional;
    StaticFunctor static_func;
    
    std::cout << "Traditional: " << traditional(3, 4) << "\n";
    std::cout << "Static: " << static_func(3, 4) << "\n";
    std::cout << "Stateless static: " << Stateless::multiply(3, 4) << "\n";
    
    Stateless stateless;
    std::cout << "Stateless member: " << stateless.stateful_multiply(3, 4) << "\n";
    
    // 在模板中使用
    auto call_func = [](auto func, int x, int y) {
        if constexpr (requires { func(x, y); }) {
            return func(x, y);
        } else if constexpr (requires { decltype(func)::operator()(x, y); }) {
            return decltype(func)::operator()(x, y);
        } else {
            return 0;
        }
    };
    
    std::cout << "\n模板调用演示:\n";
    std::cout << "Traditional via template: " << call_func(traditional, 5, 6) << "\n";
    std::cout << "Static via template: " << call_func(static_func, 5, 6) << "\n";
    
    std::cout << "\n静态调用操作符的优势:\n";
    std::cout << "- 支持无状态的函数对象\n";
    std::cout << "- 可以作为类型成员使用\n";
    std::cout << "- 更好的模板推导\n";
    std::cout << "- 支持在模板中更灵活的使用\n\n";
}

// ===== 6. 其他标准库改进 =====
void demonstrate_other_improvements() {
    std::cout << "=== 其他标准库改进 ===\n";
    
    // 1. std::to_string的改进
    std::cout << "std::to_string改进:\n";
    std::cout << "整数: " << std::to_string(42) << "\n";
    std::cout << "浮点数: " << std::to_string(3.14159) << "\n";
    std::cout << "布尔值: " << std::to_string(true) << "\n\n";
    
    // 2. std::stoi等函数的改进
    std::cout << "字符串转换改进:\n";
    try {
        std::string num_str = "123";
        int num = std::stoi(num_str);
        std::cout << "字符串转整数: " << num << "\n";
        
        std::string float_str = "3.14";
        double d = std::stod(float_str);
        std::cout << "字符串转浮点数: " << d << "\n";
    } catch (const std::exception& e) {
        std::cout << "转换错误: " << e.what() << "\n";
    }
    
    // 3. 数学常量
    std::cout << "\n数学常量:\n";
    std::cout << "π: " << std::numbers::pi << "\n";
    std::cout << "e: " << std::numbers::e << "\n";
    std::cout << "√2: " << std::numbers::sqrt2 << "\n";
    std::cout << "ln(2): " << std::numbers::ln2 << "\n";
    
    // 4. 比较操作的改进
    std::cout << "\n比较操作改进:\n";
    struct StrongInt {
        int value;
        auto operator<=>(const StrongInt&) const = default;
    };
    
    StrongInt a{10}, b{20};
    if (a < b) {
        std::cout << "a < b (使用三路比较)\n";
    }
    
    // 5. std::span的改进
    std::cout << "\nstd::span改进:\n";
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::span<int> s(vec);
    
    std::cout << "Span size: " << s.size() << "\n";
    std::cout << "Span first 3 elements: ";
    for (int x : s.first(3)) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    // 6. 容器的改进
    std::cout << "\n容器改进:\n";
    std::vector<std::string> strings{"hello", "world", "cpp23"};
    
    // contains方法
    if (std::ranges::contains(strings, std::string("world"))) {
        std::cout << "包含'world'\n";
    }
    
    // erase_if
    auto erased = std::erase_if(strings, [](const std::string& s) {
        return s.length() == 5;
    });
    std::cout << "删除了 " << erased << " 个长度为5的字符串\n";
    
    std::cout << "剩余字符串: ";
    for (const auto& s : strings) {
        std::cout << s << " ";
    }
    std::cout << "\n\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++23 补充特性和标准库改进深度解析\n";
    std::cout << "=======================================\n";
    
    demonstrate_stacktrace();
    demonstrate_flat_containers();
    demonstrate_generator();
    demonstrate_deducing_this();
    demonstrate_static_operator();
    demonstrate_other_improvements();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall -fcoroutines 07_additional_features.cpp -o additional_demo
./additional_demo

注意：某些特性可能需要特定的编译器版本和标志：
- std::stacktrace: 需要编译器支持
- std::generator: 需要-fcoroutines标志
- std::flat_map: 可能需要实验性支持
- deducing this: 需要编译器支持

关键学习点:
1. std::stacktrace提供强大的调试和错误追踪能力
2. 平坦容器提供更好的缓存局部性和性能
3. 协程生成器简化了复杂序列的生成
4. 显式对象参数支持更灵活的成员函数设计
5. 静态调用操作符支持无状态的函数对象
6. 各种标准库改进提升了日常开发体验

注意事项:
- 部分特性可能还在实验性阶段
- 需要检查编译器的具体支持情况
- 某些特性可能需要特定的编译标志
- 在生产环境中使用前需要进行充分测试
*/