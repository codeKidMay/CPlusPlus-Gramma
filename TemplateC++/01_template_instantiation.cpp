/**
 * 第1章：模板实例化的底层机制深度解析
 * 
 * 核心概念：
 * 1. 两阶段编译 - 模板定义检查和实例化时检查的分离
 * 2. 实例化时机 - 隐式实例化、显式实例化和实例化点
 * 3. ODR规则 - One Definition Rule在模板中的特殊应用
 * 4. 依赖名称查找 - 模板中名称查找的复杂性
 * 5. 诊断技巧 - 理解和调试模板编译错误
 */

#include <iostream>
#include <vector>
#include <type_traits>
#include <string>
#include <memory>
#include <chrono>

// ===== 1. 两阶段编译机制演示 =====
void demonstrate_two_phase_compilation() {
    std::cout << "=== 两阶段编译机制演示 ===\n";
    
    // 阶段一：模板定义时的语法检查
    // 编译器在此阶段检查模板语法，但不检查依赖类型的操作
    template<typename T>
    class TwoPhaseDemo {
        T data;
        // int invalid_syntax = ; // 这会在第一阶段被发现
        
    public:
        void non_dependent_function() {
            // 非依赖代码在第一阶段检查
            int x = 42;  // 这个语法检查在第一阶段完成
            // unknown_function(); // 这会在第一阶段报错
        }
        
        void dependent_function() {
            // 依赖代码只在实例化时检查（第二阶段）
            data.some_method(); // 是否存在some_method只有实例化时才知道
        }
        
        // 演示依赖名称的延迟检查
        template<typename U>
        void template_member_function() {
            T::nested_type x;  // T::nested_type的存在性在实例化时检查
            data.template some_template_method<U>(); // template关键字消歧义
        }
    };
    
    // 第二阶段：实例化时的完整类型检查
    // 只有在实例化时，编译器才会检查T是否支持相应操作
    
    class ValidType {
    public:
        using nested_type = int;
        void some_method() { std::cout << "ValidType::some_method called\n"; }
        template<typename U> void some_template_method() { 
            std::cout << "Template method with type: " << typeid(U).name() << "\n"; 
        }
    };
    
    TwoPhaseDemo<ValidType> valid_demo;
    valid_demo.non_dependent_function();
    valid_demo.dependent_function();
    valid_demo.template_member_function<int>();
    
    // TwoPhaseDemo<int> invalid_demo; // 如果调用dependent_function会编译失败
    
    std::cout << "\n";
}

// ===== 2. 实例化时机和策略 =====
void demonstrate_instantiation_timing() {
    std::cout << "=== 实例化时机和策略 ===\n";
    
    // 隐式实例化：按需实例化
    template<typename T>
    class LazyInstantiation {
    public:
        static void used_method() {
            std::cout << "LazyInstantiation::used_method for " << typeid(T).name() << "\n";
        }
        
        static void unused_method() {
            // 这个方法如果没被调用，即使T不支持某些操作也不会编译错误
            T::non_existent_member = 42; // 只有调用时才会检查
        }
        
        void conditional_usage() {
            if constexpr (std::is_arithmetic_v<T>) {
                std::cout << "Arithmetic type: " << sizeof(T) << " bytes\n";
            } else {
                std::cout << "Non-arithmetic type\n";
            }
        }
    };
    
    // 隐式实例化示例
    LazyInstantiation<int>::used_method(); // 只实例化used_method
    LazyInstantiation<double> double_instance;
    double_instance.conditional_usage();
    
    // LazyInstantiation<int>::unused_method(); // 这行取消注释会导致编译错误
    
    // 显式实例化声明和定义
    template class LazyInstantiation<std::string>; // 显式实例化定义
    extern template class LazyInstantiation<char>; // 显式实例化声明
    
    LazyInstantiation<std::string> string_instance;
    string_instance.conditional_usage();
    
    std::cout << "\n";
}

// ===== 3. 实例化点(POI)和ODR规则 =====
namespace instantiation_point_demo {
    
    template<typename T>
    void template_function(T value) {
        helper_function(value); // 依赖名称查找
    }
    
    void helper_function(int x) {
        std::cout << "helper_function(int): " << x << "\n";
    }
    
    void demonstrate_poi() {
        std::cout << "=== 实例化点和ODR规则演示 ===\n";
        
        // 实例化点的重要性
        template_function(42); // 实例化点在此处
        
        // 在实例化点之后定义的函数也能被找到（依赖名称查找）
        struct CustomType {
            int value = 100;
        };
        
        template_function(CustomType{});
        
        std::cout << "\n";
    }
}

// CustomType的helper_function必须在实例化点之前定义或在ADL中找到
void helper_function(const instantiation_point_demo::CustomType& ct) {
    std::cout << "helper_function(CustomType): " << ct.value << "\n";
}

// ===== 4. 依赖名称查找复杂性 =====
void demonstrate_dependent_name_lookup() {
    std::cout << "=== 依赖名称查找复杂性演示 ===\n";
    
    template<typename T>
    class DependentLookupDemo {
    public:
        // 依赖类型名称需要typename关键字
        using value_type = typename T::value_type;
        using size_type = typename T::size_type;
        
        void demonstrate_typename() {
            // typename指示编译器这是类型名称，而不是静态成员变量
            typename T::iterator it; // 正确的依赖类型名称声明
            
            // T::iterator it; // 错误：编译器不知道iterator是类型还是变量
        }
        
        void demonstrate_template_keyword() {
            T container;
            
            // template关键字用于依赖模板名称的消歧义
            // container.template emplace<int>(42); // 如果T有emplace模板方法
            
            // 在->或.之后使用template关键字
            auto ptr = &container;
            // ptr->template some_template_method<int>();
        }
        
        void demonstrate_adl() {
            // 参数依赖查找(ADL/Koenig查找)
            T container;
            // swap(container, another_container); // ADL会在T的命名空间中查找swap
        }
    };
    
    // 使用标准容器演示
    DependentLookupDemo<std::vector<int>> demo;
    demo.demonstrate_typename();
    
    std::cout << "\n";
}

