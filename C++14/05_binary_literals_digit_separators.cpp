/*
 * C++14 二进制字面值和数字分隔符 - 深入解析
 * 
 * C++14引入了两个重要的字面值改进：
 * 1. 二进制字面值 (0b/0B前缀) - 直接表示二进制数值
 * 2. 数字分隔符 (') - 提高大数字的可读性
 * 
 * 这些看似简单的语法糖实际上解决了系统编程中的重要问题：
 * - 位操作和掩码定义的直观性
 * - 大型数值常量的可读性和维护性
 * - 硬件寄存器配置的准确性
 * - 二进制协议和文件格式处理
 */

#include <iostream>
#include <iomanip>
#include <bitset>
#include <cstdint>
#include <string>
#include <type_traits>
#include <chrono>

namespace cpp14_literals {

// ===== 二进制字面值基础应用 =====

// 位掩码和标志位定义
namespace BitFlags {
    constexpr uint32_t READ_PERMISSION    = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    constexpr uint32_t WRITE_PERMISSION   = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    constexpr uint32_t EXECUTE_PERMISSION = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    constexpr uint32_t DELETE_PERMISSION  = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    
    // 组合权限
    constexpr uint32_t READ_WRITE = READ_PERMISSION | WRITE_PERMISSION;
    constexpr uint32_t FULL_ACCESS = 0b0000'0000'0000'0000'0000'0000'0000'1111;
    
    // 系统级标志位（更复杂的位模式）
    constexpr uint32_t CACHE_ENABLED     = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    constexpr uint32_t BUFFER_ENABLED    = 0b0000'0000'0000'0000'0000'0000'0010'0000;
    constexpr uint32_t COMPRESS_ENABLED  = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    constexpr uint32_t ENCRYPT_ENABLED   = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    
    // 状态掩码
    constexpr uint32_t PERMISSION_MASK = 0b0000'0000'0000'0000'0000'0000'0000'1111;
    constexpr uint32_t FEATURE_MASK    = 0b0000'0000'0000'0000'0000'0000'1111'0000;
    constexpr uint32_t RESERVED_MASK   = 0b1111'1111'1111'1111'1111'1111'0000'0000;
}

// 权限检查器类
class PermissionChecker {
private:
    uint32_t flags_;

public:
    explicit constexpr PermissionChecker(uint32_t flags = 0) : flags_(flags) {}
    
    constexpr bool has_permission(uint32_t permission) const {
        return (flags_ & permission) == permission;
    }
    
    constexpr void grant_permission(uint32_t permission) {
        flags_ |= permission;
    }
    
    constexpr void revoke_permission(uint32_t permission) {
        flags_ &= ~permission;
    }
    
    constexpr uint32_t get_permissions() const {
        return flags_ & BitFlags::PERMISSION_MASK;
    }
    
    constexpr uint32_t get_features() const {
        return (flags_ & BitFlags::FEATURE_MASK) >> 4;
    }
    
    // 位操作辅助函数
    constexpr int count_set_bits() const {
        uint32_t n = flags_;
        int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    }
    
    constexpr bool is_power_of_two() const {
        return flags_ != 0 && (flags_ & (flags_ - 1)) == 0;
    }
};

// ===== 数字分隔符在大数值中的应用 =====

namespace LargeNumbers {
    // 科学计算常量
    constexpr double AVOGADRO_NUMBER = 6.022'140'76e23;
    constexpr double SPEED_OF_LIGHT = 299'792'458.0;  // m/s
    constexpr double PLANCK_CONSTANT = 6.626'070'15e-34;  // J⋅s
    
    // 计算机科学中的大数值
    constexpr uint64_t BYTES_PER_TERABYTE = 1'099'511'627'776ULL;
    constexpr uint64_t NANOSECONDS_PER_YEAR = 31'556'952'000'000'000ULL;
    constexpr uint32_t MAX_32BIT_SIGNED = 2'147'483'647;
    constexpr uint64_t MAX_64BIT_UNSIGNED = 18'446'744'073'709'551'615ULL;
    
    // 金融和商业数值
    constexpr int64_t MILLION = 1'000'000;
    constexpr int64_t BILLION = 1'000'000'000;
    constexpr int64_t TRILLION = 1'000'000'000'000;
    
