/**
 * C++23 std::mdspan多维数组深度解析
 * 
 * 核心概念：
 * 1. 多维视图抽象 - 零开销的多维数组访问接口统一化
 * 2. 布局策略 - row_major、column_major、stride等内存布局控制
 * 3. 访问器模式 - 自定义内存访问行为和边界检查
 * 4. 子视图和切片 - submdspan的高效子数组操作
 * 5. 科学计算优化 - 针对数值计算的性能优化设计
 */

#include <iostream>
#include <mdspan>
#include <vector>
#include <array>
#include <memory>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <format>
#include <random>
#include <cassert>

using namespace std::chrono_literals;

// ===== 1. mdspan基础概念和多维索引 =====
void demonstrate_mdspan_basics() {
    std::cout << "=== std::mdspan基础概念和多维索引 ===\n";
    
    // 传统多维数组的问题演示
    std::cout << "传统多维数组方式对比:\n";
    
    // 方式1: C风格二维数组（大小固定）
    int c_array[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    std::cout << "C风格数组访问: " << c_array[1][2] << "\n";
    
    // 方式2: vector<vector>（内存不连续，缓存不友好）
    std::vector<std::vector<int>> vec_matrix = {
        {1, 2, 3, 4},
        {5, 6, 7, 8}, 
        {9, 10, 11, 12}
    };
    
    std::cout << "vector<vector>访问: " << vec_matrix[1][2] << "\n";
    
    // 方式3: 一维vector + 手动索引计算（容易出错）
    std::vector<int> flat_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto manual_access = [&](size_t row, size_t col) -> int& {
        return flat_data[row * 4 + col];  // 手动计算偏移
    };
    
    std::cout << "手动索引访问: " << manual_access(1, 2) << "\n";
    
    // 方式4: C++23 std::mdspan（最佳方案）
    std::mdspan<int, std::dextents<size_t, 2>> matrix_view(flat_data.data(), 3, 4);
    
    std::cout << "mdspan访问: " << matrix_view[1, 2] << "\n";
    
    std::cout << "\nmdspan的优势演示:\n";
    std::cout << "维度信息:\n";
    std::cout << std::format("  维数: {}\n", matrix_view.rank());
    std::cout << std::format("  第0维大小: {}\n", matrix_view.extent(0));
    std::cout << std::format("  第1维大小: {}\n", matrix_view.extent(1));
    std::cout << std::format("  总元素数: {}\n", matrix_view.size());
    
    // 多维遍历
    std::cout << "\n完整矩阵内容:\n";
    for (size_t i = 0; i < matrix_view.extent(0); ++i) {
        for (size_t j = 0; j < matrix_view.extent(1); ++j) {
            std::cout << std::format("{:4}", matrix_view[i, j]);
        }
        std::cout << "\n";
    }
    
    std::cout << "\n";
}

// ===== 2. 布局策略：内存布局的精确控制 =====
void demonstrate_layout_strategies() {
    std::cout << "=== 布局策略：内存布局的精确控制 ===\n";
    
    // 准备测试数据
    std::vector<double> data(12);
    std::iota(data.begin(), data.end(), 1.0);
    
    // 行主序布局（C/C++默认，row-major）
    std::mdspan<double, std::dextents<size_t, 2>, std::layout_right> 
        row_major_matrix(data.data(), 3, 4);
    
    // 列主序布局（Fortran风格，column-major）  
    std::mdspan<double, std::dextents<size_t, 2>, std::layout_left>
        col_major_matrix(data.data(), 3, 4);
    
    // 跨步布局（自定义步长）
    std::array<size_t, 2> extents{3, 4};
    std::array<size_t, 2> strides{4, 1};  // 行主序的步长
    std::layout_stride::mapping stride_mapping{extents, strides};
    std::mdspan<double, std::dextents<size_t, 2>, std::layout_stride>
        stride_matrix(data.data(), stride_mapping);
    
    std::cout << "布局策略对比:\n";
    
    // 显示不同布局的访问模式
    std::cout << "\n原始数据: ";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << std::format("{:4.0f}", data[i]);
    }
    std::cout << "\n";
    
    std::cout << "\n行主序布局 (layout_right):\n";
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            std::cout << std::format("{:4.0f}", row_major_matrix[i, j]);
        }
        std::cout << std::format("  (内存索引: {})\n", i * 4 + j);
    }
    
    std::cout << "\n列主序布局 (layout_left):\n";
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            std::cout << std::format("{:4.0f}", col_major_matrix[i, j]);
        }
        std::cout << std::format("  (内存索引: {})\n", j * 3 + i);
    }
    
    // 展示布局对缓存性能的影响
    std::cout << "\n缓存性能影响分析:\n";
    std::cout << "- 行主序：按行遍历时缓存友好，按列遍历时缓存不友好\n";
    std::cout << "- 列主序：按列遍历时缓存友好，按行遍历时缓存不友好\n";
    std::cout << "- 选择合适的布局对性能至关重要\n";
    
    std::cout << "\n";
}

