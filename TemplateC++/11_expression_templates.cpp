/**
 * C++98/11/14/17/20 表达式模板深度解析
 * 
 * 核心概念：
 * 1. 表达式模板基础原理 - 延迟计算与表达式树构建
 * 2. 数值计算优化技术 - 消除临时对象与循环融合
 * 3. 操作符重载策略 - 构建复杂表达式的语法糖
 * 4. 模板元编程应用 - 编译期表达式分析与优化
 * 5. 现代C++表达式模板 - 完美转发与可变模板
 */

#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>
#include <type_traits>
#include <cassert>

// ===== 1. 表达式模板基础原理演示 =====
void demonstrate_expression_template_basics() {
    std::cout << "=== 表达式模板基础原理演示 ===\n";
    
    // 传统方法的问题：大量临时对象
    std::cout << "1. 传统向量运算的问题:\n";
    
    class SimpleVector {
    private:
        std::vector<double> data_;
        
    public:
        explicit SimpleVector(size_t size) : data_(size) {}
        
        SimpleVector(std::initializer_list<double> list) : data_(list) {}
        
        // 传统加法运算符（产生临时对象）
        SimpleVector operator+(const SimpleVector& other) const {
            std::cout << "创建临时SimpleVector对象\n";
            SimpleVector result(data_.size());
            for (size_t i = 0; i < data_.size(); ++i) {
                result.data_[i] = data_[i] + other.data_[i];
            }
            return result;
        }
        
        // 传统乘法运算符
        SimpleVector operator*(double scalar) const {
            std::cout << "创建临时SimpleVector对象（标量乘法）\n";
            SimpleVector result(data_.size());
            for (size_t i = 0; i < data_.size(); ++i) {
                result.data_[i] = data_[i] * scalar;
            }
            return result;
        }
        
        double& operator[](size_t i) { return data_[i]; }
        const double& operator[](size_t i) const { return data_[i]; }
        
        size_t size() const { return data_.size(); }
        
        void print() const {
            std::cout << "[";
            for (size_t i = 0; i < data_.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << data_[i];
            }
            std::cout << "]";
        }
    };
    
    SimpleVector a{1.0, 2.0, 3.0};
    SimpleVector b{4.0, 5.0, 6.0};
    
    std::cout << "计算: a + b * 2.0 + a\n";
    auto result1 = a + b * 2.0 + a;  // 创建多个临时对象
    std::cout << "结果: ";
    result1.print();
    std::cout << "\n\n";
    
    // 表达式模板方法：延迟计算
    std::cout << "2. 表达式模板方法:\n";
    
    // 前向声明
    template<typename T>
    class ExprVector;
    
    // 表达式基类
    template<typename E>
    class VectorExpression {
    public:
        double operator[](size_t i) const {
            return static_cast<const E&>(*this)[i];
        }
        
        size_t size() const {
            return static_cast<const E&>(*this).size();
        }
        
        // CRTP转换操作符
        operator E&() { return static_cast<E&>(*this); }
        operator const E&() const { return static_cast<const E&>(*this); }
    };
    
    // 具体向量类
    template<typename T>
    class ExprVector : public VectorExpression<ExprVector<T>> {
    private:
        std::vector<T> data_;
        
    public:
        explicit ExprVector(size_t size) : data_(size) {}
        
        ExprVector(std::initializer_list<T> list) : data_(list) {}
        
        // 从表达式构造（这里发生实际计算）
        template<typename E>
        ExprVector(const VectorExpression<E>& expr) : data_(expr.size()) {
            std::cout << "从表达式构造ExprVector（实际计算发生）\n";
            for (size_t i = 0; i < expr.size(); ++i) {
                data_[i] = expr[i];
            }
        }
        
        // 从表达式赋值
        template<typename E>
        ExprVector& operator=(const VectorExpression<E>& expr) {
            std::cout << "从表达式赋值（实际计算发生）\n";
            for (size_t i = 0; i < expr.size(); ++i) {
                data_[i] = expr[i];
            }
            return *this;
        }
        
        T& operator[](size_t i) { return data_[i]; }
        const T& operator[](size_t i) const { return data_[i]; }
        
        size_t size() const { return data_.size(); }
        
        void print() const {
            std::cout << "[";
            for (size_t i = 0; i < data_.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << data_[i];
            }
            std::cout << "]";
        }
    };
    
    ExprVector<double> c{1.0, 2.0, 3.0};
    ExprVector<double> d{4.0, 5.0, 6.0};
    
    std::cout << "构建表达式树（不进行计算）: c + d * 2.0 + c\n";
    // 这里只构建表达式树，不进行实际计算
    // auto expr = c + d * 2.0 + c;  // 需要实现操作符
    
    std::cout << "表达式模板避免了多个临时对象的创建\n\n";
}

