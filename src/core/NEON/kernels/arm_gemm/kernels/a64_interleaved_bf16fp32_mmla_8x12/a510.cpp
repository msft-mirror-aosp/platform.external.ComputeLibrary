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
#ifdef __aarch64__

#include <cstddef>
#include "../../bfloat.hpp"

namespace arm_gemm {

void a64_interleaved_bf16fp32_mmla_8x12_a510(
    const bfloat16 *Apanel, const bfloat16 *Bpanel,
    float *Cpanel, int ablocks, int bblocks, int K) {

    struct KernelArgs {
        size_t bblocks = {};
        size_t K = {};
        const bfloat16 *Bpanel = {};
    } ka;

    ka.bblocks = bblocks;
    ka.K = (K/4) - 1;
    ka.Bpanel = Bpanel;

    __asm__ __volatile__(

      "1:"  // Height loop
      "ldr x22, [%x[args_ptr], %[offsetof_bblocks]]\n"
      "mov x21, %x[Apanel]\n"
      "ldr x20, [%x[args_ptr], %[offsetof_Bpanel]]\n"
      "2:"  // Width loop
      "mov %x[Apanel], x21\n"
      "ldr x19, [%x[args_ptr], %[offsetof_K]]\n"
      "ld1 { v0.8h }, [%x[Apanel]], #0x10\n"
      "ld1 { v4.8h }, [x20], #0x10\n"
      "ld1 { v1.8h }, [%x[Apanel]], #0x10\n"
      "cmp x19, #0x2\n"
      "movi v8.16b, #0x0\n"
      "movi v9.16b, #0x0\n"
      "ld1 { v5.8h }, [x20], #0x10\n"
      "movi v10.16b, #0x0\n"
      "movi v11.16b, #0x0\n"
      "ld1 { v2.8h }, [%x[Apanel]], #0x10\n"
      "movi v12.16b, #0x0\n"
      "movi v13.16b, #0x0\n"
      "movi v14.16b, #0x0\n"
      "movi v15.16b, #0x0\n"
      "movi v16.16b, #0x0\n"
      "movi v17.16b, #0x0\n"
      "movi v18.16b, #0x0\n"
      "movi v19.16b, #0x0\n"
      "movi v20.16b, #0x0\n"
      "movi v21.16b, #0x0\n"
      "movi v22.16b, #0x0\n"
      "movi v23.16b, #0x0\n"
      "movi v24.16b, #0x0\n"
      "movi v25.16b, #0x0\n"
      "movi v26.16b, #0x0\n"
      "movi v27.16b, #0x0\n"
      "movi v28.16b, #0x0\n"
      "movi v29.16b, #0x0\n"
      "movi v30.16b, #0x0\n"
      "movi v31.16b, #0x0\n"
      "blt 4f\n"
      "3:"  // main loop head
      "ld1 { v3.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e44ec08  // bfmmla v8.4s, v0.8h, v4.8h\n"
      ".inst 0x6e44ec2e  // bfmmla v14.4s, v1.8h, v4.8h\n"
      "ldp q6, q7, [x20], #0x20\n"
      ".inst 0x6e45ec0b  // bfmmla v11.4s, v0.8h, v5.8h\n"
      ".inst 0x6e45ec31  // bfmmla v17.4s, v1.8h, v5.8h\n"
      ".inst 0x6e44ec54  // bfmmla v20.4s, v2.8h, v4.8h\n"
      "sub x19, x19, #0x2\n"
      ".inst 0x6e45ec57  // bfmmla v23.4s, v2.8h, v5.8h\n"
      ".inst 0x6e44ec7a  // bfmmla v26.4s, v3.8h, v4.8h\n"
      "cmp x19, #0x2\n"
      ".inst 0x6e45ec7d  // bfmmla v29.4s, v3.8h, v5.8h\n"
      "ldp q4, q5, [x20], #0x20\n"
      ".inst 0x6e46ec09  // bfmmla v9.4s, v0.8h, v6.8h\n"
      ".inst 0x6e47ec0c  // bfmmla v12.4s, v0.8h, v7.8h\n"
      ".inst 0x6e44ec0a  // bfmmla v10.4s, v0.8h, v4.8h\n"
      ".inst 0x6e45ec0d  // bfmmla v13.4s, v0.8h, v5.8h\n"
      "ld1 { v0.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e46ec2f  // bfmmla v15.4s, v1.8h, v6.8h\n"
      ".inst 0x6e46ec55  // bfmmla v21.4s, v2.8h, v6.8h\n"
      ".inst 0x6e46ec7b  // bfmmla v27.4s, v3.8h, v6.8h\n"
      ".inst 0x6e47ec32  // bfmmla v18.4s, v1.8h, v7.8h\n"
      "ld1 { v6.8h }, [x20], #0x10\n"
      ".inst 0x6e44ec30  // bfmmla v16.4s, v1.8h, v4.8h\n"
      ".inst 0x6e45ec33  // bfmmla v19.4s, v1.8h, v5.8h\n"
      "ld1 { v1.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e47ec58  // bfmmla v24.4s, v2.8h, v7.8h\n"
      ".inst 0x6e47ec7e  // bfmmla v30.4s, v3.8h, v7.8h\n"
      "ld1 { v7.8h }, [x20], #0x10\n"
      ".inst 0x6e44ec56  // bfmmla v22.4s, v2.8h, v4.8h\n"
      ".inst 0x6e45ec59  // bfmmla v25.4s, v2.8h, v5.8h\n"
      "ld1 { v2.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e44ec7c  // bfmmla v28.4s, v3.8h, v4.8h\n"
      ".inst 0x6e45ec7f  // bfmmla v31.4s, v3.8h, v5.8h\n"
      "ld1 { v3.8h }, [%x[Apanel]], #0x10\n"
      "ldp q4, q5, [x20], #0x20\n"
      ".inst 0x6e46ec08  // bfmmla v8.4s, v0.8h, v6.8h\n"
      ".inst 0x6e46ec2e  // bfmmla v14.4s, v1.8h, v6.8h\n"
      ".inst 0x6e47ec0b  // bfmmla v11.4s, v0.8h, v7.8h\n"
      ".inst 0x6e47ec31  // bfmmla v17.4s, v1.8h, v7.8h\n"
      ".inst 0x6e46ec54  // bfmmla v20.4s, v2.8h, v6.8h\n"
      ".inst 0x6e47ec57  // bfmmla v23.4s, v2.8h, v7.8h\n"
      ".inst 0x6e46ec7a  // bfmmla v26.4s, v3.8h, v6.8h\n"
      ".inst 0x6e47ec7d  // bfmmla v29.4s, v3.8h, v7.8h\n"
      "ldp q6, q7, [x20], #0x20\n"
      ".inst 0x6e44ec09  // bfmmla v9.4s, v0.8h, v4.8h\n"
      ".inst 0x6e45ec0c  // bfmmla v12.4s, v0.8h, v5.8h\n"
      ".inst 0x6e46ec0a  // bfmmla v10.4s, v0.8h, v6.8h\n"
      ".inst 0x6e47ec0d  // bfmmla v13.4s, v0.8h, v7.8h\n"
      "ld1 { v0.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e44ec2f  // bfmmla v15.4s, v1.8h, v4.8h\n"
      ".inst 0x6e44ec55  // bfmmla v21.4s, v2.8h, v4.8h\n"
      ".inst 0x6e44ec7b  // bfmmla v27.4s, v3.8h, v4.8h\n"
      ".inst 0x6e45ec32  // bfmmla v18.4s, v1.8h, v5.8h\n"
      "ld1 { v4.8h }, [x20], #0x10\n"
      ".inst 0x6e46ec30  // bfmmla v16.4s, v1.8h, v6.8h\n"
      ".inst 0x6e47ec33  // bfmmla v19.4s, v1.8h, v7.8h\n"
      "ld1 { v1.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e45ec58  // bfmmla v24.4s, v2.8h, v5.8h\n"
      ".inst 0x6e45ec7e  // bfmmla v30.4s, v3.8h, v5.8h\n"
      "ld1 { v5.8h }, [x20], #0x10\n"
      ".inst 0x6e46ec56  // bfmmla v22.4s, v2.8h, v6.8h\n"
      ".inst 0x6e47ec59  // bfmmla v25.4s, v2.8h, v7.8h\n"
      "ld1 { v2.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e46ec7c  // bfmmla v28.4s, v3.8h, v6.8h\n"
      ".inst 0x6e47ec7f  // bfmmla v31.4s, v3.8h, v7.8h\n"
      "bge 3b\n"
      "4:"  // main loop skip
      "ld1 { v3.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e44ec08  // bfmmla v8.4s, v0.8h, v4.8h\n"
      ".inst 0x6e44ec2e  // bfmmla v14.4s, v1.8h, v4.8h\n"
      "ldp q6, q7, [x20], #0x20\n"
      ".inst 0x6e45ec0b  // bfmmla v11.4s, v0.8h, v5.8h\n"
      ".inst 0x6e45ec31  // bfmmla v17.4s, v1.8h, v5.8h\n"
      ".inst 0x6e44ec54  // bfmmla v20.4s, v2.8h, v4.8h\n"
      ".inst 0x6e45ec57  // bfmmla v23.4s, v2.8h, v5.8h\n"
      ".inst 0x6e44ec7a  // bfmmla v26.4s, v3.8h, v4.8h\n"
      ".inst 0x6e45ec7d  // bfmmla v29.4s, v3.8h, v5.8h\n"
      "ldp q4, q5, [x20], #0x20\n"
      ".inst 0x6e46ec09  // bfmmla v9.4s, v0.8h, v6.8h\n"
      ".inst 0x6e46ec2f  // bfmmla v15.4s, v1.8h, v6.8h\n"
      ".inst 0x6e46ec55  // bfmmla v21.4s, v2.8h, v6.8h\n"
      ".inst 0x6e46ec7b  // bfmmla v27.4s, v3.8h, v6.8h\n"
      ".inst 0x6e47ec0c  // bfmmla v12.4s, v0.8h, v7.8h\n"
      ".inst 0x6e44ec0a  // bfmmla v10.4s, v0.8h, v4.8h\n"
      ".inst 0x6e45ec0d  // bfmmla v13.4s, v0.8h, v5.8h\n"
      ".inst 0x6e47ec32  // bfmmla v18.4s, v1.8h, v7.8h\n"
      ".inst 0x6e44ec30  // bfmmla v16.4s, v1.8h, v4.8h\n"
      ".inst 0x6e45ec33  // bfmmla v19.4s, v1.8h, v5.8h\n"
      ".inst 0x6e47ec58  // bfmmla v24.4s, v2.8h, v7.8h\n"
      ".inst 0x6e47ec7e  // bfmmla v30.4s, v3.8h, v7.8h\n"
      ".inst 0x6e44ec56  // bfmmla v22.4s, v2.8h, v4.8h\n"
      ".inst 0x6e45ec59  // bfmmla v25.4s, v2.8h, v5.8h\n"
      ".inst 0x6e44ec7c  // bfmmla v28.4s, v3.8h, v4.8h\n"
      ".inst 0x6e45ec7f  // bfmmla v31.4s, v3.8h, v5.8h\n"
      "cbz x19, 5f\n"
      "ld1 { v0.8h }, [%x[Apanel]], #0x10\n"
      "ld1 { v6.8h }, [x20], #0x10\n"
      ".inst 0x6e46ec08  // bfmmla v8.4s, v0.8h, v6.8h\n"
      "ld1 { v1.8h }, [%x[Apanel]], #0x10\n"
      "ld1 { v7.8h }, [x20], #0x10\n"
      ".inst 0x6e46ec2e  // bfmmla v14.4s, v1.8h, v6.8h\n"
      "ld1 { v2.8h }, [%x[Apanel]], #0x10\n"
      "ld1 { v3.8h }, [%x[Apanel]], #0x10\n"
      ".inst 0x6e47ec0b  // bfmmla v11.4s, v0.8h, v7.8h\n"
      "ldp q4, q5, [x20], #0x20\n"
      ".inst 0x6e47ec31  // bfmmla v17.4s, v1.8h, v7.8h\n"
      ".inst 0x6e46ec54  // bfmmla v20.4s, v2.8h, v6.8h\n"
      ".inst 0x6e47ec57  // bfmmla v23.4s, v2.8h, v7.8h\n"
      ".inst 0x6e46ec7a  // bfmmla v26.4s, v3.8h, v6.8h\n"
      ".inst 0x6e47ec7d  // bfmmla v29.4s, v3.8h, v7.8h\n"
      "ldp q6, q7, [x20], #0x20\n"
      ".inst 0x6e44ec09  // bfmmla v9.4s, v0.8h, v4.8h\n"
      ".inst 0x6e44ec2f  // bfmmla v15.4s, v1.8h, v4.8h\n"
      ".inst 0x6e44ec55  // bfmmla v21.4s, v2.8h, v4.8h\n"
      ".inst 0x6e44ec7b  // bfmmla v27.4s, v3.8h, v4.8h\n"
      ".inst 0x6e45ec0c  // bfmmla v12.4s, v0.8h, v5.8h\n"
      ".inst 0x6e46ec0a  // bfmmla v10.4s, v0.8h, v6.8h\n"
      ".inst 0x6e47ec0d  // bfmmla v13.4s, v0.8h, v7.8h\n"
      ".inst 0x6e45ec32  // bfmmla v18.4s, v1.8h, v5.8h\n"
      ".inst 0x6e46ec30  // bfmmla v16.4s, v1.8h, v6.8h\n"
      ".inst 0x6e47ec33  // bfmmla v19.4s, v1.8h, v7.8h\n"
      ".inst 0x6e45ec58  // bfmmla v24.4s, v2.8h, v5.8h\n"
      ".inst 0x6e45ec7e  // bfmmla v30.4s, v3.8h, v5.8h\n"
      ".inst 0x6e46ec56  // bfmmla v22.4s, v2.8h, v6.8h\n"
      ".inst 0x6e47ec59  // bfmmla v25.4s, v2.8h, v7.8h\n"
      ".inst 0x6e46ec7c  // bfmmla v28.4s, v3.8h, v6.8h\n"
      ".inst 0x6e47ec7f  // bfmmla v31.4s, v3.8h, v7.8h\n"
      "5:"  // multiply loop done
      "subs x22, x22, #0x1\n"
      "uzp1 v4.2d, v8.2d, v11.2d\n"
      "uzp2 v8.2d, v8.2d, v11.2d\n"
      "uzp1 v11.2d, v9.2d, v12.2d\n"
      "uzp2 v9.2d, v9.2d, v12.2d\n"
      "str q4, [%x[Cpanel], #0x0]\n"
      "uzp1 v12.2d, v10.2d, v13.2d\n"
      "uzp2 v10.2d, v10.2d, v13.2d\n"
      "str q11, [%x[Cpanel], #0x10]\n"
      "str q12, [%x[Cpanel], #0x20]\n"
      "uzp1 v13.2d, v14.2d, v17.2d\n"
      "uzp2 v14.2d, v14.2d, v17.2d\n"
      "str q8, [%x[Cpanel], #0x30]\n"
      "uzp1 v17.2d, v15.2d, v18.2d\n"
      "uzp2 v15.2d, v15.2d, v18.2d\n"
      "str q9, [%x[Cpanel], #0x40]\n"
      "uzp1 v18.2d, v16.2d, v19.2d\n"
      "uzp2 v16.2d, v16.2d, v19.2d\n"
      "str q10, [%x[Cpanel], #0x50]\n"
      "uzp1 v19.2d, v20.2d, v23.2d\n"
      "uzp2 v20.2d, v20.2d, v23.2d\n"
      "str q13, [%x[Cpanel], #0x60]\n"
      "uzp1 v23.2d, v21.2d, v24.2d\n"
      "uzp2 v21.2d, v21.2d, v24.2d\n"
      "str q17, [%x[Cpanel], #0x70]\n"
      "uzp1 v24.2d, v22.2d, v25.2d\n"
      "uzp2 v22.2d, v22.2d, v25.2d\n"
      "str q18, [%x[Cpanel], #0x80]\n"
      "uzp1 v25.2d, v26.2d, v29.2d\n"
      "uzp2 v26.2d, v26.2d, v29.2d\n"
      "str q14, [%x[Cpanel], #0x90]\n"
      "uzp1 v29.2d, v27.2d, v30.2d\n"
      "uzp2 v27.2d, v27.2d, v30.2d\n"
      "str q15, [%x[Cpanel], #0xa0]\n"
      "uzp1 v30.2d, v28.2d, v31.2d\n"
      "uzp2 v28.2d, v28.2d, v31.2d\n"
      "str q16, [%x[Cpanel], #0xb0]\n"
      "str q19, [%x[Cpanel], #0xc0]\n"
      "str q23, [%x[Cpanel], #0xd0]\n"
      "str q24, [%x[Cpanel], #0xe0]\n"
      "str q20, [%x[Cpanel], #0xf0]\n"
      "str q21, [%x[Cpanel], #0x100]\n"
      "str q22, [%x[Cpanel], #0x110]\n"
      "str q25, [%x[Cpanel], #0x120]\n"
      "str q29, [%x[Cpanel], #0x130]\n"
      "str q30, [%x[Cpanel], #0x140]\n"
      "str q26, [%x[Cpanel], #0x150]\n"
      "str q27, [%x[Cpanel], #0x160]\n"
      "str q28, [%x[Cpanel], #0x170]\n"
      "add %x[Cpanel], %x[Cpanel], #0x180\n"
      "bgt 2b\n"
      "subs %x[ablocks], %x[ablocks], #0x1\n"
      "bne 1b\n"
      : [Apanel] "+&r" (Apanel), [Cpanel] "+&r" (Cpanel), [ablocks] "+&r" (ablocks)
      : [args_ptr] "r" (&ka), [offsetof_Bpanel] "I" (offsetof(KernelArgs, Bpanel)), [offsetof_K] "I" (offsetof(KernelArgs, K)), [offsetof_bblocks] "I" (offsetof(KernelArgs, bblocks))
      : "cc", "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31", "x19", "x20", "x21", "x22"
    );
}

} // namespace arm_gemm
#endif // __aarch64__
