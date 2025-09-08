/**
 * C++11 强类型枚举(Scoped Enums)深度解析
 * 
 * 核心概念：
 * 1. enum class的类型安全和作用域隔离
 * 2. 底层类型的显式指定和内存优化
 * 3. 与传统enum的兼容性和迁移策略
 * 4. 枚举在模板编程中的应用
 * 5. 类型安全的位操作和枚举组合
 */

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <bitset>

// ===== 1. 传统枚举的问题演示 =====

// 传统枚举的问题
enum TraditionalColor {
    RED,
    GREEN, 
    BLUE
};

enum TraditionalState {
    RED,     // 编译错误！与TraditionalColor::RED冲突
    ACTIVE,
    INACTIVE
};

// 解决冲突的方法：使用命名空间或前缀
namespace Colors {
    enum Type {
        Red,
        Green,
        Blue
    };
}

enum NetworkState {
    NS_CONNECTED,
    NS_CONNECTING, 
    NS_DISCONNECTED
};

void demonstrate_traditional_enum_problems() {
    std::cout << "=== 传统枚举问题演示 ===\n";
    
    // 问题1：名称污染全局作用域
    // TraditionalColor color = RED;  // OK
    // TraditionalState state = RED;  // 编译错误：重定义
    
    // 问题2：隐式转换为整数
    TraditionalColor color = GREEN;
    int color_value = color;  // 隐式转换，可能不是期望的行为
    std::cout << "传统枚举GREEN的整数值: " << color_value << std::endl;
    
    // 问题3：可以与整数进行比较
    if (color == 1) {  // 编译通过，但逻辑上可能不正确
        std::cout << "传统枚举可以与整数直接比较" << std::endl;
    }
    
    // 问题4：可以进行算术运算
    TraditionalColor next_color = static_cast<TraditionalColor>(color + 1);
    std::cout << "算术运算后的枚举值: " << next_color << std::endl;
    
    // 解决方案示例
    Colors::Type safe_color = Colors::Red;
    NetworkState net_state = NS_CONNECTED;
    
    std::cout << "使用命名空间的枚举: " << safe_color << std::endl;
    std::cout << "使用前缀的枚举: " << net_state << std::endl;
    
    std::cout << "\n";
}

// ===== 2. 强类型枚举的优势 =====

// 强类型枚举定义
enum class Color : uint8_t {  // 显式指定底层类型
    Red = 1,
    Green = 2,
    Blue = 4
};

enum class State : int {
    Red = 100,    // 不与Color::Red冲突！
    Active,
    Inactive
};

enum class Priority {
    Low,
    Medium, 
    High,
    Critical
};

// 不同的底层类型
enum class SmallEnum : char {
    A, B, C
};

enum class LargeEnum : long long {
    First = 1000000000000LL,
    Second
};

void demonstrate_scoped_enum_advantages() {
    std::cout << "=== 强类型枚举优势演示 ===\n";
    
    // 优势1：作用域安全，不污染全局命名空间
    Color color = Color::Red;
    State state = State::Active;
    
    std::cout << "可以同时使用Color::Red和State::Red，无冲突" << std::endl;
    
    // 优势2：不能隐式转换为整数
    // int color_int = color;  // 编译错误！
    int color_int = static_cast<int>(color);  // 必须显式转换
    std::cout << "Color::Red的底层值: " << color_int << std::endl;
    
    // 优势3：不能与整数直接比较
    // if (color == 1) {}  // 编译错误！
    if (color == Color::Red) {  // 必须与同类型比较
        std::cout << "类型安全的枚举比较" << std::endl;
    }
    
    // 优势4：不能进行意外的算术运算
    // Color next_color = color + 1;  // 编译错误！
    
    // 优势5：底层类型可控制
    std::cout << "不同底层类型的大小:\n";
    std::cout << "SmallEnum (char): " << sizeof(SmallEnum) << " bytes" << std::endl;
    std::cout << "Color (uint8_t): " << sizeof(Color) << " bytes" << std::endl;
    std::cout << "State (int): " << sizeof(State) << " bytes" << std::endl;
    std::cout << "LargeEnum (long long): " << sizeof(LargeEnum) << " bytes" << std::endl;
    
    // 优势6：前向声明支持（如果指定了底层类型）
    enum class ForwardDeclared : int;  // 可以前向声明
    
    std::cout << "\n";
}

