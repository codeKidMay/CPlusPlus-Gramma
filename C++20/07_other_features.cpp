/**
 * C++20其他重要特性深度解析
 * 
 * 核心概念：
 * 1. std::format - 类型安全的格式化字符串系统，Python风格格式化语法
 * 2. std::span - 非拥有连续内存视图，统一数组访问接口  
 * 3. std::jthread - 可中断线程，自动join的RAII线程管理
 * 4. 指定初始化器 - 结构化初始化语法，提高代码可读性
 * 5. 范围for循环初始化 - 循环作用域变量初始化增强
 */

#include <iostream>
#include <format>        // C++20 格式化库
#include <span>          // C++20 连续内存视图
#include <thread>        // std::jthread
#include <chrono>        // 时间库
#include <vector>
#include <array>
#include <string>
#include <stop_token>    // 线程停止令牌
#include <memory>
#include <type_traits>
#include <algorithm>
#include <numeric>

using namespace std::chrono_literals;

// ===== 1. std::format类型安全格式化系统 =====
void demonstrate_format() {
    std::cout << "=== std::format类型安全格式化系统 ===\n";
    
    // 基本格式化语法
    int value = 42;
    double pi = 3.14159;
    std::string name = "现代C++";
    
    // Python风格的格式化语法
    std::cout << "基本格式化:\n";
    std::cout << std::format("整数: {}, 浮点: {:.2f}, 字符串: {}\n", value, pi, name);
    
    // 位置参数和命名参数风格
    std::cout << "\n位置控制:\n";
    std::cout << std::format("值: {1}, 名称: {0}, 精度: {2:.4f}\n", name, value, pi);
    
    // 数字格式化选项
    std::cout << "\n数字格式化:\n";
    int num = 12345;
    std::cout << std::format("十进制: {:d}\n", num);
    std::cout << std::format("十六进制: {:x}\n", num);
    std::cout << std::format("八进制: {:o}\n", num);
    std::cout << std::format("二进制: {:b}\n", num);
    std::cout << std::format("带分隔符: {:L}\n", num);
    
    // 浮点数格式化
    std::cout << "\n浮点格式化:\n";
    double large_num = 12345.6789;
    std::cout << std::format("默认: {}\n", large_num);
    std::cout << std::format("固定精度: {:.2f}\n", large_num);
    std::cout << std::format("科学记数法: {:.2e}\n", large_num);
    std::cout << std::format("自动选择: {:.2g}\n", large_num);
    
    // 对齐和填充
    std::cout << "\n对齐和填充:\n";
    std::cout << std::format("左对齐: '{:<20}'\n", "Hello");
    std::cout << std::format("右对齐: '{:>20}'\n", "Hello");
    std::cout << std::format("居中: '{:^20}'\n", "Hello");
    std::cout << std::format("填充字符: '{:*^20}'\n", "Hello");
    
    std::cout << "\n";
}

// 自定义类型的format支持
struct Point3D {
    double x, y, z;
};

// 为自定义类型特化std::formatter
template<>
struct std::formatter<Point3D> : std::formatter<std::string> {
    auto format(const Point3D& p, auto& ctx) const {
        return std::formatter<std::string>::format(
            std::format("Point3D({:.2f}, {:.2f}, {:.2f})", p.x, p.y, p.z), ctx);
    }
};

void demonstrate_custom_format() {
    std::cout << "=== 自定义类型格式化 ===\n";
    
    Point3D point{1.23, 4.56, 7.89};
    std::cout << std::format("3D点坐标: {}\n", point);
    
    std::cout << "\n";
}

// ===== 2. std::span非拥有连续内存视图 =====
void demonstrate_span() {
    std::cout << "=== std::span非拥有连续内存视图 ===\n";
    
    // 统一的连续内存访问接口
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::array<int, 5> arr = {6, 7, 8, 9, 10};
    int c_array[] = {11, 12, 13, 14, 15};
    
    // 创建span视图
    std::span<int> vec_span(vec);
    std::span<int> arr_span(arr);  
    std::span<int> c_span(c_array);
    
    std::cout << "统一接口访问不同容器:\n";
    
    // 统一的处理函数
    auto process_data = [](std::span<const int> data, const std::string& name) {
        std::cout << name << ": ";
        for (const auto& item : data) {
            std::cout << item << " ";
        }
        std::cout << "(大小: " << data.size() << ")\n";
    };
    
    process_data(vec_span, "vector");
    process_data(arr_span, "array");
    process_data(c_span, "C数组");
    
    // 子span操作
    std::cout << "\n子span操作:\n";
    std::span<int> sub_span = vec_span.subspan(1, 3);  // 从索引1开始，长度3
    std::cout << "子span: ";
    for (auto item : sub_span) {
        std::cout << item << " ";
    }
    std::cout << "\n";
    
    // 前缀和后缀
    auto first_three = vec_span.first(3);
    auto last_two = vec_span.last(2);
    std::cout << "前3个元素: ";
    for (auto item : first_three) std::cout << item << " ";
    std::cout << "\n";
    std::cout << "后2个元素: ";
    for (auto item : last_two) std::cout << item << " ";
    std::cout << "\n";
    
    std::cout << "\n";
}

