/**
 * C++11/14/17/20 编译期计算深度解析
 * 
 * 核心概念：
 * 1. constexpr基础机制 - 编译期常量表达式计算
 * 2. 模板递归计算 - 基于模板的编译期算法实现
 * 3. consteval和constinit - C++20编译期计算增强
 * 4. 编译期数据结构 - 编译期容器和算法设计
 * 5. 实际应用案例 - 高性能编译期优化技术
 */

#include <iostream>
#include <string>
#include <array>
#include <type_traits>
#include <utility>
#include <chrono>
#include <algorithm>
#include <numeric>

// ===== 1. constexpr基础机制演示 =====
void demonstrate_constexpr_basics() {
    std::cout << "=== constexpr基础机制演示 ===\n";
    
    // 1.1 constexpr变量
    constexpr int compile_time_value = 42;
    constexpr double pi = 3.14159265359;
    
    std::cout << "1. constexpr变量:\n";
    std::cout << "编译期常量: " << compile_time_value << "\n";
    std::cout << "编译期π值: " << pi << "\n";
    
    // 1.2 constexpr函数
    constexpr int factorial(int n) {
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }
    
    constexpr int fibonacci(int n) {
        if (n <= 1) return n;
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
    
    // 编译期计算
    constexpr int fact_5 = factorial(5);
    constexpr int fib_10 = fibonacci(10);
    
    std::cout << "\n2. constexpr函数:\n";
    std::cout << "5的阶乘 (编译期): " << fact_5 << "\n";
    std::cout << "第10个斐波那契数 (编译期): " << fib_10 << "\n";
    
    // 1.3 constexpr构造函数
    struct Point {
        int x, y;
        
        constexpr Point(int x_val, int y_val) : x(x_val), y(y_val) {}
        
        constexpr int distance_squared() const {
            return x * x + y * y;
        }
        
        constexpr Point operator+(const Point& other) const {
            return Point(x + other.x, y + other.y);
        }
    };
    
    constexpr Point p1(3, 4);
    constexpr Point p2(1, 2);
    constexpr Point p3 = p1 + p2;
    constexpr int dist_sq = p1.distance_squared();
    
    std::cout << "\n3. constexpr类和方法:\n";
    std::cout << "点p1到原点距离平方: " << dist_sq << "\n";
    std::cout << "点p3坐标: (" << p3.x << ", " << p3.y << ")\n";
    
    // 1.4 constexpr条件分支
    constexpr auto compile_time_conditional = [](int n) constexpr -> int {
        if (n < 0) {
            return -n;  // 绝对值
        } else if (n == 0) {
            return 1;   // 特殊情况
        } else {
            return n * n;  // 平方
        }
    };
    
    constexpr int result1 = compile_time_conditional(-5);
    constexpr int result2 = compile_time_conditional(0);
    constexpr int result3 = compile_time_conditional(4);
    
    std::cout << "\n4. constexpr条件分支:\n";
    std::cout << "compile_time_conditional(-5) = " << result1 << "\n";
    std::cout << "compile_time_conditional(0) = " << result2 << "\n";
    std::cout << "compile_time_conditional(4) = " << result3 << "\n";
    
    // 1.5 constexpr数组和字符串处理
    constexpr const char* compile_time_string = "Hello, constexpr!";
    
    constexpr size_t string_length(const char* str) {
        size_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        return len;
    }
    
    constexpr size_t str_len = string_length(compile_time_string);
    
    std::cout << "\n5. constexpr字符串处理:\n";
    std::cout << "字符串: \"" << compile_time_string << "\"\n";
    std::cout << "长度 (编译期计算): " << str_len << "\n";
    
    std::cout << "\n";
}

// ===== 2. 模板递归计算演示 =====
void demonstrate_template_recursion() {
    std::cout << "=== 模板递归计算演示 ===\n";
    
    // 2.1 基于模板的阶乘计算
    template<int N>
    struct Factorial {
        static constexpr int value = N * Factorial<N - 1>::value;
    };
    
    template<>
    struct Factorial<0> {
        static constexpr int value = 1;
    };
    
    template<>
    struct Factorial<1> {
        static constexpr int value = 1;
    };
    
    std::cout << "1. 模板递归阶乘:\n";
    std::cout << "0! = " << Factorial<0>::value << "\n";
    std::cout << "5! = " << Factorial<5>::value << "\n";
    std::cout << "10! = " << Factorial<10>::value << "\n";
    
    // 2.2 基于模板的斐波那契数列
    template<int N>
    struct Fibonacci {
        static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
    };
    
    template<>
    struct Fibonacci<0> {
        static constexpr int value = 0;
    };
    
    template<>
    struct Fibonacci<1> {
        static constexpr int value = 1;
    };
    
    std::cout << "\n2. 模板递归斐波那契:\n";
    std::cout << "fib(0) = " << Fibonacci<0>::value << "\n";
    std::cout << "fib(8) = " << Fibonacci<8>::value << "\n";
    std::cout << "fib(15) = " << Fibonacci<15>::value << "\n";
    
    // 2.3 最大公约数计算
    template<int A, int B>
    struct GCD {
        static constexpr int value = GCD<B, A % B>::value;
    };
    
    template<int A>
    struct GCD<A, 0> {
        static constexpr int value = A;
    };
    
    std::cout << "\n3. 最大公约数计算:\n";
    std::cout << "gcd(48, 18) = " << GCD<48, 18>::value << "\n";
    std::cout << "gcd(100, 25) = " << GCD<100, 25>::value << "\n";
    std::cout << "gcd(17, 13) = " << GCD<17, 13>::value << "\n";
    
    // 2.4 幂运算
    template<int Base, int Exp>
    struct Power {
        static constexpr int value = Base * Power<Base, Exp - 1>::value;
    };
    
    template<int Base>
    struct Power<Base, 0> {
        static constexpr int value = 1;
    };
    
    std::cout << "\n4. 模板幂运算:\n";
    std::cout << "2^0 = " << Power<2, 0>::value << "\n";
    std::cout << "2^10 = " << Power<2, 10>::value << "\n";
    std::cout << "3^5 = " << Power<3, 5>::value << "\n";
    
    // 2.5 质数判断
    template<int N, int Divisor = N - 1>
    struct IsPrime {
        static constexpr bool value = (N % Divisor != 0) && IsPrime<N, Divisor - 1>::value;
    };
    
    template<int N>
    struct IsPrime<N, 1> {
        static constexpr bool value = true;
    };
    
    template<>
    struct IsPrime<1, 0> {
        static constexpr bool value = false;
    };
    
    template<>
    struct IsPrime<2, 1> {
        static constexpr bool value = true;
    };
    
    std::cout << "\n5. 质数判断:\n";
    std::cout << "1是质数: " << IsPrime<1>::value << "\n";
    std::cout << "2是质数: " << IsPrime<2>::value << "\n";
    std::cout << "17是质数: " << IsPrime<17>::value << "\n";
    std::cout << "25是质数: " << IsPrime<25>::value << "\n";
    
    std::cout << "\n";
}

// ===== 3. C++20编译期计算增强演示 =====
#if __cplusplus >= 202002L
void demonstrate_cpp20_enhancements() {
    std::cout << "=== C++20编译期计算增强演示 ===\n";
    
    // 3.1 consteval函数（只能编译期计算）
    consteval int consteval_factorial(int n) {
        return (n <= 1) ? 1 : n * consteval_factorial(n - 1);
    }
    
    consteval auto consteval_square(auto x) {
        return x * x;
    }
    
    constexpr int ce_result = consteval_factorial(6);  // 必须编译期计算
    
    std::cout << "1. consteval函数:\n";
    std::cout << "6的阶乘 (consteval): " << ce_result << "\n";
    std::cout << "5的平方 (consteval): " << consteval_square(5) << "\n";
    
    // 3.2 constinit变量（编译期初始化）
    constinit int global_counter = consteval_factorial(4);  // 编译期初始化
    
    std::cout << "\n2. constinit变量:\n";
    std::cout << "constinit全局变量: " << global_counter << "\n";
    
    // 3.3 std::is_constant_evaluated()
    constexpr auto adaptive_function(int n) -> int {
        if (std::is_constant_evaluated()) {
            // 编译期路径：使用递归算法
            return (n <= 1) ? 1 : n * adaptive_function(n - 1);
        } else {
            // 运行期路径：使用迭代算法
            int result = 1;
            for (int i = 2; i <= n; ++i) {
                result *= i;
            }
            return result;
        }
    }
    
    constexpr int compile_time_result = adaptive_function(5);
    int runtime_result = adaptive_function(5);
    
    std::cout << "\n3. 自适应编译期/运行期函数:\n";
    std::cout << "编译期结果: " << compile_time_result << "\n";
    std::cout << "运行期结果: " << runtime_result << "\n";
    
    // 3.4 constexpr动态分配（C++20）
    constexpr auto constexpr_vector_demo() {
        std::vector<int> vec;  // C++20支持编译期动态分配
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        
        int sum = 0;
        for (int val : vec) {
            sum += val;
        }
        return sum;
    }
    
    constexpr int vec_sum = constexpr_vector_demo();
    
    std::cout << "\n4. constexpr动态分配:\n";
    std::cout << "constexpr vector求和: " << vec_sum << "\n";
    
    std::cout << "\n";
}
#else
void demonstrate_cpp20_enhancements() {
    std::cout << "=== C++20编译期计算增强 (需要C++20编译器) ===\n";
    std::cout << "请使用 -std=c++20 编译选项体验C++20特性\n\n";
}
#endif

// ===== 4. 编译期数据结构演示 =====
void demonstrate_compile_time_structures() {
    std::cout << "=== 编译期数据结构演示 ===\n";
    
    // 4.1 编译期数组
    template<typename T, size_t N>
    struct ConstexprArray {
        T data[N];
        
        constexpr ConstexprArray() : data{} {}
        
        constexpr T& operator[](size_t i) {
            return data[i];
        }
        
        constexpr const T& operator[](size_t i) const {
            return data[i];
        }
        
        constexpr size_t size() const {
            return N;
        }
        
        constexpr void fill(const T& value) {
            for (size_t i = 0; i < N; ++i) {
                data[i] = value;
            }
        }
        
        constexpr T sum() const {
            T result{};
            for (size_t i = 0; i < N; ++i) {
                result += data[i];
            }
            return result;
        }
    };
    
    constexpr auto create_test_array() {
        ConstexprArray<int, 10> arr;
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = i * i;  // 平方数
        }
        return arr;
    }
    
    constexpr auto squares = create_test_array();
    constexpr int squares_sum = squares.sum();
    
    std::cout << "1. 编译期数组:\n";
    std::cout << "平方数组前5个元素: ";
    for (size_t i = 0; i < 5; ++i) {
        std::cout << squares[i] << " ";
    }
    std::cout << "\n平方数组和: " << squares_sum << "\n";
    
    // 4.2 编译期字符串
    template<size_t N>
    struct ConstexprString {
        char data[N];
        size_t length;
        
        constexpr ConstexprString() : data{}, length(0) {}
        
        constexpr ConstexprString(const char (&str)[N]) : data{}, length(N - 1) {
            for (size_t i = 0; i < N - 1; ++i) {
                data[i] = str[i];
            }
            data[N - 1] = '\0';
        }
        
        constexpr char operator[](size_t i) const {
            return data[i];
        }
        
        constexpr size_t size() const {
            return length;
        }
        
        constexpr bool contains(char c) const {
            for (size_t i = 0; i < length; ++i) {
                if (data[i] == c) return true;
            }
            return false;
        }
        
        constexpr size_t count_char(char c) const {
            size_t count = 0;
            for (size_t i = 0; i < length; ++i) {
                if (data[i] == c) ++count;
            }
            return count;
        }
    };
    
    constexpr ConstexprString hello("Hello, World!");
    constexpr bool has_comma = hello.contains(',');
    constexpr size_t l_count = hello.count_char('l');
    
    std::cout << "\n2. 编译期字符串:\n";
    std::cout << "字符串: \"";
    for (size_t i = 0; i < hello.size(); ++i) {
        std::cout << hello[i];
    }
    std::cout << "\"\n";
    std::cout << "包含逗号: " << has_comma << "\n";
    std::cout << "'l'的个数: " << l_count << "\n";
    
    // 4.3 编译期哈希表（简化版）
    template<typename Key, typename Value, size_t N>
    struct ConstexprHashMap {
        struct Entry {
            Key key{};
            Value value{};
            bool valid = false;
        };
        
        Entry data[N];
        
        constexpr ConstexprHashMap() : data{} {}
        
        constexpr size_t hash(const Key& key) const {
            // 简单哈希函数
            return static_cast<size_t>(key) % N;
        }
        
        constexpr void insert(const Key& key, const Value& value) {
            size_t index = hash(key);
            size_t original = index;
            
            while (data[index].valid && data[index].key != key) {
                index = (index + 1) % N;
                if (index == original) break;  // 表满
            }
            
            data[index].key = key;
            data[index].value = value;
            data[index].valid = true;
        }
        
        constexpr Value* find(const Key& key) {
            size_t index = hash(key);
            size_t original = index;
            
            while (data[index].valid) {
                if (data[index].key == key) {
                    return &data[index].value;
                }
                index = (index + 1) % N;
                if (index == original) break;
            }
            
            return nullptr;
        }
        
        constexpr const Value* find(const Key& key) const {
            size_t index = hash(key);
            size_t original = index;
            
            while (data[index].valid) {
                if (data[index].key == key) {
                    return &data[index].value;
                }
                index = (index + 1) % N;
                if (index == original) break;
            }
            
            return nullptr;
        }
    };
    
    constexpr auto create_hashmap() {
        ConstexprHashMap<int, int, 16> map;
        map.insert(1, 10);
        map.insert(5, 50);
        map.insert(3, 30);
        map.insert(7, 70);
        return map;
    }
    
    constexpr auto test_map = create_hashmap();
    constexpr auto* value_5 = test_map.find(5);
    constexpr auto* value_9 = test_map.find(9);
    
    std::cout << "\n3. 编译期哈希表:\n";
    std::cout << "查找键5: " << (value_5 ? *value_5 : -1) << "\n";
    std::cout << "查找键9: " << (value_9 ? *value_9 : -1) << " (不存在)\n";
    
    // 4.4 编译期排序算法
    template<typename T, size_t N>
    constexpr void constexpr_sort(T (&arr)[N]) {
        // 简单选择排序
        for (size_t i = 0; i < N - 1; ++i) {
            size_t min_idx = i;
            for (size_t j = i + 1; j < N; ++j) {
                if (arr[j] < arr[min_idx]) {
                    min_idx = j;
                }
            }
            if (min_idx != i) {
                T temp = arr[i];
                arr[i] = arr[min_idx];
                arr[min_idx] = temp;
            }
        }
    }
    
    constexpr auto create_sorted_array() {
        int arr[] = {64, 34, 25, 12, 22, 11, 90};
        constexpr_sort(arr);
        return std::array<int, 7>{arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]};
    }
    
    constexpr auto sorted_array = create_sorted_array();
    
    std::cout << "\n4. 编译期排序:\n";
    std::cout << "排序后数组: ";
    for (size_t i = 0; i < sorted_array.size(); ++i) {
        std::cout << sorted_array[i] << " ";
    }
    std::cout << "\n\n";
}

