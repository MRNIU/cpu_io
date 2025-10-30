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
 * @note 禁用 MMU、数据缓存和指令缓存，并清除 TLB
 */
inline void DisablePage() {
  // 禁用 MMU、数据缓存和指令缓存
  detail::regs::system_reg::SCTLR_EL1::M::Clear();
  detail::regs::system_reg::SCTLR_EL1::C::Clear();
  detail::regs::system_reg::SCTLR_EL1::I::Clear();
  
  // 指令同步屏障
  __asm__ volatile("isb");
  
  // 清除 TLB
  __asm__ volatile("dsb sy");
  __asm__ volatile("tlbi vmalle1");
  __asm__ volatile("dsb sy");
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
 * @brief 物理地址转换为页表项
 * @param physical_addr 物理地址
 * @param flags 页表项标志位
 * @return uint64_t 页表项值
 */
static __always_inline auto PhysicalToPageTableEntry(uint64_t physical_addr,
                                                     uint64_t flags)
    -> uint64_t {
  return ((physical_addr >> kPageOffsetBits) << kPteAttributeBits) |
         (flags & ((1ULL << kPteAttributeBits) - 1));
}

/**
 * @brief 页表项转换为物理地址
 * @param pte 页表项
 * @return uint64_t 物理地址
 */
static __always_inline auto PageTableEntryToPhysical(uint64_t pte) -> uint64_t {
  return (pte >> kPteAttributeBits) << kPageOffsetBits;
}

/**
 * @brief 检查页表项是否有效
 * @param pte 页表项
 * @return true 页表项有效
 * @return false 页表项无效
 */
static __always_inline auto IsPageTableEntryValid(uint64_t pte) -> bool {
  return (pte & kValid) != 0;
}

/**
 * @brief 获取虚拟地址的页号
 * @param virtual_addr 虚拟地址
 * @param level 页表级别（0-3，AArch64 四级页表）
 * @return uint64_t 对应级别的页号
 */
static __always_inline auto GetVirtualPageNumber(uint64_t virtual_addr,
                                                 size_t level) -> uint64_t {
  return (virtual_addr >> (kPageOffsetBits + level * kVpnBits)) & kVpnMask;
}

/**
 * @brief 计算页表级别的偏移量
 * @param level 页表级别
 * @return size_t 偏移量
 */
static __always_inline auto GetPageTableLevelShift(size_t level) -> size_t {
  return kPageOffsetBits + (level * kVpnBits);
}

/**
 * @brief 虚拟地址转物理地址（简单的内核空间映射）
 * @param virtual_addr 虚拟地址
 * @return uint64_t 物理地址
 */
static __always_inline auto VirtualToPhysical(uint64_t virtual_addr)
    -> uint64_t {
  return virtual_addr - kKernelOffset;
}

/**
 * @brief 物理地址转虚拟地址（简单的内核空间映射）
 * @param physical_addr 物理地址
 * @return uint64_t 虚拟地址
 */
static __always_inline auto PhysicalToVirtual(uint64_t physical_addr)
    -> uint64_t {
  return physical_addr + kKernelOffset;
}

/**
 * @brief 获取页面对齐的地址
 * @param addr 地址
 * @return uint64_t 页面对齐的地址
 */
static __always_inline auto PageAlign(uint64_t addr) -> uint64_t {
  return addr & ~(kPageSize - 1);
}

/**
 * @brief 获取页面上对齐的地址
 * @param addr 地址
 * @return uint64_t 页面上对齐的地址
 */
static __always_inline auto PageAlignUp(uint64_t addr) -> uint64_t {
  return (addr + kPageSize - 1) & ~(kPageSize - 1);
}

/**
 * @brief 检查地址是否页面对齐
 * @param addr 地址
 * @return true 地址页面对齐
 * @return false 地址未页面对齐
 */
static __always_inline auto IsPageAligned(uint64_t addr) -> bool {
  return (addr & (kPageSize - 1)) == 0;
}

/**
 * @brief 获取页内偏移
 * @param addr 地址
 * @return uint64_t 页内偏移
 */
static __always_inline auto GetPageOffset(uint64_t addr) -> uint64_t {
  return addr & (kPageSize - 1);
}

/**
 * @brief 创建页表项
 * @param physical_addr 物理地址
 * @param readable 可读标志，aarch64 未使用
 * @param writable 可写标志
 * @param executable 可执行标志
 * @param user_accessible 用户可访问标志
 * @param global 全局标志
 * @return uint64_t 页表项
 */
static __always_inline auto CreatePageTableEntry(
    uint64_t physical_addr, [[maybe_unused]] bool readable = true,
    bool writable = false, bool executable = false,
    bool user_accessible = false, bool global = false) -> uint64_t {
  uint64_t flags = kValid | kAf;  // AArch64需要设置访问标志位

  // 设置访问权限
  if (user_accessible) {
    if (writable) {
      flags |= kApUserReadWrite;
    } else {
      flags |= kApUserReadOnly;
    }
  } else {
    if (writable) {
      flags |= kApReadWrite;
    } else {
      flags |= kApReadOnly;
    }
  }

  // 设置执行权限（AArch64使用禁止执行位）
  if (!executable) {
    if (user_accessible) {
      flags |= kUxn;  // 用户禁止执行
    } else {
      flags |= kPxn;  // 特权禁止执行
    }
  }

  // 设置全局标志
  if (!global) {
    flags |= kNg;  // AArch64的nG位表示非全局
  }

  // 设置默认内存属性（普通内存，写回缓存）
  flags |= kAttrNormalWb | kShInnerShareable;

  return PhysicalToPageTableEntry(physical_addr, flags);
}

/**
 * @brief 检查页表项权限
 * @param pte 页表项
 * @param check_read 检查读权限
 * @param check_write 检查写权限
 * @param check_exec 检查执行权限
 * @param check_user 检查用户权限
 * @return true 权限检查通过
 * @return false 权限检查失败
 */
static __always_inline auto CheckPageTableEntryPermissions(
    uint64_t pte, bool check_read = false, bool check_write = false,
    bool check_exec = false, bool check_user = false) -> bool {
  if (!IsPageTableEntryValid(pte)) return false;

  uint64_t ap = pte & kAp;

  // 检查读权限（AArch64所有有效页都可读）
  if (check_read) {
    // 有效页表项默认可读
  }

  // 检查写权限
  if (check_write) {
    if (ap == kApReadOnly || ap == kApUserReadOnly) {
      return false;
    }
  }

  // 检查执行权限
  if (check_exec) {
    if (check_user && (pte & kUxn)) return false;
    if (!check_user && (pte & kPxn)) return false;
  }

  // 检查用户权限
  if (check_user) {
    if (ap != kApUserReadWrite && ap != kApUserReadOnly) {
      return false;
    }
  }

  return true;
}

/**
 * @brief 刷新特定地址的 TLB
 * @param virtual_addr 要刷新的虚拟地址
 * @param asid 地址空间标识符（当前忽略）
 */
static __always_inline void FlushTLBAddress(
    uint64_t virtual_addr, [[maybe_unused]] uint64_t asid = 0) {
  __asm__ volatile("dsb sy");
  __asm__ volatile("tlbi vae1is, %0" : : "r"(virtual_addr >> 12));
  __asm__ volatile("dsb sy");
  __asm__ volatile("isb");
}

/**
 * @brief 刷新所有TLB条目
 */
static __always_inline void FlushTLBAll() {
  __asm__ volatile("dsb sy");
  __asm__ volatile("tlbi vmalle1is");
  __asm__ volatile("dsb sy");
  __asm__ volatile("isb");
}

/**
 * @brief 计算地址范围包含的页数
 * @param start_addr 起始地址
 * @param end_addr 结束地址
 * @return size_t 页数
 */
static __always_inline auto GetPageCount(uint64_t start_addr, uint64_t end_addr)
    -> size_t {
  uint64_t aligned_start = PageAlign(start_addr);
  uint64_t aligned_end = PageAlignUp(end_addr);
  return (aligned_end - aligned_start) / kPageSize;
}

/**
 * @brief 获取内核页表权限
 * @param readable 可读标志，aarch64 未使用
 * @param writable 可写标志
 * @param executable 可执行标志
 * @param global 全局标志
 * @return uint64_t 内核页表权限标志
 */
static __always_inline auto GetKernelPagePermissions(
    [[maybe_unused]] bool readable = true, bool writable = true,
    bool executable = false, bool global = true) -> uint64_t {
  uint64_t flags = kValid | kAf;  // 基础标志：有效位和访问标志位

  // 设置访问权限（内核级别）
  if (writable) {
    flags |= kApReadWrite;  // EL1 读写
  } else {
    flags |= kApReadOnly;  // EL1 只读
  }

  // 设置执行权限
  if (!executable) {
    flags |= kPxn;  // 特权禁止执行
  }

  // 设置全局标志
  if (!global) {
    flags |= kNg;  // 非全局
  }

  // 设置默认内存属性（普通内存，写回缓存，内部共享）
  flags |= kAttrNormalWb | kShInnerShareable;

  return flags;
}

/**
 * @brief 获取用户页表权限
 * @param readable 可读标志，aarch64 未使用
 * @param writable 可写标志
 * @param executable 可执行标志
 * @param global 全局标志
 * @return uint64_t 用户页表权限标志
 */
static __always_inline auto GetUserPagePermissions(
    [[maybe_unused]] bool readable = true, bool writable = false,
    bool executable = false, bool global = false) -> uint64_t {
  uint64_t flags = kValid | kAf;  // 基础标志：有效位和访问标志位

  // 设置访问权限（用户级别）
  if (writable) {
    flags |= kApUserReadWrite;  // EL0/EL1 读写
  } else {
    flags |= kApUserReadOnly;  // EL0/EL1 只读
  }

  // 设置执行权限
  if (!executable) {
    flags |= kUxn;  // 用户禁止执行
  }

  // 设置全局标志
  if (!global) {
    flags |= kNg;  // 非全局（用户页面通常是非全局的）
  }

  // 设置默认内存属性（普通内存，写回缓存，内部共享）
  flags |= kAttrNormalWb | kShInnerShareable;

  return flags;
}

}  // namespace virtual_memory
}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_VIRTUAL_MEMORY_HPP_
