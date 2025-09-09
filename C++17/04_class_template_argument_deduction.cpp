/**
 * C++17 类模板参数推导深度解析
 * 
 * 核心概念：
 * 1. 自动推导机制 - 构造函数参数到模板参数的映射
 * 2. 推导指引设计 - 用户定义推导规则的高级技巧
 * 3. 标准库适配 - STL容器的推导增强
 * 4. 编译期优化 - 类型推导的性能影响分析
 * 5. 设计模式应用 - 工厂模式和RAII的现代化改进
 */

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <tuple>
#include <array>
#include <map>
#include <functional>
#include <type_traits>
#include <chrono>

// ===== 1. 自动推导机制演示 =====
template<typename T>
class SimpleContainer {
private:
    T data;
    
public:
    explicit SimpleContainer(T value) : data(std::move(value)) {
        std::cout << "构造SimpleContainer，类型: " << typeid(T).name() << "\n";
    }
    
    template<typename U>
    SimpleContainer(U&& value) : data(std::forward<U>(value)) {
        std::cout << "转发构造SimpleContainer，类型: " << typeid(T).name() << "\n";
    }
    
    const T& get() const { return data; }
};

// 多参数模板类
template<typename T, typename U>
class Pair {
private:
    T first;
    U second;
    
public:
    Pair(T f, U s) : first(std::move(f)), second(std::move(s)) {
        std::cout << "构造Pair<" << typeid(T).name() << ", " << typeid(U).name() << ">\n";
    }
    
    const T& get_first() const { return first; }
    const U& get_second() const { return second; }
};

// 变参模板类
template<typename... Args>
class VariadicContainer {
private:
    std::tuple<Args...> data;
    
public:
    explicit VariadicContainer(Args... args) : data(std::make_tuple(args...)) {
        std::cout << "构造VariadicContainer，参数数量: " << sizeof...(Args) << "\n";
    }
    
    template<size_t N>
    decltype(auto) get() const {
        return std::get<N>(data);
    }
    
    static constexpr size_t size() { return sizeof...(Args); }
};

void demonstrate_automatic_deduction() {
    std::cout << "=== 自动推导机制演示 ===\n";
    
    // C++17之前需要显式指定类型
    // SimpleContainer<int> old_way{42};
    
    // C++17自动推导
    SimpleContainer sc1{42};           // 推导为SimpleContainer<int>
    SimpleContainer sc2{3.14};         // 推导为SimpleContainer<double>
    SimpleContainer sc3{std::string{"hello"}};  // 推导为SimpleContainer<std::string>
    
    std::cout << "sc1: " << sc1.get() << "\n";
    std::cout << "sc2: " << sc2.get() << "\n";
    std::cout << "sc3: " << sc3.get() << "\n";
    
    // 多参数推导
    Pair p1{10, 20.5};                 // 推导为Pair<int, double>
    Pair p2{"key", std::vector<int>{1,2,3}};  // 推导为Pair<const char*, std::vector<int>>
    
    std::cout << "p1: (" << p1.get_first() << ", " << p1.get_second() << ")\n";
    std::cout << "p2: (" << p1.get_first() << ", size=" << p2.get_second().size() << ")\n";
    
    // 变参模板推导
    VariadicContainer vc1{1, 2.5, "hello"};  // 推导为VariadicContainer<int, double, const char*>
    std::cout << "vc1[0]: " << vc1.get<0>() << "\n";
    std::cout << "vc1[1]: " << vc1.get<1>() << "\n";
    std::cout << "vc1[2]: " << vc1.get<2>() << "\n";
    
    std::cout << "\n";
}

// ===== 2. 推导指引设计演示 =====
template<typename T>
class SmartArray {
private:
    std::vector<T> data;
    
public:
    // 从初始化列表构造
    SmartArray(std::initializer_list<T> init) : data(init) {
        std::cout << "从初始化列表构造SmartArray<" << typeid(T).name() << ">\n";
    }
    
    // 从迭代器构造
    template<typename Iterator>
    SmartArray(Iterator first, Iterator last) : data(first, last) {
        std::cout << "从迭代器构造SmartArray<" << typeid(T).name() << ">\n";
    }
    
    // 填充构造
    SmartArray(size_t count, const T& value) : data(count, value) {
        std::cout << "填充构造SmartArray<" << typeid(T).name() << ">, 数量: " << count << "\n";
    }
    
    size_t size() const { return data.size(); }
    const T& operator[](size_t index) const { return data[index]; }
};

// 推导指引：从迭代器推导元素类型
template<typename Iterator>
SmartArray(Iterator, Iterator) -> SmartArray<typename std::iterator_traits<Iterator>::value_type>;

// 推导指引：从初始化列表推导
template<typename T>
SmartArray(std::initializer_list<T>) -> SmartArray<T>;

