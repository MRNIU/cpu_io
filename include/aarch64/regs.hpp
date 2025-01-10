
/**
 * @file regs.hpp
 * @brief aarch64 寄存器相关定义
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

#ifndef CPU_IO_INCLUDE_AARCH64_REGS_HPP_
#define CPU_IO_INCLUDE_AARCH64_REGS_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <typeinfo>

/**
 * aarch64 cpu Control and Status Registers 相关定义
 * @see
 * https://developer.arm.com/documentation/#numberOfResults=48&&cf-navigationhierarchiesproducts=%20Architectures,CPU%20Architecture,A-Profile,Armv8-A
 * @note 寄存器读写设计见 arch/README.md
 */

namespace cpu_io {

namespace detail {

// 第一部分：寄存器定义
namespace register_info {

struct RegInfoBase {
  /// 寄存器数据类型
  using DataType = uint64_t;
  /// 起始位
  static constexpr uint64_t kBitOffset = 0;
  /// 位宽
  static constexpr uint64_t kBitWidth = 64;
  /// 掩码，(val & kBitMask) == 对应当前位的值
  static constexpr uint64_t kBitMask =
      (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
  /// 对应位置位掩码
  static constexpr uint64_t kAllSetMask =
      (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
};

/// 通用寄存器
struct X29Info : public RegInfoBase {};

namespace system_reg {
/// 立即数掩码，大于这个值需要使用寄存器中转
/// @see
/// https://developer.arm.com/documentation/dui0802/b/A64-General-Instructions/MSR--immediate-
static constexpr uint64_t kPSTATEImmOpMask = 0xF;

/**
 * @brief CPACR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0595/2021-03/AArch64-Registers/CPACR-EL1--Architectural-Feature-Access-Control-Register?lang=en
 */
struct CPACR_EL1Info : public RegInfoBase {
  struct Fpen {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 20;
    static constexpr uint64_t kBitWidth = 2;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief CurrentEL 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CurrentEL--Current-Exception-Level
 */
struct CurrentELInfo : public RegInfoBase {
  static constexpr const uint8_t kEL0 = 0b00;
  static constexpr const uint8_t kEL1 = 0b01;
  static constexpr const uint8_t kEL2 = 0b10;
  static constexpr const uint8_t kEL3 = 0b11;

  struct EL {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 2;
    static constexpr uint64_t kBitWidth = 2;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief SPSel 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/SPSel--Stack-Pointer-Select
 */
struct SPSelInfo : public RegInfoBase {
  static constexpr const bool kEL0 = false;
  static constexpr const bool kELx = true;

  struct SP {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief DAIF 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/DAIF--Interrupt-Mask-Bits
 */
struct DAIFInfo : public RegInfoBase {
  static constexpr const bool kNotMasked = false;
  static constexpr const bool kMasked = true;

  /// Process state D mask.
  /// Watchpoint, Breakpoint, and Software Step exceptions targeted at the
  /// current Exception level
  struct D {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 9;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  /// SError exception mask bit.
  struct A {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 8;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  /// IRQ mask bit.
  struct I {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 7;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  /// FIQ mask bit.
  struct F {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 6;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief VBAR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/VBAR-EL1--Vector-Base-Address-Register--EL1-
 */
struct VBAR_EL1Info : public RegInfoBase {
  struct Base {
    using DataType = uint64_t;
    static constexpr uint64_t kBitOffset = 11;
    static constexpr uint64_t kBitWidth = 53;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief ELR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ELR-EL1--Exception-Link-Register--EL1-
 */
struct ELR_EL1Info : public RegInfoBase {};

/**
 * @brief SPSR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/SPSR-EL1--Saved-Program-Status-Register--EL1-
 */
struct SPSR_EL1Info : public RegInfoBase {};

/**
 * @brief SP_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/SP-EL0--Stack-Pointer--EL0-
 */
struct SP_EL0Info : public RegInfoBase {};

/**
 * @brief SP_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/SP-EL1--Stack-Pointer--EL1-
 */
struct SP_EL1Info : public RegInfoBase {};

/**
 * @brief MPIDR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/MPIDR-EL1--Multiprocessor-Affinity-Register
 */
struct MPIDR_EL1Info : public RegInfoBase {
  struct Aff3 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 32;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct U {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 30;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Processor is part of a uniprocessor system.
    static constexpr const bool KUniProcessor = false;
    /// Processor is part of a multiprocessor system.
    static constexpr const bool kMultiProssor = true;
  };

  struct MT {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 24;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Performance of PEs with different affinity level 0 values, and the same
    /// values for affinity level 1 and higher, is largely independent.
    static constexpr const bool KSingleThread = false;

    /// Performance of PEs with different affinity level 0 values, and the same
    /// values for affinity level 1 and higher, is very interdependent.
    static constexpr const bool KMultiThread = true;
  };

  struct Aff2 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 16;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff1 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 8;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff0 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief TTBR0_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/TTBR0-EL1--Translation-Table-Base-Register-0--EL1-
 */
struct TTBR0_EL1Info : public RegInfoBase {};

/**
 * @brief TTBR1_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/TTBR1-EL1--Translation-Table-Base-Register-1--EL1-
 */
struct TTBR1_EL1Info : public RegInfoBase {};

/**
 * @brief TCR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/TCR-EL1--Translation-Control-Register--EL1-
 */
struct TCR_EL1Info : public RegInfoBase {};

/**
 * @brief MAIR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/MAIR-EL1--Memory-Attribute-Indirection-Register--EL1-
 */
struct MAIR_EL1Info : public RegInfoBase {
  ///	Device-nGnRnE memory.
  static constexpr const uint8_t KDeviceNGnRnE = 0b00000000;
  ///	Device-nGnRE memory.
  static constexpr const uint8_t KDeviceNGnRE = 0b00000100;
  ///	Device-nGRE memory.
  static constexpr const uint8_t KDeviceNGRE = 0b00001000;
  ///	Device-GRE memory.
  static constexpr const uint8_t KDeviceGRE = 0b00001100;

  // 0b00RW	where RW != 00	Normal memory, Outer Write-Through Transient.
  // 0b0100		Normal memory, Outer Non-cacheable.
  // 0b01RW	where RW != 00	Normal memory, Outer Write-Back Transient.
  // 0b10RW		Normal memory, Outer Write-Through Non-transient.
  // 0b11RW		Normal memory, Outer Write-Back Non-transient.

  // 0b0000		See encoding of Attr.
  // 0b00RW	where RW != 00	Normal memory, Inner Write-Through Transient.
  // 0b0100		Normal memory, Inner Non-cacheable.
  // 0b01RW	where RW != 00	Normal memory, Inner Write-Back Transient.
  // 0b10RW		Normal memory, Inner Write-Through Non-transient.
  // 0b11RW		Normal memory, Inner Write-Back Non-transient.

  // 0b0	No Allocate.
  // 0b1	Allocate.

  struct Aff7 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 56;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff6 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 48;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff5 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 40;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff4 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 32;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff3 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 24;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff2 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 16;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff1 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 8;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Aff0 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 8;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief SCTLR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/SCTLR-EL1--System-Control-Register--EL1-
 */
struct SCTLR_EL1Info : public RegInfoBase {
  struct M {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  // EL1&0 stage 1 address translation disabled.
  static constexpr const bool KDisabled = false;

  // EL1&0 stage 1 address translation enabled.
  static constexpr const bool kEnabled = true;
};

/**
 * @brief ESR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-
 */
struct ESR_EL1Info : public RegInfoBase {};

/**
 * @brief CNTV_CTL_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTV-CTL-EL0--Counter-timer-Virtual-Timer-Control-Register
 */
struct CNTV_CTL_EL0Info : public RegInfoBase {};

/**
 * @brief CNTV_TVAL_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTV-TVAL-EL0--Counter-timer-Virtual-Timer-TimerValue-Register
 */
struct CNTV_TVAL_EL0Info : public RegInfoBase {};

/**
 * @brief FAR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/FAR-EL1--Fault-Address-Register--EL1-
 */
struct FAR_EL1Info : public RegInfoBase {};

/**
 * @brief CNTVCT_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTVCT-EL0--Counter-timer-Virtual-Count-Register
 */
struct CNTVCT_EL0Info : public RegInfoBase {};

/**
 * @brief CNTFRQ_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTFRQ-EL0--Counter-timer-Frequency-Register
 */
struct CNTFRQ_EL0Info : public RegInfoBase {};

};  // namespace system_reg

};  // namespace register_info

// 第二部分：读/写模版实现
namespace read_write {
/**
 * 只读接口
 * @tparam 寄存器类型
 */
template <class RegInfo>
class ReadOnlyRegBase {
 public:
  /// @name 构造/析构函数
  /// @{
  ReadOnlyRegBase() = default;
  ReadOnlyRegBase(const ReadOnlyRegBase &) = delete;
  ReadOnlyRegBase(ReadOnlyRegBase &&) = delete;
  auto operator=(const ReadOnlyRegBase &) -> ReadOnlyRegBase & = delete;
  auto operator=(ReadOnlyRegBase &&) -> ReadOnlyRegBase & = delete;
  ~ReadOnlyRegBase() = default;
  /// @}

  /**
   * 读寄存器
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto Read() -> typename RegInfo::DataType {
    typename RegInfo::DataType value{};
    if constexpr (std::is_same_v<RegInfo, register_info::X29Info>) {
      __asm__ volatile("mov %0, x29" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::CPACR_EL1Info>) {
      __asm__ volatile("mrs %0, CPACR_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::CurrentELInfo>) {
      __asm__ volatile("mrs %0, CurrentEL" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::SPSelInfo>) {
      __asm__ volatile("mrs %0, SPSel" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::DAIFInfo>) {
      __asm__ volatile("mrs %0, DAIF" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::VBAR_EL1Info>) {
      __asm__ volatile("mrs %0, VBAR_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo, register_info::system_reg::ELR_EL1Info>) {
      __asm__ volatile("mrs %0, ELR_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::SPSR_EL1Info>) {
      __asm__ volatile("mrs %0, SPSR_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo, register_info::system_reg::SP_EL0Info>) {
      __asm__ volatile("mrs %0, SP_EL0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo, register_info::system_reg::SP_EL1Info>) {
      __asm__ volatile("mrs %0, SP_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::MPIDR_EL1Info>) {
      __asm__ volatile("mrs %0, MPIDR_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::SCTLR_EL1Info>) {
      __asm__ volatile("mrs %0, SCTLR_EL1" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::MAIR_EL1Info>) {
      __asm__ volatile("mrs %0, MAIR_EL1Info" : "=r"(value) : :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return value;
  }

  /**
   * () 重载
   */
  __always_inline auto operator()() -> typename RegInfo::DataType {
    return Read();
  }
};

/**
 * 只写接口
 * @tparam 寄存器类型
 */
template <class RegInfo>
class WriteOnlyRegBase {
 public:
  /// @name 构造/析构函数
  /// @{
  WriteOnlyRegBase() = default;
  WriteOnlyRegBase(const WriteOnlyRegBase &) = delete;
  WriteOnlyRegBase(WriteOnlyRegBase &&) = delete;
  auto operator=(const WriteOnlyRegBase &) -> WriteOnlyRegBase & = delete;
  auto operator=(WriteOnlyRegBase &&) -> WriteOnlyRegBase & = delete;
  ~WriteOnlyRegBase() = default;
  /// @}

  /**
   * 写寄存器
   * @param value 要写的值
   */
  static __always_inline void Write(typename RegInfo::DataType value) {
    if constexpr (std::is_same_v<RegInfo, register_info::X29Info>) {
      __asm__ volatile("mov x29, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::CPACR_EL1Info>) {
      __asm__ volatile("msr CPACR_EL1, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::SPSelInfo>) {
      __asm__ volatile("msr SPSel, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::DAIFInfo>) {
      __asm__ volatile("msr DAIF, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::VBAR_EL1Info>) {
      __asm__ volatile("msr VBAR_EL1, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo, register_info::system_reg::ELR_EL1Info>) {
      __asm__ volatile("msr ELR_EL1, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::SPSR_EL1Info>) {
      __asm__ volatile("msr SPSR_EL1, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo, register_info::system_reg::SP_EL0Info>) {
      __asm__ volatile("msr SP_EL0, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo, register_info::system_reg::SP_EL1Info>) {
      __asm__ volatile("msr SP_EL1, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::SCTLR_EL1Info>) {
      __asm__ volatile("msr SCTLR_EL1, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::system_reg::MAIR_EL1Info>) {
      __asm__ volatile("msr MAIR_EL1, %0" : : "r"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 写 PSTATE 寄存器，不通过寄存器中转
   * @param value 要写的值
   * @note 只能写 kPSTATEImmOpMask 范围内的值
   */
  static __always_inline void WriteImm(const uint8_t value) {
    if constexpr (std::is_same_v<RegInfo,
                                 register_info::system_reg::SPSelInfo>) {
      __asm__ volatile("msr SPSel, %0" : : "i"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 通过掩码设置寄存器
   * @param mask 掩码
   */
  static __always_inline void SetBits(uint64_t mask) {
    if constexpr (std::is_same_v<RegInfo,
                                 register_info::system_reg::CPACR_EL1Info>) {
      typename RegInfo::DataType value = 0;
      __asm__ volatile("mrs %0, CPACR_EL1" : "=r"(value)::);
      value |= mask;
      Write(value);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::SPSelInfo>) {
      typename RegInfo::DataType value = 0;
      __asm__ volatile("mrs %0, SPSel" : "=r"(value)::);
      value |= mask;
      Write(value);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::DAIFInfo>) {
      typename RegInfo::DataType value = 0;
      __asm__ volatile("mrs %0, DAIF" : "=r"(value)::);
      value |= mask;
      Write(value);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 清零寄存器
   * @param mask 掩码
   */
  static __always_inline void ClearBits(uint64_t mask) {
    if constexpr (std::is_same_v<RegInfo,
                                 register_info::system_reg::CPACR_EL1Info>) {
      typename RegInfo::DataType value = 0;
      __asm__ volatile("mrs %0, CPACR_EL1" : "=r"(value)::);
      value &= ~mask;
      Write(value);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::SPSelInfo>) {
      typename RegInfo::DataType value = 0;
      __asm__ volatile("mrs %0, SPSel" : "=r"(value)::);
      value &= ~mask;
      Write(value);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::system_reg::DAIFInfo>) {
      typename RegInfo::DataType value = 0;
      __asm__ volatile("mrs %0, DAIF" : "=r"(value)::);
      value &= ~mask;
      Write(value);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 通过掩码设置寄存器，不通过寄存器中转
   * @param mask 掩码
   * @note 只能写 kPSTATEImmOpMask 范围内的值
   */
  static __always_inline void SetBitsImm(const uint8_t mask) {
    if constexpr (std::is_same_v<RegInfo,
                                 register_info::system_reg::DAIFInfo>) {
      __asm__ volatile("msr DAIFSet, %0" : : "i"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 清零寄存器，不通过寄存器中转
   * @param mask 掩码
   * @note 只能写 kPSTATEImmOpMask 范围内的值
   */
  static __always_inline void ClearBitsImm(const uint8_t mask) {
    if constexpr (std::is_same_v<RegInfo,
                                 register_info::system_reg::DAIFInfo>) {
      __asm__ volatile("msr DAIFClr, %0" : : "i"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 向寄存器写常数
   * @tparam value 常数的值
   */
  template <uint64_t value>
  static __always_inline void WriteConst() {
    if constexpr ((value & register_info::system_reg::kPSTATEImmOpMask) ==
                  value) {
      WriteImm(value);
    } else {
      Write(value);
    }
  }

  /**
   * 通过掩码写寄存器
   * @tparam mask 掩码
   */
  template <uint64_t mask>
  static __always_inline void SetConst() {
    if constexpr ((mask & register_info::system_reg::kPSTATEImmOpMask) ==
                  mask) {
      SetBitsImm(mask);
    } else {
      SetBits(mask);
    }
  }

  /**
   * 通过掩码清零寄存器
   * @tparam mask 掩码
   */
  template <uint64_t mask>
  static __always_inline void ClearConst() {
    if constexpr ((mask & register_info::system_reg::kPSTATEImmOpMask) ==
                  mask) {
      ClearBitsImm(mask);
    } else {
      ClearBits(mask);
    }
  }

  /**
   * |= 重载
   */
  __always_inline void operator|=(uint64_t mask) { SetBits(mask); }
};

/**
 * 读写接口
 * @tparam 寄存器类型
 */
template <class RegInfo>
class ReadWriteRegBase : public ReadOnlyRegBase<RegInfo>,
                         public WriteOnlyRegBase<RegInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  ReadWriteRegBase() = default;
  ReadWriteRegBase(const ReadWriteRegBase &) = delete;
  ReadWriteRegBase(ReadWriteRegBase &&) = delete;
  auto operator=(const ReadWriteRegBase &) -> ReadWriteRegBase & = delete;
  auto operator=(ReadWriteRegBase &&) -> ReadWriteRegBase & = delete;
  ~ReadWriteRegBase() = default;
  /// @}

  /**
   * 先读后写寄存器
   * @tparam value 要写的值
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadWrite(typename RegInfo::DataType value) ->
      typename RegInfo::DataType {
    auto old_value = ReadOnlyRegBase<RegInfo>::Read();
    WriteOnlyRegBase<RegInfo>::Write(value);
    return old_value;
  }
  /**
   * 先读后通过掩码设置寄存器
   * @param mask 掩码
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadSetBits(uint64_t mask) ->
      typename RegInfo::DataType {
    auto old_value = ReadOnlyRegBase<RegInfo>::Read();
    WriteOnlyRegBase<RegInfo>::SetBits(mask);
    return old_value;
  }

  /**
   * 先读后清零寄存器
   * @param mask 掩码
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadClearBits(uint64_t mask) ->
      typename RegInfo::DataType {
    auto old_value = ReadOnlyRegBase<RegInfo>::Read();
    WriteOnlyRegBase<RegInfo>::ClearBits(mask);
    return old_value;
  }
};

/**
 * 只读位域接口
 * @tparam Reg 寄存器类型
 * @tparam RegInfo 寄存器数据信息
 */
template <class Reg, class RegInfo>
class ReadOnlyField {
 public:
  /// @name 构造/析构函数
  /// @{
  ReadOnlyField() = default;
  ReadOnlyField(const ReadOnlyField &) = delete;
  ReadOnlyField(ReadOnlyField &&) = delete;
  auto operator=(const ReadOnlyField &) -> ReadOnlyField & = delete;
  auto operator=(ReadOnlyField &&) -> ReadOnlyField & = delete;
  ~ReadOnlyField() = default;
  /// @}

  /**
   * 获取对应 Reg 的由 RegInfo 规定的指定位的值
   * @return RegInfo::DataType 指定位值的信息
   */
  static __always_inline auto Get() -> typename RegInfo::DataType {
    return static_cast<typename RegInfo::DataType>(
        (Reg::Read() & RegInfo::kBitMask) >> RegInfo::kBitOffset);
  }

  /**
   * 从指定的值获取对应 Reg 的由 RegInfo 规定的指定位的值
   * @param value 指定的值
   * @return RegInfo::DataType 指定位值的信息
   */
  static __always_inline auto Get(uint64_t value) ->
      typename RegInfo::DataType {
    return static_cast<typename RegInfo::DataType>(
        (value & RegInfo::kBitMask) >> RegInfo::kBitOffset);
  }
};

/**
 * 只写位域接口
 * @tparam Reg 寄存器类型
 * @tparam RegInfo 寄存器数据信息
 */
template <class Reg, class RegInfo>
class WriteOnlyField {
 public:
  /// @name 构造/析构函数
  /// @{
  WriteOnlyField() = default;
  WriteOnlyField(const WriteOnlyField &) = delete;
  WriteOnlyField(WriteOnlyField &&) = delete;
  auto operator=(const WriteOnlyField &) -> WriteOnlyField & = delete;
  auto operator=(WriteOnlyField &&) -> WriteOnlyField & = delete;
  ~WriteOnlyField() = default;
  /// @}

  /**
   * 置位对应 Reg 的由 RegInfo 规定的指定位
   */
  static __always_inline void Set() {
    if constexpr ((RegInfo::kBitMask &
                   register_info::system_reg::kPSTATEImmOpMask) ==
                  RegInfo::kBitMask) {
      Reg::SetBitsImm(RegInfo::kBitMask);
    } else {
      Reg::SetBits(RegInfo::kBitMask);
    }
  }

  /**
   * 清零对应 Reg 的由 RegInfo 规定的指定位
   */
  static __always_inline void Clear() {
    if constexpr ((RegInfo::kBitMask &
                   register_info::system_reg::kPSTATEImmOpMask) ==
                  RegInfo::kBitMask) {
      Reg::ClearBitsImm(RegInfo::kBitMask);
    } else {
      Reg::ClearBits(RegInfo::kBitMask);
    }
  }
};

/**
 * 读写位域接口
 * @tparam Reg 寄存器类型
 * @tparam RegInfo 寄存器数据信息
 */
template <class Reg, class RegInfo>
class ReadWriteField : public ReadOnlyField<Reg, RegInfo>,
                       public WriteOnlyField<Reg, RegInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  ReadWriteField() = default;
  ReadWriteField(const ReadWriteField &) = delete;
  ReadWriteField(ReadWriteField &&) = delete;
  auto operator=(const ReadWriteField &) -> ReadWriteField & = delete;
  auto operator=(ReadWriteField &&) -> ReadWriteField & = delete;
  ~ReadWriteField() = default;
  /// @}

  /**
   * 将寄存器的原值替换为指定值
   * @param value 新值
   */
  static __always_inline void Write(typename RegInfo::DataType value) {
    auto org_value = Reg::Read();
    auto new_value =
        (org_value & ~RegInfo::kBitMask) |
        ((static_cast<decltype(org_value)>(value) << RegInfo::kBitOffset) &
         RegInfo::kBitMask);
    Reg::Write(new_value);
  }

  /**
   * 先读出旧值，后将寄存器的值替换为指定值
   * @param value 新值
   * @return RegInfo::DataType 由寄存器规定的数据类型
   */
  static __always_inline auto ReadWrite(typename RegInfo::DataType value) ->
      typename RegInfo::DataType {
    auto org_value = Reg::Read();
    auto new_value = (org_value & ~RegInfo::kBitMask) |
                     ((value << RegInfo::kBitOffset) & RegInfo::kBitMask);
    Reg::Write(new_value);
    return static_cast<typename RegInfo::DataType>(
        (org_value & RegInfo::kBitMask) >> RegInfo::kBitOffset);
  }
};

};  // namespace read_write

// 第三部分：寄存器实例
namespace regs {
struct X29 : public read_write::ReadWriteRegBase<register_info::X29Info> {};

namespace system_reg {

struct CPACR_EL1 : public read_write::ReadWriteRegBase<
                       register_info::system_reg::CPACR_EL1Info> {
  using Fpen = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::CPACR_EL1Info>,
      register_info::system_reg::CPACR_EL1Info::Fpen>;
};

struct CurrentEL : public read_write::ReadOnlyRegBase<
                       register_info::system_reg::CurrentELInfo> {
  using EL = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::CurrentELInfo>,
      register_info::system_reg::CurrentELInfo::EL>;
};

struct SPSel : public read_write::ReadWriteRegBase<
                   register_info::system_reg::SPSelInfo> {
  using SP = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::SPSelInfo>,
      register_info::system_reg::SPSelInfo::SP>;
};

struct DAIF
    : public read_write::ReadWriteRegBase<register_info::system_reg::DAIFInfo> {
  using D = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::DAIFInfo>,
      register_info::system_reg::DAIFInfo::D>;

  using A = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::DAIFInfo>,
      register_info::system_reg::DAIFInfo::A>;

  using I = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::DAIFInfo>,
      register_info::system_reg::DAIFInfo::I>;

  using F = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::DAIFInfo>,
      register_info::system_reg::DAIFInfo::F>;
};

struct VBAR_EL1 : public read_write::ReadWriteRegBase<
                      register_info::system_reg::VBAR_EL1Info> {
  using Base = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::VBAR_EL1Info>,
      register_info::system_reg::VBAR_EL1Info::Base>;
};

struct ELR_EL1 : public read_write::ReadWriteRegBase<
                     register_info::system_reg::ELR_EL1Info> {};

struct SPSR_EL1 : public read_write::ReadWriteRegBase<
                      register_info::system_reg::SPSR_EL1Info> {};

struct SP_EL0 : public read_write::ReadWriteRegBase<
                    register_info::system_reg::SP_EL0Info> {};

struct SP_EL1 : public read_write::ReadWriteRegBase<
                    register_info::system_reg::SP_EL1Info> {};

struct MPIDR_EL1 : public read_write::ReadOnlyRegBase<
                       register_info::system_reg::MPIDR_EL1Info> {
  using Aff3 = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::MPIDR_EL1Info>,
      register_info::system_reg::MPIDR_EL1Info::Aff3>;
  using U = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::MPIDR_EL1Info>,
      register_info::system_reg::MPIDR_EL1Info::U>;
  using MT = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::MPIDR_EL1Info>,
      register_info::system_reg::MPIDR_EL1Info::MT>;
  using Aff2 = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::MPIDR_EL1Info>,
      register_info::system_reg::MPIDR_EL1Info::Aff2>;
  using Aff1 = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::MPIDR_EL1Info>,
      register_info::system_reg::MPIDR_EL1Info::Aff1>;
  using Aff0 = read_write::ReadOnlyField<
      read_write::ReadOnlyRegBase<register_info::system_reg::MPIDR_EL1Info>,
      register_info::system_reg::MPIDR_EL1Info::Aff0>;
};

struct SCTLR_EL1 : public read_write::ReadWriteRegBase<
                       register_info::system_reg::SCTLR_EL1Info> {
  using M = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::SCTLR_EL1Info>,
      register_info::system_reg::SCTLR_EL1Info::M>;
};

struct MAIR_EL1 : public read_write::ReadWriteRegBase<
                      register_info::system_reg::MAIR_EL1Info> {
  using Aff7 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff7>;

  using Aff6 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff6>;

  using Aff5 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff5>;

  using Aff4 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff4>;

  using Aff3 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff3>;

  using Aff2 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff2>;

  using Aff1 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff1>;

  using Aff0 = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::system_reg::MAIR_EL1Info>,
      register_info::system_reg::MAIR_EL1Info::Aff0>;
};

};  // namespace system_reg

};  // namespace regs

};  // namespace detail

// 第四部分：访问接口
using X29 = detail::regs::X29;
using CPACR_EL1 = detail::regs::system_reg::CPACR_EL1;
using CurrentEL = detail::regs::system_reg::CurrentEL;
using SPSel = detail::regs::system_reg::SPSel;
using DAIF = detail::regs::system_reg::DAIF;
using VBAR_EL1 = detail::regs::system_reg::VBAR_EL1;
using ELR_EL1 = detail::regs::system_reg::ELR_EL1;
using SPSR_EL1 = detail::regs::system_reg::SPSR_EL1;
using SP_EL0 = detail::regs::system_reg::SP_EL0;
using SP_EL1 = detail::regs::system_reg::SP_EL1;
using MPIDR_EL1 = detail::regs::system_reg::MPIDR_EL1;
using SCTLR_EL1 = detail::regs::system_reg::SCTLR_EL1;
using MAIR_EL1 = detail::regs::system_reg::MAIR_EL1;

};  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_REGS_HPP_