// ===== 3. 高维张量和科学计算应用 =====
template<typename T>
class Tensor3D {
private:
    std::vector<T> data_;
    std::mdspan<T, std::dextents<size_t, 3>> view_;
    
public:
    Tensor3D(size_t depth, size_t height, size_t width)
        : data_(depth * height * width), view_(data_.data(), depth, height, width) {}
    
    // 3D索引访问
    T& operator()(size_t d, size_t h, size_t w) {
        return view_[d, h, w];
    }
    
    const T& operator()(size_t d, size_t h, size_t w) const {
        return view_[d, h, w];
    }
    
    // 获取原始mdspan视图
    auto span() -> decltype(view_)& { return view_; }
    auto span() const -> const decltype(view_)& { return view_; }
    
    // 获取二维切片（固定深度索引）
    auto slice(size_t depth_idx) {
        return std::submdspan(view_, depth_idx, std::full_extent, std::full_extent);
    }
    
    // 获取一维切片（固定深度和高度）
    auto row(size_t depth_idx, size_t height_idx) {
        return std::submdspan(view_, depth_idx, height_idx, std::full_extent);
    }
    
    // 张量运算
    Tensor3D& operator+=(const Tensor3D& other) {
        for (size_t d = 0; d < view_.extent(0); ++d) {
            for (size_t h = 0; h < view_.extent(1); ++h) {
                for (size_t w = 0; w < view_.extent(2); ++w) {
                    view_[d, h, w] += other.view_[d, h, w];
                }
            }
        }
        return *this;
    }
    
    // 元素统计
    T sum() const {
        T total = T{};
        for (size_t d = 0; d < view_.extent(0); ++d) {
            for (size_t h = 0; h < view_.extent(1); ++h) {
                for (size_t w = 0; w < view_.extent(2); ++w) {
                    total += view_[d, h, w];
                }
            }
        }
        return total;
    }
    
    // 维度信息
    size_t depth() const { return view_.extent(0); }
    size_t height() const { return view_.extent(1); }
    size_t width() const { return view_.extent(2); }
    size_t size() const { return view_.size(); }
};

