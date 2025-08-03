/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_X86_64_CPU_HPP_
#define CPU_IO_INCLUDE_X86_64_CPU_HPP_

#include <cpuid.h>
#include <sys/cdefs.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
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
 * @brief APIC 信息
 */
struct ApicInfo {
  // 是否支持 APIC
  bool has_apic;
  // 是否支持 x2APIC
  bool has_x2apic;
  // APIC ID
  uint32_t apic_id;
  // 初始 APIC ID
  uint32_t initial_apic_id;
};

/**
 * @brief CPU 拓扑信息
 */
struct CpuTopologyInfo {
  // 逻辑处理器数量
  uint32_t logical_processors;
  // 每个包的核心数
  uint32_t cores_per_package;
  // 每个核心的线程数
  uint32_t threads_per_core;
};

/**
 * @brief 获取 CPU 厂商字符串
 * @return std::array<char, 13> 厂商字符串数组（包含 null 终止符）
 */
static __always_inline auto GetCpuVendorString() -> std::array<char, 13> {
  std::array<char, 13> vendor_string{};
  uint32_t eax, ebx, ecx, edx;

  // CPUID leaf 0: 获取最大基本功能号和厂商字符串
  __cpuid(0, eax, ebx, ecx, edx);

  // 厂商字符串存储在 EBX, EDX, ECX 中
  std::memcpy(vendor_string.data(), &ebx, 4);
  std::memcpy(vendor_string.data() + 4, &edx, 4);
  std::memcpy(vendor_string.data() + 8, &ecx, 4);
  vendor_string[12] = '\0';

  return vendor_string;
}

/**
 * @brief 获取 APIC 和 x2APIC 信息
 * @return ApicInfo APIC 信息
 */
static __always_inline auto GetApicInfo() -> ApicInfo {
  ApicInfo info{};
  uint32_t eax, ebx, ecx, edx;

  // CPUID leaf 1: 获取处理器信息和特性位
  __cpuid(1, eax, ebx, ecx, edx);

  // 使用 cpuid.h 中的宏定义检查 APIC 支持
  info.has_apic = (edx & (1 << 9)) != 0;

  // 使用 cpuid.h 中的宏定义检查 x2APIC 支持
  info.has_x2apic = (ecx & (1 << 21)) != 0;

  // EBX bits 31-24: 初始 APIC ID
  info.initial_apic_id = (ebx >> 24) & 0xFF;

  // 如果支持 x2APIC，获取扩展 APIC ID
  if (info.has_x2apic) {
    // CPUID leaf 0xB: 获取扩展拓扑信息
    __cpuid_count(0xB, 0, eax, ebx, ecx, edx);
    info.apic_id = edx;  // EDX: x2APIC ID
  } else {
    info.apic_id = info.initial_apic_id;
  }

  return info;
}

/**
 * @brief 获取当前 core id
 * @return size_t core id
 */
static __always_inline auto GetCurrentCoreId() -> size_t {
  auto apic_info = GetApicInfo();
  return apic_info.apic_id;
}

/**
 * @brief 获取 CPU 拓扑信息（核心数量等）
 * @return CpuTopologyInfo CPU 拓扑信息
 */
static __always_inline auto GetCpuTopologyInfo() -> CpuTopologyInfo {
  CpuTopologyInfo info{};
  uint32_t eax, ebx, ecx, edx;

  // 首先检查是否支持拓扑信息
  __cpuid(0, eax, ebx, ecx, edx);
  uint32_t max_basic_leaf = eax;

  if (max_basic_leaf >= 1) {
    // CPUID leaf 1: 获取基本信息
    __cpuid(1, eax, ebx, ecx, edx);

    // EBX bits 23-16: 逻辑处理器数量
    info.logical_processors = (ebx >> 16) & 0xFF;

    // 检查是否支持扩展拓扑信息 (leaf 0xB)
    if (max_basic_leaf >= 0xB) {
      __cpuid_count(0xB, 0, eax, ebx, ecx, edx);
      if (ebx != 0) {
        // 使用 leaf 0xB 获取详细拓扑信息

        // Level 0: SMT (超线程) 级别
        __cpuid_count(0xB, 0, eax, ebx, ecx, edx);
        uint32_t threads_per_core_shift = eax & 0x1F;
        info.threads_per_core = 1U << threads_per_core_shift;

        // Level 1: Core 级别
        __cpuid_count(0xB, 1, eax, ebx, ecx, edx);
        uint32_t logical_per_package_shift = eax & 0x1F;
        uint32_t logical_per_package = 1U << logical_per_package_shift;

        info.cores_per_package = logical_per_package / info.threads_per_core;
      } else {
        // 如果 leaf 0xB 不可用，使用 leaf 4 的方法
        if (max_basic_leaf >= 4) {
          __cpuid_count(4, 0, eax, ebx, ecx, edx);
          if ((eax & 0x1F) != 0) {  // 缓存类型不为 0 表示有效
            info.cores_per_package = ((eax >> 26) & 0x3F) + 1;
            info.threads_per_core =
                info.logical_processors / info.cores_per_package;
          } else {
            // 假设没有超线程
            info.cores_per_package = info.logical_processors;
            info.threads_per_core = 1;
          }
        } else {
          // 回退到基本假设
          info.cores_per_package = info.logical_processors;
          info.threads_per_core = 1;
        }
      }
    } else {
      // 没有扩展拓扑信息，假设没有超线程
      info.cores_per_package = info.logical_processors;
      info.threads_per_core = 1;
    }
  }

  return info;
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