// ===== 5. 实际应用案例演示 =====
void demonstrate_practical_applications() {
    std::cout << "=== 实际应用案例演示 ===\n";
    
    // 案例1：编译期正弦表生成
    template<size_t N>
    constexpr auto generate_sin_table() {
        std::array<double, N> table{};
        constexpr double pi = 3.14159265358979323846;
        
        for (size_t i = 0; i < N; ++i) {
            double angle = 2.0 * pi * i / N;
            
            // 泰勒级数近似 sin(x)
            double sin_val = 0.0;
            double term = angle;
            for (int j = 1; j <= 15; j += 2) {  // 奇数项
                sin_val += term;
                term *= -angle * angle / ((j + 1) * (j + 2));
            }
            
            table[i] = sin_val;
        }
        
        return table;
    }
    
    constexpr auto sin_table = generate_sin_table<360>();
    
    std::cout << "1. 编译期正弦表生成:\n";
    std::cout << "sin(0°) ≈ " << sin_table[0] << "\n";
    std::cout << "sin(90°) ≈ " << sin_table[90] << "\n";
    std::cout << "sin(180°) ≈ " << sin_table[180] << "\n";
    std::cout << "sin(270°) ≈ " << sin_table[270] << "\n";
    
    // 案例2：编译期CRC表生成
    constexpr uint32_t crc32_polynomial = 0xEDB88320;
    
    constexpr auto generate_crc_table() {
        std::array<uint32_t, 256> table{};
        
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 8; j > 0; --j) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ crc32_polynomial;
                } else {
                    crc >>= 1;
                }
            }
            table[i] = crc;
        }
        
        return table;
    }
    
    constexpr auto crc_table = generate_crc_table();
    
    constexpr uint32_t calculate_crc32(const char* data, size_t length) {
        uint32_t crc = 0xFFFFFFFF;
        
        for (size_t i = 0; i < length; ++i) {
            uint8_t byte = static_cast<uint8_t>(data[i]);
            crc = crc_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
        }
        
        return crc ^ 0xFFFFFFFF;
    }
    
    constexpr const char* test_string = "Hello, World!";
    constexpr uint32_t test_crc = calculate_crc32(test_string, 13);
    
    std::cout << "\n2. 编译期CRC32计算:\n";
    std::cout << "字符串: \"" << test_string << "\"\n";
    std::cout << "CRC32值: 0x" << std::hex << test_crc << std::dec << "\n";
    
    // 案例3：编译期状态机
    enum class State { IDLE, PROCESSING, COMPLETED, ERROR };
    enum class Event { START, PROCESS, FINISH, FAIL, RESET };
    
    constexpr State transition(State current, Event event) {
        switch (current) {
            case State::IDLE:
                return (event == Event::START) ? State::PROCESSING : State::IDLE;
            case State::PROCESSING:
                switch (event) {
                    case Event::FINISH: return State::COMPLETED;
                    case Event::FAIL: return State::ERROR;
                    case Event::RESET: return State::IDLE;
                    default: return State::PROCESSING;
                }
            case State::COMPLETED:
                return (event == Event::RESET) ? State::IDLE : State::COMPLETED;
            case State::ERROR:
                return (event == Event::RESET) ? State::IDLE : State::ERROR;
        }
        return State::IDLE;
    }
    
    // 编译期状态序列验证
    constexpr bool validate_sequence() {
        State state = State::IDLE;
        state = transition(state, Event::START);      // IDLE -> PROCESSING
        state = transition(state, Event::PROCESS);    // PROCESSING -> PROCESSING
        state = transition(state, Event::FINISH);     // PROCESSING -> COMPLETED
        state = transition(state, Event::RESET);      // COMPLETED -> IDLE
        return state == State::IDLE;
    }
    
    constexpr bool sequence_valid = validate_sequence();
    
    std::cout << "\n3. 编译期状态机:\n";
    std::cout << "状态转换序列验证: " << (sequence_valid ? "通过" : "失败") << "\n";
    
    // 案例4：编译期单位换算
    template<int Meters, int Seconds>
    struct Unit {
        static constexpr int meters = Meters;
        static constexpr int seconds = Seconds;
        double value;
        
        constexpr Unit(double v) : value(v) {}
        
        template<int M2, int S2>
        constexpr bool compatible_with(const Unit<M2, S2>&) const {
            return Meters == M2 && Seconds == S2;
        }
    };
    
    // 定义基本单位
    using Length = Unit<1, 0>;      // 米
    using Time = Unit<0, 1>;        // 秒
    using Velocity = Unit<1, -1>;   // 米/秒
    using Acceleration = Unit<1, -2>; // 米/秒²
    
    constexpr Length distance(100.0);         // 100米
    constexpr Time time_taken(10.0);          // 10秒
    constexpr Velocity speed(distance.value / time_taken.value);  // 10米/秒
    
    std::cout << "\n4. 编译期单位系统:\n";
    std::cout << "距离: " << distance.value << " 米\n";
    std::cout << "时间: " << time_taken.value << " 秒\n";
    std::cout << "速度: " << speed.value << " 米/秒\n";
    
    std::cout << "\n";
}

