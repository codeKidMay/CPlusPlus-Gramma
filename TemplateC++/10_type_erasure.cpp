/**
 * C++11/14/17/20 类型擦除技术深度解析
 * 
 * 核心概念：
 * 1. 类型擦除基础原理 - 隐藏具体类型的实现机制
 * 2. 函数对象的类型擦除 - std::function实现原理
 * 3. 任意类型容器设计 - std::any与自定义实现
 * 4. 接口类型擦除模式 - 运行时多态的现代替代
 * 5. 性能优化技术 - 小对象优化与内存管理
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <typeinfo>
#include <type_traits>
#include <any>
#include <chrono>
#include <algorithm>

// ===== 1. 类型擦除基础原理演示 =====
void demonstrate_type_erasure_basics() {
    std::cout << "=== 类型擦除基础原理演示 ===\n";
    
    // 基础类型擦除实现
    class TypeErasedContainer {
    private:
        // 类型擦除基类
        struct ConceptBase {
            virtual ~ConceptBase() = default;
            virtual std::unique_ptr<ConceptBase> clone() const = 0;
            virtual void print() const = 0;
            virtual std::string type_name() const = 0;
        };
        
        // 具体类型包装
        template<typename T>
        struct ConceptModel : ConceptBase {
            T data;
            
            explicit ConceptModel(T value) : data(std::move(value)) {}
            
            std::unique_ptr<ConceptBase> clone() const override {
                return std::make_unique<ConceptModel>(data);
            }
            
            void print() const override {
                std::cout << "存储的值: " << data;
            }
            
            std::string type_name() const override {
                return typeid(T).name();
            }
        };
        
        std::unique_ptr<ConceptBase> ptr_;
        
    public:
        // 构造函数模板
        template<typename T>
        TypeErasedContainer(T value) 
            : ptr_(std::make_unique<ConceptModel<std::decay_t<T>>>(std::forward<T>(value))) {
            std::cout << "类型擦除容器构造，原始类型: " << typeid(T).name() << "\n";
        }
        
        // 拷贝构造
        TypeErasedContainer(const TypeErasedContainer& other) 
            : ptr_(other.ptr_ ? other.ptr_->clone() : nullptr) {
            std::cout << "类型擦除容器拷贝构造\n";
        }
        
        // 移动构造
        TypeErasedContainer(TypeErasedContainer&&) = default;
        
        // 赋值操作符
        TypeErasedContainer& operator=(const TypeErasedContainer& other) {
            if (this != &other) {
                ptr_ = other.ptr_ ? other.ptr_->clone() : nullptr;
                std::cout << "类型擦除容器拷贝赋值\n";
            }
            return *this;
        }
        
        TypeErasedContainer& operator=(TypeErasedContainer&&) = default;
        
        // 接口方法
        void print() const {
            if (ptr_) {
                std::cout << "类型: " << ptr_->type_name() << ", ";
                ptr_->print();
            } else {
                std::cout << "空容器";
            }
            std::cout << "\n";
        }
        
        bool empty() const { return ptr_ == nullptr; }
    };
    
    // 测试基础类型擦除
    std::vector<TypeErasedContainer> containers;
    
    containers.emplace_back(42);
    containers.emplace_back(3.14);
    containers.emplace_back(std::string("Hello"));
    containers.emplace_back(std::vector<int>{1, 2, 3});
    
    std::cout << "\n存储的不同类型数据:\n";
    for (const auto& container : containers) {
        container.print();
    }
    
    std::cout << "\n";
}

// ===== 2. 函数对象的类型擦除演示 =====
void demonstrate_function_type_erasure() {
    std::cout << "=== 函数对象的类型擦除演示 ===\n";
    
    // 简化的std::function实现
    template<typename>
    class MyFunction;
    
    template<typename R, typename... Args>
    class MyFunction<R(Args...)> {
    private:
        // 函数概念基类
        struct CallableBase {
            virtual ~CallableBase() = default;
            virtual std::unique_ptr<CallableBase> clone() const = 0;
            virtual R call(Args... args) = 0;
            virtual const std::type_info& target_type() const = 0;
        };
        
        // 具体可调用对象包装
        template<typename F>
        struct CallableModel : CallableBase {
            F func;
            
            explicit CallableModel(F f) : func(std::move(f)) {}
            
            std::unique_ptr<CallableBase> clone() const override {
                return std::make_unique<CallableModel>(func);
            }
            
            R call(Args... args) override {
                if constexpr (std::is_void_v<R>) {
                    func(std::forward<Args>(args)...);
                } else {
                    return func(std::forward<Args>(args)...);
                }
            }
            
            const std::type_info& target_type() const override {
                return typeid(F);
            }
        };
        
        std::unique_ptr<CallableBase> callable_;
        
    public:
        // 默认构造
        MyFunction() = default;
        
        // 从可调用对象构造
        template<typename F>
        MyFunction(F&& f) 
            : callable_(std::make_unique<CallableModel<std::decay_t<F>>>(std::forward<F>(f))) {
            std::cout << "MyFunction构造，目标类型: " << typeid(F).name() << "\n";
        }
        
        // 拷贝构造
        MyFunction(const MyFunction& other)
            : callable_(other.callable_ ? other.callable_->clone() : nullptr) {}
        
        // 移动构造
        MyFunction(MyFunction&&) = default;
        
        // 赋值操作符
        MyFunction& operator=(const MyFunction& other) {
            if (this != &other) {
                callable_ = other.callable_ ? other.callable_->clone() : nullptr;
            }
            return *this;
        }
        
        MyFunction& operator=(MyFunction&&) = default;
        
        // 函数调用操作符
        R operator()(Args... args) {
            if (!callable_) {
                throw std::runtime_error("调用空的MyFunction对象");
            }
            
            if constexpr (std::is_void_v<R>) {
                callable_->call(std::forward<Args>(args)...);
            } else {
                return callable_->call(std::forward<Args>(args)...);
            }
        }
        
        // 工具方法
        explicit operator bool() const { return callable_ != nullptr; }
        
        const std::type_info& target_type() const {
            return callable_ ? callable_->target_type() : typeid(void);
        }
    };
    
    // 测试不同类型的可调用对象
    std::cout << "1. Lambda表达式:\n";
    MyFunction<int(int, int)> add_func = [](int a, int b) -> int {
        std::cout << "Lambda计算: " << a << " + " << b << " = ";
        return a + b;
    };
    
    int result = add_func(3, 4);
    std::cout << result << "\n";
    
    std::cout << "\n2. 函数指针:\n";
    auto multiply = [](int a, int b) -> int { return a * b; };
    MyFunction<int(int, int)> mult_func = multiply;
    std::cout << "函数对象计算: 5 * 6 = " << mult_func(5, 6) << "\n";
    
    std::cout << "\n3. 函数对象类:\n";
    struct Divider {
        int operator()(int a, int b) const {
            if (b != 0) {
                std::cout << "除法计算: " << a << " / " << b << " = ";
                return a / b;
            }
            throw std::runtime_error("除零错误");
        }
    };
    
    MyFunction<int(int, int)> div_func = Divider{};
    std::cout << div_func(15, 3) << "\n";
    
    std::cout << "\n4. void返回类型:\n";
    MyFunction<void(const std::string&)> print_func = [](const std::string& msg) {
        std::cout << "打印消息: " << msg << "\n";
    };
    
    print_func("Hello, Type Erasure!");
    
    std::cout << "\n";
}

// ===== 3. 任意类型容器设计演示 =====
void demonstrate_any_type_container() {
    std::cout << "=== 任意类型容器设计演示 ===\n";
    
    // 自定义Any实现（简化版）
    class MyAny {
    private:
        // 存储概念
        struct StorageBase {
            virtual ~StorageBase() = default;
            virtual std::unique_ptr<StorageBase> clone() const = 0;
            virtual const std::type_info& type() const = 0;
            virtual void* get_ptr() = 0;
            virtual const void* get_ptr() const = 0;
        };
        
        template<typename T>
        struct StorageImpl : StorageBase {
            T value;
            
            template<typename U>
            StorageImpl(U&& v) : value(std::forward<U>(v)) {}
            
            std::unique_ptr<StorageBase> clone() const override {
                return std::make_unique<StorageImpl<T>>(value);
            }
            
            const std::type_info& type() const override {
                return typeid(T);
            }
            
            void* get_ptr() override { return &value; }
            const void* get_ptr() const override { return &value; }
        };
        
        std::unique_ptr<StorageBase> storage_;
        
    public:
        // 构造函数
        MyAny() = default;
        
        template<typename T, 
                 typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, MyAny>>>
        MyAny(T&& value) 
            : storage_(std::make_unique<StorageImpl<std::decay_t<T>>>(std::forward<T>(value))) {
            std::cout << "MyAny存储类型: " << typeid(T).name() << "\n";
        }
        
        // 拷贝语义
        MyAny(const MyAny& other) 
            : storage_(other.storage_ ? other.storage_->clone() : nullptr) {}
        
        MyAny& operator=(const MyAny& other) {
            if (this != &other) {
                storage_ = other.storage_ ? other.storage_->clone() : nullptr;
            }
            return *this;
        }
        
        // 移动语义
        MyAny(MyAny&&) = default;
        MyAny& operator=(MyAny&&) = default;
        
        // 工具方法
        bool has_value() const { return storage_ != nullptr; }
        
        const std::type_info& type() const {
            return storage_ ? storage_->type() : typeid(void);
        }
        
        void reset() { storage_.reset(); }
        
        // 类型安全的值获取
        template<typename T>
        T& any_cast() {
            if (!storage_) {
                throw std::runtime_error("any_cast: 空MyAny对象");
            }
            
            if (storage_->type() != typeid(T)) {
                throw std::runtime_error("any_cast: 类型不匹配");
            }
            
            return *static_cast<T*>(storage_->get_ptr());
        }
        
        template<typename T>
        const T& any_cast() const {
            if (!storage_) {
                throw std::runtime_error("any_cast: 空MyAny对象");
            }
            
            if (storage_->type() != typeid(T)) {
                throw std::runtime_error("any_cast: 类型不匹配");
            }
            
            return *static_cast<const T*>(storage_->get_ptr());
        }
    };
    
    // 测试自定义Any
    std::vector<MyAny> any_container;
    
    any_container.emplace_back(42);
    any_container.emplace_back(std::string("Hello Any"));
    any_container.emplace_back(3.14159);
    any_container.emplace_back(std::vector<int>{10, 20, 30});
    
    std::cout << "\n异构容器内容:\n";
    for (size_t i = 0; i < any_container.size(); ++i) {
        const auto& item = any_container[i];
        std::cout << "索引 " << i << " - 类型: " << item.type().name();
        
        // 尝试不同类型的转换
        if (item.type() == typeid(int)) {
            std::cout << ", 值: " << item.any_cast<int>();
        } else if (item.type() == typeid(std::string)) {
            std::cout << ", 值: '" << item.any_cast<std::string>() << "'";
        } else if (item.type() == typeid(double)) {
            std::cout << ", 值: " << item.any_cast<double>();
        } else if (item.type() == typeid(std::vector<int>)) {
            const auto& vec = item.any_cast<std::vector<int>>();
            std::cout << ", 值: [";
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) std::cout << ", ";
                std::cout << vec[j];
            }
            std::cout << "]";
        }
        
        std::cout << "\n";
    }
    
    // 类型安全检查
    std::cout << "\n类型安全测试:\n";
    MyAny test_any = 123;
    
    try {
        int value = test_any.any_cast<int>();
        std::cout << "正确类型转换: " << value << "\n";
        
        // 错误类型转换（会抛出异常）
        std::string wrong_value = test_any.any_cast<std::string>();
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << "\n";
    }
    
    std::cout << "\n";
}

// ===== 4. 接口类型擦除模式演示 =====
void demonstrate_interface_type_erasure() {
    std::cout << "=== 接口类型擦除模式演示 ===\n";
    
    // 绘图接口的类型擦除实现
    class DrawableTypeErased {
    private:
        // 绘图概念接口
        struct DrawableConcept {
            virtual ~DrawableConcept() = default;
            virtual std::unique_ptr<DrawableConcept> clone() const = 0;
            virtual void draw() const = 0;
            virtual double area() const = 0;
            virtual std::string name() const = 0;
        };
        
        // 具体类型模型
        template<typename T>
        struct DrawableModel : DrawableConcept {
            T object;
            
            explicit DrawableModel(T obj) : object(std::move(obj)) {}
            
            std::unique_ptr<DrawableConcept> clone() const override {
                return std::make_unique<DrawableModel>(object);
            }
            
            void draw() const override {
                object.draw();
            }
            
            double area() const override {
                return object.area();
            }
            
            std::string name() const override {
                return object.name();
            }
        };
        
        std::unique_ptr<DrawableConcept> concept_;
        
    public:
        // 构造函数模板
        template<typename T>
        DrawableTypeErased(T drawable) 
            : concept_(std::make_unique<DrawableModel<T>>(std::move(drawable))) {}
        
        // 拷贝语义
        DrawableTypeErased(const DrawableTypeErased& other)
            : concept_(other.concept_ ? other.concept_->clone() : nullptr) {}
        
        DrawableTypeErased& operator=(const DrawableTypeErased& other) {
            if (this != &other) {
                concept_ = other.concept_ ? other.concept_->clone() : nullptr;
            }
            return *this;
        }
        
        // 移动语义
        DrawableTypeErased(DrawableTypeErased&&) = default;
        DrawableTypeErased& operator=(DrawableTypeErased&&) = default;
        
        // 接口方法
        void draw() const {
            if (concept_) concept_->draw();
        }
        
        double area() const {
            return concept_ ? concept_->area() : 0.0;
        }
        
        std::string name() const {
            return concept_ ? concept_->name() : "Empty";
        }
    };
    
    // 具体形状类型（不需要继承）
    struct Circle {
        double radius;
        
        explicit Circle(double r) : radius(r) {}
        
        void draw() const {
            std::cout << "绘制圆形，半径: " << radius;
        }
        
        double area() const {
            return 3.14159 * radius * radius;
        }
        
        std::string name() const {
            return "Circle";
        }
    };
    
    struct Rectangle {
        double width, height;
        
        Rectangle(double w, double h) : width(w), height(h) {}
        
        void draw() const {
            std::cout << "绘制矩形，尺寸: " << width << "x" << height;
        }
        
        double area() const {
            return width * height;
        }
        
        std::string name() const {
            return "Rectangle";
        }
    };
    
    struct Triangle {
        double base, height;
        
        Triangle(double b, double h) : base(b), height(h) {}
        
        void draw() const {
            std::cout << "绘制三角形，底: " << base << ", 高: " << height;
        }
        
        double area() const {
            return 0.5 * base * height;
        }
        
        std::string name() const {
            return "Triangle";
        }
    };
    
    // 测试接口类型擦除
    std::vector<DrawableTypeErased> shapes;
    
    shapes.emplace_back(Circle{5.0});
    shapes.emplace_back(Rectangle{4.0, 6.0});
    shapes.emplace_back(Triangle{3.0, 8.0});
    
    std::cout << "绘制所有形状:\n";
    double total_area = 0.0;
    
    for (const auto& shape : shapes) {
        std::cout << shape.name() << " - ";
        shape.draw();
        std::cout << ", 面积: " << shape.area() << "\n";
        total_area += shape.area();
    }
    
    std::cout << "总面积: " << total_area << "\n\n";
}

// ===== 5. 性能优化技术演示 =====
void demonstrate_performance_optimization() {
    std::cout << "=== 性能优化技术演示 ===\n";
    
    // 小对象优化的Any实现
    template<size_t BufferSize = 32>
    class OptimizedAny {
    private:
        // 内存管理策略
        enum class StorageType { Empty, Small, Large };
        
        struct VTable {
            void (*destroy)(void*);
            void (*copy)(void*, const void*);
            void (*move)(void*, void*);
            const std::type_info* (*type)();
        };
        
        template<typename T>
        static const VTable* get_vtable() {
            static const VTable vtable = {
                // destroy
                [](void* ptr) {
                    static_cast<T*>(ptr)->~T();
                },
                // copy
                [](void* dst, const void* src) {
                    new (dst) T(*static_cast<const T*>(src));
                },
                // move
                [](void* dst, void* src) {
                    new (dst) T(std::move(*static_cast<T*>(src)));
                },
                // type
                []() -> const std::type_info* {
                    return &typeid(T);
                }
            };
            return &vtable;
        }
        
        StorageType storage_type_;
        const VTable* vtable_;
        
        union {
            alignas(std::max_align_t) char buffer_[BufferSize];
            void* ptr_;
        };
        
        template<typename T>
        static constexpr bool is_small() {
            return sizeof(T) <= BufferSize && 
                   alignof(T) <= alignof(std::max_align_t) &&
                   std::is_nothrow_move_constructible_v<T>;
        }
        
    public:
        OptimizedAny() : storage_type_(StorageType::Empty), vtable_(nullptr) {}
        
        template<typename T>
        OptimizedAny(T&& value) {
            using DecayedT = std::decay_t<T>;
            vtable_ = get_vtable<DecayedT>();
            
            if constexpr (is_small<DecayedT>()) {
                storage_type_ = StorageType::Small;
                new (buffer_) DecayedT(std::forward<T>(value));
                std::cout << "小对象优化存储: " << typeid(DecayedT).name() 
                         << " (大小: " << sizeof(DecayedT) << ")\n";
            } else {
                storage_type_ = StorageType::Large;
                ptr_ = new DecayedT(std::forward<T>(value));
                std::cout << "堆内存存储: " << typeid(DecayedT).name() 
                         << " (大小: " << sizeof(DecayedT) << ")\n";
            }
        }
        
        ~OptimizedAny() {
            reset();
        }
        
        OptimizedAny(const OptimizedAny& other) 
            : storage_type_(other.storage_type_), vtable_(other.vtable_) {
            
            if (storage_type_ == StorageType::Small) {
                vtable_->copy(buffer_, other.buffer_);
            } else if (storage_type_ == StorageType::Large) {
                // 对于大对象，需要重新分配内存并拷贝
                // 这里简化处理，实际实现会更复杂
                throw std::runtime_error("大对象拷贝未完全实现");
            }
        }
        
        void reset() {
            if (vtable_) {
                if (storage_type_ == StorageType::Small) {
                    vtable_->destroy(buffer_);
                } else if (storage_type_ == StorageType::Large) {
                    vtable_->destroy(ptr_);
                    delete static_cast<char*>(ptr_);
                }
            }
            storage_type_ = StorageType::Empty;
            vtable_ = nullptr;
        }
        
        bool has_value() const {
            return storage_type_ != StorageType::Empty;
        }
        
        const std::type_info& type() const {
            return vtable_ ? *vtable_->type() : typeid(void);
        }
        
        template<typename T>
        T& any_cast() {
            if (!has_value() || type() != typeid(T)) {
                throw std::runtime_error("any_cast失败");
            }
            
            if (storage_type_ == StorageType::Small) {
                return *reinterpret_cast<T*>(buffer_);
            } else {
                return *static_cast<T*>(ptr_);
            }
        }
    };
    
    // 性能基准测试
    const int iterations = 100000;
    
    // 测试小对象性能
    std::cout << "\n小对象性能测试:\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        OptimizedAny<32> small_any(i);
        int value = small_any.any_cast<int>();
        (void)value;  // 防止优化
    }
    
    auto small_time = std::chrono::high_resolution_clock::now() - start;
    
    // 测试大对象性能
    std::cout << "\n大对象性能测试:\n";
    struct LargeObject {
        char data[1024];  // 超过缓冲区大小
        int value;
        
        LargeObject(int v) : value(v) {
            std::fill(std::begin(data), std::end(data), 'x');
        }
    };
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        OptimizedAny<32> large_any(LargeObject{i});
        // 注意：大对象的拷贝构造未完全实现，这里只测试构造性能
    }
    
    auto large_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "\n性能结果:\n";
    std::cout << "小对象处理时间: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(small_time).count() 
              << " 微秒\n";
    std::cout << "大对象处理时间: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(large_time).count() 
              << " 微秒\n";
    
    double ratio = static_cast<double>(large_time.count()) / small_time.count();
    std::cout << "性能比率 (大对象/小对象): " << ratio << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++11/14/17/20 类型擦除技术深度解析\n";
    std::cout << "==================================\n";
    
    demonstrate_type_erasure_basics();
    demonstrate_function_type_erasure();
    demonstrate_any_type_container();
    demonstrate_interface_type_erasure();
    demonstrate_performance_optimization();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 10_type_erasure.cpp -o type_erasure
./type_erasure

关键学习点:
1. 类型擦除通过虚函数实现运行时多态，隐藏具体类型
2. std::function和std::any的实现原理基于类型擦除技术
3. 接口类型擦除可以实现无侵入式的多态设计
4. 小对象优化是重要的性能优化技术
5. 类型擦除在泛型编程中提供了运行时灵活性

注意事项:
- 类型擦除会引入运行时开销（虚函数调用、内存分配）
- 需要合理权衡编译时多态和运行时多态的选择
- 小对象优化可以显著提升性能，但实现复杂
- 类型安全检查在运行时进行，编译时无法发现类型错误
*/