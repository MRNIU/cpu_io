/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_X86_64_MSR_HPP_
#define CPU_IO_INCLUDE_X86_64_MSR_HPP_

#include <cpuid.h>
#include <sys/cdefs.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "regs.hpp"

namespace cpu_io {
/**
 * @brief MSR（Model Specific Register）相关定义和操作
 */
namespace msr {

/// MSR 寄存器地址常量定义
/// @see Intel SDM Vol. 4

/// 扩展功能使能寄存器
static constexpr const uint32_t kIa32Efer = 0xC0000080;
/// SYSCALL target address
static constexpr const uint32_t kIa32Star = 0xC0000081;
/// Long mode SYSCALL target
static constexpr const uint32_t kIa32Lstar = 0xC0000082;
/// Compatibility mode SYSCALL target
static constexpr const uint32_t kIa32Cstar = 0xC0000083;
/// EFLAGS mask for syscall
static constexpr const uint32_t kIa32SyscallMask = 0xC0000084;
/// 64bit FS base
static constexpr const uint32_t kIa32FsBase = 0xC0000100;
/// 64bit GS base
static constexpr const uint32_t kIa32GsBase = 0xC0000101;
/// SwapGS GS shadow
static constexpr const uint32_t kIa32KernelGsBase = 0xC0000102;

/// 基本 MSR

/// Platform ID
static constexpr const uint32_t kIa32PlatformId = 0x00000017;
/// APIC Base Address
static constexpr const uint32_t kIa32ApicBase = 0x0000001B;
/// Feature Control
static constexpr const uint32_t kIa32FeatureControl = 0x0000003A;
/// Time Stamp Counter
static constexpr const uint32_t kIa32Tsc = 0x00000010;
/// Misc Enable
static constexpr const uint32_t kIa32MiscEnable = 0x000001A0;

/// 性能监控相关 MSR

/// Performance Global Control
static constexpr const uint32_t kIa32PerfGlobalCtrl = 0x0000038F;
/// Performance Global Status
static constexpr const uint32_t kIa32PerfGlobalStatus = 0x0000038E;
/// Performance Global Overflow Control
static constexpr const uint32_t kIa32PerfGlobalOvfCtrl = 0x00000390;

/// 内存类型范围寄存器 (MTRR)

/// MTRR Capabilities
static constexpr const uint32_t kIa32MtrrCap = 0x000000FE;
/// MTRR Default Type
static constexpr const uint32_t kIa32MtrrDefType = 0x000002FF;
/// MTRR Physical Base 0
static constexpr const uint32_t kIa32MtrrPhysBase0 = 0x00000200;
/// MTRR Physical Mask 0
static constexpr const uint32_t kIa32MtrrPhysMask0 = 0x00000201;

/// 调试相关 MSR

/// Debug Control
static constexpr const uint32_t kIa32DebugCtl = 0x000001D9;
/// Last Branch From IP
static constexpr const uint32_t kIa32LastBranchFromIp = 0x000001DB;
/// Last Branch To IP
static constexpr const uint32_t kIa32LastBranchToIp = 0x000001DC;

/**
 * @brief APIC 相关 MSR 专用命名空间
 */
namespace apic {
/// APIC Base 地址 MSR
static constexpr const uint32_t kBase = kIa32ApicBase;

/// x2APIC MSR 寄存器地址
/// x2APIC ID Register
static constexpr const uint32_t kId = 0x802;
/// x2APIC Version Register
static constexpr const uint32_t kVersion = 0x803;
/// Task Priority Register
static constexpr const uint32_t kTpr = 0x808;
/// End of Interrupt Register
static constexpr const uint32_t kEoi = 0x80B;
/// Spurious Interrupt Vector Register
static constexpr const uint32_t kSivr = 0x80F;
/// Interrupt Command Register
static constexpr const uint32_t kIcr = 0x830;
/// LVT Timer Register
static constexpr const uint32_t kLvtTimer = 0x832;
/// LVT LINT0 Register
static constexpr const uint32_t kLvtLint0 = 0x835;
/// LVT LINT1 Register
static constexpr const uint32_t kLvtLint1 = 0x836;
/// LVT Error Register
static constexpr const uint32_t kLvtError = 0x837;
/// Timer Initial Count Register
static constexpr const uint32_t kTimerInitCount = 0x838;
/// Timer Current Count Register
static constexpr const uint32_t kTimerCurrCount = 0x839;
/// Timer Divide Configuration Register
static constexpr const uint32_t kTimerDivide = 0x83E;

/// APIC Base MSR 位域定义
namespace base {
/// BSP (Bootstrap Processor) 位
static constexpr const uint64_t kBspBit = 1ULL << 8;
/// Global Enable 位
static constexpr const uint64_t kGlobalEnableBit = 1ULL << 11;
/// x2APIC Enable 位
static constexpr const uint64_t kX2ApicEnableBit = 1ULL << 10;
/// APIC Base 地址掩码
static constexpr const uint64_t kBaseMask = 0xFFFFF000ULL;
}  // namespace base

/**
 * @brief 读取 APIC Base 寄存器
 * @return uint64_t APIC Base 寄存器的值
 */
static __always_inline auto ReadBase() -> uint64_t {
  return cpu_io::Msr::Read(msr::kIa32ApicBase);
}

/**
 * @brief 写入 APIC Base 寄存器
 * @param value 要写入的值
 */
static __always_inline void WriteBase(uint64_t value) {
  cpu_io::Msr::Write(msr::kIa32ApicBase, value);
}

/**
 * @brief 检查是否为 BSP (Bootstrap Processor)
 * @return true 如果是 BSP，false 否则
 */
static __always_inline auto IsBsp() -> bool {
  return (ReadBase() & base::kBspBit) != 0;
}

/**
 * @brief 检查 APIC 是否全局启用
 * @return true 如果 APIC 全局启用，false 否则
 */
static __always_inline auto IsGloballyEnabled() -> bool {
  return (ReadBase() & base::kGlobalEnableBit) != 0;
}

/**
 * @brief 检查 x2APIC 模式是否启用
 * @return true 如果 x2APIC 模式启用，false 否则
 */
static __always_inline auto IsX2ApicEnabled() -> bool {
  return (ReadBase() & base::kX2ApicEnableBit) != 0;
}

/**
 * @brief 获取 APIC Base 物理地址
 * @return uint64_t APIC Base 物理地址
 */
static __always_inline auto GetBaseAddress() -> uint64_t {
  return ReadBase() & base::kBaseMask;
}

/**
 * @brief 启用 APIC 全局功能
 */
static __always_inline void EnableGlobally() {
  auto value = ReadBase();
  value |= base::kGlobalEnableBit;
  WriteBase(value);
}

/**
 * @brief 禁用 APIC 全局功能
 */
static __always_inline void DisableGlobally() {
  auto value = ReadBase();
  value &= ~base::kGlobalEnableBit;
  WriteBase(value);
}

/**
 * @brief 启用 x2APIC 模式
 */
static __always_inline void EnableX2Apic() {
  auto value = ReadBase();
  value |= base::kX2ApicEnableBit | base::kGlobalEnableBit;
  WriteBase(value);
}

/**
 * @brief 禁用 x2APIC 模式
 */
static __always_inline void DisableX2Apic() {
  auto value = ReadBase();
  value &= ~base::kX2ApicEnableBit;
  WriteBase(value);
}

// x2APIC MSR 寄存器操作函数

/**
 * @brief 读取 x2APIC ID 寄存器
 * @return uint32_t x2APIC ID
 */
static __always_inline auto ReadId() -> uint32_t {
  return static_cast<uint32_t>(cpu_io::Msr::Read(msr::apic::kId));
}

/**
 * @brief 读取 x2APIC 版本寄存器
 * @return uint32_t APIC 版本信息
 */
static __always_inline auto ReadVersion() -> uint32_t {
  return static_cast<uint32_t>(cpu_io::Msr::Read(msr::apic::kVersion));
}

/**
 * @brief 读取任务优先级寄存器 (TPR)
 * @return uint32_t 任务优先级值
 */
static __always_inline auto ReadTpr() -> uint32_t {
  return static_cast<uint32_t>(cpu_io::Msr::Read(msr::apic::kTpr));
}

/**
 * @brief 写入任务优先级寄存器 (TPR)
 * @param value 要设置的任务优先级值
 */
static __always_inline void WriteTpr(uint32_t value) {
  cpu_io::Msr::Write(msr::apic::kTpr, value);
}

/**
 * @brief 写入中断结束寄存器 (EOI)
 * @param value EOI 值（通常为 0）
 */
static __always_inline void WriteEoi(uint32_t value = 0) {
  cpu_io::Msr::Write(msr::apic::kEoi, value);
}

/**
 * @brief 读取虚假中断向量寄存器 (SIVR)
 * @return uint32_t SIVR 值
 */
static __always_inline auto ReadSivr() -> uint32_t {
  return static_cast<uint32_t>(cpu_io::Msr::Read(msr::apic::kSivr));
}

/**
 * @brief 写入虚假中断向量寄存器 (SIVR)
 * @param value 要设置的 SIVR 值
 */
static __always_inline void WriteSivr(uint32_t value) {
  cpu_io::Msr::Write(msr::apic::kSivr, value);
}

/**
 * @brief 读取中断命令寄存器 (ICR)
 * @return uint64_t ICR 值
 */
static __always_inline auto ReadIcr() -> uint64_t {
  return cpu_io::Msr::Read(msr::apic::kIcr);
}

/**
 * @brief 写入中断命令寄存器 (ICR)
 * @param value 要发送的 IPI 命令
 */
static __always_inline void WriteIcr(uint64_t value) {
  cpu_io::Msr::Write(msr::apic::kIcr, value);
}

/**
 * @brief 读取 LVT Timer 寄存器
 * @return uint32_t LVT Timer 寄存器值
 */
static __always_inline auto ReadLvtTimer() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kLvtTimer));
}

