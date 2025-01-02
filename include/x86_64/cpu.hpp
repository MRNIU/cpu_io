
/**
 * @file cpu.hpp
 * @brief x86_64 cpu 相关定义
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

#ifndef CPU_IO_INCLUDE_X86_64_CPU_HPP_
#define CPU_IO_INCLUDE_X86_64_CPU_HPP_

#include <sys/cdefs.h>

#include <cstdint>
#include <cstdlib>
#include <type_traits>

#include "regs.hpp"

/**
 * x86_64 cpu 相关定义
 * @note 寄存器读写设计见 arch/README.md
 * @see sdm.pdf
 * Intel® 64 and IA-32 Architectures Software Developer’s Manual
 * Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 * Order Number: 325384-083US
 * https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
 */
namespace cpu {
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

/// @name 端口
static constexpr const uint32_t kCom1 = 0x3F8;
/**
 * 串口定义
 */
class Serial {
 public:
  explicit Serial(uint32_t port) : port_(port) {
    // Disable all interrupts
    Out<uint8_t>(port_ + 1, 0x00);
    // Enable DLAB (set baud rate divisor)
    Out<uint8_t>(port_ + 3, 0x80);
    // Set divisor to 3 (lo byte) 38400 baud
    Out<uint8_t>(port_ + 0, 0x03);
    // (hi byte)
    Out<uint8_t>(port_ + 1, 0x00);
    // 8 bits, no parity, one stop bit
    Out<uint8_t>(port_ + 3, 0x03);
    // Enable FIFO, clear them, with 14-byte threshold
    Out<uint8_t>(port_ + 2, 0xC7);
    // IRQs enabled, RTS/DSR set
    Out<uint8_t>(port_ + 4, 0x0B);
    // Set in loopback mode, test the serial chip
    Out<uint8_t>(port_ + 4, 0x1E);
    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    Out<uint8_t>(port_ + 0, 0xAE);
    // Check if serial is faulty (i.e: not same byte as sent)
    if (In<uint8_t>(port_ + 0) != 0xAE) {
      asm("hlt");
    }

    // If serial is not faulty set it in normal operation mode (not-loopback
    // with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    Out<uint8_t>(port_ + 4, 0x0F);
  }

  ~Serial() = default;

  /// @name 不使用的构造函数
  /// @{
  Serial() = delete;
  Serial(const Serial &) = delete;
  Serial(Serial &&) = delete;
  auto operator=(const Serial &) -> Serial & = delete;
  auto operator=(Serial &&) -> Serial & = delete;
  /// @}

  /**
   * @brief  读一个字节
   * @return uint8_t         读取到的数据
   */
  [[nodiscard]] auto Read() const -> uint8_t {
    while (!SerialReceived()) {
      ;
    }
    return In<uint8_t>(port_);
  }

  /**
   * @brief  写一个字节
   * @param  byte              要写的数据
   */
  void Write(uint8_t byte) const {
    while (!IsTransmitEmpty()) {
      ;
    }
    Out<uint8_t>(port_, byte);
  }

 private:
  uint32_t port_;

  /**
   * @brief 串口是否接收到数据
   * @return true
   * @return false
   */
  [[nodiscard]] auto SerialReceived() const -> bool {
    return bool(In<uint8_t>(port_ + 5) & 1);
  }

  /**
   * @brief 串口是否可以发送数据
   * @return true
   * @return false
   */
  [[nodiscard]] auto IsTransmitEmpty() const -> bool {
    return bool((In<uint8_t>(port_ + 5) & 0x20) != 0);
  }
};

/**
 * 中断控制器(8259A)
 * @see https://wiki.osdev.org/8259_PIC
 * @note master 处理 8 个中断，slave 处理八个中断
 * @note 工作在 8086 模式下，中断处理完后需要通知 pic 重置 ISR 寄存器
 */
class Pic {
 public:
  /**
   * 构造函数
   * @param offset1 主片中断偏移，共 8 个
   * @param offset2 从片中断偏移，共 8 个
   */
  explicit Pic(uint8_t offset1, uint8_t offset2)
      : offset1_(offset1), offset2_(offset2) {
    // 0001 0001
    Out<uint8_t>(kMasterCommandPort, kIcw1Init | kIcw1Icw4);
    // 设置主片 IRQ 从 offset1_ 号中断开始
    Out<uint8_t>(kMasterDataPort, offset1_);
    // 设置主片 IR2 引脚连接从片
    // 4: 0000 0100
    Out<uint8_t>(kMasterDataPort, 4);
    // 设置主片按照 8086 的方式工作
    Out<uint8_t>(kMasterDataPort, kIcw48086);

    Out<uint8_t>(kSlaveCommandPort, kIcw1Init | kIcw1Icw4);
    // 设置从片 IRQ 从 offset2_ 号中断开始
    Out<uint8_t>(kPic2DataPort, offset2_);
    // 告诉从片输出引脚和主片 IR2 号相连
    // 2: 0000 0010
    Out<uint8_t>(kPic2DataPort, 2);
    // 设置从片按照 8086 的方式工作
    Out<uint8_t>(kPic2DataPort, kIcw48086);

    // 关闭所有中断
    Out<uint8_t>(kMasterDataPort, 0xFF);
    Out<uint8_t>(kPic2DataPort, 0xFF);
  }