// 推导指引：显式指定类型转换
template<typename T>
SmartArray(size_t, T) -> SmartArray<T>;

// 复杂推导指引示例：自定义容器
template<typename Key, typename Value>
class ConfigMap {
private:
    std::map<Key, Value> data;
    
public:
    ConfigMap() = default;
    
    template<typename... Pairs>
    ConfigMap(Pairs&&... pairs) {
        static_assert(sizeof...(pairs) % 2 == 0, "参数必须成对出现");
        add_pairs(std::forward<Pairs>(pairs)...);
    }
    
    void set(const Key& key, const Value& value) {
        data[key] = value;
    }
    
    const Value& get(const Key& key) const {
        return data.at(key);
    }
    
    size_t size() const { return data.size(); }
    
private:
    template<typename K, typename V>
    void add_pairs(K&& key, V&& value) {
        data.emplace(std::forward<K>(key), std::forward<V>(value));
    }
    
    template<typename K, typename V, typename... Rest>
    void add_pairs(K&& key, V&& value, Rest&&... rest) {
        data.emplace(std::forward<K>(key), std::forward<V>(value));
        add_pairs(std::forward<Rest>(rest)...);
    }
};

// 配置映射的推导指引
template<typename K, typename V, typename... Rest>
ConfigMap(K, V, Rest...) -> ConfigMap<std::decay_t<K>, std::decay_t<V>>;

void demonstrate_deduction_guides() {
    std::cout << "=== 推导指引设计演示 ===\n";
    
    // 使用推导指引
    SmartArray arr1{1, 2, 3, 4, 5};                    // 从初始化列表推导
    std::vector<double> source = {1.1, 2.2, 3.3};
    SmartArray arr2(source.begin(), source.end());     // 从迭代器推导
    SmartArray arr3(3, std::string{"default"});        // 填充构造推导
    
    std::cout << "arr1大小: " << arr1.size() << ", 首元素: " << arr1[0] << "\n";
    std::cout << "arr2大小: " << arr2.size() << ", 首元素: " << arr2[0] << "\n";
    std::cout << "arr3大小: " << arr3.size() << ", 首元素: " << arr3[0] << "\n";
    
    // 复杂推导指引
    ConfigMap config{"host", std::string{"localhost"}, "port", 8080, "debug", true};
    std::cout << "配置项数量: " << config.size() << "\n";
    std::cout << "主机: " << config.get(std::string{"host"}) << "\n";
    
    std::cout << "\n";
}

// ===== 3. 标准库适配演示 =====
void demonstrate_stdlib_adaptation() {
    std::cout << "=== 标准库适配演示 ===\n";
    
    // std::vector推导
    std::vector v1{1, 2, 3, 4, 5};                    // 推导为vector<int>
    std::vector v2{1.1, 2.2, 3.3};                    // 推导为vector<double>
    
    std::cout << "v1类型推导成功，大小: " << v1.size() << "\n";
    std::cout << "v2类型推导成功，大小: " << v2.size() << "\n";
    
    // std::array推导 - 需要推导指引
    std::array arr{1, 2, 3, 4, 5};                    // 推导为array<int, 5>
    std::cout << "数组大小: " << arr.size() << "\n";
    
    // std::tuple推导
    auto tuple1 = std::make_tuple(1, 2.5, "hello");   // 传统方式
    std::tuple tuple2{1, 2.5, "hello"};               // C++17推导
    
    std::cout << "tuple推导成功，元素0: " << std::get<0>(tuple2) << "\n";
    
    // std::pair推导
    std::pair p1{42, std::string{"answer"}};          // 推导为pair<int, string>
    std::cout << "pair推导: " << p1.first << " -> " << p1.second << "\n";
    
    // std::map推导（复杂情况）
    std::map map1{std::pair{"key1", 100}, std::pair{"key2", 200}};
    std::cout << "map推导成功，大小: " << map1.size() << "\n";
    
    // std::function推导
    auto lambda = [](int x) { return x * 2; };
    std::function func{lambda};                        // 推导为function<int(int)>
    std::cout << "function推导: " << func(21) << "\n";
    
    std::cout << "\n";
}

// ===== 4. 编译期优化演示 =====
template<typename T>
class OptimizedContainer {
private:
    T data;
    
public:
    explicit OptimizedContainer(T value) : data(std::move(value)) {}
    
    // 编译期类型检查
    static_assert(std::is_move_constructible_v<T>, "类型必须支持移动构造");
    
    const T& get() const noexcept { return data; }
    
    // 基于类型特性的优化方法
    auto size() const {
        if constexpr (requires { data.size(); }) {
            return data.size();
        } else {
            return size_t{1};
        }
    }
    
    // 条件化的序列化方法
    void serialize() const {
        if constexpr (std::is_arithmetic_v<T>) {
            std::cout << "算术类型序列化: " << data << "\n";
        } else if constexpr (requires { std::cout << data; }) {
            std::cout << "可打印类型序列化: " << data << "\n";
        } else {
            std::cout << "复杂类型序列化: [对象]\n";
        }
    }
};

