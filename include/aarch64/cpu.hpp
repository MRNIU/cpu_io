/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_AARCH64_CPU_HPP_
#define CPU_IO_INCLUDE_AARCH64_CPU_HPP_

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <typeinfo>

#include "regs.hpp"

/**
 * aarch64 cpu 相关定义
 * @note 寄存器读写设计见 arch/README.md
 */
namespace cpu_io {

// 第四部分：访问接口
using X0 = detail::regs::X0;
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
using TCR_EL1 = detail::regs::system_reg::TCR_EL1;
using ESR_EL1 = detail::regs::system_reg::ESR_EL1;
using FAR_EL1 = detail::regs::system_reg::FAR_EL1;
using CNTV_CTL_EL0 = detail::regs::system_reg::CNTV_CTL_EL0;
using CNTV_TVAL_EL0 = detail::regs::system_reg::CNTV_TVAL_EL0;
using CNTVCT_EL0 = detail::regs::system_reg::CNTVCT_EL0;
using CNTFRQ_EL0 = detail::regs::system_reg::CNTFRQ_EL0;
using ICC_PMR_EL1 = detail::regs::system_reg::ICC_PMR_EL1;
using ICC_IGRPEN1_EL1 = detail::regs::system_reg::ICC_IGRPEN1_EL1;
using ICC_SRE_EL1 = detail::regs::system_reg::ICC_SRE_EL1;
using ICC_IAR1_EL1 = detail::regs::system_reg::ICC_IAR1_EL1;
using ICC_EOIR1_EL1 = detail::regs::system_reg::ICC_EOIR1_EL1;

/**
 * @brief 允许中断
 * @todo
 */
static __always_inline void EnableInterrupt() {
  DAIF::D::Clear();
  DAIF::A::Clear();
  DAIF::I::Clear();
  DAIF::F::Clear();
}

/**
 * @brief 关闭中断
 * @todo
 */
static __always_inline void DisableInterrupt() {
  DAIF::D::Set();
  DAIF::A::Set();
  DAIF::I::Set();
  DAIF::F::Set();
}

/**
 * @brief 获取中断状态
 * @return true 允许中断
 * @return false 禁用中断
 */
static __always_inline auto GetInterruptStatus() -> bool {
  return !DAIF::I::Get() && !DAIF::F::Get();
}

/**
 * @brief 获取当前 core id
 * @return size_t core id
 */
static __always_inline auto GetCurrentCoreId() -> size_t {
  return MPIDR_EL1::Aff0::Get();
}

/**
 * @brief 初始化 FPU
 */
static __always_inline void SetupFpu() { CPACR_EL1::Fpen::Set(); }

/**
 * @brief SMC 调用返回值
 */
struct SMCReturnValue {
  uint64_t a0;
  uint64_t a1;
  uint64_t a2;
  uint64_t a3;
};

/**
 * @brief 调用安全监控
 * @param a0 参数0
 * @param a1 参数1
 * @param a2 参数2
 * @param a3 参数3
 * @param a4 参数4
 * @param a5 参数5
 * @param a6 参数6
 * @param a7 参数7
 * @param res 返回值
 */
static __always_inline auto SecureMonitorCall(
    uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4,
    uint64_t a5, uint64_t a6, uint64_t a7) -> const SMCReturnValue {
  SMCReturnValue result;
  register uint64_t x0 __asm__("x0") = a0;
  register uint64_t x1 __asm__("x1") = a1;
  register uint64_t x2 __asm__("x2") = a2;
  register uint64_t x3 __asm__("x3") = a3;
  register uint64_t x4 __asm__("x4") = a4;
  register uint64_t x5 __asm__("x5") = a5;
  register uint64_t x6 __asm__("x6") = a6;
  register uint64_t x7 __asm__("x7") = a7;

  __asm__ volatile("smc #0"
                   : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
                   : "r"(x4), "r"(x5), "r"(x6), "r"(x7)
                   : "memory");

  result.a0 = x0;
  result.a1 = x1;
  result.a2 = x2;
  result.a3 = x3;
  return result;
}

/**
 * @brief psci 接口
 * @see https://developer.arm.com/documentation/den0022/fb/?lang=en
 */
namespace psci {
static constexpr const uint64_t kVERSION = 0x84000000;
static constexpr const uint64_t kCPU_SUSPEND_32 = 0x84000001;
static constexpr const uint64_t kCPU_SUSPEND_64 = 0xC4000001;
static constexpr const uint64_t kCPU_OFF = 0x84000002;
static constexpr const uint64_t kCPU_ON_32 = 0x84000003;
static constexpr const uint64_t kCPU_ON_64 = 0xC4000003;
static constexpr const uint64_t kAFFINITY_INFO_32 = 0x84000004;
static constexpr const uint64_t kAFFINITY_INFO_64 = 0xC4000004;
static constexpr const uint64_t kMIGRATE_32 = 0x84000005;
static constexpr const uint64_t kMIGRATE_64 = 0xC4000005;
static constexpr const uint64_t kMIGRATE_INFO_TYPE = 0x84000006;
static constexpr const uint64_t kMIGRATE_INFO_UP_CPU_32 = 0x84000007;
static constexpr const uint64_t kMIGRATE_INFO_UP_CPU_64 = 0xC4000007;
static constexpr const uint64_t kSYSTEM_OFF = 0x84000008;
static constexpr const uint64_t kSYSTEM_RESET = 0x84000009;
static constexpr const uint64_t kSYSTEM_RESET2_32 = 0x84000012;
static constexpr const uint64_t kSYSTEM_RESET2_64 = 0xC4000012;
static constexpr const uint64_t kMEM_PROTECT = 0x84000013;
static constexpr const uint64_t kMEM_PROTECT_CHECK_RANGE_32 = 0x84000014;
static constexpr const uint64_t kMEM_PROTECT_CHECK_RANGE_64 = 0xC4000014;
static constexpr const uint64_t kFEATURES = 0x8400000A;
static constexpr const uint64_t kCPU_FREEZE = 0x8400000B;
static constexpr const uint64_t kCPU_DEFAULT_SUSPEND_32 = 0x8400000C;
static constexpr const uint64_t kCPU_DEFAULT_SUSPEND_64 = 0xC400000C;
static constexpr const uint64_t kNODE_HW_STATE_32 = 0x8400000D;
static constexpr const uint64_t kNODE_HW_STATE_64 = 0xC400000D;
static constexpr const uint64_t kSYSTEM_SUSPEND_32 = 0x8400000E;
static constexpr const uint64_t kSYSTEM_SUSPEND_64 = 0xC400000E;
static constexpr const uint64_t kSET_SUSPEND_MODE = 0x8400000F;
static constexpr const uint64_t kSTAT_RESIDENCY_32 = 0x84000010;
static constexpr const uint64_t kSTAT_RESIDENCY_64 = 0xC4000010;
static constexpr const uint64_t kSTAT_COUNT_32 = 0x84000011;
static constexpr const uint64_t kSTAT_COUNT_64 = 0xC4000011;

/// 错误码
enum ErrorCode {
  SUCCESS = 0,
  NOT_SUPPORTED = -1,
  INVALID_PARAMETERS = -2,
  DENIED = -3,
  ALREADY_ON = -4,
  ON_PENDING = -5,
  INTERNAL_FAILURE = -6,
  NOT_PRESENT = -7,
  DISABLED = -8,
  INVALID_ADDRESS = -9,
};

/// 电源状态编码
/// @see DEN0022F.b_Power_State_Coordination_Interface.pdf#6.5
struct StateID {
  // [15:12] Core is last in Power Level:
  // 0: Core Level
  // 1: Cluster Level
  // 2: System Level
  uint16_t core_last : 4;