// ===== 2. 数值计算优化技术演示 =====
void demonstrate_numerical_optimization() {
    std::cout << "=== 数值计算优化技术演示 ===\n";
    
    // 向量加法表达式
    template<typename L, typename R>
    class VectorAdd : public VectorExpression<VectorAdd<L, R>> {
    private:
        const L& left_;
        const R& right_;
        
    public:
        VectorAdd(const L& left, const R& right) : left_(left), right_(right) {
            std::cout << "构造加法表达式节点\n";
        }
        
        double operator[](size_t i) const {
            return left_[i] + right_[i];
        }
        
        size_t size() const {
            return left_.size();
        }
    };
    
    // 向量标量乘法表达式
    template<typename V>
    class VectorScalarMul : public VectorExpression<VectorScalarMul<V>> {
    private:
        const V& vector_;
        double scalar_;
        
    public:
        VectorScalarMul(const V& vector, double scalar) 
            : vector_(vector), scalar_(scalar) {
            std::cout << "构造标量乘法表达式节点\n";
        }
        
        double operator[](size_t i) const {
            return vector_[i] * scalar_;
        }
        
        size_t size() const {
            return vector_.size();
        }
    };
    
    // 向量减法表达式
    template<typename L, typename R>
    class VectorSub : public VectorExpression<VectorSub<L, R>> {
    private:
        const L& left_;
        const R& right_;
        
    public:
        VectorSub(const L& left, const R& right) : left_(left), right_(right) {
            std::cout << "构造减法表达式节点\n";
        }
        
        double operator[](size_t i) const {
            return left_[i] - right_[i];
        }
        
        size_t size() const {
            return left_.size();
        }
    };
    
    // 重新定义ExprVector以支持新的表达式类型
    template<typename T>
    class OptimizedVector : public VectorExpression<OptimizedVector<T>> {
    private:
        std::vector<T> data_;
        
    public:
        explicit OptimizedVector(size_t size) : data_(size) {}
        
        OptimizedVector(std::initializer_list<T> list) : data_(list) {}
        
        // 从表达式构造
        template<typename E>
        OptimizedVector(const VectorExpression<E>& expr) : data_(expr.size()) {
            std::cout << "优化计算：单次循环完成复杂表达式\n";
            for (size_t i = 0; i < expr.size(); ++i) {
                data_[i] = expr[i];  // 递归计算，编译器内联优化
            }
        }
        
        T& operator[](size_t i) { return data_[i]; }
        const T& operator[](size_t i) const { return data_[i]; }
        
        size_t size() const { return data_.size(); }
        
        void print() const {
            std::cout << "[";
            for (size_t i = 0; i < data_.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << data_[i];
            }
            std::cout << "]";
        }
    };
    
    // 操作符重载
    template<typename L, typename R>
    auto operator+(const VectorExpression<L>& left, const VectorExpression<R>& right) {
        return VectorAdd<L, R>(left, right);
    }
    
    template<typename V>
    auto operator*(const VectorExpression<V>& vector, double scalar) {
        return VectorScalarMul<V>(vector, scalar);
    }
    
    template<typename V>
    auto operator*(double scalar, const VectorExpression<V>& vector) {
        return VectorScalarMul<V>(vector, scalar);
    }
    
    template<typename L, typename R>
    auto operator-(const VectorExpression<L>& left, const VectorExpression<R>& right) {
        return VectorSub<L, R>(left, right);
    }
    
    // 测试优化效果
    OptimizedVector<double> u{1.0, 2.0, 3.0, 4.0};
    OptimizedVector<double> v{5.0, 6.0, 7.0, 8.0};
    OptimizedVector<double> w{9.0, 10.0, 11.0, 12.0};
    
    std::cout << "构建复杂表达式: (u + v * 2.0) - w * 0.5\n";
    auto complex_expr = (u + v * 2.0) - w * 0.5;
    
    std::cout << "实际计算（单次循环完成所有操作）:\n";
    OptimizedVector<double> result(complex_expr);
    
    std::cout << "结果: ";
    result.print();
    std::cout << "\n\n";
}

