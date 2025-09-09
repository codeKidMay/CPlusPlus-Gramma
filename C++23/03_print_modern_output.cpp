/**
 * C++23 std::print现代化输出系统深度解析
 * 
 * 核心概念：
 * 1. 格式化输出统一化 - 统一C++输出范式，简化日常开发
 * 2. 性能优化 - 直接系统调用，避免iostream开销
 * 3. Unicode支持 - 原生UTF-8输出和国际化支持
 * 4. 编译期检查 - 格式字符串的编译期验证
 * 5. 线程安全 - 无锁的输出机制
 */

#include <iostream>
#include <print>
#include <format>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <locale>
#include <codecvt>
#include <ranges>
#include <algorithm>
#include <fstream>
#include <sstream>

// ===== 1. 输出系统的演进历程 =====
void demonstrate_output_evolution() {
    std::cout << "=== 输出系统的演进历程 ===\n";
    
    // C语言时代：printf系列
    std::cout << "1. C语言printf方式:\n";
    printf("Hello %s, you are %d years old\n", "Alice", 25);
    printf("Pi = %.6f, Scientific = %e\n", 3.14159265359, 299792458.0);
    
    // C++早期：iostream
    std::cout << "\n2. C++ iostream方式:\n";
    std::cout << "Hello " << "Alice" << ", you are " << 25 << " years old" << std::endl;
    std::cout << "Pi = " << std::setprecision(6) << std::fixed << 3.14159265359 
              << ", Scientific = " << std::scientific << 299792458.0 << std::endl;
    
    // C++20：std::format
    std::cout << "\n3. C++20 std::format方式:\n";
    std::cout << std::format("Hello {}, you are {} years old\n", "Alice", 25);
    std::cout << std::format("Pi = {:.6f}, Scientific = {:.2e}\n", 3.14159265359, 299792458.0);
    
    // C++23：std::print（最简洁）
    std::cout << "\n4. C++23 std::print方式:\n";
    std::print("Hello {}, you are {} years old\n", "Alice", 25);
    std::print("Pi = {:.6f}, Scientific = {:.2e}\n", 3.14159265359, 299792458.0);
    std::println("自动换行: Hello {}, age {}", "Bob", 30);
    
    std::cout << "\n";
}

// ===== 2. 格式化选项和类型处理 =====
void demonstrate_formatting_options() {
    std::cout << "=== 格式化选项和类型处理 ===\n";
    
    // 数值格式化
    std::print("整数格式化:\n");
    std::println("十进制: {:d}, 十六进制: {:x}, 八进制: {:o}, 二进制: {:b}", 255, 255, 255, 255);
    std::println("带符号: {:+d}, 填充: {:06d}, 科学计数: {:e}", 42, 42, 123456789.0);
    
    // 浮点数精度控制
    std::print("\n浮点数格式化:\n");
    double pi = 3.141592653589793;
    std::println("默认: {}, 固定小数: {:.3f}, 科学计数: {:.2e}", pi, pi, pi);
    std::println("通用格式: {:g}, 百分比: {:.1%}", pi, pi);
    
    // 字符串和字符处理
    std::print("\n字符串格式化:\n");
    std::string text = "Hello";
    std::println("默认: {}, 左对齐: {:<10}, 右对齐: {:>10}, 居中: {:^10}", 
                 text, text, text, text);
    std::println("字符填充: {:*^10}, 数字填充: {:0>5}", text, 42);
    
    // 布尔值和指针
    std::print("\n特殊类型:\n");
    std::println("布尔: {}, {:d}, {:^10}", true, false, true);
    int value = 42;
    std::println("指针: {:p}, 地址: {:#018x}", &value, reinterpret_cast<uintptr_t>(&value));
    
    std::cout << "\n";
}

// ===== 3. Unicode和国际化支持 =====
void demonstrate_unicode_support() {
    std::cout << "=== Unicode和国际化支持 ===\n";
    
    // Unicode文本输出
    std::print("Unicode输出:\n");
    std::println("中文: {}, 日语: {}, 韩语: {}", "你好世界", "こんにちは世界", "안녕하세요 세계");
    std::println("Emoji: {} {} {} {}", "🌍", "🚀", "💻", "🎯");
    std::println("数学符号: ∑ ∫ ∞ π α β γ");
    std::println("特殊字符: © ® ™ € £ ¥");
    
    // 多语言数字格式化
    std::print("\n国际化数字格式:\n");
    std::locale::global(std::locale("en_US.UTF-8"));
    std::println("美国格式: {:L}, {:L}", 1234567.89, 1234567);
    
    std::locale::global(std::locale("de_DE.UTF-8"));
    std::println("德国格式: {:L}, {:L}", 1234567.89, 1234567);
    
    std::locale::global(std::locale("ja_JP.UTF-8"));
    std::println("日本格式: {:L}, {:L}", 1234567.89, 1234567);
    
    // 恢复默认locale
    std::locale::global(std::locale::classic());
    
    std::cout << "\n";
}

