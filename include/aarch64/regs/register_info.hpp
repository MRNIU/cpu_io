/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_RISCV64_REGISTER_INFO_HPP_
#define CPU_IO_INCLUDE_RISCV64_REGISTER_INFO_HPP_

#include <array>
#include <cstdint>

#include "register_info_base.h"

namespace cpu_io {

namespace detail {

namespace register_info {

/// 通用寄存器
struct X0Info : public RegInfoBase {};
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
struct TCR_EL1Info : public RegInfoBase {
  struct IPS {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 32;
    static constexpr uint64_t kBitWidth = 3;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct TG1 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 30;
    static constexpr uint64_t kBitWidth = 2;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct T1SZ {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 16;
    static constexpr uint64_t kBitWidth = 6;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct TG0 {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 14;
    static constexpr uint64_t kBitWidth = 2;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct T0SZ {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 6;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

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

    // EL1&0 stage 1 address translation disabled.
    static constexpr const bool KDisabled = false;
    // EL1&0 stage 1 address translation enabled.
    static constexpr const bool kEnabled = true;
  };
};

/**
 * @brief ESR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-
 */
struct ESR_EL1Info : public RegInfoBase {
  struct ISS2 {
    using DataType = uint32_t;
    static constexpr uint64_t kBitOffset = 32;
    static constexpr uint64_t kBitWidth = 24;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct EC {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 26;
    static constexpr uint64_t kBitWidth = 6;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct ISS {
    using DataType = uint32_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 25;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief CNTV_CTL_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTV-CTL-EL0--Counter-timer-Virtual-Timer-Control-Register
 */
struct CNTV_CTL_EL0Info : public RegInfoBase {
  struct ISTATUS {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 2;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Timer condition is not met.
    static constexpr const bool KNotMet = false;
    /// Timer condition is met.
    static constexpr const bool KMet = true;
  };

  struct IMASK {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 1;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Timer interrupt is not masked by the IMASK bit.
    static constexpr const bool KNotMasked = false;
    /// Timer interrupt is masked by the IMASK bit.
    static constexpr const bool KMasked = true;
  };

  struct ENABLE {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Timer disabled.
    static constexpr const bool KDisable = false;
    /// Timer enabled.
    static constexpr const bool KEnable = true;
  };
};

/**
 * @brief CNTV_TVAL_EL0 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTV-TVAL-EL0--Counter-timer-Virtual-Timer-TimerValue-Register
 */
struct CNTV_TVAL_EL0Info : public RegInfoBase {
  struct TimerValue {
    using DataType = uint32_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 32;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

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
struct CNTFRQ_EL0Info : public RegInfoBase {
  using DataType = uint32_t;
};

/**
 * @brief ICC_PMR_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ICC-PMR-EL1--Interrupt-Controller-Interrupt-Priority-Mask-Register
 */
struct ICC_PMR_EL1Info : public RegInfoBase {
  struct Priority {
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
 * @brief ICC_IGRPEN1_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ICC-IGRPEN1-EL1--Interrupt-Controller-Interrupt-Group-1-Enable-Register
 */
struct ICC_IGRPEN1_EL1Info : public RegInfoBase {
  struct Enable {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Group 1 interrupts are disabled for the current Security state.
    static constexpr const bool kDisabel = false;

    /// Group 1 interrupts are enabled for the current Security state.
    static constexpr const bool kEnabel = true;
  };
};

/**
 * @brief ICC_SRE_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ICC-SRE-EL1--Interrupt-Controller-System-Register-Enable-Register--EL1-
 */
struct ICC_SRE_EL1Info : public RegInfoBase {
  struct DIB {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 2;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// IRQ bypass enabled.
    static constexpr const bool kEnabel = false;

    /// IRQ bypass disabled.
    static constexpr const bool kDisabel = true;
  };

  struct DFB {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 1;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// FIQ bypass enabled.
    static constexpr const bool kEnabel = false;

    /// FIQ bypass disabled.
    static constexpr const bool kDisabel = true;
  };

  struct SRE {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  /// The memory-mapped interface must be used. Access at EL1 to any ICC_*
  /// System register other than ICC_SRE_EL1 is trapped to EL1.
  static constexpr const bool kDisabel = false;

  /// The System register interface for the current Security state is enabled.
  static constexpr const bool kEnabel = true;
};

/**
 * @brief ICC_IAR1_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ICC-IAR1-EL1--Interrupt-Controller-Interrupt-Acknowledge-Register-1
 */
struct ICC_IAR1_EL1Info : public RegInfoBase {
  struct INTID {
    using DataType = uint32_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 24;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief ICC_EOIR1_EL1 寄存器定义
 * @see
 * https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/ICC-EOIR1-EL1--Interrupt-Controller-End-Of-Interrupt-Register-1
 */
struct ICC_EOIR1_EL1Info : public RegInfoBase {
  struct INTID {
    using DataType = uint32_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 24;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;

    /// Group 1 interrupts are disabled for the current Security state.
    static constexpr const bool kDisabel = false;

    /// Group 1 interrupts are enabled for the current Security state.
    static constexpr const bool kEnabel = true;
  };
};

}  // namespace system_reg

}  // namespace register_info

}  // namespace detail

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_RISCV64_REGISTER_INFO_HPP_
