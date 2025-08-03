/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_X86_64_REGS_HPP_
#define CPU_IO_INCLUDE_X86_64_REGS_HPP_

#include <sys/cdefs.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

/**
 * x86_64 cpu Control Registers 相关定义
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
struct RbpInfo : public RegInfoBase {};

/**
 * @brief efer 寄存器
 * @see sdm.pdf#2.2.1
 */
struct MsrInfo : public RegInfoBase {};

/**
 * @brief rflags 寄存器
 * @see sdm.pdf#2.3
 */
struct RflagsInfo : public RegInfoBase {
  struct If {
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
 * @brief 系统段与门描述符类型
 * @see sdm.pdf#3.5
 */
enum SystemSegmentAndGateDescriptorTypes : uint8_t {
  kReserved0 = 0,
  k16BitTssAvailable = 1,
  kLdt = 2,
  k16BitTssBusy = 3,
  k16BitCallGate = 4,
  kTaskGate = 5,
  k16BitInterruptGate = 6,
  k16BitTrapGate = 7,
  kReserved8 = 8,
  k32BitTssAvailable = 9,
  k64BitTssAvailable = k32BitTssAvailable,
  kReserved10 = 10,
  k32BitTssBusy = 11,
  k64BitTssBusy = k32BitTssBusy,
  k32BitCallGate = 12,
  k64BitCallGate = k32BitCallGate,
  kReserved13 = 13,
  k32BitInterruptGate = 14,
  k64BitInterruptGate = k32BitInterruptGate,
  k32BitTrapGate = 15,
  k64BitTrapGate = k32BitTrapGate,
};

/// 描述符权限级别
enum DescriptorDpl : uint8_t {
  kRing0 = 0,
  kRing1 = 1,
  kRing2 = 2,
  kRing3 = 3,
};

/// 描述符存在位
enum DescriptorP : uint8_t {
  kNotPresent = 0,
  kPresent = 1,
};

/**
 * @brief gdtr 寄存器
 * @see sdm.pdf#2.4.1
 * @see sdm.pdf#3.5.1
 * @see sdm.pdf#3.5.2
 */
struct GdtrInfo : public RegInfoBase {
  /**
   * 段描述符结构
   * @see sdm.pdf#3.4.5
   * @see sdm.pdf#5.2.1
   * @see sdm.pdf#5.3.1
   */
  class SegmentDescriptor {
   public:
    enum Type : uint8_t {
      kDataReadOnly = 0,
      kDataReadOnlyAccessed = 1,
      kDataReadWrite = 2,
      kDataReadWriteAccessed = 3,
      kDataReadOnlyExpandDown = 4,
      kDataReadOnlyExpandDownAccessed = 5,
      kDataReadWriteExpandDown = 6,
      kDataReadWriteExpandDownAccessed = 7,

      kCodeExecuteOnly = 8,
      kCodeExecuteOnlyAccessed = 9,
      kCodeExecuteRead = 10,
      kCodeExecuteReadAccessed = 11,
      kCodeExecuteOnlyConforming = 12,
      kCodeExecuteOnlyConformingAccessed = 13,
      kCodeExecuteReadConforming = 14,
      kCodeExecuteReadConformingAccessed = 15
    };

    enum S : uint8_t {
      kSystem = 0,
      kCodeData = 1,
    };

    using DPL = DescriptorDpl;
    using P = DescriptorP;

    enum AVL : uint8_t {
      kNotAvailable = 0,
      kAvailable = 1,
    };

    enum L : uint8_t {
      kLegacy = 0,
      k64Bit = 1,
    };

    union {
      struct {
        uint64_t limit_low : 16;
        uint64_t base_low : 16;
        uint64_t base_mid : 8;
        /// Segment type
        uint64_t type : 4;
        /// Descriptor type
        uint64_t s : 1;
        /// Specifies the privilege level of the segment
        uint64_t dpl : 2;
        /// Indicates whether the segment is present in memory (set) or not
        /// present (clear).
        uint64_t p : 1;
        uint64_t limit_high : 4;
        /// Available for use by system software
        uint64_t avl : 1;
        /// 64-bit code segment (IA-32e mode only)
        uint64_t l : 1;
        uint64_t db : 1;
        uint64_t g : 1;
        uint64_t base_high : 8;
      } segment_descriptor;
      uint64_t val;
    };

