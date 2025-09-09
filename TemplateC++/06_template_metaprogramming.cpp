/**
 * 第6章：模板元编程基础深度解析
 * 
 * 核心概念：
 * 1. 类型计算系统 - 类型作为数据的函数式编程模型
 * 2. 编译期数据结构 - 类型列表、类型映射等数据结构
 * 3. 元函数设计 - 值元函数、类型元函数和模板元函数
 * 4. 控制流实现 - 编译期条件、循环和递归控制
 * 5. 算法抽象 - 编译期算法的设计和实现原则
 */

#include <iostream>
#include <type_traits>
#include <tuple>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <utility>

// ===== 1. 类型作为数据的函数式编程模型 =====
void demonstrate_type_as_data() {
    std::cout << "=== 类型作为数据的函数式编程模型 ===\n";
    
    // 基础类型操作：类型"变量"和"函数"
    template<typename T>
    struct TypeWrapper {
        using type = T;
    };
    
    // 类型"函数"：添加指针
    template<typename T>
    struct AddPointer {
        using type = T*;
    };
    
    // 类型"函数"：移除const
    template<typename T>
    struct RemoveConst {
        using type = T;
    };
    
    template<typename T>
    struct RemoveConst<const T> {
        using type = T;
    };
    
    // 类型"函数"组合
    template<typename T>
    using AddPointerAndRemoveConst = typename AddPointer<typename RemoveConst<T>::type>::type;
    
    // 演示类型计算
    using Type1 = int;
    using Type2 = AddPointer<Type1>::type;  // int*
    using Type3 = RemoveConst<const int>::type;  // int
    using Type4 = AddPointerAndRemoveConst<const int>;  // int*
    
    std::cout << "类型计算演示:\n";
    std::cout << "  int -> int*: " << std::is_same_v<Type2, int*> << "\n";
    std::cout << "  const int -> int: " << std::is_same_v<Type3, int> << "\n";
    std::cout << "  const int -> int*: " << std::is_same_v<Type4, int*> << "\n";
    
    // 高阶类型函数：接受类型函数作为参数
    template<template<typename> class F, typename T>
    struct ApplyTypeFunction {
        using type = typename F<T>::type;
    };
    
    // 类型函数的柯里化
    template<template<typename> class F>
    struct CurriedTypeFunction {
        template<typename T>
        using apply = typename F<T>::type;
    };
    
    using CurriedAddPointer = CurriedTypeFunction<AddPointer>;
    using Result = CurriedAddPointer::apply<int>;  // int*
    
    static_assert(std::is_same_v<Result, int*>);
    std::cout << "高阶类型函数测试通过\n";
    
    std::cout << "\n";
}

