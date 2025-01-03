
/**
 * @file regs.hpp
 * @brief riscv64 寄存器相关定义
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

#ifndef CPU_IO_INCLUDE_RISCV64_REGS_HPP_
#define CPU_IO_INCLUDE_RISCV64_REGS_HPP_

#include <sys/cdefs.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

/**
 * riscv64 cpu Control and Status Registers 相关定义
 * @note 寄存器读写设计见 arch/README.md
 * @see priv-isa.pdf
 * https://github.com/riscv/riscv-isa-manual/releases/tag/20240411/priv-isa-asciidoc.pdf
 * @see riscv-abi.pdf
 * https://github.com/riscv-non-isa/riscv-elf-psabi-doc/releases/tag/v1.0
 * @see
 * https://github.com/five-embeddev/riscv-scratchpad/blob/master/baremetal-startup-cxx/src/riscv-csr.hpp
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
struct FpInfo : public RegInfoBase {};

namespace csr {
/// 立即数掩码，大于这个值需要使用寄存器中转
static constexpr uint64_t kCsrImmOpMask = 0x1F;

/**
 * @brief sstatus 寄存器定义
 * @see priv-isa.pdf#10.1.1
 */
struct SstatusInfo : public RegInfoBase {
  struct Sie {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 1;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Spie {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 5;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Spp {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 8;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief stvec 寄存器定义
 * @see priv-isa.pdf#10.1.2
 */
struct StvecInfo : public RegInfoBase {
  /// 中断模式 直接
  static constexpr const uint64_t kDirect = 0x0;
  /// 中断模式 向量
  static constexpr const uint64_t kVectored = 0x1;

  struct Mode {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 2;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Base {
    using DataType = uint64_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 62;
    static constexpr uint64_t kBitMask = ~0x3;
    static constexpr uint64_t kAllSetMask = ~0x3;
  };
};

/**
 * @brief sip 寄存器定义
 * @see priv-isa.pdf#10.1.3
 */
struct SipInfo : public RegInfoBase {
  struct Ssip {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 1;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Stip {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 5;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Seip {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 9;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief sip 寄存器定义
 * @see priv-isa.pdf#10.1.3
 */
struct SieInfo : public RegInfoBase {
  struct Ssie {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 1;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Stie {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 5;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Seie {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 9;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief time 寄存器定义
 * @see priv-isa.pdf#10.1.4
 */
struct TimeInfo : public RegInfoBase {};

/**
 * @brief cycle 寄存器定义
 * @see priv-isa.pdf#10.1.4
 */
struct CycleInfo : public RegInfoBase {};

/**
 * @brief instret 寄存器定义
 * @see priv-isa.pdf#10.1.4
 */
struct InstretInfo : public RegInfoBase {};

/**
 * @brief sscratch 寄存器定义
 * @see priv-isa.pdf#10.1.6
 */
struct SscratchInfo : public RegInfoBase {};

/**
 * @brief sepc 寄存器定义
 * @see priv-isa.pdf#10.1.7
 */
struct SepcInfo : public RegInfoBase {};

/**
 * @brief scause 寄存器定义
 * @see priv-isa.pdf#10.1.8
 */
struct ScauseInfo : public RegInfoBase {
  enum {
    // 中断
    kInterrupt = 1ULL << 63,
    kSupervisorSoftwareInterrupt = kInterrupt + 1,
    kSupervisorTimerInterrupt = kInterrupt + 5,
    kSupervisorExternalInterrupt = kInterrupt + 9,
    kCounterOverflowInterrupt = kInterrupt + 13,

    // 异常
    kInstructionAddressMisaligned = 0,
    kInstructionAccessFault = 1,
    kIllegalInstruction = 2,
    kBreakpoint = 3,
    kLoadAddressMisaligned = 4,
    kLoadAccessFault = 5,
    kStoreAmoAddressMisaligned = 6,
    kStoreAmoAccessFault = 7,
    kEcallUserMode = 8,
    kEcallSuperMode = 9,
    kReserved4 = 10,
    kEcallMachineMode = 11,
    kInstructionPageFault = 12,
    kLoadPageFault = 13,
    kReserved5 = 14,
    kStoreAmoPageFault = 15,
    kSoftwareCheck = 18,
    kHardwareError = 19,
  };