/**
 * @brief 写入 LVT Timer 寄存器
 * @param value 要设置的 LVT Timer 值
 */
static __always_inline void WriteLvtTimer(uint32_t value) {
  cpu_io::Msr::Write(::cpu_io::msr::apic::kLvtTimer, value);
}

/**
 * @brief 读取 LVT LINT0 寄存器
 * @return uint32_t LVT LINT0 寄存器值
 */
static __always_inline auto ReadLvtLint0() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kLvtLint0));
}

/**
 * @brief 写入 LVT LINT0 寄存器
 * @param value 要设置的 LVT LINT0 值
 */
static __always_inline void WriteLvtLint0(uint32_t value) {
  cpu_io::Msr::Write(::cpu_io::msr::apic::kLvtLint0, value);
}

/**
 * @brief 读取 LVT LINT1 寄存器
 * @return uint32_t LVT LINT1 寄存器值
 */
static __always_inline auto ReadLvtLint1() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kLvtLint1));
}

/**
 * @brief 写入 LVT LINT1 寄存器
 * @param value 要设置的 LVT LINT1 值
 */
static __always_inline void WriteLvtLint1(uint32_t value) {
  cpu_io::Msr::Write(::cpu_io::msr::apic::kLvtLint1, value);
}

/**
 * @brief 读取 LVT Error 寄存器
 * @return uint32_t LVT Error 寄存器值
 */
