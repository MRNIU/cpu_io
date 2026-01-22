/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_X86_64_CONTEXT_HPP_
#define CPU_IO_INCLUDE_X86_64_CONTEXT_HPP_

#include <cstdint>

namespace cpu_io {

struct TrapContext {
  /// @todo

  // 统一的跨架构访问器方法（占位实现）
  __always_inline uint64_t& UserStackPointer() {
    static uint64_t dummy = 0;
    return dummy;
  }
  __always_inline uint64_t& ThreadPointer() {
    static uint64_t dummy = 0;
    return dummy;
  }
  __always_inline uint64_t& ReturnValue() {
    static uint64_t dummy = 0;
    return dummy;
  }
};

struct CalleeSavedContext {
  /// @todo
};

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_X86_64_CONTEXT_HPP_
