
/**
 * @file cpu.hpp
 * @brief aarch64 cpu 相关定义
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

#ifndef CPU_IO_INCLUDE_AARCH64_CPU_HPP_
#define CPU_IO_INCLUDE_AARCH64_CPU_HPP_

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <typeinfo>

#include "regs.hpp"

/**
 * aarch64 cpu 相关定义
 * @note 寄存器读写设计见 arch/README.md
 */
namespace cpu_io {

/**
 * @brief 初始化 FPU
 */
static __always_inline void SetupFpu() {
  asm volatile("mrs x0, CPACR_EL1");
  asm volatile("orr x0, x0, #(0b11 << 20)");
  asm volatile("msr CPACR_EL1, x0");
  asm volatile("isb");
}

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_CPU_HPP_