// ===== 3. 枚举类型转换和实用工具 =====

// 枚举到字符串转换
template<typename EnumType>
std::string enum_to_string(EnumType value);

// Color的特化
template<>
std::string enum_to_string<Color>(Color value) {
    switch (value) {
        case Color::Red:   return "Red";
        case Color::Green: return "Green";  
        case Color::Blue:  return "Blue";
        default:           return "Unknown Color";
    }
}

// State的特化
template<>
std::string enum_to_string<State>(State value) {
    switch (value) {
        case State::Red:      return "Red State";
        case State::Active:   return "Active";
        case State::Inactive: return "Inactive";
        default:              return "Unknown State";
    }
}

// Priority的特化
template<>
std::string enum_to_string<Priority>(Priority value) {
    switch (value) {
        case Priority::Low:      return "Low";
        case Priority::Medium:   return "Medium";
        case Priority::High:     return "High";
        case Priority::Critical: return "Critical";
        default:                 return "Unknown Priority";
    }
}

// 字符串到枚举转换
template<typename EnumType>
EnumType string_to_enum(const std::string& str);

template<>
Color string_to_enum<Color>(const std::string& str) {
    if (str == "Red")   return Color::Red;
    if (str == "Green") return Color::Green;
    if (str == "Blue")  return Color::Blue;
    throw std::invalid_argument("Unknown color: " + str);
}

template<>
Priority string_to_enum<Priority>(const std::string& str) {
    if (str == "Low")      return Priority::Low;
    if (str == "Medium")   return Priority::Medium;
    if (str == "High")     return Priority::High;
    if (str == "Critical") return Priority::Critical;
    throw std::invalid_argument("Unknown priority: " + str);
}

// 枚举迭代器
template<typename EnumType, EnumType First, EnumType Last>
class enum_iterator {
private:
    using underlying_type = std::underlying_type_t<EnumType>;
    underlying_type current_;
    
public:
    explicit enum_iterator(EnumType start) 
        : current_(static_cast<underlying_type>(start)) {}
    
    EnumType operator*() const {
        return static_cast<EnumType>(current_);
    }
    
    enum_iterator& operator++() {
        ++current_;
        return *this;
    }
    
    bool operator!=(const enum_iterator& other) const {
        return current_ != other.current_;
    }
    
    static enum_iterator begin() {
        return enum_iterator(First);
    }
    
    static enum_iterator end() {
        return enum_iterator(static_cast<EnumType>(
            static_cast<underlying_type>(Last) + 1
        ));
    }
};

void demonstrate_enum_utilities() {
    std::cout << "=== 枚举实用工具演示 ===\n";
    
    // 枚举到字符串转换
    std::cout << "枚举到字符串转换:\n";
    Color color = Color::Blue;
    State state = State::Active;
    Priority priority = Priority::High;
    
    std::cout << "Color::Blue -> " << enum_to_string(color) << std::endl;
    std::cout << "State::Active -> " << enum_to_string(state) << std::endl;
    std::cout << "Priority::High -> " << enum_to_string(priority) << std::endl;
    
    // 字符串到枚举转换
    std::cout << "\n字符串到枚举转换:\n";
    try {
        Color parsed_color = string_to_enum<Color>("Red");
        std::cout << "\"Red\" -> " << enum_to_string(parsed_color) << std::endl;
        
        Priority parsed_priority = string_to_enum<Priority>("Critical");
        std::cout << "\"Critical\" -> " << enum_to_string(parsed_priority) << std::endl;
        
        // 错误情况
        Color invalid_color = string_to_enum<Color>("Purple");
    } catch (const std::exception& e) {
        std::cout << "转换错误: " << e.what() << std::endl;
    }
    
    // 枚举迭代
    std::cout << "\n枚举迭代:\n";
    using PriorityIter = enum_iterator<Priority, Priority::Low, Priority::Critical>;
    
    std::cout << "所有优先级: ";
    for (auto it = PriorityIter::begin(); it != PriorityIter::end(); ++it) {
        std::cout << enum_to_string(*it) << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n";
}

// ===== 4. 位标志枚举的设计模式 =====

enum class Permission : unsigned int {
    None    = 0,
    Read    = 1 << 0,    // 0001
    Write   = 1 << 1,    // 0010  
    Execute = 1 << 2,    // 0100
    Delete  = 1 << 3,    // 1000
    
    // 组合权限
    ReadWrite = Read | Write,
    ReadExecute = Read | Execute,
    All = Read | Write | Execute | Delete
};

// 位操作符重载
constexpr Permission operator|(Permission lhs, Permission rhs) {
    using underlying = std::underlying_type_t<Permission>;
    return static_cast<Permission>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
    );
}

