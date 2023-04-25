/*
 * Copyright (c) 2021-2022 Arm Limited.
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

#if defined(ARM_COMPUTE_ENABLE_SVE)

#include "arm_gemm.hpp"
#include <cstdint>

namespace arm_conv {
namespace depthwise {

void sve_s8qs_nhwc_3x3_s1_output2x2_dot_depthfirst_impl(
  const unsigned int n_channels,
  const int8_t *const *const inptrs,
  const int8_t *params,
  const int32_t *,  // Bias, should be wrapped into the parameters
  const arm_gemm::Requantize32& qp,
  const int32_t *, const int32_t *,  // Requant parameters, also wrapped
  int8_t *const *const outptrs
)
{
  __asm__ __volatile__(
    "ldp x11, x10, [%x[inptrs], #0x0]\n"
    "ptrue p2.b\n"
    "ldp x9, x28, [%x[inptrs], #0x10]\n"
    "addvl SP, SP, #-8\n"
    "ldp x27, x26, [%x[inptrs], #0x20]\n"
    "mov x25, #0x0\n"
    "ldp x24, x23, [%x[inptrs], #0x30]\n"
    "whilelt p1.b, x25, %x[n_channels]\n"
    "ldp x22, x21, [%x[outptrs], #0x0]\n"
    "ldp x20, x19, [%x[outptrs], #0x10]\n"
    "ld1rw { z6.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_minval]]\n"
    "ld1rw { z5.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_maxval]]\n"
    "ld1rw { z4.s }, p2/Z, [%x[qp], %[offsetof_Requantize32_c_offset]]\n"
    "1:"  // Loop
    "ld1b { z19.b }, p1/Z, [x11, x25]\n"
    "whilelt p0.s, x25, %x[n_channels]\n"
    "ld1b { z18.b }, p1/Z, [x10, x25]\n"
    "ldp x11, x10, [%x[inptrs], #0x40]\n"
    "ld1b { z16.b }, p1/Z, [x9, x25]\n"
    "zip1 z21.b, z19.b, z16.b\n"
    "ld1b { z17.b }, p1/Z, [x28, x25]\n"
    "zip2 z19.b, z19.b, z16.b\n"
    "ldp x9, x28, [%x[inptrs], #0x50]\n"
    "ld1b { z23.b }, p1/Z, [x27, x25]\n"
    "zip1 z16.b, z18.b, z17.b\n"
    "ld1b { z20.b }, p1/Z, [x26, x25]\n"
    "zip2 z18.b, z18.b, z17.b\n"
    "ldp x27, x26, [%x[inptrs], #0x60]\n"
    "zip1 z3.b, z21.b, z16.b\n"
    "ld1b { z17.b }, p1/Z, [x24, x25]\n"
    "zip2 z2.b, z21.b, z16.b\n"
    "ld1b { z16.b }, p1/Z, [x23, x25]\n"
    "zip1 z29.b, z19.b, z18.b\n"
    "ldp x24, x23, [%x[inptrs], #0x70]\n"
    "zip2 z28.b, z19.b, z18.b\n"
    "ld1b { z22.b }, p1/Z, [x11, x25]\n"
    "zip1 z19.b, z23.b, z17.b\n"
    "ld1b { z21.b }, p1/Z, [x10, x25]\n"
    "zip2 z27.b, z23.b, z17.b\n"
    "ldp x11, x10, [%x[inptrs], #0x0]\n"
    "zip1 z18.b, z20.b, z16.b\n"
    "ld1b { z17.b }, p1/Z, [x9, x25]\n"
    "zip2 z20.b, z20.b, z16.b\n"
    "ld1b { z16.b }, p1/Z, [x28, x25]\n"
    "zip1 z1.b, z19.b, z18.b\n"
    "ldp x9, x28, [%x[inptrs], #0x10]\n"
    "zip2 z0.b, z19.b, z18.b\n"
    "ld1b { z19.b }, p1/Z, [x27, x25]\n"
    "zip1 z26.b, z22.b, z17.b\n"
    "ld1b { z25.b }, p1/Z, [x26, x25]\n"
    "zip2 z24.b, z22.b, z17.b\n"
    "ldp x27, x26, [%x[inptrs], #0x20]\n"
    "zip1 z23.b, z21.b, z16.b\n"
    "ld1b { z18.b }, p1/Z, [x24, x25]\n"
    "zip2 z22.b, z21.b, z16.b\n"
    "ld1b { z21.b }, p1/Z, [x23, x25]\n"
    "zip1 z17.b, z27.b, z20.b\n"
    "ldp x24, x23, [%x[inptrs], #0x30]\n"
    "zip2 z16.b, z27.b, z20.b\n"
    "st1b { z29.b }, p2, [SP]\n"
    "zip1 z20.b, z19.b, z18.b\n"
    "st1b { z28.b }, p2, [SP, #1, MUL VL]\n"
    "zip2 z19.b, z19.b, z18.b\n"
    "st1b { z17.b }, p2, [SP, #2, MUL VL]\n"
    "zip1 z18.b, z25.b, z21.b\n"
    "st1b { z16.b }, p2, [SP, #3, MUL VL]\n"
    "zip2 z17.b, z25.b, z21.b\n"
    "ld1w { z31.s }, p2/Z, [%x[params]]\n"
    "zip1 z30.b, z26.b, z23.b\n"
    "ld1b { z29.b }, p2/Z, [%x[params], #1, MUL VL]\n"
    "zip2 z28.b, z26.b, z23.b\n"
    "ld1b { z27.b }, p2/Z, [%x[params], #2, MUL VL]\n"
    "zip1 z16.b, z24.b, z22.b\n"
    "st1b { z16.b }, p2, [SP, #4, MUL VL]\n"
    "zip2 z16.b, z24.b, z22.b\n"
    "st1b { z16.b }, p2, [SP, #5, MUL VL]\n"
    "zip1 z26.b, z20.b, z18.b\n"
    "ld1b { z25.b }, p2/Z, [%x[params], #3, MUL VL]\n"
    "zip2 z24.b, z20.b, z18.b\n"
    "ld1w { z23.s }, p2/Z, [%x[params], #4, MUL VL]\n"
    "zip1 z16.b, z19.b, z17.b\n"
    "st1b { z16.b }, p2, [SP, #6, MUL VL]\n"
    "zip2 z16.b, z19.b, z17.b\n"
    "st1b { z16.b }, p2, [SP, #7, MUL VL]\n"
    "mov z22.d, z31.d\n"
    "ld1w { z21.s }, p2/Z, [%x[params], #5, MUL VL]\n"
    "mov z20.d, z31.d\n"
    "mov z19.d, z31.d\n"
    "sdot z31.s, z29.b, z3.b\n"
    "sdot z20.s, z29.b, z1.b\n"
    "ext z3.b, z3.b, z3.b, #0x1\n"
    "sdot z31.s, z27.b, z1.b\n"
    "ext z1.b, z1.b, z1.b, #0x1\n"
    "sdot z20.s, z27.b, z30.b\n"
    "sdot z22.s, z29.b, z3.b\n"
    "ld1b { z3.b }, p2/Z, [SP]\n"
    "sdot z31.s, z25.b, z30.b\n"
    "ext z30.b, z30.b, z30.b, #0x1\n"
    "sdot z20.s, z25.b, z26.b\n"
    "ext z26.b, z26.b, z26.b, #0x1\n"
    "sdot z19.s, z29.b, z1.b\n"
    "ld1b { z29.b }, p2/Z, [%x[params], #7, MUL VL]\n"
    "sdot z22.s, z27.b, z1.b\n"
    "ld1b { z1.b }, p2/Z, [SP, #2, MUL VL]\n"
    ".inst 0x04b777ff  // sqrdmulh z31.s, z31.s, z23.s\n"
    ".inst 0x04b77694  // sqrdmulh z20.s, z20.s, z23.s\n"
    "sdot z19.s, z27.b, z30.b\n"
    "sdot z22.s, z25.b, z30.b\n"
    "ld1b { z30.b }, p2/Z, [SP, #4, MUL VL]\n"
    "and z16.d, z31.d, z21.d\n"
    "asr z16.s, z16.s, #0x1f\n"
    "sdot z19.s, z25.b, z26.b\n"
    "ld1b { z26.b }, p2/Z, [SP, #6, MUL VL]\n"
    ".inst 0x04b776d6  // sqrdmulh z22.s, z22.s, z23.s\n"
    "and z18.d, z20.d, z21.d\n"
    "asr z18.s, z18.s, #0x1f\n"
    ".inst 0x04b77673  // sqrdmulh z19.s, z19.s, z23.s\n"
    "sqadd z31.s, z31.s, z16.s\n"
    "and z17.d, z22.d, z21.d\n"
    "asr z17.s, z17.s, #0x1f\n"
    "and z16.d, z19.d, z21.d\n"
    "sqadd z20.s, z20.s, z18.s\n"
    "asr z16.s, z16.s, #0x1f\n"
    ".inst 0x44828abf  // srshl z31.s, p2/M, z31.s, z21.s\n"
    "sqadd z22.s, z22.s, z17.s\n"
    ".inst 0x44828ab4  // srshl z20.s, p2/M, z20.s, z21.s\n"
    "add z31.s, z31.s, z4.s\n"
    "sqadd z19.s, z19.s, z16.s\n"
    "add z20.s, z20.s, z4.s\n"
    ".inst 0x44828ab6  // srshl z22.s, p2/M, z22.s, z21.s\n"
    "smax z31.s, p2/M, z31.s, z6.s\n"
    "smax z20.s, p2/M, z20.s, z6.s\n"
    ".inst 0x44828ab3  // srshl z19.s, p2/M, z19.s, z21.s\n"
    "add z22.s, z22.s, z4.s\n"
    "smin z31.s, p2/M, z31.s, z5.s\n"
    "st1b { z31.s }, p0, [x22, x25]\n"
    "add z19.s, z19.s, z4.s\n"
    "smax z22.s, p2/M, z22.s, z6.s\n"
    "ld1w { z31.s }, p2/Z, [%x[params], #6, MUL VL]\n"
    "addvl %x[params], %x[params], #16\n"
    "smin z20.s, p2/M, z20.s, z5.s\n"
    "ld1b { z27.b }, p2/Z, [%x[params], #-8, MUL VL]\n"
    "ld1b { z25.b }, p2/Z, [%x[params], #-7, MUL VL]\n"
    "smax z19.s, p2/M, z19.s, z6.s\n"
    "ld1w { z23.s }, p2/Z, [%x[params], #-6, MUL VL]\n"
    "smin z22.s, p2/M, z22.s, z5.s\n"
    "ld1w { z21.s }, p2/Z, [%x[params], #-5, MUL VL]\n"
    "smin z19.s, p2/M, z19.s, z5.s\n"
    "st1b { z20.s }, p0, [x20, x25]\n"
    "mov z20.d, z31.d\n"
    "st1b { z22.s }, p0, [x21, x25]\n"
    "mov z22.d, z31.d\n"
    "st1b { z19.s }, p0, [x19, x25]\n"
    "mov z19.d, z31.d\n"
    "incw x25\n"
    "sdot z31.s, z29.b, z2.b\n"
    "whilelt p0.s, x25, %x[n_channels]\n"
    "sdot z20.s, z29.b, z0.b\n"
    "ext z2.b, z2.b, z2.b, #0x1\n"
    "sdot z31.s, z27.b, z0.b\n"
    "sdot z20.s, z27.b, z28.b\n"
    "ext z0.b, z0.b, z0.b, #0x1\n"
    "sdot z22.s, z29.b, z2.b\n"
    "ld1b { z2.b }, p2/Z, [SP, #1, MUL VL]\n"
    "sdot z31.s, z25.b, z28.b\n"
    "sdot z20.s, z25.b, z24.b\n"
    "ext z28.b, z28.b, z28.b, #0x1\n"
    "ext z24.b, z24.b, z24.b, #0x1\n"
    "sdot z19.s, z29.b, z0.b\n"
    "ld1b { z29.b }, p2/Z, [%x[params], #-3, MUL VL]\n"
    "sdot z22.s, z27.b, z0.b\n"
    "ld1b { z0.b }, p2/Z, [SP, #3, MUL VL]\n"
    ".inst 0x04b777ff  // sqrdmulh z31.s, z31.s, z23.s\n"
    ".inst 0x04b77694  // sqrdmulh z20.s, z20.s, z23.s\n"
    "sdot z19.s, z27.b, z28.b\n"
    "ld1b { z27.b }, p2/Z, [%x[params], #-2, MUL VL]\n"
    "sdot z22.s, z25.b, z28.b\n"
    "ld1b { z28.b }, p2/Z, [SP, #5, MUL VL]\n"
    "and z16.d, z31.d, z21.d\n"
    "asr z16.s, z16.s, #0x1f\n"
    "sdot z19.s, z25.b, z24.b\n"
    "ld1b { z25.b }, p2/Z, [%x[params], #-1, MUL VL]\n"
    ".inst 0x04b776d6  // sqrdmulh z22.s, z22.s, z23.s\n"
    "ld1b { z24.b }, p2/Z, [SP, #7, MUL VL]\n"
    "and z18.d, z20.d, z21.d\n"
    "asr z18.s, z18.s, #0x1f\n"
    ".inst 0x04b77673  // sqrdmulh z19.s, z19.s, z23.s\n"
    "ld1w { z23.s }, p2/Z, [%x[params]]\n"
    "sqadd z31.s, z31.s, z16.s\n"
    "and z17.d, z22.d, z21.d\n"
    "asr z17.s, z17.s, #0x1f\n"
    "and z16.d, z19.d, z21.d\n"
    "sqadd z20.s, z20.s, z18.s\n"
    "asr z16.s, z16.s, #0x1f\n"
    ".inst 0x44828abf  // srshl z31.s, p2/M, z31.s, z21.s\n"
    "sqadd z22.s, z22.s, z17.s\n"
    ".inst 0x44828ab4  // srshl z20.s, p2/M, z20.s, z21.s\n"
    "add z31.s, z31.s, z4.s\n"
    "sqadd z19.s, z19.s, z16.s\n"
    "add z20.s, z20.s, z4.s\n"
    ".inst 0x44828ab6  // srshl z22.s, p2/M, z22.s, z21.s\n"
    "smax z31.s, p2/M, z31.s, z6.s\n"
    "smax z20.s, p2/M, z20.s, z6.s\n"
    ".inst 0x44828ab3  // srshl z19.s, p2/M, z19.s, z21.s\n"
    "ld1w { z21.s }, p2/Z, [%x[params], #1, MUL VL]\n"
    "add z22.s, z22.s, z4.s\n"
    "smin z31.s, p2/M, z31.s, z5.s\n"
    "st1b { z31.s }, p0, [x22, x25]\n"
    "add z19.s, z19.s, z4.s\n"
    "smax z22.s, p2/M, z22.s, z6.s\n"
    "ld1w { z31.s }, p2/Z, [%x[params], #-4, MUL VL]\n"
    "smin z20.s, p2/M, z20.s, z5.s\n"
    "st1b { z20.s }, p0, [x20, x25]\n"
    "mov z20.d, z31.d\n"
    "smin z22.s, p2/M, z22.s, z5.s\n"
    "st1b { z22.s }, p0, [x21, x25]\n"
    "mov z22.d, z31.d\n"
    "sdot z20.s, z29.b, z1.b\n"
    "smax z19.s, p2/M, z19.s, z6.s\n"
    "sdot z20.s, z27.b, z30.b\n"
    "smin z19.s, p2/M, z19.s, z5.s\n"
    "st1b { z19.s }, p0, [x19, x25]\n"
    "mov z19.d, z31.d\n"
    "incw x25\n"
    "sdot z31.s, z29.b, z3.b\n"
    "whilelt p0.s, x25, %x[n_channels]\n"
    "sdot z20.s, z25.b, z26.b\n"
    "ext z3.b, z3.b, z3.b, #0x1\n"
    "ext z26.b, z26.b, z26.b, #0x1\n"
    "sdot z31.s, z27.b, z1.b\n"
    "ext z1.b, z1.b, z1.b, #0x1\n"
    "sdot z22.s, z29.b, z3.b\n"
    ".inst 0x04b77694  // sqrdmulh z20.s, z20.s, z23.s\n"
    "sdot z31.s, z25.b, z30.b\n"
    "ext z30.b, z30.b, z30.b, #0x1\n"
    "sdot z19.s, z29.b, z1.b\n"
    "ld1b { z29.b }, p2/Z, [%x[params], #3, MUL VL]\n"
    "sdot z22.s, z27.b, z1.b\n"
    "and z18.d, z20.d, z21.d\n"
    "asr z18.s, z18.s, #0x1f\n"
    "sdot z19.s, z27.b, z30.b\n"
    "ld1b { z27.b }, p2/Z, [%x[params], #4, MUL VL]\n"
    "sdot z22.s, z25.b, z30.b\n"
    ".inst 0x04b777ff  // sqrdmulh z31.s, z31.s, z23.s\n"
    "sdot z19.s, z25.b, z26.b\n"
    "ld1b { z25.b }, p2/Z, [%x[params], #5, MUL VL]\n"
    "and z16.d, z31.d, z21.d\n"
    "asr z16.s, z16.s, #0x1f\n"
    ".inst 0x04b776d6  // sqrdmulh z22.s, z22.s, z23.s\n"
    "sqadd z20.s, z20.s, z18.s\n"
    ".inst 0x04b77673  // sqrdmulh z19.s, z19.s, z23.s\n"
    "ld1w { z23.s }, p2/Z, [%x[params], #6, MUL VL]\n"
    "and z17.d, z22.d, z21.d\n"
    "asr z17.s, z17.s, #0x1f\n"
    "sqadd z31.s, z31.s, z16.s\n"
    "and z16.d, z19.d, z21.d\n"
    "asr z16.s, z16.s, #0x1f\n"
    ".inst 0x44828ab4  // srshl z20.s, p2/M, z20.s, z21.s\n"
    ".inst 0x44828abf  // srshl z31.s, p2/M, z31.s, z21.s\n"
    "sqadd z22.s, z22.s, z17.s\n"
    "add z20.s, z20.s, z4.s\n"
    "add z31.s, z31.s, z4.s\n"
    "sqadd z19.s, z19.s, z16.s\n"
    ".inst 0x44828ab6  // srshl z22.s, p2/M, z22.s, z21.s\n"
    "smax z20.s, p2/M, z20.s, z6.s\n"
    "smax z31.s, p2/M, z31.s, z6.s\n"
    ".inst 0x44828ab3  // srshl z19.s, p2/M, z19.s, z21.s\n"
    "ld1w { z21.s }, p2/Z, [%x[params], #7, MUL VL]\n"
    "add z22.s, z22.s, z4.s\n"
    "smin z20.s, p2/M, z20.s, z5.s\n"
    "st1b { z20.s }, p0, [x20, x25]\n"
    "add z19.s, z19.s, z4.s\n"
    "smin z31.s, p2/M, z31.s, z5.s\n"
    "st1b { z31.s }, p0, [x22, x25]\n"
    "smax z22.s, p2/M, z22.s, z6.s\n"
    "smax z19.s, p2/M, z19.s, z6.s\n"
    "ld1w { z31.s }, p2/Z, [%x[params], #2, MUL VL]\n"
    "addvl %x[params], %x[params], #8\n"
    "mov z20.d, z31.d\n"
    "smin z22.s, p2/M, z22.s, z5.s\n"
    "st1b { z22.s }, p0, [x21, x25]\n"
    "mov z22.d, z31.d\n"
    "sdot z20.s, z29.b, z0.b\n"
    "smin z19.s, p2/M, z19.s, z5.s\n"
    "st1b { z19.s }, p0, [x19, x25]\n"
    "mov z19.d, z31.d\n"
    "incw x25\n"
    "sdot z31.s, z29.b, z2.b\n"
    "whilelt p0.s, x25, %x[n_channels]\n"
    "sdot z20.s, z27.b, z28.b\n"
    "ext z2.b, z2.b, z2.b, #0x1\n"
    "sdot z31.s, z27.b, z0.b\n"
    "sdot z20.s, z25.b, z24.b\n"
    "ext z0.b, z0.b, z0.b, #0x1\n"
    "ext z24.b, z24.b, z24.b, #0x1\n"
    "sdot z22.s, z29.b, z2.b\n"
    "sdot z31.s, z25.b, z28.b\n"
    "ext z28.b, z28.b, z28.b, #0x1\n"
    "sdot z19.s, z29.b, z0.b\n"
    "sdot z22.s, z27.b, z0.b\n"
    ".inst 0x04b777ff  // sqrdmulh z31.s, z31.s, z23.s\n"
    ".inst 0x04b77694  // sqrdmulh z20.s, z20.s, z23.s\n"
    "sdot z19.s, z27.b, z28.b\n"
    "sdot z22.s, z25.b, z28.b\n"
    "and z16.d, z31.d, z21.d\n"
    "asr z16.s, z16.s, #0x1f\n"
    "sdot z19.s, z25.b, z24.b\n"
    ".inst 0x04b776d6  // sqrdmulh z22.s, z22.s, z23.s\n"
    "and z18.d, z20.d, z21.d\n"
    "asr z18.s, z18.s, #0x1f\n"
    "and z17.d, z22.d, z21.d\n"
    ".inst 0x04b77673  // sqrdmulh z19.s, z19.s, z23.s\n"
    "asr z17.s, z17.s, #0x1f\n"
    "sqadd z31.s, z31.s, z16.s\n"
    "and z16.d, z19.d, z21.d\n"
    "asr z16.s, z16.s, #0x1f\n"
    "sqadd z20.s, z20.s, z18.s\n"
    ".inst 0x44828abf  // srshl z31.s, p2/M, z31.s, z21.s\n"
    "sqadd z22.s, z22.s, z17.s\n"
    "add z31.s, z31.s, z4.s\n"
    ".inst 0x44828ab4  // srshl z20.s, p2/M, z20.s, z21.s\n"
    "sqadd z19.s, z19.s, z16.s\n"
    ".inst 0x44828ab6  // srshl z22.s, p2/M, z22.s, z21.s\n"
    "smax z31.s, p2/M, z31.s, z6.s\n"
    "add z20.s, z20.s, z4.s\n"
    ".inst 0x44828ab3  // srshl z19.s, p2/M, z19.s, z21.s\n"
    "add z22.s, z22.s, z4.s\n"
    "smin z31.s, p2/M, z31.s, z5.s\n"
    "st1b { z31.s }, p0, [x22, x25]\n"
    "add z19.s, z19.s, z4.s\n"
    "smax z22.s, p2/M, z22.s, z6.s\n"
    "smax z20.s, p2/M, z20.s, z6.s\n"
    "smax z19.s, p2/M, z19.s, z6.s\n"
    "smin z22.s, p2/M, z22.s, z5.s\n"
    "st1b { z22.s }, p0, [x21, x25]\n"
    "smin z20.s, p2/M, z20.s, z5.s\n"
    "smin z19.s, p2/M, z19.s, z5.s\n"
    "st1b { z20.s }, p0, [x20, x25]\n"
    "st1b { z19.s }, p0, [x19, x25]\n"
    "incw x25\n"
    "whilelt p1.b, x25, %x[n_channels]\n"
    "b.any 1b\n"
    "addvl SP, SP, #8\n"
    : [params] "+&r" (params)
    : [inptrs] "r" (inptrs), [n_channels] "r" ((long unsigned int) n_channels), [offsetof_Requantize32_c_offset] "I" (offsetof(arm_gemm::Requantize32, c_offset)), [offsetof_Requantize32_maxval] "I" (offsetof(arm_gemm::Requantize32, maxval)), [offsetof_Requantize32_minval] "I" (offsetof(arm_gemm::Requantize32, minval)), [outptrs] "r" (outptrs), [qp] "r" (&qp)
    : "cc", "memory", "p0", "p1", "p2", "x9", "x10", "x11", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
  );
}

}  // namespace depthwise
}  // namespace arm_conv

#endif  // defined(ARM_COMPUTE_ENABLE_SVE)