  /// @name 构造/析构函数
  /// @{
  Pic() = delete;
  Pic(const Pic &) = delete;
  Pic(Pic &&) = delete;
  auto operator=(const Pic &) -> Pic & = delete;
  auto operator=(Pic &&) -> Pic & = delete;
  ~Pic() = default;
  /// @}

  /**
   * 开启 pic 的 no 中断
   * @param no 中断号
   */
  void Enable(uint8_t no) const {
    uint8_t mask = 0;
    if (no >= offset2_) {
      mask = ((In<uint8_t>(kPic2DataPort)) & (~(1 << (no % 8))));
      Out<uint8_t>(kPic2DataPort, mask);
    } else {
      mask = ((In<uint8_t>(kMasterDataPort)) & (~(1 << (no % 8))));
      Out<uint8_t>(kMasterDataPort, mask);
    }
  }

  /**
   * 关闭 8259A 芯片的所有中断
   */
  static void Disable() {
    // 屏蔽所有中断
    Out<uint8_t>(kMasterDataPort, 0xFF);
    Out<uint8_t>(kPic2DataPort, 0xFF);
  }

  /**
   * 关闭 pic 的 no 中断
   * @param no 中断号
   */
  void Disable(uint8_t no) const {
    uint8_t mask = 0;
    if (no >= offset2_) {
      mask = ((In<uint8_t>(kPic2DataPort)) | (1 << (no % 8)));
      Out<uint8_t>(kPic2DataPort, mask);
    } else {
      mask = ((In<uint8_t>(kMasterDataPort)) | (1 << (no % 8)));
      Out<uint8_t>(kMasterDataPort, mask);
    }
  }

  /**
   * 通知 pic no 中断处理完毕
   * @param no 中断号
   */
  void Clear(uint8_t no) const {
    // 按照我们的设置，从 offset1_ 号中断起为用户自定义中断
    // 因为单片的 Intel 8259A 芯片只能处理 8 级中断
    // 故大于等于 offset2_ 的中断号是由从片处理的
    if (no >= offset2_) {
      // 发送重设信号给从片
      Out<uint8_t>(kSlaveCommandPort, kEoi);
    } else {
      // 发送重设信号给主片
      Out<uint8_t>(kMasterCommandPort, kEoi);
    }
  }

  /**
   * Returns the combined value of the cascaded PICs irq request register
   * @return uint16_t 值
   */
  static auto GetIrr() -> uint16_t { return GetIrqReg(kOcw3ReadIrr); }

  /**
   * Returns the combined value of the cascaded PICs in-service register
   * @return uint16_t 值
   */
  static auto GetIsr() -> uint16_t { return GetIrqReg(kOcw3ReadIsr); }

 private:
  uint8_t offset1_;
  uint8_t offset2_;

  /// Master (IRQs 0-7)
  static constexpr const uint8_t kMaster = 0x20;
  /// Slave  (IRQs 8-15)
  static constexpr const uint8_t kSlave = 0xA0;
  static constexpr const uint8_t kMasterCommandPort = kMaster;
  static constexpr const uint8_t kMasterDataPort = kMaster + 1;
  static constexpr const uint8_t kSlaveCommandPort = kSlave;
  static constexpr const uint8_t kPic2DataPort = kSlave + 1;
  /// End-of-interrupt command code
  static constexpr const uint8_t kEoi = 0x20;

  /// Indicates that ICW4 will be present
  static constexpr const uint8_t kIcw1Icw4 = 0x01;
  /// Single (cascade) mode
  static constexpr const uint8_t kIcw1Single = 0x02;
  /// Call address interval 4 (8)
  static constexpr const uint8_t kIcw1Interval4 = 0x04;
  /// Level triggered (edge) mode
  static constexpr const uint8_t kIcw1Level = 0x08;
  /// Initialization - required!
  static constexpr const uint8_t kIcw1Init = 0x10;

  /// OCW3 irq ready next CMD read
  static constexpr const uint8_t kOcw3ReadIrr = 0x0A;
  /// OCW3 irq service next CMD read
  static constexpr const uint8_t kOcw3ReadIsr = 0x0B;