static __always_inline auto ReadLvtError() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kLvtError));
}

/**
 * @brief 写入 LVT Error 寄存器
 * @param value 要设置的 LVT Error 值
 */
static __always_inline void WriteLvtError(uint32_t value) {
  cpu_io::Msr::Write(::cpu_io::msr::apic::kLvtError, value);
}

/**
 * @brief 读取定时器初始计数寄存器
 * @return uint32_t 定时器初始计数值
 */
static __always_inline auto ReadTimerInitCount() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kTimerInitCount));
}

/**
 * @brief 写入定时器初始计数寄存器
 * @param value 要设置的定时器初始计数值
 */
static __always_inline void WriteTimerInitCount(uint32_t value) {
  cpu_io::Msr::Write(::cpu_io::msr::apic::kTimerInitCount, value);
}

/**
 * @brief 读取定时器当前计数寄存器
 * @return uint32_t 定时器当前计数值
 */
static __always_inline auto ReadTimerCurrCount() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kTimerCurrCount));
}

/**
 * @brief 读取定时器分频配置寄存器
 * @return uint32_t 定时器分频配置值
 */
static __always_inline auto ReadTimerDivide() -> uint32_t {
  return static_cast<uint32_t>(
      cpu_io::Msr::Read(::cpu_io::msr::apic::kTimerDivide));
}

/**
 * @brief 写入定时器分频配置寄存器
 * @param value 要设置的定时器分频配置值
 */
static __always_inline void WriteTimerDivide(uint32_t value) {
  cpu_io::Msr::Write(::cpu_io::msr::apic::kTimerDivide, value);
}

}  // namespace apic

}  // namespace msr

};  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_X86_64_MSR_HPP_