// ===== 4. 性能对比和优化 =====
class PerformanceBenchmark {
public:
    static void run_output_performance_comparison() {
        std::cout << "=== 输出性能对比测试 ===\n";
        
        const int iterations = 100000;
        std::vector<int> data(iterations);
        std::iota(data.begin(), data.end(), 1);
        
        // 测试printf性能
        auto test_printf = [&]() {
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                printf("%d ", i);
            }
            printf("\n");
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        };
        
        // 测试iostream性能
        auto test_iostream = [&]() {
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                std::cout << i << " ";
            }
            std::cout << "\n";
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        };
        
        // 测试std::print性能
        auto test_std_print = [&]() {
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                std::print("{} ", i);
            }
            std::println("");
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        };
        
        // 执行测试
        std::cout << "测试参数: " << iterations << " 次迭代\n\n";
        
        // printf测试
        auto printf_time = test_printf();
        std::println("printf耗时: {}ms", printf_time.count());
        
        // iostream测试
        auto iostream_time = test_iostream();
        std::println("iostream耗时: {}ms", iostream_time.count());
        
        // std::print测试
        auto print_time = test_std_print();
        std::println("std::print耗时: {}ms", print_time.count());
        
        // 性能分析
        std::print("\n性能分析:\n");
        std::println("printf相对性能: {:.2f}x", 
                     static_cast<double>(iostream_time.count()) / printf_time.count());
        std::println("iostream相对性能: {:.2f}x", 
                     static_cast<double>(iostream_time.count()) / iostream_time.count());
        std::println("std::print相对性能: {:.2f}x", 
                     static_cast<double>(iostream_time.count()) / print_time.count());
        
        std::println("\nstd::print的优势:");
        std::println("- 直接系统调用，避免iostream的同步开销");
        std::println("- 编译期格式字符串验证");
        std::println("- 更少的虚函数调用");
        std::println("- 更好的编译器优化机会");
        std::println("- 线程安全的输出机制");
        
        std::cout << "\n";
    }
};

// ===== 5. 线程安全和并发输出 =====
void demonstrate_thread_safety() {
    std::cout << "=== 线程安全和并发输出 ===\n";
    
    const int thread_count = 4;
    const int messages_per_thread = 5;
    
    // 线程安全的std::print测试
    std::println("启动 {} 个线程并发输出...", thread_count);
    
    auto worker = [](int thread_id, int message_count) {
        for (int i = 0; i < message_count; ++i) {
            std::println("线程 {} - 消息 {}", thread_id, i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(worker, i, messages_per_thread);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::println("所有线程完成输出");
    
    // 对比iostream的线程安全问题
    std::println("\niostream线程安全问题演示:");
    std::vector<std::thread> unsafe_threads;
    
    auto unsafe_worker = [](int thread_id, int message_count) {
        for (int i = 0; i < message_count; ++i) {
            std::cout << "不安全线程 " << thread_id << " - 消息 " << i << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    for (int i = 0; i < 2; ++i) {
        unsafe_threads.emplace_back(unsafe_worker, i, 3);
    }
    
    for (auto& t : unsafe_threads) {
        t.join();
    }
    
    std::cout << "\n";
}

// ===== 6. 文件输出和重定向 =====
void demonstrate_file_output() {
    std::cout << "=== 文件输出和重定向 ===\n";
    
    // 标准输出重定向到文件
    std::println("重定向输出到文件...");
    
    // 保存原始缓冲区
    std::streambuf* orig_cout = std::cout.rdbuf();
    
    // 重定向到文件
    std::ofstream file("output_demo.txt");
    std::cout.rdbuf(file.rdbuf());
    
    // 使用std::print输出（不会重定向，直接输出到stdout）
    std::print("这是直接输出的内容\n");
    
    // 使用iostream输出（会被重定向）
    std::cout << "这是重定向到文件的内容" << std::endl;
    
    // 恢复原始缓冲区
    std::cout.rdbuf(orig_cout);
    file.close();
    
    std::println("输出重定向完成，请查看 output_demo.txt");
    
    // 读取并显示文件内容
    std::ifstream input("output_demo.txt");
    if (input.is_open()) {
        std::println("文件内容:");
        std::string line;
        while (std::getline(input, line)) {
            std::println("  {}", line);
        }
        input.close();
    }
    
    // 错误输出到stderr
    std::print(stderr, "错误信息: 这是一个错误输出示例\n");
    std::println(stderr, "错误信息: 带自动换行的错误输出");
    
    std::cout << "\n";
}

// ===== 7. 自定义格式化器和适配器 =====
// 自定义类型的格式化支持
struct Point {
    double x, y, z;
    
    Point(double x, double y, double z) : x(x), y(y), z(z) {}
};

// 为Point类型提供格式化支持
template<>
struct std::formatter<Point> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    
    auto format(const Point& p, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "Point({:.2f}, {:.2f}, {:.2f})", p.x, p.y, p.z);
    }
};

// 复杂的自定义格式化器
struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) 
        : r(r), g(g), b(b), a(a) {}
};

template<>
struct std::formatter<Color> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it == ':') {
            ++it;
            if (it != ctx.end()) {
                format_spec = *it++;
            }
        }
        return it;
    }
    
    auto format(const Color& c, std::format_context& ctx) const {
        switch (format_spec) {
            case 'h':
                return std::format_to(ctx.out(), "#{:02x}{:02x}{:02x}", c.r, c.g, c.b);
            case 'd':
                return std::format_to(ctx.out(), "rgb({}, {}, {})", c.r, c.g, c.b);
            case 'f':
                return std::format_to(ctx.out(), "rgba({:.2f}, {:.2f}, {:.2f}, {:.2f})", 
                                     c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0);
            default:
                return std::format_to(ctx.out(), "Color(r={}, g={}, b={}, a={})", 
                                     c.r, c.g, c.b, c.a);
        }
    }
    
