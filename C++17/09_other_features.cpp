/**
 * C++17 其他重要特性深度解析
 * 
 * 核心概念：
 * 1. 嵌套命名空间语法 - 简化深层命名空间声明
 * 2. 强制拷贝省略 - 编译器保证的返回值优化
 * 3. 异常规范简化 - noexcept的统一化
 * 4. 新的属性标准 - [[fallthrough]]、[[maybe_unused]]等
 * 5. 其他语法改进 - 十六进制浮点字面量、auto推导等
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <utility>
#include <type_traits>

// ===== 1. 嵌套命名空间语法演示 =====
// C++14及之前的写法
namespace Company {
    namespace Project {
        namespace Module {
            namespace Core {
                class LegacyComponent {
                public:
                    void legacy_method() {
                        std::cout << "Legacy nested namespace component\n";
                    }
                };
            }
        }
    }
}

// C++17简化写法
namespace Company::Project::Module::Core {
    class ModernComponent {
    public:
        void modern_method() {
            std::cout << "Modern nested namespace component\n";
        }
    };
    
    // 内部命名空间仍然可以进一步嵌套
    namespace Detail {
        void internal_function() {
            std::cout << "Internal detail function\n";
        }
    }
}

// 命名空间别名的现代化用法
namespace CpmcCore = Company::Project::Module::Core;

// 匿名命名空间的嵌套
namespace Company::Project::Module {
    namespace {  // 匿名命名空间
        constexpr int INTERNAL_CONSTANT = 42;
        
        void internal_helper() {
            std::cout << "Internal helper in anonymous namespace\n";
        }
    }
    
    void use_internal() {
        internal_helper();
        std::cout << "Using internal constant: " << INTERNAL_CONSTANT << "\n";
    }
}

void demonstrate_nested_namespace_syntax() {
    std::cout << "=== 嵌套命名空间语法演示 ===\n";
    
    // 使用传统方式声明的组件
    Company::Project::Module::Core::LegacyComponent legacy;
    legacy.legacy_method();
    
    // 使用现代方式声明的组件
    Company::Project::Module::Core::ModernComponent modern;
    modern.modern_method();
    
    // 调用内部命名空间的函数
    Company::Project::Module::Core::Detail::internal_function();
    
    // 使用命名空间别名
    CpmcCore::ModernComponent aliased;
    aliased.modern_method();
    
    // 使用匿名命名空间的功能
    Company::Project::Module::use_internal();
    
    std::cout << "\n";
}

// ===== 2. 强制拷贝省略演示 =====
// 用于测试拷贝省略的类
class CopyElidingTest {
private:
    std::string data;
    int id;
    static inline int counter = 0;
    
public:
    // 默认构造函数
    CopyElidingTest() : data("default"), id(++counter) {
        std::cout << "构造函数 #" << id << "\n";
    }
    
    // 带参数的构造函数
    explicit CopyElidingTest(const std::string& str) : data(str), id(++counter) {
        std::cout << "参数构造函数 #" << id << " (" << data << ")\n";
    }
    
    // 拷贝构造函数
    CopyElidingTest(const CopyElidingTest& other) : data(other.data), id(++counter) {
        std::cout << "拷贝构造函数 #" << id << " (从 #" << other.id << ")\n";
    }
    
    // 移动构造函数
    CopyElidingTest(CopyElidingTest&& other) noexcept : data(std::move(other.data)), id(++counter) {
        std::cout << "移动构造函数 #" << id << " (从 #" << other.id << ")\n";
    }
    
    // 拷贝赋值运算符
    CopyElidingTest& operator=(const CopyElidingTest& other) {
        if (this != &other) {
            data = other.data;
            std::cout << "拷贝赋值 #" << id << " = #" << other.id << "\n";
        }
        return *this;
    }
    
    // 移动赋值运算符
    CopyElidingTest& operator=(CopyElidingTest&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            std::cout << "移动赋值 #" << id << " = #" << other.id << "\n";
        }
        return *this;
    }
    
    // 析构函数
    ~CopyElidingTest() {
        std::cout << "析构函数 #" << id << "\n";
    }
    
    const std::string& get_data() const { return data; }
    int get_id() const { return id; }
    
    static void reset_counter() { counter = 0; }
};

// C++17强制拷贝省略的情况
CopyElidingTest create_by_value() {
    return CopyElidingTest("created_by_value");  // 强制省略拷贝
}

CopyElidingTest create_temporary() {
    return CopyElidingTest{"temporary"};  // 强制省略拷贝
}

// 条件返回（可能无法省略）
CopyElidingTest conditional_create(bool condition) {
    if (condition) {
        return CopyElidingTest("condition_true");   // 可能省略
    } else {
        return CopyElidingTest("condition_false");  // 可能省略
    }
}

// NRVO (Named Return Value Optimization)
CopyElidingTest named_return_value() {
    CopyElidingTest obj("named_object");
    // ... 一些操作
    return obj;  // NRVO：命名返回值优化
}

// 不能省略的情况
CopyElidingTest cannot_elide(CopyElidingTest obj) {
    return obj;  // 参数不能省略拷贝
}

void demonstrate_copy_elision() {
    std::cout << "=== 强制拷贝省略演示 ===\n";
    
    CopyElidingTest::reset_counter();
    
    // 1. 强制拷贝省略 - 临时对象直接构造
    std::cout << "1. 强制拷贝省略 - 返回临时对象:\n";
    auto obj1 = create_by_value();  // 在C++17中保证不发生拷贝
    std::cout << "结果: " << obj1.get_data() << " #" << obj1.get_id() << "\n\n";
    
    // 2. 强制拷贝省略 - 花括号初始化
    std::cout << "2. 强制拷贝省略 - 花括号初始化:\n";
    auto obj2 = create_temporary();
    std::cout << "结果: " << obj2.get_data() << " #" << obj2.get_id() << "\n\n";
    
    // 3. NRVO - 编译器优化（不保证，但通常会发生）
    std::cout << "3. NRVO - 命名返回值优化:\n";
    auto obj3 = named_return_value();
    std::cout << "结果: " << obj3.get_data() << " #" << obj3.get_id() << "\n\n";
    
    // 4. 条件返回 - 可能的省略
    std::cout << "4. 条件返回:\n";
    auto obj4 = conditional_create(true);
    std::cout << "结果: " << obj4.get_data() << " #" << obj4.get_id() << "\n\n";
    
    // 5. 不能省略的情况
    std::cout << "5. 参数传递（不能省略）:\n";
    CopyElidingTest param_obj("parameter");
    auto obj5 = cannot_elide(param_obj);  // 这里会发生拷贝或移动
    std::cout << "结果: " << obj5.get_data() << " #" << obj5.get_id() << "\n\n";
    
    std::cout << "\n";
}

// ===== 3. 异常规范简化演示 =====
// C++17废弃的异常规范
// void old_style_exception_spec() throw(std::runtime_error);  // 已废弃

// C++17只保留noexcept
void no_exception_function() noexcept {
    // 这个函数保证不抛出异常
    std::cout << "这个函数不会抛出异常\n";
}

void may_throw_function() {
    // 没有noexcept，可能抛出异常
    throw std::runtime_error("This function may throw");
}

// 条件化的noexcept
template<typename T>
void conditional_noexcept_function(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) {
    T local_copy = std::forward<T>(value);
    std::cout << "Conditional noexcept function\n";
}

// noexcept说明符在移动语义中的重要性
class NoexceptDemo {
private:
    std::vector<int> data;
    
public:
    // 带有noexcept的移动构造函数
    NoexceptDemo(NoexceptDemo&& other) noexcept 
        : data(std::move(other.data)) {
        std::cout << "noexcept移动构造函数\n";
    }
    
    // 带有noexcept的移动赋值运算符
    NoexceptDemo& operator=(NoexceptDemo&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            std::cout << "noexcept移动赋值运算符\n";
        }
        return *this;
    }
    
    // 拷贝构造函数（不是noexcept）
    NoexceptDemo(const NoexceptDemo& other) : data(other.data) {
        std::cout << "拷贝构造函数\n";
    }
    
    explicit NoexceptDemo(size_t size) : data(size) {}
};

void demonstrate_exception_specification() {
    std::cout << "=== 异常规范简化演示 ===\n";
    
    // noexcept函数调用
    std::cout << "调用noexcept函数:\n";
    no_exception_function();
    
    // 检查函数的noexcept属性
    std::cout << "函数noexcept属性检查:\n";
    std::cout << "no_exception_function是noexcept: " 
              << noexcept(no_exception_function()) << "\n";
    std::cout << "may_throw_function是noexcept: " 
              << noexcept(may_throw_function()) << "\n";
    
    // 条件化noexcept
    std::cout << "条件化noexcept测试:\n";
    int value = 42;
    std::cout << "int的conditional_noexcept是noexcept: " 
              << noexcept(conditional_noexcept_function(value)) << "\n";
    
    // 容器操作中的noexcept重要性
    std::cout << "容器重分配时的noexcept效应:\n";
    std::vector<NoexceptDemo> vec;
    vec.emplace_back(10);
    vec.emplace_back(20);
    
    std::cout << "vector扩容时...\n";
    vec.resize(10);  // 这里会使用移动语义，因为移动构造函数是noexcept的
    
    std::cout << "\n";
}

// ===== 4. 新的属性标准演示 =====
// [[fallthrough]] 属性
void demonstrate_fallthrough_attribute(int value) {
    switch (value) {
        case 1:
            std::cout << "Case 1\n";
            [[fallthrough]];  // 明确表示故意fallthrough
        case 2:
            std::cout << "Case 1 or 2\n";
            break;
        case 3:
            std::cout << "Case 3\n";
            [[fallthrough]];
        default:
            std::cout << "Case 3 or default\n";
            break;
    }
}

// [[maybe_unused]] 属性
void demonstrate_maybe_unused_attribute() {
    [[maybe_unused]] int unused_variable = 42;  // 不会产生未使用变量警告
    
    int used_variable = 10;
    std::cout << "Used variable: " << used_variable << "\n";
    
    // 参数可能未使用
    auto lambda = []([[maybe_unused]] int param1, int param2) {
        // param1在某些编译配置下可能未使用
        return param2 * 2;
    };
    
    std::cout << "Lambda result: " << lambda(1, 5) << "\n";
}

// [[nodiscard]] 属性 
[[nodiscard]] int important_calculation(int x, int y) {
    return x * x + y * y;
}

[[nodiscard]] std::optional<std::string> parse_string(const std::string& input) {
    if (input.empty()) {
        return std::nullopt;
    }
    return input + "_parsed";
}

class [[nodiscard]] ImportantResource {
public:
    explicit ImportantResource(const std::string& name) : resource_name(name) {
        std::cout << "获取资源: " << resource_name << "\n";
    }
    
    ~ImportantResource() {
        std::cout << "释放资源: " << resource_name << "\n";
    }
    
private:
    std::string resource_name;
};

void demonstrate_nodiscard_attribute() {
    std::cout << "=== [[nodiscard]] 属性演示 ===\n";
    
    // 正确使用：保存返回值
    auto result = important_calculation(3, 4);
    std::cout << "计算结果: " << result << "\n";
    
    // 错误用法（会产生警告）：
    // important_calculation(5, 6);  // 警告：丢弃nodiscard函数的返回值
    
    // optional的nodiscard使用
    auto parsed = parse_string("test");
    if (parsed) {
        std::cout << "解析结果: " << *parsed << "\n";
    }
    
    // 类的nodiscard
    auto resource = ImportantResource("critical_resource");  // 正确：保存对象
    // ImportantResource("temp_resource");  // 警告：临时对象立即销毁
    
    std::cout << "\n";
}

void demonstrate_attributes() {
    std::cout << "=== 新的属性标准演示 ===\n";
    
    // [[fallthrough]] 演示
    std::cout << "[[fallthrough]] 属性演示:\n";
    demonstrate_fallthrough_attribute(1);
    demonstrate_fallthrough_attribute(3);
    
    // [[maybe_unused]] 演示
    std::cout << "[[maybe_unused]] 属性演示:\n";
    demonstrate_maybe_unused_attribute();
    
    // [[nodiscard]] 演示
    demonstrate_nodiscard_attribute();
    
    std::cout << "\n";
}

// ===== 5. 其他语法改进演示 =====
void demonstrate_other_syntax_improvements() {
    std::cout << "=== 其他语法改进演示 ===\n";
    
    // 1. 十六进制浮点字面量
    std::cout << "十六进制浮点字面量:\n";
    double hex_float = 0x1.2p3;  // 1.125 * 2^3 = 9.0
    std::cout << "0x1.2p3 = " << hex_float << "\n";
    
    double another_hex = 0x1.Ap-1;  // 1.625 * 2^(-1) = 0.8125
    std::cout << "0x1.Ap-1 = " << another_hex << "\n";
    
    // 2. auto推导的改进
    std::cout << "auto推导改进:\n";
    
    // 模板参数推导
    auto lambda_with_auto = [](auto x, auto y) {
        return x + y;
    };
    
    std::cout << "auto lambda: " << lambda_with_auto(1, 2.5) << "\n";
    std::cout << "auto lambda: " << lambda_with_auto(std::string{"Hello"}, std::string{" World"}) << "\n";
    
    // 3. 静态断言的改进
    std::cout << "静态断言改进:\n";
    static_assert(sizeof(int) >= 4);  // C++17可以省略错误消息
    static_assert(std::is_integral_v<int>, "int必须是整型");  // 仍然可以提供消息
    
    std::cout << "静态断言通过\n";
    
    // 4. if和switch的初始化语句
    std::cout << "if和switch初始化语句:\n";
    
    // if语句中的初始化
    if (auto result = important_calculation(2, 3); result > 10) {
        std::cout << "if初始化: 结果 " << result << " 大于10\n";
    } else {
        std::cout << "if初始化: 结果 " << result << " 不大于10\n";
    }
    
    // switch语句中的初始化
    switch (auto value = important_calculation(1, 1); value % 3) {
        case 0:
            std::cout << "switch初始化: " << value << " 能被3整除\n";
            break;
        case 1:
            std::cout << "switch初始化: " << value << " 除以3余1\n";
            break;
        case 2:
            std::cout << "switch初始化: " << value << " 除以3余2\n";
            break;
    }
    
    // 5. 范围for循环的改进
    std::cout << "范围for循环改进:\n";
    std::vector<std::pair<std::string, int>> pairs = {
        {"apple", 1}, {"banana", 2}, {"cherry", 3}
    };
    
    // 结构化绑定 + 范围for
    for (const auto& [fruit, count] : pairs) {
        std::cout << "水果: " << fruit << ", 数量: " << count << "\n";
    }
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 其他重要特性深度解析\n";
    std::cout << "===========================\n";
    
    demonstrate_nested_namespace_syntax();
    demonstrate_copy_elision();
    demonstrate_exception_specification();
    demonstrate_attributes();
    demonstrate_other_syntax_improvements();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 09_other_features.cpp -o other_features
./other_features

关键学习点:
1. 嵌套命名空间语法大幅简化了深层命名空间的声明
2. 强制拷贝省略确保了返回值优化的可靠性
3. 异常规范简化为只有noexcept，提供了更清晰的异常安全保证
4. 新属性标准提供了更好的代码文档化和编译器优化提示
5. 各种语法改进提升了代码的简洁性和表达力

注意事项:
- 嵌套命名空间语法不能用于部分特化
- 拷贝省略只在特定情况下被强制执行
- noexcept对移动语义和容器性能有重要影响
- 属性标准需要编译器支持才能发挥作用
- 新语法特性要注意向后兼容性
*/