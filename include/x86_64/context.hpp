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
  /// @todo x86_64 实现待补充
  uint64_t return_address;
  uint64_t stack_pointer;
  uint64_t entry_function;
  uint64_t entry_argument;

  // 跨架构访问器方法
  __always_inline uint64_t& ReturnAddress() { return return_address; }
  __always_inline uint64_t& EntryFunction() { return entry_function; }
  __always_inline uint64_t& EntryArgument() { return entry_argument; }
  __always_inline uint64_t& StackPointer() { return stack_pointer; }
};

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_X86_64_CONTEXT_HPP_