  /// 最大中断数
  static constexpr const uint32_t kInterruptMaxCount = 16;

  /// 中断名
  static constexpr std::array<const char *, kInterruptMaxCount>
      kInterruptNames = {
          "Reserved", "Supervisor Software Interrupt", "Reserved", "Reserved",
          "Reserved", "Supervisor Timer Interrupt",    "Reserved", "Reserved",
          "Reserved", "Supervisor External Interrupt", "Reserved", "Reserved",
          "Reserved", "Counter-overflow Interrupt",    "Reserved", "Reserved",
      };

  /// 最大异常数
  static constexpr const uint32_t kExceptionMaxCount = 20;

  /// 异常名
  static constexpr std::array<const char *, kExceptionMaxCount>
      kExceptionNames = {
          "Instruction Address Misaligned",
          "Instruction Access Fault",
          "Illegal Instruction",
          "Breakpoint",
          "Load Address Misaligned",
          "Load Access Fault",
          "Store/AMO Address Misaligned",
          "Store/AMO Access Fault",
          "Environment Call from U-mode",
          "Environment Call from S-mode",
          "Reserved",
          "Reserved",
          "Instruction Page Fault",
          "Load Page Fault",
          "Reserved",
          "Store/AMO Page Fault",
          "Reserved",
          "Reserved",
          "SoftwareCheck",
          "HardwareError",
      };

