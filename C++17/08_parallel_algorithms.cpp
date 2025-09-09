/**
 * C++17 并行算法性能飞跃深度解析
 * 
 * 核心概念：
 * 1. 执行策略体系 - 串行、并行、向量化的统一接口
 * 2. 并行算法原理 - 任务分解和工作窃取的实现机制
 * 3. SIMD向量化优化 - 单指令多数据的性能提升
 * 4. 内存访问模式 - 缓存友好的并行数据处理
 * 5. 性能测试与调优 - 不同负载下的最优策略选择
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include <functional>

// ===== 1. 执行策略体系演示 =====
// 不同执行策略的性能特征展示
class ExecutionPolicyDemo {
public:
    // 计算密集型操作：复杂数学计算
    static double expensive_computation(int value) {
        double result = value;
        for (int i = 0; i < 1000; ++i) {
            result = std::sin(result) * std::cos(result) + std::sqrt(std::abs(result));
        }
        return result;
    }
    
    // 简单操作：适合向量化
    static int simple_computation(int value) {
        return value * value + value * 2 + 1;
    }
    
    template<typename ExecutionPolicy>
    static auto test_transform(ExecutionPolicy&& policy, 
                              const std::vector<int>& input,
                              const std::string& policy_name) {
        std::vector<double> output(input.size());
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::transform(policy, input.begin(), input.end(), output.begin(), expensive_computation);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << policy_name << " transform: " << duration.count() << "ms\n";
        return std::make_pair(duration.count(), std::move(output));
    }
    
    template<typename ExecutionPolicy>
    static auto test_simple_transform(ExecutionPolicy&& policy,
                                     const std::vector<int>& input,
                                     const std::string& policy_name) {
        std::vector<int> output(input.size());
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::transform(policy, input.begin(), input.end(), output.begin(), simple_computation);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << policy_name << " simple transform: " << duration.count() << "μs\n";
        return std::make_pair(duration.count(), std::move(output));
    }
};

void demonstrate_execution_policies() {
    std::cout << "=== 执行策略体系演示 ===\n";
    
    // 生成测试数据
    std::vector<int> data(100000);
    std::iota(data.begin(), data.end(), 1);
    
    std::cout << "数据大小: " << data.size() << " 元素\n";
    std::cout << "硬件并发线程数: " << std::thread::hardware_concurrency() << "\n\n";
    
    // 计算密集型操作测试
    std::cout << "计算密集型操作性能比较:\n";
    
    auto [seq_time, seq_result] = ExecutionPolicyDemo::test_transform(
        std::execution::seq, data, "串行执行");
    
    auto [par_time, par_result] = ExecutionPolicyDemo::test_transform(
        std::execution::par, data, "并行执行");
    
    auto [par_unseq_time, par_unseq_result] = ExecutionPolicyDemo::test_transform(
        std::execution::par_unseq, data, "并行+向量化");
    
    auto [unseq_time, unseq_result] = ExecutionPolicyDemo::test_transform(
        std::execution::unseq, data, "纯向量化");
    
    std::cout << "并行加速比: " << (seq_time / (double)par_time) << "x\n";
    std::cout << "并行+向量化加速比: " << (seq_time / (double)par_unseq_time) << "x\n\n";
    
    // 简单操作测试（更适合向量化）
    std::cout << "简单操作性能比较:\n";
    
    auto [seq_simple_time, seq_simple_result] = ExecutionPolicyDemo::test_simple_transform(
        std::execution::seq, data, "串行执行");
    
    auto [par_simple_time, par_simple_result] = ExecutionPolicyDemo::test_simple_transform(
        std::execution::par, data, "并行执行");
    
    auto [unseq_simple_time, unseq_simple_result] = ExecutionPolicyDemo::test_simple_transform(
        std::execution::unseq, data, "纯向量化");
    
    std::cout << "向量化加速比: " << (seq_simple_time / (double)unseq_simple_time) << "x\n";
    
    std::cout << "\n";
}

// ===== 2. 并行算法原理演示 =====
// 自定义并行算法实现原理展示
class ParallelAlgorithmPrinciples {
public:
    // 并行归约的分治策略
    template<typename Iterator, typename BinaryOp>
    static auto parallel_reduce_manual(Iterator first, Iterator last, BinaryOp op) {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        
        auto distance = std::distance(first, last);
        if (distance == 0) return ValueType{};
        if (distance == 1) return *first;
        
        // 分治策略：递归分解
        auto mid = first + distance / 2;
        
        // 模拟并行执行（实际实现会使用线程池）
        auto left_future = std::async(std::launch::async, [=]() {
            return parallel_reduce_manual(first, mid, op);
        });
        
        auto right_result = parallel_reduce_manual(mid, last, op);
        auto left_result = left_future.get();
        
        return op(left_result, right_result);
    }
    
    // 并行排序的分治实现概念
    template<typename Iterator, typename Compare>
    static void parallel_sort_manual(Iterator first, Iterator last, Compare comp) {
        auto distance = std::distance(first, last);
        if (distance <= 1000) {  // 小数据集使用串行排序
            std::sort(first, last, comp);
            return;
        }
        
        // 选择枢轴并分区
        auto pivot = *(first + distance / 2);
        auto mid = std::partition(first, last, [=](const auto& value) {
            return comp(value, pivot);
        });
        
        // 并行排序两个分区
        auto left_future = std::async(std::launch::async, [=]() {
            parallel_sort_manual(first, mid, comp);
        });
        
        parallel_sort_manual(mid, last, comp);
        left_future.wait();
    }
};

void demonstrate_parallel_algorithm_principles() {
    std::cout << "=== 并行算法原理演示 ===\n";
    
    // 准备测试数据
    std::vector<int> data(1000000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);
    
    std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
    
    std::cout << "测试数据: " << data.size() << " 个随机整数\n\n";
    
    // 并行求和性能比较
    std::cout << "并行求和性能比较:\n";
    
    // 标准库串行版本
    auto start = std::chrono::high_resolution_clock::now();
    auto sequential_sum = std::accumulate(data.begin(), data.end(), 0LL);
    auto seq_end = std::chrono::high_resolution_clock::now();
    
    // 标准库并行版本
    auto par_start = std::chrono::high_resolution_clock::now();
    auto parallel_sum = std::reduce(std::execution::par, data.begin(), data.end(), 0LL);
    auto par_end = std::chrono::high_resolution_clock::now();
    
    // 手动实现的并行版本
    auto manual_start = std::chrono::high_resolution_clock::now();
    auto manual_sum = ParallelAlgorithmPrinciples::parallel_reduce_manual(
        data.begin(), data.end(), std::plus<long long>{});
    auto manual_end = std::chrono::high_resolution_clock::now();
    
    auto seq_time = std::chrono::duration_cast<std::chrono::milliseconds>(seq_end - start);
    auto par_time = std::chrono::duration_cast<std::chrono::milliseconds>(par_end - par_start);
    auto manual_time = std::chrono::duration_cast<std::chrono::milliseconds>(manual_end - manual_start);
    
    std::cout << "串行求和: " << sequential_sum << " (" << seq_time.count() << "ms)\n";
    std::cout << "并行求和: " << parallel_sum << " (" << par_time.count() << "ms)\n";
    std::cout << "手动并行: " << manual_sum << " (" << manual_time.count() << "ms)\n";
    std::cout << "结果验证: " << (sequential_sum == parallel_sum && parallel_sum == manual_sum ? "通过" : "失败") << "\n";
    std::cout << "并行加速比: " << (seq_time.count() / (double)par_time.count()) << "x\n\n";
    
    // 并行排序演示
    std::cout << "并行排序演示:\n";
    std::vector<int> sort_data = data;  // 拷贝一份用于排序
    
    auto sort_start = std::chrono::high_resolution_clock::now();
    std::sort(std::execution::par, sort_data.begin(), sort_data.end());
    auto sort_end = std::chrono::high_resolution_clock::now();
    
    auto sort_time = std::chrono::duration_cast<std::chrono::milliseconds>(sort_end - sort_start);
    std::cout << "并行排序耗时: " << sort_time.count() << "ms\n";
    std::cout << "排序验证: " << (std::is_sorted(sort_data.begin(), sort_data.end()) ? "成功" : "失败") << "\n";
    
    std::cout << "\n";
}

// ===== 3. SIMD向量化优化演示 =====
// 向量化友好的数据处理
class SIMDOptimizationDemo {
public:
    // 向量化友好的操作
    static std::vector<float> vectorized_math_operations(const std::vector<float>& input) {
        std::vector<float> output(input.size());
        
        // 这种操作容易被向量化
        std::transform(std::execution::unseq, input.begin(), input.end(), output.begin(),
            [](float x) {
                return x * x + 2.0f * x + 1.0f;  // 简单的多项式计算
            });
        
        return output;
    }
    
    // 向量化不友好的操作
    static std::vector<float> non_vectorized_operations(const std::vector<float>& input) {
        std::vector<float> output(input.size());
        
        std::transform(std::execution::seq, input.begin(), input.end(), output.begin(),
            [](float x) {
                // 分支和函数调用不利于向量化
                if (x > 0) {
                    return std::log(x);
                } else {
                    return 0.0f;
                }
            });
        
        return output;
    }
    
    // 内存访问模式优化
    static void demonstrate_memory_access_patterns() {
        const size_t size = 1000000;
        std::vector<int> data(size);
        std::iota(data.begin(), data.end(), 1);
        
        std::cout << "内存访问模式对并行性能的影响:\n";
        
        // 顺序访问（缓存友好）
        auto start = std::chrono::high_resolution_clock::now();
        auto sequential_sum = std::reduce(std::execution::par, data.begin(), data.end(), 0LL);
        auto seq_end = std::chrono::high_resolution_clock::now();
        
        // 随机访问（缓存不友好）
        std::vector<size_t> indices(size);
        std::iota(indices.begin(), indices.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);
        
        auto random_start = std::chrono::high_resolution_clock::now();
        long long random_sum = 0;
        std::for_each(std::execution::par, indices.begin(), indices.end(),
            [&](size_t idx) {
                random_sum += data[idx];  // 随机内存访问
            });
        auto random_end = std::chrono::high_resolution_clock::now();
        
        auto seq_time = std::chrono::duration_cast<std::chrono::microseconds>(seq_end - start);
        auto random_time = std::chrono::duration_cast<std::chrono::microseconds>(random_end - random_start);
        
        std::cout << "顺序访问: " << sequential_sum << " (" << seq_time.count() << "μs)\n";
        std::cout << "随机访问: " << random_sum << " (" << random_time.count() << "μs)\n";
        std::cout << "性能差异: " << (random_time.count() / (double)seq_time.count()) << "x\n";
    }
};

void demonstrate_simd_optimization() {
    std::cout << "=== SIMD向量化优化演示 ===\n";
    
    // 生成测试数据
    std::vector<float> data(1000000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.1f, 100.0f);
    
    std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
    
    std::cout << "测试数据: " << data.size() << " 个浮点数\n\n";
    
    // 向量化友好操作测试
    std::cout << "向量化友好操作测试:\n";
    
    auto vec_start = std::chrono::high_resolution_clock::now();
    auto vectorized_result = SIMDOptimizationDemo::vectorized_math_operations(data);
    auto vec_end = std::chrono::high_resolution_clock::now();
    
    auto vec_time = std::chrono::duration_cast<std::chrono::microseconds>(vec_end - vec_start);
    std::cout << "向量化操作耗时: " << vec_time.count() << "μs\n";
    
    // 非向量化操作测试
    std::cout << "非向量化操作测试:\n";
    
    auto non_vec_start = std::chrono::high_resolution_clock::now();
    auto non_vectorized_result = SIMDOptimizationDemo::non_vectorized_operations(data);
    auto non_vec_end = std::chrono::high_resolution_clock::now();
    
    auto non_vec_time = std::chrono::duration_cast<std::chrono::microseconds>(non_vec_end - non_vec_start);
    std::cout << "非向量化操作耗时: " << non_vec_time.count() << "μs\n";
    
    std::cout << "向量化性能提升: " << (non_vec_time.count() / (double)vec_time.count()) << "x\n\n";
    
    // 内存访问模式演示
    SIMDOptimizationDemo::demonstrate_memory_access_patterns();
    
    std::cout << "\n";
}

// ===== 4. 内存访问模式优化演示 =====
class MemoryAccessOptimization {
public:
    // 矩阵乘法：演示缓存友好的访问模式
    struct Matrix {
        std::vector<std::vector<double>> data;
        size_t rows, cols;
        
        Matrix(size_t r, size_t c) : rows(r), cols(c) {
            data.resize(rows);
            for (auto& row : data) {
                row.resize(cols);
            }
        }
        
        void fill_random() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dis(0.0, 1.0);
            
            for (auto& row : data) {
                for (auto& val : row) {
                    val = dis(gen);
                }
            }
        }
    };
    
    // 传统矩阵乘法（缓存不友好）
    static Matrix naive_matrix_multiply(const Matrix& a, const Matrix& b) {
        Matrix result(a.rows, b.cols);
        
        for (size_t i = 0; i < a.rows; ++i) {
            for (size_t j = 0; j < b.cols; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < a.cols; ++k) {
                    sum += a.data[i][k] * b.data[k][j];  // b的列访问不连续
                }
                result.data[i][j] = sum;
            }
        }
        
        return result;
    }
    
    // 并行矩阵乘法
    static Matrix parallel_matrix_multiply(const Matrix& a, const Matrix& b) {
        Matrix result(a.rows, b.cols);
        
        // 并行化外层循环
        std::vector<size_t> row_indices(a.rows);
        std::iota(row_indices.begin(), row_indices.end(), 0);
        
        std::for_each(std::execution::par, row_indices.begin(), row_indices.end(),
            [&](size_t i) {
                for (size_t j = 0; j < b.cols; ++j) {
                    double sum = 0.0;
                    for (size_t k = 0; k < a.cols; ++k) {
                        sum += a.data[i][k] * b.data[k][j];
                    }
                    result.data[i][j] = sum;
                }
            });
        
        return result;
    }
};

void demonstrate_memory_access_patterns() {
    std::cout << "=== 内存访问模式优化演示 ===\n";
    
    const size_t matrix_size = 500;
    
    MemoryAccessOptimization::Matrix a(matrix_size, matrix_size);
    MemoryAccessOptimization::Matrix b(matrix_size, matrix_size);
    
    a.fill_random();
    b.fill_random();
    
    std::cout << "矩阵大小: " << matrix_size << "x" << matrix_size << "\n\n";
    
    // 串行矩阵乘法
    std::cout << "串行矩阵乘法:\n";
    auto serial_start = std::chrono::high_resolution_clock::now();
    auto serial_result = MemoryAccessOptimization::naive_matrix_multiply(a, b);
    auto serial_end = std::chrono::high_resolution_clock::now();
    
    auto serial_time = std::chrono::duration_cast<std::chrono::milliseconds>(serial_end - serial_start);
    std::cout << "串行乘法耗时: " << serial_time.count() << "ms\n";
    
    // 并行矩阵乘法
    std::cout << "并行矩阵乘法:\n";
    auto parallel_start = std::chrono::high_resolution_clock::now();
    auto parallel_result = MemoryAccessOptimization::parallel_matrix_multiply(a, b);
    auto parallel_end = std::chrono::high_resolution_clock::now();
    
    auto parallel_time = std::chrono::duration_cast<std::chrono::milliseconds>(parallel_end - parallel_start);
    std::cout << "并行乘法耗时: " << parallel_time.count() << "ms\n";
    std::cout << "并行加速比: " << (serial_time.count() / (double)parallel_time.count()) << "x\n";
    
    // 验证结果正确性（检查部分元素）
    bool results_match = true;
    for (size_t i = 0; i < std::min(size_t{10}, matrix_size); ++i) {
        for (size_t j = 0; j < std::min(size_t{10}, matrix_size); ++j) {
            if (std::abs(serial_result.data[i][j] - parallel_result.data[i][j]) > 1e-10) {
                results_match = false;
                break;
            }
        }
        if (!results_match) break;
    }
    
    std::cout << "结果验证: " << (results_match ? "通过" : "失败") << "\n";
    
    std::cout << "\n";
}

// ===== 5. 性能测试与调优演示 =====
class PerformanceTuningDemo {
public:
    // 不同负载类型的性能测试
    enum class WorkloadType {
        CPU_INTENSIVE,      // CPU密集型
        MEMORY_INTENSIVE,   // 内存密集型
        BALANCED           // 平衡型
    };
    
    static double cpu_intensive_work(int iterations) {
        double result = 1.0;
        for (int i = 0; i < iterations; ++i) {
            result = std::sin(result) + std::cos(result);
        }
        return result;
    }
    
    static int memory_intensive_work(const std::vector<int>& data, int offset) {
        int sum = 0;
        for (size_t i = 0; i < data.size(); i += offset) {
            sum += data[i];
        }
        return sum;
    }
    
    template<typename ExecutionPolicy>
    static auto benchmark_workload(ExecutionPolicy&& policy,
                                  WorkloadType workload,
                                  const std::string& policy_name) {
        const size_t data_size = 100000;
        std::vector<int> data(data_size);
        std::iota(data.begin(), data.end(), 1);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        switch (workload) {
            case WorkloadType::CPU_INTENSIVE: {
                std::vector<double> results(data.size());
                std::transform(policy, data.begin(), data.end(), results.begin(),
                    [](int x) { return cpu_intensive_work(1000); });
                break;
            }
            case WorkloadType::MEMORY_INTENSIVE: {
                std::vector<int> results(data.size());
                std::transform(policy, data.begin(), data.end(), results.begin(),
                    [&data](int x) { return memory_intensive_work(data, x % 100 + 1); });
                break;
            }
            case WorkloadType::BALANCED: {
                std::vector<double> results(data.size());
                std::transform(policy, data.begin(), data.end(), results.begin(),
                    [&data](int x) {
                        double cpu_result = cpu_intensive_work(100);
                        int mem_result = memory_intensive_work(data, x % 10 + 1);
                        return cpu_result + mem_result;
                    });
                break;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << policy_name << ": " << duration.count() << "ms\n";
        return duration.count();
    }
    
    static void compare_execution_policies(WorkloadType workload, const std::string& workload_name) {
        std::cout << workload_name << " 负载性能比较:\n";
        
        auto seq_time = benchmark_workload(std::execution::seq, workload, "串行");
        auto par_time = benchmark_workload(std::execution::par, workload, "并行");
        auto unseq_time = benchmark_workload(std::execution::unseq, workload, "向量化");
        auto par_unseq_time = benchmark_workload(std::execution::par_unseq, workload, "并行+向量化");
        
        std::cout << "并行加速比: " << (seq_time / (double)par_time) << "x\n";
        std::cout << "向量化加速比: " << (seq_time / (double)unseq_time) << "x\n";
        std::cout << "并行+向量化加速比: " << (seq_time / (double)par_unseq_time) << "x\n\n";
    }
};

void demonstrate_performance_tuning() {
    std::cout << "=== 性能测试与调优演示 ===\n";
    
    std::cout << "硬件信息:\n";
    std::cout << "CPU核心数: " << std::thread::hardware_concurrency() << "\n\n";
    
    // 测试不同类型的工作负载
    PerformanceTuningDemo::compare_execution_policies(
        PerformanceTuningDemo::WorkloadType::CPU_INTENSIVE, "CPU密集型");
    
    PerformanceTuningDemo::compare_execution_policies(
        PerformanceTuningDemo::WorkloadType::MEMORY_INTENSIVE, "内存密集型");
    
    PerformanceTuningDemo::compare_execution_policies(
        PerformanceTuningDemo::WorkloadType::BALANCED, "平衡型");
    
    // 推荐的使用指导
    std::cout << "执行策略选择指导:\n";
    std::cout << "1. CPU密集型任务：优先使用 std::execution::par\n";
    std::cout << "2. 简单数学运算：优先使用 std::execution::unseq 或 par_unseq\n";
    std::cout << "3. 内存密集型任务：根据数据访问模式选择合适的策略\n";
    std::cout << "4. 小数据集：可能串行执行更快，避免线程开销\n";
    std::cout << "5. 复杂控制流：避免使用向量化策略\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 并行算法性能飞跃深度解析\n";
    std::cout << "=================================\n";
    
    demonstrate_execution_policies();
    demonstrate_parallel_algorithm_principles();
    demonstrate_simd_optimization();
    demonstrate_memory_access_patterns();
    demonstrate_performance_tuning();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall -ltbb 08_parallel_algorithms.cpp -o parallel_algorithms
./parallel_algorithms

注意：某些系统可能需要链接Intel TBB库（-ltbb）以支持并行算法

关键学习点:
1. C++17提供了四种执行策略：seq、par、unseq、par_unseq
2. 并行算法通过任务分解和工作窃取实现高效的多核利用
3. SIMD向量化对简单数学运算有显著性能提升
4. 内存访问模式对并行性能有重大影响
5. 不同类型的工作负载需要选择合适的执行策略

注意事项:
- 并行算法需要编译器和标准库的支持
- 小数据集可能不适合并行处理，存在线程开销
- 向量化要求操作简单且无分支
- 要注意数据竞争和内存一致性问题
*/