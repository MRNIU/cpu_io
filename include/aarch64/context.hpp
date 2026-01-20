/**
 * @copyright Copyright The cpu_io Contributors
 */

#ifndef CPU_IO_INCLUDE_AARCH64_CONTEXT_HPP_
#define CPU_IO_INCLUDE_AARCH64_CONTEXT_HPP_

#include <cstdint>

namespace cpu_io {

/**
 * @brief AArch64 寄存器上下文结构体
 * 包含所有通用寄存器 (x0-x30)、必要的特权级系统寄存器以及浮点/SIMD寄存器
 * 用于中断/异常处理 (保存完整现场)
 *
 * 布局优化说明:
 * 按访问频率和语义分为三大块，提升缓存局部性和汇编访问效率：
 *
 * 1. 通用寄存器块 (256 字节, 0-255)
 *    - x0-x30 (31 个) + padding(1)
 *    - 每次中断必定访问，连续存储便于批量 STP/LDP 指令
 *
 * 2. 浮点寄存器块 (528 字节, 256-783)
 *    - q0-q31 (32 个 128 位) + fpsr + fpcr + padding(6)
 *    - 惰性保存优化：仅在任务使用浮点时保存
 *    - 128 位对齐，支持 STP/LDP q 指令
 *
 * 3. 系统寄存器块 (72 字节, 784-855)
 *    - sp_el0 + elr_el1 + spsr_el1 + esr_el1 + tpidr_el0 + tpidr_el1 +
 * ttbr0_el1 + ttbr1_el1 + sp_el1
 *    - 任务切换/页表切换相关，访问频率低
 *
 * 总计: 32 + 66 + 9 = 107 个 uint64_t = 856 字节 (16 字节对齐)
 *
 * 支持功能:
 * - 用户态/内核态切换 (sp/sp_el1, elr, spsr)
 * - 页表切换 (ttbr0)
 * - 任务调度 (tpidr_el1 指向 TCB)
 * - 线程本地存储 (tpidr/tpidr_el1)
 * - 惰性浮点保存 (FPU context switch optimization)
 */
struct TrapContext {
  // ==================== 通用寄存器块 (0-255 字节) ====================
  // x0-x7: 参数/结果寄存器
  uint64_t x0;
  uint64_t x1;
  uint64_t x2;
  uint64_t x3;
  uint64_t x4;
  uint64_t x5;
  uint64_t x6;
  uint64_t x7;
  // x8-x15: 间接结果位置寄存器 + 临时寄存器
  uint64_t x8;
  uint64_t x9;
  uint64_t x10;
  uint64_t x11;
  uint64_t x12;
  uint64_t x13;
  uint64_t x14;
  uint64_t x15;
  // x16-x17: 过程内调用临时寄存器
  uint64_t x16;
  uint64_t x17;
  // x18: 平台寄存器 (平台 ABI 保留)
  uint64_t x18;
  // x19-x28: 被调用者保存寄存器
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  // x29: 帧指针
  uint64_t x29;
  // x30: 链接寄存器 (返回地址)
  uint64_t x30;

  // 对齐填充 (保证下面的 q 寄存器从 256 字节开始，即 32 * uint64_t)
  uint64_t _padding0;

  // SIMD & 浮点寄存器 (每个 128 位，存储为 2 个 64 位)
  // 注意: 从偏移 256 开始，16 字节对齐，可使用 STP/LDP Qn 指令
  uint64_t q0[2];
  uint64_t q1[2];
  uint64_t q2[2];
  uint64_t q3[2];
  uint64_t q4[2];
  uint64_t q5[2];
  uint64_t q6[2];
  uint64_t q7[2];
  uint64_t q8[2];
  uint64_t q9[2];
  uint64_t q10[2];
  uint64_t q11[2];
  uint64_t q12[2];
  uint64_t q13[2];
  uint64_t q14[2];
  uint64_t q15[2];
  uint64_t q16[2];
  uint64_t q17[2];
  uint64_t q18[2];
  uint64_t q19[2];
  uint64_t q20[2];
  uint64_t q21[2];
  uint64_t q22[2];
  uint64_t q23[2];
  uint64_t q24[2];
  uint64_t q25[2];
  uint64_t q26[2];
  uint64_t q27[2];
  uint64_t q28[2];
  uint64_t q29[2];
  uint64_t q30[2];
  uint64_t q31[2];

  // 浮点状态寄存器
  uint64_t fpsr;
  // 浮点控制寄存器
  uint64_t fpcr;

  // 对齐填充 (保证系统寄存器块从 16 字节边界开始)
  uint64_t _padding1[6];

  // SP_EL0 (用户态栈指针)
  uint64_t sp_el0;
  // ELR_EL1 (异常链接寄存器 / 程序计数器)
  uint64_t elr_el1;
  // SPSR_EL1 (保存的程序状态寄存器)
  uint64_t spsr_el1;
  // ESR_EL1 (异常综合症寄存器 - 仅供参考)
  uint64_t esr_el1;
  // TPIDR_EL0 (用户态线程本地存储)
  uint64_t tpidr_el0;
  // TPIDR_EL1 (内核线程指针 - 当前任务 TCB)
  uint64_t tpidr_el1;
  // TTBR0_EL1 (用户空间页表基址)
  uint64_t ttbr0_el1;
  // SP_EL1 (内核栈指针 - 独立内核栈)
  uint64_t sp_el1;
};

/**
 * @brief 线程切换上下文 (SwitchTo)
 * 仅包含 Callee-saved 寄存器: x19-x30 (12个), sp, pc, q8-q15 (8个，每个 128
 * 位用 2 个 uint64_t 存储，共 16 个 uint64_t) 用于函数调用间的上下文切换
 * (Cooperative) 总计: 12 + 2 + 16 = 30 个 uint64_t
 * 共 30 * 8 = 240 字节
 */
struct CalleeSavedContext {
  // 被调用者保存的通用寄存器
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  // 帧指针
  uint64_t x29;
  // 链接寄存器
  uint64_t x30;

  // 被调用者保存的 SIMD & 浮点寄存器 (128 位，存储为 2 个 64 位)
  uint64_t q8[2];
  uint64_t q9[2];
  uint64_t q10[2];
  uint64_t q11[2];
  uint64_t q12[2];
  uint64_t q13[2];
  uint64_t q14[2];
  uint64_t q15[2];

  // 栈指针
  // SP_EL1 (内核栈指针 - 独立内核栈)
  uint64_t sp_el1;
  // 程序计数器
  uint64_t pc;
};

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_CONTEXT_HPP_
