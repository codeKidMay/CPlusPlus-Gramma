/**
 * C++17 std::string_view零拷贝字符串视图深度解析
 * 
 * 核心概念：
 * 1. 零拷贝字符串操作 - 避免不必要的内存分配和数据复制
 * 2. 统一字符串接口 - 兼容C字符串、std::string和字符数组
 * 3. 内存安全陷阱 - 生命周期管理和悬空指针防范
 * 4. 性能优化原理 - 视图语义和懒惰求值的应用
 * 5. 字符串算法现代化 - 高效的文本处理和解析
 */

#include <iostream>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <cassert>

// ===== 1. 零拷贝字符串操作演示 =====
// 传统方式：频繁拷贝
std::string traditional_substring(const std::string& str, size_t pos, size_t len) {
    return str.substr(pos, len);  // 产生拷贝
}

bool traditional_starts_with(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;  // 产生拷贝
}

// C++17方式：零拷贝
std::string_view modern_substring(std::string_view str, size_t pos, size_t len) {
    return str.substr(pos, len);  // 只是调整指针和长度，无拷贝
}

bool modern_starts_with(std::string_view str, std::string_view prefix) {
    return str.substr(0, prefix.length()) == prefix;  // 无拷贝比较
}

// 字符串处理函数族
class StringProcessor {
public:
    // 统一接口处理各种字符串源
    static size_t count_words(std::string_view text) {
        if (text.empty()) return 0;
        
        size_t count = 0;
        bool in_word = false;
        
        for (char c : text) {
            if (std::isspace(c)) {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                ++count;
            }
        }
        
        return count;
    }
    
    static std::string_view trim_left(std::string_view text) {
        auto start = text.find_first_not_of(" \t\n\r");
        return start == std::string_view::npos ? std::string_view{} : text.substr(start);
    }
    
    static std::string_view trim_right(std::string_view text) {
        auto end = text.find_last_not_of(" \t\n\r");
        return end == std::string_view::npos ? std::string_view{} : text.substr(0, end + 1);
    }
    
    static std::string_view trim(std::string_view text) {
        return trim_left(trim_right(text));
    }
    
    static bool contains(std::string_view text, std::string_view pattern) {
        return text.find(pattern) != std::string_view::npos;
    }
};

void demonstrate_zero_copy_operations() {
    std::cout << "=== 零拷贝字符串操作演示 ===\n";
    
    const char* c_string = "Hello, World! This is a C string.";
    std::string std_string = "Hello, World! This is a std::string.";
    char char_array[] = "Hello, World! This is a char array.";
    
    // string_view可以统一处理各种字符串源
    std::vector<std::string_view> views = {
        c_string,           // 从C字符串构造
        std_string,         // 从std::string构造
        char_array,         // 从char数组构造
        "字面量字符串"       // 从字符串字面量构造
    };
    
    std::cout << "处理不同类型的字符串源:\n";
    for (size_t i = 0; i < views.size(); ++i) {
        std::cout << "源 " << i+1 << ": " << views[i] << "\n";
        std::cout << "  长度: " << views[i].length() << "\n";
        std::cout << "  单词数: " << StringProcessor::count_words(views[i]) << "\n";
        std::cout << "  包含'World': " << StringProcessor::contains(views[i], "World") << "\n";
        
        // 零拷贝子串
        auto sub = modern_substring(views[i], 0, 5);
        std::cout << "  前5个字符: " << sub << "\n";
    }
    
    // 修剪操作演示
    std::string_view padded_text = "   \t  Hello World  \n\r  ";
    std::cout << "原始文本: \"" << padded_text << "\"\n";
    std::cout << "左修剪: \"" << StringProcessor::trim_left(padded_text) << "\"\n";
    std::cout << "右修剪: \"" << StringProcessor::trim_right(padded_text) << "\"\n";
    std::cout << "完全修剪: \"" << StringProcessor::trim(padded_text) << "\"\n";
    
    std::cout << "\n";
}

// ===== 2. 统一字符串接口演示 =====
// 函数重载的复杂性（传统方法）
namespace traditional_overloads {
    void process_text(const char* text) {
        std::cout << "处理C字符串: " << text << "\n";
    }
    
    void process_text(const std::string& text) {
        std::cout << "处理std::string: " << text << "\n";
    }
    
