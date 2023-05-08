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

#pragma once

#ifdef __ARM_FEATURE_SVE


namespace {

void sve_transpose_interleave_3VL(uint16_t *out, const uint16_t *in, size_t width, size_t in_stride, size_t height)
{
    size_t out_stride = 3 * height * get_vector_length<uint8_t>();

    __asm__ __volatile__(
      "ptrue p2.b\n"
      "cmp %x[height], #0x4\n"
      "blt 4f\n"
      "1:"  // Main row loop: Head
      "mov x26, %x[in]\n"
      "mov x25, %x[out]\n"
      "add x24, x26, %x[in_stride]\n"
      "add x23, x24, %x[in_stride]\n"
      "add x22, x23, %x[in_stride]\n"
      "add %x[in], x22, %x[in_stride]\n"
      "sub %x[height], %x[height], #0x4\n"
      "mov x21, %x[width]\n"
      "2:"  // Main row loop: Column loop
      "mov x20, x21\n"
      "mov x19, x25\n"
      "whilelt p0.h, XZR, x20\n"
      "ld1h { z27.h }, p0/Z, [x26]\n"
      "ld1h { z26.h }, p0/Z, [x24]\n"
      "dech x20\n"
      "ld1h { z25.h }, p0/Z, [x23]\n"
      "whilelt p1.h, XZR, x20\n"
      "ld1h { z24.h }, p0/Z, [x22]\n"
      "dech x20\n"
      "ld1h { z23.h }, p1/Z, [x26, #1, MUL VL]\n"
      "whilelt p0.h, XZR, x20\n"
      "ld1h { z22.h }, p1/Z, [x24, #1, MUL VL]\n"
      "add x25, x25, %x[out_stride]\n"
      "ld1h { z21.h }, p0/Z, [x26, #2, MUL VL]\n"
      "addvl x26, x26, #3\n"
      "ld1h { z20.h }, p0/Z, [x24, #2, MUL VL]\n"
      "addvl x24, x24, #3\n"
      "ld1h { z19.h }, p1/Z, [x23, #1, MUL VL]\n"
      "dech x21, ALL, MUL #3\n"
      "ld1h { z18.h }, p0/Z, [x23, #2, MUL VL]\n"
      "addvl x23, x23, #3\n"
      "ld1h { z17.h }, p1/Z, [x22, #1, MUL VL]\n"
      "cmp x21, #0x0\n"
      "ld1h { z16.h }, p0/Z, [x22, #2, MUL VL]\n"
      "addvl x22, x22, #3\n"
      "st1h { z27.h }, p2, [x19]\n"
      "st1h { z23.h }, p2, [x19, #1, MUL VL]\n"
      "st1h { z21.h }, p2, [x19, #2, MUL VL]\n"
      "st1h { z26.h }, p2, [x19, #3, MUL VL]\n"
      "st1h { z22.h }, p2, [x19, #4, MUL VL]\n"
      "st1h { z20.h }, p2, [x19, #5, MUL VL]\n"
      "st1h { z25.h }, p2, [x19, #6, MUL VL]\n"
      "st1h { z19.h }, p2, [x19, #7, MUL VL]\n"
      "addvl x19, x19, #12\n"
      "st1h { z18.h }, p2, [x19, #-4, MUL VL]\n"
      "st1h { z24.h }, p2, [x19, #-3, MUL VL]\n"
      "st1h { z17.h }, p2, [x19, #-2, MUL VL]\n"
      "st1h { z16.h }, p2, [x19, #-1, MUL VL]\n"
      "bgt 2b\n"
      "3:"  // Main row loop: Column loop skip
      "addvl %x[out], %x[out], #12\n"
      "cmp %x[height], #0x4\n"
      "bge 1b\n"
      "cbz %x[height], 8f\n"
      "4:"  // Main loop skip

      "5:"  // Tail row loop: Head
      "mov x26, %x[in]\n"
      "mov x25, %x[out]\n"
      "add %x[in], x26, %x[in_stride]\n"
      "sub %x[height], %x[height], #0x1\n"
      "mov x20, %x[width]\n"
      "6:"  // Tail row loop: Column loop
      "mov x19, x20\n"
      "dech x20, ALL, MUL #3\n"
      "whilelt p0.h, XZR, x19\n"
      "ld1h { z18.h }, p0/Z, [x26]\n"
      "dech x19\n"
      "whilelt p0.h, XZR, x19\n"
      "ld1h { z17.h }, p0/Z, [x26, #1, MUL VL]\n"
      "dech x19\n"
      "whilelt p0.h, XZR, x19\n"
      "ld1h { z16.h }, p0/Z, [x26, #2, MUL VL]\n"
      "st1h { z18.h }, p2, [x25]\n"
      "addvl x26, x26, #3\n"
      "st1h { z17.h }, p2, [x25, #1, MUL VL]\n"
      "cmp x20, #0x0\n"
      "st1h { z16.h }, p2, [x25, #2, MUL VL]\n"
      "add x25, x25, %x[out_stride]\n"
      "bgt 6b\n"
      "7:"  // Tail row loop: Column loop skip
      "addvl %x[out], %x[out], #3\n"
      "cmp %x[height], #0x1\n"
      "bge 5b\n"
      "8:"  // Done

      : [height] "+&r" (height), [in] "+&r" (in), [out] "+&r" (out)
      : [in_stride] "r" (in_stride), [out_stride] "r" (out_stride), [width] "r" (width)
      : "cc", "memory", "p0", "p1", "p2", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27"
    );
}

} // anonymous namespace

template<>
void Transform<3, 1, true, VLType::SVE>(
    float *out, const float *in, int stride, int x0, int xmax, int k0, int kmax)
{
    sve_transpose_interleave_3VL(
        reinterpret_cast<uint16_t *>(out),
        reinterpret_cast<const uint16_t *>(in + k0 * stride + x0),
        (xmax-x0) * sizeof(float) / 2,
        stride * sizeof(float),
        (kmax-k0)
    );
}

template<>
void Transform<3, 1, true, VLType::SVE>(
    __fp16 *out, const __fp16 *in, int stride, int x0, int xmax, int k0, int kmax)
{
    sve_transpose_interleave_3VL(
        reinterpret_cast<uint16_t *>(out),
        reinterpret_cast<const uint16_t *>(in + k0 * stride + x0),
        (xmax-x0) * sizeof(__fp16) / 2,
        stride * sizeof(__fp16),
        (kmax-k0)
    );
}

template<>
void Transform<3, 1, true, VLType::SVE>(
    double *out, const double *in, int stride, int x0, int xmax, int k0, int kmax)
{
    sve_transpose_interleave_3VL(
        reinterpret_cast<uint16_t *>(out),
        reinterpret_cast<const uint16_t *>(in + k0 * stride + x0),
        (xmax-x0) * sizeof(double) / 2,
        stride * sizeof(double),
        (kmax-k0)
    );
}

#endif