// span的高级应用：矩阵视图
class Matrix2D {
private:
    std::vector<double> data_;
    size_t rows_, cols_;

public:
    Matrix2D(size_t rows, size_t cols) : data_(rows * cols), rows_(rows), cols_(cols) {}
    
    // 返回指定行的span视图
    std::span<double> row(size_t r) {
        return std::span<double>(data_.data() + r * cols_, cols_);
    }
    
    std::span<const double> row(size_t r) const {
        return std::span<const double>(data_.data() + r * cols_, cols_);
    }
    
    // 设置矩阵数据
    void set_data(std::initializer_list<std::initializer_list<double>> init) {
        size_t r = 0;
        for (const auto& row_data : init) {
            auto row_span = row(r++);
            std::copy(row_data.begin(), row_data.end(), row_span.begin());
        }
    }
    
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
};

void demonstrate_matrix_span() {
    std::cout << "=== 矩阵span视图应用 ===\n";
    
    Matrix2D matrix(3, 4);
    matrix.set_data({
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0}
    });
    
    std::cout << "矩阵数据:\n";
    for (size_t i = 0; i < matrix.rows(); ++i) {
        auto row_span = matrix.row(i);
        std::cout << "行" << i << ": ";
        for (double val : row_span) {
            std::cout << std::format("{:6.1f}", val);
        }
        std::cout << "\n";
    }
    
    // 对特定行进行操作
    std::cout << "\n对第2行每个元素乘以2:\n";
    auto row1 = matrix.row(1);
    std::transform(row1.begin(), row1.end(), row1.begin(), 
                   [](double x) { return x * 2.0; });
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        auto row_span = matrix.row(i);
        std::cout << "行" << i << ": ";
        for (double val : row_span) {
            std::cout << std::format("{:6.1f}", val);
        }
        std::cout << "\n";
    }
    
    std::cout << "\n";
}

// ===== 3. std::jthread可中断线程管理 =====
void demonstrate_jthread() {
    std::cout << "=== std::jthread可中断线程管理 ===\n";
    
    std::cout << "基本jthread使用:\n";
    
    // 自动join的线程
    {
        std::jthread worker([](std::stop_token stoken) {
            int count = 0;
            while (!stoken.stop_requested() && count < 5) {
                std::cout << std::format("工作线程运行中... {}\n", ++count);
                std::this_thread::sleep_for(200ms);
            }
            std::cout << "工作线程结束\n";
        });
        
        std::this_thread::sleep_for(1s);
        std::cout << "请求停止线程\n";
        worker.request_stop();  // 请求停止
        // 析构时自动join，无需手动调用
    }
    
    std::cout << "\n协作式中断示例:\n";
    
    // 更复杂的中断处理
    std::jthread data_processor([](std::stop_token stoken) {
        std::vector<int> data;
        
        // 数据生成阶段
        for (int i = 0; i < 100 && !stoken.stop_requested(); ++i) {
            data.push_back(i * i);
            if (i % 20 == 0) {
                std::cout << std::format("生成数据... 进度: {}%\n", i);
            }
            std::this_thread::sleep_for(50ms);
        }
        
        if (stoken.stop_requested()) {
            std::cout << "数据生成被中断\n";
            return;
        }
        
        // 数据处理阶段
        std::cout << "开始数据处理...\n";
        int sum = 0;
        for (size_t i = 0; i < data.size() && !stoken.stop_requested(); ++i) {
            sum += data[i];
            if (i % 30 == 0) {
                std::cout << std::format("处理数据... 进度: {}%\n", 
                         static_cast<int>(i * 100 / data.size()));
            }
            std::this_thread::sleep_for(30ms);
        }
        
        if (!stoken.stop_requested()) {
            std::cout << std::format("数据处理完成，总和: {}\n", sum);
        } else {
            std::cout << "数据处理被中断\n";
        }
    });
    
    std::this_thread::sleep_for(2s);
    std::cout << "主线程请求中断...\n";
    data_processor.request_stop();
    
    std::cout << "\n";
}

// stop_token的高级应用
class InterruptibleTask {
private:
    std::jthread worker_;
    std::string task_name_;
    
public:
    InterruptibleTask(const std::string& name) : task_name_(name) {}
    
