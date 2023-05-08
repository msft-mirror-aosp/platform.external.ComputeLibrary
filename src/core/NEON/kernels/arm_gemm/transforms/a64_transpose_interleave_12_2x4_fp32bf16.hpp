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

#ifdef __aarch64__

namespace {

void a64_transpose_interleave_12_2x4_fp32bf16(bfloat16 *out, const float *in, size_t width, size_t in_stride, size_t height)
{
    float *pad_row = reinterpret_cast<float *>(alloca(width * sizeof(float)));

    if (height % 4) {
        memset(pad_row, 0, width * sizeof(float));
    }

    size_t out_stride = 12 * roundup<size_t>(height, 4) * sizeof(bfloat16);

    __asm__ __volatile__(
      "cmp %x[height], #0x8\n"
      "blt 10f\n"
      "1:"  // Main row loop: Head
      "mov x28, %x[in]\n"
      "mov x27, %x[out]\n"
      "add x26, x28, %x[in_stride]\n"
      "add x25, x26, %x[in_stride]\n"
      "add x24, x25, %x[in_stride]\n"
      "add x23, x24, %x[in_stride]\n"
      "add x22, x23, %x[in_stride]\n"
      "add x21, x22, %x[in_stride]\n"
      "add x20, x21, %x[in_stride]\n"
      "add %x[in], x20, %x[in_stride]\n"
      "sub %x[height], %x[height], #0x8\n"
      "mov x19, %x[width]\n"
      "cmp x19, #0x18\n"
      "blt 3f\n"
      "2:"  // Main row loop: Unroll column loop
      "ldr q12, [x28], #0x10\n"
      "sub x19, x19, #0x18\n"
      "ldr q20, [x26], #0x10\n"
      "cmp x19, #0x18\n"
      "ldr q11, [x25], #0x10\n"
      "zip1 v29.4s, v12.4s, v11.4s\n"
      "ldr q5, [x28], #0x10\n"
      "zip2 v0.4s, v12.4s, v11.4s\n"
      "ldr q28, [x26], #0x10\n"
      "ldr q17, [x25], #0x10\n"
      "zip1 v23.4s, v5.4s, v17.4s\n"
      "ldr q25, [x28], #0x10\n"
      "zip2 v18.4s, v5.4s, v17.4s\n"
      "ldr q6, [x26], #0x10\n"
      "ldr q31, [x25], #0x10\n"
      "zip1 v21.4s, v25.4s, v31.4s\n"
      "ldr q16, [x28], #0x10\n"
      "zip2 v10.4s, v25.4s, v31.4s\n"
      "ldr q11, [x26], #0x10\n"
      "ldr q1, [x25], #0x10\n"
      "zip1 v13.4s, v16.4s, v1.4s\n"
      "ldr q14, [x28], #0x10\n"
      "zip2 v24.4s, v16.4s, v1.4s\n"
      "ldr q4, [x26], #0x10\n"
      "ldr q22, [x25], #0x10\n"
      "zip1 v1.4s, v14.4s, v22.4s\n"
      "ldr q15, [x28], #0x10\n"
      "zip2 v8.4s, v14.4s, v22.4s\n"
      "ldr q31, [x26], #0x10\n"
      "ldr q3, [x25], #0x10\n"
      "zip1 v27.4s, v15.4s, v3.4s\n"
      "ldr q30, [x24], #0x10\n"
      "zip2 v22.4s, v15.4s, v3.4s\n"
      "ldr q15, [x23], #0x10\n"
      "ldr q5, [x22], #0x10\n"
      "zip1 v16.4s, v20.4s, v30.4s\n"
      "ldr q3, [x24], #0x10\n"
      "zip2 v7.4s, v20.4s, v30.4s\n"
      "ldr q26, [x23], #0x10\n"
      "zip1 v12.4s, v29.4s, v16.4s\n"
      "ldr q25, [x22], #0x10\n"
      ".inst 0x0ea16994  // bfcvtn v20.4h, v12.4s\n"
      "ldr q2, [x21], #0x10\n"
      "zip2 v16.4s, v29.4s, v16.4s\n"
      "ldr q19, [x24], #0x10\n"
      "zip1 v12.4s, v0.4s, v7.4s\n"
      "ldr q9, [x23], #0x10\n"
      ".inst 0x4ea16a14  // bfcvtn2 v20.8h, v16.4s\n"
      "ldr q14, [x22], #0x10\n"
      ".inst 0x0ea1699e  // bfcvtn v30.4h, v12.4s\n"
      "ldr q12, [x21], #0x10\n"
      "zip2 v16.4s, v0.4s, v7.4s\n"
      "ldr q7, [x24], #0x10\n"
      "zip1 v29.4s, v28.4s, v3.4s\n"
      "ldr q0, [x23], #0x10\n"
      ".inst 0x4ea16a1e  // bfcvtn2 v30.8h, v16.4s\n"
      "ldr q17, [x22], #0x10\n"
      "zip1 v16.4s, v23.4s, v29.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip2 v23.4s, v23.4s, v29.4s\n"
      "ldr q29, [x24], #0x10\n"
      "zip2 v28.4s, v28.4s, v3.4s\n"
      "ldr q3, [x23], #0x10\n"
      ".inst 0x4ea16af0  // bfcvtn2 v16.8h, v23.4s\n"
      "zip1 v23.4s, v18.4s, v28.4s\n"
      ".inst 0x0ea16af7  // bfcvtn v23.4h, v23.4s\n"
      "zip2 v28.4s, v18.4s, v28.4s\n"
      "ldr q18, [x24], #0x10\n"
      ".inst 0x4ea16b97  // bfcvtn2 v23.8h, v28.4s\n"
      "zip1 v28.4s, v6.4s, v19.4s\n"
      "zip2 v6.4s, v6.4s, v19.4s\n"
      "zip1 v19.4s, v21.4s, v28.4s\n"
      ".inst 0x0ea16a73  // bfcvtn v19.4h, v19.4s\n"
      "zip2 v28.4s, v21.4s, v28.4s\n"
      "ldr q21, [x23], #0x10\n"
      ".inst 0x4ea16b93  // bfcvtn2 v19.8h, v28.4s\n"
      "zip1 v28.4s, v10.4s, v6.4s\n"
      ".inst 0x0ea16b9c  // bfcvtn v28.4h, v28.4s\n"
      "zip2 v6.4s, v10.4s, v6.4s\n"
      "ldr q10, [x22], #0x10\n"
      ".inst 0x4ea168dc  // bfcvtn2 v28.8h, v6.4s\n"
      "zip1 v6.4s, v11.4s, v7.4s\n"
      "zip2 v7.4s, v11.4s, v7.4s\n"
      "zip1 v11.4s, v13.4s, v6.4s\n"
      ".inst 0x0ea1696b  // bfcvtn v11.4h, v11.4s\n"
      "zip2 v13.4s, v13.4s, v6.4s\n"
      "ldr q6, [x22], #0x10\n"
      ".inst 0x4ea169ab  // bfcvtn2 v11.8h, v13.4s\n"
      "zip1 v13.4s, v24.4s, v7.4s\n"
      ".inst 0x0ea169ad  // bfcvtn v13.4h, v13.4s\n"
      "zip2 v7.4s, v24.4s, v7.4s\n"
      "ldr q24, [x21], #0x10\n"
      ".inst 0x4ea168ed  // bfcvtn2 v13.8h, v7.4s\n"
      "zip1 v7.4s, v4.4s, v29.4s\n"
      "zip2 v29.4s, v4.4s, v29.4s\n"
      "zip1 v4.4s, v1.4s, v7.4s\n"
      ".inst 0x0ea16884  // bfcvtn v4.4h, v4.4s\n"
      "zip2 v7.4s, v1.4s, v7.4s\n"
      "ldr q1, [x21], #0x10\n"
      ".inst 0x4ea168e4  // bfcvtn2 v4.8h, v7.4s\n"
      "zip1 v7.4s, v8.4s, v29.4s\n"
      ".inst 0x0ea168e7  // bfcvtn v7.4h, v7.4s\n"
      "zip2 v8.4s, v8.4s, v29.4s\n"
      "ldr q29, [x21], #0x10\n"
      ".inst 0x4ea16907  // bfcvtn2 v7.8h, v8.4s\n"
      "zip1 v8.4s, v31.4s, v18.4s\n"
      "zip2 v31.4s, v31.4s, v18.4s\n"
      "zip1 v18.4s, v27.4s, v8.4s\n"
      ".inst 0x0ea16a52  // bfcvtn v18.4h, v18.4s\n"
      "zip2 v27.4s, v27.4s, v8.4s\n"
      "ldr q8, [x21], #0x10\n"
      ".inst 0x4ea16b72  // bfcvtn2 v18.8h, v27.4s\n"
      "zip1 v27.4s, v22.4s, v31.4s\n"
      ".inst 0x0ea16b7b  // bfcvtn v27.4h, v27.4s\n"
      "zip2 v31.4s, v22.4s, v31.4s\n"
      "ldr q22, [x20], #0x10\n"
      ".inst 0x4ea16bfb  // bfcvtn2 v27.8h, v31.4s\n"
      "zip1 v31.4s, v15.4s, v2.4s\n"
      "zip2 v2.4s, v15.4s, v2.4s\n"
      "zip1 v15.4s, v26.4s, v12.4s\n"
      "zip2 v26.4s, v26.4s, v12.4s\n"
      "zip1 v12.4s, v5.4s, v22.4s\n"
      "zip2 v22.4s, v5.4s, v22.4s\n"
      "zip1 v5.4s, v31.4s, v12.4s\n"
      ".inst 0x0ea168a5  // bfcvtn v5.4h, v5.4s\n"
      "zip2 v31.4s, v31.4s, v12.4s\n"
      "ldr q12, [x20], #0x10\n"
      ".inst 0x4ea16be5  // bfcvtn2 v5.8h, v31.4s\n"
      "zip1 v31.4s, v2.4s, v22.4s\n"
      ".inst 0x0ea16bff  // bfcvtn v31.4h, v31.4s\n"
      "zip2 v2.4s, v2.4s, v22.4s\n"
      "ldr q22, [x20], #0x10\n"
      ".inst 0x4ea1685f  // bfcvtn2 v31.8h, v2.4s\n"
      "zip1 v2.4s, v25.4s, v12.4s\n"
      "zip2 v25.4s, v25.4s, v12.4s\n"
      "zip1 v12.4s, v15.4s, v2.4s\n"
      ".inst 0x0ea1698c  // bfcvtn v12.4h, v12.4s\n"
      "zip2 v15.4s, v15.4s, v2.4s\n"
      "ldr q2, [x20], #0x10\n"
      ".inst 0x4ea169ec  // bfcvtn2 v12.8h, v15.4s\n"
      "zip1 v15.4s, v26.4s, v25.4s\n"
      ".inst 0x0ea169ef  // bfcvtn v15.4h, v15.4s\n"
      "zip2 v25.4s, v26.4s, v25.4s\n"
      "ldr q26, [x20], #0x10\n"
      ".inst 0x4ea16b2f  // bfcvtn2 v15.8h, v25.4s\n"
      "ldr q25, [x20], #0x10\n"
      "str q20, [x27, #0x0]\n"
      "zip1 v20.4s, v9.4s, v24.4s\n"
      "zip2 v24.4s, v9.4s, v24.4s\n"
      "str q30, [x27, #0x10]\n"
      "zip1 v9.4s, v14.4s, v22.4s\n"
      "str q16, [x27, #0x20]\n"
      "zip1 v16.4s, v20.4s, v9.4s\n"
      "str q23, [x27, #0x30]\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "str q19, [x27, #0x40]\n"
      "zip2 v9.4s, v20.4s, v9.4s\n"
      "str q28, [x27, #0x50]\n"
      "zip2 v22.4s, v14.4s, v22.4s\n"
      "str q5, [x27, #0x60]\n"
      ".inst 0x4ea16930  // bfcvtn2 v16.8h, v9.4s\n"
      "str q31, [x27, #0x70]\n"
      "zip1 v19.4s, v24.4s, v22.4s\n"
      "str q12, [x27, #0x80]\n"
      ".inst 0x0ea16a6c  // bfcvtn v12.4h, v19.4s\n"
      "str q15, [x27, #0x90]\n"
      "zip2 v9.4s, v24.4s, v22.4s\n"
      "str q16, [x27, #0xa0]\n"
      "zip1 v15.4s, v0.4s, v1.4s\n"
      ".inst 0x4ea1692c  // bfcvtn2 v12.8h, v9.4s\n"
      "str q12, [x27, #0xb0]\n"
      "zip1 v20.4s, v17.4s, v2.4s\n"
      "add x27, x27, %x[out_stride]\n"
      "zip1 v16.4s, v15.4s, v20.4s\n"
      "str q11, [x27, #0x0]\n"
      "zip2 v9.4s, v15.4s, v20.4s\n"
      "str q13, [x27, #0x10]\n"
      ".inst 0x0ea16a0f  // bfcvtn v15.4h, v16.4s\n"
      "str q4, [x27, #0x20]\n"
      "zip2 v14.4s, v0.4s, v1.4s\n"
      "str q7, [x27, #0x30]\n"
      "zip2 v31.4s, v17.4s, v2.4s\n"
      "str q18, [x27, #0x40]\n"
      ".inst 0x4ea1692f  // bfcvtn2 v15.8h, v9.4s\n"
      "str q27, [x27, #0x50]\n"
      "zip1 v22.4s, v14.4s, v31.4s\n"
      "str q15, [x27, #0x60]\n"
      ".inst 0x0ea16ac9  // bfcvtn v9.4h, v22.4s\n"
      "zip2 v11.4s, v14.4s, v31.4s\n"
      "zip1 v18.4s, v3.4s, v29.4s\n"
      "zip1 v27.4s, v10.4s, v26.4s\n"
      ".inst 0x4ea16969  // bfcvtn2 v9.8h, v11.4s\n"
      "str q9, [x27, #0x70]\n"
      "zip1 v13.4s, v18.4s, v27.4s\n"
      "zip2 v9.4s, v18.4s, v27.4s\n"
      ".inst 0x0ea169b3  // bfcvtn v19.4h, v13.4s\n"
      "zip2 v18.4s, v3.4s, v29.4s\n"
      "zip2 v1.4s, v10.4s, v26.4s\n"
      ".inst 0x4ea16933  // bfcvtn2 v19.8h, v9.4s\n"
      "str q19, [x27, #0x80]\n"
      "zip1 v16.4s, v18.4s, v1.4s\n"
      "zip2 v20.4s, v18.4s, v1.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip1 v18.4s, v21.4s, v8.4s\n"
      "zip1 v2.4s, v6.4s, v25.4s\n"
      ".inst 0x4ea16a90  // bfcvtn2 v16.8h, v20.4s\n"
      "str q16, [x27, #0x90]\n"
      "zip1 v16.4s, v18.4s, v2.4s\n"
      "zip2 v20.4s, v18.4s, v2.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip2 v18.4s, v21.4s, v8.4s\n"
      "zip2 v17.4s, v6.4s, v25.4s\n"
      ".inst 0x4ea16a90  // bfcvtn2 v16.8h, v20.4s\n"
      "str q16, [x27, #0xa0]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v17.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      ".inst 0x4ea16a30  // bfcvtn2 v16.8h, v17.4s\n"
      "str q16, [x27, #0xb0]\n"
      "add x27, x27, %x[out_stride]\n"
      "bge 2b\n"
      "3:"  // Main row loop: Unroll column loop skip
      "cmp x19, #0xc\n"
      "blt 5f\n"
      "4:"  // Main row loop: Column loop
      "ldr q18, [x28], #0x10\n"
      "sub x19, x19, #0xc\n"
      "ldr q21, [x26], #0x10\n"
      "cmp x19, #0xc\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v19.4s, v18.4s, v16.4s\n"
      "ldr q17, [x28], #0x10\n"
      "zip2 v20.4s, v18.4s, v16.4s\n"
      "ldr q8, [x26], #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v7.4s, v17.4s, v16.4s\n"
      "ldr q18, [x28], #0x10\n"
      "zip2 v6.4s, v17.4s, v16.4s\n"
      "ldr q5, [x26], #0x10\n"
      "ldr q17, [x25], #0x10\n"
      "zip1 v4.4s, v18.4s, v17.4s\n"
      "ldr q16, [x24], #0x10\n"
      "zip2 v3.4s, v18.4s, v17.4s\n"
      "ldr q2, [x23], #0x10\n"
      "ldr q1, [x22], #0x10\n"
      "zip1 v17.4s, v21.4s, v16.4s\n"
      "ldr q0, [x24], #0x10\n"
      "zip2 v18.4s, v21.4s, v16.4s\n"
      "ldr q31, [x23], #0x10\n"
      "zip1 v16.4s, v19.4s, v17.4s\n"
      "ldr q30, [x22], #0x10\n"
      ".inst 0x0ea16a1d  // bfcvtn v29.4h, v16.4s\n"
      "ldr q28, [x21], #0x10\n"
      "zip2 v17.4s, v19.4s, v17.4s\n"
      "ldr q27, [x24], #0x10\n"
      "zip1 v16.4s, v20.4s, v18.4s\n"
      "ldr q26, [x23], #0x10\n"
      ".inst 0x4ea16a3d  // bfcvtn2 v29.8h, v17.4s\n"
      "ldr q25, [x22], #0x10\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "ldr q24, [x21], #0x10\n"
      "zip2 v16.4s, v20.4s, v18.4s\n"
      "ldr q23, [x20], #0x10\n"
      "zip1 v17.4s, v8.4s, v0.4s\n"
      "ldr q22, [x21], #0x10\n"
      ".inst 0x4ea16a13  // bfcvtn2 v19.8h, v16.4s\n"
      "zip1 v16.4s, v7.4s, v17.4s\n"
      "ldr q21, [x20], #0x10\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      "ldr q20, [x20], #0x10\n"
      "zip2 v16.4s, v7.4s, v17.4s\n"
      "zip2 v17.4s, v8.4s, v0.4s\n"
      "str q29, [x27, #0x0]\n"
      ".inst 0x4ea16a12  // bfcvtn2 v18.8h, v16.4s\n"
      "str q19, [x27, #0x10]\n"
      "zip1 v16.4s, v6.4s, v17.4s\n"
      "str q18, [x27, #0x20]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v6.4s, v17.4s\n"
      "zip1 v17.4s, v5.4s, v27.4s\n"
      "zip1 v16.4s, v4.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x30]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v4.4s, v17.4s\n"
      "zip2 v17.4s, v5.4s, v27.4s\n"
      "zip1 v16.4s, v3.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x40]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v16.4s, v3.4s, v17.4s\n"
      "zip1 v18.4s, v2.4s, v28.4s\n"
      "zip1 v17.4s, v1.4s, v23.4s\n"
      ".inst 0x4ea16a13  // bfcvtn2 v19.8h, v16.4s\n"
      "str q19, [x27, #0x50]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v19.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip2 v18.4s, v2.4s, v28.4s\n"
      "zip2 v17.4s, v1.4s, v23.4s\n"
      ".inst 0x4ea16a70  // bfcvtn2 v16.8h, v19.4s\n"
      "str q16, [x27, #0x60]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v19.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip1 v18.4s, v31.4s, v24.4s\n"
      "zip1 v17.4s, v30.4s, v21.4s\n"
      ".inst 0x4ea16a70  // bfcvtn2 v16.8h, v19.4s\n"
      "str q16, [x27, #0x70]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v19.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip2 v18.4s, v31.4s, v24.4s\n"
      "zip2 v17.4s, v30.4s, v21.4s\n"
      ".inst 0x4ea16a70  // bfcvtn2 v16.8h, v19.4s\n"
      "str q16, [x27, #0x80]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v19.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip1 v18.4s, v26.4s, v22.4s\n"
      "zip1 v17.4s, v25.4s, v20.4s\n"
      ".inst 0x4ea16a70  // bfcvtn2 v16.8h, v19.4s\n"
      "str q16, [x27, #0x90]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v19.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip2 v18.4s, v26.4s, v22.4s\n"
      "zip2 v17.4s, v25.4s, v20.4s\n"
      ".inst 0x4ea16a70  // bfcvtn2 v16.8h, v19.4s\n"
      "str q16, [x27, #0xa0]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v17.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      ".inst 0x4ea16a30  // bfcvtn2 v16.8h, v17.4s\n"
      "str q16, [x27, #0xb0]\n"
      "add x27, x27, %x[out_stride]\n"
      "bge 4b\n"
      "5:"  // Main row loop: Column loop skip
      "cmp x19, #0x4\n"
      "blt 7f\n"
      "6:"  // Main row loop: width 4 loop: loop
      "ldr q20, [x28], #0x10\n"
      "sub x19, x19, #0x4\n"
      "ldr q18, [x26], #0x10\n"
      "cmp x19, #0x4\n"
      "ldr q17, [x25], #0x10\n"
      "zip1 v19.4s, v20.4s, v17.4s\n"
      "ldr q16, [x24], #0x10\n"
      "zip2 v25.4s, v20.4s, v17.4s\n"
      "ldr q24, [x23], #0x10\n"
      "ldr q23, [x22], #0x10\n"
      "zip1 v17.4s, v18.4s, v16.4s\n"
      "ldr q22, [x21], #0x10\n"
      "zip2 v21.4s, v18.4s, v16.4s\n"
      "ldr q20, [x20], #0x10\n"
      "zip1 v16.4s, v19.4s, v17.4s\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      "zip2 v17.4s, v19.4s, v17.4s\n"
      "zip1 v16.4s, v25.4s, v21.4s\n"
      ".inst 0x4ea16a32  // bfcvtn2 v18.8h, v17.4s\n"
      "str q18, [x27, #0x0]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v16.4s, v25.4s, v21.4s\n"
      "zip1 v18.4s, v24.4s, v22.4s\n"
      "zip1 v17.4s, v23.4s, v20.4s\n"
      ".inst 0x4ea16a13  // bfcvtn2 v19.8h, v16.4s\n"
      "str q19, [x27, #0x10]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v19.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip2 v18.4s, v24.4s, v22.4s\n"
      "zip2 v17.4s, v23.4s, v20.4s\n"
      ".inst 0x4ea16a70  // bfcvtn2 v16.8h, v19.4s\n"
      "str q16, [x27, #0x60]\n"
      "zip1 v16.4s, v18.4s, v17.4s\n"
      "zip2 v17.4s, v18.4s, v17.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      ".inst 0x4ea16a30  // bfcvtn2 v16.8h, v17.4s\n"
      "str q16, [x27, #0x70]\n"
      "add x27, x27, #0x20\n"
      "bge 6b\n"
      "7:"  // Main row loop: width 4 loop: skip
      "cmp x19, #0x1\n"
      "blt 9f\n"
      "8:"  // Main row loop: width 1 loop: loop
      "ldr s18, [x28], #0x4\n"
      "sub x19, x19, #0x1\n"
      "ldr s17, [x26], #0x4\n"
      "cmp x19, #0x1\n"
      "ldr s16, [x25], #0x4\n"
      "zip1 v18.4s, v18.4s, v16.4s\n"
      "ldr s16, [x24], #0x4\n"
      "ldr s20, [x23], #0x4\n"
      "zip1 v16.4s, v17.4s, v16.4s\n"
      "ldr s19, [x22], #0x4\n"
      "ldr s17, [x21], #0x4\n"
      "zip1 v16.4s, v18.4s, v16.4s\n"
      "ldr s18, [x20], #0x4\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "zip1 v17.4s, v20.4s, v17.4s\n"
      "str d16, [x27, #0x0]\n"
      "zip1 v16.4s, v19.4s, v18.4s\n"
      "zip1 v16.4s, v17.4s, v16.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "str d16, [x27, #0x60]\n"
      "add x27, x27, #0x8\n"
      "bge 8b\n"
      "9:"  // Main row loop: width 1 loop: skip
      "add %x[out], %x[out], #0xc0\n"
      "cmp %x[height], #0x8\n"
      "bge 1b\n"
      "cbz %x[height], 20f\n"
      "10:"  // Main loop skip

      "11:"  // Tail row loop: Head
      "mov x28, %x[in]\n"
      "mov x27, %x[out]\n"
      "add x26, x28, %x[in_stride]\n"
      "add x25, x26, %x[in_stride]\n"
      "add x24, x25, %x[in_stride]\n"
      "add %x[in], x24, %x[in_stride]\n"
      "cmp %x[height], #0x3\n"
      "csel x24, x24, %x[pad_row], GT\n"
      "csel x25, x25, %x[pad_row], GE\n"
      "cmp %x[height], #0x1\n"
      "csel x26, x26, %x[pad_row], GT\n"
      "sub %x[height], %x[height], #0x4\n"
      "mov x19, %x[width]\n"
      "cmp x19, #0x18\n"
      "blt 13f\n"
      "12:"  // Tail row loop: Unroll column loop
      "ldr q17, [x28], #0x10\n"
      "sub x19, x19, #0x18\n"
      "ldr q20, [x26], #0x10\n"
      "cmp x19, #0x18\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v19.4s, v17.4s, v16.4s\n"
      "ldr q18, [x28], #0x10\n"
      "zip2 v9.4s, v17.4s, v16.4s\n"
      "ldr q8, [x26], #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v7.4s, v18.4s, v16.4s\n"
      "ldr q17, [x28], #0x10\n"
      "zip2 v6.4s, v18.4s, v16.4s\n"
      "ldr q5, [x26], #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v4.4s, v17.4s, v16.4s\n"
      "ldr q18, [x28], #0x10\n"
      "zip2 v3.4s, v17.4s, v16.4s\n"
      "ldr q2, [x26], #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v1.4s, v18.4s, v16.4s\n"
      "ldr q17, [x28], #0x10\n"
      "zip2 v0.4s, v18.4s, v16.4s\n"
      "ldr q31, [x26], #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v30.4s, v17.4s, v16.4s\n"
      "ldr q18, [x28], #0x10\n"
      "zip2 v29.4s, v17.4s, v16.4s\n"
      "ldr q28, [x26], #0x10\n"
      "ldr q17, [x25], #0x10\n"
      "zip1 v27.4s, v18.4s, v17.4s\n"
      "ldr q16, [x24], #0x10\n"
      "zip2 v26.4s, v18.4s, v17.4s\n"
      "ldr q25, [x24], #0x10\n"
      "zip1 v17.4s, v20.4s, v16.4s\n"
      "zip2 v24.4s, v20.4s, v16.4s\n"
      "ldr q23, [x24], #0x10\n"
      "zip1 v16.4s, v19.4s, v17.4s\n"
      "zip2 v17.4s, v19.4s, v17.4s\n"
      "ldr q22, [x24], #0x10\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip1 v16.4s, v9.4s, v24.4s\n"
      "ldr q21, [x24], #0x10\n"
      ".inst 0x4ea16a33  // bfcvtn2 v19.8h, v17.4s\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      "ldr q20, [x24], #0x10\n"
      "zip2 v16.4s, v9.4s, v24.4s\n"
      "zip1 v17.4s, v8.4s, v25.4s\n"
      "str q19, [x27, #0x0]\n"
      ".inst 0x4ea16a12  // bfcvtn2 v18.8h, v16.4s\n"
      "str q18, [x27, #0x10]\n"
      "zip1 v16.4s, v7.4s, v17.4s\n"
      "zip2 v19.4s, v7.4s, v17.4s\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      "zip2 v17.4s, v8.4s, v25.4s\n"
      "zip1 v16.4s, v6.4s, v17.4s\n"
      ".inst 0x4ea16a72  // bfcvtn2 v18.8h, v19.4s\n"
      "str q18, [x27, #0x20]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v6.4s, v17.4s\n"
      "zip1 v17.4s, v5.4s, v23.4s\n"
      "zip1 v16.4s, v4.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x30]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v4.4s, v17.4s\n"
      "zip2 v17.4s, v5.4s, v23.4s\n"
      "zip1 v16.4s, v3.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x40]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v3.4s, v17.4s\n"
      "zip1 v17.4s, v2.4s, v22.4s\n"
      "zip1 v16.4s, v1.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x50]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "add x27, x27, %x[out_stride]\n"
      "zip2 v18.4s, v1.4s, v17.4s\n"
      "zip2 v17.4s, v2.4s, v22.4s\n"
      "zip1 v16.4s, v0.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x0]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v0.4s, v17.4s\n"
      "zip1 v17.4s, v31.4s, v21.4s\n"
      "zip1 v16.4s, v30.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x10]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v30.4s, v17.4s\n"
      "zip2 v17.4s, v31.4s, v21.4s\n"
      "zip1 v16.4s, v29.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x20]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v29.4s, v17.4s\n"
      "zip1 v17.4s, v28.4s, v20.4s\n"
      "zip1 v16.4s, v27.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x30]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v17.4s, v27.4s, v17.4s\n"
      "zip2 v18.4s, v28.4s, v20.4s\n"
      "zip1 v16.4s, v26.4s, v18.4s\n"
      ".inst 0x4ea16a33  // bfcvtn2 v19.8h, v17.4s\n"
      "str q19, [x27, #0x40]\n"
      ".inst 0x0ea16a11  // bfcvtn v17.4h, v16.4s\n"
      "zip2 v16.4s, v26.4s, v18.4s\n"
      ".inst 0x4ea16a11  // bfcvtn2 v17.8h, v16.4s\n"
      "str q17, [x27, #0x50]\n"
      "add x27, x27, %x[out_stride]\n"
      "bge 12b\n"
      "13:"  // Tail row loop: Unroll column loop skip
      "cmp x19, #0xc\n"
      "blt 15f\n"
      "14:"  // Tail row loop: Column loop
      "ldr q18, [x28], #0x10\n"
      "sub x19, x19, #0xc\n"
      "ldr q20, [x26], #0x10\n"
      "cmp x19, #0xc\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v19.4s, v18.4s, v16.4s\n"
      "ldr q17, [x28], #0x10\n"
      "zip2 v29.4s, v18.4s, v16.4s\n"
      "ldr q28, [x26], #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "zip1 v27.4s, v17.4s, v16.4s\n"
      "ldr q18, [x28], #0x10\n"
      "zip2 v26.4s, v17.4s, v16.4s\n"
      "ldr q25, [x26], #0x10\n"
      "ldr q17, [x25], #0x10\n"
      "zip1 v24.4s, v18.4s, v17.4s\n"
      "ldr q16, [x24], #0x10\n"
      "zip2 v23.4s, v18.4s, v17.4s\n"
      "ldr q22, [x24], #0x10\n"
      "zip1 v17.4s, v20.4s, v16.4s\n"
      "zip2 v21.4s, v20.4s, v16.4s\n"
      "ldr q20, [x24], #0x10\n"
      "zip1 v16.4s, v19.4s, v17.4s\n"
      "zip2 v19.4s, v19.4s, v17.4s\n"
      ".inst 0x0ea16a11  // bfcvtn v17.4h, v16.4s\n"
      "zip1 v16.4s, v29.4s, v21.4s\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      ".inst 0x4ea16a71  // bfcvtn2 v17.8h, v19.4s\n"
      "str q17, [x27, #0x0]\n"
      "zip2 v16.4s, v29.4s, v21.4s\n"
      "zip1 v17.4s, v28.4s, v22.4s\n"
      ".inst 0x4ea16a12  // bfcvtn2 v18.8h, v16.4s\n"
      "str q18, [x27, #0x10]\n"
      "zip1 v16.4s, v27.4s, v17.4s\n"
      "zip2 v19.4s, v27.4s, v17.4s\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      "zip2 v17.4s, v28.4s, v22.4s\n"
      "zip1 v16.4s, v26.4s, v17.4s\n"
      ".inst 0x4ea16a72  // bfcvtn2 v18.8h, v19.4s\n"
      "str q18, [x27, #0x20]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v18.4s, v26.4s, v17.4s\n"
      "zip1 v17.4s, v25.4s, v20.4s\n"
      "zip1 v16.4s, v24.4s, v17.4s\n"
      ".inst 0x4ea16a53  // bfcvtn2 v19.8h, v18.4s\n"
      "str q19, [x27, #0x30]\n"
      ".inst 0x0ea16a13  // bfcvtn v19.4h, v16.4s\n"
      "zip2 v17.4s, v24.4s, v17.4s\n"
      "zip2 v18.4s, v25.4s, v20.4s\n"
      "zip1 v16.4s, v23.4s, v18.4s\n"
      ".inst 0x4ea16a33  // bfcvtn2 v19.8h, v17.4s\n"
      "str q19, [x27, #0x40]\n"
      ".inst 0x0ea16a11  // bfcvtn v17.4h, v16.4s\n"
      "zip2 v16.4s, v23.4s, v18.4s\n"
      ".inst 0x4ea16a11  // bfcvtn2 v17.8h, v16.4s\n"
      "str q17, [x27, #0x50]\n"
      "add x27, x27, %x[out_stride]\n"
      "bge 14b\n"
      "15:"  // Tail row loop: Column loop skip
      "cmp x19, #0x4\n"
      "blt 17f\n"
      "16:"  // Tail row loop: width 4 loop: loop
      "ldr q19, [x28], #0x10\n"
      "sub x19, x19, #0x4\n"
      "ldr q18, [x26], #0x10\n"
      "cmp x19, #0x4\n"
      "ldr q17, [x25], #0x10\n"
      "zip1 v21.4s, v19.4s, v17.4s\n"
      "ldr q16, [x24], #0x10\n"
      "zip2 v20.4s, v19.4s, v17.4s\n"
      "zip1 v17.4s, v18.4s, v16.4s\n"
      "zip2 v19.4s, v18.4s, v16.4s\n"
      "zip1 v16.4s, v21.4s, v17.4s\n"
      ".inst 0x0ea16a12  // bfcvtn v18.4h, v16.4s\n"
      "zip2 v17.4s, v21.4s, v17.4s\n"
      "zip1 v16.4s, v20.4s, v19.4s\n"
      ".inst 0x4ea16a32  // bfcvtn2 v18.8h, v17.4s\n"
      "str q18, [x27, #0x0]\n"
      ".inst 0x0ea16a11  // bfcvtn v17.4h, v16.4s\n"
      "zip2 v16.4s, v20.4s, v19.4s\n"
      ".inst 0x4ea16a11  // bfcvtn2 v17.8h, v16.4s\n"
      "str q17, [x27, #0x10]\n"
      "add x27, x27, #0x20\n"
      "bge 16b\n"
      "17:"  // Tail row loop: width 4 loop: skip
      "cmp x19, #0x1\n"
      "blt 19f\n"
      "18:"  // Tail row loop: width 1 loop: loop
      "ldr s17, [x28], #0x4\n"
      "sub x19, x19, #0x1\n"
      "ldr s18, [x26], #0x4\n"
      "cmp x19, #0x1\n"
      "ldr s16, [x25], #0x4\n"
      "zip1 v17.4s, v17.4s, v16.4s\n"
      "ldr s16, [x24], #0x4\n"
      "zip1 v16.4s, v18.4s, v16.4s\n"
      "zip1 v16.4s, v17.4s, v16.4s\n"
      ".inst 0x0ea16a10  // bfcvtn v16.4h, v16.4s\n"
      "str d16, [x27, #0x0]\n"
      "add x27, x27, #0x8\n"
      "bge 18b\n"
      "19:"  // Tail row loop: width 1 loop: skip
      "add %x[out], %x[out], #0x60\n"
      "cmp %x[height], #0x1\n"
      "bge 11b\n"
      "20:"  // Done

      : [height] "+&r" (height), [in] "+&r" (in), [out] "+&r" (out)
      : [in_stride] "r" (in_stride), [out_stride] "r" (out_stride), [pad_row] "r" (pad_row), [width] "r" (width)
      : "cc", "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

} // anonymous namespace
template<>
void Transform<12, 4, true, VLType::None>(
    bfloat16 *out, const float *in, int stride, int x0, int xmax, int k0, int kmax)
{
    a64_transpose_interleave_12_2x4_fp32bf16(
        out,
        in + k0 * stride + x0,
        (xmax-x0),
        stride * sizeof(float),
        (kmax-k0)
    );
}

#endif