    // 二进制存储单位
    constexpr uint64_t KILOBYTE = 1'024;
    constexpr uint64_t MEGABYTE = 1'048'576;  // 1024^2
    constexpr uint64_t GIGABYTE = 1'073'741'824;  // 1024^3
    constexpr uint64_t TERABYTE = 1'099'511'627'776;  // 1024^4
}

// ===== 硬件寄存器配置应用 =====

// 模拟ARM处理器控制寄存器
struct ARMControlRegister {
    union {
        uint32_t value;
        struct {
            uint32_t mmu_enable        : 1;  // bit 0
            uint32_t alignment_check   : 1;  // bit 1
            uint32_t dcache_enable     : 1;  // bit 2
            uint32_t write_buffer      : 1;  // bit 3
            uint32_t exception_endian  : 1;  // bit 7
            uint32_t system_protect    : 1;  // bit 8
            uint32_t rom_protect       : 1;  // bit 9
            uint32_t icache_enable     : 1;  // bit 12
            uint32_t high_vectors      : 1;  // bit 13
            uint32_t round_robin       : 1;  // bit 14
            uint32_t reserved          : 17; // 其余位
        } bits;
    };
    
    // 预定义配置
    static constexpr uint32_t DEFAULT_CONFIG = 
        0b0000'0000'0000'0000'0000'0000'0000'0000;
    
    static constexpr uint32_t PERFORMANCE_CONFIG = 
        0b0000'0000'0000'0001'0001'0000'0000'1101;  // MMU + DCCache + ICache + 高性能选项
    
    static constexpr uint32_t SECURE_CONFIG = 
        0b0000'0000'0000'0000'0011'0000'0000'0001;  // 仅MMU + 安全保护
    
    constexpr ARMControlRegister(uint32_t val = DEFAULT_CONFIG) : value(val) {}
    
    constexpr bool is_mmu_enabled() const { return bits.mmu_enable; }
    constexpr bool is_cache_enabled() const { return bits.dcache_enable && bits.icache_enable; }
    
    constexpr void enable_performance_features() {
        value |= PERFORMANCE_CONFIG;
    }
    
    constexpr void enable_security_features() {
        value |= SECURE_CONFIG;
    }
};

// ===== 网络协议和二进制格式处理 =====

// IPv4地址处理
class IPv4Address {
private:
    uint32_t address_;
    
public:
    constexpr IPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
        : address_((static_cast<uint32_t>(a) << 24) |
                   (static_cast<uint32_t>(b) << 16) |
                   (static_cast<uint32_t>(c) << 8) |
                    static_cast<uint32_t>(d)) {}
    
    constexpr explicit IPv4Address(uint32_t addr) : address_(addr) {}
    