// 性能测试：类型推导 vs 显式指定
template<typename T>
auto measure_construction(const T& value, const std::string& method) {
    constexpr int iterations = 1000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        OptimizedContainer container{value};  // 使用推导
        auto& result = container.get();
        (void)result;  // 防止优化
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << method << " 构造耗时: " << duration.count() << " 微秒\n";
    return duration.count();
}

void demonstrate_compile_time_optimization() {
    std::cout << "=== 编译期优化演示 ===\n";
    
    // 不同类型的优化容器
    OptimizedContainer oc1{42};
    OptimizedContainer oc2{std::string{"hello"}};
    OptimizedContainer oc3{std::vector<int>{1, 2, 3, 4, 5}};
    
    std::cout << "整数容器大小: " << oc1.size() << "\n";
    std::cout << "字符串容器大小: " << oc2.size() << "\n";
    std::cout << "向量容器大小: " << oc3.size() << "\n";
    
    // 条件化序列化
    oc1.serialize();
    oc2.serialize();
    oc3.serialize();
    
    // 性能测试
    measure_construction(42, "整数推导");
    measure_construction(std::string{"test"}, "字符串推导");
    
    std::cout << "\n";
}

// ===== 5. 设计模式应用演示 =====
// 工厂模式的现代化
template<typename T, typename... Args>
class Factory {
private:
    static inline size_t instance_count = 0;
    
public:
    static auto create(Args... args) {
        ++instance_count;
        std::cout << "创建实例 #" << instance_count << "\n";
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    static size_t get_instance_count() { return instance_count; }
};

// 推导指引使工厂更易用
template<typename T, typename... Args>
Factory(T, Args...) -> Factory<T, Args...>;

// RAII包装器
template<typename Resource, typename Deleter = std::default_delete<Resource>>
class RAIIWrapper {
private:
    std::unique_ptr<Resource, Deleter> resource;
    
public:
    template<typename... Args>
    explicit RAIIWrapper(Args&&... args) 
        : resource(std::make_unique<Resource>(std::forward<Args>(args)...)) {
        std::cout << "RAII获取资源\n";
    }
    
    ~RAIIWrapper() {
        std::cout << "RAII释放资源\n";
    }
    
    Resource* get() const { return resource.get(); }
    Resource& operator*() const { return *resource; }
    Resource* operator->() const { return resource.get(); }
};

// 推导指引
template<typename... Args>
RAIIWrapper(Args&&...) -> RAIIWrapper<std::common_type_t<Args...>>;

// 建造者模式的简化
template<typename T>
class Builder {
private:
    T object;
    
public:
    Builder() = default;
    
    template<typename U>
    Builder& set_value(U&& value) {
        object = std::forward<U>(value);
        return *this;
    }
    
    T build() && { return std::move(object); }
};

// 推导指引
template<typename T>
Builder(T) -> Builder<T>;

void demonstrate_design_patterns() {
    std::cout << "=== 设计模式应用演示 ===\n";
    
    // 现代化工厂模式
    auto str_ptr = Factory<std::string>::create("factory created");
    auto int_ptr = Factory<int>::create(42);
    
    std::cout << "工厂创建的字符串: " << *str_ptr << "\n";
    std::cout << "工厂创建的整数: " << *int_ptr << "\n";
    std::cout << "字符串工厂实例数: " << Factory<std::string>::get_instance_count() << "\n";
    
    // RAII包装器
    {
        RAIIWrapper wrapper{std::string{"resource"}};
        std::cout << "使用RAII资源: " << *wrapper << "\n";
    }  // 自动释放资源
    
    // 建造者模式
    auto built_string = Builder{}
        .set_value(std::string{"built"})
        .build();
    
    std::cout << "建造者构建结果: " << built_string << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 类模板参数推导深度解析\n";
    std::cout << "==============================\n";
    
    demonstrate_automatic_deduction();
    demonstrate_deduction_guides();
    demonstrate_stdlib_adaptation();
    demonstrate_compile_time_optimization();
    demonstrate_design_patterns();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 04_class_template_argument_deduction.cpp -o class_template_deduction
./class_template_deduction

关键学习点:
1. 类模板参数推导简化了模板类的使用，无需显式指定类型
2. 推导指引提供了自定义推导规则的强大机制
3. 标准库容器都支持了参数推导，提升了代码简洁性
4. 编译期类型推导不会影响运行时性能
5. 现代设计模式可以通过推导变得更加简洁易用

注意事项:
- 推导指引的设计要考虑类型安全和用户意图
- 某些复杂情况下可能需要显式指定类型
- 推导失败时编译错误可能较难理解
- 与auto关键字结合使用时要注意类型的一致性
*/