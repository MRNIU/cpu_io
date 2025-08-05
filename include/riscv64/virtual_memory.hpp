/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_RISCV64_CPU_HPP_
#define CPU_IO_INCLUDE_RISCV64_CPU_HPP_

#include <cstdint>
#include <cstdlib>

#include "regs.hpp"

namespace cpu_io {

namespace virtual_memory {
/// 页表项位偏移定义
static constexpr uint8_t kValidOffset = 0;
static constexpr uint8_t kReadOffset = 1;
static constexpr uint8_t kWriteOffset = 2;
static constexpr uint8_t kExecOffset = 3;
static constexpr uint8_t kUserOffset = 4;
static constexpr uint8_t kGlobalOffset = 5;
static constexpr uint8_t kAccessedOffset = 6;
static constexpr uint8_t kDirtyOffset = 7;

/// 页表项权限位定义
static constexpr uint8_t kValid = 1 << kValidOffset;
static constexpr uint8_t kRead = 1 << kReadOffset;
static constexpr uint8_t kWrite = 1 << kWriteOffset;
static constexpr uint8_t kExec = 1 << kExecOffset;
static constexpr uint8_t kUser = 1 << kUserOffset;
static constexpr uint8_t kGlobal = 1 << kGlobalOffset;
static constexpr uint8_t kAccessed = 1 << kAccessedOffset;
static constexpr uint8_t kDirty = 1 << kDirtyOffset;

/// 页大小常量 (4KB)
static constexpr size_t kPageSize = 4096;
/// 内核虚拟地址相对物理地址的偏移
static constexpr size_t kKernelOffset = 0x0;

/// 页表项属性位数
static constexpr size_t kPteAttributeBits = 10;
/// 页内偏移位数
static constexpr size_t kPageOffsetBits = 12;
/// 虚拟页号位数
static constexpr size_t kVpnBits = 9;
/// 虚拟页号位掩码
static constexpr size_t kVpnMask = 0x1FF;
/// 页表级数（三级页表）
static constexpr size_t kPageTableLevels = 3;

/// 页表项权限标志
static constexpr uint8_t kPteValid = kValid;
static constexpr uint8_t kPteRead = kRead;
static constexpr uint8_t kPteWrite = kWrite;
static constexpr uint8_t kPteExecute = kExec;
static constexpr uint8_t kPteUser = kUser;
static constexpr uint8_t kPteGlobal = kGlobal;
static constexpr uint8_t kPteAccessed = kAccessed;
static constexpr uint8_t kPteDirty = kDirty;

/**
 * @brief 开启分页
 */
inline void EnablePage() {
  Satp::Asid::Write(0);
  Satp::Mode::Write(detail::register_info::csr::SatpInfo::kSv39);
}

/**
 * @brief 关闭分页
 */
inline void DisablePage() {
  Satp::Mode::Write(detail::register_info::csr::SatpInfo::kBare);
}

/**
 * @brief 设置页目录
 * @param pgd 要设置的页表物理地址
 */
inline void SetPageDirectory(uint64_t pgd) {
  Satp::Ppn::Write(pgd >> detail::register_info::csr::SatpInfo::kPpnOffset);
}

/**
 * @brief 获取页目录
 * @return uint64_t 页目录物理地址值
 */
inline auto GetPageDirectory() -> uint64_t {
  return Satp::Ppn::Get() << detail::register_info::csr::SatpInfo::kPpnOffset;
}

/**
 * @brief 刷新页表缓存
 * @param addr 要刷新的虚拟地址（当前忽略，刷新全部）
 */
inline void FlushPage(uint64_t addr) {
  (void)addr;
  __asm__ volatile("sfence.vma zero, zero");
}

}  // namespace virtual_memory
}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_RISCV64_CPU_HPP_