constexpr Permission operator&(Permission lhs, Permission rhs) {
    using underlying = std::underlying_type_t<Permission>;
    return static_cast<Permission>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs)
    );
}

constexpr Permission operator^(Permission lhs, Permission rhs) {
    using underlying = std::underlying_type_t<Permission>;
    return static_cast<Permission>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs)
    );
}

constexpr Permission operator~(Permission perm) {
    using underlying = std::underlying_type_t<Permission>;
    return static_cast<Permission>(~static_cast<underlying>(perm));
}

Permission& operator|=(Permission& lhs, Permission rhs) {
    return lhs = lhs | rhs;
}

Permission& operator&=(Permission& lhs, Permission rhs) {
    return lhs = lhs & rhs;
}

Permission& operator^=(Permission& rhs, Permission lhs) {
    return rhs = rhs ^ lhs;
}

// 权限检查函数
bool has_permission(Permission permissions, Permission check) {
    return (permissions & check) == check;
}

// 权限转字符串
std::string permission_to_string(Permission perm) {
    if (perm == Permission::None) return "None";
    
    std::string result;
    if (has_permission(perm, Permission::Read))    result += "Read ";
    if (has_permission(perm, Permission::Write))   result += "Write ";
    if (has_permission(perm, Permission::Execute)) result += "Execute ";
    if (has_permission(perm, Permission::Delete))  result += "Delete ";
    
    if (!result.empty()) {
        result.pop_back();  // 移除最后的空格
    }
    
    return result.empty() ? "None" : result;
}

void demonstrate_bitflag_enums() {
    std::cout << "=== 位标志枚举演示 ===\n";
    
    // 创建权限组合
    Permission user_perm = Permission::Read | Permission::Write;
    Permission admin_perm = Permission::All;
    
    std::cout << "用户权限: " << permission_to_string(user_perm) << std::endl;
    std::cout << "管理员权限: " << permission_to_string(admin_perm) << std::endl;
    
    // 权限检查
    std::cout << "\n权限检查:\n";
    std::cout << "用户有读权限: " << (has_permission(user_perm, Permission::Read) ? "是" : "否") << std::endl;
    std::cout << "用户有执行权限: " << (has_permission(user_perm, Permission::Execute) ? "是" : "否") << std::endl;
    std::cout << "管理员有删除权限: " << (has_permission(admin_perm, Permission::Delete) ? "是" : "否") << std::endl;
    
    // 权限操作
    std::cout << "\n权限操作:\n";
    Permission modified_perm = user_perm;
    std::cout << "原始权限: " << permission_to_string(modified_perm) << std::endl;
    
    // 添加权限
    modified_perm |= Permission::Execute;
    std::cout << "添加执行权限: " << permission_to_string(modified_perm) << std::endl;
    
    // 移除权限
    modified_perm &= ~Permission::Write;
    std::cout << "移除写权限: " << permission_to_string(modified_perm) << std::endl;
    
    // 切换权限
    modified_perm ^= Permission::Delete;
    std::cout << "切换删除权限: " << permission_to_string(modified_perm) << std::endl;
    
    // 位表示
    std::cout << "\n位表示:\n";
    auto show_bits = [](Permission perm) {
        std::bitset<8> bits(static_cast<unsigned int>(perm));
        std::cout << permission_to_string(perm) << ": " << bits << std::endl;
    };
    
    show_bits(Permission::Read);
    show_bits(Permission::Write);
    show_bits(Permission::Execute);
    show_bits(Permission::ReadWrite);
    show_bits(admin_perm);
    
    std::cout << "\n";
}

