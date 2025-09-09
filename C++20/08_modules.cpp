/**
 * C++20 Modules模块化架构深度解析（概念演示版本）
 * 
 * 核心概念：
 * 1. 模块接口单元 - 定义模块导出的接口，替代传统头文件
 * 2. 模块实现单元 - 实现模块的具体功能，隐藏实现细节
 * 3. 模块分区 - 大型模块的逻辑分割和组织方式
 * 4. 私有模块片段 - 模块内部实现细节的封装机制
 * 5. 模块导入和依赖管理 - 现代化的模块依赖解析系统
 */

/*
重要说明：
C++20 Modules是一个复杂的编译系统特性，需要编译器和构建系统的深度支持。
目前主流编译器的Modules支持情况：
- GCC 11+: 实验性支持，需要特殊编译选项
- Clang 15+: 部分支持，仍在完善中  
- MSVC 19.28+: 较好支持，需要/experimental:module

本文件以概念演示的方式展示Modules的设计思想和语法结构，
实际使用需要配置相应的编译器和构建环境。
*/

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <format>

// ===== 模块概念演示 =====

/*
// 这是传统的模块接口单元语法示例 (需要modules支持的编译器)

// math_utilities.cppm (模块接口单元)
export module math_utilities;

import <iostream>;
import <vector>;
import <numeric>;

// 导出的函数声明
export namespace math_utils {
    double calculate_mean(const std::vector<double>& data);
    double calculate_variance(const std::vector<double>& data);
    
    template<typename T>
    T power(T base, int exponent);
    
    class Statistics {
    public:
        explicit Statistics(std::vector<double> data);
        
        double mean() const;
        double variance() const;
        double standard_deviation() const;
        size_t size() const;
        
    private:
        std::vector<double> data_;
        mutable double cached_mean_ = 0.0;
        mutable bool mean_calculated_ = false;
    };
}

// 模块内部使用的辅助函数（不导出）
namespace {
    double sum_of_squares(const std::vector<double>& data, double mean);
}

// math_utilities.cpp (模块实现单元)
module math_utilities;

// 实现导出的函数
namespace math_utils {
    double calculate_mean(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    }
    
    double calculate_variance(const std::vector<double>& data) {
        if (data.size() < 2) return 0.0;
        double mean = calculate_mean(data);
        return sum_of_squares(data, mean) / (data.size() - 1);
    }
    
    template<typename T>
    T power(T base, int exponent) {
        T result = T{1};
        for (int i = 0; i < exponent; ++i) {
            result *= base;
        }
        return result;
    }
    
    // Statistics类的实现
    Statistics::Statistics(std::vector<double> data) : data_(std::move(data)) {}
    
    double Statistics::mean() const {
        if (!mean_calculated_) {
            cached_mean_ = calculate_mean(data_);
            mean_calculated_ = true;
        }
        return cached_mean_;
    }
    
    double Statistics::variance() const {
        return calculate_variance(data_);
    }
    
    double Statistics::standard_deviation() const {
        return std::sqrt(variance());
    }
    
    size_t Statistics::size() const {
        return data_.size();
    }
}

// 内部辅助函数实现
namespace {
    double sum_of_squares(const std::vector<double>& data, double mean) {
        return std::accumulate(data.begin(), data.end(), 0.0,
            [mean](double acc, double value) {
                double diff = value - mean;
                return acc + diff * diff;
            });
    }
}

// main.cpp (使用模块的代码)
import math_utilities;

int main() {
    std::vector<double> dataset = {1.2, 2.5, 3.1, 4.8, 2.9, 1.7, 3.4, 2.8, 4.1, 3.6};
    
    // 直接使用导出的函数
    double mean = math_utils::calculate_mean(dataset);
    double variance = math_utils::calculate_variance(dataset);
    
    std::cout << std::format("平均值: {:.2f}\n", mean);
    std::cout << std::format("方差: {:.2f}\n", variance);
    
    // 使用导出的类
    math_utils::Statistics stats(dataset);
    std::cout << std::format("统计对象 - 均值: {:.2f}, 标准差: {:.2f}\n", 
                             stats.mean(), stats.standard_deviation());
    
    return 0;
}

编译命令示例:
// GCC (实验性)
g++ -std=c++20 -fmodules-ts -xc++-system-header iostream
g++ -std=c++20 -fmodules-ts --precompile math_utilities.cppm -o math_utilities.gcm
g++ -std=c++20 -fmodules-ts -c math_utilities.cpp -o math_utilities.o
g++ -std=c++20 -fmodules-ts main.cpp math_utilities.o -o program

// MSVC
cl /std:c++20 /experimental:module /EHsc /c math_utilities.cppm
cl /std:c++20 /experimental:module /EHsc main.cpp math_utilities.obj

*/

