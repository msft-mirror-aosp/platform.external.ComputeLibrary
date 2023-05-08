/*
 * Copyright (c) 2022 Arm Limited.
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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifdef __ARM_FEATURE_SVE
#ifdef ARM_COMPUTE_ENABLE_SME2

#include "arm_gemm.hpp"
#include "../../utils.hpp"

#include <cassert>
#include <limits>

namespace arm_gemm {

void sme2_gemv_fp32_mla_16VL (
    const float *A_ptr, const float *B_ptr, float *output_ptr,
    size_t N, size_t K,
    const float *bias, Activation act, bool
)
{
    struct KernelArgs {
        float maxval = static_cast<float>(std::numeric_limits<float>::infinity());
        float minval = - static_cast<float>(std::numeric_limits<float>::infinity());
        const float *B_ptr = {};
        size_t output_offset = {};
        unsigned int input_initial_col = {};
    } ka;

    unsigned long flags=0;
    ka.B_ptr = B_ptr;
    switch(act.type) {
        default:
        case Activation::Type::None:
            break;
        case Activation::Type::BoundedReLU:
            ka.maxval = static_cast<float>(act.param1);
            /* fall through */
        case Activation::Type::ReLU:
            ka.minval = 0;
            flags |= 0x2;
            break;
    }
    __asm__ __volatile__(
      "ptrue p1.b\n"
      ".inst 0xd503477f  // SMSTART ZA\n"
      "cntw x27, ALL, MUL #4\n"
      "add x26, %x[N], x27\n"
      "sub x26, x26, #0x1\n"
      "udiv x26, x26, x27\n"
      "add x21, x26, #0x3\n"
      "and x21, x21, #0xfffffffffffffffc\n"
      "mul x21, x21, x27\n"
      "mul x21, x21, %x[K]\n"
      "mov x9, #0x0\n"
      "mov x25, %x[B_ptr]\n"
      "mov x24, %x[output_ptr]\n"
      "ptrue p1.b\n"
      ".inst 0x25207811  // ptrue pn9.b\n"
      "lsl x21, x21, #0x2\n"
      "mov x20, #0x1\n"
      "1:"  // RHS size check loop
      "cmp x21, #0x200000\n"
      "blt 2f\n"
      "tbnz x21, #0, 3f\n"
      "lsr x21, x21, #0x1\n"
      "lsl x20, x20, #0x1\n"
      "b 1b\n"
      "2:"  // RHS do prefetch
      "lsl x19, x21, #0x26\n"
      "sub x20, x20, #0x1\n"
      "lsl x20, x20, #0x16\n"
      "orr x21, x21, x19\n"
      "orr x21, x21, x20\n"
      ".inst 0xf8b54b3a  // rprfm pldonce, x21, [x25]\n"
      "3:"  // RHS prefetch exit
      "mov x23, %x[bias]\n"
      "4:"  // Column loop
      "cmp x26, #0x4\n"
      "bge 28f\n"
      "cmp x26, #0x2\n"
      "bgt 20f\n"
      "beq 12f\n"
      "mov x22, %x[A_ptr]\n"
      "lsl x21, %x[K], #0x2\n"
      "mov x19, %x[N]\n"
      "mov x20, %x[K]\n"
      ".inst 0xf8b54ad8  // rprfm pldmany, x21, [x22]\n"
      ".inst 0x25b367f0  // whilelt p8.s, XZR, x19, VLx4\n"
      "cbz x23, 5f\n"
      ".inst 0xa040c6e0  // ld1w { z0.s-z3.s }, pn9.b/Z, [x23]\n"
      ".inst 0xc0042c00  // mova za.d[x9, #0], { z0.d-z3.d }\n"
      "b 6f\n"
      "5:"  // Width 1: no bias
      ".inst 0xc00800ff  // zero { zad0, zad1, zad2, zad3, zad4, zad5, zad6, zad7 }\n"
      "6:"  // Width 1: setup done
      "cmp x20, #0x4\n"
      "ble 8f\n"
      "7:"  // Width 1: Multiply loop: Main loop head
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "sub x20, x20, #0x4\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      "cmp x20, #0x4\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      "add x22, x22, #0x10\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "bgt 7b\n"
      "8:"  // Width 1: Multiply loop: Single iteration only
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      "add x22, x22, #0x10\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      "ble 9f\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      "ble 9f\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      "ble 9f\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "9:"  // Width 1: Multiply loop: multiply skip
      "tbz %x[flags], #1, 10f\n"
      "add x20, %x[args_ptr], %[offset_min]\n"
      "add x19, %x[args_ptr], %[offset_max]\n"
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      "ld1rw { z0.s }, p1/Z, [x20]\n"
      "ld1rw { z6.s }, p1/Z, [x19]\n"
      ".inst 0xc1a6c808  // fclamp { z8.s-z11.s }, z0.s, z6.s\n"
      ".inst 0xa060c308  // st1w { z8.s-z11.s }, p8, [x24]\n"
      "addvl x24, x24, #4\n"
      "b 11f\n"
      "10:"  // Width 1: No activation
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      ".inst 0xa060c308  // st1w { z8.s-z11.s }, p8, [x24]\n"
      "addvl x24, x24, #4\n"
      "11:"  // Width 1: Output done
      "b 36f\n"
      "12:"  // Width 2
      "mov x22, %x[A_ptr]\n"
      "lsl x21, %x[K], #0x2\n"
      "sub x19, %x[N], x27\n"
      "mov x20, %x[K]\n"
      ".inst 0xf8b54ad8  // rprfm pldmany, x21, [x22]\n"
      ".inst 0x25b367f0  // whilelt p8.s, XZR, x19, VLx4\n"
      "cbz x23, 13f\n"
      ".inst 0xa040c6e0  // ld1w { z0.s-z3.s }, pn9.b/Z, [x23]\n"
      ".inst 0xc0042c00  // mova za.d[x9, #0], { z0.d-z3.d }\n"
      ".inst 0xa041c6e8  // ld1w { z8.s-z11.s }, pn9.b/Z, [x23, #0x4, MUL VL]\n"
      ".inst 0xc0042d01  // mova za.d[x9, #1], { z8.d-z11.d }\n"
      "b 14f\n"
      "13:"  // Width 2: no bias
      ".inst 0xc00800ff  // zero { zad0, zad1, zad2, zad3, zad4, zad5, zad6, zad7 }\n"
      "14:"  // Width 2: setup done
      "cmp x20, #0x4\n"
      "ble 16f\n"
      "15:"  // Width 2: Multiply loop: Main loop head
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "sub x20, x20, #0x4\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      "cmp x20, #0x4\n"
      "add x22, x22, #0x10\n"
      ".inst 0xa041c725  // ldnt1w { z4.s-z7.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa081  // fmla za.s[x9, 1], { z4.s-z7.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa601  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaa01  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa041c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaf01  // fmla za.s[x9, 1], { z24.s-z27.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "bgt 15b\n"
      "16:"  // Width 2: Multiply loop: Single iteration only
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      "add x22, x22, #0x10\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      ".inst 0xa041c725  // ldnt1w { z4.s-z7.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa081  // fmla za.s[x9, 1], { z4.s-z7.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      "ble 17f\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa601  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      "ble 17f\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaa01  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      "ble 17f\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa041c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaf01  // fmla za.s[x9, 1], { z24.s-z27.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "17:"  // Width 2: Multiply loop: multiply skip
      "tbz %x[flags], #1, 18f\n"
      "add x20, %x[args_ptr], %[offset_min]\n"
      "add x19, %x[args_ptr], %[offset_max]\n"
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      "ld1rw { z0.s }, p1/Z, [x20]\n"
      ".inst 0xc0062c34  // mova { z20.d-z23.d }, za.d[x9, #1]\n"
      "ld1rw { z6.s }, p1/Z, [x19]\n"
      ".inst 0xc1a6c808  // fclamp { z8.s-z11.s }, z0.s, z6.s\n"
      ".inst 0xa060c708  // st1w { z8.s-z11.s }, pn9.b, [x24]\n"
      ".inst 0xc1a6c814  // fclamp { z20.s-z23.s }, z0.s, z6.s\n"
      ".inst 0xa061c314  // st1w { z20.s-z23.s }, p8, [x24, #0x4, MUL VL]\n"
      "addvl x24, x24, #8\n"
      "b 19f\n"
      "18:"  // Width 2: No activation
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      ".inst 0xa060c708  // st1w { z8.s-z11.s }, pn9.b, [x24]\n"
      ".inst 0xc0062c34  // mova { z20.d-z23.d }, za.d[x9, #1]\n"
      ".inst 0xa061c314  // st1w { z20.s-z23.s }, p8, [x24, #0x4, MUL VL]\n"
      "addvl x24, x24, #8\n"
      "19:"  // Width 2: Output done
      "b 36f\n"
      "20:"  // Width 3
      "mov x19, #0x2\n"
      "mov x22, %x[A_ptr]\n"
      "lsl x21, %x[K], #0x2\n"
      "msub x19, x27, x19, %x[N]\n"
      "mov x20, %x[K]\n"
      ".inst 0xf8b54ad8  // rprfm pldmany, x21, [x22]\n"
      ".inst 0x25b367f0  // whilelt p8.s, XZR, x19, VLx4\n"
      "cbz x23, 21f\n"
      ".inst 0xa040c6e0  // ld1w { z0.s-z3.s }, pn9.b/Z, [x23]\n"
      ".inst 0xc0042c00  // mova za.d[x9, #0], { z0.d-z3.d }\n"
      ".inst 0xa041c6e8  // ld1w { z8.s-z11.s }, pn9.b/Z, [x23, #0x4, MUL VL]\n"
      ".inst 0xc0042d01  // mova za.d[x9, #1], { z8.d-z11.d }\n"
      ".inst 0xa042c6e4  // ld1w { z4.s-z7.s }, pn9.b/Z, [x23, #0x8, MUL VL]\n"
      ".inst 0xc0042c82  // mova za.d[x9, #2], { z4.d-z7.d }\n"
      "b 22f\n"
      "21:"  // Width 3: no bias
      ".inst 0xc00800ff  // zero { zad0, zad1, zad2, zad3, zad4, zad5, zad6, zad7 }\n"
      "22:"  // Width 3: setup done
      "cmp x20, #0x4\n"
      "ble 24f\n"
      "23:"  // Width 3: Multiply loop: Main loop head
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "sub x20, x20, #0x4\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      "cmp x20, #0x4\n"
      "add x22, x22, #0x10\n"
      ".inst 0xa041c725  // ldnt1w { z4.s-z7.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa081  // fmla za.s[x9, 1], { z4.s-z7.s }, z10.s[0]\n"
      ".inst 0xa042c735  // ldnt1w { z20.s-z23.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa282  // fmla za.s[x9, 2], { z20.s-z23.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa601  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[1]\n"
      ".inst 0xa042c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa702  // fmla za.s[x9, 2], { z24.s-z27.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaa01  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[2]\n"
      ".inst 0xa042c73d  // ldnt1w { z28.s-z31.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aab82  // fmla za.s[x9, 2], { z28.s-z31.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa041c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaf01  // fmla za.s[x9, 1], { z24.s-z27.s }, z10.s[3]\n"
      ".inst 0xa042c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aae02  // fmla za.s[x9, 2], { z16.s-z19.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "bgt 23b\n"
      "24:"  // Width 3: Multiply loop: Single iteration only
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      "add x22, x22, #0x10\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      ".inst 0xa041c725  // ldnt1w { z4.s-z7.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa081  // fmla za.s[x9, 1], { z4.s-z7.s }, z10.s[0]\n"
      ".inst 0xa042c735  // ldnt1w { z20.s-z23.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa282  // fmla za.s[x9, 2], { z20.s-z23.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      "ble 25f\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa601  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[1]\n"
      ".inst 0xa042c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa702  // fmla za.s[x9, 2], { z24.s-z27.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      "ble 25f\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaa01  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[2]\n"
      ".inst 0xa042c73d  // ldnt1w { z28.s-z31.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aab82  // fmla za.s[x9, 2], { z28.s-z31.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      "ble 25f\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa041c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaf01  // fmla za.s[x9, 1], { z24.s-z27.s }, z10.s[3]\n"
      ".inst 0xa042c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aae02  // fmla za.s[x9, 2], { z16.s-z19.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "25:"  // Width 3: Multiply loop: multiply skip
      "tbz %x[flags], #1, 26f\n"
      "add x20, %x[args_ptr], %[offset_min]\n"
      "add x19, %x[args_ptr], %[offset_max]\n"
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      "ld1rw { z0.s }, p1/Z, [x20]\n"
      ".inst 0xc0062c34  // mova { z20.d-z23.d }, za.d[x9, #1]\n"
      "ld1rw { z6.s }, p1/Z, [x19]\n"
      ".inst 0xc1a6c808  // fclamp { z8.s-z11.s }, z0.s, z6.s\n"
      ".inst 0xc0062c50  // mova { z16.d-z19.d }, za.d[x9, #2]\n"
      ".inst 0xa060c708  // st1w { z8.s-z11.s }, pn9.b, [x24]\n"
      ".inst 0xc1a6c814  // fclamp { z20.s-z23.s }, z0.s, z6.s\n"
      ".inst 0xa061c714  // st1w { z20.s-z23.s }, pn9.b, [x24, #0x4, MUL VL]\n"
      ".inst 0xc1a6c810  // fclamp { z16.s-z19.s }, z0.s, z6.s\n"
      ".inst 0xa062c310  // st1w { z16.s-z19.s }, p8, [x24, #0x8, MUL VL]\n"
      "addvl x24, x24, #12\n"
      "b 27f\n"
      "26:"  // Width 3: No activation
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      ".inst 0xa060c708  // st1w { z8.s-z11.s }, pn9.b, [x24]\n"
      ".inst 0xc0062c34  // mova { z20.d-z23.d }, za.d[x9, #1]\n"
      ".inst 0xa061c714  // st1w { z20.s-z23.s }, pn9.b, [x24, #0x4, MUL VL]\n"
      ".inst 0xc0062c50  // mova { z16.d-z19.d }, za.d[x9, #2]\n"
      ".inst 0xa062c310  // st1w { z16.s-z19.s }, p8, [x24, #0x8, MUL VL]\n"
      "addvl x24, x24, #12\n"
      "27:"  // Width 3: Output done
      "b 36f\n"
      "28:"  // Width 4
      "mov x19, #0x3\n"
      "mov x22, %x[A_ptr]\n"
      "lsl x21, %x[K], #0x2\n"
      "msub x19, x27, x19, %x[N]\n"
      "mov x20, %x[K]\n"
      ".inst 0xf8b54ad8  // rprfm pldmany, x21, [x22]\n"
      ".inst 0x25b367f0  // whilelt p8.s, XZR, x19, VLx4\n"
      "cbz x23, 29f\n"
      ".inst 0xa040c6e0  // ld1w { z0.s-z3.s }, pn9.b/Z, [x23]\n"
      ".inst 0xc0042c00  // mova za.d[x9, #0], { z0.d-z3.d }\n"
      ".inst 0xa041c6e8  // ld1w { z8.s-z11.s }, pn9.b/Z, [x23, #0x4, MUL VL]\n"
      ".inst 0xc0042d01  // mova za.d[x9, #1], { z8.d-z11.d }\n"
      ".inst 0xa042c6e4  // ld1w { z4.s-z7.s }, pn9.b/Z, [x23, #0x8, MUL VL]\n"
      ".inst 0xc0042c82  // mova za.d[x9, #2], { z4.d-z7.d }\n"
      ".inst 0xa043c6f0  // ld1w { z16.s-z19.s }, pn9.b/Z, [x23, #0xc, MUL VL]\n"
      ".inst 0xc0042e03  // mova za.d[x9, #3], { z16.d-z19.d }\n"
      "addvl x23, x23, #16\n"
      "b 30f\n"
      "29:"  // Width 4: no bias
      ".inst 0xc00800ff  // zero { zad0, zad1, zad2, zad3, zad4, zad5, zad6, zad7 }\n"
      "30:"  // Width 4: setup done
      "cmp x20, #0x4\n"
      "ble 32f\n"
      "31:"  // Width 4: Multiply loop: Main loop head
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "sub x20, x20, #0x4\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      "cmp x20, #0x4\n"
      "add x22, x22, #0x10\n"
      ".inst 0xa041c725  // ldnt1w { z4.s-z7.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa081  // fmla za.s[x9, 1], { z4.s-z7.s }, z10.s[0]\n"
      ".inst 0xa042c735  // ldnt1w { z20.s-z23.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa282  // fmla za.s[x9, 2], { z20.s-z23.s }, z10.s[0]\n"
      ".inst 0xa043c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aa203  // fmla za.s[x9, 3], { z16.s-z19.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa601  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[1]\n"
      ".inst 0xa042c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa702  // fmla za.s[x9, 2], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa043c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aa603  // fmla za.s[x9, 3], { z16.s-z19.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaa01  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[2]\n"
      ".inst 0xa042c73d  // ldnt1w { z28.s-z31.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aab82  // fmla za.s[x9, 2], { z28.s-z31.s }, z10.s[2]\n"
      ".inst 0xa043c735  // ldnt1w { z20.s-z23.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aaa83  // fmla za.s[x9, 3], { z20.s-z23.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa041c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaf01  // fmla za.s[x9, 1], { z24.s-z27.s }, z10.s[3]\n"
      ".inst 0xa042c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aae02  // fmla za.s[x9, 2], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa043c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aae03  // fmla za.s[x9, 3], { z16.s-z19.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "bgt 31b\n"
      "32:"  // Width 4: Multiply loop: Single iteration only
      "whilelt p0.s, XZR, x20\n"
      "ld1rqw { z10.s }, p0/Z, [x22]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xa040c721  // ldnt1w { z0.s-z3.s }, pn9.b/Z, [x25]\n"
      "add x22, x22, #0x10\n"
      ".inst 0xc15aa000  // fmla za.s[x9, 0], { z0.s-z3.s }, z10.s[0]\n"
      ".inst 0xa041c725  // ldnt1w { z4.s-z7.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa081  // fmla za.s[x9, 1], { z4.s-z7.s }, z10.s[0]\n"
      ".inst 0xa042c735  // ldnt1w { z20.s-z23.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa282  // fmla za.s[x9, 2], { z20.s-z23.s }, z10.s[0]\n"
      ".inst 0xa043c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aa203  // fmla za.s[x9, 3], { z16.s-z19.s }, z10.s[0]\n"
      "addvl x25, x25, #16\n"
      "ble 33f\n"
      ".inst 0xa040c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa700  // fmla za.s[x9, 0], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aa601  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[1]\n"
      ".inst 0xa042c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aa702  // fmla za.s[x9, 2], { z24.s-z27.s }, z10.s[1]\n"
      ".inst 0xa043c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aa603  // fmla za.s[x9, 3], { z16.s-z19.s }, z10.s[1]\n"
      "addvl x25, x25, #16\n"
      "ble 33f\n"
      ".inst 0xa040c72d  // ldnt1w { z12.s-z15.s }, pn9.b/Z, [x25]\n"
      "subs x20, x20, #0x1\n"
      ".inst 0xc15aa980  // fmla za.s[x9, 0], { z12.s-z15.s }, z10.s[2]\n"
      ".inst 0xa041c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaa01  // fmla za.s[x9, 1], { z16.s-z19.s }, z10.s[2]\n"
      ".inst 0xa042c73d  // ldnt1w { z28.s-z31.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aab82  // fmla za.s[x9, 2], { z28.s-z31.s }, z10.s[2]\n"
      ".inst 0xa043c735  // ldnt1w { z20.s-z23.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aaa83  // fmla za.s[x9, 3], { z20.s-z23.s }, z10.s[2]\n"
      "addvl x25, x25, #16\n"
      "ble 33f\n"
      ".inst 0xa040c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25]\n"
      ".inst 0xc15aae00  // fmla za.s[x9, 0], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa041c739  // ldnt1w { z24.s-z27.s }, pn9.b/Z, [x25, #0x4, MUL VL]\n"
      ".inst 0xc15aaf01  // fmla za.s[x9, 1], { z24.s-z27.s }, z10.s[3]\n"
      ".inst 0xa042c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0x8, MUL VL]\n"
      ".inst 0xc15aae02  // fmla za.s[x9, 2], { z16.s-z19.s }, z10.s[3]\n"
      ".inst 0xa043c731  // ldnt1w { z16.s-z19.s }, pn9.b/Z, [x25, #0xc, MUL VL]\n"
      ".inst 0xc15aae03  // fmla za.s[x9, 3], { z16.s-z19.s }, z10.s[3]\n"
      "addvl x25, x25, #16\n"
      "33:"  // Width 4: Multiply loop: multiply skip
      "tbz %x[flags], #1, 34f\n"
      "add x20, %x[args_ptr], %[offset_min]\n"
      "add x19, %x[args_ptr], %[offset_max]\n"
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      "ld1rw { z0.s }, p1/Z, [x20]\n"
      ".inst 0xc0062c34  // mova { z20.d-z23.d }, za.d[x9, #1]\n"
      "ld1rw { z6.s }, p1/Z, [x19]\n"
      ".inst 0xc1a6c808  // fclamp { z8.s-z11.s }, z0.s, z6.s\n"
      ".inst 0xc0062c50  // mova { z16.d-z19.d }, za.d[x9, #2]\n"
      ".inst 0xa060c708  // st1w { z8.s-z11.s }, pn9.b, [x24]\n"
      ".inst 0xc1a6c814  // fclamp { z20.s-z23.s }, z0.s, z6.s\n"
      ".inst 0xc0062c78  // mova { z24.d-z27.d }, za.d[x9, #3]\n"
      ".inst 0xa061c714  // st1w { z20.s-z23.s }, pn9.b, [x24, #0x4, MUL VL]\n"
      ".inst 0xc1a6c810  // fclamp { z16.s-z19.s }, z0.s, z6.s\n"
      ".inst 0xa062c710  // st1w { z16.s-z19.s }, pn9.b, [x24, #0x8, MUL VL]\n"
      ".inst 0xc1a6c818  // fclamp { z24.s-z27.s }, z0.s, z6.s\n"
      ".inst 0xa063c318  // st1w { z24.s-z27.s }, p8, [x24, #0xc, MUL VL]\n"
      "addvl x24, x24, #16\n"
      "b 35f\n"
      "34:"  // Width 4: No activation
      ".inst 0xc0062c08  // mova { z8.d-z11.d }, za.d[x9, #0]\n"
      ".inst 0xa060c708  // st1w { z8.s-z11.s }, pn9.b, [x24]\n"
      ".inst 0xc0062c34  // mova { z20.d-z23.d }, za.d[x9, #1]\n"
      ".inst 0xa061c714  // st1w { z20.s-z23.s }, pn9.b, [x24, #0x4, MUL VL]\n"
      ".inst 0xc0062c50  // mova { z16.d-z19.d }, za.d[x9, #2]\n"
      ".inst 0xa062c710  // st1w { z16.s-z19.s }, pn9.b, [x24, #0x8, MUL VL]\n"
      ".inst 0xc0062c78  // mova { z24.d-z27.d }, za.d[x9, #3]\n"
      ".inst 0xa063c318  // st1w { z24.s-z27.s }, p8, [x24, #0xc, MUL VL]\n"
      "addvl x24, x24, #16\n"
      "35:"  // Width 4: Output done
      "subs x26, x26, #0x4\n"
      "sub %x[N], %x[N], x27, LSL #2\n"
      "bgt 4b\n"
      "36:"  // Exit
      ".inst 0xd503467f  // SMSTOP\n"
      "ptrue p1.b\n"
      : [N] "+&r" (N)
      : [A_ptr] "r" (A_ptr), [B_ptr] "r" (B_ptr), [K] "r" (K), [args_ptr] "r" (&ka), [bias] "r" (bias), [flags] "r" (flags), [offset_max] "I" (offsetof(KernelArgs, maxval)), [offset_min] "I" (offsetof(KernelArgs, minval)), [output_ptr] "r" (output_ptr)
      : "cc", "memory", "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13", "p14", "p15", "x9", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
    );
}

} // namespace arm_gemm

#endif // ARM_COMPUTE_ENABLE_SME2
#endif