  // [11:8] System Level Local Power State
  // 0: Run
  // 2: Retention
  // 3: Powerdown
  uint16_t system_state : 4;

  // [7:4] Cluster Level Local Power State
  // 0: Run
  // 2: Retention
  // 3: Powerdown
  uint16_t cluster_state : 4;

  // [3:0] Core Level Local Power State
  // 0: Run
  // 1: Standby
  // 2: Retention
  // 3: Powerdown
  uint16_t core_state : 4;
} __attribute__((packed));

/// 电源状态
/// @see DEN0022F.b_Power_State_Coordination_Interface.pdf#5.4.2
struct PowerState {
  // [31:26] Reserved. Must be zero.
  uint32_t reserved1 : 6;
  // [25:24] PowerLevel
  // Level 0: for cores
  // Level 1: for clusters
  // Level 2: for system
  uint32_t power_level : 2;
  // [23:17] Reserved. Must be zero.
  uint32_t reserved0 : 7;
  // [16] StateType
  uint32_t state_type : 1;
  // [15:0] StateID
  struct StateID state_id;
} __attribute__((packed));

/**
 * Suspend execution on a core or higher-level topology node.
 * @param power_state 电源状态
 * @param entry_point_address 用于恢复的地址
 * @param context_id 上下文标识
 * @return ErrorCode 错误码
 * @see DEN0022F.b_Power_State_Coordination_Interface.pdf#5.1.2
 */
static __always_inline auto CpuSuspend(PowerState power_state,
                                       uint64_t entry_point_address,
                                       uint64_t context_id) -> enum ErrorCode {
  (void)power_state; (void)entry_point_address; (void)context_id;
  return SUCCESS;
}

/**
 * Power down the calling core
 * @return ErrorCode 错误码
 * @see DEN0022F.b_Power_State_Coordination_Interface.pdf#5.1.3
 */
static __always_inline auto CpuOff() -> enum ErrorCode {
  return (ErrorCode)SecureMonitorCall(kCPU_OFF, 0, 0, 0, 0, 0, 0, 0).a0;
}

/**
 * Power up a core
 * @param target_cpu 低功耗状态
 * Bits[40:63]: Must be zero
 * Bits[32:39] Aff3: Match Aff3 of target core MPIDR
 * Bits[24:31] Must be zero
 * Bits[16:23] Aff2: Match Aff2 of target core MPIDR
 * Bits[8:15] Aff1: Match Aff1 of target core MPIDR
 * Bits[0:7] Aff0: Match Aff0 of target core MPIDR
 * @param entry_point_address 启动地址
 * @param context_id 上下文标识
 * @return ErrorCode 错误码
 * @see DEN0022F.b_Power_State_Coordination_Interface.pdf#5.1.4
 */
static __always_inline auto CpuOn(uint64_t target_cpu,
                                  uint64_t entry_point_address,
                                  uint64_t context_id) -> enum ErrorCode {
  return (ErrorCode)SecureMonitorCall(kCPU_ON_64, target_cpu,
                                      entry_point_address, context_id, 0, 0, 0,
                                      0)
      .a0;
}

}  // namespace psci
}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_CPU_HPP_