// ===== 2. 编译期数据结构设计 =====
void demonstrate_compile_time_data_structures() {
    std::cout << "=== 编译期数据结构设计 ===\n";
    
    // 类型列表（TypeList）
    template<typename... Types>
    struct TypeList {
        static constexpr std::size_t size = sizeof...(Types);
    };
    
    // 类型列表操作
    
    // 获取头部类型
    template<typename List>
    struct Head;
    
    template<typename H, typename... T>
    struct Head<TypeList<H, T...>> {
        using type = H;
    };
    
    template<typename List>
    using Head_t = typename Head<List>::type;
    
    // 获取尾部列表
    template<typename List>
    struct Tail;
    
    template<typename H, typename... T>
    struct Tail<TypeList<H, T...>> {
        using type = TypeList<T...>;
    };
    
    template<typename List>
    using Tail_t = typename Tail<List>::type;
    
    // 在头部添加类型
    template<typename T, typename List>
    struct Prepend;
    
    template<typename T, typename... Types>
    struct Prepend<T, TypeList<Types...>> {
        using type = TypeList<T, Types...>;
    };
    
    template<typename T, typename List>
    using Prepend_t = typename Prepend<T, List>::type;
    
    // 在尾部添加类型
    template<typename List, typename T>
    struct Append;
    
    template<typename... Types, typename T>\n    struct Append<TypeList<Types...>, T> {\n        using type = TypeList<Types..., T>;\n    };\n    \n    template<typename List, typename T>\n    using Append_t = typename Append<List, T>::type;\n    \n    // 连接两个类型列表\n    template<typename List1, typename List2>\n    struct Concat;\n    \n    template<typename... Types1, typename... Types2>\n    struct Concat<TypeList<Types1...>, TypeList<Types2...>> {\n        using type = TypeList<Types1..., Types2...>;\n    };\n    \n    template<typename List1, typename List2>\n    using Concat_t = typename Concat<List1, List2>::type;\n    \n    // 获取指定位置的类型\n    template<std::size_t N, typename List>\n    struct At {\n        using type = typename At<N-1, Tail_t<List>>::type;\n    };\n    \n    template<typename List>\n    struct At<0, List> {\n        using type = Head_t<List>;\n    };\n    \n    template<std::size_t N, typename List>\n    using At_t = typename At<N, List>::type;\n    \n    // 查找类型在列表中的位置\n    template<typename T, typename List, std::size_t Index = 0>\n    struct IndexOf;\n    \n    template<typename T, typename H, typename... Tail, std::size_t Index>\n    struct IndexOf<T, TypeList<H, Tail...>, Index> {\n        static constexpr std::size_t value = \n            std::is_same_v<T, H> ? Index : IndexOf<T, TypeList<Tail...>, Index + 1>::value;\n    };\n    \n    template<typename T, std::size_t Index>\n    struct IndexOf<T, TypeList<>, Index> {\n        static constexpr std::size_t value = static_cast<std::size_t>(-1);  // 未找到\n    };\n    \n    template<typename T, typename List>\n    inline constexpr std::size_t IndexOf_v = IndexOf<T, List>::value;\n    \n    // 类型映射（TypeMap）\n    template<typename Key, typename Value>\n    struct TypePair {\n        using key = Key;\n        using value = Value;\n    };\n    \n    template<typename... Pairs>\n    struct TypeMap {\n        static constexpr std::size_t size = sizeof...(Pairs);\n    };\n    \n    // 从类型映射中查找值\n    template<typename Key, typename Map>\n    struct MapLookup;\n    \n    template<typename Key, typename... Pairs>\n    struct MapLookup<Key, TypeMap<Pairs...>> {\n    private:\n        template<typename Pair>\n        static auto lookup_impl() {\n            if constexpr (std::is_same_v<Key, typename Pair::key>) {\n                return typename Pair::value{};\n            } else {\n                return void{};\n            }\n        }\n        \n        using result = decltype((lookup_impl<Pairs>(), ...));\n        \n    public:\n        using type = std::conditional_t<std::is_same_v<result, void>, void, result>;\n    };\n    \n    // 演示类型列表操作\n    using TestList = TypeList<int, double, char, std::string>;\n    using HeadType = Head_t<TestList>;  // int\n    using TailList = Tail_t<TestList>;  // TypeList<double, char, std::string>\n    using SecondType = At_t<1, TestList>;  // double\n    \n    static_assert(std::is_same_v<HeadType, int>);\n    static_assert(std::is_same_v<SecondType, double>);\n    static_assert(IndexOf_v<char, TestList> == 2);\n    static_assert(IndexOf_v<float, TestList> == static_cast<std::size_t>(-1));\n    \n    std::cout << \"类型列表操作验证通过\\n\";\n    \n    // 演示类型映射\n    using TestMap = TypeMap<\n        TypePair<int, std::string>,\n        TypePair<double, std::vector<int>>,\n        TypePair<char, std::array<int, 10>>\n    >;\n    \n    std::cout << \"类型数据结构测试完成\\n\";\n    \n    std::cout << \"\\n\";\n}\n\n// ===== 3. 元函数的设计模式 =====\nvoid demonstrate_metafunction_design() {\n    std::cout << \"=== 元函数的设计模式 ===\\n\";\n    \n    // 值元函数：计算编译期常量\n    template<int N>\n    struct Factorial {\n        static constexpr int value = N * Factorial<N-1>::value;\n    };\n    \n    template<>\n    struct Factorial<0> {\n        static constexpr int value = 1;\n    };\n    \n    template<int N>\n    inline constexpr int Factorial_v = Factorial<N>::value;\n    \n    // 类型元函数：计算类型转换\n    template<bool Condition, typename TrueType, typename FalseType>\n    struct ConditionalType {\n        using type = TrueType;\n    };\n    \n    template<typename TrueType, typename FalseType>\n    struct ConditionalType<false, TrueType, FalseType> {\n        using type = FalseType;\n    };\n    \n    template<bool Condition, typename TrueType, typename FalseType>\n    using ConditionalType_t = typename ConditionalType<Condition, TrueType, FalseType>::type;\n    \n    // 模板元函数：操作模板本身\n    template<template<typename> class F, template<typename> class G>\n    struct ComposeTemplates {\n        template<typename T>\n        using type = typename F<typename G<T>::type>::type;\n    };\n    \n    // 元函数的组合和链式调用\n    template<typename T>\n    struct DoublePointer {\n        using type = T**;\n    };\n    \n    template<typename T>\n    struct AddConst {\n        using type = const T;\n    };\n    \n    using Composed = ComposeTemplates<DoublePointer, AddConst>;\n    using Result = Composed::type<int>;  // const int**\n    \n    static_assert(std::is_same_v<Result, const int**>);\n    \n    // 元函数的递归定义\n    template<template<typename> class F, int N>\n    struct Repeat {\n        template<typename T>\n        using type = typename F<typename Repeat<F, N-1>::template type<T>>::type;\n    };\n    \n    template<template<typename> class F>\n    struct Repeat<F, 0> {\n        template<typename T>\n        using type = T;\n    };\n    \n    template<typename T>\n    struct AddPointer {\n        using type = T*;\n    };\n    \n    using TriplePointer = Repeat<AddPointer, 3>::type<int>;  // int***\n    static_assert(std::is_same_v<TriplePointer, int***>);\n    \n    // 元函数的惰性求值\n    template<typename T>\n    struct LazyEval {\n        using type = T;\n    };\n    \n    template<template<typename> class F, typename T>\n    struct LazyEval<F<T>> {\n        using type = typename F<T>::type;\n    };\n    \n    // 元函数的缓存机制（简单示例）\n    template<int N, typename = void>\n    struct FibonacciCached {\n        static constexpr int value = FibonacciCached<N-1>::value + FibonacciCached<N-2>::value;\n    };\n    \n    template<>\n    struct FibonacciCached<0> {\n        static constexpr int value = 0;\n    };\n    \n    template<>\n    struct FibonacciCached<1> {\n        static constexpr int value = 1;\n    };\n    \n    constexpr int fib10 = Factorial_v<10>;\n    constexpr int fib_cached = FibonacciCached<10>::value;\n    \n    std::cout << \"阶乘(10): \" << fib10 << \"\\n\";\n    std::cout << \"斐波那契(10): \" << fib_cached << \"\\n\";\n    std::cout << \"元函数设计模式验证通过\\n\";\n    \n    std::cout << \"\\n\";\n}\n\n// ===== 4. TMP中的控制流实现 =====\nvoid demonstrate_tmp_control_flow() {\n    std::cout << \"=== TMP中的控制流实现 ===\\n\";\n    \n    // 编译期if-else\n    template<bool Condition>\n    struct If {\n        template<typename ThenType, typename ElseType>\n        using type = ThenType;\n    };\n    \n    template<>\n    struct If<false> {\n        template<typename ThenType, typename ElseType>\n        using type = ElseType;\n    };\n    \n    template<bool Condition, typename ThenType, typename ElseType>\n    using If_t = typename If<Condition>::template type<ThenType, ElseType>;\n    \n    // 编译期switch-case\n    template<int Value, typename DefaultType, typename... Cases>\n    struct Switch {\n        using type = DefaultType;\n    };\n    \n    template<int TargetValue, typename CaseType>\n    struct Case {\n        static constexpr int value = TargetValue;\n        using type = CaseType;\n    };\n    \n    template<int Value, typename DefaultType, typename FirstCase, typename... RestCases>\n    struct Switch<Value, DefaultType, FirstCase, RestCases...> {\n        using type = std::conditional_t<\n            Value == FirstCase::value,\n            typename FirstCase::type,\n            typename Switch<Value, DefaultType, RestCases...>::type\n        >;\n    };\n    \n    template<int Value, typename DefaultType, typename... Cases>\n    using Switch_t = typename Switch<Value, DefaultType, Cases...>::type;\n    \n    // 编译期循环（通过递归实现）\n    template<int N, template<int> class F>\n    struct Loop {\n        static constexpr int value = F<N>::value + Loop<N-1, F>::value;\n    };\n    \n    template<template<int> class F>\n    struct Loop<0, F> {\n        static constexpr int value = F<0>::value;\n    };\n    \n    template<int N>\n    struct Identity {\n        static constexpr int value = N;\n    };\n    \n    template<int N>\n    struct Square {\n        static constexpr int value = N * N;\n    };\n    \n    // 编译期while循环\n    template<template<int> class Condition, template<int> class Body, int Initial>\n    struct While {\n    private:\n        template<int Current>\n        struct Helper {\n            static constexpr int value = \n                Condition<Current>::value ? \n                Helper<Body<Current>::value>::value : \n                Current;\n        };\n        \n    public:\n        static constexpr int value = Helper<Initial>::value;\n    };\n    \n    template<int N>\n    struct LessThan100 {\n        static constexpr bool value = N < 100;\n    };\n    \n    template<int N>\n    struct MultiplyBy2 {\n        static constexpr int value = N * 2;\n    };\n    \n    // 编译期for循环（固定次数）\n    template<int Start, int End, template<int> class Body>\n    struct For {\n        static constexpr int value = \n            (Start < End) ? \n            Body<Start>::value + For<Start + 1, End, Body>::value :\n            0;\n    };\n    \n    // 演示控制流\n    using ConditionalResult = If_t<true, int, double>;  // int\n    static_assert(std::is_same_v<ConditionalResult, int>);\n    \n    using SwitchResult = Switch_t<2, void,\n        Case<1, int>,\n        Case<2, double>,\n        Case<3, char>\n    >;  // double\n    static_assert(std::is_same_v<SwitchResult, double>);\n    \n    constexpr int loop_sum = Loop<5, Identity>::value;  // 0+1+2+3+4+5 = 15\n    constexpr int square_sum = Loop<3, Square>::value;  // 0+1+4+9 = 14\n    constexpr int for_sum = For<1, 6, Identity>::value; // 1+2+3+4+5 = 15\n    \n    std::cout << \"循环求和(0-5): \" << loop_sum << \"\\n\";\n    std::cout << \"平方和(0-3): \" << square_sum << \"\\n\";\n    std::cout << \"For循环(1-5): \" << for_sum << \"\\n\";\n    std::cout << \"控制流实现验证通过\\n\";\n    \n    std::cout << \"\\n\";\n}\n\n// ===== 5. 编译期算法抽象 =====\nvoid demonstrate_compile_time_algorithms() {\n    std::cout << \"=== 编译期算法抽象 ===\\n\";\n    \n    // 类型列表的map算法\n    template<template<typename> class F, typename List>\n    struct Map;\n    \n    template<template<typename> class F, typename... Types>\n    struct Map<F, TypeList<Types...>> {\n        using type = TypeList<typename F<Types>::type...>;\n    };\n    \n    template<template<typename> class F, typename List>\n    using Map_t = typename Map<F, List>::type;\n    \n    // 类型列表的filter算法\n    template<template<typename> class Predicate, typename List>\n    struct Filter;\n    \n    template<template<typename> class Predicate>\n    struct Filter<Predicate, TypeList<>> {\n        using type = TypeList<>;\n    };\n    \n    template<template<typename> class Predicate, typename Head, typename... Tail>\n    struct Filter<Predicate, TypeList<Head, Tail...>> {\n    private:\n        using rest = typename Filter<Predicate, TypeList<Tail...>>::type;\n        \n    public:\n        using type = std::conditional_t<\n            Predicate<Head>::value,\n            Prepend_t<Head, rest>,\n            rest\n        >;\n    };\n    \n    template<template<typename> class Predicate, typename List>\n    using Filter_t = typename Filter<Predicate, List>::type;\n    \n    // 类型列表的reduce算法\n    template<template<typename, typename> class F, typename Initial, typename List>\n    struct Reduce {\n        using type = Initial;\n    };\n    \n    template<template<typename, typename> class F, typename Initial, typename Head, typename... Tail>\n    struct Reduce<F, Initial, TypeList<Head, Tail...>> {\n        using type = typename Reduce<F, typename F<Initial, Head>::type, TypeList<Tail...>>::type;\n    };\n    \n    template<template<typename, typename> class F, typename Initial, typename List>\n    using Reduce_t = typename Reduce<F, Initial, List>::type;\n    \n    // 类型列表排序（按大小）\n    template<typename List>\n    struct Sort;\n    \n    template<typename T1, typename T2>\n    struct TypeSizeLess {\n        static constexpr bool value = sizeof(T1) < sizeof(T2);\n    };\n    \n    // 简单的插入排序实现\n    template<typename T, typename SortedList>\n    struct InsertSorted;\n    \n    template<typename T>\n    struct InsertSorted<T, TypeList<>> {\n        using type = TypeList<T>;\n    };\n    \n    template<typename T, typename Head, typename... Tail>\n    struct InsertSorted<T, TypeList<Head, Tail...>> {\n        using type = std::conditional_t<\n            TypeSizeLess<T, Head>::value,\n            TypeList<T, Head, Tail...>,\n            Prepend_t<Head, typename InsertSorted<T, TypeList<Tail...>>::type>\n        >;\n    };\n    \n    template<>\n    struct Sort<TypeList<>> {\n        using type = TypeList<>;\n    };\n    \n    template<typename Head, typename... Tail>\n    struct Sort<TypeList<Head, Tail...>> {\n        using type = typename InsertSorted<Head, typename Sort<TypeList<Tail...>>::type>::type;\n    };\n    \n    template<typename List>\n    using Sort_t = typename Sort<List>::type;\n    \n    // 编译期查找算法\n    template<typename T, typename List>\n    struct Contains;\n    \n    template<typename T>\n    struct Contains<T, TypeList<>> : std::false_type {};\n    \n    template<typename T, typename Head, typename... Tail>\n    struct Contains<T, TypeList<Head, Tail...>> : \n        std::conditional_t<std::is_same_v<T, Head>, \n                          std::true_type, \n                          Contains<T, TypeList<Tail...>>> {};\n    \n    template<typename T, typename List>\n    inline constexpr bool Contains_v = Contains<T, List>::value;\n    \n    // 演示算法应用\n    using OriginalList = TypeList<double, char, int, long long>;\n    using PointerList = Map_t<AddPointer, OriginalList>;\n    using SortedList = Sort_t<OriginalList>;\n    \n    template<typename T>\n    struct IsIntegral {\n        static constexpr bool value = std::is_integral_v<T>;\n    };\n    \n    using IntegralList = Filter_t<IsIntegral, OriginalList>;\n    \n    // 类型大小累加\n    template<typename Acc, typename T>\n    struct SizeAccumulator {\n        using type = std::integral_constant<std::size_t, Acc::value + sizeof(T)>;\n    };\n    \n    using TotalSize = Reduce_t<SizeAccumulator, std::integral_constant<std::size_t, 0>, OriginalList>;\n    \n    std::cout << \"算法应用演示:\\n\";\n    std::cout << \"  原始列表大小: \" << OriginalList::size << \"\\n\";\n    std::cout << \"  指针列表大小: \" << PointerList::size << \"\\n\";\n    std::cout << \"  排序列表大小: \" << SortedList::size << \"\\n\";\n    std::cout << \"  整数类型数量: \" << IntegralList::size << \"\\n\";\n    std::cout << \"  总字节数: \" << TotalSize::value << \"\\n\";\n    std::cout << \"  包含double: \" << Contains_v<double, OriginalList> << \"\\n\";\n    std::cout << \"  包含float: \" << Contains_v<float, OriginalList> << \"\\n\";\n    \n    std::cout << \"编译期算法验证通过\\n\";\n    \n    std::cout << \"\\n\";\n}\n\n// ===== 6. 高级TMP技术和性能分析 =====\nvoid demonstrate_advanced_tmp_techniques() {\n    std::cout << \"=== 高级TMP技术和性能分析 ===\\n\";\n    \n    // 编译期字符串处理\n    template<char... chars>\n    struct CompileTimeString {\n        static constexpr char data[] = {chars..., '\\0'};\n        static constexpr std::size_t length = sizeof...(chars);\n        \n        template<char... other_chars>\n        constexpr auto operator+(CompileTimeString<other_chars...>) const {\n            return CompileTimeString<chars..., other_chars...>{};\n        }\n    };\n    \n    template<char... chars>\n    constexpr char CompileTimeString<chars...>::data[];\n    \n    // 编译期哈希计算\n    constexpr std::size_t compile_time_hash(const char* str, std::size_t length) {\n        std::size_t hash = 0;\n        for (std::size_t i = 0; i < length; ++i) {\n            hash = hash * 31 + str[i];\n        }\n        return hash;\n    }\n    \n    // 编译期素数生成（埃拉托斯特尼筛法）\n    template<int N>\n    struct SieveOfEratosthenes {\n        static constexpr std::array<bool, N+1> sieve = []() constexpr {\n            std::array<bool, N+1> is_prime{};\n            for (int i = 2; i <= N; ++i) {\n                is_prime[i] = true;\n            }\n            \n            for (int i = 2; i * i <= N; ++i) {\n                if (is_prime[i]) {\n                    for (int j = i * i; j <= N; j += i) {\n                        is_prime[j] = false;\n                    }\n                }\n            }\n            return is_prime;\n        }();\n        \n        static constexpr bool is_prime(int n) {\n            return n >= 2 && n <= N && sieve[n];\n        }\n        \n        static constexpr int count_primes() {\n            int count = 0;\n            for (int i = 2; i <= N; ++i) {\n                if (sieve[i]) ++count;\n            }\n            return count;\n        }\n    };\n    \n    // 编译期矩阵运算\n    template<int Rows, int Cols>\n    struct Matrix {\n        std::array<std::array<int, Cols>, Rows> data{};\n        \n        constexpr Matrix() = default;\n        \n        template<int OtherCols>\n        constexpr auto multiply(const Matrix<Cols, OtherCols>& other) const {\n            Matrix<Rows, OtherCols> result{};\n            \n            for (int i = 0; i < Rows; ++i) {\n                for (int j = 0; j < OtherCols; ++j) {\n                    for (int k = 0; k < Cols; ++k) {\n                        result.data[i][j] += data[i][k] * other.data[k][j];\n                    }\n                }\n            }\n            return result;\n        }\n    };\n    \n    // TMP性能测试：编译期vs运行期\n    template<int N>\n    struct CompileTimeFibonacci {\n        static constexpr long long value = \n            CompileTimeFibonacci<N-1>::value + CompileTimeFibonacci<N-2>::value;\n    };\n    \n    template<>\n    struct CompileTimeFibonacci<0> {\n        static constexpr long long value = 0;\n    };\n    \n    template<>\n    struct CompileTimeFibonacci<1> {\n        static constexpr long long value = 1;\n    };\n    \n    constexpr long long runtime_fibonacci(int n) {\n        if (n <= 1) return n;\n        long long a = 0, b = 1;\n        for (int i = 2; i <= n; ++i) {\n            long long temp = a + b;\n            a = b;\n            b = temp;\n        }\n        return b;\n    }\n    \n    // 编译期质数计算演示\n    constexpr auto prime_sieve = SieveOfEratosthenes<100>{};\n    constexpr int prime_count = prime_sieve.count_primes();\n    \n    std::cout << \"高级TMP技术演示:\\n\";\n    std::cout << \"  100以内素数个数: \" << prime_count << \"\\n\";\n    std::cout << \"  17是素数: \" << prime_sieve.is_prime(17) << \"\\n\";\n    std::cout << \"  18是素数: \" << prime_sieve.is_prime(18) << \"\\n\";\n    \n    // 编译期vs运行期性能对比\n    constexpr long long compile_time_fib = CompileTimeFibonacci<40>::value;\n    long long runtime_fib = runtime_fibonacci(40);\n    \n    std::cout << \"  编译期斐波那契(40): \" << compile_time_fib << \"\\n\";\n    std::cout << \"  运行期斐波那契(40): \" << runtime_fib << \"\\n\";\n    std::cout << \"  结果一致: \" << (compile_time_fib == runtime_fib) << \"\\n\";\n    \n    // 类型系统的状态机\n    struct StateA {};\n    struct StateB {};\n    struct StateC {};\n    \n    template<typename CurrentState>\n    struct StateMachine;\n    \n    template<>\n    struct StateMachine<StateA> {\n        using next_state = StateB;\n        static constexpr const char* name = \"State A\";\n    };\n    \n    template<>\n    struct StateMachine<StateB> {\n        using next_state = StateC;\n        static constexpr const char* name = \"State B\";\n    };\n    \n    template<>\n    struct StateMachine<StateC> {\n        using next_state = StateA;\n        static constexpr const char* name = \"State C\";\n    };\n    \n    template<typename State, int Steps>\n    struct RunStateMachine {\n        using final_state = typename RunStateMachine<typename StateMachine<State>::next_state, Steps-1>::final_state;\n    };\n    \n    template<typename State>\n    struct RunStateMachine<State, 0> {\n        using final_state = State;\n    };\n    \n    using FinalState = RunStateMachine<StateA, 5>::final_state;\n    std::cout << \"  状态机运行5步后: \" << StateMachine<FinalState>::name << \"\\n\";\n    \n    std::cout << \"高级TMP技术验证通过\\n\";\n    \n    std::cout << \"\\n\";\n}\n\n// ===== 主函数 =====\nint main() {\n    std::cout << \"第6章：模板元编程基础深度解析\\n\";\n    std::cout << \"===============================\\n\";\n    \n    demonstrate_type_as_data();\n    demonstrate_compile_time_data_structures();\n    demonstrate_metafunction_design();\n    demonstrate_tmp_control_flow();\n    demonstrate_compile_time_algorithms();\n    demonstrate_advanced_tmp_techniques();\n    \n    return 0;\n}\n\n/*\n编译和运行建议:\ng++ -std=c++20 -O2 -Wall -Wextra 06_template_metaprogramming.cpp -o template_metaprogramming\n./template_metaprogramming\n\n关键学习点:\n1. 模板元编程将类型视为数据，模板视为函数，形成完整的编译期计算系统\n2. 类型列表是TMP中最基础的数据结构，支持各种算法操作\n3. 元函数分为值元函数、类型元函数和模板元函数三种形式\n4. TMP中的控制流通过模板特化和递归实现\n5. 编译期算法能够在编译时完成复杂的计算和类型操作\n\n高级技巧:\n- 使用类型列表实现编译期数据结构\n- 通过模板递归实现编译期循环\n- 利用SFINAE实现编译期条件分支\n- 组合多个元函数实现复杂的类型变换\n- 使用constexpr结合TMP实现编译期计算\n\n性能优势:\n- 所有计算在编译期完成，运行时零开销\n- 编译期错误检查，提高程序安全性\n- 代码生成优化，编译器能进行更好的优化\n- 类型安全的泛型编程\n\n注意事项:\n- TMP代码可读性较差，需要良好的文档\n- 递归深度受编译器限制\n- 编译时间可能较长\n- 模板实例化可能导致代码膨胀\n- 调试困难，错误信息复杂\n*/