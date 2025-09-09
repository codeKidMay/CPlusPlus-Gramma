/*
 * C++14 类型别名模板深度解析
 * 
 * C++14引入了类型别名模板（Alias Templates），这是模板编程的重要改进：
 * 1. 模板类型别名 - 使用using声明创建模板别名
 * 2. 标准库类型特征别名 - _t后缀的简化形式
 * 3. SFINAE应用 - 更清晰的enable_if写法
 * 4. 模板元编程 - 简化元函数的写法
 * 5. 策略模式 - 通过类型别名实现策略选择
 * 
 * 类型别名模板的核心价值：
 * - 简化复杂的模板类型表达式
 * - 提高模板代码的可读性和维护性
 * - 减少模板实例化的编译时间
 * - 使模板编程更加直观和优雅
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <complex>
#include <array>
#include <map>
#include <set>
#include <deque>
#include <list>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>

namespace cpp14_alias_templates {

// ===== 1. 基础类型别名模板 =====

namespace BasicAliases {
    // 容器类型别名
    template<typename T>
    using Vector = std::vector<T>;
    
    template<typename T>
    using List = std::list<T>;
    
    template<typename T>
    using Deque = std::deque<T>;
    
    template<typename T>
    using Set = std::set<T>;
    
    template<typename Key, typename Value>
    using Map = std::map<Key, Value>;
    
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    
    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
    
    // 复杂数值类型别名
    template<typename T>
    using Complex = std::complex<T>;
    
    template<typename T, size_t Size>
    using Array = std::array<T, Size>;
    
    // 函数类型别名
    template<typename T>
    using Comparator = bool(*)(const T&, const T&);
    
    template<typename T>
    using UnaryFunction = void(*)(T);
    
    template<typename T, typename U>
    using BinaryFunction = void(*)(T, U);
    
    // 元组类型别名
    template<typename... Types>
    using Tuple = std::tuple<Types...>;
    
    // 配置元组
    template<typename T>
    using ConfigEntry = std::tuple<std::string, T, std::string>;  // name, value, description
    
    // 容器演示
    void demonstrate_containers() {
        Vector<int> numbers = {1, 2, 3, 4, 5};
        Set<std::string> names = {"Alice", "Bob", "Charlie"};
        Map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}, {"Charlie", 92}};
        
        std::cout << "数字容器: ";
        for (const auto& num : numbers) {
            std::cout << num << " ";
        }
        std::cout << "\n";
        
        std::cout << "姓名集合: ";
        for (const auto& name : names) {
            std::cout << name << " ";
        }
        std::cout << "\n";
        
        std::cout << "分数映射:\n";
        for (const auto& [name, score] : scores) {
            std::cout << "  " << name << ": " << score << "\n";
        }
    }
}

// ===== 2. 标准库类型特征别名 =====

namespace StdTraitsAliases {
    // C++14标准库类型特征别名
    template<typename T>
    using RemoveConst = typename std::remove_const<T>::type;
    
    template<typename T>
    using RemoveConst_t = std::remove_const_t<T>;
    
    template<typename T>
    using RemoveVolatile = typename std::remove_volatile<T>::type;
    
    template<typename T>
    using RemoveVolatile_t = std::remove_volatile_t<T>;
    
    template<typename T>
    using RemoveCV = typename std::remove_cv<T>::type;
    
    template<typename T>
    using RemoveCV_t = std::remove_cv_t<T>;
    
    template<typename T>
    using AddConst = typename std::add_const<T>::type;
    
    template<typename T>
    using AddConst_t = std::add_const_t<T>;
    
    template<typename T>
    using AddVolatile = typename std::add_volatile<T>::type;
    
    template<typename T>
    using AddVolatile_t = std::add_volatile_t<T>;
    
    template<typename T>
    using AddCV = typename std::add_cv<T>::type;
    
    template<typename T>
    using AddCV_t = std::add_cv_t<T>;
    
    // 引用相关
    template<typename T>
    using RemoveReference = typename std::remove_reference<T>::type;
    
    template<typename T>
    using RemoveReference_t = std::remove_reference_t<T>;
    
    template<typename T>
    using AddLValueReference = typename std::add_lvalue_reference<T>::type;
    
    template<typename T>
    using AddLValueReference_t = std::add_lvalue_reference_t<T>;
    
    template<typename T>
    using AddRValueReference = typename std::add_rvalue_reference<T>::type;
    
    template<typename T>
    using AddRValueReference_t = std::add_rvalue_reference_t<T>;
    
    // 指针相关
    template<typename T>
    using RemovePointer = typename std::remove_pointer<T>::type;
    
    template<typename T>
    using RemovePointer_t = std::remove_pointer_t<T>;
    
    template<typename T>
    using AddPointer = typename std::add_pointer<T>::type;
    
    template<typename T>
    using AddPointer_t = std::add_pointer_t<T>;
    
    // 符号相关
    template<typename T>
    using MakeSigned = typename std::make_signed<T>::type;
    
    template<typename T>
    using MakeSigned_t = std::make_signed_t<T>;
    
    template<typename T>
    using MakeUnsigned = typename std::make_unsigned<T>::type;
    
    template<typename T>
    using MakeUnsigned_t = std::make_unsigned_t<T>;
    
    // 数组相关
    template<typename T>
    using RemoveExtent = typename std::remove_extent<T>::type;
    
    template<typename T>
    using RemoveExtent_t = std::remove_extent_t<T>;
    
    template<typename T>
    using RemoveAllExtents = typename std::remove_all_extents<T>::type;
    
    template<typename T>
    using RemoveAllExtents_t = std::remove_all_extents_t<T>;
    
    // 类型转换
    template<typename T>
    using Decay = typename std::decay<T>::type;
    
    template<typename T>
    using Decay_t = std::decay_t<T>;
    
    // 条件选择
    template<bool B, typename T, typename F>
    using Conditional = typename std::conditional<B, T, F>::type;
    
    template<bool B, typename T, typename F>
    using Conditional_t = std::conditional_t<B, T, F>;
    
    // 启用_if
    template<bool B, typename T = void>
    using EnableIf = typename std::enable_if<B, T>::type;
    
    template<bool B, typename T = void>
    using EnableIf_t = std::enable_if_t<B, T>;
    
    // 公共类型
    template<typename... Types>
    using CommonType = typename std::common_type<Types...>::type;
    
    template<typename... Types>
    using CommonType_t = std::common_type_t<Types...>;
    
    // 底层类型
    template<typename T>
    using UnderlyingType = typename std::underlying_type<T>::type;
    
    template<typename T>
    using UnderlyingType_t = std::underlying_type_t<T>;
    
    // 类型特征比较演示
    template<typename T>
    void demonstrate_type_traits() {
        std::cout << "类型分析: " << typeid(T).name() << "\n";
        std::cout << "  原始类型: " << typeid(RemoveCV_t<T>).name() << "\n";
        std::cout << "  去引用类型: " << typeid(RemoveReference_t<T>).name() << "\n";
        std::cout << "  衰减类型: " << typeid(Decay_t<T>).name() << "\n";
        std::cout << "  指针类型: " << typeid(AddPointer_t<T>).name() << "\n";
        std::cout << "  const类型: " << typeid(AddConst_t<T>).name() << "\n";
        std::cout << "  有符号版本: " << typeid(MakeSigned_t<T>).name() << "\n";
        std::cout << "  无符号版本: " << typeid(MakeUnsigned_t<T>).name() << "\n";
    }
}

// ===== 3. SFINAE和类型约束 =====

namespace SFINAEExamples {
    // 传统写法 vs C++14别名模板写法
    
    // 传统写法
    template<typename T, typename Enable = typename std::enable_if<std::is_integral<T>::value>::type>
    T add_numbers_old(T a, T b) {
        return a + b;
    }
    
    // C++14别名模板写法
    template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    T add_numbers_new(T a, T b) {
        return a + b;
    }
    
    // 更简洁的写法
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, T> add_numbers_simple(T a, T b) {
        return a + b;
    }
    
    // 类型安全的容器操作
    template<typename Container>
    using ValueType_t = typename Container::value_type;
    
    template<typename Container>
    using SizeType_t = typename Container::size_type;
    
    template<typename Container>
    using Iterator_t = typename Container::iterator;
    
    template<typename Container>
    using ConstIterator_t = typename Container::const_iterator;
    
    // 容器元素类型约束
    template<typename Container>
    void print_container(const Container& container) {
        static_assert(std::is_same_v<ValueType_t<Container>, 
                                       typename Container::value_type>, 
                     "值类型不匹配");
        
        std::cout << "容器内容: ";
        for (const auto& item : container) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
    
    // 数值类型约束
    template<typename T>
    using IsIntegral = std::enable_if_t<std::is_integral_v<T>, bool>;
    
    template<typename T>
    using IsFloatingPoint = std::enable_if_t<std::is_floating_point_v<T>, bool>;
    
    template<typename T>
    using IsArithmetic = std::enable_if_t<std::is_arithmetic_v<T>, bool>;
    
    template<typename T, IsIntegral<T> = true>
    T safe_add_integral(T a, T b) {
        if (a > 0 && b > std::numeric_limits<T>::max() - a) {
            throw std::overflow_error("整数加法溢出");
        }
        if (a < 0 && b < std::numeric_limits<T>::min() - a) {
            throw std::underflow_error("整数加法下溢");
        }
        return a + b;
    }
    
    template<typename T, IsFloatingPoint<T> = true>
    T safe_add_floating(T a, T b) {
        return a + b;  // 浮点数有内置的溢出处理
    }
    
    // 容器类型约束
    template<typename Container>
    using IsSequenceContainer = std::enable_if_t<
        std::is_same_v<Container, std::vector<typename Container::value_type>> ||
        std::is_same_v<Container, std::list<typename Container::value_type>> ||
        std::is_same_v<Container, std::deque<typename Container::value_type>>,
        bool
    >;
    
    template<typename Container>
    using IsAssociativeContainer = std::enable_if_t<
        std::is_same_v<Container, std::map<typename Container::key_type, typename Container::mapped_type>> ||
        std::is_same_v<Container, std::set<typename Container::key_type>>,
        bool
    >;
    
    template<typename Container, IsSequenceContainer<Container> = true>
    void process_sequence_container(const Container& container) {
        std::cout << "处理序列容器，大小: " << container.size() << "\n";
    }
    
    template<typename Container, IsAssociativeContainer<Container> = true>
    void process_associative_container(const Container& container) {
        std::cout << "处理关联容器，大小: " << container.size() << "\n";
    }
}

// ===== 4. 模板元编程 =====

namespace TemplateMetaprogramming {
    // 类型列表
    template<typename... Types>
    struct TypeList {
        using type = TypeList<Types...>;
        static constexpr size_t size = sizeof...(Types);
    };
    
    // 类型列表操作
    template<typename List>
    using Front = typename List::front;
    
    template<typename List>
    using PopFront = typename List::pop_front;
    
    template<typename List>
    using PushFront = typename List::push_front;
    
    template<typename List>
    using PushBack = typename List::push_back;
    
    // 具体类型列表定义
    struct EmptyList {};
    
    template<typename Head, typename Tail = EmptyList>
    struct Cons {
        using front = Head;
        using pop_front = Tail;
        static constexpr size_t size = 1 + Tail::size;
    };
    
    template<typename Head>
    struct Cons<Head, EmptyList> {
        using front = Head;
        using pop_front = EmptyList;
        static constexpr size_t size = 1;
    };
    
    // 类型查找
    template<typename List, typename T>
    struct Contains;
    
    template<typename T, typename... Types>
    struct Contains<TypeList<Types...>, T> 
        : std::disjunction<std::is_same<T, Types>...> {};
    
    template<typename List, typename T>
    using Contains_t = typename Contains<List, T>::type;
    
    template<typename List, typename T>
    constexpr bool Contains_v = Contains<List, T>::value;
    
    // 类型索引
    template<typename List, typename T, size_t Index = 0>
    struct TypeIndex;
    
    template<typename T, typename First, typename... Rest, size_t Index>
    struct TypeIndex<TypeList<First, Rest...>, T, Index> {
        static constexpr size_t value = 
            std::is_same_v<T, First> ? Index : TypeIndex<TypeList<Rest...>, T, Index + 1>::value;
    };
    
    template<typename T, size_t Index>
    struct TypeIndex<TypeList<>, T, Index> {
        static constexpr size_t value = -1;
    };
    
    template<typename List, typename T>
    constexpr size_t TypeIndex_v = TypeIndex<List, T>::value;
    
    // 类型选择
    template<typename List, size_t Index>
    struct TypeAt;
    
    template<typename First, typename... Rest, size_t Index>
    struct TypeAt<TypeList<First, Rest...>, Index> {
        using type = typename TypeAt<TypeList<Rest...>, Index - 1>::type;
    };
    
    template<typename First, typename... Rest>
    struct TypeAt<TypeList<First, Rest...>, 0> {
        using type = First;
    };
    
    template<typename List, size_t Index>
    using TypeAt_t = typename TypeAt<List, Index>::type;
    
    // 函数签名类型别名
    template<typename ReturnType, typename... Args>
    using FunctionType = ReturnType(Args...);
    
    template<typename ReturnType, typename... Args>
    using FunctionPointer = ReturnType(*)(Args...);
    
    template<typename ReturnType, typename... Args>
    using MemberFunctionPointer = ReturnType(*)(Args...);
    
    // 策略类型别名
    template<typename T>
    using LessComparator = std::less<T>;
    
    template<typename T>
    using GreaterComparator = std::greater<T>;
    
    template<typename T>
    using EqualComparator = std::equal_to<T>;
    
    // 容器策略
    template<typename T>
    using VectorContainer = std::vector<T>;
    
    template<typename T>
    using ListContainer = std::list<T>;
    
    template<typename T>
    using SetContainer = std::set<T>;
    
    // 智能指针策略
    template<typename T>
    using UniqueOwnership = std::unique_ptr<T>;
    
    template<typename T>
    using SharedOwnership = std::shared_ptr<T>;
    
    // 内存分配策略
    template<typename T>
    using DefaultAllocator = std::allocator<T>;
    
    template<typename T>
    using PoolAllocator = std::allocator<T>;  // 简化的池分配器
    
    // 策略组合
    template<typename T, template<typename> class Container = VectorContainer, 
             template<typename> class Ownership = UniqueOwnership,
             template<typename> class Allocator = DefaultAllocator>
    class ResourceManager {
    private:
        Container<T> resources_;
        Allocator<T> allocator_;
        
    public:
        template<typename... Args>
        Ownership<T> create_resource(Args&&... args) {
            auto resource = std::make_unique<T>(std::forward<Args>(args)...);
            resources_.push_back(*resource);
            return resource;
        }
        
        const Container<T>& get_resources() const {
            return resources_;
        }
        
        size_t size() const {
            return resources_.size();
        }
    };
}

// ===== 5. 实际应用示例 =====

namespace PracticalExamples {
    // 类型安全的配置系统
    template<typename T>
    using ConfigValue = std::tuple<std::string, T, std::string, bool>;  // name, value, description, required
    
    template<typename... Types>
    using ConfigSchema = std::tuple<ConfigValue<Types>...>;
    
    class ConfigManager {
    private:
        using IntConfig = ConfigValue<int>;
        using StringConfig = ConfigValue<std::string>;
        using BoolConfig = ConfigValue<bool>;
        using DoubleConfig = ConfigValue<double>;
        
        ConfigSchema<int, std::string, bool, double> schema_;
        
    public:
        ConfigManager() {
            schema_ = std::make_tuple(
                IntConfig{"port", 8080, "服务器端口", true},
                StringConfig{"host", "localhost", "服务器地址", true},
                BoolConfig{"debug", false, "调试模式", false},
                DoubleConfig{"timeout", 30.0, "超时时间(秒)", false}
            );
        }
        
        template<typename T>
        T get_config(const std::string& name) const {
            return std::get<T>(get_config_value<T>(name));
        }
        
        template<typename T>
        void set_config(const std::string& name, T value) {
            set_config_value<T>(name, value);
        }
        
        void print_config() const {
            std::cout << "当前配置:\n";
            std::cout << "  端口: " << std::get<1>(std::get<0>(schema_)) << "\n";
            std::cout << "  主机: " << std::get<1>(std::get<1>(schema_)) << "\n";
            std::cout << "  调试: " << (std::get<1>(std::get<2>(schema_)) ? "开启" : "关闭") << "\n";
            std::cout << "  超时: " << std::get<1>(std::get<3>(schema_)) << "s\n";
        }
        
    private:
        template<typename T>
        ConfigValue<T>& get_config_value(const std::string& name) {
            return find_config_value<T>(name, schema_);
        }
        
        template<typename T>
        const ConfigValue<T>& get_config_value(const std::string& name) const {
            return find_config_value<T>(name, schema_);
        }
        
        template<typename T>
        void set_config_value(const std::string& name, T value) {
            auto& config = find_config_value<T>(name, schema_);
            std::get<1>(config) = value;
        }
        
        template<typename T, typename... ConfigTypes>
        static ConfigValue<T>& find_config_value(const std::string& name, std::tuple<ConfigTypes...>& schema) {
            return std::get<ConfigValue<T>>(find_config_by_name(name, schema));
        }
        
        template<typename T, typename... ConfigTypes>
        static const ConfigValue<T>& find_config_value(const std::string& name, const std::tuple<ConfigTypes...>& schema) {
            return std::get<ConfigValue<T>>(find_config_by_name(name, schema));
        }
        
        template<typename... ConfigTypes>
        static auto& find_config_by_name(const std::string& name, std::tuple<ConfigTypes...>& schema) {
            return find_config_by_name_impl(name, schema, std::index_sequence_for<ConfigTypes...>{});
        }
        
        template<typename... ConfigTypes>
        static const auto& find_config_by_name(const std::string& name, const std::tuple<ConfigTypes...>& schema) {
            return find_config_by_name_impl(name, schema, std::index_sequence_for<ConfigTypes...>{});
        }
        
        template<typename... ConfigTypes, size_t... Indices>
        static auto& find_config_by_name_impl(const std::string& name, std::tuple<ConfigTypes...>& schema, std::index_sequence<Indices...>) {
            std::array<std::reference_wrapper<std::tuple_element_t<Indices, std::tuple<ConfigTypes...>>>, sizeof...(Indices)> configs = {std::ref(std::get<Indices>(schema))...};
            
            for (auto& config_ref : configs) {
                auto& config = config_ref.get();
                if (std::get<0>(config) == name) {
                    return config;
                }
            }
            
            throw std::runtime_error("配置项不存在: " + name);
        }
    };
    
    // 类型安全的信号槽系统
    template<typename... Args>
    using SignalFunction = std::function<void(Args...)>;
    
    template<typename... Args>
    using SlotList = std::vector<SignalFunction<Args...>>;
    
    template<typename... Args>
    class Signal {
    private:
        SlotList<Args...> slots_;
        
    public:
        void connect(SignalFunction<Args...> slot) {
            slots_.push_back(slot);
        }
        
        void emit(Args... args) {
            for (const auto& slot : slots_) {
                slot(args...);
            }
        }
        
        size_t slot_count() const {
            return slots_.size();
        }
    };
    
    // 策略模式示例
    template<typename T>
    using SortingStrategy = std::function<void(std::vector<T>&)>;
    
    template<typename T>
    class Sorter {
    private:
        SortingStrategy<T> strategy_;
        
    public:
        explicit Sorter(SortingStrategy<T> strategy) : strategy_(strategy) {}
        
        void sort(std::vector<T>& data) {
            strategy_(data);
        }
        
        void set_strategy(SortingStrategy<T> strategy) {
            strategy_ = strategy;
        }
    };
    
    // 预定义的排序策略
    template<typename T>
    SortingStrategy<T> quick_sort_strategy() {
        return [](std::vector<T>& data) {
            std::sort(data.begin(), data.end());
        };
    }
    
    template<typename T>
    SortingStrategy<T> stable_sort_strategy() {
        return [](std::vector<T>& data) {
            std::stable_sort(data.begin(), data.end());
        };
    }
    
    template<typename T>
    SortingStrategy<T> partial_sort_strategy(size_t k) {
        return [k](std::vector<T>& data) {
            if (k < data.size()) {
                std::partial_sort(data.begin(), data.begin() + k, data.end());
            }
        };
    }
}

} // namespace cpp14_alias_templates

// ===== 主函数 =====

int main() {
    std::cout << "=== C++14 类型别名模板深度解析 ===\n";
    
    using namespace cpp14_alias_templates;
    
    // 1. 基础类型别名演示
    std::cout << "\n===== 1. 基础类型别名演示 =====\n";
    using namespace BasicAliases;
    
    demonstrate_containers();
    
    // 复杂数值类型
    Complex<double> c1(3.0, 4.0);
    Complex<float> c2(1.0f, 2.0f);
    
    std::cout << "复数1: " << c1.real() << " + " << c1.imag() << "i\n";
    std::cout << "复数2: " << c2.real() << " + " << c2.imag() << "i\n";
    
    // 智能指针
    UniquePtr<std::string> ptr1 = std::make_unique<std::string>("Hello");
    SharedPtr<std::string> ptr2 = std::make_shared<std::string>("World");
    
    std::cout << "智能指针内容: " << *ptr1 << " " << *ptr2 << "\n";
    
    // 2. 标准库类型特征演示
    std::cout << "\n===== 2. 标准库类型特征演示 =====\n";
    using namespace StdTraitsAliases;
    
    demonstrate_type_traits<const int&>();
    std::cout << "\n";
    demonstrate_type_traits<volatile std::string>();
    std::cout << "\n";
    demonstrate_type_traits<double* const>();
    
    // 类型特征比较
    std::cout << "\n类型特征比较:\n";
    std::cout << "int是整数: " << std::is_integral_v<int> << "\n";
    std::cout << "double是浮点数: " << std::is_floating_point_v<double> << "\n";
    std::cout << "std::string是类: " << std::is_class_v<std::string> << "\n";
    std::cout << "int*是指针: " << std::is_pointer_v<int*> << "\n";
    
    // 3. SFINAE演示
    std::cout << "\n===== 3. SFINAE演示 =====\n";
    using namespace SFINAEExamples;
    
    // 类型安全加法
    std::cout << "整数加法: " << add_numbers_simple(10, 20) << "\n";
    std::cout << "浮点数加法: " << add_numbers_simple(10.5, 20.3) << "\n";
    
    // 安全加法
    try {
        std::cout << "安全整数加法: " << safe_add_integral(100, 200) << "\n";
        std::cout << "安全浮点加法: " << safe_add_floating(10.5, 20.3) << "\n";
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << "\n";
    }
    
    // 容器处理
    Vector<int> vec = {1, 2, 3, 4, 5};
    Set<std::string> set = {"apple", "banana", "cherry"};
    Map<std::string, int> map = {{"one", 1}, {"two", 2}, {"three", 3}};
    
    process_sequence_container(vec);
    process_associative_container(set);
    process_associative_container(map);
    
    // 4. 模板元编程演示
    std::cout << "\n===== 4. 模板元编程演示 =====\n";
    using namespace TemplateMetaprogramming;
    
    // 类型列表
    using MyTypes = TypeList<int, std::string, double, bool>;
    
    std::cout << "类型列表大小: " << MyTypes::size << "\n";
    std::cout << "包含int: " << Contains_v<MyTypes, int> << "\n";
    std::cout << "包含float: " << Contains_v<MyTypes, float> << "\n";
    std::cout << "int的索引: " << TypeIndex_v<MyTypes, int> << "\n";
    std::cout << "double的索引: " << TypeIndex_v<MyTypes, double> << "\n";
    
    // 类型选择
    using FirstType = TypeAt_t<MyTypes, 0>;
    using ThirdType = TypeAt_t<MyTypes, 2>;
    
    std::cout << "第一个类型: " << typeid(FirstType).name() << "\n";
    std::cout << "第三个类型: " << typeid(ThirdType).name() << "\n";
    
    // 策略模式
    ResourceManager<std::string, ListContainer, SharedOwnership> string_manager;
    auto str1 = string_manager.create_resource("Hello");
    auto str2 = string_manager.create_resource("World");
    
    std::cout << "字符串管理器大小: " << string_manager.size() << "\n";
    
    // 5. 实际应用演示
    std::cout << "\n===== 5. 实际应用演示 =====\n";
    using namespace PracticalExamples;
    
    // 配置管理器
    ConfigManager config_manager;
    config_manager.print_config();
    
    config_manager.set_config("port", 9090);
    config_manager.set_config("debug", true);
    config_manager.set_config("timeout", 60.0);
    
    std::cout << "\n更新后的配置:\n";
    config_manager.print_config();
    
    // 信号槽系统
    Signal<std::string, int> message_signal;
    
    message_signal.connect([](const std::string& msg, int priority) {
        std::cout << "槽1收到消息: " << msg << " (优先级: " << priority << ")\n";
    });
    
    message_signal.connect([](const std::string& msg, int priority) {
        std::cout << "槽2收到消息: " << msg << " (优先级: " << priority << ")\n";
    });
    
    message_signal.emit("重要通知", 1);
    message_signal.emit("普通消息", 5);
    
    // 策略模式排序
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3};
    Sorter<int> sorter(quick_sort_strategy<int>());
    
    std::cout << "\n原始数组: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    sorter.sort(numbers);
    std::cout << "快速排序后: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    // 重新打乱并使用稳定排序
    std::shuffle(numbers.begin(), numbers.end(), std::mt19937(std::random_device()()));
    sorter.set_strategy(stable_sort_strategy<int>());
    sorter.sort(numbers);
    
    std::cout << "稳定排序后: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall 08_alias_templates.cpp -o alias_templates
./alias_templates

关键学习点:
1. 类型别名模板使用using声明，比typedef更加灵活和强大
2. 标准库提供了_t后缀的类型特征别名，简化了模板元编程
3. 类型别名模板可以大大提高模板代码的可读性和维护性
4. SFINAE技术通过类型别名模板变得更加简洁和直观
5. 策略模式可以通过类型别名模板实现灵活的组件组合
6. 类型别名模板是现代C++模板编程的重要工具

注意事项:
- 类型别名模板不能特化，需要使用类模板特化
- 类型别名模板在模板参数推导中的作用与原始类型相同
- 过度使用类型别名可能导致代码难以理解和调试
- 类型别名模板应该有清晰的命名，避免混淆
- 在复杂的模板元编程中，类型别名模板可以显著提高代码质量
*/