    explicit SegmentDescriptor(Type type, S s, DPL dpl, P p, AVL avl, L l)
        : val(0) {
      segment_descriptor.type = type;
      segment_descriptor.s = s;
      segment_descriptor.dpl = dpl;
      segment_descriptor.p = p;
      segment_descriptor.avl = avl;
      segment_descriptor.l = l;
    }

    /// @name 构造/析构函数
    /// @{
    SegmentDescriptor() = default;
    SegmentDescriptor(const SegmentDescriptor &) = default;
    SegmentDescriptor(SegmentDescriptor &&) = default;
    auto operator=(const SegmentDescriptor &) -> SegmentDescriptor & = default;
    auto operator=(SegmentDescriptor &&) -> SegmentDescriptor & = default;
    ~SegmentDescriptor() = default;
    /// @}
  };

  /// gdt 数量
  static constexpr const uint16_t kMaxCount = 5;
  /// 首个空描述符索引
  static constexpr const uint16_t kNullIndex = 0;
  /// 内核代码段描述符索引
  static constexpr const uint16_t kKernelCodeIndex = 1;
  /// 内核数据段描述符索引
  static constexpr const uint16_t kKernelDataIndex = 2;
  /// 用户代码段描述符索引
  static constexpr const uint16_t kUserCodeIndex = 3;
  /// 用户数据段描述符索引
  static constexpr const uint16_t kUserDataIndex = 4;

  struct Gdtr {
    /// 全局描述符表限长
    uint16_t limit;
    /// 全局描述符表基地址
    SegmentDescriptor *base;
  } __attribute__((packed));

  using DataType = Gdtr;

