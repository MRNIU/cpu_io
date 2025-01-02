
/**
 * @file cpu_io.h
 * @brief cpu_io 头文件
 * @author Zone.N (Zone.Niuzh@hotmail.com)
 * @version 1.0
 * @date 2025-01-02
 * @copyright MIT LICENSE
 * https://github.com/MRNIU/cpu_io
 * @par change log:
 * <table>
 * <tr><th>Date<th>Author<th>Description
 * <tr><td>2025-01-02<td>Zone.N (Zone.Niuzh@hotmail.com)<td>创建文件
 * </table>
 */

#ifndef CPU_IO_INCLUDE_CPU_IO_H_
#define CPU_IO_INCLUDE_CPU_IO_H_

#ifdef __x86_64__
#include "x86_64/cpu.hpp"
#elif __riscv
#include "riscv64/cpu.hpp"
#elif __aarch64__
#include "aarch64/cpu.hpp"
#endif

#endif /* CPU_IO_INCLUDE_CPU_IO_H_ */