private:
    char format_spec = 'n'; // normal format
};

void demonstrate_custom_formatters() {
    std::cout << "=== 自定义格式化器和适配器 ===\n";
    
    // 使用自定义Point格式化
    Point p1(1.5, 2.7, 3.14);
    Point p2(10.0, 20.0, 30.0);
    
    std::println("点坐标: {}", p1);
    std::println("向量计算: {} + {} = ({:.2f}, {:.2f}, {:.2f})", 
                 p1, p2, p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
    
    // 使用自定义Color格式化
    Color red(255, 0, 0);
    Color green(0, 255, 0);
    Color blue(0, 0, 255, 128);
    
    std::println("颜色信息:");
    std::println("  红色: {}", red);
    std::println("  红色(十六进制): {:h}", red);
    std::println("  红色(十进制): {:d}", red);
    std::println("  红色(浮点): {:f}", red);
    
    std::println("  蓝色: {}", blue);
    std::println("  蓝色(十六进制): {:h}", blue);
    std::println("  蓝色(十进制): {:d}", blue);
    std::println("  蓝色(浮点): {:f}", blue);
    
    // 容器的格式化输出
    std::vector<Point> points{p1, p2, Point(0.0, 0.0, 0.0)};
    std::println("点集合: [{}]", std::format("{}", std::format("{}\n", points[0])));
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++23 std::print现代化输出系统深度解析\n";
    std::cout << "=========================================\n";
    
    demonstrate_output_evolution();
    demonstrate_formatting_options();
    demonstrate_unicode_support();
    PerformanceBenchmark::run_output_performance_comparison();
    demonstrate_thread_safety();
    demonstrate_file_output();
    demonstrate_custom_formatters();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall 03_print_modern_output.cpp -o print_demo
./print_demo

注意：std::print需要C++23支持，目前编译器支持情况：
- GCC 14+: 完整支持
- Clang 18+: 完整支持
- MSVC 19.38+: 完整支持

如果编译器不支持，可以使用替代方案：
- 使用fmtlib库（std::format和std::print的前身）
- 使用C++20的std::format + iostream

关键学习点:
1. std::print提供了简洁、高效、类型安全的输出方案
2. 支持丰富的格式化选项和Unicode国际化
3. 性能显著优于传统的iostream输出
4. 线程安全的并发输出机制
5. 支持自定义类型的格式化扩展

注意事项:
- std::print直接输出到stdout，不支持重定向
- 需要编译器支持C++23标准
- 格式字符串在编译期进行验证，提高安全性
- 在性能关键路径上推荐使用std::print
*/