// ===== 5. 枚举在状态机中的应用 =====

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
    Failed
};

enum class ConnectionEvent {
    Connect,
    ConnectionEstablished,
    ConnectionLost,
    Reconnect,
    Disconnect,
    Error
};

class StateMachine {
private:
    ConnectionState current_state_;
    std::unordered_map<std::pair<ConnectionState, ConnectionEvent>, ConnectionState, 
                       std::hash<std::pair<int, int>>> transitions_;
    
    // 自定义哈希函数的辅助结构
    struct PairHash {
        size_t operator()(const std::pair<ConnectionState, ConnectionEvent>& p) const {
            auto h1 = std::hash<int>{}(static_cast<int>(p.first));
            auto h2 = std::hash<int>{}(static_cast<int>(p.second));
            return h1 ^ (h2 << 1);
        }
    };
    
    std::unordered_map<std::pair<ConnectionState, ConnectionEvent>, 
                       ConnectionState, PairHash> state_transitions_;
    
public:
    StateMachine() : current_state_(ConnectionState::Disconnected) {
        // 定义状态转换表
        state_transitions_[{ConnectionState::Disconnected, ConnectionEvent::Connect}] = 
            ConnectionState::Connecting;
        state_transitions_[{ConnectionState::Connecting, ConnectionEvent::ConnectionEstablished}] = 
            ConnectionState::Connected;
        state_transitions_[{ConnectionState::Connecting, ConnectionEvent::Error}] = 
            ConnectionState::Failed;
        state_transitions_[{ConnectionState::Connected, ConnectionEvent::ConnectionLost}] = 
            ConnectionState::Reconnecting;
        state_transitions_[{ConnectionState::Connected, ConnectionEvent::Disconnect}] = 
            ConnectionState::Disconnected;
        state_transitions_[{ConnectionState::Reconnecting, ConnectionEvent::ConnectionEstablished}] = 
            ConnectionState::Connected;
        state_transitions_[{ConnectionState::Reconnecting, ConnectionEvent::Error}] = 
            ConnectionState::Failed;
        state_transitions_[{ConnectionState::Failed, ConnectionEvent::Connect}] = 
            ConnectionState::Connecting;
    }
    
    ConnectionState get_state() const { return current_state_; }
    
    bool process_event(ConnectionEvent event) {
        auto key = std::make_pair(current_state_, event);
        auto it = state_transitions_.find(key);
        
        if (it != state_transitions_.end()) {
            ConnectionState old_state = current_state_;
            current_state_ = it->second;
            
            std::cout << "状态转换: " << state_to_string(old_state) 
                      << " -> " << state_to_string(current_state_)
                      << " (事件: " << event_to_string(event) << ")" << std::endl;
            return true;
        } else {
            std::cout << "无效的状态转换: " << state_to_string(current_state_)
                      << " + " << event_to_string(event) << std::endl;
            return false;
        }
    }
    
private:
    std::string state_to_string(ConnectionState state) const {
        switch (state) {
            case ConnectionState::Disconnected: return "Disconnected";
            case ConnectionState::Connecting:   return "Connecting";
            case ConnectionState::Connected:    return "Connected";
            case ConnectionState::Reconnecting: return "Reconnecting";
            case ConnectionState::Failed:       return "Failed";
            default:                            return "Unknown";
        }
    }
    