    // 需要为每种字符串类型提供重载...
}

// string_view的统一接口
void modern_process_text(std::string_view text) {
    std::cout << "统一处理: " << text << " (长度: " << text.length() << ")\n";
}

// 高级字符串分析器
class TextAnalyzer {
public:
    struct AnalysisResult {
        size_t char_count;
        size_t word_count;
        size_t line_count;
        size_t digit_count;
        size_t alpha_count;
        std::string_view longest_word;
    };
    
    static AnalysisResult analyze(std::string_view text) {
        AnalysisResult result{};
        result.char_count = text.length();
        result.line_count = 1;
        
        std::string_view current_word;
        std::string_view longest_word;
        bool in_word = false;
        
        for (size_t i = 0; i < text.length(); ++i) {
            char c = text[i];
            
            if (c == '\n') {
                ++result.line_count;
            }
            
            if (std::isdigit(c)) {
                ++result.digit_count;
            } else if (std::isalpha(c)) {
                ++result.alpha_count;
            }
            
            if (std::isspace(c)) {
                if (in_word) {
                    if (current_word.length() > longest_word.length()) {
                        longest_word = current_word;
                    }
                    ++result.word_count;
                    in_word = false;
                }
            } else if (!in_word) {
                in_word = true;
                current_word = text.substr(i);
            }
        }
        
        // 处理最后一个单词
        if (in_word) {
            // 找到当前单词的结尾
            auto end = current_word.find_first_of(" \t\n\r");
            if (end != std::string_view::npos) {
                current_word = current_word.substr(0, end);
            }
            if (current_word.length() > longest_word.length()) {
                longest_word = current_word;
            }
            ++result.word_count;
        }
        
        result.longest_word = longest_word;
        return result;
    }
};

void demonstrate_unified_interface() {
    std::cout << "=== 统一字符串接口演示 ===\n";
    
    // 不同来源的字符串都可以用同一个函数处理
    const char* c_str = "C字符串";
    std::string std_str = "标准字符串";
    std::string_view view_str = "视图字符串";
    
    modern_process_text(c_str);
    modern_process_text(std_str);
    modern_process_text(view_str);
    modern_process_text("字面量");
    
    // 复杂文本分析
    std::string_view sample_text = R"(
这是一个示例文本，用于演示string_view的强大功能。
它包含多行文本，有数字123和456，
以及各种英文单词like performance和optimization。
String_view provides zero-copy string operations.
)";
    
    auto analysis = TextAnalyzer::analyze(sample_text);
    
    std::cout << "文本分析结果:\n";
    std::cout << "  字符数: " << analysis.char_count << "\n";
    std::cout << "  单词数: " << analysis.word_count << "\n";
    std::cout << "  行数: " << analysis.line_count << "\n";
    std::cout << "  数字字符数: " << analysis.digit_count << "\n";
    std::cout << "  字母字符数: " << analysis.alpha_count << "\n";
    std::cout << "  最长单词: \"" << analysis.longest_word << "\"\n";
    
    std::cout << "\n";
}

// ===== 3. 内存安全陷阱演示 =====
// 危险的用法示例
std::string_view dangerous_return_local_string() {
    std::string local = "这是一个局部字符串";
    return local;  // 危险！返回指向即将销毁对象的视图
}  // local在此处销毁，返回的string_view指向无效内存

std::string_view dangerous_return_temp_substr() {
    std::string base = "基础字符串";
    return base.substr(0, 4);  // 危险！substr返回临时string对象
}

// 安全的用法示例
std::string_view safe_return_static_string() {
    static const std::string static_str = "这是静态字符串";
    return static_str;  // 安全：static对象生命周期持续整个程序运行期
}

std::string_view safe_return_parameter(const std::string& param) {
    return param;  // 安全：返回的视图引用调用者拥有的对象
}

class SafeStringViewHolder {
private:
    std::string storage;  // 拥有字符串数据
    std::string_view view; // 指向自己拥有的数据
    
public:
    explicit SafeStringViewHolder(std::string data) 
        : storage(std::move(data)), view(storage) {}
    
    std::string_view get_view() const { return view; }
    
    // 更新数据时同步更新视图
    void update_data(std::string new_data) {
        storage = std::move(new_data);
        view = storage;  // 重新指向新数据
    }
};

