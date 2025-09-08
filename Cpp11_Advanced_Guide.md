# C++11 深度语法手册

## 核心设计哲学

C++11是C++历史上最重要的版本，引入了现代C++的核心概念。其设计哲学围绕三个核心：
1. **零开销抽象**：新特性不应引入运行时开销
2. **类型安全**：编译期检查代替运行期检查
3. **表达能力**：让程序员更好地表达意图

## 1. 右值引用与移动语义

### 设计动机
传统C++中的拷贝语义存在性能瓶颈，特别是对于临时对象和大型容器。移动语义通过"窃取"资源而非拷贝来解决这个问题。

### 深层机制

```cpp
class Resource {
    int* data;
    size_t size;
public:
    // 移动构造函数
    Resource(Resource&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;  // 窃取资源
        other.size = 0;
    }
    
    // 移动赋值运算符
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data;      // 释放当前资源
            data = other.data;  // 窃取资源
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};
```

### 值类别系统重构
C++11重新定义了值类别：
- **lvalue**: 有身份，不可移动
- **xvalue**: 有身份，可移动 (eXpiring value)
- **prvalue**: 无身份，可移动 (pure rvalue)

```cpp
// 完美转发的实现原理
template<typename T>
void forward_func(T&& param) {
    target_func(std::forward<T>(param));
}
```

### 引用折叠规则
```cpp
T& + & = T&
T& + && = T&
T&& + & = T&
T&& + && = T&&
```

## 2. Lambda表达式的深层实现

### 闭包类生成机制
编译器为每个lambda生成唯一的闭包类：

```cpp
auto lambda = [x, &y](int z) mutable -> int {
    return x + y + z;
};

// 编译器生成的等价类
class __lambda_unique {
    int x;      // 按值捕获
    int& y;     // 按引用捕获
public:
    __lambda_unique(int x_, int& y_) : x(x_), y(y_) {}
    int operator()(int z) { return x + y + z; }
};
```

### 捕获机制的性能考量
```cpp
// 广义捕获 (C++14预告)
auto func = [ptr = std::move(unique_ptr)](){ 
    return ptr.get(); 
};

// 按值捕获的优化
auto lambda = [=](){ return expensive_object.process(); };
// 编译器可能优化为只捕获需要的成员
```

## 3. 智能指针的RAII进化

### unique_ptr的实现细节
```cpp
template<typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
    T* ptr;
    Deleter deleter;
public:
    // 移动语义是其核心
    unique_ptr(unique_ptr&& other) noexcept 
        : ptr(other.release()), deleter(std::move(other.deleter)) {}
    
    // 禁止拷贝，体现独占语义
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
};
```

### shared_ptr的引用计数机制
```cpp
// 控制块设计
struct control_block {
    std::atomic<size_t> ref_count;
    std::atomic<size_t> weak_count;
    virtual void dispose() = 0;  // 销毁对象
    virtual void destroy() = 0;  // 销毁控制块
};
```

### weak_ptr解决循环引用
weak_ptr不影响引用计数，通过`expired()`和`lock()`安全访问对象。

## 4. 变长模板参数的元编程革命

### 参数包展开机制
```cpp
template<typename... Args>
void print(Args... args) {
    // C++11方式：递归展开
    print_impl(args...);
}

template<typename First, typename... Rest>
void print_impl(First&& first, Rest&&... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {  // C++17特性预告
        print_impl(rest...);
    }
}
```

### 折叠表达式的前身
```cpp
// 参数包在表达式中的展开
template<typename... Args>
auto sum(Args... args) -> decltype((args + ...)) {  // C++17语法预告
    // C++11实现方式
    return sum_impl(args...);
}
```

## 5. constexpr的编译期计算

### 常量表达式的扩展
```cpp
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 编译期计算，生成常量
constexpr int result = factorial(5);  // 在编译期得到120
```

### 字面值类型要求
constexpr函数的参数和返回值必须是字面值类型：
- 算术类型
- 引用类型  
- 字面值类的类型

## 6. decltype的类型推导

### 与auto的区别
```cpp
int x = 42;
auto a = x;        // int
decltype(x) b = x; // int
decltype((x)) c = x; // int&，注意表达式的引用性

// 尾置返回类型的应用
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}
```

## 7. 线程库的设计哲学

### std::thread的RAII设计
```cpp
class thread {
    std::thread::id id;
    native_handle_type handle;
public:
    ~thread() {
        if (joinable()) {
            std::terminate();  // 防止资源泄露
        }
    }
};
```

### 原子操作的内存模型
```cpp
std::atomic<int> counter{0};

// 不同内存序的性能权衡
counter.fetch_add(1, std::memory_order_relaxed);  // 最宽松
counter.fetch_add(1, std::memory_order_acq_rel);  // 获取-释放
counter.fetch_add(1, std::memory_order_seq_cst);  // 顺序一致（默认）
```

## 8. 初始化列表的统一初始化

### 大括号初始化的优先级
```cpp
std::vector<int> v1{10, 20};     // initializer_list构造
std::vector<int> v2(10, 20);     // 普通构造函数

// 窄化转换的预防
int x = 3.14;    // 允许
int y{3.14};     // 编译错误，防止窄化
```

### initializer_list的实现
```cpp
template<typename T>
class initializer_list {
    const T* begin_;
    size_t size_;
    
    // 编译器生成，指向静态存储区
    constexpr initializer_list(const T* b, size_t s) 
        : begin_(b), size_(s) {}
public:
    constexpr const T* begin() const noexcept { return begin_; }
    constexpr const T* end() const noexcept { return begin_ + size_; }
    constexpr size_t size() const noexcept { return size_; }
};
```

## 9. 类型特征与SFINAE增强

### enable_if的典型应用
```cpp
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
process_integral(T value) {
    return value * 2;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type  
process_integral(T value) {
    return value * 2.0;
}
```

## 10. 枚举类的类型安全

### 强类型枚举的优势
```cpp
enum class Color : uint8_t { Red, Green, Blue };
enum class State : uint8_t { Red, Active, Inactive };  // Red不冲突

Color c = Color::Red;    // 必须指定作用域
// int x = Color::Red;   // 编译错误，不允许隐式转换
int x = static_cast<int>(Color::Red);  // 显式转换
```

## 设计思想总结

C++11的设计体现了几个重要思想：
1. **移动语义**：从根本上改变了C++的性能模型
2. **类型推导**：减少冗余，增强代码表达力
3. **编译期计算**：将运行期工作转移到编译期
4. **RAII强化**：智能指针等工具让资源管理更安全
5. **并发原语**：为多线程编程提供标准化工具
6. **函数式特性**：lambda表达式引入函数式编程范式

这些特性共同构建了现代C++的基础，为后续版本的演进奠定了坚实基础。