/**
 * C++11/14/17/20 模板在标准库中的应用深度解析
 * 
 * 核心概念：
 * 1. STL容器模板设计 - 容器的泛型实现和内存管理策略
 * 2. 算法库模板技术 - 迭代器、函数对象和算法的解耦设计
 * 3. 智能指针模板体系 - RAII和所有权的模板化实现
 * 4. Ranges库设计哲学 - C++20范围和视图的惰性求值机制
 * 5. 元编程工具库 - type_traits和编译期计算的标准库支持
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <map>
#include <ranges>

void demonstrate_stl_container_templates() {
    std::cout << "=== STL容器模板设计深度解析 ===\n";
    
    // 1. 容器模板参数设计
    std::cout << "1. 容器模板参数设计:\n";
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::list<std::string> lst = {"hello", "world", "template"};
    std::map<int, std::string> mp = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    std::cout << "vector: ";
    for (int x : vec) std::cout << x << " ";
    std::cout << "\n";
    
    std::cout << "list: ";
    for (const auto& s : lst) std::cout << s << " ";
    std::cout << "\n";
    
    std::cout << "map: ";
    for (const auto& [k, v] : mp) std::cout << k << ":" << v << " ";
    std::cout << "\n";
    
    // 2. 容器类型特征
    std::cout << "\n2. 容器类型特征:\n";
    std::cout << "vector value_type: " << typeid(std::vector<int>::value_type).name() << "\n";
    std::cout << "vector size_type: " << typeid(std::vector<int>::size_type).name() << "\n";
    std::cout << "vector allocator_type: " << typeid(std::vector<int>::allocator_type).name() << "\n";
    
    std::cout << "\n";
}

void demonstrate_algorithm_template_techniques() {
    std::cout << "=== 算法库模板技术深度解析 ===\n";
    
    // 1. 标准算法使用
    std::cout << "1. 标准算法使用:\n";
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    auto found = std::find(numbers.begin(), numbers.end(), 5);
    if (found != numbers.end()) {
        std::cout << "找到数字5在位置: " << std::distance(numbers.begin(), found) << "\n";
    }
    
    auto even_find = std::find_if(numbers.begin(), numbers.end(), 
                                 [](int x) { return x % 2 == 0; });
    if (even_find != numbers.end()) {
        std::cout << "找到第一个偶数: " << *even_find << "\n";
    }
    
    auto count = std::count_if(numbers.begin(), numbers.end(), 
                               [](int x) { return x > 5; });
    std::cout << "大于5的数字个数: " << count << "\n";
    
    std::sort(numbers.begin(), numbers.end(), std::greater<int>());
    std::cout << "降序排序: ";
    for (int x : numbers) std::cout << x << " ";
    std::cout << "\n";
    
    // 2. 迭代器类别演示
    std::cout << "\n2. 迭代器类别演示:\n";
    std::vector<int>::iterator vec_it = numbers.begin();
    std::advance(vec_it, 3);
    std::cout << "vector迭代器前进3步后指向: " << *vec_it << "\n";
    
    std::list<int> list_numbers = {1, 2, 3, 4, 5};
    std::list<int>::iterator list_it = list_numbers.begin();
    std::advance(list_it, 2);
    std::cout << "list迭代器前进2步后指向: " << *list_it << "\n";
    
    std::cout << "\n";
}

void demonstrate_smart_pointer_templates() {
    std::cout << "=== 智能指针模板体系深度解析 ===\n";
    
    // 1. unique_ptr演示
    std::cout << "1. unique_ptr演示:\n";
    std::unique_ptr<int> ptr1(new int(42));
    std::cout << "unique_ptr值: " << *ptr1 << "\n";
    
    // 移动语义
    std::unique_ptr<int> ptr2 = std::move(ptr1);
    std::cout << "移动后ptr2值: " << *ptr2 << "\n";
    std::cout << "ptr1是否为空: " << (ptr1 ? "否" : "是") << "\n";
    
    // 2. shared_ptr引用计数
    std::cout << "\n2. shared_ptr引用计数:\n";
    std::shared_ptr<int> shared1(new int(100));
    std::shared_ptr<int> shared2 = shared1;
    std::shared_ptr<int> shared3 = shared2;
    
    std::cout << "shared1引用计数: " << shared1.use_count() << "\n";
    std::cout << "shared2引用计数: " << shared2.use_count() << "\n";
    std::cout << "shared3引用计数: " << shared3.use_count() << "\n";
    
    shared2.reset();
    std::cout << "shared2重置后shared1引用计数: " << shared1.use_count() << "\n";
    
    // 3. weak_ptr解决循环引用
    std::cout << "\n3. weak_ptr使用:\n";
    std::weak_ptr<int> weak_ptr = shared1;
    std::cout << "weak_ptr expired: " << weak_ptr.expired() << "\n";
    
    if (auto locked = weak_ptr.lock()) {
        std::cout << "weak_ptr锁定值: " << *locked << "\n";
    }
    
    std::cout << "\n";
}

void demonstrate_ranges_design_philosophy() {
    std::cout << "=== Ranges库设计哲学深度解析 ===\n";
    
    // 1. 基本Range概念
    std::cout << "1. 基本Range概念:\n";
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    std::cout << "原始数据: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n";
    
    // 2. C++20 Ranges使用
    std::cout << "\n2. C++20 Ranges使用:\n";
    
    auto even_numbers = data | std::views::filter([](int x) { return x % 2 == 0; });
    std::cout << "过滤偶数: ";
    for (int x : even_numbers) std::cout << x << " ";
    std::cout << "\n";
    
    auto squared = even_numbers | std::views::transform([](int x) { return x * x; });
    std::cout << "平方变换: ";
    for (int x : squared) std::cout << x << " ";
    std::cout << "\n";
    
    // 3. Ranges组合
    auto combined = data | std::views::filter([](int x) { return x > 3; })
                         | std::views::transform([](int x) { return x * 2; });
    
    std::cout << "组合视图(>3然后*2): ";
    for (int x : combined) std::cout << x << " ";
    std::cout << "\n";
    
    // 4. Ranges算法
    std::cout << "\n4. Ranges算法:\n";
    auto result = std::ranges::find_if(data, [](int x) { return x > 7; });
    if (result != data.end()) {
        std::cout << "找到第一个大于7的数: " << *result << "\n";
    }
    
    auto sorted_view = std::views::reverse(data);
    std::cout << "反转视图: ";
    for (int x : sorted_view) std::cout << x << " ";
    std::cout << "\n";
    
    // 5. 惰性求值演示
    std::cout << "\n5. 惰性求值演示:\n";
    auto infinite = std::views::iota(1) | std::views::take(5);
    std::cout << "无限序列取前5个: ";
    for (int x : infinite) std::cout << x << " ";
    std::cout << "\n";
    
    std::cout << "\n";
}

void demonstrate_metaprogramming_tools() {
    std::cout << "=== 元编程工具库深度解析 ===\n";
    
    // 1. 标准type_traits使用
    std::cout << "1. 标准type_traits使用:\n";
    std::cout << "is_const_v<const int>: " << std::is_const_v<const int> << "\n";
    std::cout << "is_reference_v<int&>: " << std::is_reference_v<int&> << "\n";
    std::cout << "is_pointer_v<int*>: " << std::is_pointer_v<int*> << "\n";
    std::cout << "is_integral_v<int>: " << std::is_integral_v<int> << "\n";
    std::cout << "is_floating_point_v<double>: " << std::is_floating_point_v<double> << "\n";
    
    // 2. 类型转换trait
    std::cout << "\n2. 类型转换trait:\n";
    std::cout << "remove_const_t<const int>: " << typeid(std::remove_const_t<const int>).name() << "\n";
    std::cout << "remove_reference_t<int&>: " << typeid(std::remove_reference_t<int&>).name() << "\n";
    std::cout << "add_pointer_t<int>: " << typeid(std::add_pointer_t<int>).name() << "\n";
    
    // 3. 条件类型选择
    std::cout << "\n3. 条件类型选择:\n";
    using SelectedType = std::conditional_t<true, int, double>;
    std::cout << "conditional_t<true, int, double>: " << typeid(SelectedType).name() << "\n";
    
    using FalseType = std::conditional_t<false, int, double>;
    std::cout << "conditional_t<false, int, double>: " << typeid(FalseType).name() << "\n";
    
    // 4. 编译期常量
    std::cout << "\n4. 编译期常量:\n";
    constexpr int array_size = 10;
    std::array<int, array_size> arr{};
    std::cout << "使用编译期常量的array大小: " << arr.size() << "\n";
    
    // 5. 变参模板和折叠表达式
    std::cout << "\n5. 变参模板和折叠表达式:\n";
    
    auto print_all = [](const auto&... args) {
        ((std::cout << args << " "), ...);
        std::cout << "\n";
    };
    
    auto sum_all = [](const auto&... args) {
        return (args + ...);
    };
    
    auto all_true = [](const auto&... args) {
        return (args && ...);
    };
    
    print_all("Hello", 42, 3.14, "World");
    std::cout << "求和结果: " << sum_all(1, 2, 3, 4, 5) << "\n";
    std::cout << "全部为真: " << all_true(true, true, false, true) << "\n";
    
    // 6. SFINAE演示
    std::cout << "\n6. SFINAE概念演示:\n";
    std::cout << "vector是否有size方法: " 
              << std::is_invocable_v<decltype(&std::vector<int>::size), std::vector<int>> << "\n";
    std::cout << "int是否有size方法: " 
              << std::is_invocable_v<decltype(&std::vector<int>::size), int> << "\n";
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++11/14/17/20 模板在标准库中的应用深度解析\n";
    std::cout << "==========================================\n\n";
    
    demonstrate_stl_container_templates();
    demonstrate_algorithm_template_techniques();
    demonstrate_smart_pointer_templates();
    demonstrate_ranges_design_philosophy();
    demonstrate_metaprogramming_tools();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++20 -O2 -Wall 15_template_standard_library.cpp -o template_stdlib
./template_stdlib

关键学习点:
1. STL容器的模板参数设计和分配器机制
2. 算法库的迭代器概念约束和标签分发优化
3. 智能指针的RAII实现和引用计数机制
4. Ranges库的惰性求值和组合设计哲学
5. 元编程工具库的类型计算和SFINAE检测技术

注意事项:
- 理解容器、算法、迭代器三者之间的解耦设计
- 掌握智能指针的所有权语义和性能权衡
- 学习Ranges库的函数式编程思想
- 熟悉type_traits的元编程应用模式
- 注意C++20 Concepts对模板约束的改进
*/