void demonstrate_memory_safety_pitfalls() {
    std::cout << "=== 内存安全陷阱演示 ===\n";
    
    // 安全用法示例
    std::cout << "安全用法:\n";
    
    // 1. 使用字面量（生命周期持续整个程序）
    std::string_view literal_view = "字符串字面量";
    std::cout << "字面量视图: " << literal_view << "\n";
    
    // 2. 使用静态字符串
    auto static_view = safe_return_static_string();
    std::cout << "静态字符串视图: " << static_view << "\n";
    
    // 3. 视图引用外部拥有的对象
    std::string owned_string = "外部拥有的字符串";
    auto param_view = safe_return_parameter(owned_string);
    std::cout << "参数视图: " << param_view << "\n";
    
    // 4. 使用安全的持有者类
    SafeStringViewHolder holder{"持有者管理的字符串"};
    std::cout << "安全持有者视图: " << holder.get_view() << "\n";
    
    holder.update_data("更新后的字符串");
    std::cout << "更新后的视图: " << holder.get_view() << "\n";
    
    // 危险用法警告（不执行，仅说明）
    std::cout << "\n危险用法警告:\n";
    std::cout << "1. 不要返回指向局部string对象的string_view\n";
    std::cout << "2. 不要返回临时string对象（如substr结果）的string_view\n";
    std::cout << "3. 确保string_view的生命周期不超过其引用的字符串\n";
    std::cout << "4. 在多线程环境中注意字符串的并发修改\n";
    
    std::cout << "\n";
}

// ===== 4. 性能优化原理演示 =====
// 性能测试类
class PerformanceComparator {
public:
    // 传统方式：频繁拷贝
    static size_t traditional_word_count(const std::vector<std::string>& texts) {
        size_t total = 0;
        for (const auto& text : texts) {
            std::istringstream iss(text);  // 构造流对象
            std::string word;
            while (iss >> word) {  // 每次读取都可能涉及内存分配
                ++total;
            }
        }
        return total;
    }
    
    // 现代方式：零拷贝
    static size_t modern_word_count(const std::vector<std::string_view>& texts) {
        size_t total = 0;
        for (auto text : texts) {
            bool in_word = false;
            for (char c : text) {
                if (std::isspace(c)) {
                    in_word = false;
                } else if (!in_word) {
                    in_word = true;
                    ++total;
                }
            }
        }
        return total;
    }
    
    // 字符串查找性能测试
    static size_t traditional_find_count(const std::vector<std::string>& texts, const std::string& pattern) {
        size_t count = 0;
        for (const auto& text : texts) {
            auto pos = text.find(pattern);
            while (pos != std::string::npos) {
                ++count;
                pos = text.find(pattern, pos + 1);
            }
        }
        return count;
    }
    
    static size_t modern_find_count(const std::vector<std::string_view>& texts, std::string_view pattern) {
        size_t count = 0;
        for (auto text : texts) {
            auto pos = text.find(pattern);
            while (pos != std::string_view::npos) {
                ++count;
                pos = text.find(pattern, pos + 1);
            }
        }
        return count;
    }
    
    // 子串提取性能测试
    static std::vector<std::string> traditional_extract_substrings(const std::vector<std::string>& texts) {
        std::vector<std::string> results;
        for (const auto& text : texts) {
            if (text.length() > 10) {
                results.push_back(text.substr(0, 10));  // 创建新string对象
            }
        }
        return results;
    }
    
    static std::vector<std::string_view> modern_extract_substrings(const std::vector<std::string_view>& texts) {
        std::vector<std::string_view> results;
        for (auto text : texts) {
            if (text.length() > 10) {
                results.push_back(text.substr(0, 10));  // 只是调整指针和长度
            }
        }
        return results;
    }
};

template<typename Func>
auto measure_performance(Func func, const std::string& description) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << description << ": " << duration.count() << " 微秒 (结果: " << result << ")\n";
    
    return std::make_pair(duration.count(), result);
}