// ===== 3. 操作符重载策略演示 =====
void demonstrate_operator_overloading() {
    std::cout << "=== 操作符重载策略演示 ===\n";
    
    // 矩阵表达式模板
    template<typename E>
    class MatrixExpression {
    public:
        double operator()(size_t i, size_t j) const {
            return static_cast<const E&>(*this)(i, j);
        }
        
        size_t rows() const {
            return static_cast<const E&>(*this).rows();
        }
        
        size_t cols() const {
            return static_cast<const E&>(*this).cols();
        }
    };
    
    // 矩阵类
    class Matrix : public MatrixExpression<Matrix> {
    private:
        std::vector<std::vector<double>> data_;
        size_t rows_, cols_;
        
    public:
        Matrix(size_t rows, size_t cols) : rows_(rows), cols_(cols) {
            data_.resize(rows);
            for (auto& row : data_) {
                row.resize(cols);
            }
        }
        
        Matrix(std::initializer_list<std::initializer_list<double>> list) {
            rows_ = list.size();
            cols_ = list.begin()->size();
            data_.reserve(rows_);
            
            for (const auto& row : list) {
                data_.emplace_back(row);
            }
        }
        
        template<typename E>
        Matrix(const MatrixExpression<E>& expr) 
            : rows_(expr.rows()), cols_(expr.cols()) {
            data_.resize(rows_);
            for (auto& row : data_) {
                row.resize(cols_);
            }
            
            std::cout << "矩阵表达式计算 (" << rows_ << "x" << cols_ << ")\n";
            for (size_t i = 0; i < rows_; ++i) {
                for (size_t j = 0; j < cols_; ++j) {
                    data_[i][j] = expr(i, j);
                }
            }
        }
        
        double& operator()(size_t i, size_t j) { return data_[i][j]; }
        const double& operator()(size_t i, size_t j) const { return data_[i][j]; }
        
        size_t rows() const { return rows_; }
        size_t cols() const { return cols_; }
        
        void print() const {
            std::cout << "Matrix(" << rows_ << "x" << cols_ << "):\n";
            for (size_t i = 0; i < rows_; ++i) {
                std::cout << "[";
                for (size_t j = 0; j < cols_; ++j) {
                    if (j > 0) std::cout << " ";
                    std::cout << std::setw(6) << data_[i][j];
                }
                std::cout << "]\n";
            }
        }
    };
    
    // 矩阵加法表达式
    template<typename L, typename R>
    class MatrixAdd : public MatrixExpression<MatrixAdd<L, R>> {
    private:
        const L& left_;
        const R& right_;
        
    public:
        MatrixAdd(const L& left, const R& right) : left_(left), right_(right) {}
        
        double operator()(size_t i, size_t j) const {
            return left_(i, j) + right_(i, j);
        }
        
        size_t rows() const { return left_.rows(); }
        size_t cols() const { return left_.cols(); }
    };
    
    // 矩阵标量乘法表达式
    template<typename M>
    class MatrixScalarMul : public MatrixExpression<MatrixScalarMul<M>> {
    private:
        const M& matrix_;
        double scalar_;
        
    public:
        MatrixScalarMul(const M& matrix, double scalar) 
            : matrix_(matrix), scalar_(scalar) {}
        
        double operator()(size_t i, size_t j) const {
            return matrix_(i, j) * scalar_;
        }
        
        size_t rows() const { return matrix_.rows(); }
        size_t cols() const { return matrix_.cols(); }
    };
    
    // 矩阵转置表达式
    template<typename M>
    class MatrixTranspose : public MatrixExpression<MatrixTranspose<M>> {
    private:
        const M& matrix_;
        
    public:
        explicit MatrixTranspose(const M& matrix) : matrix_(matrix) {}
        
        double operator()(size_t i, size_t j) const {
            return matrix_(j, i);  // 转置：交换行列索引
        }
        
        size_t rows() const { return matrix_.cols(); }
        size_t cols() const { return matrix_.rows(); }
    };
    
    // 操作符重载定义
    template<typename L, typename R>
    auto operator+(const MatrixExpression<L>& left, const MatrixExpression<R>& right) {
        return MatrixAdd<L, R>(left, right);
    }
    
    template<typename M>
    auto operator*(const MatrixExpression<M>& matrix, double scalar) {
        return MatrixScalarMul<M>(matrix, scalar);
    }
    
    template<typename M>
    auto operator*(double scalar, const MatrixExpression<M>& matrix) {
        return MatrixScalarMul<M>(matrix, scalar);
    }
    
    template<typename M>
    auto transpose(const MatrixExpression<M>& matrix) {
        return MatrixTranspose<M>(matrix);
    }
    
    // 测试复杂矩阵表达式
    Matrix A{{1, 2, 3}, {4, 5, 6}};
    Matrix B{{7, 8, 9}, {10, 11, 12}};
    
    std::cout << "矩阵A:\n";
    A.print();
    
    std::cout << "矩阵B:\n";
    B.print();
    
    std::cout << "构建复杂表达式: (A + B * 2.0) + transpose(A)\n";
    auto complex_matrix_expr = (A + B * 2.0) + transpose(A);
    
    std::cout << "计算结果:\n";
    Matrix result_matrix(complex_matrix_expr);
    result_matrix.print();
    
    std::cout << "\n";
}