// ===== 5. 模板编译错误诊断技巧 =====
void demonstrate_error_diagnosis() {
    std::cout << "=== 模板编译错误诊断技巧 ===\n";
    
    // 技巧1：使用static_assert进行编译期断言
    template<typename T>
    class DiagnosisDemo {
        // 在模板开始就检查类型要求
        static_assert(std::is_copy_constructible_v<T>, 
                     "DiagnosisDemo requires T to be copy constructible");
        static_assert(sizeof(T) >= 4, 
                     "DiagnosisDemo requires T to be at least 4 bytes");
        
        T data;
        
    public:
        // 技巧2：使用SFINAE友好的设计
        template<typename U = T>
        auto get_size() -> decltype(std::declval<U>().size()) {
            return data.size();
        }
        
        // 技巧3：提供清晰的错误信息
        template<typename U = T>
        std::enable_if_t<std::is_arithmetic_v<U>, U> get_value() {
            return data;
        }
        
        template<typename U = T>
        std::enable_if_t<!std::is_arithmetic_v<U>, std::string> get_description() {
            return "Non-arithmetic type";
        }
    };
    
    // 正确使用
    DiagnosisDemo<std::string> string_demo;
    if constexpr (requires { string_demo.get_size(); }) {
        std::cout << "String has size method\n";
    }
    
    DiagnosisDemo<int> int_demo;
    std::cout << "Int value: " << int_demo.get_value() << "\n";
    
    // 错误示例（注释掉避免编译错误）
    // DiagnosisDemo<char> char_demo; // 编译错误：char只有1字节
    
    std::cout << "\n";
}

// ===== 6. 实例化性能分析 =====
void demonstrate_instantiation_performance() {
    std::cout << "=== 实例化性能分析 ===\n";
    
    // 模板实例化的编译期开销
    template<int N>
    struct CompileTimeFactorial {
        static constexpr int value = N * CompileTimeFactorial<N-1>::value;
    };
    
    template<>
    struct CompileTimeFactorial<0> {
        static constexpr int value = 1;
    };
    
    // 测量编译期计算
    auto start = std::chrono::high_resolution_clock::now();
    
    constexpr int fact10 = CompileTimeFactorial<10>::value;
    constexpr int fact15 = CompileTimeFactorial<15>::value;
    
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "10! = " << fact10 << "\n";
    std::cout << "15! = " << fact15 << "\n";
    
    // 对比模板实例化数量
    template<typename... Types>
    struct CountInstantiations {
        static constexpr size_t count = sizeof...(Types);
        
        void print_info() {
            std::cout << "Template instantiated with " << count << " types\n";
            ((std::cout << "  Type: " << typeid(Types).name() << "\n"), ...);
        }
    };
    
    CountInstantiations<int, double, std::string> multi_type;
    multi_type.print_info();
    
    std::cout << "\n";
}

// ===== 7. 高级实例化控制技术 =====
void demonstrate_advanced_instantiation_control() {
    std::cout << "=== 高级实例化控制技术 ===\n";
    
    // 延迟实例化技术
    template<typename T>
    class LazyEvaluation {
        mutable std::unique_ptr<T> cached_result;
        
    public:
        template<typename... Args>
        const T& get(Args&&... args) const {
            if (!cached_result) {
                cached_result = std::make_unique<T>(std::forward<Args>(args)...);
            }
            return *cached_result;
        }
        
        void reset() const {
            cached_result.reset();
        }
    };
    
    // 条件实例化
    template<typename T, bool Condition = std::is_trivially_destructible_v<T>>
    struct ConditionalInstantiation;
    
    template<typename T>
    struct ConditionalInstantiation<T, true> {
        static void process(T& value) {
            std::cout << "Processing trivially destructible type\n";
        }
    };
    
    template<typename T>
    struct ConditionalInstantiation<T, false> {
        static void process(T& value) {
            std::cout << "Processing non-trivially destructible type\n";
            // 特殊处理逻辑
        }
    };
    
    LazyEvaluation<std::string> lazy_string;
    std::cout << "Lazy string: " << lazy_string.get("Hello, Template!") << "\n";
    
    int trivial_int = 42;
    std::string non_trivial_string = "test";
    
    ConditionalInstantiation<int>::process(trivial_int);
    ConditionalInstantiation<std::string>::process(non_trivial_string);
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "第1章：模板实例化的底层机制深度解析\n";
    std::cout << "==========================================\n";
    
    demonstrate_two_phase_compilation();
    demonstrate_instantiation_timing();
    instantiation_point_demo::demonstrate_poi();
    demonstrate_dependent_name_lookup();
    demonstrate_error_diagnosis();
    demonstrate_instantiation_performance();
    demonstrate_advanced_instantiation_control();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall -Wextra 01_template_instantiation.cpp -o template_instantiation
./template_instantiation

关键学习点:
1. 两阶段编译是理解模板错误的关键
2. 实例化时机影响编译性能和错误出现时间
3. 依赖名称查找规则复杂但有规律可循
4. typename和template关键字在模板中的消歧义作用
5. 合理的错误诊断设计能大幅提升开发体验

注意事项:
- 理解POI对名称查找的影响
- 掌握ODR在模板中的特殊规则  
- 使用编译期断言和SFINAE提供友好错误信息
- 避免不必要的模板实例化以提升编译性能
*/