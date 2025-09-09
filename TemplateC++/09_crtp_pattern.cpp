/**
 * C++98/11/14/17/20 CRTP设计模式深度解析
 * 
 * 核心概念：
 * 1. CRTP基础机制 - 编译期多态的实现原理
 * 2. 静态接口与鸭子类型 - 编译期接口约束
 * 3. CRTP在设计模式中的应用 - 单例、观察者等
 * 4. 性能优化与代码复用 - 零开销抽象技术
 * 5. CRTP的高级技巧 - 多层继承与混入模式
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <type_traits>
#include <concepts>

// ===== 1. CRTP基础机制演示 =====
void demonstrate_crtp_basics() {
    std::cout << "=== CRTP基础机制演示 ===\n";
    
    // 基础CRTP模式
    template<typename Derived>
    class CRTPBase {
    public:
        // 静态多态接口
        void interface() {
            std::cout << "CRTP接口调用 -> ";
            static_cast<Derived*>(this)->implementation();
        }
        
        // 通用功能实现
        void common_functionality() {
            std::cout << "基类通用功能\n";
            static_cast<Derived*>(this)->specific_behavior();
        }
        
        // 获取派生类类型信息
        std::string get_type_name() const {
            return typeid(Derived).name();
        }
        
    protected:
        // 阻止通过基类指针删除
        ~CRTPBase() = default;
        
    private:
        // 友元函数用于类型安全检查
        friend Derived;
    };
    
    // 具体实现类1
    class ConcreteA : public CRTPBase<ConcreteA> {
    public:
        void implementation() {
            std::cout << "ConcreteA的具体实现\n";
        }
        
        void specific_behavior() {
            std::cout << "ConcreteA特定行为\n";
        }
        
        // ConcreteA特有方法
        void unique_method_a() {
            std::cout << "ConcreteA独有方法\n";
        }
    };
    
    // 具体实现类2
    class ConcreteB : public CRTPBase<ConcreteB> {
    public:
        void implementation() {
            std::cout << "ConcreteB的具体实现\n";
        }
        
        void specific_behavior() {
            std::cout << "ConcreteB特定行为\n";
        }
        
        // ConcreteB特有方法
        void unique_method_b() {
            std::cout << "ConcreteB独有方法\n";
        }
    };
    
    // 测试CRTP多态
    ConcreteA obj_a;
    ConcreteB obj_b;
    
    std::cout << "对象A类型: " << obj_a.get_type_name() << "\n";
    obj_a.interface();
    obj_a.common_functionality();
    obj_a.unique_method_a();
    
    std::cout << "\n对象B类型: " << obj_b.get_type_name() << "\n";
    obj_b.interface();
    obj_b.common_functionality();
    obj_b.unique_method_b();
    
    std::cout << "\n";
}

// ===== 2. 静态接口与鸭子类型演示 =====
void demonstrate_static_interface() {
    std::cout << "=== 静态接口与鸭子类型演示 ===\n";
    
    // 静态接口定义（通过CRTP）
    template<typename Drawable>
    class DrawableBase {
    public:
        void draw() {
            std::cout << "开始绘制 -> ";
            static_cast<Drawable*>(this)->do_draw();
            std::cout << "绘制完成\n";
        }
        
        void set_color(const std::string& color) {
            std::cout << "设置颜色: " << color << " -> ";
            static_cast<Drawable*>(this)->apply_color(color);
        }
        
        // 静态接口验证
        template<typename T = Drawable>
        static constexpr bool has_draw_method() {
            return requires(T* obj) {
                obj->do_draw();
                obj->apply_color(std::declval<std::string>());
            };
        }
    };
    
    // 圆形实现
    class Circle : public DrawableBase<Circle> {
    private:
        double radius_;
        std::string color_;
        
    public:
        Circle(double r) : radius_(r), color_("black") {}
        
        void do_draw() {
            std::cout << "绘制半径为 " << radius_ << " 的圆形";
        }
        
        void apply_color(const std::string& color) {
            color_ = color;
            std::cout << "圆形颜色应用: " << color_;
        }
        
        double get_radius() const { return radius_; }
    };
    
    // 矩形实现
    class Rectangle : public DrawableBase<Rectangle> {
    private:
        double width_, height_;
        std::string color_;
        
    public:
        Rectangle(double w, double h) : width_(w), height_(h), color_("black") {}
        
        void do_draw() {
            std::cout << "绘制 " << width_ << "x" << height_ << " 矩形";
        }
        
        void apply_color(const std::string& color) {
            color_ = color;
            std::cout << "矩形颜色应用: " << color_;
        }
        
        double get_area() const { return width_ * height_; }
    };
    
    // 通用绘图函数模板（鸭子类型）
    template<typename Drawable>
    void generic_draw_function(Drawable& drawable) {
        static_assert(DrawableBase<Drawable>::template has_draw_method<Drawable>(), 
                      "类型必须实现绘图接口");
        
        std::cout << "通用绘图函数调用:\n";
        drawable.set_color("红色");
        std::cout << "\n";
        drawable.draw();
    }
    
    // 测试静态接口
    Circle circle(5.0);
    Rectangle rectangle(10.0, 20.0);
    
    generic_draw_function(circle);
    generic_draw_function(rectangle);
    
    // 编译时接口检查演示
    std::cout << "\n编译时接口检查:\n";
    std::cout << "Circle是否实现绘图接口: " 
              << DrawableBase<Circle>::has_draw_method() << "\n";
    std::cout << "Rectangle是否实现绘图接口: " 
              << DrawableBase<Rectangle>::has_draw_method() << "\n";
    
    std::cout << "\n";
}

// ===== 3. CRTP在设计模式中的应用演示 =====
void demonstrate_crtp_design_patterns() {
    std::cout << "=== CRTP在设计模式中的应用演示 ===\n";
    
    // 1. 单例模式的CRTP实现
    template<typename T>
    class Singleton {
    public:
        static T& getInstance() {
            static T instance;
            return instance;
        }
        
        // 禁用拷贝和赋值
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;
        
    protected:
        Singleton() = default;
        virtual ~Singleton() = default;
    };
    
    // 具体单例类
    class Logger : public Singleton<Logger> {
        friend class Singleton<Logger>;  // 允许基类访问构造函数
        
    private:
        std::vector<std::string> logs_;
        
        Logger() {
            std::cout << "Logger单例创建\n";
        }
        
    public:
        void log(const std::string& message) {
            logs_.push_back(message);
            std::cout << "LOG: " << message << "\n";
        }
        
        size_t get_log_count() const { return logs_.size(); }
    };
    
    // 2. 观察者模式的CRTP实现
    template<typename Derived>
    class Observable {
    private:
        std::vector<std::function<void(const Derived&)>> observers_;
        
    public:
        void add_observer(std::function<void(const Derived&)> observer) {
            observers_.push_back(observer);
        }
        
        void notify_observers() {
            const auto& derived = static_cast<const Derived&>(*this);
            for (auto& observer : observers_) {
                observer(derived);
            }
        }
    };
    
    // 具体被观察对象
    class Counter : public Observable<Counter> {
    private:
        int count_ = 0;
        
    public:
        void increment() {
            ++count_;
            notify_observers();
        }
        
        int get_count() const { return count_; }
    };
    
    // 3. 策略模式的CRTP实现
    template<typename Strategy>
    class ContextWithStrategy {
    public:
        template<typename... Args>
        void execute_strategy(Args&&... args) {
            std::cout << "上下文执行策略 -> ";
            static_cast<Strategy*>(this)->strategy_implementation(std::forward<Args>(args)...);
        }
    };
    
    class ConcreteStrategyA : public ContextWithStrategy<ConcreteStrategyA> {
    public:
        void strategy_implementation(const std::string& data) {
            std::cout << "策略A处理: " << data << "\n";
        }
    };
    
    class ConcreteStrategyB : public ContextWithStrategy<ConcreteStrategyB> {
    public:
        void strategy_implementation(const std::string& data) {
            std::cout << "策略B处理: " << data << " (大写: ";
            for (char c : data) {
                std::cout << static_cast<char>(std::toupper(c));
            }
            std::cout << ")\n";
        }
    };
    
    // 测试设计模式
    std::cout << "1. 单例模式测试:\n";
    Logger& logger1 = Logger::getInstance();
    Logger& logger2 = Logger::getInstance();
    
    logger1.log("第一条消息");
    logger2.log("第二条消息");
    std::cout << "日志条数: " << logger1.get_log_count() << "\n";
    std::cout << "是否为同一实例: " << (&logger1 == &logger2) << "\n\n";
    
    std::cout << "2. 观察者模式测试:\n";
    Counter counter;
    
    counter.add_observer([](const Counter& c) {
        std::cout << "观察者1: 计数器值为 " << c.get_count() << "\n";
    });
    
    counter.add_observer([](const Counter& c) {
        if (c.get_count() % 2 == 0) {
            std::cout << "观察者2: 计数器达到偶数 " << c.get_count() << "\n";
        }
    });
    
    counter.increment();
    counter.increment();
    counter.increment();
    
    std::cout << "\n3. 策略模式测试:\n";
    ConcreteStrategyA strategy_a;
    ConcreteStrategyB strategy_b;
    
    strategy_a.execute_strategy("hello world");
    strategy_b.execute_strategy("hello world");
    
    std::cout << "\n";
}

// ===== 4. 性能优化与代码复用演示 =====
void demonstrate_performance_optimization() {
    std::cout << "=== 性能优化与代码复用演示 ===\n";
    
    // 性能优化：编译时多态vs运行时多态
    
    // 传统虚函数接口
    class VirtualBase {
    public:
        virtual ~VirtualBase() = default;
        virtual int calculate() const = 0;
        virtual std::string name() const = 0;
    };
    
    class VirtualDerived : public VirtualBase {
    public:
        int calculate() const override {
            return 42 * 2;
        }
        
        std::string name() const override {
            return "VirtualDerived";
        }
    };
    
    // CRTP接口
    template<typename Derived>
    class CRTPBase {
    public:
        int calculate() const {
            return static_cast<const Derived*>(this)->calculate_impl();
        }
        
        std::string name() const {
            return static_cast<const Derived*>(this)->name_impl();
        }
    };
    
    class CRTPDerived : public CRTPBase<CRTPDerived> {
    public:
        int calculate_impl() const {
            return 42 * 2;
        }
        
        std::string name_impl() const {
            return "CRTPDerived";
        }
    };
    
    // 代码复用：CRTP混入模式
    template<typename Derived>
    class Printable {
    public:
        void print() const {
            std::cout << "打印: " << static_cast<const Derived*>(this)->to_string() << "\n";
        }
    };
    
    template<typename Derived>
    class Serializable {
    public:
        std::string serialize() const {
            return "序列化[" + static_cast<const Derived*>(this)->to_string() + "]";
        }
    };
    
    template<typename Derived>
    class Comparable {
    public:
        bool equals(const Derived& other) const {
            return static_cast<const Derived*>(this)->compare_impl(other);
        }
        
        bool operator==(const Derived& other) const {
            return equals(other);
        }
        
        bool operator!=(const Derived& other) const {
            return !equals(other);
        }
    };
    
    // 组合多个CRTP基类
    class Person : public Printable<Person>, 
                   public Serializable<Person>, 
                   public Comparable<Person> {
    private:
        std::string name_;
        int age_;
        
    public:
        Person(std::string name, int age) : name_(std::move(name)), age_(age) {}
        
        // 为CRTP基类提供接口实现
        std::string to_string() const {
            return name_ + "(" + std::to_string(age_) + ")";
        }
        
        bool compare_impl(const Person& other) const {
            return name_ == other.name_ && age_ == other.age_;
        }
        
        // 普通访问器
        const std::string& get_name() const { return name_; }
        int get_age() const { return age_; }
    };
    
    // 性能基准测试
    const int iterations = 1000000;
    
    // 虚函数版本基准测试
    std::unique_ptr<VirtualBase> virtual_obj = std::make_unique<VirtualDerived>();
    auto start = std::chrono::high_resolution_clock::now();
    
    int virtual_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        virtual_sum += virtual_obj->calculate();
    }
    
    auto virtual_time = std::chrono::high_resolution_clock::now() - start;
    
    // CRTP版本基准测试
    CRTPDerived crtp_obj;
    start = std::chrono::high_resolution_clock::now();
    
    int crtp_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        crtp_sum += crtp_obj.calculate();
    }
    
    auto crtp_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "性能对比结果:\n";
    std::cout << "虚函数版本耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(virtual_time).count() 
              << " 微秒 (结果: " << virtual_sum << ")\n";
    std::cout << "CRTP版本耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(crtp_time).count() 
              << " 微秒 (结果: " << crtp_sum << ")\n";
    
    double speedup = static_cast<double>(virtual_time.count()) / crtp_time.count();
    std::cout << "CRTP性能提升: " << speedup << "x\n\n";
    
    // 代码复用测试
    std::cout << "代码复用测试:\n";
    Person person1("Alice", 25);
    Person person2("Bob", 30);
    Person person3("Alice", 25);
    
    person1.print();
    std::cout << "序列化结果: " << person1.serialize() << "\n";
    std::cout << "person1 == person2: " << (person1 == person2) << "\n";
    std::cout << "person1 == person3: " << (person1 == person3) << "\n";
    
    std::cout << "\n";
}

// ===== 5. CRTP的高级技巧演示 =====
void demonstrate_advanced_crtp() {
    std::cout << "=== CRTP的高级技巧演示 ===\n";
    
    // 1. 递归CRTP（多层继承）
    template<typename Derived>
    class Level1Base {
    public:
        void level1_method() {
            std::cout << "Level1调用 -> ";
            static_cast<Derived*>(this)->level1_impl();
        }
        
    protected:
        void level1_common() {
            std::cout << "Level1通用功能\n";
        }
    };
    
    template<typename Derived>
    class Level2Base : public Level1Base<Level2Base<Derived>> {
    public:
        void level2_method() {
            std::cout << "Level2调用 -> ";
            static_cast<Derived*>(this)->level2_impl();
        }
        
        void level1_impl() {
            std::cout << "Level2实现Level1接口 -> ";
            static_cast<Derived*>(this)->level1_delegation();
        }
        
    protected:
        void level2_common() {
            std::cout << "Level2通用功能\n";
        }
    };
    
    class FinalImplementation : public Level2Base<FinalImplementation> {
    public:
        void level1_delegation() {
            std::cout << "最终类处理Level1\n";
        }
        
        void level2_impl() {
            std::cout << "最终类处理Level2\n";
        }
    };
    
    // 2. 条件CRTP（根据模板参数选择基类）
    template<bool EnableFeatureA, bool EnableFeatureB>
    struct ConditionalBase;
    
    // 特化：都不启用
    template<>
    struct ConditionalBase<false, false> {
        void info() { std::cout << "基础版本"; }
    };
    
    // 特化：只启用A
    template<>
    struct ConditionalBase<true, false> {
        void info() { std::cout << "启用功能A"; }
        void feature_a() { std::cout << "执行功能A\n"; }
    };
    
    // 特化：只启用B
    template<>
    struct ConditionalBase<false, true> {
        void info() { std::cout << "启用功能B"; }
        void feature_b() { std::cout << "执行功能B\n"; }
    };
    
    // 特化：都启用
    template<>
    struct ConditionalBase<true, true> {
        void info() { std::cout << "启用功能A和B"; }
        void feature_a() { std::cout << "执行功能A\n"; }
        void feature_b() { std::cout << "执行功能B\n"; }
    };
    
    template<typename Derived, bool FeatureA = false, bool FeatureB = false>
    class ConfigurableBase : public ConditionalBase<FeatureA, FeatureB> {
    public:
        void configure() {
            std::cout << "配置: ";
            ConditionalBase<FeatureA, FeatureB>::info();
            std::cout << "\n";
            
            static_cast<Derived*>(this)->custom_behavior();
        }
    };
    
    // 不同配置的实现
    class BasicImpl : public ConfigurableBase<BasicImpl, false, false> {
    public:
        void custom_behavior() {
            std::cout << "基础实现行为\n";
        }
    };
    
    class FeatureAImpl : public ConfigurableBase<FeatureAImpl, true, false> {
    public:
        void custom_behavior() {
            std::cout << "功能A实现行为\n";
            feature_a();
        }
    };
    
    class FullFeatureImpl : public ConfigurableBase<FullFeatureImpl, true, true> {
    public:
        void custom_behavior() {
            std::cout << "全功能实现行为\n";
            feature_a();
            feature_b();
        }
    };
    
    // 3. CRTP计数器（统计实例数量）
    template<typename Derived>
    class InstanceCounter {
    private:
        static inline int count_ = 0;
        
    protected:
        InstanceCounter() { ++count_; }
        InstanceCounter(const InstanceCounter&) { ++count_; }
        ~InstanceCounter() { --count_; }
        
    public:
        static int get_instance_count() { return count_; }
    };
    
    class CountedClass : public InstanceCounter<CountedClass> {
    private:
        std::string name_;
        
    public:
        CountedClass(std::string name) : name_(std::move(name)) {
            std::cout << "创建 " << name_ << ", 实例数: " << get_instance_count() << "\n";
        }
        
        ~CountedClass() {
            std::cout << "销毁 " << name_ << ", 剩余实例数: " << get_instance_count() - 1 << "\n";
        }
        
        const std::string& get_name() const { return name_; }
    };
    
    // 测试高级技巧
    std::cout << "1. 递归CRTP测试:\n";
    FinalImplementation final_impl;
    final_impl.level1_method();
    final_impl.level2_method();
    
    std::cout << "\n2. 条件CRTP测试:\n";
    BasicImpl basic;
    FeatureAImpl feature_a;
    FullFeatureImpl full_feature;
    
    basic.configure();
    feature_a.configure();
    full_feature.configure();
    
    std::cout << "\n3. CRTP实例计数测试:\n";
    {
        CountedClass obj1("对象1");
        {
            CountedClass obj2("对象2");
            CountedClass obj3("对象3");
            std::cout << "当前实例数: " << CountedClass::get_instance_count() << "\n";
        }
        std::cout << "内层作用域结束后实例数: " << CountedClass::get_instance_count() << "\n";
    }
    std::cout << "外层作用域结束后实例数: " << CountedClass::get_instance_count() << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++98/11/14/17/20 CRTP设计模式深度解析\n";
    std::cout << "=====================================\n";
    
    demonstrate_crtp_basics();
    demonstrate_static_interface();
    demonstrate_crtp_design_patterns();
    demonstrate_performance_optimization();
    demonstrate_advanced_crtp();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 09_crtp_pattern.cpp -o crtp_pattern
./crtp_pattern

C++20版本:
g++ -std=c++20 -O2 -Wall 09_crtp_pattern.cpp -o crtp_pattern

关键学习点:
1. CRTP通过模板实现编译期多态，避免虚函数开销
2. 静态接口提供编译时类型检查和鸭子类型语义
3. CRTP可以优雅实现多种设计模式（单例、观察者、策略等）
4. 性能优势显著，同时支持强大的代码复用机制
5. 高级技巧包括递归CRTP、条件CRTP和实例统计等

注意事项:
- CRTP要求在编译期确定派生类类型
- 需要小心处理构造和析构的顺序
- 模板实例化可能导致代码膨胀
- 静态接口约束需要合理设计以保证类型安全
*/