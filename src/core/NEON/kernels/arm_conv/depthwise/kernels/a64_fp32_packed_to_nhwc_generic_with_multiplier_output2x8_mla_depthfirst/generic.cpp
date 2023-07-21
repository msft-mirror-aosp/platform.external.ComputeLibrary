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

#if defined(__aarch64__)
#include <cstddef>
#include <cstdint>

namespace arm_conv {
namespace depthwise {

void a64_fp32_packed_to_nhwc_generic_with_multiplier_output2x8_mla_depthfirst_impl(
  const float *const *const inptrs,
  float *const *const outptrs,
  const float *weights,
  const float *bias,
  const unsigned int kernel_points,
  const unsigned int n_output_channels,
  const float activation_min,
  const float activation_max
)
{
  const float minmax_vals[2] = { activation_min, activation_max };

  __asm__ __volatile__(
    "ld1r { v11.4s }, [%x[minmax_vals]]\n"
    "mov x10, #0x0\n"
    "add x19, %x[minmax_vals], #0x4\n"
    "ld1r { v10.4s }, [x19]\n"
    "lsr x9, %x[n_output_channels], #0x2\n"
    "cbz x9, 8f\n"
    "1:"  // Output channel loop
    "movi v16.16b, #0x0\n"
    "cbz %x[bias], 2f\n"
    "lsl x19, x10, #0x2\n"
    "ldr q16, [%x[bias], x19]\n"
    "2:"  // Output channel loop: Load bias: Done
    "mov v9.16b, v16.16b\n"
    "ldr q8, [%x[weights], #0x0]\n"
    "mov x19, %x[inptrs]\n"
    "mov v7.16b, v16.16b\n"
    "ldp x24, x28, [x19], #0x10\n"
    "lsr x20, %x[kernel_points], #0x1\n"
    "mov v6.16b, v16.16b\n"
    "ldr q5, [x24, #0x0]\n"
    "mov v4.16b, v16.16b\n"
    "add %x[weights], %x[weights], #0x10\n"
    "mov v3.16b, v16.16b\n"
    "ldr q2, [x24, #0x10]\n"
    "mov v1.16b, v16.16b\n"
    "ldr q0, [x28, #0x0]\n"
    "mov v31.16b, v16.16b\n"
    "ldr q30, [x28, #0x10]\n"
    "mov v29.16b, v16.16b\n"
    "mov v28.16b, v16.16b\n"
    "mov v27.16b, v16.16b\n"
    "mov v26.16b, v16.16b\n"
    "mov v25.16b, v16.16b\n"
    "mov v24.16b, v16.16b\n"
    "mov v23.16b, v16.16b\n"
    "mov v22.16b, v16.16b\n"
    "mov v21.16b, v16.16b\n"
    "cbz x20, 6f\n"
    "ldp x24, x28, [x19], #0x10\n"
    "ldr q20, [%x[weights], #0x0]\n"
    "subs x20, x20, #0x1\n"
    "add %x[weights], %x[weights], #0x10\n"
    "ldr q19, [x24, #0x0]\n"
    "ldr q18, [x24, #0x10]\n"
    "ldr q17, [x28, #0x0]\n"
    "ldr q16, [x28, #0x10]\n"
    "beq 4f\n"
    "3:"  // Output channel loop: Kernel loop
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "ldp x24, x28, [x19], #0x10\n"
    "subs x20, x20, #0x1\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "ldr q5, [x24, #0x0]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "ldr q2, [x24, #0x10]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "ldr q0, [x28, #0x0]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "ldr q30, [x28, #0x10]\n"
    "fmla v9.4s, v20.4s, v19.s[0]\n"
    "ldr q8, [%x[weights], #0x0]\n"
    "fmla v7.4s, v20.4s, v19.s[1]\n"
    "ldp x24, x28, [x19], #0x10\n"
    "fmla v6.4s, v20.4s, v19.s[2]\n"
    "fmla v4.4s, v20.4s, v19.s[3]\n"
    "ldr q19, [x24, #0x0]\n"
    "fmla v3.4s, v20.4s, v18.s[0]\n"
    "fmla v1.4s, v20.4s, v18.s[1]\n"
    "fmla v31.4s, v20.4s, v18.s[2]\n"
    "fmla v29.4s, v20.4s, v18.s[3]\n"
    "ldr q18, [x24, #0x10]\n"
    "fmla v28.4s, v20.4s, v17.s[0]\n"
    "fmla v27.4s, v20.4s, v17.s[1]\n"
    "fmla v26.4s, v20.4s, v17.s[2]\n"
    "fmla v25.4s, v20.4s, v17.s[3]\n"
    "ldr q17, [x28, #0x0]\n"
    "fmla v24.4s, v20.4s, v16.s[0]\n"
    "fmla v23.4s, v20.4s, v16.s[1]\n"
    "fmla v22.4s, v20.4s, v16.s[2]\n"
    "fmla v21.4s, v20.4s, v16.s[3]\n"
    "ldr q16, [x28, #0x10]\n"
    "ldr q20, [%x[weights], #0x10]\n"
    "add %x[weights], %x[weights], #0x20\n"
    "bgt 3b\n"
    "4:"  // Output channel loop: Kernel loop tail
    "tbnz %x[kernel_points], #0, 5f\n"
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "ldr x19, [%x[outptrs], #0x0]\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "ldr x20, [%x[outptrs], #0x8]\n"
    "lsl x27, x10, #0x2\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "ldr x21, [%x[outptrs], #0x10]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "ldr x22, [%x[outptrs], #0x18]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "ldr x23, [%x[outptrs], #0x20]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "ldr x24, [%x[outptrs], #0x28]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "ldr x25, [%x[outptrs], #0x30]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "ldr x26, [%x[outptrs], #0x38]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "fmla v9.4s, v20.4s, v19.s[0]\n"
    "fmla v7.4s, v20.4s, v19.s[1]\n"
    "fmla v6.4s, v20.4s, v19.s[2]\n"
    "fmla v4.4s, v20.4s, v19.s[3]\n"
    "fmla v3.4s, v20.4s, v18.s[0]\n"
    "fmla v1.4s, v20.4s, v18.s[1]\n"
    "fmla v31.4s, v20.4s, v18.s[2]\n"
    "fmla v29.4s, v20.4s, v18.s[3]\n"
    "fmla v28.4s, v20.4s, v17.s[0]\n"
    "fmla v27.4s, v20.4s, v17.s[1]\n"
    "fmla v26.4s, v20.4s, v17.s[2]\n"
    "fmla v25.4s, v20.4s, v17.s[3]\n"
    "fmla v24.4s, v20.4s, v16.s[0]\n"
    "fmla v23.4s, v20.4s, v16.s[1]\n"
    "fmla v22.4s, v20.4s, v16.s[2]\n"
    "fmla v21.4s, v20.4s, v16.s[3]\n"
    "fmin v9.4s, v9.4s, v10.4s\n"
    "fmin v7.4s, v7.4s, v10.4s\n"
    "fmin v6.4s, v6.4s, v10.4s\n"
    "fmax v9.4s, v9.4s, v11.4s\n"
    "str q9, [x19, x27]\n"
    "fmax v7.4s, v7.4s, v11.4s\n"
    "fmax v6.4s, v6.4s, v11.4s\n"
    "ldr x19, [%x[outptrs], #0x40]\n"
    "fmin v4.4s, v4.4s, v10.4s\n"
    "str q7, [x20, x27]\n"
    "fmin v3.4s, v3.4s, v10.4s\n"
    "fmin v1.4s, v1.4s, v10.4s\n"
    "str q6, [x21, x27]\n"
    "fmax v4.4s, v4.4s, v11.4s\n"
    "ldr x20, [%x[outptrs], #0x48]\n"
    "fmin v31.4s, v31.4s, v10.4s\n"
    "ldr x21, [%x[outptrs], #0x50]\n"
    "fmax v3.4s, v3.4s, v11.4s\n"
    "str q4, [x22, x27]\n"
    "fmax v1.4s, v1.4s, v11.4s\n"
    "ldr x22, [%x[outptrs], #0x58]\n"
    "fmax v31.4s, v31.4s, v11.4s\n"
    "str q3, [x23, x27]\n"
    "fmin v29.4s, v29.4s, v10.4s\n"
    "str q1, [x24, x27]\n"
    "fmin v28.4s, v28.4s, v10.4s\n"
    "str q31, [x25, x27]\n"
    "fmin v27.4s, v27.4s, v10.4s\n"
    "ldr x23, [%x[outptrs], #0x60]\n"
    "fmax v29.4s, v29.4s, v11.4s\n"
    "ldr x24, [%x[outptrs], #0x68]\n"
    "fmax v28.4s, v28.4s, v11.4s\n"
    "ldr x25, [%x[outptrs], #0x70]\n"
    "fmax v27.4s, v27.4s, v11.4s\n"
    "str q29, [x26, x27]\n"
    "fmin v26.4s, v26.4s, v10.4s\n"
    "str q28, [x19, x27]\n"
    "fmin v25.4s, v25.4s, v10.4s\n"
    "str q27, [x20, x27]\n"
    "fmin v24.4s, v24.4s, v10.4s\n"
    "ldr x26, [%x[outptrs], #0x78]\n"
    "fmax v26.4s, v26.4s, v11.4s\n"
    "str q26, [x21, x27]\n"
    "fmax v25.4s, v25.4s, v11.4s\n"
    "fmax v24.4s, v24.4s, v11.4s\n"
    "str q25, [x22, x27]\n"
    "fmin v23.4s, v23.4s, v10.4s\n"
    "fmin v22.4s, v22.4s, v10.4s\n"
    "str q24, [x23, x27]\n"
    "fmin v21.4s, v21.4s, v10.4s\n"
    "fmax v23.4s, v23.4s, v11.4s\n"
    "str q23, [x24, x27]\n"
    "fmax v22.4s, v22.4s, v11.4s\n"
    "fmax v21.4s, v21.4s, v11.4s\n"
    "str q22, [x25, x27]\n"
    "str q21, [x26, x27]\n"
    "b 7f\n"
    "5:"  // Output channel loop: Odd tail
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "ldp x24, x28, [x19], #0x10\n"
    "lsl x27, x10, #0x2\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "ldr x19, [%x[outptrs], #0x0]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "ldr x20, [%x[outptrs], #0x8]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "ldr q5, [x24, #0x0]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "ldr x21, [%x[outptrs], #0x10]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "ldr x22, [%x[outptrs], #0x18]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "ldr x23, [%x[outptrs], #0x20]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "ldr q2, [x24, #0x10]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "ldr x24, [%x[outptrs], #0x28]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "ldr x25, [%x[outptrs], #0x30]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "ldr x26, [%x[outptrs], #0x38]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "ldr q0, [x28, #0x0]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "ldr q30, [x28, #0x10]\n"
    "fmla v9.4s, v20.4s, v19.s[0]\n"
    "ldr q8, [%x[weights], #0x0]\n"
    "add %x[weights], %x[weights], #0x10\n"
    "fmla v7.4s, v20.4s, v19.s[1]\n"
    "fmla v6.4s, v20.4s, v19.s[2]\n"
    "fmla v4.4s, v20.4s, v19.s[3]\n"
    "fmla v3.4s, v20.4s, v18.s[0]\n"
    "fmla v1.4s, v20.4s, v18.s[1]\n"
    "fmla v31.4s, v20.4s, v18.s[2]\n"
    "fmla v29.4s, v20.4s, v18.s[3]\n"
    "fmla v28.4s, v20.4s, v17.s[0]\n"
    "fmla v27.4s, v20.4s, v17.s[1]\n"
    "fmla v26.4s, v20.4s, v17.s[2]\n"
    "fmla v25.4s, v20.4s, v17.s[3]\n"
    "fmla v24.4s, v20.4s, v16.s[0]\n"
    "fmla v23.4s, v20.4s, v16.s[1]\n"
    "fmla v22.4s, v20.4s, v16.s[2]\n"
    "fmla v21.4s, v20.4s, v16.s[3]\n"
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "fmin v9.4s, v9.4s, v10.4s\n"
    "fmin v7.4s, v7.4s, v10.4s\n"
    "fmin v6.4s, v6.4s, v10.4s\n"
    "fmax v9.4s, v9.4s, v11.4s\n"
    "str q9, [x19, x27]\n"
    "fmax v7.4s, v7.4s, v11.4s\n"
    "fmax v6.4s, v6.4s, v11.4s\n"
    "ldr x19, [%x[outptrs], #0x40]\n"
    "fmin v4.4s, v4.4s, v10.4s\n"
    "str q7, [x20, x27]\n"
    "fmin v3.4s, v3.4s, v10.4s\n"
    "fmin v1.4s, v1.4s, v10.4s\n"
    "str q6, [x21, x27]\n"
    "fmax v4.4s, v4.4s, v11.4s\n"
    "ldr x20, [%x[outptrs], #0x48]\n"
    "fmin v31.4s, v31.4s, v10.4s\n"
    "ldr x21, [%x[outptrs], #0x50]\n"
    "fmax v3.4s, v3.4s, v11.4s\n"
    "str q4, [x22, x27]\n"
    "fmax v1.4s, v1.4s, v11.4s\n"
    "ldr x22, [%x[outptrs], #0x58]\n"
    "fmax v31.4s, v31.4s, v11.4s\n"
    "str q3, [x23, x27]\n"
    "fmin v29.4s, v29.4s, v10.4s\n"
    "str q1, [x24, x27]\n"
    "fmin v28.4s, v28.4s, v10.4s\n"
    "str q31, [x25, x27]\n"
    "fmin v27.4s, v27.4s, v10.4s\n"
    "ldr x23, [%x[outptrs], #0x60]\n"
    "fmax v29.4s, v29.4s, v11.4s\n"
    "ldr x24, [%x[outptrs], #0x68]\n"
    "fmax v28.4s, v28.4s, v11.4s\n"
    "ldr x25, [%x[outptrs], #0x70]\n"
    "fmax v27.4s, v27.4s, v11.4s\n"
    "str q29, [x26, x27]\n"
    "fmin v26.4s, v26.4s, v10.4s\n"
    "str q28, [x19, x27]\n"
    "fmin v25.4s, v25.4s, v10.4s\n"
    "str q27, [x20, x27]\n"
    "fmin v24.4s, v24.4s, v10.4s\n"
    "ldr x26, [%x[outptrs], #0x78]\n"
    "fmax v26.4s, v26.4s, v11.4s\n"
    "str q26, [x21, x27]\n"
    "fmax v25.4s, v25.4s, v11.4s\n"
    "fmax v24.4s, v24.4s, v11.4s\n"
    "str q25, [x22, x27]\n"
    "fmin v23.4s, v23.4s, v10.4s\n"
    "fmin v22.4s, v22.4s, v10.4s\n"
    "str q24, [x23, x27]\n"
    "fmin v21.4s, v21.4s, v10.4s\n"
    "fmax v23.4s, v23.4s, v11.4s\n"
    "str q23, [x24, x27]\n"
    "fmax v22.4s, v22.4s, v11.4s\n"
    "fmax v21.4s, v21.4s, v11.4s\n"
    "str q22, [x25, x27]\n"
    "str q21, [x26, x27]\n"
    "b 7f\n"
    "6:"  // Output channel loop: Single kernel point
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "ldr x19, [%x[outptrs], #0x0]\n"
    "lsl x27, x10, #0x2\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "ldr x20, [%x[outptrs], #0x8]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "ldr x21, [%x[outptrs], #0x10]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "ldr x22, [%x[outptrs], #0x18]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "ldr x23, [%x[outptrs], #0x20]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "ldr x24, [%x[outptrs], #0x28]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "ldr x25, [%x[outptrs], #0x30]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "ldr x26, [%x[outptrs], #0x38]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "fmin v9.4s, v9.4s, v10.4s\n"
    "fmin v7.4s, v7.4s, v10.4s\n"
    "fmin v6.4s, v6.4s, v10.4s\n"
    "fmax v9.4s, v9.4s, v11.4s\n"
    "str q9, [x19, x27]\n"
    "fmax v7.4s, v7.4s, v11.4s\n"
    "fmax v6.4s, v6.4s, v11.4s\n"
    "ldr x19, [%x[outptrs], #0x40]\n"
    "fmin v4.4s, v4.4s, v10.4s\n"
    "str q7, [x20, x27]\n"
    "fmin v3.4s, v3.4s, v10.4s\n"
    "fmin v1.4s, v1.4s, v10.4s\n"
    "str q6, [x21, x27]\n"
    "fmax v4.4s, v4.4s, v11.4s\n"
    "ldr x20, [%x[outptrs], #0x48]\n"
    "fmin v31.4s, v31.4s, v10.4s\n"
    "ldr x21, [%x[outptrs], #0x50]\n"
    "fmax v3.4s, v3.4s, v11.4s\n"
    "str q4, [x22, x27]\n"
    "fmax v1.4s, v1.4s, v11.4s\n"
    "ldr x22, [%x[outptrs], #0x58]\n"
    "fmax v31.4s, v31.4s, v11.4s\n"
    "str q3, [x23, x27]\n"
    "fmin v29.4s, v29.4s, v10.4s\n"
    "str q1, [x24, x27]\n"
    "fmin v28.4s, v28.4s, v10.4s\n"
    "str q31, [x25, x27]\n"
    "fmin v27.4s, v27.4s, v10.4s\n"
    "ldr x23, [%x[outptrs], #0x60]\n"
    "fmax v29.4s, v29.4s, v11.4s\n"
    "ldr x24, [%x[outptrs], #0x68]\n"
    "fmax v28.4s, v28.4s, v11.4s\n"
    "ldr x25, [%x[outptrs], #0x70]\n"
    "fmax v27.4s, v27.4s, v11.4s\n"
    "str q29, [x26, x27]\n"
    "fmin v26.4s, v26.4s, v10.4s\n"
    "str q28, [x19, x27]\n"
    "fmin v25.4s, v25.4s, v10.4s\n"
    "str q27, [x20, x27]\n"
    "fmin v24.4s, v24.4s, v10.4s\n"
    "ldr x26, [%x[outptrs], #0x78]\n"
    "fmax v26.4s, v26.4s, v11.4s\n"
    "str q26, [x21, x27]\n"
    "fmax v25.4s, v25.4s, v11.4s\n"
    "fmax v24.4s, v24.4s, v11.4s\n"
    "str q25, [x22, x27]\n"
    "fmin v23.4s, v23.4s, v10.4s\n"
    "fmin v22.4s, v22.4s, v10.4s\n"
    "str q24, [x23, x27]\n"
    "fmin v21.4s, v21.4s, v10.4s\n"
    "fmax v23.4s, v23.4s, v11.4s\n"
    "str q23, [x24, x27]\n"
    "fmax v22.4s, v22.4s, v11.4s\n"
    "fmax v21.4s, v21.4s, v11.4s\n"
    "str q22, [x25, x27]\n"
    "str q21, [x26, x27]\n"
    "7:"  // Output channel loop: Done
    "add x10, x10, #0x4\n"
    "cmp x10, x9, LSL #2\n"
    "blt 1b\n"
    "tst %x[n_output_channels], #0x3\n"
    "beq 19f\n"
    "8:"  // Output channel oddments
    "movi v16.16b, #0x0\n"
    "cbz %x[bias], 11f\n"
    "add x19, %x[bias], x10, LSL #2\n"
    "tbz %x[n_output_channels], #1, 9f\n"
    "ld1 { v16.d }[0], [x19], #0x8\n"
    "tbz %x[n_output_channels], #0, 10f\n"
    "ld1 { v16.s }[2], [x19]\n"
    "b 10f\n"
    "9:"  // Output channel oddments: Load bias: Bit 1: Unset
    "tbz %x[n_output_channels], #0, 10f\n"
    "ld1 { v16.s }[0], [x19]\n"
    "10:"  // Output channel oddments: Load bias: Bit 1: End

    "11:"  // Output channel oddments: Load bias: Done
    "mov v9.16b, v16.16b\n"
    "ldr q8, [%x[weights], #0x0]\n"
    "mov x19, %x[inptrs]\n"
    "mov v7.16b, v16.16b\n"
    "ldp x24, x28, [x19], #0x10\n"
    "lsr x20, %x[kernel_points], #0x1\n"
    "mov v6.16b, v16.16b\n"
    "ldr q5, [x24, #0x0]\n"
    "mov v4.16b, v16.16b\n"
    "add %x[weights], %x[weights], #0x10\n"
    "mov v3.16b, v16.16b\n"
    "ldr q2, [x24, #0x10]\n"
    "mov v1.16b, v16.16b\n"
    "ldr q0, [x28, #0x0]\n"
    "mov v31.16b, v16.16b\n"
    "ldr q30, [x28, #0x10]\n"
    "mov v29.16b, v16.16b\n"
    "mov v28.16b, v16.16b\n"
    "mov v27.16b, v16.16b\n"
    "mov v26.16b, v16.16b\n"
    "mov v25.16b, v16.16b\n"
    "mov v24.16b, v16.16b\n"
    "mov v23.16b, v16.16b\n"
    "mov v22.16b, v16.16b\n"
    "mov v21.16b, v16.16b\n"
    "cbz x20, 15f\n"
    "ldp x24, x28, [x19], #0x10\n"
    "ldr q20, [%x[weights], #0x0]\n"
    "subs x20, x20, #0x1\n"
    "add %x[weights], %x[weights], #0x10\n"
    "ldr q19, [x24, #0x0]\n"
    "ldr q18, [x24, #0x10]\n"
    "ldr q17, [x28, #0x0]\n"
    "ldr q16, [x28, #0x10]\n"
    "beq 13f\n"
    "12:"  // Output channel oddments: Kernel loop
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "ldp x24, x28, [x19], #0x10\n"
    "subs x20, x20, #0x1\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "ldr q5, [x24, #0x0]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "ldr q2, [x24, #0x10]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "ldr q0, [x28, #0x0]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "ldr q30, [x28, #0x10]\n"
    "fmla v9.4s, v20.4s, v19.s[0]\n"
    "ldr q8, [%x[weights], #0x0]\n"
    "fmla v7.4s, v20.4s, v19.s[1]\n"
    "ldp x24, x28, [x19], #0x10\n"
    "fmla v6.4s, v20.4s, v19.s[2]\n"
    "fmla v4.4s, v20.4s, v19.s[3]\n"
    "ldr q19, [x24, #0x0]\n"
    "fmla v3.4s, v20.4s, v18.s[0]\n"
    "fmla v1.4s, v20.4s, v18.s[1]\n"
    "fmla v31.4s, v20.4s, v18.s[2]\n"
    "fmla v29.4s, v20.4s, v18.s[3]\n"
    "ldr q18, [x24, #0x10]\n"
    "fmla v28.4s, v20.4s, v17.s[0]\n"
    "fmla v27.4s, v20.4s, v17.s[1]\n"
    "fmla v26.4s, v20.4s, v17.s[2]\n"
    "fmla v25.4s, v20.4s, v17.s[3]\n"
    "ldr q17, [x28, #0x0]\n"
    "fmla v24.4s, v20.4s, v16.s[0]\n"
    "fmla v23.4s, v20.4s, v16.s[1]\n"
    "fmla v22.4s, v20.4s, v16.s[2]\n"
    "fmla v21.4s, v20.4s, v16.s[3]\n"
    "ldr q16, [x28, #0x10]\n"
    "ldr q20, [%x[weights], #0x10]\n"
    "add %x[weights], %x[weights], #0x20\n"
    "bgt 12b\n"
    "13:"  // Output channel oddments: Kernel loop tail
    "tbnz %x[kernel_points], #0, 14f\n"
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "fmla v9.4s, v20.4s, v19.s[0]\n"
    "fmla v7.4s, v20.4s, v19.s[1]\n"
    "fmla v6.4s, v20.4s, v19.s[2]\n"
    "fmla v4.4s, v20.4s, v19.s[3]\n"
    "fmla v3.4s, v20.4s, v18.s[0]\n"
    "fmla v1.4s, v20.4s, v18.s[1]\n"
    "fmla v31.4s, v20.4s, v18.s[2]\n"
    "fmla v29.4s, v20.4s, v18.s[3]\n"
    "fmla v28.4s, v20.4s, v17.s[0]\n"
    "fmla v27.4s, v20.4s, v17.s[1]\n"
    "fmla v26.4s, v20.4s, v17.s[2]\n"
    "fmla v25.4s, v20.4s, v17.s[3]\n"
    "fmla v24.4s, v20.4s, v16.s[0]\n"
    "fmla v23.4s, v20.4s, v16.s[1]\n"
    "fmla v22.4s, v20.4s, v16.s[2]\n"
    "fmla v21.4s, v20.4s, v16.s[3]\n"
    "b 16f\n"
    "14:"  // Output channel oddments: Odd tail
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "ldp x24, x28, [x19], #0x10\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "ldr q5, [x24, #0x0]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "ldr q2, [x24, #0x10]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "ldr q0, [x28, #0x0]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "ldr q30, [x28, #0x10]\n"
    "fmla v9.4s, v20.4s, v19.s[0]\n"
    "ldr q8, [%x[weights], #0x0]\n"
    "add %x[weights], %x[weights], #0x10\n"
    "fmla v7.4s, v20.4s, v19.s[1]\n"
    "fmla v6.4s, v20.4s, v19.s[2]\n"
    "fmla v4.4s, v20.4s, v19.s[3]\n"
    "fmla v3.4s, v20.4s, v18.s[0]\n"
    "fmla v1.4s, v20.4s, v18.s[1]\n"
    "fmla v31.4s, v20.4s, v18.s[2]\n"
    "fmla v29.4s, v20.4s, v18.s[3]\n"
    "fmla v28.4s, v20.4s, v17.s[0]\n"
    "fmla v27.4s, v20.4s, v17.s[1]\n"
    "fmla v26.4s, v20.4s, v17.s[2]\n"
    "fmla v25.4s, v20.4s, v17.s[3]\n"
    "fmla v24.4s, v20.4s, v16.s[0]\n"
    "fmla v23.4s, v20.4s, v16.s[1]\n"
    "fmla v22.4s, v20.4s, v16.s[2]\n"
    "fmla v21.4s, v20.4s, v16.s[3]\n"
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "b 16f\n"
    "15:"  // Output channel oddments: Single kernel point
    "fmla v9.4s, v8.4s, v5.s[0]\n"
    "fmla v7.4s, v8.4s, v5.s[1]\n"
    "fmla v6.4s, v8.4s, v5.s[2]\n"
    "fmla v4.4s, v8.4s, v5.s[3]\n"
    "fmla v3.4s, v8.4s, v2.s[0]\n"
    "fmla v1.4s, v8.4s, v2.s[1]\n"
    "fmla v31.4s, v8.4s, v2.s[2]\n"
    "fmla v29.4s, v8.4s, v2.s[3]\n"
    "fmla v28.4s, v8.4s, v0.s[0]\n"
    "fmla v27.4s, v8.4s, v0.s[1]\n"
    "fmla v26.4s, v8.4s, v0.s[2]\n"
    "fmla v25.4s, v8.4s, v0.s[3]\n"
    "fmla v24.4s, v8.4s, v30.s[0]\n"
    "fmla v23.4s, v8.4s, v30.s[1]\n"
    "fmla v22.4s, v8.4s, v30.s[2]\n"
    "fmla v21.4s, v8.4s, v30.s[3]\n"
    "16:"  // Output channel oddments: Done
    "fmin v9.4s, v9.4s, v10.4s\n"
    "fmin v7.4s, v7.4s, v10.4s\n"
    "fmin v6.4s, v6.4s, v10.4s\n"
    "fmin v4.4s, v4.4s, v10.4s\n"
    "fmax v9.4s, v9.4s, v11.4s\n"
    "fmax v7.4s, v7.4s, v11.4s\n"
    "fmax v6.4s, v6.4s, v11.4s\n"
    "fmax v4.4s, v4.4s, v11.4s\n"
    "fmin v3.4s, v3.4s, v10.4s\n"
    "fmin v1.4s, v1.4s, v10.4s\n"
    "fmin v31.4s, v31.4s, v10.4s\n"
    "fmax v3.4s, v3.4s, v11.4s\n"
    "fmax v1.4s, v1.4s, v11.4s\n"
    "fmax v31.4s, v31.4s, v11.4s\n"
    "fmin v29.4s, v29.4s, v10.4s\n"
    "fmin v28.4s, v28.4s, v10.4s\n"
    "fmin v27.4s, v27.4s, v10.4s\n"
    "fmax v29.4s, v29.4s, v11.4s\n"
    "fmax v28.4s, v28.4s, v11.4s\n"
    "fmax v27.4s, v27.4s, v11.4s\n"
    "fmin v26.4s, v26.4s, v10.4s\n"
    "fmin v25.4s, v25.4s, v10.4s\n"
    "fmin v24.4s, v24.4s, v10.4s\n"
    "fmax v26.4s, v26.4s, v11.4s\n"
    "fmax v25.4s, v25.4s, v11.4s\n"
    "fmax v24.4s, v24.4s, v11.4s\n"
    "fmin v23.4s, v23.4s, v10.4s\n"
    "fmin v22.4s, v22.4s, v10.4s\n"
    "fmin v21.4s, v21.4s, v10.4s\n"
    "fmax v23.4s, v23.4s, v11.4s\n"
    "fmax v22.4s, v22.4s, v11.4s\n"
    "fmax v21.4s, v21.4s, v11.4s\n"
    "tbz %x[n_output_channels], #1, 17f\n"
    "ldr x19, [%x[outptrs], #0x0]\n"
    "ldr x20, [%x[outptrs], #0x8]\n"
    "add x19, x19, x10, LSL #2\n"
    "ldr x21, [%x[outptrs], #0x10]\n"
    "ldr x22, [%x[outptrs], #0x18]\n"
    "add x20, x20, x10, LSL #2\n"
    "st1 { v9.d }[0], [x19]\n"
    "add x21, x21, x10, LSL #2\n"
    "st1 { v7.d }[0], [x20]\n"
    "ldr x23, [%x[outptrs], #0x20]\n"
    "add x22, x22, x10, LSL #2\n"
    "st1 { v6.d }[0], [x21]\n"
    "add x23, x23, x10, LSL #2\n"
    "st1 { v4.d }[0], [x22]\n"
    "ldr x24, [%x[outptrs], #0x28]\n"
    "add x24, x24, x10, LSL #2\n"
    "st1 { v3.d }[0], [x23]\n"
    "ldr x25, [%x[outptrs], #0x30]\n"
    "add x25, x25, x10, LSL #2\n"
    "st1 { v1.d }[0], [x24]\n"
    "ldr x26, [%x[outptrs], #0x38]\n"
    "add x26, x26, x10, LSL #2\n"
    "st1 { v31.d }[0], [x25]\n"
    "ldr x19, [%x[outptrs], #0x40]\n"
    "add x19, x19, x10, LSL #2\n"
    "st1 { v29.d }[0], [x26]\n"
    "ldr x20, [%x[outptrs], #0x48]\n"
    "add x20, x20, x10, LSL #2\n"
    "st1 { v28.d }[0], [x19]\n"
    "ldr x21, [%x[outptrs], #0x50]\n"
    "add x21, x21, x10, LSL #2\n"
    "st1 { v27.d }[0], [x20]\n"
    "ldr x22, [%x[outptrs], #0x58]\n"
    "add x22, x22, x10, LSL #2\n"
    "st1 { v26.d }[0], [x21]\n"
    "ldr x23, [%x[outptrs], #0x60]\n"
    "add x23, x23, x10, LSL #2\n"
    "st1 { v25.d }[0], [x22]\n"
    "ldr x24, [%x[outptrs], #0x68]\n"
    "add x24, x24, x10, LSL #2\n"
    "st1 { v24.d }[0], [x23]\n"
    "ldr x25, [%x[outptrs], #0x70]\n"
    "add x25, x25, x10, LSL #2\n"
    "st1 { v23.d }[0], [x24]\n"
    "ldr x26, [%x[outptrs], #0x78]\n"
    "add x26, x26, x10, LSL #2\n"
    "st1 { v22.d }[0], [x25]\n"
    "add x10, x10, #0x2\n"
    "st1 { v21.d }[0], [x26]\n"
    "tbz %x[n_output_channels], #0, 18f\n"
    "ldr x19, [%x[outptrs], #0x0]\n"
    "ldr x20, [%x[outptrs], #0x8]\n"
    "add x19, x19, x10, LSL #2\n"
    "ldr x21, [%x[outptrs], #0x10]\n"
    "ldr x22, [%x[outptrs], #0x18]\n"
    "add x20, x20, x10, LSL #2\n"
    "st1 { v9.s }[2], [x19]\n"
    "add x21, x21, x10, LSL #2\n"
    "st1 { v7.s }[2], [x20]\n"
    "ldr x23, [%x[outptrs], #0x20]\n"
    "add x22, x22, x10, LSL #2\n"
    "st1 { v6.s }[2], [x21]\n"
    "add x23, x23, x10, LSL #2\n"
    "st1 { v4.s }[2], [x22]\n"
    "ldr x24, [%x[outptrs], #0x28]\n"
    "add x24, x24, x10, LSL #2\n"
    "st1 { v3.s }[2], [x23]\n"
    "ldr x25, [%x[outptrs], #0x30]\n"
    "add x25, x25, x10, LSL #2\n"
    "st1 { v1.s }[2], [x24]\n"
    "ldr x26, [%x[outptrs], #0x38]\n"
    "add x26, x26, x10, LSL #2\n"
    "st1 { v31.s }[2], [x25]\n"
    "ldr x19, [%x[outptrs], #0x40]\n"
    "add x19, x19, x10, LSL #2\n"
    "st1 { v29.s }[2], [x26]\n"
    "ldr x20, [%x[outptrs], #0x48]\n"
    "add x20, x20, x10, LSL #2\n"
    "st1 { v28.s }[2], [x19]\n"
    "ldr x21, [%x[outptrs], #0x50]\n"
    "add x21, x21, x10, LSL #2\n"
    "st1 { v27.s }[2], [x20]\n"
    "ldr x22, [%x[outptrs], #0x58]\n"
    "add x22, x22, x10, LSL #2\n"
    "st1 { v26.s }[2], [x21]\n"
    "ldr x23, [%x[outptrs], #0x60]\n"
    "add x23, x23, x10, LSL #2\n"
    "st1 { v25.s }[2], [x22]\n"
    "ldr x24, [%x[outptrs], #0x68]\n"
    "add x24, x24, x10, LSL #2\n"
    "st1 { v24.s }[2], [x23]\n"
    "ldr x25, [%x[outptrs], #0x70]\n"
    "add x25, x25, x10, LSL #2\n"
    "st1 { v23.s }[2], [x24]\n"
    "ldr x26, [%x[outptrs], #0x78]\n"
    "add x26, x26, x10, LSL #2\n"
    "st1 { v22.s }[2], [x25]\n"
    "st1 { v21.s }[2], [x26]\n"
    "b 18f\n"
    "17:"  // Output channel oddments: Done: Store: Bit 1: Unset
    "tbz %x[n_output_channels], #0, 18f\n"
    "ldr x19, [%x[outptrs], #0x0]\n"
    "ldr x20, [%x[outptrs], #0x8]\n"
    "add x19, x19, x10, LSL #2\n"
    "ldr x21, [%x[outptrs], #0x10]\n"
    "ldr x22, [%x[outptrs], #0x18]\n"
    "add x20, x20, x10, LSL #2\n"
    "st1 { v9.s }[0], [x19]\n"
    "add x21, x21, x10, LSL #2\n"
    "st1 { v7.s }[0], [x20]\n"
    "ldr x23, [%x[outptrs], #0x20]\n"
    "add x22, x22, x10, LSL #2\n"
    "st1 { v6.s }[0], [x21]\n"
    "add x23, x23, x10, LSL #2\n"
    "st1 { v4.s }[0], [x22]\n"
    "ldr x24, [%x[outptrs], #0x28]\n"
    "add x24, x24, x10, LSL #2\n"
    "st1 { v3.s }[0], [x23]\n"
    "ldr x25, [%x[outptrs], #0x30]\n"
    "add x25, x25, x10, LSL #2\n"
    "st1 { v1.s }[0], [x24]\n"
    "ldr x26, [%x[outptrs], #0x38]\n"
    "add x26, x26, x10, LSL #2\n"
    "st1 { v31.s }[0], [x25]\n"
    "ldr x19, [%x[outptrs], #0x40]\n"
    "add x19, x19, x10, LSL #2\n"
    "st1 { v29.s }[0], [x26]\n"
    "ldr x20, [%x[outptrs], #0x48]\n"
    "add x20, x20, x10, LSL #2\n"
    "st1 { v28.s }[0], [x19]\n"
    "ldr x21, [%x[outptrs], #0x50]\n"
    "add x21, x21, x10, LSL #2\n"
    "st1 { v27.s }[0], [x20]\n"
    "ldr x22, [%x[outptrs], #0x58]\n"
    "add x22, x22, x10, LSL #2\n"
    "st1 { v26.s }[0], [x21]\n"
    "ldr x23, [%x[outptrs], #0x60]\n"
    "add x23, x23, x10, LSL #2\n"
    "st1 { v25.s }[0], [x22]\n"
    "ldr x24, [%x[outptrs], #0x68]\n"
    "add x24, x24, x10, LSL #2\n"
    "st1 { v24.s }[0], [x23]\n"
    "ldr x25, [%x[outptrs], #0x70]\n"
    "add x25, x25, x10, LSL #2\n"
    "st1 { v23.s }[0], [x24]\n"
    "ldr x26, [%x[outptrs], #0x78]\n"
    "add x26, x26, x10, LSL #2\n"
    "st1 { v22.s }[0], [x25]\n"
    "st1 { v21.s }[0], [x26]\n"
    "18:"  // Output channel oddments: Done: Store: Bit 1: End

    "19:"  // Done

    : [weights] "+&r" (weights)
    : [bias] "r" (bias), [inptrs] "r" (inptrs), [kernel_points] "r" ((uint64_t) kernel_points), [minmax_vals] "r" (minmax_vals), [n_output_channels] "r" ((uint64_t) n_output_channels), [outptrs] "r" (outptrs)
    : "cc", "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31", "x9", "x10", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
  );
}

}  // namespace depthwise
}  // namespace arm_conv
#endif // defined(__aarch64__)