// ===== 4. 模板元编程应用演示 =====
void demonstrate_metaprogramming_optimization() {
    std::cout << "=== 模板元编程应用演示 ===\n";
    
    // 编译期表达式分析
    template<typename E>
    struct ExpressionTraits {
        static constexpr bool is_vector = false;
        static constexpr bool is_scalar = false;
        static constexpr bool is_binary_op = false;
        static constexpr bool is_unary_op = false;
    };
    
    // 向量类型特化
    template<typename T>
    struct ExpressionTraits<OptimizedVector<T>> {
        static constexpr bool is_vector = true;
        static constexpr bool is_scalar = false;
        static constexpr bool is_binary_op = false;
        static constexpr bool is_unary_op = false;
    };
    
    // 二元操作特化
    template<typename L, typename R>
    struct ExpressionTraits<VectorAdd<L, R>> {
        static constexpr bool is_vector = true;
        static constexpr bool is_scalar = false;
        static constexpr bool is_binary_op = true;
        static constexpr bool is_unary_op = false;
    };
    
    // 表达式复杂度计算
    template<typename E>
    constexpr int expression_complexity() {
        if constexpr (ExpressionTraits<E>::is_vector && !ExpressionTraits<E>::is_binary_op) {
            return 1;  // 基础向量
        } else if constexpr (ExpressionTraits<E>::is_binary_op) {
            return 1 + expression_complexity<typename E::left_type>() + 
                       expression_complexity<typename E::right_type>();
        } else {
            return 1;
        }
    }
    
    // 智能优化的向量类
    template<typename T>
    class SmartVector : public VectorExpression<SmartVector<T>> {
    private:
        std::vector<T> data_;
        
    public:
        explicit SmartVector(size_t size) : data_(size) {}
        
        SmartVector(std::initializer_list<T> list) : data_(list) {}
        
        // 根据表达式复杂度选择不同优化策略
        template<typename E>
        SmartVector(const VectorExpression<E>& expr) : data_(expr.size()) {
            constexpr int complexity = 1;  // 简化处理
            
            if constexpr (complexity < 3) {
                std::cout << "简单表达式：直接计算\n";
                for (size_t i = 0; i < expr.size(); ++i) {
                    data_[i] = expr[i];
                }
            } else if constexpr (complexity < 6) {
                std::cout << "中等复杂表达式：分块计算\n";
                // 分块处理大向量
                const size_t block_size = 1024;
                for (size_t block = 0; block < (expr.size() + block_size - 1) / block_size; ++block) {
                    size_t start = block * block_size;
                    size_t end = std::min(start + block_size, expr.size());
                    for (size_t i = start; i < end; ++i) {
                        data_[i] = expr[i];
                    }
                }
            } else {
                std::cout << "复杂表达式：并行计算\n";
                // 在实际实现中，这里会使用并行算法
                for (size_t i = 0; i < expr.size(); ++i) {
                    data_[i] = expr[i];
                }
            }
        }
        
        T& operator[](size_t i) { return data_[i]; }
        const T& operator[](size_t i) const { return data_[i]; }
        
        size_t size() const { return data_.size(); }
        
        void print() const {
            std::cout << "[";
            for (size_t i = 0; i < std::min(data_.size(), size_t(10)); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << data_[i];
            }
            if (data_.size() > 10) std::cout << ", ...";
            std::cout << "]";
        }
    };
    
    // SIMD优化提示
    template<typename T>
    class SIMDVector : public VectorExpression<SIMDVector<T>> {
    private:
        alignas(32) std::vector<T> data_;  // AVX对齐
        
    public:
        explicit SIMDVector(size_t size) : data_(size) {}
        
        template<typename E>
        SIMDVector(const VectorExpression<E>& expr) : data_(expr.size()) {
            std::cout << "SIMD优化计算（模拟）\n";
            
            // 在实际实现中，这里会使用SIMD指令
            for (size_t i = 0; i < expr.size(); ++i) {
                data_[i] = expr[i];
            }
        }
        
        T& operator[](size_t i) { return data_[i]; }
        const T& operator[](size_t i) const { return data_[i]; }
        
        size_t size() const { return data_.size(); }
        
        void print() const {
            std::cout << "[SIMD-";
            for (size_t i = 0; i < std::min(data_.size(), size_t(5)); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << data_[i];
            }
            if (data_.size() > 5) std::cout << ", ...";
            std::cout << "]";
        }
    };
    
    // 测试不同优化策略
    SmartVector<double> x{1, 2, 3, 4, 5};
    SmartVector<double> y{6, 7, 8, 9, 10};
    
    std::cout << "智能优化向量计算:\n";
    SmartVector<double> smart_result = x + y * 2.0;
    std::cout << "结果: ";
    smart_result.print();
    std::cout << "\n\n";
    
    std::cout << "SIMD优化向量计算:\n";
    SIMDVector<double> simd_x{1, 2, 3, 4, 5, 6, 7, 8};
    SIMDVector<double> simd_y{8, 7, 6, 5, 4, 3, 2, 1};
    // SIMDVector<double> simd_result = simd_x + simd_y;  // 需要实现操作符
    std::cout << "SIMD计算模拟完成\n\n";
}