// ===== 性能基准测试 =====
void benchmark_compile_time_vs_runtime() {
    std::cout << "=== 编译期 vs 运行期性能基准测试 ===\n";
    
    const int iterations = 1000000;
    
    // 运行期计算
    auto runtime_factorial = [](int n) -> long long {
        long long result = 1;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    };
    
    auto runtime_fibonacci = [](int n) -> long long {
        if (n <= 1) return n;
        long long a = 0, b = 1;
        for (int i = 2; i <= n; ++i) {
            long long temp = a + b;
            a = b;
            b = temp;
        }
        return b;
    };
    
    // 编译期预计算的值
    constexpr long long compile_time_fact_10 = factorial(10);
    constexpr long long compile_time_fib_20 = fibonacci(20);
    
    // 基准测试：运行期计算
    auto start = std::chrono::high_resolution_clock::now();
    
    long long runtime_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        runtime_sum += runtime_factorial(10) + runtime_fibonacci(20);
    }
    
    auto runtime_duration = std::chrono::high_resolution_clock::now() - start;
    
    // 基准测试：使用编译期预计算值
    start = std::chrono::high_resolution_clock::now();
    
    long long compile_time_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        compile_time_sum += compile_time_fact_10 + compile_time_fib_20;
    }
    
    auto compile_time_duration = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "性能对比结果 (" << iterations << " 次迭代):\n";
    std::cout << "运行期计算耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(runtime_duration).count() 
              << " 微秒 (结果: " << runtime_sum << ")\n";
    std::cout << "编译期预计算耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(compile_time_duration).count() 
              << " 微秒 (结果: " << compile_time_sum << ")\n";
    
    double speedup = static_cast<double>(runtime_duration.count()) / compile_time_duration.count();
    std::cout << "编译期优势: " << speedup << "x 性能提升\n";
    
    // 查表法性能测试
    constexpr auto factorial_table = []() {
        std::array<long long, 21> table{};
        table[0] = 1;
        for (int i = 1; i < 21; ++i) {
            table[i] = table[i-1] * i;
        }
        return table;
    }();
    
    start = std::chrono::high_resolution_clock::now();
    
    long long table_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        table_sum += factorial_table[10] + factorial_table[15];
    }
    
    auto table_duration = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "查表法耗时: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(table_duration).count() 
              << " 微秒 (结果: " << table_sum << ")\n";
    
    std::cout << "\n编译期计算优势总结:\n";
    std::cout << "- 零运行时计算开销\n";
    std::cout << "- 编译期错误检测\n";
    std::cout << "- 代码优化和常量折叠\n";
    std::cout << "- 减少运行时内存占用\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++11/14/17/20 编译期计算深度解析\n";
    std::cout << "=================================\n";
    
    demonstrate_constexpr_basics();
    demonstrate_template_recursion();
    demonstrate_cpp20_enhancements();
    demonstrate_compile_time_structures();
    demonstrate_practical_applications();
    benchmark_compile_time_vs_runtime();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 14_compile_time_computation.cpp -o compile_time_computation
./compile_time_computation

C++20版本:
g++ -std=c++20 -O2 -Wall 14_compile_time_computation.cpp -o compile_time_computation

关键学习点:
1. constexpr实现编译期常量表达式计算，提供零开销抽象
2. 模板递归是C++11-17时代主要的编译期计算技术
3. C++20的consteval/constinit提供更精确的编译期控制
4. 编译期数据结构支持复杂的元编程算法实现
5. 实际应用中编译期计算可显著提升性能

注意事项:
- 编译期计算会增加编译时间，需要权衡得失
- constexpr函数有复杂度限制，避免过度递归
- C++20的constexpr动态分配仅在编译期有效
- 合理使用编译期计算可以实现零开销的高性能代码
*/