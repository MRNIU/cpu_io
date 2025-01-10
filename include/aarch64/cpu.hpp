
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
 * @brief 允许中断
 * @todo
 */
static __always_inline void EnableInterrupt() {
  DAIF::I::Clear();
  DAIF::F::Clear();
}

/**
 * @brief 关闭中断
 * @todo
 */
static __always_inline void DisableInterrupt() {
  DAIF::I::Set();
  DAIF::F::Set();
}

/**
 * @brief 获取中断状态
 * @return true 允许中断
 * @return false 禁用中断
 */
static __always_inline auto GetInterruptStatus() -> bool {
  return !DAIF::I::Get() && !DAIF::F::Get();
}

/**
 * @brief 获取当前 core id
 * @return size_t core id
 */
static __always_inline auto GetCurrentCoreId() -> size_t {
  return MPIDR_EL1::Aff0::Get();
}

/**
 * @brief 初始化 FPU
 */
static __always_inline void SetupFpu() { CPACR_EL1::Fpen::Set(); }

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_CPU_HPP_
