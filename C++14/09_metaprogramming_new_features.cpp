/*
 * C++14 元编程新特性深度解析
 * 
 * C++14虽然主要是对C++11的完善，但在元编程方面也有重要改进：
 * 1. 变量模板（Variable Templates） - 类型特征的编译期值表示
 * 2. constexpr函数扩展 - 更强大的编译期计算能力
 * 3. 泛型Lambda捕获 - 元编程中的闭包增强
 * 4. 类型别名模板 - 简化元函数的写法
 * 5. 编译期数组大小推导 - std::size()等
 * 6. 更强大的std::integral_constant - 编译期常量
 * 
 * C++14元编程的核心价值：
 * - 将更多计算从运行时移到编译期
 * - 提供更好的编译期类型检查
 * - 减少运行时开销
 * - 实现更加类型安全的程序设计
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
#include <complex>
#include <ratio>
#include <chrono>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>

namespace cpp14_metaprogramming {

// ===== 1. 变量模板深度应用 =====

namespace VariableTemplates {
    // 标准库类型特征的变量模板版本
    template<typename T>
    constexpr bool is_integral_v = std::is_integral<T>::value;
    
    template<typename T>
    constexpr bool is_floating_point_v = std::is_floating_point<T>::value;
    
    template<typename T>
    constexpr bool is_pointer_v = std::is_pointer<T>::value;
    
    template<typename T>
    constexpr bool is_reference_v = std::is_reference<T>::value;
    
    template<typename T>
    constexpr bool is_const_v = std::is_const<T>::value;
    
    template<typename T>
    constexpr bool is_volatile_v = std::is_volatile<T>::value;
    
    template<typename T>
    constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;
    
    template<typename T>
    constexpr bool is_signed_v = std::is_signed<T>::value;
    
    template<typename T>
    constexpr bool is_unsigned_v = std::is_unsigned<T>::value;
    
    template<typename T>
    constexpr size_t alignment_v = alignof(T);
    
    template<typename T>
    constexpr size_t size_v = sizeof(T);
    
    // 数学常量的变量模板
    template<typename T = double>
    constexpr T pi_v = T(3.14159265358979323846);
    
    template<typename T = double>
    constexpr T e_v = T(2.71828182845904523536);
    
    template<typename T = double>
    constexpr T golden_ratio_v = T(1.61803398874989484820);
    
    template<typename T = double>
    constexpr T sqrt2_v = T(1.41421356237309504880);
    
    // 单位转换变量模板
    template<typename T = double>
    constexpr T degrees_to_radians_v = pi_v<T> / T(180);
    
    template<typename T = double>
    constexpr T radians_to_degrees_v = T(180) / pi_v<T>;
    
    // 物理常量
    template<typename T = double>
    constexpr T speed_of_light_v = T(299792458.0);  // m/s
    
    template<typename T = double>
    constexpr T planck_constant_v = T(6.62607015e-34);  // J⋅s
    
    template<typename T = double>
    constexpr T avogadro_number_v = T(6.02214076e23);  // mol⁻¹
    
    template<typename T = double>
    constexpr T boltzmann_constant_v = T(1.380649e-23);  // J/K
    
    // 编译期数学函数
    template<typename T>
    constexpr T abs_v(T x) {
        return x < 0 ? -x : x;
    }
    
    template<typename T>
    constexpr T min_v(T a, T b) {
        return a < b ? a : b;
    }
    
    template<typename T>
    constexpr T max_v(T a, T b) {
        return a > b ? a : b;
    }
    
    template<typename T>
    constexpr T clamp_v(T value, T min_val, T max_val) {
        return value < min_val ? min_val : (value > max_val ? max_val : value);
    }
    
    // 编译期幂运算
    template<typename T>
    constexpr T pow_v(T base, int exp) {
        return exp == 0 ? T(1) : 
               exp == 1 ? base :
               exp % 2 == 0 ? pow_v(base * base, exp / 2) :
               base * pow_v(base * base, (exp - 1) / 2);
    }
    
    // 编译期平方根（牛顿迭代法）
    template<typename T>
    constexpr T sqrt_helper_v(T x, T guess, T prev) {
        return guess == prev ? guess : sqrt_helper_v(x, (guess + x / guess) / 2, guess);
    }
    
    template<typename T>
    constexpr T sqrt_v(T x) {
        return x < 0 ? T(0) : sqrt_helper_v(x, x / 2, x);
    }
    
    // 编译期三角函数（泰勒级数展开）
    template<typename T>
    constexpr T sin_helper_v(T x, T term, T sum, int n, int max_terms) {
        return n > max_terms ? sum : 
               sin_helper_v(x, -term * x * x / ((2 * n) * (2 * n + 1)), 
                          sum + term, n + 1, max_terms);
    }
    
    template<typename T>
    constexpr T sin_v(T x) {
        // 将x归一化到[-π, π]范围
        x = x - (2 * pi_v<T> * static_cast<int>(x / (2 * pi_v<T>)));
        return sin_helper_v(x, x, 0, 1, 10);
    }
    
    template<typename T>
    constexpr T cos_v(T x) {
        return sin_v(x + pi_v<T> / 2);
    }
    
    // 编译期对数计算
    template<typename T>
    constexpr T log_helper_v(T x, T result, T prev_result, int iterations) {
        return iterations == 0 || abs_v(result - prev_result) < T(1e-10) ? 
               result : log_helper_v(x, result - (exp_v(result) - x) / exp_v(result), 
                                    result, iterations - 1);
    }
    
    template<typename T>
    constexpr T log_v(T x) {
        return x <= 0 ? T(0) : log_helper_v(x, 0, 1, 20);
    }
    
    template<typename T>
    constexpr T exp_v(T x) {
        T result = 1;
        T term = 1;
        for (int i = 1; i <= 20; ++i) {
            term *= x / i;
            result += term;
        }
        return result;
    }
    
    // 类型特征检查器
    template<typename T>
    struct TypeAnalyzer {
        static constexpr bool is_integral = is_integral_v<T>;
        static constexpr bool is_floating_point = is_floating_point_v<T>;
        static constexpr bool is_pointer = is_pointer_v<T>;
        static constexpr bool is_reference = is_reference_v<T>;
        static constexpr bool is_const = is_const_v<T>;
        static constexpr bool is_volatile = is_volatile_v<T>;
        static constexpr bool is_arithmetic = is_arithmetic_v<T>;
        static constexpr bool is_signed = is_signed_v<T>;
        static constexpr bool is_unsigned = is_unsigned_v<T>;
        static constexpr size_t size = size_v<T>;
        static constexpr size_t alignment = alignment_v<T>;
        
        static void analyze() {
            std::cout << "类型分析: " << typeid(T).name() << "\n";
            std::cout << "  是否为整数: " << is_integral << "\n";
            std::cout << "  是否为浮点数: " << is_floating_point << "\n";
            std::cout << "  是否为指针: " << is_pointer << "\n";
            std::cout << "  是否为引用: " << is_reference << "\n";
            std::cout << "  是否为const: " << is_const << "\n";
            std::cout << "  是否为volatile: " << is_volatile << "\n";
            std::cout << "  是否为算术类型: " << is_arithmetic << "\n";
            std::cout << "  是否为有符号: " << is_signed << "\n";
            std::cout << "  是否为无符号: " << is_unsigned << "\n";
            std::cout << "  大小: " << size << " 字节\n";
            std::cout << "  对齐: " << alignment << " 字节\n";
        }
    };
}

// ===== 2. 编译期算法和数据结构 =====

namespace CompileTimeAlgorithms {
    // 编译期数组大小推导
    template<typename T, size_t N>
    constexpr size_t array_size_v = N;
    
    template<typename T, size_t N>
    constexpr size_t array_size(const T (&)[N]) {
        return N;
    }
    
    // 编译期字符串长度
    constexpr size_t string_length(const char* str) {
        return *str == '\0' ? 0 : 1 + string_length(str + 1);
    }
    
    // 编译期字符串比较
    constexpr bool string_equal(const char* a, const char* b) {
        return *a == '\0' && *b == '\0' ? true :
               *a != *b ? false :
               string_equal(a + 1, b + 1);
    }
    
    // 编译期斐波那契数列
    template<int N>
    struct Fibonacci {
        static constexpr unsigned long long value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
    };
    
    template<>
    struct Fibonacci<0> {
        static constexpr unsigned long long value = 0;
    };
    
    template<>
    struct Fibonacci<1> {
        static constexpr unsigned long long value = 1;
    };
    
    // 变量模板版本
    template<int N>
    constexpr unsigned long long fibonacci_v = Fibonacci<N>::value;
    
    // 编译期阶乘
    template<int N>
    struct Factorial {
        static constexpr unsigned long long value = N * Factorial<N-1>::value;
    };
    
    template<>
    struct Factorial<0> {
        static constexpr unsigned long long value = 1;
    };
    
    template<int N>
    constexpr unsigned long long factorial_v = Factorial<N>::value;
    
    // 编译期最大公约数
    constexpr unsigned long long gcd(unsigned long long a, unsigned long long b) {
        return b == 0 ? a : gcd(b, a % b);
    }
    
    // 编译期最小公倍数
    constexpr unsigned long long lcm(unsigned long long a, unsigned long long b) {
        return a * b / gcd(a, b);
    }
    
    // 编译期素数检查
    constexpr bool is_prime(unsigned long long n) {
        return n <= 1 ? false :
               n == 2 ? true :
               n % 2 == 0 ? false :
               [&]() {
                   for (unsigned long long i = 3; i * i <= n; i += 2) {
                       if (n % i == 0) return false;
                   }
                   return true;
               }();
    }
    
    // 编译期数组求和
    template<typename T, size_t N>
    constexpr T array_sum(const T (&arr)[N]) {
        T sum = 0;
        for (size_t i = 0; i < N; ++i) {
            sum += arr[i];
        }
        return sum;
    }
    
    // 编译期数组最大值
    template<typename T, size_t N>
    constexpr T array_max(const T (&arr)[N]) {
        T max_val = arr[0];
        for (size_t i = 1; i < N; ++i) {
            if (arr[i] > max_val) {
                max_val = arr[i];
            }
        }
        return max_val;
    }
    
    // 编译期数组最小值
    template<typename T, size_t N>
    constexpr T array_min(const T (&arr)[N]) {
        T min_val = arr[0];
        for (size_t i = 1; i < N; ++i) {
            if (arr[i] < min_val) {
                min_val = arr[i];
            }
        }
        return min_val;
    }
    
    // 编译期数组查找
    template<typename T, size_t N>
    constexpr size_t array_find(const T (&arr)[N], T value) {
        for (size_t i = 0; i < N; ++i) {
            if (arr[i] == value) {
                return i;
            }
        }
        return N;  // 未找到
    }
    
    // 编译期排序（插入排序）
    template<typename T, size_t N>
    constexpr void insertion_sort_helper(T (&arr)[N], size_t i) {
        if (i >= N) return;
        
        T key = arr[i];
        int j = static_cast<int>(i) - 1;
        
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
        
        insertion_sort_helper(arr, i + 1);
    }
    
    template<typename T, size_t N>
    constexpr void array_sort(T (&arr)[N]) {
        insertion_sort_helper(arr, 1);
    }
    
    // 编译期矩阵运算
    template<typename T, size_t M, size_t N>
    class Matrix {
    private:
        T data_[M][N];
        
    public:
        constexpr Matrix() : data_{} {}
        
        constexpr T& operator()(size_t i, size_t j) { return data_[i][j]; }
        constexpr const T& operator()(size_t i, size_t j) const { return data_[i][j]; }
        
        constexpr size_t rows() const { return M; }
        constexpr size_t cols() const { return N; }
        
        // 编译期矩阵转置
        constexpr Matrix<T, N, M> transpose() const {
            Matrix<T, N, M> result;
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    result(j, i) = data_[i][j];
                }
            }
            return result;
        }
        
        // 编译期矩阵加法
        constexpr Matrix<T, M, N> operator+(const Matrix<T, M, N>& other) const {
            Matrix<T, M, N> result;
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    result(i, j) = data_[i][j] + other(i, j);
                }
            }
            return result;
        }
        
        // 编译期矩阵乘法
        template<size_t P>
        constexpr Matrix<T, M, P> operator*(const Matrix<T, N, P>& other) const {
            Matrix<T, M, P> result;
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < P; ++j) {
                    result(i, j) = 0;
                    for (size_t k = 0; k < N; ++k) {
                        result(i, j) += data_[i][k] * other(k, j);
                    }
                }
            }
            return result;
        }
    };
}

// ===== 3. 类型列表和元组操作 =====

namespace TypeListOperations {
    // 类型列表定义
    template<typename... Types>
    struct TypeList {
        using type = TypeList<Types...>;
        static constexpr size_t size = sizeof...(Types);
    };
    
    // 空类型列表
    struct EmptyTypeList {};
    
    // 类型列表操作
    template<typename List>
    struct Front;
    
    template<typename Head, typename... Tail>
    struct Front<TypeList<Head, Tail...>> {
        using type = Head;
    };
    
    template<typename List>
    using Front_t = typename Front<List>::type;
    
    template<typename List>
    struct PopFront;
    
    template<typename Head, typename... Tail>
    struct PopFront<TypeList<Head, Tail...>> {
        using type = TypeList<Tail...>;
    };
    
    template<typename List>
    using PopFront_t = typename PopFront<List>::type;
    
    template<typename List, typename T>
    struct PushFront;
    
    template<typename... Types, typename T>
    struct PushFront<TypeList<Types...>, T> {
        using type = TypeList<T, Types...>;
    };
    
    template<typename List, typename T>
    using PushFront_t = typename PushFront<List, T>::type;
    
    template<typename List, typename T>
    struct PushBack;
    
    template<typename... Types, typename T>
    struct PushBack<TypeList<Types...>, T> {
        using type = TypeList<Types..., T>;
    };
    
    template<typename List, typename T>
    using PushBack_t = typename PushBack<List, T>::type;
    
    // 类型查找
    template<typename List, typename T>
    struct Contains;
    
    template<typename T, typename... Types>
    struct Contains<TypeList<Types...>, T> 
        : std::disjunction<std::is_same<T, Types>...> {};
    
    template<typename List, typename T>
    constexpr bool Contains_v = Contains<List, T>::value;
    
    // 类型索引
    template<typename List, typename T, size_t Index = 0>
    struct IndexOf;
    
    template<typename T, typename First, typename... Rest, size_t Index>
    struct IndexOf<TypeList<First, Rest...>, T, Index> {
        static constexpr size_t value = 
            std::is_same_v<T, First> ? Index : IndexOf<TypeList<Rest...>, T, Index + 1>::value;
    };
    
    template<typename T, size_t Index>
    struct IndexOf<TypeList<>, T, Index> {
        static constexpr size_t value = static_cast<size_t>(-1);
    };
    
    template<typename List, typename T>
    constexpr size_t IndexOf_v = IndexOf<List, T>::value;
    
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
    
    // 类型连接
    template<typename List1, typename List2>
    struct Concat;
    
    template<typename... Types1, typename... Types2>
    struct Concat<TypeList<Types1...>, TypeList<Types2...>> {
        using type = TypeList<Types1..., Types2...>;
    };
    
    template<typename List1, typename List2>
    using Concat_t = typename Concat<List1, List2>::type;
    
    // 类型去重
    template<typename List>
    struct Unique;
    
    template<typename Head, typename... Tail>
    struct Unique<TypeList<Head, Tail...>> {
        using type = std::conditional_t<
            Contains_v<TypeList<Tail...>, Head>,
            typename Unique<TypeList<Tail...>>::type,
            PushFront_t<typename Unique<TypeList<Tail...>>::type, Head>
        >;
    };
    
    template<>
    struct Unique<TypeList<>> {
        using type = TypeList<>;
    };
    
    template<typename List>
    using Unique_t = typename Unique<List>::type;
    
    // 类型过滤器
    template<typename List, template<typename> class Predicate>
    struct Filter;
    
    template<typename Head, typename... Tail, template<typename> class Predicate>
    struct Filter<TypeList<Head, Tail...>, Predicate> {
        using type = std::conditional_t<
            Predicate<Head>::value,
            PushFront_t<typename Filter<TypeList<Tail...>, Predicate>::type, Head>,
            typename Filter<TypeList<Tail...>, Predicate>::type
        >;
    };
    
    template<template<typename> class Predicate>
    struct Filter<TypeList<>, Predicate> {
        using type = TypeList<>;
    };
    
    template<typename List, template<typename> class Predicate>
    using Filter_t = typename Filter<List, Predicate>::type;
    
    // 类型转换器
    template<typename List, template<typename> class Transform>
    struct TransformTypes;
    
    template<typename... Types, template<typename> class Transform>
    struct TransformTypes<TypeList<Types...>, Transform> {
        using type = TypeList<typename Transform<Types>::type...>;
    };
    
    template<typename List, template<typename> class Transform>
    using TransformTypes_t = typename TransformTypes<List, Transform>::type;
    
    // 元组操作
    template<typename Tuple, typename Func>
    constexpr void tuple_for_each(Tuple&& tuple, Func&& func) {
        std::apply([&func](auto&&... args) {
            (func(std::forward<decltype(args)>(args)), ...);
        }, std::forward<Tuple>(tuple));
    }
    
    template<typename Tuple, typename Func>
    constexpr auto tuple_transform(Tuple&& tuple, Func&& func) {
        return std::apply([&func](auto&&... args) {
            return std::make_tuple(func(std::forward<decltype(args)>(args))...);
        }, std::forward<Tuple>(tuple));
    }
    
    template<typename Tuple, typename Func>
    constexpr auto tuple_filter(Tuple&& tuple, Func&& func) {
        return std::apply([&func](auto&&... args) {
            return std::make_tuple(func(std::forward<decltype(args)>(args))...);
        }, std::forward<Tuple>(tuple));
    }
}

// ===== 4. 编译期策略和模式 =====

namespace CompileTimePatterns {
    // 编译期策略选择
    template<typename T>
    struct SortingStrategy {
        using type = void;  // 默认策略
    };
    
    template<>
    struct SortingStrategy<int> {
        using type = std::integral_constant<int, 1>;  // 快速排序
    };
    
    template<>
    struct SortingStrategy<double> {
        using type = std::integral_constant<int, 2>;  // 归并排序
    };
    
    template<typename T>
    using SortingStrategy_t = typename SortingStrategy<T>::type;
    
    // 编译期工厂模式
    template<typename T>
    struct Creator {
        static T* create() { return new T(); }
    };
    
    template<typename T>
    struct Factory {
        static T* create() {
            return Creator<T>::create();
        }
    };
    
    // 编译期访问者模式
    template<typename... Types>
    struct Visitor;
    
    template<typename T, typename... Types>
    struct Visitor<T, Types...> {
        virtual void visit(const T&) = 0;
        virtual ~Visitor() = default;
    };
    
    template<typename T>
    struct Visitable {
        virtual void accept(Visitor<T>& visitor) const = 0;
        virtual ~Visitable() = default;
    };
    
    // 编译期观察者模式
    template<typename T>
    class Observer {
    public:
        virtual void on_notify(const T& data) = 0;
        virtual ~Observer() = default;
    };
    
    template<typename T>
    class Subject {
    private:
        std::vector<Observer<T>*> observers_;
        
    public:
        void attach(Observer<T>* observer) {
            observers_.push_back(observer);
        }
        
        void detach(Observer<T>* observer) {
            observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), 
                            observers_.end());
        }
        
        void notify(const T& data) {
            for (auto* observer : observers_) {
                observer->on_notify(data);
            }
        }
    };
    
    // 编译期策略模式
    template<typename T>
    class CompressionStrategy {
    public:
        virtual std::vector<uint8_t> compress(const T& data) = 0;
        virtual ~CompressionStrategy() = default;
    };
    
    template<typename T>
    class NoCompression : public CompressionStrategy<T> {
    public:
        std::vector<uint8_t> compress(const T& data) override {
            return std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(&data), 
                                       reinterpret_cast<const uint8_t*>(&data) + sizeof(T));
        }
    };
    
    template<typename T>
    class RLECompression : public CompressionStrategy<T> {
    public:
        std::vector<uint8_t> compress(const T& data) override {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&data);
            std::vector<uint8_t> compressed;
            
            for (size_t i = 0; i < sizeof(T); ) {
                uint8_t current = bytes[i];
                size_t count = 1;
                
                while (i + count < sizeof(T) && bytes[i + count] == current && count < 255) {
                    count++;
                }
                
                compressed.push_back(static_cast<uint8_t>(count));
                compressed.push_back(current);
                
                i += count;
            }
            
            return compressed;
        }
    };
    
    // 编译期策略工厂
    template<typename T, int Strategy>
    class CompressionFactory {
    public:
        static std::unique_ptr<CompressionStrategy<T>> create() {
            if constexpr (Strategy == 0) {
                return std::make_unique<NoCompression<T>>();
            } else if constexpr (Strategy == 1) {
                return std::make_unique<RLECompression<T>>();
            } else {
                return std::make_unique<NoCompression<T>>();
            }
        }
    };
}

// ===== 5. 编译期容器和算法 =====

namespace CompileTimeContainers {
    // 编译期固定大小数组
    template<typename T, size_t Size>
    class FixedArray {
    private:
        T data_[Size];
        
    public:
        constexpr FixedArray() : data_{} {}
        
        constexpr T& operator[](size_t index) { return data_[index]; }
        constexpr const T& operator[](size_t index) const { return data_[index]; }
        
        constexpr size_t size() const { return Size; }
        constexpr bool empty() const { return Size == 0; }
        
        constexpr T* begin() { return data_; }
        constexpr const T* begin() const { return data_; }
        constexpr T* end() { return data_ + Size; }
        constexpr const T* end() const { return data_ + Size; }
        
        constexpr T& front() { return data_[0]; }
        constexpr const T& front() const { return data_[0]; }
        constexpr T& back() { return data_[Size - 1]; }
        constexpr const T& back() const { return data_[Size - 1]; }
        
        constexpr void fill(const T& value) {
            for (size_t i = 0; i < Size; ++i) {
                data_[i] = value;
            }
        }
        
        constexpr void swap(FixedArray& other) {
            for (size_t i = 0; i < Size; ++i) {
                std::swap(data_[i], other.data_[i]);
            }
        }
    };
    
    // 编译期位集合
    template<size_t Size>
    class BitSet {
    private:
        using WordType = uint64_t;
        static constexpr size_t WordSize = sizeof(WordType) * 8;
        static constexpr size_t WordCount = (Size + WordSize - 1) / WordSize;
        
        WordType words_[WordCount] = {};
        
        constexpr size_t word_index(size_t pos) const {
            return pos / WordSize;
        }
        
        constexpr size_t bit_index(size_t pos) const {
            return pos % WordSize;
        }
        
    public:
        constexpr BitSet() = default;
        
        constexpr bool test(size_t pos) const {
            if (pos >= Size) return false;
            return (words_[word_index(pos)] & (WordType(1) << bit_index(pos))) != 0;
        }
        
        constexpr void set(size_t pos, bool value = true) {
            if (pos >= Size) return;
            if (value) {
                words_[word_index(pos)] |= (WordType(1) << bit_index(pos));
            } else {
                words_[word_index(pos)] &= ~(WordType(1) << bit_index(pos));
            }
        }
        
        constexpr void reset(size_t pos) {
            set(pos, false);
        }
        
        constexpr void flip(size_t pos) {
            if (pos >= Size) return;
            words_[word_index(pos)] ^= (WordType(1) << bit_index(pos));
        }
        
        constexpr size_t count() const {
            size_t result = 0;
            for (size_t i = 0; i < WordCount; ++i) {
                result += __builtin_popcountll(words_[i]);
            }
            return result;
        }
        
        constexpr bool all() const {
            return count() == Size;
        }
        
        constexpr bool any() const {
            return count() > 0;
        }
        
        constexpr bool none() const {
            return count() == 0;
        }
    };
    
    // 编译期字符串
    template<size_t Size>
    class FixedString {
    private:
        char data_[Size + 1] = {};  // +1 for null terminator
        
    public:
        constexpr FixedString() = default;
        
        constexpr FixedString(const char (&str)[Size + 1]) {
            for (size_t i = 0; i < Size; ++i) {
                data_[i] = str[i];
            }
            data_[Size] = '\0';
        }
        
        constexpr char& operator[](size_t index) { return data_[index]; }
        constexpr const char& operator[](size_t index) const { return data_[index]; }
        
        constexpr size_t size() const { return Size; }
        constexpr const char* c_str() const { return data_; }
        
        constexpr bool operator==(const FixedString& other) const {
            for (size_t i = 0; i < Size; ++i) {
                if (data_[i] != other.data_[i]) {
                    return false;
                }
            }
            return true;
        }
        
        constexpr bool operator!=(const FixedString& other) const {
            return !(*this == other);
        }
    };
}

} // namespace cpp14_metaprogramming

// ===== 主函数 =====

int main() {
    std::cout << "=== C++14 元编程新特性深度解析 ===\n";
    
    using namespace cpp14_metaprogramming;
    
    // 1. 变量模板演示
    std::cout << "\n===== 1. 变量模板演示 =====\n";
    using namespace VariableTemplates;
    
    std::cout << "数学常量:\n";
    std::cout << "π = " << pi_v<double> << "\n";
    std::cout << "e = " << e_v<double> << "\n";
    std::cout << "黄金比例 = " << golden_ratio_v<double> << "\n";
    std::cout << "√2 = " << sqrt2_v<double> << "\n";
    
    std::cout << "\n单位转换:\n";
    std::cout << "90度 = " << 90.0 * degrees_to_radians_v<double> << " 弧度\n";
    std::cout << "π/2 弧度 = " << (pi_v<double> / 2.0) * radians_to_degrees_v<double> << " 度\n";
    
    std::cout << "\n编译期数学函数:\n";
    std::cout << "2^10 = " << pow_v(2.0, 10) << "\n";
    std::cout << "√16 = " << sqrt_v(16.0) << "\n";
    std::cout << "sin(π/6) = " << sin_v(pi_v<double> / 6.0) << "\n";
    std::cout << "cos(π/4) = " << cos_v(pi_v<double> / 4.0) << "\n";
    std::cout << "log(e) = " << log_v(e_v<double>) << "\n";
    std::cout << "exp(1) = " << exp_v(1.0) << "\n";
    
    // 类型分析
    std::cout << "\n类型分析:\n";
    TypeAnalyzer<const int&>::analyze();
    std::cout << "\n";
    TypeAnalyzer<volatile std::string>::analyze();
    
    // 2. 编译期算法演示
    std::cout << "\n===== 2. 编译期算法演示 =====\n";
    using namespace CompileTimeAlgorithms;
    
    std::cout << "斐波那契数列:\n";
    std::cout << "F(10) = " << fibonacci_v<10> << "\n";
    std::cout << "F(20) = " << fibonacci_v<20> << "\n";
    std::cout << "F(30) = " << fibonacci_v<30> << "\n";
    
    std::cout << "\n阶乘:\n";
    std::cout << "5! = " << factorial_v<5> << "\n";
    std::cout << "10! = " << factorial_v<10> << "\n";
    
    std::cout << "\n数论运算:\n";
    std::cout << "gcd(48, 18) = " << gcd(48, 18) << "\n";
    std::cout << "lcm(12, 18) = " << lcm(12, 18) << "\n";
    std::cout << "is_prime(17) = " << is_prime(17) << "\n";
    std::cout << "is_prime(25) = " << is_prime(25) << "\n";
    
    // 编译期数组操作
    constexpr int test_array[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    constexpr size_t array_size = array_size(test_array);
    constexpr int array_sum = array_sum(test_array);
    constexpr int array_max = array_max(test_array);
    constexpr int array_min = array_min(test_array);
    
    std::cout << "\n编译期数组操作:\n";
    std::cout << "数组大小: " << array_size << "\n";
    std::cout << "数组求和: " << array_sum << "\n";
    std::cout << "数组最大值: " << array_max << "\n";
    std::cout << "数组最小值: " << array_min << "\n";
    std::cout << "查找8的位置: " << array_find(test_array, 8) << "\n";
    
    // 编译期矩阵运算
    std::cout << "\n编译期矩阵运算:\n";
    Matrix<int, 2, 3> mat1;
    mat1(0, 0) = 1; mat1(0, 1) = 2; mat1(0, 2) = 3;
    mat1(1, 0) = 4; mat1(1, 1) = 5; mat1(1, 2) = 6;
    
    Matrix<int, 3, 2> mat2;
    mat2(0, 0) = 7; mat2(0, 1) = 8;
    mat2(1, 0) = 9; mat2(1, 1) = 10;
    mat2(2, 0) = 11; mat2(2, 1) = 12;
    
    auto result = mat1 * mat2;
    std::cout << "矩阵乘法结果:\n";
    std::cout << result(0, 0) << " " << result(0, 1) << "\n";
    std::cout << result(1, 0) << " " << result(1, 1) << "\n";
    
    // 3. 类型列表操作演示
    std::cout << "\n===== 3. 类型列表操作演示 =====\n";
    using namespace TypeListOperations;
    
    using MyTypes = TypeList<int, double, std::string, char, int, double>;
    
    std::cout << "类型列表信息:\n";
    std::cout << "大小: " << MyTypes::size << "\n";
    std::cout << "包含int: " << Contains_v<MyTypes, int> << "\n";
    std::cout << "包含float: " << Contains_v<MyTypes, float> << "\n";
    std::cout << "int的索引: " << IndexOf_v<MyTypes, int> << "\n";
    std::cout << "double的索引: " << IndexOf_v<MyTypes, double> << "\n";
    
    using FirstType = Front_t<MyTypes>;
    using UniqueTypes = Unique_t<MyTypes>;
    
    std::cout << "第一个类型: " << typeid(FirstType).name() << "\n";
    std::cout << "去重后大小: " << UniqueTypes::size << "\n";
    
    // 类型过滤器
    template<typename T>
    using IsIntegral = std::is_integral<T>;
    
    using IntegralTypes = Filter_t<MyTypes, IsIntegral>;
    std::cout << "整数类型数量: " << IntegralTypes::size << "\n";
    
    // 4. 编译期模式演示
    std::cout << "\n===== 4. 编译期模式演示 =====\n";
    using namespace CompileTimePatterns;
    
    // 策略选择
    using IntStrategy = SortingStrategy_t<int>;
    using DoubleStrategy = SortingStrategy_t<double>;
    
    std::cout << "int排序策略: " << IntStrategy::value << "\n";
    std::cout << "double排序策略: " << DoubleStrategy::value << "\n";
    
    // 压缩策略
    auto no_compression = CompressionFactory<int, 0>::create();
    auto rle_compression = CompressionFactory<int, 1>::create();
    
    int test_data = 0x11223344;
    auto compressed1 = no_compression->compress(test_data);
    auto compressed2 = rle_compression->compress(test_data);
    
    std::cout << "无压缩大小: " << compressed1.size() << " 字节\n";
    std::cout << "RLE压缩大小: " << compressed2.size() << " 字节\n";
    
    // 5. 编译期容器演示
    std::cout << "\n===== 5. 编译期容器演示 =====\n";
    using namespace CompileTimeContainers;
    
    // 固定数组
    FixedArray<int, 5> fixed_array;
    fixed_array.fill(42);
    std::cout << "固定数组[0] = " << fixed_array[0] << "\n";
    std::cout << "固定数组大小: " << fixed_array.size() << "\n";
    
    // 位集合
    BitSet<8> bitset;
    bitset.set(0);
    bitset.set(3);
    bitset.set(5);
    
    std::cout << "位集合: ";
    for (size_t i = 0; i < 8; ++i) {
        std::cout << (bitset.test(i) ? "1" : "0");
    }
    std::cout << "\n";
    std::cout << "设置的位数: " << bitset.count() << "\n";
    
    // 固定字符串
    FixedString<5> fixed_str("Hello");
    std::cout << "固定字符串: " << fixed_str.c_str() << "\n";
    std::cout << "固定字符串大小: " << fixed_str.size() << "\n";
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++14 -O2 -Wall 09_metaprogramming_new_features.cpp -o metaprogramming
./metaprogramming

关键学习点:
1. 变量模板提供了编译期值的便捷表示，简化了类型特征的使用
2. C++14的constexpr函数扩展大大增强了编译期计算能力
3. 编译期算法和数据结构可以在编译时完成大量计算工作
4. 类型列表操作提供了强大的类型系统元编程能力
5. 编译期设计模式可以实现零开销的抽象
6. 编译期容器提供了类型安全和性能保证

注意事项:
- 编译期计算需要所有操作都是constexpr的
- 过度复杂的编译期计算会增加编译时间
- 编译期递归深度受编译器限制
- 变量模板不能特化，需要使用类模板特化
- 编译期调试比较困难，需要仔细验证逻辑
- 编译期内存使用也有限制，避免过大的编译期数据结构
*/