void demonstrate_performance_optimization() {
    std::cout << "=== 性能优化原理演示 ===\n";
    
    // 准备测试数据
    std::vector<std::string> string_data;
    std::vector<std::string_view> view_data;
    
    const char* sample_texts[] = {
        "The quick brown fox jumps over the lazy dog",
        "C++17 introduces many powerful features for modern programming",
        "String_view provides zero-copy string operations with great performance",
        "Memory management becomes easier with smart pointers and RAII",
        "Template metaprogramming allows compile-time computations"
    };
    
    // 构造大量测试数据
    for (int i = 0; i < 10000; ++i) {
        for (const char* text : sample_texts) {
            string_data.emplace_back(text);
            view_data.emplace_back(string_data.back());
        }
    }
    
    std::cout << "测试数据量: " << string_data.size() << " 个字符串\n";
    
    // 单词计数性能比较
    std::cout << "\n单词计数性能比较:\n";
    auto [traditional_time1, traditional_count] = measure_performance(
        [&]() { return PerformanceComparator::traditional_word_count(string_data); },
        "传统方式"
    );
    
    auto [modern_time1, modern_count] = measure_performance(
        [&]() { return PerformanceComparator::modern_word_count(view_data); },
        "string_view方式"
    );
    
    assert(traditional_count == modern_count);  // 结果应该相同
    std::cout << "性能提升: " << (traditional_time1 / (double)modern_time1) << "x\n";
    
    // 字符串查找性能比较
    std::cout << "\n字符串查找性能比较 (搜索 'the'):\n";
    auto [traditional_time2, traditional_finds] = measure_performance(
        [&]() { return PerformanceComparator::traditional_find_count(string_data, "the"); },
        "传统方式"
    );
    
    auto [modern_time2, modern_finds] = measure_performance(
        [&]() { return PerformanceComparator::modern_find_count(view_data, "the"); },
        "string_view方式"
    );
    
    assert(traditional_finds == modern_finds);
    std::cout << "性能提升: " << (traditional_time2 / (double)modern_time2) << "x\n";
    
    // 内存使用分析
    std::cout << "\n内存使用分析:\n";
    std::cout << "std::string大小: " << sizeof(std::string) << " 字节\n";
    std::cout << "std::string_view大小: " << sizeof(std::string_view) << " 字节\n";
    std::cout << "存储 " << string_data.size() << " 个字符串:\n";
    std::cout << "  std::string容器内存: ~" << (string_data.size() * sizeof(std::string)) << " 字节\n";
    std::cout << "  string_view容器内存: ~" << (view_data.size() * sizeof(std::string_view)) << " 字节\n";
    
    std::cout << "\n";
}

// ===== 5. 字符串算法现代化演示 =====
// 高效的字符串分割器
class ModernStringSplitter {
public:
    static std::vector<std::string_view> split(std::string_view text, char delimiter) {
        std::vector<std::string_view> tokens;
        size_t start = 0;
        
        for (size_t pos = 0; pos <= text.size(); ++pos) {
            if (pos == text.size() || text[pos] == delimiter) {
                if (pos > start) {
                    tokens.emplace_back(text.substr(start, pos - start));
                }
                start = pos + 1;
            }
        }
        
        return tokens;
    }
    
    static std::vector<std::string_view> split_any(std::string_view text, std::string_view delimiters) {
        std::vector<std::string_view> tokens;
        size_t start = 0;
        
        for (size_t pos = 0; pos <= text.size(); ++pos) {
            if (pos == text.size() || delimiters.find(text[pos]) != std::string_view::npos) {
                if (pos > start) {
                    tokens.emplace_back(text.substr(start, pos - start));
                }
                start = pos + 1;
            }
        }
        
        return tokens;
    }
};

// 高效的文本解析器
class TextParser {
public:
    struct ParsedData {
        std::string_view key;
        std::string_view value;
        bool valid;
    };
    
    static ParsedData parse_key_value(std::string_view line, char separator = '=') {
        auto pos = line.find(separator);
        if (pos == std::string_view::npos) {
            return {{}, {}, false};
        }
        
        auto key = StringProcessor::trim(line.substr(0, pos));
        auto value = StringProcessor::trim(line.substr(pos + 1));
        
        return {key, value, !key.empty()};
    }
    
    static std::vector<ParsedData> parse_config(std::string_view config_text) {
        std::vector<ParsedData> results;
        auto lines = ModernStringSplitter::split(config_text, '\n');
        
        for (auto line : lines) {
            line = StringProcessor::trim(line);
            if (line.empty() || line[0] == '#') {  // 跳过空行和注释
                continue;
            }
            
            auto parsed = parse_key_value(line);
            if (parsed.valid) {
                results.push_back(parsed);
            }
        }
        
        return results;
    }
};