// ===== 传统方式vs模块化方式对比演示 =====

namespace traditional_approach {
    // 传统的头文件方式存在的问题演示
    
    // 模拟头文件的重复包含问题
    void demonstrate_header_problems() {
        std::cout << "=== 传统头文件方式的问题 ===\n";
        
        std::cout << "1. 头文件重复包含导致编译时间增长\n";
        std::cout << "2. 宏污染和名称冲突风险\n";  
        std::cout << "3. 实现细节暴露，破坏封装性\n";
        std::cout << "4. 依赖关系复杂，难以管理\n";
        std::cout << "5. 模板实例化导致的代码膨胀\n";
        
        std::cout << "\n传统方式编译过程:\n";
        std::cout << "  源文件 -> 预处理器 -> 词法分析 -> 语法分析 -> ... \n";
        std::cout << "  每个翻译单元都要重新处理相同的头文件内容\n";
        
        std::cout << "\n";
    }
}

namespace modules_concept {
    // 模块化方式的优势演示
    
    void demonstrate_modules_advantages() {
        std::cout << "=== C++20 Modules的优势 ===\n";
        
        std::cout << "1. 编译时间优化:\n";
        std::cout << "   - 模块接口只编译一次，可重复使用\n";
        std::cout << "   - 避免重复的头文件预处理\n";
        std::cout << "   - 增量编译更高效\n\n";
        
        std::cout << "2. 更好的封装性:\n";
        std::cout << "   - 只导出明确声明的接口\n";
        std::cout << "   - 实现细节完全隐藏\n";
        std::cout << "   - 避免宏和内部名称泄漏\n\n";
        
        std::cout << "3. 依赖管理改善:\n";
        std::cout << "   - 明确的模块导入声明\n";
        std::cout << "   - 循环依赖检测\n";
        std::cout << "   - 更清晰的模块边界\n\n";
        
        std::cout << "4. 工具支持增强:\n";
        std::cout << "   - IDE可以更准确的代码补全\n";
        std::cout << "   - 重构工具理解模块边界\n";
        std::cout << "   - 调试信息更精确\n\n";
        
        std::cout << "\n";
    }
    
    // 模拟模块化的数学库实现
    class MathLibraryDemo {
    private:
        // 这些将是模块内部的私有实现
        static double internal_precision_;
        
        static double safe_divide(double a, double b) {
            return (std::abs(b) < internal_precision_) ? 0.0 : a / b;
        }
        
    public:
        // 这些将是导出的公共接口
        struct BasicMath {
            static double add(double a, double b) { return a + b; }
            static double multiply(double a, double b) { return a * b; }
            static double divide(double a, double b) { return safe_divide(a, b); }
        };
        
        struct VectorMath {
            static double dot_product(const std::vector<double>& a, 
                                    const std::vector<double>& b) {
                if (a.size() != b.size()) return 0.0;
                return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
            }
            
            static std::vector<double> vector_add(const std::vector<double>& a,
                                                const std::vector<double>& b) {
                std::vector<double> result;
                if (a.size() != b.size()) return result;
                
                result.reserve(a.size());
                std::transform(a.begin(), a.end(), b.begin(), 
                             std::back_inserter(result),
                             [](double x, double y) { return x + y; });
                return result;
            }
        };
        
        class Matrix {
        private:
            std::vector<std::vector<double>> data_;
            size_t rows_, cols_;
            
        public:
            Matrix(size_t rows, size_t cols) 
                : rows_(rows), cols_(cols), data_(rows, std::vector<double>(cols, 0.0)) {}
            
            double& at(size_t row, size_t col) { 
                return data_[row][col]; 
            }
            
            const double& at(size_t row, size_t col) const { 
                return data_[row][col]; 
            }
            
            Matrix multiply(const Matrix& other) const {
                if (cols_ != other.rows_) {
                    throw std::invalid_argument("矩阵维度不匹配");
                }
                
                Matrix result(rows_, other.cols_);
                for (size_t i = 0; i < rows_; ++i) {
                    for (size_t j = 0; j < other.cols_; ++j) {
                        for (size_t k = 0; k < cols_; ++k) {
                            result.at(i, j) += at(i, k) * other.at(k, j);
                        }
                    }
                }
                return result;
            }
            
            size_t rows() const { return rows_; }
            size_t cols() const { return cols_; }
        };
    };
    
    double MathLibraryDemo::internal_precision_ = 1e-10;
    
