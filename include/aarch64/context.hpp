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
 * 31 个通用寄存器(x0-x30) + 2 个特殊寄存器(sp, pc) + 32
 * 个浮点寄存器(q0-q31，每个 128 位用 2 个 uint64_t 存储，共 64 个 uint64_t) +
 * 3 个状态寄存器(fpsr, fpcr, esr_elx)
 * 总计: 31 + 2 + 64 + 3 = 100 个 uint64_t
 * 共 100 * 8 = 800 字节
 */
struct TrapContext {
  // General Purpose Registers
  // x0-x7: Argument/Result Registers
  uint64_t x0;
  uint64_t x1;
  uint64_t x2;
  uint64_t x3;
  uint64_t x4;
  uint64_t x5;
  uint64_t x6;
  uint64_t x7;
  // x8-x15: Indirect Result Location Register + Temporary Registers
  uint64_t x8;
  uint64_t x9;
  uint64_t x10;
  uint64_t x11;
  uint64_t x12;
  uint64_t x13;
  uint64_t x14;
  uint64_t x15;
  // x16-x17: Intra-procedure-call Temporary Registers
  uint64_t x16;
  uint64_t x17;
  // x18: Platform Register (reserved by platform ABI)
  uint64_t x18;
  // x19-x28: Callee-saved Registers
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
  // x29: Frame Pointer
  uint64_t x29;
  // x30: Link Register (Return Address)
  uint64_t x30;

  // Stack Pointer
  uint64_t sp;
  // Program Counter (Exception Link Register - ELR_ELx)
  uint64_t pc;

  // SIMD & Floating Point Registers (128-bit each, stored as 2 x 64-bit)
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

  // Floating Point Status Register
  uint64_t fpsr;
  // Floating Point Control Register
  uint64_t fpcr;

  // Exception Syndrome Register
  uint64_t esr_elx;
};

/**
 * @brief 线程切换上下文 (SwitchTo)
 * 仅包含 Callee-saved 寄存器: x19-x30 (12个), sp, pc, q8-q15 (8个，每个 128
 * 位用 2 个 uint64_t 存储，共 16 个 uint64_t) 用于函数调用间的上下文切换
 * (Cooperative) 总计: 12 + 2 + 16 = 30 个 uint64_t 
 * 共 30 * 8 = 240 字节
 */
struct CalleeSavedContext {
  // Callee-saved General Purpose Registers
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
  // Frame Pointer
  uint64_t x29;
  // Link Register
  uint64_t x30;

  // Stack Pointer
  uint64_t sp;
  // Program Counter
  uint64_t pc;

  // Callee-saved SIMD & Floating Point Registers (128-bit, stored as 2 x
  // 64-bit)
  uint64_t q8[2];
  uint64_t q9[2];
  uint64_t q10[2];
  uint64_t q11[2];
  uint64_t q12[2];
  uint64_t q13[2];
  uint64_t q14[2];
  uint64_t q15[2];
};

}  // namespace cpu_io

#endif  // CPU_IO_INCLUDE_AARCH64_CONTEXT_HPP_