    void start(std::function<void(std::stop_token)> task) {
        worker_ = std::jthread([this, task](std::stop_token stoken) {
            std::cout << std::format("任务 '{}' 开始执行\n", task_name_);
            
            // 注册停止回调
            std::stop_callback callback(stoken, [this] {
                std::cout << std::format("任务 '{}' 收到停止信号\n", task_name_);
            });
            
            task(stoken);
            
            std::cout << std::format("任务 '{}' 执行完成\n", task_name_);
        });
    }
    
    void stop() {
        if (worker_.joinable()) {
            worker_.request_stop();
        }
    }
    
    bool is_running() const {
        return worker_.joinable();
    }
    
    ~InterruptibleTask() = default;  // jthread自动join
};

void demonstrate_interruptible_task() {
    std::cout << "=== 可中断任务管理 ===\n";
    
    InterruptibleTask task("数据分析");
    
    task.start([](std::stop_token stoken) {
        for (int phase = 1; phase <= 3; ++phase) {
            std::cout << std::format("执行阶段 {}\n", phase);
            
            for (int step = 1; step <= 10 && !stoken.stop_requested(); ++step) {
                std::cout << std::format("  步骤 {}/10\n", step);
                std::this_thread::sleep_for(100ms);
            }
            
            if (stoken.stop_requested()) {
                std::cout << "任务在阶段 " << phase << " 被中断\n";
                return;
            }
        }
    });
    
    std::this_thread::sleep_for(1500ms);
    std::cout << "请求停止任务...\n";
    task.stop();
    
    std::cout << "\n";
}

// ===== 4. 指定初始化器(Designated Initializers) =====
struct Config {
    std::string server_name = "default";
    int port = 8080;
    bool ssl_enabled = false;
    double timeout = 30.0;
    int max_connections = 100;
};

struct DatabaseConfig {
    std::string host;
    int port;
    std::string database;
    std::string username;
    bool use_ssl;
    int connection_pool_size;
};

void demonstrate_designated_initializers() {
    std::cout << "=== 指定初始化器 ===\n";
    
    // C++20指定初始化器语法
    std::cout << "服务器配置:\n";
    Config web_config{
        .server_name = "生产服务器",
        .port = 443,
        .ssl_enabled = true,
        .timeout = 60.0
        // max_connections使用默认值
    };
    
    std::cout << std::format("服务器: {}\n", web_config.server_name);
    std::cout << std::format("端口: {}\n", web_config.port);
    std::cout << std::format("SSL: {}\n", web_config.ssl_enabled ? "启用" : "禁用");
    std::cout << std::format("超时: {:.1f}秒\n", web_config.timeout);
    std::cout << std::format("最大连接: {}\n", web_config.max_connections);
    
    std::cout << "\n数据库配置:\n";
    DatabaseConfig db_config{
        .host = "192.168.1.100",
        .port = 5432,
        .database = "production_db",
        .username = "app_user",
        .use_ssl = true,
        .connection_pool_size = 20
    };
    
    std::cout << std::format("数据库主机: {}:{}\n", db_config.host, db_config.port);
    std::cout << std::format("数据库名: {}\n", db_config.database);
    std::cout << std::format("用户名: {}\n", db_config.username);
    std::cout << std::format("SSL连接: {}\n", db_config.use_ssl ? "是" : "否");
    std::cout << std::format("连接池大小: {}\n", db_config.connection_pool_size);
    
    std::cout << "\n";
}

// ===== 5. 范围for循环初始化增强 =====
void demonstrate_range_for_init() {
    std::cout << "=== 范围for循环初始化增强 ===\n";
    
    // 在循环中初始化变量，限制作用域
    std::cout << "处理多个数据集:\n";
    
    // C++20: 可以在范围for循环中初始化变量
    for (auto dataset = std::vector<int>{1, 2, 3, 4, 5}; 
         const auto& value : dataset) {
        std::cout << "值: " << value << " ";
    }
    std::cout << "\n";
    
    // 复杂的初始化示例
    std::cout << "\n配置驱动的数据处理:\n";
    for (auto config = Config{.server_name = "处理服务器", .port = 9090}; 
         const auto& [name, port] = std::make_pair(config.server_name, config.port); 
         /* 空的范围表达式 */) {
        
        std::cout << std::format("使用配置 - 服务器: {}, 端口: {}\n", name, port);
        break;  // 演示用，实际中可能有其他逻辑
    }
    
    // 实用示例：带锁的范围循环
    std::cout << "\n线程安全的数据访问:\n";
    static std::vector<std::string> shared_data = {"数据1", "数据2", "数据3"};
    static std::mutex data_mutex;
    
    for (std::lock_guard lock(data_mutex); const auto& item : shared_data) {
        std::cout << "安全访问: " << item << "\n";
        // lock在循环结束时自动释放
    }
    
    std::cout << "\n";
}