    std::string event_to_string(ConnectionEvent event) const {
        switch (event) {
            case ConnectionEvent::Connect:               return "Connect";
            case ConnectionEvent::ConnectionEstablished: return "ConnectionEstablished";
            case ConnectionEvent::ConnectionLost:        return "ConnectionLost";
            case ConnectionEvent::Reconnect:            return "Reconnect";
            case ConnectionEvent::Disconnect:           return "Disconnect";
            case ConnectionEvent::Error:                return "Error";
            default:                                    return "Unknown";
        }
    }
};

void demonstrate_state_machine_enums() {
    std::cout << "=== 状态机中的枚举应用演示 ===\n";
    
    StateMachine machine;
    
    std::cout << "初始状态: " << static_cast<int>(machine.get_state()) << std::endl;
    
    // 模拟连接过程
    std::vector<ConnectionEvent> events = {
        ConnectionEvent::Connect,
        ConnectionEvent::ConnectionEstablished,
        ConnectionEvent::ConnectionLost,
        ConnectionEvent::ConnectionEstablished,
        ConnectionEvent::Disconnect,
        ConnectionEvent::Connect,
        ConnectionEvent::Error,
        ConnectionEvent::Connect,
        ConnectionEvent::ConnectionEstablished
    };
    
    for (auto event : events) {
        machine.process_event(event);
    }
    
    std::cout << "\n";
}

// ===== 6. 模板中的枚举类型检测 =====

// 检测是否为枚举类型
template<typename T>
constexpr bool is_scoped_enum_v = std::is_enum<T>::value && !std::is_convertible<T, int>::value;

// 枚举的通用处理函数
template<typename EnumType>
std::enable_if_t<std::is_enum<EnumType>::value, std::underlying_type_t<EnumType>>
to_underlying(EnumType e) {
    return static_cast<std::underlying_type_t<EnumType>>(e);
}

// 安全的枚举递增
template<typename EnumType>
std::enable_if_t<std::is_enum<EnumType>::value, EnumType>
next_enum(EnumType current, EnumType max_value) {
    auto underlying = to_underlying(current);
    auto max_underlying = to_underlying(max_value);
    
    if (underlying < max_underlying) {
        return static_cast<EnumType>(underlying + 1);
    }
    return current;  // 已达到最大值
}

// 枚举范围检查
template<typename EnumType>
std::enable_if_t<std::is_enum<EnumType>::value, bool>
in_range(EnumType value, EnumType min_val, EnumType max_val) {
    auto val_underlying = to_underlying(value);
    auto min_underlying = to_underlying(min_val);
    auto max_underlying = to_underlying(max_val);
    
    return val_underlying >= min_underlying && val_underlying <= max_underlying;
}

void demonstrate_template_enum_handling() {
    std::cout << "=== 模板中的枚举处理演示 ===\n";
    
    // 类型检测
    std::cout << "类型检测:\n";
    std::cout << "Priority是枚举: " << std::is_enum<Priority>::value << std::endl;
    std::cout << "Priority是强类型枚举: " << is_scoped_enum_v<Priority> << std::endl;
    std::cout << "int是枚举: " << std::is_enum<int>::value << std::endl;
    
    // 底层值获取
    std::cout << "\n底层值转换:\n";
    Priority p = Priority::Medium;
    std::cout << "Priority::Medium的底层值: " << to_underlying(p) << std::endl;
    
    Color c = Color::Blue;
    std::cout << "Color::Blue的底层值: " << static_cast<int>(to_underlying(c)) << std::endl;
    
    // 枚举递增
    std::cout << "\n枚举递增:\n";
    Priority current = Priority::Low;
    while (current != Priority::Critical) {
        std::cout << enum_to_string(current) << " -> ";
        current = next_enum(current, Priority::Critical);
        std::cout << enum_to_string(current) << std::endl;
    }
    
    // 范围检查
    std::cout << "\n范围检查:\n";
    std::cout << "Priority::Medium在Low-High范围内: " 
              << in_range(Priority::Medium, Priority::Low, Priority::High) << std::endl;
    std::cout << "Priority::Critical在Low-High范围内: " 
              << in_range(Priority::Critical, Priority::Low, Priority::High) << std::endl;
    
    std::cout << "\n";
}