    // 常见网络地址（使用二进制字面值表示）
    static constexpr IPv4Address LOCALHOST{0b0111'1111'0000'0000'0000'0000'0000'0001};  // 127.0.0.1
    static constexpr IPv4Address BROADCAST{0b1111'1111'1111'1111'1111'1111'1111'1111}; // 255.255.255.255
    static constexpr IPv4Address PRIVATE_A{0b0000'1010'0000'0000'0000'0000'0000'0001};  // 10.0.0.1
    static constexpr IPv4Address PRIVATE_B{0b1010'1100'0001'0000'0000'0000'0000'0001}; // 172.16.0.1
    static constexpr IPv4Address PRIVATE_C{0b1100'0000'1010'1000'0000'0000'0000'0001}; // 192.168.0.1
    
    constexpr uint8_t get_octet(int index) const {
        return static_cast<uint8_t>((address_ >> (24 - 8 * index)) & 0xFF);
    }
    
    constexpr uint32_t get_raw() const { return address_; }
    
    constexpr bool is_private() const {
        // 10.0.0.0/8
        if ((address_ & 0xFF00'0000) == 0x0A00'0000) return true;
        // 172.16.0.0/12
        if ((address_ & 0xFFF0'0000) == 0xAC10'0000) return true;
        // 192.168.0.0/16
        if ((address_ & 0xFFFF'0000) == 0xC0A8'0000) return true;
        return false;
    }
    
    constexpr bool is_multicast() const {
        // 224.0.0.0/4
        return (address_ & 0xF000'0000) == 0xE000'0000;
    }
};

// 网络数据包头部
struct EthernetHeader {
    static constexpr uint16_t TYPE_IPV4 = 0x0800;
    static constexpr uint16_t TYPE_IPV6 = 0x86DD;
    static constexpr uint16_t TYPE_ARP  = 0x0806;
    
    // 使用二进制字面值定义特殊MAC地址
    static constexpr uint64_t BROADCAST_MAC = 0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111ULL;
    static constexpr uint64_t NULL_MAC      = 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000ULL;
    
    uint8_t destination[6];
    uint8_t source[6];
    uint16_t ethertype;
    
    constexpr bool is_broadcast() const {
        uint64_t dest_mac = 0;
        for (int i = 0; i < 6; ++i) {
            dest_mac |= static_cast<uint64_t>(destination[i]) << (8 * (5 - i));
        }
        return dest_mac == BROADCAST_MAC;
    }
};

// ===== 位操作算法和优化 =====

// 高效位操作函数集合
namespace BitOperations {
    // 快速计算整数的二进制表示中1的个数（Population Count）
    constexpr int popcount_manual(uint64_t x) {
        // Brian Kernighan算法
        int count = 0;
        while (x) {
            x &= x - 1;  // 清除最低位的1
            ++count;
        }
        return count;
    }
    
    // 使用位操作实现的快速幂运算
    constexpr uint64_t fast_power(uint64_t base, uint64_t exp) {
        uint64_t result = 1;
        while (exp > 0) {
            if (exp & 1) {
                result *= base;
            }
            base *= base;
            exp >>= 1;
        }
        return result;
    }
    
    // 位反转函数
    constexpr uint32_t reverse_bits(uint32_t x) {
        uint32_t result = 0;
        for (int i = 0; i < 32; ++i) {
            if (x & (1U << i)) {
                result |= 1U << (31 - i);
            }
        }
        return result;
    }
    
    // 找出最高位的1的位置
    constexpr int find_msb(uint64_t x) {
        if (x == 0) return -1;
        int position = 0;
        while (x > 1) {
            x >>= 1;
            ++position;
        }
        return position;
    }
    
    // 检查是否为2的幂
    constexpr bool is_power_of_two(uint64_t x) {
        return x != 0 && (x & (x - 1)) == 0;
    }
    
    // 向上舍入到最近的2的幂
    constexpr uint64_t next_power_of_two(uint64_t x) {
        if (x <= 1) return 1;
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        return x + 1;
    }
}

// ===== 编译时位模式匹配和验证 =====

template<uint64_t Pattern, uint64_t Mask = ~0ULL>
struct BitPatternMatcher {
    static constexpr uint64_t pattern = Pattern;
    static constexpr uint64_t mask = Mask;
    
    static constexpr bool matches(uint64_t value) {
        return (value & mask) == (pattern & mask);
    }
    
    static constexpr uint64_t extract(uint64_t value) {
        return value & mask;
    }
    
    static constexpr uint64_t apply(uint64_t value) {
        return (value & ~mask) | (pattern & mask);
    }
};

// 指令格式匹配器（模拟RISC-V指令）
using RTypeInstr = BitPatternMatcher<0b0000000'00000'00000'000'00000'0110011, 0b1111111'00000'00000'111'00000'1111111>;
using ITypeInstr = BitPatternMatcher<0b000000000000'00000'000'00000'0010011, 0b000000000000'00000'111'00000'1111111>;
using LoadInstr  = BitPatternMatcher<0b000000000000'00000'000'00000'0000011, 0b000000000000'00000'111'00000'1111111>;

// ===== 性能基准测试 =====

void benchmark_literal_formats() {
    std::cout << "\n===== 字面值格式性能对比 =====\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 测试不同字面值格式的编译时常量
    constexpr int iterations = 1000000;
    
    // 二进制字面值测试
    volatile uint32_t binary_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        binary_sum += 0b1010'1010'1010'1010'1010'1010'1010'1010;
    }
    
    auto mid1 = std::chrono::high_resolution_clock::now();
    
    // 十六进制字面值测试
    volatile uint32_t hex_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        hex_sum += 0xAAAA'AAAA;
    }
    
    auto mid2 = std::chrono::high_resolution_clock::now();
    
    // 十进制字面值测试
    volatile uint32_t decimal_sum = 0;
    for (int i = 0; i < iterations; ++i) {
        decimal_sum += 2'863'311'530;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto binary_time = std::chrono::duration_cast<std::chrono::nanoseconds>(mid1 - start);
    auto hex_time = std::chrono::duration_cast<std::chrono::nanoseconds>(mid2 - mid1);
    auto decimal_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mid2);
    
    std::cout << "二进制字面值: " << binary_time.count() / iterations << " ns/op\n";
    std::cout << "十六进制字面值: " << hex_time.count() / iterations << " ns/op\n";
    std::cout << "十进制字面值: " << decimal_time.count() / iterations << " ns/op\n";
    
    // 验证结果相等
    std::cout << "结果验证: " << (binary_sum == hex_sum && hex_sum == decimal_sum ? "通过" : "失败") << "\n";
}

void demonstrate_readability_benefits() {
    std::cout << "\n===== 可读性改进示例 =====\n";
    
    // 对比传统写法和C++14新语法
    std::cout << "传统写法 vs C++14新语法:\n";
    
    // 文件权限
    std::cout << "文件权限 (rwxrwxrwx):\n";
    std::cout << "  传统: 0x1FF (" << std::oct << 0777 << std::dec << ")\n";
    std::cout << "  C++14: 0b111'111'111\n";
    
    // 大数值
    std::cout << "大数值:\n";
    std::cout << "  传统: 1000000\n";
    std::cout << "  C++14: 1'000'000\n";
    
    // 位掩码
    std::cout << "32位掩码:\n";
    std::cout << "  传统: 0xAAAAAAAA\n";
    std::cout << "  C++14: 0b1010'1010'1010'1010'1010'1010'1010'1010\n";
    
    // 网络地址
    constexpr auto addr = IPv4Address::PRIVATE_A;
    std::cout << "IP地址 (10.0.0.1):\n";
    std::cout << "  传统: 0x0A000001\n";
    std::cout << "  C++14: 0b0000'1010'0000'0000'0000'0000'0000'0001\n";
    std::cout << "  解析: " << static_cast<int>(addr.get_octet(0)) << "."
              << static_cast<int>(addr.get_octet(1)) << "."
              << static_cast<int>(addr.get_octet(2)) << "."
              << static_cast<int>(addr.get_octet(3)) << "\n";
}

// ===== 实际应用案例分析 =====

// 图像处理中的像素操作
class RGBAPixel {
private:
    uint32_t pixel_;
    
public:
    constexpr RGBAPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : pixel_((static_cast<uint32_t>(a) << 24) |
                 (static_cast<uint32_t>(r) << 16) |
                 (static_cast<uint32_t>(g) << 8) |
                  static_cast<uint32_t>(b)) {}
    
    // 使用二进制字面值定义常用颜色
    static constexpr RGBAPixel RED    {0b1111'1111'0000'0000'0000'0000'1111'1111};
    static constexpr RGBAPixel GREEN  {0b0000'0000'1111'1111'0000'0000'1111'1111};
    static constexpr RGBAPixel BLUE   {0b0000'0000'0000'0000'1111'1111'1111'1111};
    static constexpr RGBAPixel BLACK  {0b0000'0000'0000'0000'0000'0000'1111'1111};
    static constexpr RGBAPixel WHITE  {0b1111'1111'1111'1111'1111'1111'1111'1111};
    static constexpr RGBAPixel TRANSPARENT{0b0000'0000'0000'0000'0000'0000'0000'0000};
    
    constexpr explicit RGBAPixel(uint32_t pixel) : pixel_(pixel) {}
    
    constexpr uint8_t red() const   { return static_cast<uint8_t>((pixel_ >> 16) & 0xFF); }
    constexpr uint8_t green() const { return static_cast<uint8_t>((pixel_ >> 8) & 0xFF); }
    constexpr uint8_t blue() const  { return static_cast<uint8_t>(pixel_ & 0xFF); }
    constexpr uint8_t alpha() const { return static_cast<uint8_t>((pixel_ >> 24) & 0xFF); }
    
    constexpr uint32_t raw() const { return pixel_; }
    
    // 像素混合操作
    constexpr RGBAPixel blend_with(const RGBAPixel& other, uint8_t blend_factor) const {
        uint8_t inv_factor = 255 - blend_factor;
        return RGBAPixel{
            static_cast<uint8_t>((red() * inv_factor + other.red() * blend_factor) / 255),
            static_cast<uint8_t>((green() * inv_factor + other.green() * blend_factor) / 255),
            static_cast<uint8_t>((blue() * inv_factor + other.blue() * blend_factor) / 255),
            static_cast<uint8_t>((alpha() * inv_factor + other.alpha() * blend_factor) / 255)
        };
    }
};

} // namespace cpp14_literals

// ===== 编译器优化和代码生成分析 =====

/*
 * 编译器如何处理C++14字面值：
 * 
 * 1. 词法分析阶段
 *    - 二进制字面值 (0b/0B) 直接转换为整数值
 *    - 数字分隔符在词法分析时被忽略，不影响数值
 *    - 所有字面值格式产生相同的内部表示
 * 
 * 2. 编译时优化
 *    - 常量折叠：所有字面值在编译时就确定值
 *    - 死代码消除：未使用的常量定义被移除
 *    - 内联展开：constexpr函数中的字面值直接内联
 * 
 * 3. 代码生成
 *    - 字面值格式不影响最终机器码
 *    - 优化器可能会选择最高效的指令序列
 *    - 相同数值的不同表示形式产生完全相同的汇编代码
 * 
 * 4. 调试信息
 *    - 调试器通常显示原始字面值格式
 *    - 有助于理解程序员的原始意图
 *    - 支持在调试时切换数值显示格式
 */

int main() {
    std::cout << "=== C++14 二进制字面值和数字分隔符示例 ===\n";
    
    // 基本用法演示
    std::cout << "\n===== 基本语法演示 =====\n";
    constexpr auto binary_val = 0b1010'1100;
    constexpr auto large_num = 1'234'567'890;
    constexpr auto float_num = 3.141'592'653'589'793;
    
    std::cout << "二进制字面值: 0b1010'1100 = " << binary_val << " (十进制)\n";
    std::cout << "大整数: 1'234'567'890 = " << large_num << "\n";
    std::cout << "浮点数: 3.141'592'653'589'793 = " << std::fixed << std::setprecision(15) << float_num << "\n";
    
    // 位操作演示
    std::cout << "\n===== 权限系统演示 =====\n";
    using namespace cpp14_literals;
    
    PermissionChecker checker(BitFlags::READ_WRITE | BitFlags::CACHE_ENABLED);
    std::cout << "权限检查:\n";
    std::cout << "  读权限: " << (checker.has_permission(BitFlags::READ_PERMISSION) ? "有" : "无") << "\n";
    std::cout << "  写权限: " << (checker.has_permission(BitFlags::WRITE_PERMISSION) ? "有" : "无") << "\n";
    std::cout << "  执行权限: " << (checker.has_permission(BitFlags::EXECUTE_PERMISSION) ? "有" : "无") << "\n";
    std::cout << "  设置的位数: " << checker.count_set_bits() << "\n";
    
    // 网络地址演示
    std::cout << "\n===== 网络地址处理 =====\n";
    constexpr auto localhost = IPv4Address::LOCALHOST;
    constexpr auto private_addr = IPv4Address::PRIVATE_C;
    
    std::cout << "localhost是否为私有地址: " << (localhost.is_private() ? "是" : "否") << "\n";
    std::cout << "192.168.0.1是否为私有地址: " << (private_addr.is_private() ? "是" : "否") << "\n";
    
    // 硬件寄存器演示
    std::cout << "\n===== 硬件寄存器配置 =====\n";
    ARMControlRegister ctrl_reg(ARMControlRegister::PERFORMANCE_CONFIG);
    std::cout << "MMU启用: " << (ctrl_reg.is_mmu_enabled() ? "是" : "否") << "\n";
    std::cout << "缓存启用: " << (ctrl_reg.is_cache_enabled() ? "是" : "否") << "\n";
    std::cout << "寄存器值: 0x" << std::hex << ctrl_reg.value << std::dec << "\n";
    
    // 位操作算法演示
    std::cout << "\n===== 位操作算法 =====\n";
    constexpr uint64_t test_num = 0b1100'1010'0101'1001;
    std::cout << "数值: " << std::bitset<16>(test_num) << "\n";
    std::cout << "1的个数: " << BitOperations::popcount_manual(test_num) << "\n";
    std::cout << "是否为2的幂: " << (BitOperations::is_power_of_two(test_num) ? "是" : "否") << "\n";
    std::cout << "最高位位置: " << BitOperations::find_msb(test_num) << "\n";
    std::cout << "下一个2的幂: " << BitOperations::next_power_of_two(test_num) << "\n";
    
    // 像素处理演示
    std::cout << "\n===== 像素处理演示 =====\n";
    constexpr auto red_pixel = RGBAPixel::RED;
    constexpr auto blue_pixel = RGBAPixel::BLUE;
    constexpr auto blended = red_pixel.blend_with(blue_pixel, 128);  // 50%混合
    
    std::cout << "红色像素 RGB: (" << static_cast<int>(red_pixel.red()) << ", "
              << static_cast<int>(red_pixel.green()) << ", " << static_cast<int>(red_pixel.blue()) << ")\n";
    std::cout << "混合结果 RGB: (" << static_cast<int>(blended.red()) << ", "
              << static_cast<int>(blended.green()) << ", " << static_cast<int>(blended.blue()) << ")\n";
    
    cpp14_literals::demonstrate_readability_benefits();
    cpp14_literals::benchmark_literal_formats();
    
    return 0;
}