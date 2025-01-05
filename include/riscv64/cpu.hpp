
/**
 * @file cpu.hpp
 * @brief riscv64 cpu 相关定义
 * @author Zone.N (Zone.Niuzh@hotmail.com)
 * @version 1.0
 * @date 2024-03-05
 * @copyright MIT LICENSE
 * https://github.com/MRNIU/cpu_io
 * @par change log:
 * <table>
 * <tr><th>Date<th>Author<th>Description
 * <tr><td>2024-03-05<td>Zone.N (Zone.Niuzh@hotmail.com)<td>创建文件
 * </table>
 */

#ifndef CPU_IO_INCLUDE_RISCV64_CPU_HPP_
#define CPU_IO_INCLUDE_RISCV64_CPU_HPP_

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <typeinfo>

#include "regs.hpp"

namespace cpu_io {

/**
 * @brief 允许中断
 */
static __always_inline void EnableInterrupt() { Sstatus::Sie::Set(); }

/**
 * @brief 关闭中断
 */
static __always_inline void DisableInterrupt() { Sstatus::Sie::Clear(); }

/**
 * @brief 获取中断状态
 */
static __always_inline auto GetInterruptStatus() -> bool {
  return Sstatus::Sie::Get();
}

namespace vmm {
enum {
  VALID_OFFSET = 0,
  READ_OFFSET = 1,
  WRITE_OFFSET = 2,
  EXEC_OFFSET = 3,
  USER_OFFSET = 4,
  GLOBAL_OFFSET = 5,
  ACCESSED_OFFSET = 6,
  DIRTY_OFFSET = 7,
  VALID = 1 << VALID_OFFSET,
  READ = 1 << READ_OFFSET,
  WRITE = 1 << WRITE_OFFSET,
  EXEC = 1 << EXEC_OFFSET,
  USER = 1 << USER_OFFSET,
  GLOBAL = 1 << GLOBAL_OFFSET,
  ACCESSED = 1 << ACCESSED_OFFSET,
  DIRTY = 1 << DIRTY_OFFSET,
};
/// 有效位
static constexpr const uint8_t VMM_PAGE_VALID = VALID;
/// 可读位
static constexpr const uint8_t VMM_PAGE_READABLE = READ;
/// 可写位s
static constexpr const uint8_t VMM_PAGE_WRITABLE = WRITE;
/// 可执行位
static constexpr const uint8_t VMM_PAGE_EXECUTABLE = EXEC;
/// 用户位
static constexpr const uint8_t VMM_PAGE_USER = USER;
/// 全局位，我们不会使用
static constexpr const uint8_t VMM_PAGE_GLOBAL = GLOBAL;
/// 已使用位，用于替换算法
static constexpr const uint8_t VMM_PAGE_ACCESSED = ACCESSED;
/// 已修改位，用于替换算法
static constexpr const uint8_t VMM_PAGE_DIRTY = DIRTY;
/// 内核虚拟地址相对物理地址的偏移
static constexpr const size_t KERNEL_OFFSET = 0x0;
/// PTE 属性位数
static constexpr const size_t VMM_PTE_PROP_BITS = 10;
/// PTE 页内偏移位数
static constexpr const size_t VMM_PAGE_OFF_BITS = 12;
/// VPN 位数
static constexpr const size_t VMM_VPN_BITS = 9;
/// VPN 位数掩码，9 位 VPN
static constexpr const size_t VMM_VPN_BITS_MASK = 0x1FF;
/// riscv64 使用了三级页表
static constexpr const size_t VMM_PT_LEVEL = 3;

// 开启 PG
inline void EnablePage() {
  Satp::Asid::Write(0);
  Satp::Mode::Write(detail::register_info::csr::SatpInfo::kSv39);
}
inline void DisablePage() {
  Satp::Mode::Write(detail::register_info::csr::SatpInfo::kBare);
}

/**
 * @brief 设置 页目录
 * @param  _pgd            要设置的页表
 * @return true            成功
 * @return false           失败
 */
inline void SetPageDirectory(uint64_t pgd) {
  Satp::Ppn::Write(pgd >> detail::register_info::csr::SatpInfo::kPpnOffset);
}

/**
 * @brief 获取页目录
 * @return uint64_t        页目录值
 */
inline auto GetPageDirectory() -> uint64_t {
  return Satp::Ppn::Get() << detail::register_info::csr::SatpInfo::kPpnOffset;
}

inline void FlushPage(uint64_t addr) {
  (void)addr;
  __asm__ volatile("sfence.vma zero, zero");
}

}  // namespace vmm
}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_RISCV64_CPU_HPP_