  struct ExceptionCode {
    using DataType = uint64_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 63;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Interrupt {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 63;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief stval 寄存器定义
 * @see priv-isa.pdf#10.1.9
 */
struct StvalInfo : public RegInfoBase {};

/**
 * @brief satp 寄存器定义
 * @see priv-isa.pdf#10.1.11
 */
struct SatpInfo : public RegInfoBase {
  enum : uint8_t {
    kBare = 0,
    kSv39 = 8,
    kSv48 = 9,
    kSv57 = 10,
    kSv64 = 11,
  };

  /// 最大内存模式数
  static constexpr const uint32_t kModeMaxCount = 16;

  static constexpr std::array<const char *, kModeMaxCount> kModeNames = {
      "Bare",     "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
      "Reserved", "Reserved", "SV39",     "SV48",     "SV57",     "SV64",
  };

  struct Ppn {
    using DataType = uint64_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 44;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
  struct Asid {
    using DataType = uint16_t;
    static constexpr uint64_t kBitOffset = 44;
    static constexpr uint64_t kBitWidth = 16;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
  struct Mode {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 60;
    static constexpr uint64_t kBitWidth = 4;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief stimecmp 寄存器定义
 * @see priv-isa.pdf#16.1.1
 */
struct StimecmpInfo : public RegInfoBase {};

};  // namespace csr

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
    if constexpr (std::is_same_v<RegInfo, register_info::FpInfo>) {
      __asm__ volatile("mv %0, fp" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrr %0, sstatus" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrr %0, stvec" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrr %0, sip" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrr %0, sie" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::TimeInfo>) {
      __asm__ volatile("csrr %0, time" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::CycleInfo>) {
      __asm__ volatile("csrr %0, cycle" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::InstretInfo>) {
      __asm__ volatile("csrr %0, instret" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrr %0, sscratch" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrr %0, sepc" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrr %0, scause" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrr %0, stval" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrr %0, satp" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StimecmpInfo>) {
      __asm__ volatile("csrr %0, stimecmp" : "=r"(value) : :);
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
    if constexpr (std::is_same_v<RegInfo, register_info::FpInfo>) {
      __asm__ volatile("mv fp, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrw sstatus, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrw stvec, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrw sip, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrw sie, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrw sscratch, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrw sepc, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrw scause, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrw stval, %0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrw satp, %0" : : "r"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 写 csr 寄存器，不通过寄存器中转
   * @param value 要写的值
   * @note 只能写 kCsrImmOpMask 范围内的值
   */
  static __always_inline void WriteImm(const uint8_t value) {
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrwi sstatus, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrwi stvec, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrwi sip, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrwi sie, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrwi sscratch, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrwi sepc, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrwi scause, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrwi stval, %0" : : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrwi satp, %0" : : "i"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 通过掩码设置寄存器
   * @param mask 掩码
   */
  static __always_inline void SetBits(uint64_t mask) {
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrs zero, sstatus, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrs zero, stvec, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrs zero, sip, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrs zero, sie, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrs zero, sscratch, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrs zero, sepc, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrs zero, scause, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrs zero, stval, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrs zero, satp, %0" : : "r"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 清零寄存器
   * @param mask 掩码
   */
  static __always_inline void ClearBits(uint64_t mask) {
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrc zero, sstatus, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrc zero, stvec, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrc zero, sip, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrc zero, sie, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrc zero, sscratch, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrc zero, sepc, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrc zero, scause, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrc zero, stval, %0" : : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrc zero, satp, %0" : : "r"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 通过掩码设置寄存器，不通过寄存器中转
   * @param mask 掩码
   * @note 只能写 kCsrImmOpMask 范围内的值
   */
  static __always_inline void SetBitsImm(const uint8_t mask) {
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrsi zero, sstatus, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrsi zero, stvec, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrsi zero, sip, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrsi zero, sie, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrsi zero, sscratch, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrsi zero, sepc, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrsi zero, scause, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrsi zero, stval, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrsi zero, satp, %0" : : "i"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 清零寄存器，不通过寄存器中转
   * @param mask 掩码
   * @note 只能写 kCsrImmOpMask 范围内的值
   */
  static __always_inline void ClearBitsImm(const uint8_t mask) {
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrci zero, sstatus, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrci zero, stvec, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrci zero, sip, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrci zero, sie, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrci zero, sscratch, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrci zero, sepc, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrci zero, scause, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrci zero, stval, %0" : : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrci zero, satp, %0" : : "i"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 向寄存器写常数
   * @tparam value 常数的值
   */
  template <uint64_t value>
  static void WriteConst() {
    if constexpr ((value & register_info::csr::kCsrImmOpMask) == value) {
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
  static void SetConst() {
    if constexpr ((mask & register_info::csr::kCsrImmOpMask) == mask) {
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
  static void ClearConst() {
    if constexpr ((mask & register_info::csr::kCsrImmOpMask) == mask) {
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
    typename RegInfo::DataType old_value{};
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrw %0, sstatus, %1"
                       : "=r"(old_value)
                       : "r"(value)
                       :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrw %0, stvec, %1" : "=r"(old_value) : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrw %0, sip, %1" : "=r"(old_value) : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrw %0, sie, %1" : "=r"(old_value) : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrw %0, sscratch, %1"
                       : "=r"(old_value)
                       : "r"(value)
                       :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrw %0, sepc, %1" : "=r"(old_value) : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrw %0, scause, %1" : "=r"(old_value) : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrw %0, stval, %1" : "=r"(old_value) : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrw %0, satp, %1" : "=r"(old_value) : "r"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return old_value;
  }

  /**
   * 先读后写寄存器，不通过寄存器中转
   * @param value 要写的值
   * @return RegInfo::DataType 寄存器的值
   * @note 只能写 kCsrImmOpMask 范围内的值
   */
  static __always_inline auto ReadWriteImm(const uint8_t value) ->
      typename RegInfo::DataType {
    typename RegInfo::DataType old_value{};
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrwi %0, sstatus, %1"
                       : "=r"(old_value)
                       : "i"(value)
                       :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrwi %0, stvec, %1" : "=r"(old_value) : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrwi %0, sip, %1" : "=r"(old_value) : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrwi %0, sie, %1" : "=r"(old_value) : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrwi %0, sscratch, %1"
                       : "=r"(old_value)
                       : "i"(value)
                       :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrwi %0, sepc, %1" : "=r"(old_value) : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrwi %0, scause, %1"
                       : "=r"(old_value)
                       : "i"(value)
                       :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrwi %0, stval, %1" : "=r"(old_value) : "i"(value) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrwi %0, satp, %1" : "=r"(old_value) : "i"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return old_value;
  }

  /**
   * 先读后写常数到寄存器
   * @tparam value 要写的值
   * @return RegInfo::DataType 寄存器的值
   */
  template <uint64_t value>
  static auto ReadWriteConst() -> typename RegInfo::DataType {
    if constexpr ((value & register_info::csr::kCsrImmOpMask) == value) {
      return ReadWriteRegBase<RegInfo>::ReadWriteImm(value);
    } else {
      return ReadWrite(value);
    }
  }

  /**
   * 先读后通过掩码设置寄存器
   * @param mask 掩码
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadSetBits(uint64_t mask) ->
      typename RegInfo::DataType {
    typename RegInfo::DataType value{};
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrs %0, sstatus, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrs %0, stvec, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrs %0, sip, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrs %0, sie, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrs %0, sscratch, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrs %0, sepc, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrs %0, scause, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrs %0, stval, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrs %0, satp, %1" : "=r"(value) : "r"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return value;
  }

  /**
   * 先读后通过掩码设置寄存器，不通过寄存器中转
   * @param mask 掩码
   * @note 只能写 kCsrImmOpMask 范围内的值
   */
  static __always_inline auto ReadSetBitsImm(const uint8_t mask) ->
      typename RegInfo::DataType {
    typename RegInfo::DataType value{};
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrsi %0, sstatus, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrsi %0, stvec, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrsi %0, sip, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrsi %0, sie, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrsi %0, sscratch, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrsi %0, sepc, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrsi %0, scause, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrsi %0, stval, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrsi %0, satp, %1" : "=r"(value) : "i"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return value;
  }

  /**
   * 通过常数掩码先读后写寄存器
   * @tparam mask 掩码
   * @return RegInfo::DataType 寄存器的值
   */
  template <uint64_t mask>
  static auto ReadSetBitsConst() -> typename RegInfo::DataType {
    if constexpr ((mask & register_info::csr::kCsrImmOpMask) == mask) {
      return ReadSetBitsImm(mask);
    } else {
      return ReadSetBits(mask);
    }
  }

  /**
   * 先读后清零寄存器
   * @param mask 掩码
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadClearBits(uint64_t mask) ->
      typename RegInfo::DataType {
    typename RegInfo::DataType value{};
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrc %0, sstatus, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrc %0, stvec, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrc %0, sip, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrc %0, sie, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrc %0, sscratch, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrc %0, sepc, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrc %0, scause, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrc %0, stval, %1" : "=r"(value) : "r"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrc %0, satp, %1" : "=r"(value) : "r"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return value;
  }

  /**
   * 先读后清零寄存器，不通过寄存器中转
   * @param mask 掩码
   * @note 只能写 kCsrImmOpMask 范围内的值
   */
  static __always_inline auto ReadClearBitsImm(const uint8_t mask) ->
      typename RegInfo::DataType {
    typename RegInfo::DataType value{};
    if constexpr (std::is_same_v<RegInfo, register_info::csr::SstatusInfo>) {
      __asm__ volatile("csrrci %0, sstatus, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvecInfo>) {
      __asm__ volatile("csrrci %0, stvec, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SipInfo>) {
      __asm__ volatile("csrrci %0, sip, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::csr::SieInfo>) {
      __asm__ volatile("csrrci %0, sie, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SscratchInfo>) {
      __asm__ volatile("csrrci %0, sscratch, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SepcInfo>) {
      __asm__ volatile("csrrci %0, sepc, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::ScauseInfo>) {
      __asm__ volatile("csrrci %0, scause, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::StvalInfo>) {
      __asm__ volatile("csrrci %0, stval, %1" : "=r"(value) : "i"(mask) :);
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::csr::SatpInfo>) {
      __asm__ volatile("csrrci %0, satp, %1" : "=r"(value) : "i"(mask) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return value;
  }

  /**
   * 通过常数掩码先读后清零寄存器
   * @tparam mask 掩码
   * @return RegInfo::DataType 寄存器的值
   */
  template <uint64_t mask>
  static auto ReadClearBitsConst() -> typename RegInfo::DataType {
    if constexpr ((mask & register_info::csr::kCsrImmOpMask) == mask) {
      return WriteOnlyRegBase<RegInfo>::ReadClearBitsImm(mask);
    } else {
      return ReadClearBits(mask);
    }
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
  static __always_inline auto Get(typename RegInfo::DataType value) ->
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
    if constexpr ((RegInfo::kBitMask & register_info::csr::kCsrImmOpMask) ==
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
    if constexpr ((RegInfo::kBitMask & register_info::csr::kCsrImmOpMask) ==
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
    auto new_value =
        (org_value & ~RegInfo::kBitMask) |
        ((static_cast<decltype(org_value)>(value) << RegInfo::kBitOffset) &
         RegInfo::kBitMask);
    Reg::Write(new_value);
    return static_cast<typename RegInfo::DataType>(
        (org_value & RegInfo::kBitMask) >> RegInfo::kBitOffset);
  }
};

};  // namespace read_write

// 第三部分：寄存器实例
namespace regs {
class Fp : public read_write::ReadWriteRegBase<register_info::FpInfo> {};

namespace csr {

class Sstatus
    : public read_write::ReadWriteRegBase<register_info::csr::SstatusInfo> {
 public:
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SstatusInfo>,
      register_info::csr::SstatusInfo::Sie>
      sie;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SstatusInfo>,
      register_info::csr::SstatusInfo::Spie>
      spie;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SstatusInfo>,
      register_info::csr::SstatusInfo::Spp>
      spp;

  /// @name 构造/析构函数
  /// @{
  Sstatus() = default;
  Sstatus(const Sstatus &) = delete;
  Sstatus(Sstatus &&) = delete;
  auto operator=(const Sstatus &) -> Sstatus & = delete;
  auto operator=(Sstatus &&) -> Sstatus & = delete;
  virtual ~Sstatus() = default;
  /// @}
};

class Stvec
    : public read_write::ReadWriteRegBase<register_info::csr::StvecInfo> {
 public:
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::StvecInfo>,
      register_info::csr::StvecInfo::Base>
      base;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::StvecInfo>,
      register_info::csr::StvecInfo::Mode>
      mode;

  static void SetDirect(uint64_t addr) {
    read_write::ReadWriteField<
        read_write::ReadWriteRegBase<register_info::csr::StvecInfo>,
        register_info::csr::StvecInfo::Base>::Write(addr);
    read_write::ReadWriteField<
        read_write::ReadWriteRegBase<register_info::csr::StvecInfo>,
        register_info::csr::StvecInfo::Mode>::
        Write(register_info::csr::StvecInfo::kDirect);
  }

  /// @name 构造/析构函数
  /// @{
  Stvec() = default;
  Stvec(const Stvec &) = delete;
  Stvec(Stvec &&) = delete;
  auto operator=(const Stvec &) -> Stvec & = delete;
  auto operator=(Stvec &&) -> Stvec & = delete;
  virtual ~Stvec() = default;
  /// @}
};

class Sip : public read_write::ReadWriteRegBase<register_info::csr::SipInfo> {
 public:
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SipInfo>,
      register_info::csr::SipInfo::Ssip>
      ssip;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SipInfo>,
      register_info::csr::SipInfo::Stip>
      stip;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SipInfo>,
      register_info::csr::SipInfo::Seip>
      seip;

  /// @name 构造/析构函数
  /// @{
  Sip() = default;
  Sip(const Sip &) = delete;
  Sip(Sip &&) = delete;
  auto operator=(const Sip &) -> Sip & = delete;
  auto operator=(Sip &&) -> Sip & = delete;
  virtual ~Sip() = default;
  /// @}
};

class Sie : public read_write::ReadWriteRegBase<register_info::csr::SieInfo> {
 public:
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SieInfo>,
      register_info::csr::SieInfo::Ssie>
      ssie;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SieInfo>,
      register_info::csr::SieInfo::Stie>
      stie;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SieInfo>,
      register_info::csr::SieInfo::Seie>
      seie;

  /// @name 构造/析构函数
  /// @{
  Sie() = default;
  Sie(const Sie &) = delete;
  Sie(Sie &&) = delete;
  auto operator=(const Sie &) -> Sie & = delete;
  auto operator=(Sie &&) -> Sie & = delete;
  virtual ~Sie() = default;
  /// @}
};

class Time : public read_write::ReadOnlyRegBase<register_info::csr::TimeInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Time() = default;
  Time(const Time &) = delete;
  Time(Time &&) = delete;
  auto operator=(const Time &) -> Time & = delete;
  auto operator=(Time &&) -> Time & = delete;
  virtual ~Time() = default;
  /// @}
};

class Cycle
    : public read_write::ReadOnlyRegBase<register_info::csr::CycleInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Cycle() = default;
  Cycle(const Cycle &) = delete;
  Cycle(Cycle &&) = delete;
  auto operator=(const Cycle &) -> Cycle & = delete;
  auto operator=(Cycle &&) -> Cycle & = delete;
  virtual ~Cycle() = default;
  /// @}
};

class Instret
    : public read_write::ReadOnlyRegBase<register_info::csr::InstretInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Instret() = default;
  Instret(const Instret &) = delete;
  Instret(Instret &&) = delete;
  auto operator=(const Instret &) -> Instret & = delete;
  auto operator=(Instret &&) -> Instret & = delete;
  virtual ~Instret() = default;
  /// @}
};

class Sscratch
    : public read_write::ReadWriteRegBase<register_info::csr::SscratchInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Sscratch() = default;
  Sscratch(const Sscratch &) = delete;
  Sscratch(Sscratch &&) = delete;
  auto operator=(const Sscratch &) -> Sscratch & = delete;
  auto operator=(Sscratch &&) -> Sscratch & = delete;
  virtual ~Sscratch() = default;
  /// @}
};

class Sepc : public read_write::ReadWriteRegBase<register_info::csr::SepcInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Sepc() = default;
  Sepc(const Sepc &) = delete;
  Sepc(Sepc &&) = delete;
  auto operator=(const Sepc &) -> Sepc & = delete;
  auto operator=(Sepc &&) -> Sepc & = delete;
  virtual ~Sepc() = default;
  /// @}
};

class Scause
    : public read_write::ReadWriteRegBase<register_info::csr::ScauseInfo> {
 public:
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::ScauseInfo>,
      register_info::csr::ScauseInfo::Interrupt>
      interrupt;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::ScauseInfo>,
      register_info::csr::ScauseInfo::ExceptionCode>
      exception_code;

  /// @name 构造/析构函数
  /// @{
  Scause() = default;
  Scause(const Scause &) = delete;
  Scause(Scause &&) = delete;
  auto operator=(const Scause &) -> Scause & = delete;
  auto operator=(Scause &&) -> Scause & = delete;
  virtual ~Scause() = default;
  /// @}
};

class Stval
    : public read_write::ReadWriteRegBase<register_info::csr::StvalInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Stval() = default;
  Stval(const Stval &) = delete;
  Stval(Stval &&) = delete;
  auto operator=(const Stval &) -> Stval & = delete;
  auto operator=(Stval &&) -> Stval & = delete;
  virtual ~Stval() = default;
  /// @}
};

class Satp : public read_write::ReadWriteRegBase<register_info::csr::SatpInfo> {
 public:
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SatpInfo>,
      register_info::csr::SatpInfo::Ppn>
      ppn;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SatpInfo>,
      register_info::csr::SatpInfo::Asid>
      asid;
  read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::csr::SatpInfo>,
      register_info::csr::SatpInfo::Mode>
      mode;

  /// @name 构造/析构函数
  /// @{
  Satp() = default;
  Satp(const Satp &) = delete;
  Satp(Satp &&) = delete;
  auto operator=(const Satp &) -> Satp & = delete;
  auto operator=(Satp &&) -> Satp & = delete;
  virtual ~Satp() = default;
  /// @}
};

class Stimecmp
    : public read_write::ReadOnlyRegBase<register_info::csr::StimecmpInfo> {
 public:
  /// @name 构造/析构函数
  /// @{
  Stimecmp() = default;
  Stimecmp(const Stimecmp &) = delete;
  Stimecmp(Stimecmp &&) = delete;
  auto operator=(const Stimecmp &) -> Stimecmp & = delete;
  auto operator=(Stimecmp &&) -> Stimecmp & = delete;
  virtual ~Stimecmp() = default;
  /// @}
};

};  // namespace csr

};  // namespace regs

};  // namespace detail

// 第四部分：访问接口
using Fp = detail::regs::Fp;
using Sstatus = detail::regs::csr::Sstatus;
using Stvec = detail::regs::csr::Stvec;
using Sip = detail::regs::csr::Sip;
using Sie = detail::regs::csr::Sie;
using Time = detail::regs::csr::Time;
using Cycle = detail::regs::csr::Cycle;
using Instret = detail::regs::csr::Instret;
using Sscratch = detail::regs::csr::Sscratch;
using Sepc = detail::regs::csr::Sepc;
using Scause = detail::regs::csr::Scause;
using Stval = detail::regs::csr::Stval;
using Satp = detail::regs::csr::Satp;
using Stimecmp = detail::regs::csr::Stimecmp;

};  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_RISCV64_REGS_HPP_