  struct Limit {
    using DataType = uint16_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 16;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Base {
    using DataType = SegmentDescriptor *;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 64;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief ldtr 寄存器
 * @see sdm.pdf#2.4.2
 * @see sdm.pdf#6.3.1
 */
struct LdtrInfo : public RegInfoBase {};

/**
 * @brief idtr 寄存器
 * @see sdm.pdf#2.4.3
 */
struct IdtrInfo : public RegInfoBase {
  /// 最大中断数
  static constexpr const uint32_t kInterruptMaxCount = 256;

  /// 中断号
  /// @see sdm.pdf#6.3.1
  enum {
    kDivideError = 0,
    kDebugException = 1,
    kNmiInterrupt = 2,
    kBreakpoint = 3,
    kOverflow = 4,
    kBoundRangeExceeded = 5,
    kInvalidOpcode = 6,
    kDeviceNotAvailable = 7,
    kDoubleFault = 8,
    // 386 后不再使用
    kCoprocessorSegmentOverrun = 9,
    kInvalidTss = 10,
    kSegmentNotPresent = 11,
    kStackSegmentFault = 12,
    kGeneralProtection = 13,
    kPageFault = 14,
    kX87FpuFloatingPointError = 16,
    kAlignmentCheck = 17,
    kMachineCheck = 18,
    kSIMDFloatingPointException = 19,
    kVirtualizationException = 20,
    ControlProtectionException = 21,

    /// @todo
    /// 电脑系统计时器
    kIrq0 = 32,
    /// 键盘
    kIrq1 = 33,
    /// 与 IRQ9 相接，MPU-401 MD 使用
    kIrq2 = 34,
    /// 串口设备
    kIrq3 = 35,
    /// 串口设备
    kIrq4 = 36,
    /// 建议声卡使用
    kIrq5 = 37,
    /// 软驱传输控制使用
    kIrq6 = 38,
    /// 打印机传输控制使用
    kIrq7 = 39,
    /// 即时时钟
    kIrq8 = 40,
    /// 与 IRQ2 相接，可设定给其他硬件
    kIrq9 = 41,
    /// 建议网卡使用
    kIrq10 = 42,
    /// 建议 AGP 显卡使用
    kIrq11 = 43,
    /// 接 PS/2 鼠标，也可设定给其他硬件
    kIrq12 = 44,
    /// 协处理器使用
    kIrq13 = 45,
    /// SATA 主硬盘
    kIrq14 = 46,
    /// SATA 从硬盘
    kIrq15 = 47,
    /// 系统调用
    kIrq128 = 128,
  };

  /// 中断名
  static constexpr std::array<const char *, kInterruptMaxCount>
      kInterruptNames = {
          "Divide Error",
          "Debug Exception",
          "NMI Interrupt",
          "Breakpoint",
          "Overflow",
          "BOUND Range Exceeded",
          "Invalid Opcode (Undefined Opcode)",
          "Device Not Available (No Math Coprocessor)",
          "Double Fault",
          "Coprocessor Segment Overrun (reserved)",
          "Invalid TSS",
          "Segment Not Present",
          "Stack-Segment Fault",
          "General Protection",
          "Page Fault",
          "(Intel reserved. Do not use.)",
          "x87 FPU Floating-Point Error (Math Fault)",
          "Alignment Check",
          "Machine Check",
          "SIMD Floating-Point Exception",
          "Virtualization Exception",
          "Control Protection Exception",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Reserved",
          "Irq0",
          "Irq1",
          "Irq2",
          "Irq3",
          "Irq4",
          "Irq5",
          "Irq6",
          "Irq7",
          "Irq8",
          "Irq9",
          "Irq10",
          "Irq11",
          "Irq12",
          "Irq13",
          "Irq14",
          "Irq15",
      };

  /**
   * @brief 错误码结构
   * @see sdm.pdf#6.13
   */
  struct ErrorCode {
    union {
      struct {
        // When set, indicates that the exception occurred during
        // delivery of an event external to the program, such as an interrupt or
        // an earlier exception.1 The bit is cleared if the exception occurred
        // during delivery of a software interrupt (INT n, INT3, or INTO).
        uint32_t ext : 1;
        // When set, indicates that the index portion of the error code refers
        // to a gate descriptor in the IDT; when clear, indicates that the index
        // refers to a descriptor in the GDT or the current LDT.
        uint32_t idt : 1;
        // Only used when the IDT flag is clear. When set, the TI flag indicates
        // that the index portion of the error code refers to a segment or gate
        // descriptor in the LDT; when clear, it indi- cates that the index
        // refers to a descriptor in the current GDT.
        uint32_t ti : 1;
        // The segment selector index field provides an index into the IDT, GDT,
        // or current LDT to the segment or gate selector being referenced by
        // the error code.
        uint32_t segment_selector_index : 29;
      } error_code;
      uint32_t val;
    };
  };

  /**
   * @brief idt 结构
   * @see sdm.pdf#6.14.1
   */
  class Idt {
   public:
    using Type = SystemSegmentAndGateDescriptorTypes;
    using DPL = DescriptorDpl;
    using P = DescriptorP;

    union {
      struct {
        // 低位地址
        uint64_t offset1 : 16;
        // 段选择子
        uint64_t selector : 16;
        // 中断栈表
        uint64_t ist : 3;
        // 填充 0
        uint64_t zero0 : 5;
        // 类型
        uint64_t type : 4;
        // 填充 0
        uint64_t zero1 : 1;
        // 权限
        uint64_t dpl : 2;
        // 存在位
        uint64_t p : 1;
        // 中段地址
        uint64_t offset2 : 16;
        // 高位地址
        uint64_t offset3 : 32;
        // 保留
        uint64_t reserved : 32;
      } idt;
      std::array<uint64_t, 2> val;
    };

    explicit Idt(uint64_t base, uint16_t selector, uint8_t ist, uint8_t type,
                 uint8_t dpl, uint8_t p)
        : val{} {
      idt.offset1 = base & 0xFFFF;
      idt.selector = selector;
      idt.ist = ist;
      idt.type = type;
      idt.dpl = dpl;
      idt.p = p;
      idt.offset2 = (base >> 16) & 0xFFFF;
      idt.offset3 = base >> 32;
    }

    /// @name 构造/析构函数
    /// @{
    Idt() = default;
    Idt(const Idt &) = default;
    Idt(Idt &&) = default;
    auto operator=(const Idt &) -> Idt & = default;
    auto operator=(Idt &&) -> Idt & = default;
    ~Idt() = default;
    /// @}
  };

  /**
   * @brief idtr 结构
   * @see sdm.pdf#2.4
   */
  struct Idtr {
    uint16_t limit;
    Idt *base;
  } __attribute__((packed));

  using DataType = Idtr;
  static constexpr uint64_t kBitWidth = 80;

  struct Limit {
    using DataType = uint16_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 16;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Base {
    using DataType = uint64_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 64;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/**
 * @brief tr 寄存器
 * @see sdm.pdf#2.4.4
 */
struct TrInfo : public RegInfoBase {};

/**
 * @brief cr 寄存器
 * @see sdm.pdf#2.5
 */
namespace cr {

struct Cr0Info : public RegInfoBase {
  struct Pe {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Pg {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 31;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

struct Cr2Info : public RegInfoBase {};

struct Cr3Info : public RegInfoBase {
  struct Pwt {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 3;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Pcd {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 4;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct PageDirectoryBase {
    using DataType = uint64_t;
    static constexpr uint64_t kBitOffset = 12;
    static constexpr uint64_t kBitWidth = 52;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

struct Cr4Info : public RegInfoBase {
  struct Pae {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 5;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

struct Cr8Info : public RegInfoBase {};

};  // namespace cr

/**
 * @brief xcr0 寄存器
 * @see sdm.pdf#2.6
 */
struct Xcr0Info : public RegInfoBase {};

/**
 * @brief 段寄存器
 * @see sdm.pdf#3.4.3
 * @note In 64-bit mode the CS/SS/DS/ES segments are ignored and the base
 * address is always 0 to provide a full 64bit address space. The FS and GS
 * segments are still functional in 64-bit mode.
 */
namespace segment_register {
/**
 * @brief 段选择子
 * @see sdm.pdf#3.4.2
 */
struct SegmentSelector : public RegInfoBase {
  using DataType = uint16_t;
  static constexpr uint64_t kBitWidth = 16;
  struct Rpl {
    using DataType = uint8_t;
    static constexpr uint64_t kBitOffset = 0;
    static constexpr uint64_t kBitWidth = 2;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Ti {
    using DataType = bool;
    static constexpr uint64_t kBitOffset = 2;
    static constexpr uint64_t kBitWidth = 1;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };

  struct Index {
    using DataType = uint16_t;
    static constexpr uint64_t kBitOffset = 3;
    static constexpr uint64_t kBitWidth = 13;
    static constexpr uint64_t kBitMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) << kBitOffset : ~0ULL;
    static constexpr uint64_t kAllSetMask =
        (kBitWidth < 64) ? ((1ULL << kBitWidth) - 1) : ~0ULL;
  };
};

/// 无法直接写入，通常是在调用远跳转、调用或返回时更改
struct CsInfo : public SegmentSelector {};

struct SsInfo : public SegmentSelector {};

struct DsInfo : public SegmentSelector {};

struct EsInfo : public SegmentSelector {};

struct FsInfo : public SegmentSelector {};

struct GsInfo : public SegmentSelector {};

};  // namespace segment_register

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
    if constexpr (std::is_same_v<RegInfo, register_info::RbpInfo>) {
      __asm__ volatile("mov %%rbp, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::RflagsInfo>) {
      __asm__ volatile("pushfq; popq %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::GdtrInfo>) {
      __asm__ volatile("sgdt %0" : "=m"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::LdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::IdtrInfo>) {
      __asm__ volatile("sidt %0" : "=m"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::TrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr0Info>) {
      __asm__ volatile("mov %%cr0, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr2Info>) {
      __asm__ volatile("mov %%cr2, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr3Info>) {
      __asm__ volatile("mov %%cr3, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr4Info>) {
      __asm__ volatile("mov %%cr4, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr8Info>) {
      __asm__ volatile("mov %%cr8, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::Xcr0Info>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::CsInfo>) {
      __asm__ volatile("mov %%cs, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::SsInfo>) {
      __asm__ volatile("mov %%ss, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::DsInfo>) {
      __asm__ volatile("mov %%ds, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::EsInfo>) {
      __asm__ volatile("mov %%es, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::FsInfo>) {
      __asm__ volatile("mov %%fs, %0" : "=r"(value) : :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::GsInfo>) {
      __asm__ volatile("mov %%gs, %0" : "=r"(value) : :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
    return value;
  }

  static __always_inline auto Read(uint32_t offset) ->
      typename RegInfo::DataType {
    typename RegInfo::DataType value{};
    if constexpr (std::is_same_v<RegInfo, register_info::MsrInfo>) {
      uint32_t low{};
      uint32_t high{};
      __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(offset) :);
      value = (static_cast<uint64_t>(high) << 32) | low;
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
    if constexpr (std::is_same_v<RegInfo, register_info::RbpInfo>) {
      __asm__ volatile("mov %0, %%rbp" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::RflagsInfo>) {
      __asm__ volatile("pushq %0; popfq" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::GdtrInfo>) {
      __asm__ volatile("lgdt %0" : : "m"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::LdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::IdtrInfo>) {
      __asm__ volatile("lidt %0" : : "m"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::TrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr0Info>) {
      __asm__ volatile("mov %0, %%cr0" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr2Info>) {
      __asm__ volatile("mov %0, %%cr2" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr3Info>) {
      __asm__ volatile("mov %0, %%cr3" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr4Info>) {
      __asm__ volatile("mov %0, %%cr4" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr8Info>) {
      __asm__ volatile("mov %0, %%cr8" : : "r"(value) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::Xcr0Info>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::CsInfo>) {
      auto JumpFunction = [=](uint16_t value) {
        __asm__ volatile(
            "push %0\n\t"
            "pushq $WriteCsLabel\n\t"
            "retfq\n\t"
            "WriteCsLabel: \n\t"
            :
            : "r"(value)
            :);
      };
      JumpFunction(value);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::SsInfo>) {
      __asm__ volatile("mov %0, %%ss" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::DsInfo>) {
      __asm__ volatile("mov %0, %%ds" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::EsInfo>) {
      __asm__ volatile("mov %0, %%es" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::FsInfo>) {
      __asm__ volatile("mov %0, %%fs" : : "r"(value) :);
    } else if constexpr (std::is_same_v<
                             RegInfo,
                             register_info::segment_register::GsInfo>) {
      __asm__ volatile("mov %0, %%gs" : : "r"(value) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  static __always_inline void Write(uint32_t offset,
                                    typename RegInfo::DataType value) {
    if constexpr (std::is_same_v<RegInfo, register_info::MsrInfo>) {
      uint32_t low = value & 0xFFFFFFFF;
      uint32_t high = value >> 32;
      __asm__ volatile("wrmsr" : : "c"(offset), "a"(low), "d"(high) :);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 通过偏移设置寄存器
   * @param offset 位偏移
   */
  static __always_inline void SetBits(uint64_t offset) {
    if constexpr (std::is_same_v<RegInfo, register_info::RbpInfo>) {
      __asm__ volatile("bts %%rbp, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::RflagsInfo>) {
      if (offset == register_info::RflagsInfo::If::kBitOffset) {
        __asm__ volatile("sti");
      } else {
        typename RegInfo::DataType old_value = 0;
        __asm__ volatile("pushfq; popq %0" : "=r"(old_value) : :);
        auto new_value = old_value | (1 << offset);
        Write(new_value);
      }
    } else if constexpr (std::is_same_v<RegInfo, register_info::GdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::LdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::IdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::TrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr0Info>) {
      __asm__ volatile("bts %%cr0, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr2Info>) {
      __asm__ volatile("bts %%cr2, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr3Info>) {
      __asm__ volatile("bts %%cr3, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr4Info>) {
      __asm__ volatile("bts %%cr4, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr8Info>) {
      __asm__ volatile("bts %%cr8, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::Xcr0Info>) {
      static_assert(sizeof(RegInfo) == 0);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * 清零寄存器
   * @param offset 位偏移
   */
  static __always_inline void ClearBits(uint64_t offset) {
    if constexpr (std::is_same_v<RegInfo, register_info::RbpInfo>) {
      __asm__ volatile("btr %%rbp, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::RflagsInfo>) {
      if (offset == register_info::RflagsInfo::If::kBitOffset) {
        __asm__ volatile("cli");
      } else {
        typename RegInfo::DataType old_value = 0;
        __asm__ volatile("pushfq; popq %0" : "=r"(old_value) : :);
        auto new_value = old_value & (~(1ULL << offset));
        Write(new_value);
      }
    } else if constexpr (std::is_same_v<RegInfo, register_info::GdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::LdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::IdtrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::TrInfo>) {
      static_assert(sizeof(RegInfo) == 0);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr0Info>) {
      __asm__ volatile("btr %%cr0, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr2Info>) {
      __asm__ volatile("btr %%cr2, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr3Info>) {
      __asm__ volatile("btr %%cr3, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr4Info>) {
      __asm__ volatile("btr %%cr4, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::cr::Cr8Info>) {
      __asm__ volatile("btr %%cr8, %0" : : "r"(offset) :);
    } else if constexpr (std::is_same_v<RegInfo, register_info::Xcr0Info>) {
      static_assert(sizeof(RegInfo) == 0);
    } else {
      static_assert(sizeof(RegInfo) == 0);
    }
  }

  /**
   * |= 重载
   */
  __always_inline void operator|=(uint64_t offset) { SetBits(offset); }
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
   * 通过偏移先读后写寄存器
   * @param offset 偏移
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadSetBits(uint64_t offset) ->
      typename RegInfo::DataType {
    auto old_value = ReadOnlyRegBase<RegInfo>::Read();
    WriteOnlyRegBase<RegInfo>::SetBits(offset);
    return old_value;
  }

  /**
   * 通过偏移先读后清零寄存器
   * @param offset 偏移
   * @return RegInfo::DataType 寄存器的值
   */
  static __always_inline auto ReadClearBits(uint64_t offset) ->
      typename RegInfo::DataType {
    auto old_value = ReadOnlyRegBase<RegInfo>::Read();
    WriteOnlyRegBase<RegInfo>::ClearBits(offset);
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
    if constexpr (std::is_same_v<RegInfo, register_info::GdtrInfo::Limit>) {
      return Reg::Read().limit;
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::GdtrInfo::Base>) {
      return Reg::Read().base;
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::IdtrInfo::Limit>) {
      return Reg::Read().limit;
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::IdtrInfo::Base>) {
      return Reg::Read().base;
    } else {
      return static_cast<typename RegInfo::DataType>(
          (Reg::Read() & RegInfo::kBitMask) >> RegInfo::kBitOffset);
    }
  }

  /**
   * 从指定的值获取对应 Reg 的由 RegInfo 规定的指定位的值
   * @param value 指定的值
   * @return RegInfo::DataType 指定位值的信息
   */
  static __always_inline auto Get(uint64_t value) ->
      typename RegInfo::DataType {
    if constexpr (std::is_same_v<RegInfo, register_info::GdtrInfo::Limit>) {
      return value;
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::GdtrInfo::Base>) {
      return value;
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::IdtrInfo::Limit>) {
      return value;
    } else if constexpr (std::is_same_v<RegInfo,
                                        register_info::IdtrInfo::Base>) {
      return value;
    } else {
      return static_cast<typename RegInfo::DataType>(
          (value & RegInfo::kBitMask) >> RegInfo::kBitOffset);
    }
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
  static __always_inline void Set() { Reg::SetBits(RegInfo::kBitOffset); }

  /**
   * 清零对应 Reg 的由 RegInfo 规定的指定位
   */
  static __always_inline void Clear() { Reg::ClearBits(RegInfo::kBitOffset); }
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
    auto new_value = (org_value & ~RegInfo::kBitMask) |
                     ((value << RegInfo::kBitOffset) & RegInfo::kBitMask);
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

namespace regs {

// 第三部分：寄存器实例
struct Rbp : public read_write::ReadWriteRegBase<register_info::RbpInfo> {};

struct Msr : public read_write::ReadWriteRegBase<register_info::MsrInfo> {};

struct Rflags : public read_write::ReadWriteRegBase<register_info::RflagsInfo> {
  using If = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::RflagsInfo>,
      register_info::RflagsInfo::If>;
};

struct Gdtr : public read_write::ReadWriteRegBase<register_info::GdtrInfo> {
  using Limit = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::GdtrInfo>,
      register_info::GdtrInfo::Limit>;
  using Base = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::GdtrInfo>,
      register_info::GdtrInfo::Base>;
};

struct Ldtr : public read_write::ReadWriteRegBase<register_info::LdtrInfo> {};

struct Idtr : public read_write::ReadWriteRegBase<register_info::IdtrInfo> {
  using Limit = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::IdtrInfo>,
      register_info::IdtrInfo::Limit>;
  using Base = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::IdtrInfo>,
      register_info::IdtrInfo::Base>;
};

struct Tr : public read_write::ReadWriteRegBase<register_info::TrInfo> {};

namespace cr {

struct Cr0 : public read_write::ReadWriteRegBase<register_info::cr::Cr0Info> {
  using Pe = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::cr::Cr0Info>,
      register_info::cr::Cr0Info::Pe>;
  using Pg = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::cr::Cr0Info>,
      register_info::cr::Cr0Info::Pg>;
};

struct Cr2 : public read_write::ReadWriteRegBase<register_info::cr::Cr2Info> {};

struct Cr3 : public read_write::ReadWriteRegBase<register_info::cr::Cr3Info> {
  using Pwt = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::cr::Cr3Info>,
      register_info::cr::Cr3Info::Pwt>;
  using Pcd = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::cr::Cr3Info>,
      register_info::cr::Cr3Info::Pcd>;
  using PageDirectoryBase = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::cr::Cr3Info>,
      register_info::cr::Cr3Info::PageDirectoryBase>;
};

struct Cr4 : public read_write::ReadWriteRegBase<register_info::cr::Cr4Info> {
  using Pae = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::cr::Cr4Info>,
      register_info::cr::Cr4Info::Pae>;
};

struct Cr8 : public read_write::ReadWriteRegBase<register_info::cr::Cr8Info> {};

};  // namespace cr

struct Xcr0 : public read_write::ReadWriteRegBase<register_info::Xcr0Info> {};

namespace segment_register {
struct Cs : public read_write::ReadWriteRegBase<
                register_info::segment_register::CsInfo> {
  using Rpl = read_write::ReadOnlyField<
      read_write::ReadWriteRegBase<register_info::segment_register::CsInfo>,
      register_info::segment_register::CsInfo::Rpl>;

  using Ti = read_write::ReadOnlyField<
      read_write::ReadWriteRegBase<register_info::segment_register::CsInfo>,
      register_info::segment_register::CsInfo::Ti>;

  using Index = read_write::ReadOnlyField<
      read_write::ReadWriteRegBase<register_info::segment_register::CsInfo>,
      register_info::segment_register::CsInfo::Index>;
};

struct Ss : public read_write::ReadWriteRegBase<
                register_info::segment_register::SsInfo> {
  using Rpl = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::SsInfo>,
      register_info::segment_register::SsInfo::Rpl>;

  using Ti = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::SsInfo>,
      register_info::segment_register::SsInfo::Ti>;

  using Index = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::SsInfo>,
      register_info::segment_register::SsInfo::Index>;
};

struct Ds : public read_write::ReadWriteRegBase<
                register_info::segment_register::DsInfo> {
  using Rpl = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::DsInfo>,
      register_info::segment_register::DsInfo::Rpl>;

  using Ti = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::DsInfo>,
      register_info::segment_register::DsInfo::Ti>;

  using Index = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::DsInfo>,
      register_info::segment_register::DsInfo::Index>;
};

struct Es : public read_write::ReadWriteRegBase<
                register_info::segment_register::EsInfo> {
  using Rpl = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::EsInfo>,
      register_info::segment_register::EsInfo::Rpl>;

  using Ti = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::EsInfo>,
      register_info::segment_register::EsInfo::Ti>;

  using Index = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::EsInfo>,
      register_info::segment_register::EsInfo::Index>;
};

struct Fs : public read_write::ReadWriteRegBase<
                register_info::segment_register::FsInfo> {
  using Rpl = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::FsInfo>,
      register_info::segment_register::FsInfo::Rpl>;

  using Ti = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::FsInfo>,
      register_info::segment_register::FsInfo::Ti>;

  using Index = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::FsInfo>,
      register_info::segment_register::FsInfo::Index>;
};

struct Gs : public read_write::ReadWriteRegBase<
                register_info::segment_register::GsInfo> {
  using Rpl = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::GsInfo>,
      register_info::segment_register::GsInfo::Rpl>;
  using Ti = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::GsInfo>,
      register_info::segment_register::GsInfo::Ti>;
  using Index = read_write::ReadWriteField<
      read_write::ReadWriteRegBase<register_info::segment_register::GsInfo>,
      register_info::segment_register::GsInfo::Index>;
};
};  // namespace segment_register

};  // namespace regs

};  // namespace detail

// 第四部分：访问接口
using Rbp = detail::regs::Rbp;
using Msr = detail::regs::Msr;
using Rflags = detail::regs::Rflags;
using Gdtr = detail::regs::Gdtr;
using Ldtr = detail::regs::Ldtr;
using Idtr = detail::regs::Idtr;
using Tr = detail::regs::Tr;
using Cr0 = detail::regs::cr::Cr0;
using Cr2 = detail::regs::cr::Cr2;
using Cr3 = detail::regs::cr::Cr3;
using Cr4 = detail::regs::cr::Cr4;
using Cr8 = detail::regs::cr::Cr8;
using Xcr0 = detail::regs::Xcr0;
using Cs = detail::regs::segment_register::Cs;
using Ss = detail::regs::segment_register::Ss;
using Ds = detail::regs::segment_register::Ds;
using Es = detail::regs::segment_register::Es;
using Fs = detail::regs::segment_register::Fs;
using Gs = detail::regs::segment_register::Gs;

};  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_X86_64_REGS_HPP_
