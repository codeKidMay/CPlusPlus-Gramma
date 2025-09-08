/**
 * C++11 Lambda表达式深度解析
 * 
 * 核心概念：
 * 1. 闭包类的编译器生成机制
 * 2. 捕获模式的内存布局和性能影响
 * 3. 函数对象与lambda的等价性
 * 4. 类型擦除和std::function的开销
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <type_traits>

// ===== 1. Lambda语法全览 =====

void demonstrate_lambda_syntax() {
    std::cout << "=== Lambda语法全览 ===\n";
    
    // 最简单的lambda
    auto simple = []() { std::cout << "简单lambda\n"; };
    simple();
    
    // 带参数的lambda
    auto with_params = [](int x, double y) { 
        return x + y; 
    };
    std::cout << "带参数: " << with_params(5, 3.14) << std::endl;
    
    // 显式返回类型
    auto explicit_return = [](int x) -> double { 
        return x * 3.14; 
    };
    std::cout << "显式返回: " << explicit_return(10) << std::endl;
    
    // 多语句lambda
    auto multi_statement = [](int x) {
        int result = x * x;
        result += 10;
        std::cout << "计算过程: " << x << "² + 10 = " << result << std::endl;
        return result;
    };
    multi_statement(5);
}

// ===== 2. 捕获模式深度解析 =====

void demonstrate_capture_modes() {
    std::cout << "\n=== 捕获模式深度解析 ===\n";
    
    int x = 10;
    double y = 3.14;
    std::string name = "Lambda";
    
    // 按值捕获 - 拷贝一份数据到闭包类中
    auto capture_by_value = [x, y](int multiplier) {
        std::cout << "按值捕获: x=" << x << ", y=" << y << std::endl;
        return x * multiplier + y;
        // 注意：这里的x和y是副本，修改不影响原变量
    };
    std::cout << "结果: " << capture_by_value(2) << std::endl;
    
    // 按引用捕获 - 存储引用到闭包类中
    auto capture_by_reference = [&x, &y](int increment) {
        x += increment;  // 直接修改原变量
        y += increment;
        std::cout << "按引用捕获修改后: x=" << x << ", y=" << y << std::endl;
    };
    capture_by_reference(5);
    std::cout << "原变量被修改: x=" << x << ", y=" << y << std::endl;
    
    // 混合捕获
    auto mixed_capture = [x, &name](const std::string& suffix) {
        name += suffix;  // 按引用捕获，可以修改
        std::cout << "混合捕获: x=" << x << ", name=" << name << std::endl;
    };
    mixed_capture("_modified");
    
    // 捕获所有 - 注意性能影响
    auto capture_all_by_value = [=]() {
        std::cout << "全部按值捕获: x=" << x << ", y=" << y << ", name=" << name << std::endl;
        // 编译器会拷贝当前作用域的所有可捕获变量
    };
    capture_all_by_value();
    
    auto capture_all_by_reference = [&]() {
        x *= 2;
        y *= 2;
        name += "_ref_all";
        std::cout << "全部按引用捕获并修改\n";
    };
    capture_all_by_reference();
    std::cout << "修改后: x=" << x << ", y=" << y << ", name=" << name << std::endl;
}

// ===== 3. mutable关键字与闭包状态 =====

void demonstrate_mutable_lambda() {
    std::cout << "\n=== mutable Lambda演示 ===\n";
    
    int counter = 0;
    
    // 非mutable lambda - 按值捕获的变量不能修改
    auto immutable_counter = [counter]() {
        std::cout << "不可变计数器: " << counter << std::endl;
        // counter++; // 编译错误！
    };
    
    // mutable lambda - 可以修改按值捕获的变量副本
    auto mutable_counter = [counter](int increment) mutable {
        counter += increment;  // 修改的是副本
        std::cout << "可变计数器副本: " << counter << std::endl;
        return counter;
    };
    
    std::cout << "原始counter: " << counter << std::endl;
    mutable_counter(10);
    mutable_counter(20);
    std::cout << "调用后原始counter: " << counter << std::endl;  // 不变
    
    // 实际应用：创建有状态的lambda
    auto accumulator = [sum = 0](int value) mutable -> int {
        sum += value;
        return sum;
    };
    
    std::cout << "累加器测试:\n";
    std::cout << accumulator(5) << std::endl;   // 5
    std::cout << accumulator(10) << std::endl;  // 15
    std::cout << accumulator(3) << std::endl;   // 18
}

// ===== 4. 编译器生成的闭包类机制 =====

class ManualClosure {
private:
    int captured_x;
    double& captured_y_ref;
    std::string captured_name;
    
public:
    ManualClosure(int x, double& y, const std::string& name)
        : captured_x(x), captured_y_ref(y), captured_name(name) {}
    
    void operator()(int multiplier) const {
        std::cout << "手动闭包类: x=" << captured_x 
                  << ", y=" << captured_y_ref 
                  << ", name=" << captured_name
                  << ", multiplier=" << multiplier << std::endl;
    }
    
    // 编译器生成的类型信息
    using is_lambda = std::true_type;
};

void demonstrate_closure_class_equivalent() {
    std::cout << "\n=== 闭包类等价演示 ===\n";
    
    int x = 42;
    double y = 2.718;
    std::string name = "ClosureDemo";
    
    // Lambda表达式
    auto lambda = [x, &y, name](int multiplier) {
        std::cout << "Lambda: x=" << x 
                  << ", y=" << y 
                  << ", name=" << name
                  << ", multiplier=" << multiplier << std::endl;
    };
    
    // 等价的手动闭包类
    ManualClosure manual_closure(x, y, name);
    
    std::cout << "Lambda调用:\n";
    lambda(10);
    
    std::cout << "手动闭包调用:\n";
    manual_closure(10);
    
    // 类型信息对比
    std::cout << "Lambda类型大小: " << sizeof(lambda) << " bytes\n";
    std::cout << "手动闭包大小: " << sizeof(manual_closure) << " bytes\n";
}

// ===== 5. Lambda与函数指针的转换 =====

void demonstrate_lambda_to_function_pointer() {
    std::cout << "\n=== Lambda与函数指针转换 ===\n";
    
    // 无捕获的lambda可以转换为函数指针
    auto no_capture = [](int x, int y) -> int {
        return x + y;
    };
    
    // 转换为函数指针
    int (*func_ptr)(int, int) = no_capture;
    std::cout << "函数指针调用: " << func_ptr(10, 20) << std::endl;
    
    // 也可以显式转换
    auto func_ptr2 = static_cast<int(*)(int, int)>(no_capture);
    std::cout << "显式转换调用: " << func_ptr2(15, 25) << std::endl;
    
    // 有捕获的lambda无法转换为函数指针
    int captured = 100;
    auto with_capture = [captured](int x) {
        return x + captured;
    };
    // int (*bad_ptr)(int) = with_capture;  // 编译错误！
    
    std::cout << "有捕获的lambda调用: " << with_capture(50) << std::endl;
}

// ===== 6. std::function的类型擦除开销 =====

void demonstrate_function_wrapper() {
    std::cout << "\n=== std::function包装器演示 ===\n";
    
    // 直接lambda调用（零开销）
    auto direct_lambda = [](int x) { return x * x; };
    
    // std::function包装（有类型擦除开销）
    std::function<int(int)> wrapped_lambda = [](int x) { return x * x; };
    
    // 普通函数
    auto square_func = [](int x) -> int { return x * x; };
    
    // 性能测试
    const int iterations = 10000000;
    int result = 0;
    
    // 测试直接lambda调用
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        result += direct_lambda(i % 100);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto direct_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 测试std::function包装调用
    result = 0;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        result += wrapped_lambda(i % 100);
    }
    end = std::chrono::high_resolution_clock::now();
    auto wrapped_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "直接lambda调用时间: " << direct_time.count() << "μs\n";
    std::cout << "std::function调用时间: " << wrapped_time.count() << "μs\n";
    std::cout << "性能差异: " << (static_cast<double>(wrapped_time.count()) / direct_time.count()) << "x\n";
    
    // 大小对比
    std::cout << "直接lambda大小: " << sizeof(direct_lambda) << " bytes\n";
    std::cout << "std::function大小: " << sizeof(wrapped_lambda) << " bytes\n";
}

// ===== 7. Lambda在算法中的应用 =====

void demonstrate_lambda_with_algorithms() {
    std::cout << "\n=== Lambda在STL算法中的应用 ===\n";
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // 查找第一个偶数
    auto even_it = std::find_if(numbers.begin(), numbers.end(), 
                                [](int n) { return n % 2 == 0; });
    if (even_it != numbers.end()) {
        std::cout << "第一个偶数: " << *even_it << std::endl;
    }
    
    // 计算偶数个数
    auto even_count = std::count_if(numbers.begin(), numbers.end(),
                                   [](int n) { return n % 2 == 0; });
    std::cout << "偶数个数: " << even_count << std::endl;
    
    // 变换：所有数字平方
    std::vector<int> squares(numbers.size());
    std::transform(numbers.begin(), numbers.end(), squares.begin(),
                  [](int n) { return n * n; });
    
    std::cout << "平方数: ";
    for (int sq : squares) {
        std::cout << sq << " ";
    }
    std::cout << std::endl;
    
    // 复杂条件：捕获外部状态进行过滤
    int threshold = 5;
    std::vector<int> filtered;
    std::copy_if(numbers.begin(), numbers.end(), std::back_inserter(filtered),
                [threshold](int n) { return n > threshold; });
    
    std::cout << "大于" << threshold << "的数: ";
    for (int n : filtered) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // 排序：自定义比较器
    std::vector<std::string> words = {"apple", "banana", "cherry", "date"};
    std::sort(words.begin(), words.end(), 
              [](const std::string& a, const std::string& b) {
                  return a.length() < b.length();  // 按长度排序
              });
    
    std::cout << "按长度排序: ";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
}

// ===== 8. 递归Lambda的实现技巧 =====

void demonstrate_recursive_lambda() {
    std::cout << "\n=== 递归Lambda演示 ===\n";
    
    // 方法1: 使用std::function
    std::function<int(int)> factorial = [&](int n) -> int {
        return n <= 1 ? 1 : n * factorial(n - 1);
    };
    
    std::cout << "5! = " << factorial(5) << std::endl;
    
    // 方法2: 使用auto和引用捕获（C++14后更简洁）
    auto fibonacci = [](int n) {
        std::function<int(int)> fib = [&](int x) -> int {
            return x <= 1 ? x : fib(x-1) + fib(x-2);
        };
        return fib(n);
    };
    
    std::cout << "fibonacci(10) = " << fibonacci(10) << std::endl;
    
    // 方法3: 使用固定点组合子（高阶技巧）
    auto make_recursive = [](auto func) {
        return [=](auto&&... args) {
            return func(func, std::forward<decltype(args)>(args)...);
        };
    };
    
    auto gcd = make_recursive([](auto gcd_impl, int a, int b) -> int {
        return b == 0 ? a : gcd_impl(gcd_impl, b, a % b);
    });
    
    std::cout << "gcd(48, 18) = " << gcd(48, 18) << std::endl;
}

// ===== 9. Lambda的实际应用场景 =====

class EventSystem {
private:
    std::vector<std::function<void(int)>> callbacks;
    
public:
    void add_callback(std::function<void(int)> callback) {
        callbacks.push_back(callback);
    }
    
    void trigger_event(int event_data) {
        std::cout << "触发事件，数据: " << event_data << std::endl;
        for (auto& callback : callbacks) {
            callback(event_data);
        }
    }
};

void demonstrate_practical_applications() {
    std::cout << "\n=== Lambda实际应用场景 ===\n";
    
    EventSystem event_system;
    
    // 应用1: 事件回调系统
    int counter = 0;
    event_system.add_callback([&counter](int data) {
        counter += data;
        std::cout << "回调1: 累计值 = " << counter << std::endl;
    });
    
    event_system.add_callback([](int data) {
        std::cout << "回调2: 数据平方 = " << data * data << std::endl;
    });
    
    event_system.trigger_event(5);
    event_system.trigger_event(3);
    
    // 应用2: 资源管理和RAII
    auto resource_manager = [](const std::string& resource_name) {
        std::cout << "获取资源: " << resource_name << std::endl;
        return [resource_name]() {  // 返回清理lambda
            std::cout << "释放资源: " << resource_name << std::endl;
        };
    };
    
    {
        auto cleanup = resource_manager("数据库连接");
        std::cout << "使用资源...\n";
        // cleanup会在作用域结束时自动调用
    }
    
    // 应用3: 条件工厂
    auto create_filter = [](const std::string& type) {
        if (type == "even") {
            return [](int n) { return n % 2 == 0; };
        } else if (type == "positive") {
            return [](int n) { return n > 0; };
        } else {
            return [](int n) { return true; };
        }
    };
    
    std::vector<int> test_data = {-2, -1, 0, 1, 2, 3, 4, 5};
    
    auto even_filter = create_filter("even");
    auto positive_filter = create_filter("positive");
    
    std::cout << "偶数: ";
    for (int n : test_data) {
        if (even_filter(n)) std::cout << n << " ";
    }
    std::cout << std::endl;
    
    std::cout << "正数: ";
    for (int n : test_data) {
        if (positive_filter(n)) std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===== 10. Lambda的陷阱和最佳实践 =====

void demonstrate_lambda_pitfalls() {
    std::cout << "\n=== Lambda陷阱和最佳实践 ===\n";
    
    std::vector<std::function<void()>> delayed_actions;
    
    // 陷阱1: 悬空引用
    {
        int local_var = 42;
        
        // 危险：按引用捕获局部变量
        delayed_actions.push_back([&local_var]() {
            // 当local_var销毁后，这里就是悬空引用
            std::cout << "危险的引用: " << local_var << std::endl;
        });
        
        // 安全：按值捕获
        delayed_actions.push_back([local_var]() {
            std::cout << "安全的值捕获: " << local_var << std::endl;
        });
        
    }  // local_var在这里被销毁
    
    std::cout << "执行延迟动作...\n";
    // delayed_actions[0]();  // 未定义行为！悬空引用
    delayed_actions[1]();     // 安全
    
    // 陷阱2: 意外的拷贝开销
    struct ExpensiveToCopy {
        std::vector<int> data;
        ExpensiveToCopy() : data(1000, 42) {
            std::cout << "构造昂贵对象\n";
        }
        ExpensiveToCopy(const ExpensiveToCopy&) : data(1000, 42) {
            std::cout << "拷贝昂贵对象（开销大！）\n";
        }
    };
    
    ExpensiveToCopy expensive;
    
    // 不好：按值捕获导致昂贵拷贝
    auto bad_lambda = [expensive]() {
        std::cout << "使用拷贝的对象，大小: " << expensive.data.size() << std::endl;
    };
    
    // 好：按引用捕获（注意生命周期）
    auto good_lambda = [&expensive]() {
        std::cout << "使用引用的对象，大小: " << expensive.data.size() << std::endl;
    };
    
    // 更好：智能指针捕获
    auto smart_ptr = std::make_shared<ExpensiveToCopy>();
    auto best_lambda = [smart_ptr]() {
        std::cout << "使用智能指针，大小: " << smart_ptr->data.size() << std::endl;
    };
    
    bad_lambda();   // 触发拷贝
    good_lambda();  // 无拷贝但要注意生命周期
    best_lambda();  // 无拷贝且自动管理生命周期
    
    // 最佳实践总结
    std::cout << "\nLambda最佳实践:\n";
    std::cout << "1. 优先使用按值捕获小对象\n";
    std::cout << "2. 对于大对象使用引用捕获或智能指针\n";
    std::cout << "3. 避免捕获this指针的悬空引用\n";
    std::cout << "4. 使用mutable仅当确实需要修改捕获的副本\n";
    std::cout << "5. 对于性能敏感代码避免std::function包装\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 Lambda表达式深度解析\n";
    std::cout << "===========================\n";
    
    // 基础语法
    demonstrate_lambda_syntax();
    
    // 捕获模式
    demonstrate_capture_modes();
    
    // mutable关键字
    demonstrate_mutable_lambda();
    
    // 闭包类机制
    demonstrate_closure_class_equivalent();
    
    // 函数指针转换
    demonstrate_lambda_to_function_pointer();
    
    // std::function开销
    demonstrate_function_wrapper();
    
    // STL算法应用
    demonstrate_lambda_with_algorithms();
    
    // 递归lambda
    demonstrate_recursive_lambda();
    
    // 实际应用
    demonstrate_practical_applications();
    
    // 陷阱和最佳实践
    demonstrate_lambda_pitfalls();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 02_lambda_expressions.cpp -o lambda_demo
./lambda_demo

关键学习点:
1. 理解编译器如何生成闭包类
2. 掌握不同捕获模式的性能影响
3. 学会在合适场景使用lambda vs 函数对象
4. 理解std::function的类型擦除开销
5. 避免常见的生命周期和性能陷阱
6. 掌握lambda在现代C++中的惯用法
*/