    void demonstrate_modular_design() {
        std::cout << "=== 模块化设计演示 ===\n";
        
        // 基础数学运算
        std::cout << "基础数学运算:\n";
        double a = 15.5, b = 3.2;
        std::cout << std::format("  {} + {} = {}\n", a, b, 
                                MathLibraryDemo::BasicMath::add(a, b));
        std::cout << std::format("  {} * {} = {}\n", a, b,
                                MathLibraryDemo::BasicMath::multiply(a, b));
        std::cout << std::format("  {} / {} = {}\n", a, b,
                                MathLibraryDemo::BasicMath::divide(a, b));
        
        // 向量运算
        std::cout << "\n向量运算:\n";
        std::vector<double> v1 = {1.0, 2.0, 3.0};
        std::vector<double> v2 = {4.0, 5.0, 6.0};
        
        double dot = MathLibraryDemo::VectorMath::dot_product(v1, v2);
        std::cout << std::format("  向量点积: {}\n", dot);
        
        auto sum = MathLibraryDemo::VectorMath::vector_add(v1, v2);
        std::cout << "  向量相加: [";
        for (size_t i = 0; i < sum.size(); ++i) {
            std::cout << sum[i];
            if (i < sum.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
        
        // 矩阵运算
        std::cout << "\n矩阵运算:\n";
        MathLibraryDemo::Matrix m1(2, 3);
        m1.at(0, 0) = 1; m1.at(0, 1) = 2; m1.at(0, 2) = 3;
        m1.at(1, 0) = 4; m1.at(1, 1) = 5; m1.at(1, 2) = 6;
        
        MathLibraryDemo::Matrix m2(3, 2);
        m2.at(0, 0) = 7; m2.at(0, 1) = 8;
        m2.at(1, 0) = 9; m2.at(1, 1) = 10;
        m2.at(2, 0) = 11; m2.at(2, 1) = 12;
        
        try {
            auto result = m1.multiply(m2);
            std::cout << "  矩阵乘法结果 (2x2):\n";
            for (size_t i = 0; i < result.rows(); ++i) {
                std::cout << "    [";
                for (size_t j = 0; j < result.cols(); ++j) {
                    std::cout << std::format("{:6.0f}", result.at(i, j));
                    if (j < result.cols() - 1) std::cout << ", ";
                }
                std::cout << "]\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  错误: " << e.what() << "\n";
        }
        
        std::cout << "\n";
    }
}

// ===== 模块分区概念演示 =====
namespace module_partitions {
    /*
    模块分区允许将大型模块分割为逻辑单元：
    
    // graphics.cppm (主模块接口)
    export module graphics;
    
    export import :shapes;     // 导入并重新导出shapes分区
    export import :colors;     // 导入并重新导出colors分区  
    import :internal_utils;    // 仅导入，不重新导出
    
    // graphics-shapes.cppm (shapes分区)
    export module graphics:shapes;
    
    export class Circle {
    public:
        Circle(double radius);
        double area() const;
        double circumference() const;
    private:
        double radius_;
    };
    
    export class Rectangle {
    public:
        Rectangle(double width, double height);
        double area() const;
        double perimeter() const;
    private:
        double width_, height_;
    };
    
    // graphics-colors.cppm (colors分区)  
    export module graphics:colors;
    
    export struct RGB {
        uint8_t r, g, b;
        RGB(uint8_t red, uint8_t green, uint8_t blue);
    };
    
    export struct HSV {
        double h, s, v;
        HSV(double hue, double saturation, double value);
        RGB to_rgb() const;
    };
    */
    
    // 模拟分区设计的概念演示
    void demonstrate_partition_concept() {
        std::cout << "=== 模块分区概念演示 ===\n";
        
        std::cout << "大型模块可以分解为逻辑分区:\n";
        std::cout << "1. 主模块接口 - 统一的对外接口\n";
        std::cout << "2. 功能分区 - 按功能领域划分的子模块\n";
        std::cout << "3. 实现分区 - 内部实现细节的封装\n";
        std::cout << "4. 工具分区 - 辅助工具和实用函数\n\n";
        
        std::cout << "分区的优势:\n";
        std::cout << "- 更好的代码组织和可维护性\n";
        std::cout << "- 独立的编译单元，支持增量编译\n";
        std::cout << "- 清晰的模块内部架构\n";
        std::cout << "- 支持大型项目的模块化开发\n\n";
        
        std::cout << "分区的编译依赖关系:\n";
        std::cout << "  主模块 <- 功能分区A <- 实现细节\n";
        std::cout << "         <- 功能分区B <- 实现细节\n";
        std::cout << "         <- 内部工具分区\n\n";
        
        std::cout << "\n";
    }
}

// ===== 私有模块片段概念演示 =====
namespace private_module_fragment {
    /*
    私有模块片段允许在同一个文件中混合接口和实现：
    
    // example.cppm
    export module example;
    
    import <iostream>;
    import <vector>;
    
    // 导出的接口
    export class DataProcessor {
    public:
        void process_data(const std::vector<int>& data);
        int get_result() const;
        
    private:
        int result_ = 0;
        void internal_process(int value);  // 声明私有方法
    };
    
    // 私有模块片段开始
    module :private;
    
    // 私有实现，不会被导出
    void DataProcessor::internal_process(int value) {
        result_ += value * 2;  // 具体的处理逻辑
    }
    
    void DataProcessor::process_data(const std::vector<int>& data) {
        result_ = 0;
        for (int value : data) {
            internal_process(value);
        }
    }
    
    int DataProcessor::get_result() const {
        return result_;
    }
    
    // 其他私有辅助函数
    namespace {
        bool is_valid_data(int value) {
            return value >= 0 && value <= 1000;
        }
    }
    */
    
    void demonstrate_private_fragment_concept() {
        std::cout << "=== 私有模块片段概念演示 ===\n";
        
        std::cout << "私有模块片段的用途:\n";
        std::cout << "1. 接口和实现在同一文件中\n";
        std::cout << "2. 实现细节不会被导出\n";
        std::cout << "3. 减少文件数量和管理复杂性\n";
        std::cout << "4. 保持良好的封装性\n\n";
        
        std::cout << "文件结构:\n";
        std::cout << "  export module name;     // 模块声明\n";
        std::cout << "  // 导出的接口声明\n";
        std::cout << "  module :private;        // 私有片段开始\n";
        std::cout << "  // 私有实现代码\n\n";
        
        std::cout << "编译时处理:\n";
        std::cout << "- 接口部分生成模块接口单元\n";
        std::cout << "- 私有片段编译为目标代码\n";
        std::cout << "- 使用者只能访问导出的接口\n\n";
        
        std::cout << "\n";
    }
}

// ===== 模块导入和依赖管理 =====
namespace module_dependencies {
    void demonstrate_import_mechanisms() {
        std::cout << "=== 模块导入和依赖管理 ===\n";
        
        std::cout << "导入语句的类型:\n";
        std::cout << "1. import module_name;           // 导入命名模块\n";
        std::cout << "2. import :partition_name;       // 导入模块分区\n";
        std::cout << "3. import <header>;              // 导入头文件单元\n";
        std::cout << "4. export import module_name;    // 重新导出模块\n\n";
        
        std::cout << "依赖解析过程:\n";
        std::cout << "1. 编译器分析import声明\n";
        std::cout << "2. 查找已编译的模块接口单元\n";
        std::cout << "3. 建立依赖关系图\n";
        std::cout << "4. 检测循环依赖\n";
        std::cout << "5. 确定编译顺序\n\n";
        
        std::cout << "与传统#include的区别:\n";
        std::cout << "- import是语法结构，不是文本替换\n";
        std::cout << "- 模块边界清晰，避免名称污染\n";
        std::cout << "- 支持循环依赖检测和解决\n";
        std::cout << "- 更快的编译速度和增量编译\n\n";
        
        std::cout << "\n";
    }
    
    void demonstrate_migration_strategy() {
        std::cout << "=== 从头文件到模块的迁移策略 ===\n";
        
        std::cout << "渐进式迁移步骤:\n";
        std::cout << "1. 识别独立的功能模块\n";
        std::cout << "2. 创建模块接口单元\n";
        std::cout << "3. 迁移实现代码\n";
        std::cout << "4. 更新依赖关系\n";
        std::cout << "5. 测试和验证\n\n";
        
        std::cout << "兼容性考虑:\n";
        std::cout << "- 保留传统头文件作为过渡期接口\n";
        std::cout << "- 使用条件编译支持多种构建方式\n";
        std::cout << "- 逐步替换第三方库依赖\n";
        std::cout << "- 工具链升级和团队培训\n\n";
        
        std::cout << "最佳实践建议:\n";
        std::cout << "- 从叶子模块开始迁移\n";
        std::cout << "- 保持模块接口的稳定性\n";
        std::cout << "- 使用语义版本控制模块接口\n";
        std::cout << "- 建立模块化的代码审查流程\n\n";
        
        std::cout << "\n";
    }
}

// ===== 性能对比演示 =====
namespace performance_comparison {
    void demonstrate_compilation_benefits() {
        std::cout << "=== 编译性能优势演示 ===\n";
        
        std::cout << "传统头文件方式的编译开销:\n";
        std::cout << "- 每个翻译单元重复预处理相同头文件\n";
        std::cout << "- 模板实例化重复进行\n";
        std::cout << "- 大量的文本处理和语法分析\n";
        std::cout << "- 链接时模板去重的额外开销\n\n";
        
        std::cout << "模块化方式的性能提升:\n";
        std::cout << "- 模块接口只编译一次\n";
        std::cout << "- 预编译的二进制模块接口\n";
        std::cout << "- 减少重复的语法分析\n";
        std::cout << "- 更精确的增量编译\n\n";
        
        std::cout << "理论性能提升 (基于编译器厂商测试):\n";
        std::cout << "- 编译时间: 减少20-50%\n";
        std::cout << "- 内存使用: 减少15-30%\n";
        std::cout << "- 增量编译: 提升30-70%\n";
        std::cout << "- 链接时间: 减少10-25%\n\n";
        
        std::cout << "注意：具体性能提升取决于:\n";
        std::cout << "- 项目规模和复杂度\n";
        std::cout == "- 编译器实现质量\n";
        std::cout << "- 模块划分的合理性\n";
        std::cout << "- 构建系统的优化程度\n\n";
        
        std::cout << "\n";
    }
}

// ===== 实际应用场景演示 =====
void demonstrate_real_world_scenarios() {
    std::cout << "=== 实际应用场景 ===\n";
    
    std::cout << "适合使用Modules的场景:\n";
    std::cout << "1. 大型C++项目的架构重构\n";
    std::cout << "2. 库的API设计和版本管理\n";
    std::cout << "3. 跨团队的代码共享和复用\n";
    std::cout << "4. 编译性能要求较高的项目\n";
    std::cout << "5. 新项目的现代化架构设计\n\n";
    
    std::cout << "当前的限制和考虑:\n";
    std::cout << "1. 编译器支持仍在完善中\n";
    std::cout << "2. 构建系统需要适配升级\n";
    std::cout << "3. 第三方库的模块化程度有限\n";
    std::cout << "4. 团队学习成本和技术储备\n";
    std::cout << "5. 调试工具的兼容性问题\n\n";
    
    std::cout << "未来发展趋势:\n";
    std::cout << "- 标准库全面模块化\n";
    std::cout << "- 包管理器与模块系统集成\n";
    std::cout << "- IDE和工具链的深度集成\n";
    std::cout << "- 跨语言模块接口标准\n";
    std::cout << "- 云端模块编译和缓存服务\n\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20 Modules模块化架构深度解析\n";
    std::cout << "==================================\n";
    
    traditional_approach::demonstrate_header_problems();
    modules_concept::demonstrate_modules_advantages();
    modules_concept::demonstrate_modular_design();
    module_partitions::demonstrate_partition_concept();
    private_module_fragment::demonstrate_private_fragment_concept();
    module_dependencies::demonstrate_import_mechanisms();
    module_dependencies::demonstrate_migration_strategy();
    performance_comparison::demonstrate_compilation_benefits();
    demonstrate_real_world_scenarios();
    
    return 0;
}

/*
编译和运行建议:
标准编译 (概念演示版本):
g++ -std=c++20 -O2 -Wall 08_modules.cpp -o modules_demo
./modules_demo

实际模块编译 (需要支持modules的编译器):
// GCC (实验性支持)
g++ -std=c++20 -fmodules-ts -xc++-system-header iostream vector
g++ -std=c++20 -fmodules-ts --precompile module.cppm -o module.gcm  
g++ -std=c++20 -fmodules-ts main.cpp module.o -o program

// MSVC (较好支持)  
cl /std:c++20 /experimental:module /EHsc /c module.cppm
cl /std:c++20 /experimental:module /EHsc main.cpp module.obj

// Clang (部分支持)
clang++ -std=c++20 -fmodules -c module.cppm -o module.pcm
clang++ -std=c++20 -fmodule-file=module.pcm main.cpp -o program

关键学习点:
1. Modules彻底改变了C++的编译模型和代码组织方式
2. 提供了更好的封装性、性能和依赖管理
3. 支持模块分区和私有片段的灵活架构设计
4. 需要编译器、构建系统和工具链的全面支持
5. 是C++现代化发展的重要里程碑

注意事项:
- 当前编译器对Modules的支持仍在快速发展中
- 需要配置相应的编译器选项和构建环境
- 与传统头文件的混合使用需要careful考虑
- 第三方库的模块化支持需要时间完善
- 团队采用需要考虑学习成本和工具链迁移
*/