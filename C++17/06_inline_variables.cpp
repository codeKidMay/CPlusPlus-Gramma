/**
 * C++17 内联变量深度解析
 * 
 * 核心概念：
 * 1. 单一定义规则突破 - 头文件中的全局变量定义
 * 2. 模板变量内联化 - 消除多重定义问题
 * 3. 静态成员初始化 - 类内直接定义静态成员
 * 4. 编译器实现机制 - 链接期的符号合并
 * 5. 性能和内存优化 - ODR违反的解决和优化
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <type_traits>
#include <chrono>
#include <memory>

// ===== 1. 单一定义规则突破演示 =====
// 传统方式需要声明和定义分离
extern int traditional_counter;  // 声明，需要在cpp文件中定义

// C++17内联变量：声明即定义
inline int global_counter = 0;
inline std::string app_name = "ModernApp";
inline const double pi = 3.141592653589793;

// 复杂类型的内联变量
inline std::vector<std::string> supported_formats{".txt", ".log", ".dat", ".csv"};
inline std::map<std::string, int> error_codes{
    {"SUCCESS", 0},
    {"FILE_NOT_FOUND", 404},
    {"ACCESS_DENIED", 403},
    {"INTERNAL_ERROR", 500}
};

// 内联函数中的静态变量也受益
inline int& get_instance_counter() {
    static inline int counter = 0;  // 内联静态变量
    return counter;
}

void demonstrate_global_inline_variables() {
    std::cout << "=== 单一定义规则突破演示 ===\n";
    
    // 使用内联全局变量
    std::cout << "应用名称: " << app_name << "\n";
    std::cout << "初始计数器: " << global_counter << "\n";
    std::cout << "圆周率: " << pi << "\n";
    
    // 修改内联变量
    ++global_counter;
    app_name += "_v2";
    
    std::cout << "修改后计数器: " << global_counter << "\n";
    std::cout << "修改后应用名: " << app_name << "\n";
    
    // 复杂类型的使用
    std::cout << "支持的格式数量: " << supported_formats.size() << "\n";
    for (const auto& format : supported_formats) {
        std::cout << "  " << format << "\n";
    }
    
    std::cout << "错误代码映射:\n";
    for (const auto& [name, code] : error_codes) {
        std::cout << "  " << name << ": " << code << "\n";
    }
    
    // 内联静态变量
    auto& counter = get_instance_counter();
    std::cout << "实例计数器: " << counter++ << "\n";
    std::cout << "递增后: " << counter << "\n";
    
    std::cout << "\n";
}

// ===== 2. 模板变量内联化演示 =====
// C++14风格的模板变量（会在每个翻译单元生成定义）
template<typename T>
constexpr bool is_arithmetic_old_style = std::is_arithmetic<T>::value;

// C++17内联模板变量（避免多重定义）
template<typename T>
inline constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;

template<typename T>
inline constexpr bool is_pointer_v = std::is_pointer<T>::value;

template<typename T>
inline constexpr size_t type_size_v = sizeof(T);

// 复杂的模板变量
template<typename T>
inline constexpr bool is_small_type_v = sizeof(T) <= sizeof(void*);

template<typename T>
inline const std::string type_name_v = typeid(T).name();

// 编译期计算的内联变量
template<int N>
inline constexpr int fibonacci_v = (N <= 1) ? N : fibonacci_v<N-1> + fibonacci_v<N-2>;

// 类型特征的内联化包装
template<typename T>
inline constexpr bool is_container_v = requires(T t) {
    t.begin();
    t.end();
    t.size();
};

void demonstrate_template_inline_variables() {
    std::cout << "=== 模板变量内联化演示 ===\n";
    
    // 基本类型特征
    std::cout << "int是算术类型: " << is_arithmetic_v<int> << "\n";
    std::cout << "std::string是算术类型: " << is_arithmetic_v<std::string> << "\n";
    std::cout << "int*是指针类型: " << is_pointer_v<int*> << "\n";
    
    // 类型大小检查
    std::cout << "int大小: " << type_size_v<int> << " 字节\n";
    std::cout << "std::string大小: " << type_size_v<std::string> << " 字节\n";
    std::cout << "int是小类型: " << is_small_type_v<int> << "\n";
    std::cout << "std::string是小类型: " << is_small_type_v<std::string> << "\n";
    
    // 编译期斐波那契数列
    std::cout << "fibonacci(10): " << fibonacci_v<10> << "\n";
    std::cout << "fibonacci(15): " << fibonacci_v<15> << "\n";
    
    // 容器检查
    std::cout << "vector是容器: " << is_container_v<std::vector<int>> << "\n";
    std::cout << "int是容器: " << is_container_v<int> << "\n";
    
    std::cout << "\n";
}

// ===== 3. 静态成员初始化演示 =====
class DatabaseConfig {
private:
    static inline int connection_count = 0;
    
public:
    // 基本类型的内联静态成员
    static inline int max_connections = 100;
    static inline double timeout_seconds = 30.0;
    static inline bool enable_logging = true;
    
    // 复杂类型的内联静态成员
    static inline std::string default_host = "localhost";
    static inline std::vector<int> supported_ports{3306, 5432, 27017};
    static inline std::map<std::string, std::string> connection_params{
        {"charset", "utf8mb4"},
        {"timezone", "UTC"},
        {"pool_size", "10"}
    };
    
    // 内联静态方法访问内联静态成员
    static int create_connection() {
        if (connection_count >= max_connections) {
            throw std::runtime_error("超过最大连接数");
        }
        return ++connection_count;
    }
    
    static int get_active_connections() { return connection_count; }
    
    static void close_connection() {
        if (connection_count > 0) --connection_count;
    }
    
    // 编译期常量
    static inline constexpr int PROTOCOL_VERSION = 42;
    static inline constexpr const char* DRIVER_NAME = "ModernDB";
};

// 模板类的内联静态成员
template<typename T>
class ResourcePool {
public:
    static inline size_t max_size = 1000;
    static inline std::vector<std::unique_ptr<T>> available_resources;
    static inline size_t created_count = 0;
    
    static std::unique_ptr<T> acquire() {
        if (!available_resources.empty()) {
            auto resource = std::move(available_resources.back());
            available_resources.pop_back();
            return resource;
        }
        
        if (created_count >= max_size) {
            return nullptr;  // 资源池已满
        }
        
        ++created_count;
        return std::make_unique<T>();
    }
    
    static void release(std::unique_ptr<T> resource) {
        if (resource && available_resources.size() < max_size / 2) {
            available_resources.push_back(std::move(resource));
        }
    }
    
    static size_t available_count() { return available_resources.size(); }
    static size_t total_created() { return created_count; }
};

void demonstrate_static_member_initialization() {
    std::cout << "=== 静态成员初始化演示 ===\n";
    
    // 访问内联静态成员
    std::cout << "数据库配置:\n";
    std::cout << "  最大连接数: " << DatabaseConfig::max_connections << "\n";
    std::cout << "  超时时间: " << DatabaseConfig::timeout_seconds << "s\n";
    std::cout << "  启用日志: " << (DatabaseConfig::enable_logging ? "是" : "否") << "\n";
    std::cout << "  默认主机: " << DatabaseConfig::default_host << "\n";
    std::cout << "  协议版本: " << DatabaseConfig::PROTOCOL_VERSION << "\n";
    std::cout << "  驱动名称: " << DatabaseConfig::DRIVER_NAME << "\n";
    
    std::cout << "支持的端口: ";
    for (const auto& port : DatabaseConfig::supported_ports) {
        std::cout << port << " ";
    }
    std::cout << "\n";
    
    std::cout << "连接参数:\n";
    for (const auto& [key, value] : DatabaseConfig::connection_params) {
        std::cout << "  " << key << ": " << value << "\n";
    }
    
    // 使用静态方法
    try {
        int conn1 = DatabaseConfig::create_connection();
        int conn2 = DatabaseConfig::create_connection();
        std::cout << "创建连接: " << conn1 << ", " << conn2 << "\n";
        std::cout << "活动连接数: " << DatabaseConfig::get_active_connections() << "\n";
        
        DatabaseConfig::close_connection();
        std::cout << "关闭连接后活动数: " << DatabaseConfig::get_active_connections() << "\n";
    } catch (const std::exception& e) {
        std::cout << "连接错误: " << e.what() << "\n";
    }
    
    // 模板类的静态成员
    auto str_resource = ResourcePool<std::string>::acquire();
    std::cout << "字符串资源池创建数: " << ResourcePool<std::string>::total_created() << "\n";
    std::cout << "可用资源数: " << ResourcePool<std::string>::available_count() << "\n";
    
    ResourcePool<std::string>::release(std::move(str_resource));
    std::cout << "释放后可用资源数: " << ResourcePool<std::string>::available_count() << "\n";
    
    std::cout << "\n";
}

// ===== 4. 编译器实现机制演示 =====
// 演示内联变量的符号合并
namespace detail {
    inline int debug_counter = 0;
    inline const char* build_info = "Debug Build";
    
    struct DebugInfo {
        static inline std::string version = "1.0.0";
        static inline int build_number = 12345;
    };
}

// 多个翻译单元中包含此头文件时，这些内联变量只会有一个定义
void increment_debug_counter() {
    ++detail::debug_counter;
}

int get_debug_counter() {
    return detail::debug_counter;
}

// 内联变量的地址检查（证明是同一个对象）
const void* get_counter_address() {
    return &detail::debug_counter;
}

const void* get_build_info_address() {
    return detail::build_info;
}

void demonstrate_compiler_implementation() {
    std::cout << "=== 编译器实现机制演示 ===\n";
    
    // 初始状态
    std::cout << "初始调试计数器: " << get_debug_counter() << "\n";
    std::cout << "构建信息: " << detail::build_info << "\n";
    std::cout << "版本: " << detail::DebugInfo::version << "\n";
    std::cout << "构建号: " << detail::DebugInfo::build_number << "\n";
    
    // 修改计数器
    increment_debug_counter();
    increment_debug_counter();
    std::cout << "递增后计数器: " << get_debug_counter() << "\n";
    
    // 地址验证（同一翻译单元内必然相同，多翻译单元时链接器确保唯一性）
    std::cout << "计数器地址: " << get_counter_address() << "\n";
    std::cout << "构建信息地址: " << get_build_info_address() << "\n";
    std::cout << "调试版本地址: " << &detail::DebugInfo::version << "\n";
    
    std::cout << "\n";
}

// ===== 5. 性能和内存优化演示 =====
// 传统方式的性能问题模拟
namespace traditional {
    // 需要在每个使用的翻译单元中定义
    const std::vector<int>& get_constant_data() {
        static const std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        return data;
    }
    
    const std::map<std::string, double>& get_constant_map() {
        static const std::map<std::string, double> constants = {
            {"pi", 3.141592653589793},
            {"e", 2.718281828459045},
            {"phi", 1.618033988749895}
        };
        return constants;
    }
}

// C++17内联变量方式
namespace modern {
    inline const std::vector<int> constant_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    inline const std::map<std::string, double> constant_map = {
        {"pi", 3.141592653589793},
        {"e", 2.718281828459045},
        {"phi", 1.618033988749895}
    };
}

// 性能测试
template<typename Func>
auto measure_access_time(Func func, const std::string& name, int iterations = 1000000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto result = func();
        (void)result;  // 防止编译器优化
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << name << "访问耗时: " << duration.count() << " 微秒\n";
    return duration.count();
}

void demonstrate_performance_optimization() {
    std::cout << "=== 性能和内存优化演示 ===\n";
    
    // 内存使用分析
    std::cout << "内存使用分析:\n";
    std::cout << "内联vector大小: " << modern::constant_data.size() << " 元素\n";
    std::cout << "内联map大小: " << modern::constant_map.size() << " 元素\n";
    
    // 访问性能测试
    auto traditional_time = measure_access_time(
        []() { return traditional::get_constant_data().size(); },
        "传统方式",
        100000
    );
    
    auto modern_time = measure_access_time(
        []() { return modern::constant_data.size(); },
        "内联变量",
        100000
    );
    
    std::cout << "性能提升比例: " << (traditional_time / (double)modern_time) << "x\n";
    
    // 地址比较（证明是真正的全局单一实例）
    std::cout << "内联vector地址: " << &modern::constant_data << "\n";
    std::cout << "内联map地址: " << &modern::constant_map << "\n";
    
    // 编译期常量的优化
    constexpr auto compile_time_size = modern::constant_data.size();  // 编译期确定
    std::cout << "编译期确定的大小: " << compile_time_size << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 内联变量深度解析\n";
    std::cout << "=======================\n";
    
    demonstrate_global_inline_variables();
    demonstrate_template_inline_variables();
    demonstrate_static_member_initialization();
    demonstrate_compiler_implementation();
    demonstrate_performance_optimization();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 06_inline_variables.cpp -o inline_variables
./inline_variables

关键学习点:
1. 内联变量解决了头文件中全局变量定义的ODR问题
2. 模板变量的内联化避免了多翻译单元的重复定义
3. 静态成员可以在类内直接初始化，简化了代码结构
4. 编译器通过符号合并确保内联变量的唯一性
5. 相比传统方法，内联变量提供了更好的性能和内存效率

注意事项:
- 内联变量在所有翻译单元中必须有相同的定义
- 模板的内联静态成员在模板实例化时才会生成
- 内联变量的地址在程序运行期间是固定不变的
- 与constexpr结合使用可以获得编译期优化
*/