// ===== 7. 最佳实践和性能考虑 =====

void demonstrate_best_practices() {
    std::cout << "=== 强类型枚举最佳实践 ===\n";
    
    std::cout << "1. 设计原则:\n";
    std::cout << "✓ 优先使用enum class而不是传统enum\n";
    std::cout << "✓ 显式指定底层类型以控制内存占用\n";
    std::cout << "✓ 使用有意义的名称和明确的值\n";
    std::cout << "✓ 为位标志枚举重载位操作符\n";
    std::cout << "✓ 提供枚举到字符串的转换函数\n";
    
    std::cout << "\n2. 性能考虑:\n";
    std::cout << "• 枚举是零开销抽象，编译后与整数相同\n";
    std::cout << "• 选择合适的底层类型可以优化内存使用\n";
    std::cout << "• switch语句通常会被优化为跳转表\n";
    std::cout << "• 编译期计算使用constexpr\n";
    
    // 内存占用对比
    std::cout << "\n3. 内存占用优化示例:\n";
    
    enum class TinyEnum : uint8_t { A, B, C };
    enum class RegularEnum { A, B, C };
    enum class LargeEnum : uint64_t { A, B, C };
    
    std::cout << "TinyEnum (uint8_t): " << sizeof(TinyEnum) << " byte" << std::endl;
    std::cout << "RegularEnum (default): " << sizeof(RegularEnum) << " bytes" << std::endl;
    std::cout << "LargeEnum (uint64_t): " << sizeof(LargeEnum) << " bytes" << std::endl;
    
    // 在结构体中的影响
    struct WithTinyEnum {
        TinyEnum e;
        char c;
    };
    
    struct WithRegularEnum {
        RegularEnum e;
        char c;
    };
    
    std::cout << "\n结构体大小对比:\n";
    std::cout << "WithTinyEnum: " << sizeof(WithTinyEnum) << " bytes" << std::endl;
    std::cout << "WithRegularEnum: " << sizeof(WithRegularEnum) << " bytes" << std::endl;
    
    std::cout << "\n4. 迁移策略:\n";
    std::cout << "• 逐步将传统enum替换为enum class\n";
    std::cout << "• 使用类型别名保持API兼容性\n";
    std::cout << "• 提供转换函数协助迁移\n";
    std::cout << "• 在新代码中一律使用enum class\n";
    
    std::cout << "\n5. 常见陷阱:\n";
    std::cout << "⚠ 不要忘记指定底层类型（如果内存敏感）\n";
    std::cout << "⚠ 注意位操作符的正确实现\n";
    std::cout << "⚠ 避免在switch中遗漏case\n";
    std::cout << "⚠ 小心枚举值的连续性假设\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====

int main() {
    std::cout << "C++11 强类型枚举深度解析\n";
    std::cout << "=========================\n";
    
    // 传统枚举问题
    demonstrate_traditional_enum_problems();
    
    // 强类型枚举优势
    demonstrate_scoped_enum_advantages();
    
    // 实用工具
    demonstrate_enum_utilities();
    
    // 位标志应用
    demonstrate_bitflag_enums();
    
    // 状态机应用
    demonstrate_state_machine_enums();
    
    // 模板处理
    demonstrate_template_enum_handling();
    
    // 最佳实践
    demonstrate_best_practices();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++11 -O2 -Wall 10_scoped_enums.cpp -o scoped_enums_demo
./scoped_enums_demo

关键学习点:
1. 理解强类型枚举相比传统枚举的优势
2. 掌握底层类型指定对内存和性能的影响
3. 学会设计类型安全的位标志操作
4. 了解枚举在状态机设计中的应用
5. 掌握枚举的字符串转换和实用工具设计
6. 理解枚举在模板编程中的类型检测
7. 学会枚举相关的最佳实践和迁移策略
8. 避免常见的枚举使用陷阱

注意事项:
- 优先使用enum class获得类型安全
- 根据取值范围选择合适的底层类型
- 为复杂应用场景设计配套的实用函数
- 注意编译器优化对枚举性能的影响
*/