// CSV解析器
class CSVParser {
public:
    static std::vector<std::vector<std::string_view>> parse(std::string_view csv_data) {
        std::vector<std::vector<std::string_view>> rows;
        auto lines = ModernStringSplitter::split(csv_data, '\n');
        
        for (auto line : lines) {
            if (!line.empty()) {
                auto fields = parse_csv_line(line);
                if (!fields.empty()) {
                    rows.push_back(std::move(fields));
                }
            }
        }
        
        return rows;
    }
    
private:
    static std::vector<std::string_view> parse_csv_line(std::string_view line) {
        std::vector<std::string_view> fields;
        size_t start = 0;
        bool in_quotes = false;
        
        for (size_t pos = 0; pos <= line.size(); ++pos) {
            if (pos == line.size()) {
                fields.emplace_back(line.substr(start, pos - start));
                break;
            }
            
            char c = line[pos];
            if (c == '"') {
                in_quotes = !in_quotes;
            } else if (c == ',' && !in_quotes) {
                fields.emplace_back(line.substr(start, pos - start));
                start = pos + 1;
            }
        }
        
        return fields;
    }
};

void demonstrate_modern_string_algorithms() {
    std::cout << "=== 字符串算法现代化演示 ===\n";
    
    // 字符串分割演示
    std::string_view sample_text = "apple,banana,cherry;grape:orange";
    
    std::cout << "单字符分割 (逗号):\n";
    auto comma_tokens = ModernStringSplitter::split(sample_text, ',');
    for (const auto& token : comma_tokens) {
        std::cout << "  \"" << token << "\"\n";
    }
    
    std::cout << "多字符分割 (逗号、分号、冒号):\n";
    auto multi_tokens = ModernStringSplitter::split_any(sample_text, ",;:");
    for (const auto& token : multi_tokens) {
        std::cout << "  \"" << token << "\"\n";
    }
    
    // 配置文件解析演示
    std::string_view config_data = R"(
# 这是配置文件
name = MyApplication
version = 1.0.0
debug = true

# 数据库配置
db.host = localhost
db.port = 3306
db.name = mydb
)";
    
    std::cout << "\n配置文件解析:\n";
    auto config_items = TextParser::parse_config(config_data);
    for (const auto& item : config_items) {
        std::cout << "  " << item.key << " = " << item.value << "\n";
    }
    
    // CSV解析演示
    std::string_view csv_data = R"(Name,Age,City
"John Doe",30,"New York"
Jane Smith,25,London
"Bob Johnson",35,"Los Angeles")";
    
    std::cout << "\nCSV解析:\n";
    auto csv_rows = CSVParser::parse(csv_data);
    for (size_t i = 0; i < csv_rows.size(); ++i) {
        std::cout << "行 " << i+1 << ": ";
        for (size_t j = 0; j < csv_rows[i].size(); ++j) {
            std::cout << "[" << csv_rows[i][j] << "]";
            if (j < csv_rows[i].size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }
    
    std::cout << "\n";
}

// ===== 主函数 =====
int main() {
    std::cout << "C++17 std::string_view零拷贝字符串视图深度解析\n";
    std::cout << "==============================================\n";
    
    demonstrate_zero_copy_operations();
    demonstrate_unified_interface();
    demonstrate_memory_safety_pitfalls();
    demonstrate_performance_optimization();
    demonstrate_modern_string_algorithms();
    
    return 0;
}

/*
编译和运行建议:
g++ -std=c++17 -O2 -Wall 07_string_view.cpp -o string_view
./string_view

关键学习点:
1. string_view提供零拷贝的字符串操作，显著提升性能
2. 统一了C字符串、std::string和字符数组的接口
3. 必须注意生命周期管理，避免悬空指针问题
4. 在字符串处理算法中可以获得显著的性能提升
5. 是现代C++字符串处理的最佳实践基础

注意事项:
- 不要返回指向局部string对象的string_view
- 确保string_view的生命周期不超过其引用的字符串
- 在API设计中优先使用string_view作为只读字符串参数
- 与std::string配合使用时要注意所有权和生命周期问题
*/