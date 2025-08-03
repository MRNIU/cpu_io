/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_X86_64_IO_HPP_
#define CPU_IO_INCLUDE_X86_64_IO_HPP_

#include <cstdint>

namespace cpu_io {
/**
 * @brief  从端口读数据
 * @tparam T              要读的数据类型
 * @param  port           要读的端口
 * @return uint8_t        读取到的数据
 */
template <class T>
static __always_inline auto In(const uint32_t port) -> T {
  T data;
  if constexpr (std::is_same_v<T, uint8_t>) {
    __asm__ volatile("inb %1, %0" : "=a"(data) : "dN"(port));
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    __asm__ volatile("inw %1, %0" : "=a"(data) : "dN"(port));
  } else if constexpr (std::is_same_v<T, uint32_t>) {
    __asm__ volatile("inl %1, %0" : "=a"(data) : "dN"(port));
  } else {
    static_assert(sizeof(T) == 0);
  }
  return data;
}

/**
 * @brief  向端口写数据
 * @tparam T              要写的数据类型
 * @param  port           要写的端口
 * @param  data           要写的数据
 */
template <class T>
static __always_inline void Out(const uint32_t port, const T data) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(data));
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    __asm__ volatile("outw %1, %0" : : "dN"(port), "a"(data));
  } else if constexpr (std::is_same_v<T, uint32_t>) {
    __asm__ volatile("outl %1, %0" : : "dN"(port), "a"(data));
  } else {
    static_assert(sizeof(T) == 0);
  }
}

};  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_X86_64_IO_HPP_
