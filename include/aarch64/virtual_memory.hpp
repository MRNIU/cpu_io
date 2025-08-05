/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_AARCH64_VIRTUAL_MEMORY_HPP_
#define CPU_IO_INCLUDE_AARCH64_VIRTUAL_MEMORY_HPP_

#include <cstdint>
#include <cstdlib>

#include "regs.hpp"

namespace cpu_io {

namespace virtual_memory {
/// 页表项位偏移定义
static constexpr uint8_t kValidOffset = 0;
static constexpr uint8_t kTableOffset = 1;
static constexpr uint8_t kAttrIndxOffset = 2;
static constexpr uint8_t kNsOffset = 5;
static constexpr uint8_t kApOffset = 6;
static constexpr uint8_t kShOffset = 8;
static constexpr uint8_t kAfOffset = 10;
static constexpr uint8_t kNgOffset = 11;
static constexpr uint8_t kDbmOffset = 51;
static constexpr uint8_t kPxnOffset = 53;
static constexpr uint8_t kUxnOffset = 54;

/// 页表项权限位定义
static constexpr uint64_t kValid = 1ULL << kValidOffset;
static constexpr uint64_t kTable = 1ULL << kTableOffset;
static constexpr uint64_t kAttrIndx = 7ULL << kAttrIndxOffset;
static constexpr uint64_t kNs = 1ULL << kNsOffset;
static constexpr uint64_t kAp = 3ULL << kApOffset;
static constexpr uint64_t kSh = 3ULL << kShOffset;
static constexpr uint64_t kAf = 1ULL << kAfOffset;
static constexpr uint64_t kNg = 1ULL << kNgOffset;
static constexpr uint64_t kDbm = 1ULL << kDbmOffset;
static constexpr uint64_t kPxn = 1ULL << kPxnOffset;
static constexpr uint64_t kUxn = 1ULL << kUxnOffset;

/// 页大小常量 (4KB)
static constexpr size_t kPageSize = 4096;
/// 内核虚拟地址相对物理地址的偏移
static constexpr size_t kKernelOffset = 0x0;

/// 页表项属性位数
static constexpr size_t kPteAttributeBits = 12;
/// 页内偏移位数
static constexpr size_t kPageOffsetBits = 12;
/// 虚拟页号位数
static constexpr size_t kVpnBits = 9;
/// 虚拟页号位掩码
static constexpr size_t kVpnMask = 0x1FF;
/// 页表级数（四级页表）
static constexpr size_t kPageTableLevels = 4;

/// 页表项权限标志
static constexpr uint64_t kPteValid = kValid;
static constexpr uint64_t kPteTable = kTable;
static constexpr uint64_t kPteAttrIndx = kAttrIndx;
static constexpr uint64_t kPteNs = kNs;
static constexpr uint64_t kPteAp = kAp;
static constexpr uint64_t kPteSh = kSh;
static constexpr uint64_t kPteAf = kAf;
static constexpr uint64_t kPteNg = kNg;
static constexpr uint64_t kPteDbm = kDbm;
static constexpr uint64_t kPtePxn = kPxn;
static constexpr uint64_t kPteUxn = kUxn;

/// 访问权限值
/// EL1 读写
static constexpr uint64_t kApReadWrite = 0ULL << kApOffset;
/// EL1 只读
static constexpr uint64_t kApReadOnly = 2ULL << kApOffset;
/// EL0/EL1 读写
static constexpr uint64_t kApUserReadWrite = 1ULL << kApOffset;
/// EL0/EL1 只读
static constexpr uint64_t kApUserReadOnly = 3ULL << kApOffset;

/// 共享性属性值
/// 非共享
static constexpr uint64_t kShNonShareable = 0ULL << kShOffset;
/// 外部共享
static constexpr uint64_t kShOuterShareable = 2ULL << kShOffset;
/// 内部共享
static constexpr uint64_t kShInnerShareable = 3ULL << kShOffset;

/// 内存属性索引值
/// 设备内存
static constexpr uint64_t kAttrDevice = 0ULL << kAttrIndxOffset;
/// 普通内存，非缓存
static constexpr uint64_t kAttrNormalNc = 1ULL << kAttrIndxOffset;
/// 普通内存，写透
static constexpr uint64_t kAttrNormalWt = 2ULL << kAttrIndxOffset;
/// 普通内存，写回
static constexpr uint64_t kAttrNormalWb = 3ULL << kAttrIndxOffset;

/**
 * @brief 开启分页
 */
inline void EnablePage() {
  detail::regs::system_reg::SCTLR_EL1::M::Set();
  __asm__ volatile("isb");
}

/**
 * @brief 关闭分页
 */
inline void DisablePage() {
  detail::regs::system_reg::SCTLR_EL1::M::Clear();
  __asm__ volatile("isb");
}

/**
 * @brief 设置页目录
 * @param pgd 要设置的页表物理地址
 */
inline void SetPageDirectory(uint64_t pgd) {
  detail::regs::system_reg::TTBR1_EL1::Write(pgd);
  __asm__ volatile("isb");
}

/**
 * @brief 获取页目录
 * @return uint64_t 页目录物理地址值
 */
inline auto GetPageDirectory() -> uint64_t {
  return detail::regs::system_reg::TTBR1_EL1::Read();
}

/**
 * @brief 刷新页表缓存
 * @param addr 要刷新的虚拟地址（当前忽略，刷新全部）
 */
inline void FlushPage(uint64_t addr) {
  (void)addr;
  __asm__ volatile("dsb sy");
  __asm__ volatile("tlbi vmalle1is");
  __asm__ volatile("dsb sy");
  __asm__ volatile("isb");
}

}  // namespace virtual_memory
}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_VIRTUAL_MEMORY_HPP_
