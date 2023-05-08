/*
 * Copyright (c) 2021 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "arm_gemm.hpp"
#include <cstddef>
#include <cstdint>

#if defined(ARM_COMPUTE_ENABLE_SVE)

namespace arm_conv {
namespace depthwise {

void sve_s8q_packed_to_nhwc_5x5_s1_with_multiplier_output4x2_dot_depthfirst_impl(
  const int8_t *const *const inptrs,
  int8_t *const *const outptrs,
  const void *params,
  const unsigned int n_output_channels,
  const arm_gemm::Requantize32& qp
)
{
  __asm__ __volatile__(
    "mov z20.b, #0x1\n"
    "ldr x24, [%x[inptrs], #0x0]\n"
    "ptrue p2.b\n"
    "mov z22.s, #0x1\n"
    "ldr x23, [%x[inptrs], #0x8]\n"
    "lsl x9, %x[n_channels], #0x2\n"
    "mov z30.s, #0x0\n"
    "ldr x22, [%x[inptrs], #0x10]\n"
    "addvl SP, SP, #-8\n"
    "mov z28.s, #0x0\n"
    "ldr x21, [%x[inptrs], #0x18]\n"
    "mov x20, #0x6\n"
    "mov z29.s, #0x0\n"
    "ldr x19, [%x[inptrs], #0x20]\n"
    "whilelt p0.b, XZR, x20\n"
    "mov z27.s, #0x0\n"
    "ld1b { z0.b }, p0/Z, [x24]\n"
    "mov x28, #0x0\n"
    "mov z26.s, #0x0\n"
    "ld1b { z3.b }, p0/Z, [x23]\n"
    "mov x27, #0x0\n"
    "mov z25.s, #0x0\n"
    "ld1b { z5.b }, p0/Z, [x22]\n"
    "whilelt p1.b, x28, x9\n"
    "mov z15.d, z0.d\n"
    "ld1b { z4.b }, p0/Z, [x21]\n"
    "mov z24.s, #0x0\n"
    "ld1b { z6.b }, p0/Z, [x19]\n"
    "ext z15.b, z15.b, z15.b, #0x1\n"
    "ldr x21, [%x[inptrs], #0x28]\n"
    "mov z16.d, z3.d\n"
    "ldr x20, [%x[inptrs], #0x30]\n"
    "ext z16.b, z16.b, z16.b, #0x1\n"
    "ldr x19, [%x[inptrs], #0x38]\n"
    "mov z18.d, z5.d\n"
    "ld1b { z7.b }, p0/Z, [x21]\n"
    "zip1 z0.d, z0.d, z15.d\n"
    "ld1b { z1.b }, p0/Z, [x20]\n"
    "mov z0.q, z0.q[0]\n"
    "ld1b { z2.b }, p0/Z, [x19]\n"
    "zip1 z3.d, z3.d, z16.d\n"
    "ld1rw { z15.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_b_offset]]\n"
    "mov z3.q, z3.q[0]\n"
    "ldp x26, x25, [%x[outptrs], #0x0]\n"
    "ext z18.b, z18.b, z18.b, #0x1\n"
    "ldp x24, x23, [%x[outptrs], #0x10]\n"
    "mov z16.d, z4.d\n"
    "ldp x22, x21, [%x[outptrs], #0x20]\n"
    "ext z16.b, z16.b, z16.b, #0x1\n"
    "ldp x20, x19, [%x[outptrs], #0x30]\n"
    "mov z17.d, z6.d\n"
    "ld1rw { z14.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_c_offset]]\n"
    "zip1 z5.d, z5.d, z18.d\n"
    "ld1rw { z31.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_minval]]\n"
    "mov z5.q, z5.q[0]\n"
    "ld1rw { z12.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_maxval]]\n"
    "zip1 z4.d, z4.d, z16.d\n"
    "ld1w { z13.s }, p1/Z, [%x[params]]\n"
    "mov z4.q, z4.q[0]\n"
    "ld1b { z8.b }, p1/Z, [%x[params], #1, MUL VL]\n"
    "ext z17.b, z17.b, z17.b, #0x1\n"
    "ld1b { z9.b }, p1/Z, [%x[params], #2, MUL VL]\n"
    "mov z16.d, z7.d\n"
    "ld1b { z10.b }, p1/Z, [%x[params], #3, MUL VL]\n"
    "ext z16.b, z16.b, z16.b, #0x1\n"
    "ld1b { z11.b }, p1/Z, [%x[params], #4, MUL VL]\n"
    "addvl %x[params], %x[params], #5\n"
    "zip1 z6.d, z6.d, z17.d\n"
    "mov z17.d, z1.d\n"
    "mov z6.q, z6.q[0]\n"
    "zip1 z7.d, z7.d, z16.d\n"
    "mov z7.q, z7.q[0]\n"
    "ext z17.b, z17.b, z17.b, #0x1\n"
    "mov z16.d, z2.d\n"
    "ext z16.b, z16.b, z16.b, #0x1\n"
    "mov z23.s, #0x0\n"
    "zip1 z1.d, z1.d, z17.d\n"
    "mov z1.q, z1.q[0]\n"
    "zip1 z2.d, z2.d, z16.d\n"
    "mov z2.q, z2.q[0]\n"
    "mov z18.s, #0x0\n"
    "mov z17.s, #0x0\n"
    "mov z16.s, #0x0\n"
    "mov z21.s, #0x0\n"
    "mov z19.s, #0x0\n"
    "sdot z30.s, z20.b, z0.b[0]\n"
    "sdot z28.s, z20.b, z0.b[2]\n"
    "sdot z29.s, z20.b, z3.b[0]\n"
    "sdot z27.s, z20.b, z3.b[2]\n"
    "sdot z30.s, z22.b, z0.b[1]\n"
    "sdot z28.s, z22.b, z0.b[3]\n"
    "sdot z29.s, z22.b, z3.b[1]\n"
    "sdot z27.s, z22.b, z3.b[3]\n"
    "sdot z26.s, z20.b, z5.b[0]\n"
    "sdot z25.s, z20.b, z5.b[2]\n"
    "sdot z24.s, z20.b, z4.b[0]\n"
    "sdot z23.s, z20.b, z4.b[2]\n"
    "sdot z26.s, z22.b, z5.b[1]\n"
    "sdot z25.s, z22.b, z5.b[3]\n"
    "sdot z24.s, z22.b, z4.b[1]\n"
    "sdot z23.s, z22.b, z4.b[3]\n"
    "sdot z18.s, z20.b, z6.b[0]\n"
    "sdot z17.s, z20.b, z6.b[2]\n"
    "sdot z16.s, z20.b, z7.b[0]\n"
    "sdot z21.s, z20.b, z7.b[2]\n"
    "sdot z18.s, z22.b, z6.b[1]\n"
    "sdot z17.s, z22.b, z6.b[3]\n"
    "sdot z16.s, z22.b, z7.b[1]\n"
    "sdot z21.s, z22.b, z7.b[3]\n"
    "sdot z19.s, z20.b, z1.b[0]\n"
    "mov z30.d, z30.d\n"
    "mov z28.d, z28.d\n"
    "add z30.s, z30.s, z29.s\n"
    "sdot z19.s, z22.b, z1.b[1]\n"
    "add z28.s, z28.s, z27.s\n"
    "add z30.s, z30.s, z26.s\n"
    "mov z29.d, z29.d\n"
    "add z28.s, z28.s, z25.s\n"
    "add z30.s, z30.s, z24.s\n"
    "mov z27.d, z27.d\n"
    "add z28.s, z28.s, z23.s\n"
    "add z30.s, z30.s, z18.s\n"
    "add z29.s, z29.s, z26.s\n"
    "add z28.s, z28.s, z17.s\n"
    "add z27.s, z27.s, z25.s\n"
    "add z29.s, z29.s, z24.s\n"
    "mov z26.d, z26.d\n"
    "add z27.s, z27.s, z23.s\n"
    "add z29.s, z29.s, z18.s\n"
    "mov z25.d, z25.d\n"
    "add z27.s, z27.s, z17.s\n"
    "add z29.s, z29.s, z16.s\n"
    "add z26.s, z26.s, z24.s\n"
    "add z27.s, z27.s, z21.s\n"
    "add z25.s, z25.s, z23.s\n"
    "add z26.s, z26.s, z18.s\n"
    "mov z24.d, z24.d\n"
    "add z25.s, z25.s, z17.s\n"
    "add z26.s, z26.s, z16.s\n"
    "mov z23.d, z23.d\n"
    "add z25.s, z25.s, z21.s\n"
    "add z26.s, z26.s, z19.s\n"
    "add z24.s, z24.s, z18.s\n"
    "mov z18.s, #0x0\n"
    "sdot z18.s, z20.b, z1.b[2]\n"
    "add z23.s, z23.s, z17.s\n"
    "mov z17.s, #0x0\n"
    "sdot z17.s, z20.b, z2.b[0]\n"
    "sdot z18.s, z22.b, z1.b[3]\n"
    "add z24.s, z24.s, z16.s\n"
    "mov z16.s, #0x0\n"
    "sdot z17.s, z22.b, z2.b[1]\n"
    "sdot z16.s, z20.b, z2.b[2]\n"
    "add z25.s, z25.s, z18.s\n"
    "add z23.s, z23.s, z21.s\n"
    "add z24.s, z24.s, z19.s\n"
    "sdot z16.s, z22.b, z2.b[3]\n"
    "add z23.s, z23.s, z18.s\n"
    "add z24.s, z24.s, z17.s\n"
    "neg z15.s, p2/M, z15.s\n"
    "add z23.s, z23.s, z16.s\n"
    "mul z30.s, p2/M, z30.s, z15.s\n"
    "st1w { z30.s }, p2, [SP]\n"
    "add z30.s, z30.s, z13.s\n"
    "mul z28.s, p2/M, z28.s, z15.s\n"
    "st1w { z28.s }, p2, [SP, #1, MUL VL]\n"
    "add z28.s, z28.s, z13.s\n"
    "mul z29.s, p2/M, z29.s, z15.s\n"
    "st1w { z29.s }, p2, [SP, #2, MUL VL]\n"
    "add z29.s, z29.s, z13.s\n"
    "mul z27.s, p2/M, z27.s, z15.s\n"
    "st1w { z27.s }, p2, [SP, #3, MUL VL]\n"
    "add z27.s, z27.s, z13.s\n"
    "mul z26.s, p2/M, z26.s, z15.s\n"
    "st1w { z26.s }, p2, [SP, #4, MUL VL]\n"
    "add z26.s, z26.s, z13.s\n"
    "mul z25.s, p2/M, z25.s, z15.s\n"
    "st1w { z25.s }, p2, [SP, #5, MUL VL]\n"
    "add z25.s, z25.s, z13.s\n"
    "mul z24.s, p2/M, z24.s, z15.s\n"
    "st1w { z24.s }, p2, [SP, #6, MUL VL]\n"
    "add z24.s, z24.s, z13.s\n"
    "mul z23.s, p2/M, z23.s, z15.s\n"
    "st1w { z23.s }, p2, [SP, #7, MUL VL]\n"
    "add z23.s, z23.s, z13.s\n"
    "1:"  // Loop
    "sdot z30.s, z8.b, z0.b[0]\n"
    "ld1w { z22.s }, p2/Z, [%x[params], #6, MUL VL]\n"
    "incb x28\n"
    "sdot z28.s, z8.b, z0.b[2]\n"
    "ld1w { z21.s }, p2/Z, [%x[params], #7, MUL VL]\n"
    "whilelt p0.s, x27, %x[n_channels]\n"
    "sdot z29.s, z8.b, z3.b[0]\n"
    "whilelt p1.b, x28, x9\n"
    "sdot z27.s, z8.b, z3.b[2]\n"
    "sdot z26.s, z8.b, z5.b[0]\n"
    "sdot z25.s, z8.b, z5.b[2]\n"
    "sdot z24.s, z8.b, z4.b[0]\n"
    "sdot z23.s, z8.b, z4.b[2]\n"
    "ld1b { z8.b }, p2/Z, [%x[params]]\n"
    "sdot z30.s, z9.b, z0.b[1]\n"
    "sdot z28.s, z9.b, z0.b[3]\n"
    "sdot z29.s, z9.b, z3.b[1]\n"
    "sdot z27.s, z9.b, z3.b[3]\n"
    "sdot z26.s, z9.b, z5.b[1]\n"
    "sdot z25.s, z9.b, z5.b[3]\n"
    "sdot z24.s, z9.b, z4.b[1]\n"
    "sdot z23.s, z9.b, z4.b[3]\n"
    "ld1b { z9.b }, p2/Z, [%x[params], #1, MUL VL]\n"
    "sdot z30.s, z10.b, z3.b[0]\n"
    "sdot z28.s, z10.b, z3.b[2]\n"
    "sdot z29.s, z10.b, z5.b[0]\n"
    "sdot z27.s, z10.b, z5.b[2]\n"
    "sdot z26.s, z10.b, z4.b[0]\n"
    "sdot z25.s, z10.b, z4.b[2]\n"
    "sdot z24.s, z10.b, z6.b[0]\n"
    "sdot z23.s, z10.b, z6.b[2]\n"
    "ld1b { z10.b }, p2/Z, [%x[params], #2, MUL VL]\n"
    "sdot z30.s, z11.b, z3.b[1]\n"
    "sdot z28.s, z11.b, z3.b[3]\n"
    "sdot z29.s, z11.b, z5.b[1]\n"
    "sdot z27.s, z11.b, z5.b[3]\n"
    "sdot z26.s, z11.b, z4.b[1]\n"
    "sdot z25.s, z11.b, z4.b[3]\n"
    "sdot z24.s, z11.b, z6.b[1]\n"
    "sdot z23.s, z11.b, z6.b[3]\n"
    "ld1b { z11.b }, p2/Z, [%x[params], #3, MUL VL]\n"
    "sdot z30.s, z8.b, z5.b[0]\n"
    "sdot z28.s, z8.b, z5.b[2]\n"
    "sdot z29.s, z8.b, z4.b[0]\n"
    "sdot z27.s, z8.b, z4.b[2]\n"
    "sdot z26.s, z8.b, z6.b[0]\n"
    "sdot z25.s, z8.b, z6.b[2]\n"
    "sdot z24.s, z8.b, z7.b[0]\n"
    "sdot z23.s, z8.b, z7.b[2]\n"
    "ld1b { z8.b }, p2/Z, [%x[params], #4, MUL VL]\n"
    "sdot z30.s, z9.b, z5.b[1]\n"
    "sdot z28.s, z9.b, z5.b[3]\n"
    "sdot z29.s, z9.b, z4.b[1]\n"
    "sdot z27.s, z9.b, z4.b[3]\n"
    "sdot z26.s, z9.b, z6.b[1]\n"
    "sdot z25.s, z9.b, z6.b[3]\n"
    "sdot z24.s, z9.b, z7.b[1]\n"
    "sdot z23.s, z9.b, z7.b[3]\n"
    "ld1b { z9.b }, p2/Z, [%x[params], #5, MUL VL]\n"
    "addvl %x[params], %x[params], #16\n"
    "sdot z30.s, z10.b, z4.b[0]\n"
    "ld1w { z13.s }, p1/Z, [%x[params], #-8, MUL VL]\n"
    "sdot z28.s, z10.b, z4.b[2]\n"
    "sdot z29.s, z10.b, z6.b[0]\n"
    "sdot z27.s, z10.b, z6.b[2]\n"
    "sdot z26.s, z10.b, z7.b[0]\n"
    "sdot z25.s, z10.b, z7.b[2]\n"
    "sdot z24.s, z10.b, z1.b[0]\n"
    "sdot z23.s, z10.b, z1.b[2]\n"
    "ld1b { z10.b }, p1/Z, [%x[params], #-5, MUL VL]\n"
    "sdot z30.s, z11.b, z4.b[1]\n"
    "sdot z28.s, z11.b, z4.b[3]\n"
    "sdot z29.s, z11.b, z6.b[1]\n"
    "sdot z27.s, z11.b, z6.b[3]\n"
    "sdot z26.s, z11.b, z7.b[1]\n"
    "sdot z25.s, z11.b, z7.b[3]\n"
    "sdot z24.s, z11.b, z1.b[1]\n"
    "sdot z23.s, z11.b, z1.b[3]\n"
    "ld1b { z11.b }, p1/Z, [%x[params], #-4, MUL VL]\n"
    "sdot z30.s, z8.b, z6.b[0]\n"
    "sdot z28.s, z8.b, z6.b[2]\n"
    "sdot z29.s, z8.b, z7.b[0]\n"
    "sdot z27.s, z8.b, z7.b[2]\n"
    "sdot z26.s, z8.b, z1.b[0]\n"
    "sdot z25.s, z8.b, z1.b[2]\n"
    "sdot z24.s, z8.b, z2.b[0]\n"
    "sdot z23.s, z8.b, z2.b[2]\n"
    "ld1b { z8.b }, p1/Z, [%x[params], #-7, MUL VL]\n"
    "sdot z30.s, z9.b, z6.b[1]\n"
    "sdot z28.s, z9.b, z6.b[3]\n"
    "sdot z29.s, z9.b, z7.b[1]\n"
    "sdot z27.s, z9.b, z7.b[3]\n"
    "sdot z26.s, z9.b, z1.b[1]\n"
    "sdot z25.s, z9.b, z1.b[3]\n"
    "sdot z24.s, z9.b, z2.b[1]\n"
    "sdot z23.s, z9.b, z2.b[3]\n"
    "ld1b { z9.b }, p1/Z, [%x[params], #-6, MUL VL]\n"
    "addvl %x[params], %x[params], #-3\n"
    ".inst 0x04b677de  // sqrdmulh z30.s, z30.s, z22.s\n"
    ".inst 0x04b6779c  // sqrdmulh z28.s, z28.s, z22.s\n"
    ".inst 0x04b677bd  // sqrdmulh z29.s, z29.s, z22.s\n"
    ".inst 0x04b6777b  // sqrdmulh z27.s, z27.s, z22.s\n"
    ".inst 0x04b6775a  // sqrdmulh z26.s, z26.s, z22.s\n"
    "and z20.d, z30.d, z21.d\n"
    "asr z20.s, z20.s, #0x1f\n"
    "and z19.d, z28.d, z21.d\n"
    "and z18.d, z29.d, z21.d\n"
    "asr z19.s, z19.s, #0x1f\n"
    "and z17.d, z27.d, z21.d\n"
    "and z16.d, z26.d, z21.d\n"
    "asr z18.s, z18.s, #0x1f\n"
    ".inst 0x04b67739  // sqrdmulh z25.s, z25.s, z22.s\n"
    "asr z17.s, z17.s, #0x1f\n"
    "sqadd z30.s, z30.s, z20.s\n"
    ".inst 0x04b67718  // sqrdmulh z24.s, z24.s, z22.s\n"
    "asr z16.s, z16.s, #0x1f\n"
    ".inst 0x04b676f7  // sqrdmulh z23.s, z23.s, z22.s\n"
    "sqadd z28.s, z28.s, z19.s\n"
    "sqadd z29.s, z29.s, z18.s\n"
    "and z18.d, z25.d, z21.d\n"
    "asr z18.s, z18.s, #0x1f\n"
    "sqadd z27.s, z27.s, z17.s\n"
    "sqadd z26.s, z26.s, z16.s\n"
    "and z17.d, z24.d, z21.d\n"
    "asr z17.s, z17.s, #0x1f\n"
    "and z16.d, z23.d, z21.d\n"
    ".inst 0x44828abe  // srshl z30.s, p2/M, z30.s, z21.s\n"
    "asr z16.s, z16.s, #0x1f\n"
    "sqadd z25.s, z25.s, z18.s\n"
    ".inst 0x44828abc  // srshl z28.s, p2/M, z28.s, z21.s\n"
    "add z30.s, z30.s, z14.s\n"
    "sqadd z24.s, z24.s, z17.s\n"
    ".inst 0x44828abd  // srshl z29.s, p2/M, z29.s, z21.s\n"
    "add z28.s, z28.s, z14.s\n"
    "sqadd z23.s, z23.s, z16.s\n"
    "smin z30.s, p2/M, z30.s, z12.s\n"
    "add z29.s, z29.s, z14.s\n"
    "smin z28.s, p2/M, z28.s, z12.s\n"
    ".inst 0x44828abb  // srshl z27.s, p2/M, z27.s, z21.s\n"
    "smax z30.s, p2/M, z30.s, z31.s\n"
    "st1b { z30.s }, p0, [x26, x27]\n"
    "add z27.s, z27.s, z14.s\n"
    "smax z28.s, p2/M, z28.s, z31.s\n"
    "ld1w { z30.s }, p2/Z, [SP]\n"
    "smin z29.s, p2/M, z29.s, z12.s\n"
    "st1b { z28.s }, p0, [x25, x27]\n"
    "add z30.s, z30.s, z13.s\n"
    "smin z27.s, p2/M, z27.s, z12.s\n"
    "ld1w { z28.s }, p2/Z, [SP, #1, MUL VL]\n"
    "smax z29.s, p2/M, z29.s, z31.s\n"
    "st1b { z29.s }, p0, [x24, x27]\n"
    "add z28.s, z28.s, z13.s\n"
    "smax z27.s, p2/M, z27.s, z31.s\n"
    "ld1w { z29.s }, p2/Z, [SP, #2, MUL VL]\n"
    ".inst 0x44828aba  // srshl z26.s, p2/M, z26.s, z21.s\n"
    "st1b { z27.s }, p0, [x23, x27]\n"
    "add z29.s, z29.s, z13.s\n"
    ".inst 0x44828ab9  // srshl z25.s, p2/M, z25.s, z21.s\n"
    "ld1w { z27.s }, p2/Z, [SP, #3, MUL VL]\n"
    "add z26.s, z26.s, z14.s\n"
    ".inst 0x44828ab8  // srshl z24.s, p2/M, z24.s, z21.s\n"
    ".inst 0x44828ab7  // srshl z23.s, p2/M, z23.s, z21.s\n"
    "add z25.s, z25.s, z14.s\n"
    "add z27.s, z27.s, z13.s\n"
    "add z24.s, z24.s, z14.s\n"
    "add z23.s, z23.s, z14.s\n"
    "smin z26.s, p2/M, z26.s, z12.s\n"
    "smin z25.s, p2/M, z25.s, z12.s\n"
    "smin z24.s, p2/M, z24.s, z12.s\n"
    "smin z23.s, p2/M, z23.s, z12.s\n"
    "smax z26.s, p2/M, z26.s, z31.s\n"
    "st1b { z26.s }, p0, [x22, x27]\n"
    "smax z25.s, p2/M, z25.s, z31.s\n"
    "smax z24.s, p2/M, z24.s, z31.s\n"
    "ld1w { z26.s }, p2/Z, [SP, #4, MUL VL]\n"
    "smax z23.s, p2/M, z23.s, z31.s\n"
    "st1b { z25.s }, p0, [x21, x27]\n"
    "add z26.s, z26.s, z13.s\n"
    "st1b { z24.s }, p0, [x20, x27]\n"
    "st1b { z23.s }, p0, [x19, x27]\n"
    "incw x27\n"
    "ld1w { z25.s }, p2/Z, [SP, #5, MUL VL]\n"
    "add z25.s, z25.s, z13.s\n"
    "ld1w { z24.s }, p2/Z, [SP, #6, MUL VL]\n"
    "ld1w { z23.s }, p2/Z, [SP, #7, MUL VL]\n"
    "add z24.s, z24.s, z13.s\n"
    "add z23.s, z23.s, z13.s\n"
    "b.any 1b\n"
    "addvl SP, SP, #8\n"
    : [params] "+&r" (params)
    : [inptrs] "r" (inptrs), [n_channels] "r" (n_output_channels), [offsetof_Requantize32_b_offset] "I" (offsetof(arm_gemm::Requantize32, b_offset)), [offsetof_Requantize32_c_offset] "I" (offsetof(arm_gemm::Requantize32, c_offset)), [offsetof_Requantize32_maxval] "I" (offsetof(arm_gemm::Requantize32, maxval)), [offsetof_Requantize32_minval] "I" (offsetof(arm_gemm::Requantize32, minval)), [outptrs] "r" (outptrs), [qp] "r" (&qp)
    : "cc", "memory", "p0", "p1", "p2", "x9", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
  );
}

}  // namespace depthwise
}  // namespace arm_conv

#endif  // defined(ARM_COMPUTE_ENABLE_SVE)