// ===== 5. 现代C++表达式模板演示 =====
void demonstrate_modern_expression_templates() {
    std::cout << "=== 现代C++表达式模板演示 ===\n";
    
    // C++17 auto模板参数
    template<auto Value>
    struct ConstantExpression : public VectorExpression<ConstantExpression<Value>> {
        double operator[](size_t) const { return static_cast<double>(Value); }
        size_t size() const { return std::numeric_limits<size_t>::max(); }  // 无限长度
    };
    
    // 可变参数模板表达式
    template<typename... Expressions>
    class MultiExpression : public VectorExpression<MultiExpression<Expressions...>> {
    private:
        std::tuple<const Expressions&...> expressions_;
        
        template<size_t I = 0>
        double compute_at_index(size_t i) const {
            if constexpr (I < sizeof...(Expressions)) {
                const auto& expr = std::get<I>(expressions_);
                if constexpr (I == 0) {
                    return expr[i];
                } else {
                    return expr[i] + compute_at_index<I + 1>(i);
                }
            } else {
                return 0.0;
            }
        }
        
    public:
        explicit MultiExpression(const Expressions&... exprs) : expressions_(exprs...) {}
        
        double operator[](size_t i) const {
            return compute_at_index(i);
        }
        
        size_t size() const {
            return std::get<0>(expressions_).size();
        }
    };
    
    // 工厂函数
    template<typename... Expressions>
    auto make_multi_expression(const Expressions&... exprs) {
        return MultiExpression<Expressions...>(exprs...);
    }
    
    // Lambda表达式支持
    template<typename Func>
    class LambdaExpression : public VectorExpression<LambdaExpression<Func>> {
    private:
        Func func_;
        size_t size_;
        
    public:
        LambdaExpression(Func func, size_t size) : func_(func), size_(size) {}
        
        double operator[](size_t i) const {
            return func_(i);
        }
        
        size_t size() const { return size_; }
    };
    
    template<typename Func>
    auto make_lambda_expression(Func func, size_t size) {
        return LambdaExpression<Func>(func, size);
    }
    
    // 测试现代表达式模板
    OptimizedVector<double> modern_a{1, 2, 3, 4, 5};
    OptimizedVector<double> modern_b{6, 7, 8, 9, 10};
    
    std::cout << "C++17常量表达式:\n";
    // auto const_expr = ConstantExpression<42>{};
    // std::cout << "常量值: " << const_expr[0] << "\n\n";
    
    std::cout << "Lambda表达式向量:\n";
    auto lambda_expr = make_lambda_expression([](size_t i) -> double {
        return static_cast<double>(i * i);  // 平方序列
    }, 5);
    
    std::cout << "Lambda序列: [";
    for (size_t i = 0; i < lambda_expr.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << lambda_expr[i];
    }
    std::cout << "]\n\n";
    
    std::cout << "可变参数多表达式:\n";
    auto multi_expr = make_multi_expression(modern_a, modern_b);
    OptimizedVector<double> multi_result(multi_expr);
    std::cout << "多表达式结果: ";
    multi_result.print();
    std::cout << "\n\n";
}

