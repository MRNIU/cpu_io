/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_X86_64_CPU_HPP_
#define CPU_IO_INCLUDE_X86_64_CPU_HPP_

#include <sys/cdefs.h>

#include <cstdint>
#include <type_traits>

#include "cpuid.hpp"
#include "io.hpp"
#include "msr.hpp"
#include "pic.hpp"
#include "pit.hpp"
#include "regs.hpp"
#include "serial.hpp"

/**
 * x86_64 cpu 相关定义
 * @note 寄存器读写设计见 arch/README.md
 * @see sdm.pdf
 * Intel® 64 and IA-32 Architectures Software Developer’s Manual
 * Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 * Order Number: 325384-083US
 * https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
 */
namespace cpu_io {

/**
 * @brief 允许中断
 */
static __always_inline void EnableInterrupt() { Rflags::If::Set(); }

/**
 * @brief 关闭中断
 */
static __always_inline void DisableInterrupt() { Rflags::If::Clear(); }

/**
 * @brief 获取中断状态
 */
static __always_inline auto GetInterruptStatus() -> bool {
  return Rflags::If::Get();
}

/**
 * @brief 获取当前 core id
 * @return size_t core id
 */
static __always_inline auto GetCurrentCoreId() -> size_t {
  return cpuid::GetExtendedApicId();
}

/// 中断上下文，由 cpu 自动压入，无错误码
struct InterruptContext {
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
};

/// 中断上下文，由 cpu 自动压入，有错误码
struct InterruptContextErrorCode {
  detail::register_info::IdtrInfo::ErrorCode error_code;
  uint32_t padding;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
};

};  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_X86_64_CPU_HPP_