  /// 8086/88 (MCS-80/85) mode
  static constexpr const uint8_t kIcw48086 = 0x01;
  /// Auto (normal) EOI
  static constexpr const uint8_t kIcw4Auto = 0x02;
  /// Buffered mode/slave
  static constexpr const uint8_t kIcw4BufferSlave = 0x08;
  /// Buffered mode/master
  static constexpr const uint8_t kIcw4BufferMaster = 0x0C;
  /// Special fully nested (not)
  static constexpr const uint8_t kIcw4Sfnm = 0x10;

  /**
   * 获取中断请求寄存器的值
   * @note OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
   * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain
   * @param ocw3 OCW3
   * @return uint16_t 值
   */
  static auto GetIrqReg(uint8_t ocw3) -> uint16_t {
    Out<uint8_t>(kMasterCommandPort, ocw3);
    Out<uint8_t>(kSlaveCommandPort, ocw3);
    return (In<uint8_t>(kSlaveCommandPort) << 8) |
           In<uint8_t>(kMasterCommandPort);
  }
};

/**
 * 时钟控制器(8253/8254)
 * @see https://en.wikipedia.org/wiki/Intel_8253
 * @see https://wiki.osdev.org/Programmable_Interval_Timer
 */
class Pit {
 public:
  /**
   * 构造函数
   * @param frequency 每秒中断次数
   */
  explicit Pit(uint16_t frequency) {
    uint16_t divisor = kMaxFrequency / frequency;

    // 设置 8253/8254 芯片工作在模式 3 下
    Out<uint8_t>(kCommand, static_cast<uint8_t>(kChannel0) |
                               static_cast<uint8_t>(kHighAndLow) |
                               static_cast<uint8_t>(kSquareWaveGenerator));

    // 分别写入低字节和高字节
    Out<uint8_t>(kChannel0Data, divisor & 0xFF);
    Out<uint8_t>(kChannel0Data, divisor >> 8);
  }

  /// @name 构造/析构函数
  /// @{
  Pit() = delete;
  Pit(const Pit &) = delete;
  Pit(Pit &&) = delete;
  auto operator=(const Pit &) -> Pit & = delete;
  auto operator=(Pit &&) -> Pit & = delete;
  ~Pit() = default;
  /// @}

  /**
   * 计数器更新
   */
  void Ticks() { ticks_ += 1; }

  /**
   * 获取时钟中断次数
   * @return size_t 时钟中断次数
   */
  [[nodiscard]] auto GetTicks() const -> size_t { return ticks_; }

 private:
  /// 最大频率
  static constexpr const size_t kMaxFrequency = 1193180;
  /// 通道 0 数据端口
  static constexpr const size_t kChannel0Data = 0x40;
  /// 模式/命令端口
  static constexpr const size_t kCommand = 0x43;

  /**
   * Bits         Usage
   * 6 and 7      Select channel :
   *                 0 0 = Channel 0
   *                 0 1 = Channel 1
   *                 1 0 = Channel 2
   *                 1 1 = Read-back command (8254 only)
   */
  enum Channel {
    kChannel0 = 0x0,
    kChannel1 = 0x40,
    kChannel2 = 0x80,
  };

  /**
   * Bits         Usage
   * 4 and 5      Access mode :
   *                 0 0 = Latch count value command
   *                 0 1 = Access mode: lobyte only
   *                 1 0 = Access mode: hibyte only
   *                 1 1 = Access mode: lobyte/hibyte
   */
  enum Access {
    kLatchCount = 0x0,
    kLowOnly = 0x10,
    kHighOnly = 0x20,
    kHighAndLow = 0x30,
  };

  /**
   * Bits         Usage
   * 1 to 3       Operating mode :
   *                 0 0 0 = Mode 0 (interrupt on terminal count)
   *                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
   *                 0 1 0 = Mode 2 (rate generator)
   *                 0 1 1 = Mode 3 (square wave generator)
   *                 1 0 0 = Mode 4 (software triggered strobe)
   *                 1 0 1 = Mode 5 (hardware triggered strobe)
   *                 1 1 0 = Mode 2 (rate generator, same as 010b)
   *                 1 1 1 = Mode 3 (square wave generator, same as 011b)
   * 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
   */
  enum Mode {
    kInterruptOnTerminalCount = 0x0,
    kHardwareRetriggerableOneShot = 0x2,
    kRateGenerator = 0x4,
    kSquareWaveGenerator = 0x6,
    kSoftwareTriggeredStrobe = 0x8,
    kHardwareTriggeredStrobe = 0xA,
  };

  /// 计数器
  volatile size_t ticks_ = 0;
};

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
  register_info::IdtrInfo::ErrorCode error_code;
  uint32_t padding;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
};

};  // namespace cpu

#endif  // CPU_IO_INCLUDE_X86_64_CPU_HPP_