// ===== 综合示例：现代C++20数据处理管道 =====
class DataPipeline {
private:
    std::jthread worker_;
    std::vector<double> buffer_;
    
public:
    struct PipelineConfig {
        size_t buffer_size = 1000;
        double processing_delay_ms = 10.0;
        bool enable_logging = true;
        std::string pipeline_name = "默认管道";
    };
    
    void start(PipelineConfig config = {}) {
        worker_ = std::jthread([this, config](std::stop_token stoken) {
            buffer_.reserve(config.buffer_size);
            
            if (config.enable_logging) {
                std::cout << std::format("启动数据管道: {}\n", config.pipeline_name);
            }
            
            int batch_count = 0;
            while (!stoken.stop_requested()) {
                // 生成数据批次
                std::vector<double> batch;
                for (size_t i = 0; i < 100 && !stoken.stop_requested(); ++i) {
                    batch.push_back(static_cast<double>(batch_count * 100 + i) * 0.1);
                }
                
                if (stoken.stop_requested()) break;
                
                // 使用span处理数据
                std::span<const double> batch_span(batch);
                double sum = std::accumulate(batch_span.begin(), batch_span.end(), 0.0);
                double avg = sum / batch_span.size();
                
                if (config.enable_logging) {
                    std::cout << std::format("批次 {}: 平均值 = {:.2f}\n", 
                             ++batch_count, avg);
                }
                
                // 存储到缓冲区
                buffer_.insert(buffer_.end(), batch.begin(), batch.end());
                
                // 保持缓冲区大小
                if (buffer_.size() > config.buffer_size) {
                    buffer_.erase(buffer_.begin(), 
                                 buffer_.begin() + (buffer_.size() - config.buffer_size));
                }
                
                std::this_thread::sleep_for(
                    std::chrono::duration<double, std::milli>(config.processing_delay_ms)
                );
            }
            
            if (config.enable_logging) {
                std::cout << std::format("数据管道 '{}' 停止，缓冲区大小: {}\n", 
                         config.pipeline_name, buffer_.size());
            }
        });
    }
    
    void stop() {
        if (worker_.joinable()) {
            worker_.request_stop();
        }
    }
    
    std::span<const double> get_buffer() const {
        return std::span<const double>(buffer_);
    }
};

void demonstrate_comprehensive_example() {
    std::cout << "=== 综合示例：现代C++20数据处理管道 ===\n";
    
    DataPipeline pipeline;
    
    // 使用指定初始化器配置管道
    pipeline.start({
        .buffer_size = 500,
        .processing_delay_ms = 200.0,
        .enable_logging = true,
        .pipeline_name = "实时数据分析管道"
    });
    
    std::this_thread::sleep_for(2s);
    
    // 获取当前缓冲区状态
    auto buffer_span = pipeline.get_buffer();
    if (!buffer_span.empty()) {
        double total = std::accumulate(buffer_span.begin(), buffer_span.end(), 0.0);
        std::cout << std::format("\n当前缓冲区统计:\n");
        std::cout << std::format("  数据点数量: {}\n", buffer_span.size());
        std::cout << std::format("  数据总和: {:.2f}\n", total);
        std::cout << std::format("  平均值: {:.2f}\n", total / buffer_span.size());
        
        // 显示最后几个数据点
        auto last_10 = buffer_span.last(std::min<size_t>(10, buffer_span.size()));
        std::cout << "  最后10个数据点: ";
        for (double val : last_10) {
            std::cout << std::format("{:.1f} ", val);
        }
        std::cout << "\n";
    }
    
    pipeline.stop();
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++20其他重要特性深度解析\n";
    std::cout << "============================\n";
    
    demonstrate_format();
    demonstrate_custom_format();
    demonstrate_span();
    demonstrate_matrix_span();
    demonstrate_jthread();
    demonstrate_interruptible_task();
    demonstrate_designated_initializers();
    demonstrate_range_for_init();
    demonstrate_comprehensive_example();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 07_other_features.cpp -o other_features -pthread
./other_features

关键学习点:
1. std::format提供了类型安全、Python风格的字符串格式化
2. std::span统一了连续内存的访问接口，零拷贝视图
3. std::jthread支持协作式中断和自动资源管理
4. 指定初始化器提高了结构体初始化的可读性
5. 范围for循环初始化增强了变量作用域控制

注意事项:
- std::format需要较新的编译器支持(GCC 13+, Clang 14+, MSVC 19.29+)
- std::span是非拥有视图，需要确保底层数据生命周期
- std::jthread的stop_token是协作式的，需要代码主动检查
- 指定初始化器要求按声明顺序初始化
- 某些特性可能需要特定的编译器版本和标准库实现
*/