// ===== 性能基准测试 =====
void benchmark_expression_templates() {
    std::cout << "=== 表达式模板性能基准测试 ===\n";
    
    const size_t vector_size = 1000000;
    const int iterations = 100;
    
    // 生成测试数据
    std::vector<double> data_a(vector_size), data_b(vector_size), data_c(vector_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 100.0);
    
    for (size_t i = 0; i < vector_size; ++i) {
        data_a[i] = dist(gen);
        data_b[i] = dist(gen);
        data_c[i] = dist(gen);
    }
    
    // 传统方法基准测试
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<double> temp1(vector_size), temp2(vector_size), result(vector_size);
        
        // 传统方法：多个循环，多个临时对象
        for (size_t i = 0; i < vector_size; ++i) {
            temp1[i] = data_b[i] * 2.0;
        }
        
        for (size_t i = 0; i < vector_size; ++i) {
            temp2[i] = data_a[i] + temp1[i];
        }
        
        for (size_t i = 0; i < vector_size; ++i) {
            result[i] = temp2[i] - data_c[i];
        }
    }
    
    auto traditional_time = std::chrono::high_resolution_clock::now() - start;
    
    // 表达式模板方法基准测试
    OptimizedVector<double> expr_a(data_a.begin(), data_a.end());
    OptimizedVector<double> expr_b(data_b.begin(), data_b.end());
    OptimizedVector<double> expr_c(data_c.begin(), data_c.end());
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < iterations; ++iter) {
        // 表达式模板：单个循环，无临时对象
        OptimizedVector<double> result = (expr_a + expr_b * 2.0) - expr_c;
        (void)result;  // 防止优化掉
    }
    
    auto expression_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "性能对比结果:\n";
    std::cout << "向量大小: " << vector_size << "\n";
    std::cout << "迭代次数: " << iterations << "\n";
    std::cout << "传统方法耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(traditional_time).count() 
              << " 毫秒\n";
    std::cout << "表达式模板耗时: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(expression_time).count() 
              << " 毫秒\n";
    
    double speedup = static_cast<double>(traditional_time.count()) / expression_time.count();
    std::cout << "性能提升: " << speedup << "x\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++98/11/14/17/20 表达式模板深度解析\n";
    std::cout << "====================================\n";
    
    demonstrate_expression_template_basics();
    demonstrate_numerical_optimization();
    demonstrate_operator_overloading();
    demonstrate_metaprogramming_optimization();
    demonstrate_modern_expression_templates();
    benchmark_expression_templates();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 11_expression_templates.cpp -o expression_templates
./expression_templates

关键学习点:
1. 表达式模板通过延迟计算避免临时对象，实现高效数值运算
2. CRTP和操作符重载是实现表达式模板的核心技术
3. 模板元编程可以实现编译期表达式优化和自动向量化
4. 现代C++特性（auto、可变参数模板）简化表达式模板实现
5. 表达式模板在数值计算库中有广泛应用（Eigen、Blaze等）

注意事项:
- 表达式模板会显著增加编译时间和二进制大小
- 需要careful设计以避免悬挂引用问题
- 调试复杂表达式模板代码较困难
- 过度使用可能降低代码可读性
*/