void demonstrate_high_dimensional_tensors() {
    std::cout << "=== 高维张量和科学计算应用 ===\n";
    
    // 创建3D张量（深度=2, 高度=3, 宽度=4）
    Tensor3D<double> tensor(2, 3, 4);
    
    // 初始化张量数据
    std::cout << "初始化3D张量 (2×3×4):\n";
    double value = 1.0;
    for (size_t d = 0; d < tensor.depth(); ++d) {
        std::cout << std::format("深度层 {}:\n", d);
        for (size_t h = 0; h < tensor.height(); ++h) {
            for (size_t w = 0; w < tensor.width(); ++w) {
                tensor(d, h, w) = value++;
                std::cout << std::format("{:6.1f}", tensor(d, h, w));
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    
    // 切片操作演示
    std::cout << "切片操作演示:\n";
    
    // 获取第0层的二维切片
    auto slice_0 = tensor.slice(0);
    std::cout << "第0层切片 (3×4):\n";
    for (size_t h = 0; h < slice_0.extent(0); ++h) {
        for (size_t w = 0; w < slice_0.extent(1); ++w) {
            std::cout << std::format("{:6.1f}", slice_0[h, w]);
        }
        std::cout << "\n";
    }
    
    // 获取第1层第2行的一维切片
    auto row_slice = tensor.row(1, 2);
    std::cout << "\n第1层第2行切片 (1×4): ";
    for (size_t w = 0; w < row_slice.extent(0); ++w) {
        std::cout << std::format("{:6.1f}", row_slice[w]);
    }
    std::cout << "\n";
    
    // 张量运算
    std::cout << "\n张量运算演示:\n";
    Tensor3D<double> tensor2(2, 3, 4);
    
    // 初始化第二个张量
    for (size_t d = 0; d < tensor2.depth(); ++d) {
        for (size_t h = 0; h < tensor2.height(); ++h) {
            for (size_t w = 0; w < tensor2.width(); ++w) {
                tensor2(d, h, w) = (d + 1) * (h + 1) * (w + 1);
            }
        }
    }
    
    std::cout << std::format("张量1总和: {:.1f}\n", tensor.sum());
    std::cout << std::format("张量2总和: {:.1f}\n", tensor2.sum());
    
    // 张量加法
    tensor += tensor2;
    std::cout << std::format("张量相加后总和: {:.1f}\n", tensor.sum());
    
    std::cout << "\n";
}

// ===== 4. 矩阵运算和线性代数应用 =====
class MatrixOperations {
public:
    // 矩阵乘法：C = A * B
    static void multiply(
        std::mdspan<const double, std::dextents<size_t, 2>> A,
        std::mdspan<const double, std::dextents<size_t, 2>> B,
        std::mdspan<double, std::dextents<size_t, 2>> C
    ) {
        assert(A.extent(1) == B.extent(0));
        assert(C.extent(0) == A.extent(0));
        assert(C.extent(1) == B.extent(1));
        
        // 经典的三重循环矩阵乘法
        for (size_t i = 0; i < C.extent(0); ++i) {
            for (size_t j = 0; j < C.extent(1); ++j) {
                C[i, j] = 0.0;
                for (size_t k = 0; k < A.extent(1); ++k) {
                    C[i, j] += A[i, k] * B[k, j];
                }
            }
        }
    }
    
    // 分块矩阵乘法（缓存优化）
    static void multiply_blocked(
        std::mdspan<const double, std::dextents<size_t, 2>> A,
        std::mdspan<const double, std::dextents<size_t, 2>> B,
        std::mdspan<double, std::dextents<size_t, 2>> C,
        size_t block_size = 64
    ) {
        assert(A.extent(1) == B.extent(0));
        assert(C.extent(0) == A.extent(0));
        assert(C.extent(1) == B.extent(1));
        
        // 初始化结果矩阵
        for (size_t i = 0; i < C.extent(0); ++i) {
            for (size_t j = 0; j < C.extent(1); ++j) {
                C[i, j] = 0.0;
            }
        }
        
        // 分块计算
        for (size_t ii = 0; ii < C.extent(0); ii += block_size) {
            for (size_t jj = 0; jj < C.extent(1); jj += block_size) {
                for (size_t kk = 0; kk < A.extent(1); kk += block_size) {
                    
                    size_t i_end = std::min(ii + block_size, C.extent(0));
                    size_t j_end = std::min(jj + block_size, C.extent(1));
                    size_t k_end = std::min(kk + block_size, A.extent(1));
                    
                    // 块内计算
                    for (size_t i = ii; i < i_end; ++i) {
                        for (size_t j = jj; j < j_end; ++j) {
                            for (size_t k = kk; k < k_end; ++k) {
                                C[i, j] += A[i, k] * B[k, j];
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 矩阵转置
    static void transpose(
        std::mdspan<const double, std::dextents<size_t, 2>> input,
        std::mdspan<double, std::dextents<size_t, 2>> output
    ) {
        assert(input.extent(0) == output.extent(1));
        assert(input.extent(1) == output.extent(0));
        
        for (size_t i = 0; i < input.extent(0); ++i) {
            for (size_t j = 0; j < input.extent(1); ++j) {
                output[j, i] = input[i, j];
            }
        }
    }
};

void demonstrate_matrix_operations() {
    std::cout << "=== 矩阵运算和线性代数应用 ===\n";
    
    // 创建测试矩阵
    const size_t M = 4, N = 3, K = 3;
    
    std::vector<double> A_data(M * N);
    std::vector<double> B_data(N * K);
    std::vector<double> C_data(M * K);
    std::vector<double> C_blocked_data(M * K);
    
    // 初始化矩阵A (4×3)
    std::iota(A_data.begin(), A_data.end(), 1.0);
    
    // 初始化矩阵B (3×3)
    for (size_t i = 0; i < B_data.size(); ++i) {
        B_data[i] = static_cast<double>(i % 3 + 1);
    }
    
    // 创建mdspan视图
    std::mdspan<const double, std::dextents<size_t, 2>> A(A_data.data(), M, N);
    std::mdspan<const double, std::dextents<size_t, 2>> B(B_data.data(), N, K);
    std::mdspan<double, std::dextents<size_t, 2>> C(C_data.data(), M, K);
    std::mdspan<double, std::dextents<size_t, 2>> C_blocked(C_blocked_data.data(), M, K);
    
    // 显示输入矩阵
    std::cout << "矩阵A (4×3):\n";
    for (size_t i = 0; i < A.extent(0); ++i) {
        for (size_t j = 0; j < A.extent(1); ++j) {
            std::cout << std::format("{:6.1f}", A[i, j]);
        }
        std::cout << "\n";
    }
    
    std::cout << "\n矩阵B (3×3):\n";
    for (size_t i = 0; i < B.extent(0); ++i) {
        for (size_t j = 0; j < B.extent(1); ++j) {
            std::cout << std::format("{:6.1f}", B[i, j]);
        }
        std::cout << "\n";
    }
    
    // 执行矩阵乘法
    auto start = std::chrono::high_resolution_clock::now();
    MatrixOperations::multiply(A, B, C);
    auto end = std::chrono::high_resolution_clock::now();
    auto standard_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 执行分块矩阵乘法
    start = std::chrono::high_resolution_clock::now();
    MatrixOperations::multiply_blocked(A, B, C_blocked, 2);
    end = std::chrono::high_resolution_clock::now();
    auto blocked_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 显示结果
    std::cout << "\n矩阵乘法结果C = A × B (4×3):\n";
    for (size_t i = 0; i < C.extent(0); ++i) {
        for (size_t j = 0; j < C.extent(1); ++j) {
            std::cout << std::format("{:8.1f}", C[i, j]);
        }
        std::cout << "\n";
    }
    
    // 验证两种算法结果一致性
    bool results_match = true;
    for (size_t i = 0; i < C.extent(0); ++i) {
        for (size_t j = 0; j < C.extent(1); ++j) {
            if (std::abs(C[i, j] - C_blocked[i, j]) > 1e-10) {
                results_match = false;
                break;
            }
        }
    }
    
    std::cout << std::format("\n性能对比:\n");
    std::cout << std::format("标准算法时间: {}μs\n", standard_time.count());
    std::cout << std::format("分块算法时间: {}μs\n", blocked_time.count());
    std::cout << std::format("结果一致性: {}\n", results_match ? "通过" : "失败");
    
    // 矩阵转置演示
    std::vector<double> AT_data(N * M);
    std::mdspan<double, std::dextents<size_t, 2>> AT(AT_data.data(), N, M);
    
    MatrixOperations::transpose(A, AT);
    
    std::cout << "\n矩阵A转置 (3×4):\n";
    for (size_t i = 0; i < AT.extent(0); ++i) {
        for (size_t j = 0; j < AT.extent(1); ++j) {
            std::cout << std::format("{:6.1f}", AT[i, j]);
        }
        std::cout << "\n";
    }
    
    std::cout << "\n";
}

// ===== 5. 自定义访问器和边界检查 =====
template<typename T>
class BoundsCheckingAccessor {
private:
    std::string_view name_;
    
public:
    using offset_policy = BoundsCheckingAccessor;
    using element_type = T;
    using reference = T&;
    using data_handle_type = T*;
    
    explicit BoundsCheckingAccessor(std::string_view name = "mdspan") : name_(name) {}
    
    template<typename... Indices>
    reference access(data_handle_type p, Indices... indices) const {
        // 这里可以添加边界检查逻辑
        // 在实际应用中，需要访问extent信息
        std::cout << std::format("[{}] 访问索引: (", name_);
        ((std::cout << indices << ", "), ...);
        std::cout << "\b\b) ";
        
        return p[0];  // 简化实现
    }
    
    typename offset_policy::data_handle_type 
    offset(data_handle_type p, size_t i) const {
        return p + i;
    }
};

template<typename T>
class DebugAccessor {
public:
    using offset_policy = DebugAccessor;
    using element_type = T;
    using reference = T&;
    using data_handle_type = T*;
    
    template<typename... Indices>
    reference access(data_handle_type p, Indices... indices) const {
        size_t linear_index = 0;
        // 简化的线性索引计算（实际需要更复杂的逻辑）
        ((linear_index = linear_index + indices), ...);
        
        std::cout << std::format("[DEBUG] 线性索引: {} ", linear_index);
        return *(p + linear_index);
    }
    
    typename offset_policy::data_handle_type 
    offset(data_handle_type p, size_t i) const {
        return p + i;
    }
};

void demonstrate_custom_accessors() {
    std::cout << "=== 自定义访问器和边界检查 ===\n";
    
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    
    // 标准访问器
    std::mdspan<int, std::dextents<size_t, 2>> standard_view(data.data(), 3, 4);
    
    std::cout << "标准访问器使用:\n";
    std::cout << "访问 [1,2]: " << standard_view[1, 2] << "\n";
    
    // 注意：自定义访问器的完整实现需要更复杂的模板机制
    // 这里展示概念性的用法
    
    std::cout << "\n自定义访问器概念:\n";
    std::cout << "- 可以添加边界检查\n";
    std::cout << "- 可以添加访问日志\n";
    std::cout << "- 可以实现特殊的内存访问模式\n";
    std::cout << "- 可以集成性能监控\n";
    
    std::cout << "\n";
}

// ===== 6. 性能基准测试 =====
class PerformanceBenchmark {
public:
    static void run_memory_layout_benchmark() {
        std::cout << "=== 内存布局性能基准测试 ===\n";
        
        const size_t rows = 1000;
        const size_t cols = 1000;
        const size_t iterations = 100;
        
        std::vector<double> data(rows * cols, 1.0);
        
        // 行主序视图
        std::mdspan<double, std::dextents<size_t, 2>, std::layout_right>
            row_major(data.data(), rows, cols);
        
        // 列主序视图（需要重新排列数据）
        std::vector<double> col_data(rows * cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                col_data[j * rows + i] = data[i * cols + j];
            }
        }
        std::mdspan<double, std::dextents<size_t, 2>, std::layout_left>
            col_major(col_data.data(), rows, cols);
        
        // 测试按行遍历的性能
        auto test_row_traversal = [&](auto& matrix, const std::string& name) {
            auto start = std::chrono::high_resolution_clock::now();
            
            double sum = 0.0;
            for (size_t iter = 0; iter < iterations; ++iter) {
                for (size_t i = 0; i < matrix.extent(0); ++i) {
                    for (size_t j = 0; j < matrix.extent(1); ++j) {
                        sum += matrix[i, j];
                    }
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << std::format("{} 按行遍历: {}ms (sum={})\n", 
                                    name, duration.count(), sum);
            return duration;
        };
        
        // 测试按列遍历的性能
        auto test_col_traversal = [&](auto& matrix, const std::string& name) {
            auto start = std::chrono::high_resolution_clock::now();
            
            double sum = 0.0;
            for (size_t iter = 0; iter < iterations; ++iter) {
                for (size_t j = 0; j < matrix.extent(1); ++j) {
                    for (size_t i = 0; i < matrix.extent(0); ++i) {
                        sum += matrix[i, j];
                    }
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << std::format("{} 按列遍历: {}ms (sum={})\n", 
                                    name, duration.count(), sum);
            return duration;
        };
        
        std::cout << std::format("测试参数: {}×{} 矩阵, {} 次迭代\n\n", rows, cols, iterations);
        
        auto rm_row_time = test_row_traversal(row_major, "行主序");
        auto rm_col_time = test_col_traversal(row_major, "行主序");
        
        std::cout << "\n";
        
        auto cm_row_time = test_row_traversal(col_major, "列主序");
        auto cm_col_time = test_col_traversal(col_major, "列主序");
        
        std::cout << "\n性能分析:\n";
        std::cout << std::format("行主序布局 - 按行/按列时间比: {:.2f}\n", 
                                static_cast<double>(rm_col_time.count()) / rm_row_time.count());
        std::cout << std::format("列主序布局 - 按行/按列时间比: {:.2f}\n",
                                static_cast<double>(cm_row_time.count()) / cm_col_time.count());
        
        std::cout << "\n结论:\n";
        std::cout << "- 选择与访问模式匹配的内存布局至关重要\n";
        std::cout << "- 不匹配的布局会导致显著的性能下降\n";
        std::cout << "- mdspan让布局选择变得灵活和明确\n";
        
        std::cout << "\n";
    }
};

// ===== 主函数 =====
int main() {
    std::cout << "C++23 std::mdspan多维数组深度解析\n";
    std::cout << "==================================\n";
    
    demonstrate_mdspan_basics();
    demonstrate_layout_strategies();
    demonstrate_high_dimensional_tensors();
    demonstrate_matrix_operations();
    demonstrate_custom_accessors();
    PerformanceBenchmark::run_memory_layout_benchmark();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++23 -O2 -Wall 02_mdspan_multidimensional.cpp -o mdspan_demo
./mdspan_demo

注意：std::mdspan需要C++23支持，目前编译器支持情况：
- GCC 14+: 实验性支持，可能需要 -std=c++2b
- Clang 17+: 部分支持  
- MSVC 19.34+: 实验性支持

如果编译器不支持，可以使用参考实现：
- kokkos/mdspan: https://github.com/kokkos/mdspan

关键学习点:
1. mdspan提供了零开销的多维数组抽象，统一了不同容器的多维访问
2. 布局策略控制内存访问模式，对性能有重要影响
3. 子视图和切片操作支持高效的数据分割和处理
4. 自定义访问器可以添加边界检查、日志等额外功能
5. 在科学计算和数值分析中有广泛应用前景

注意事项:
- 选择合适的内存布局对性能至关重要
- mdspan是视图，不拥有数据，需要确保底层数据生命周期
- 多维索引的语法使用逗号而不是多个方括号
- 自定义访问器和布局的实现较为复杂，需要深入理解模板机制
*/