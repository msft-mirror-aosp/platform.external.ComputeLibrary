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

#if defined(__ARM_FEATURE_SVE)

template <>
void interleave_block<2, 4, VLType::SME, true>(
  int8_t * &out, const int8_t * const *in,
  size_t width, size_t height, size_t row_offset, bool first
)
{
  __asm__ __volatile__(
      ".inst 0xd503477f  // SMSTART ZA\n"
      "mov z20.b, #0x1\n"
      "mov z19.s, #0x0\n"
      "cntb x20\n"
      "mov z18.s, #0x0\n"
      "cntw x16\n"
      "cntw x15, ALL, MUL #2\n"
      "cntw x14, ALL, MUL #3\n"
      "ptrue p2.b\n"
      "mov x19, %x[width]\n"
      "incb x19\n"
      "sub x19, x19, #0x1\n"
      "udiv x19, x19, x20\n" // n_passes = ceildiv(width, VL<T>)
      "sub x13, x19, #0x1\n"
      "lsr x13, x13, #0x1\n" // n_loops = (n_passes - 1) / 2
      "and x11, x19, #0x1\n" // odd_tail = bool(n_passes & 0x1)
      "mov x19, %x[width]\n"
      "sub x10, x20, #0x1\n"
      "ands x10, x19, x10\n"
      "csel x10, x10, x20, NE\n"
      "add x10, x10, #0x3\n"
      "lsr x10, x10, #0x2\n"
      "sub x9, x16, #0x2\n"
      "ptrue p11.s\n"
      "lsl x20, %x[height], #0x1\n" // height * 2
      "lsl x19, x16, #0x1\n"
      "whilelt p9.b, XZR, x20\n"
      "whilelt p8.b, x19, x20\n"
      "zip1 p10.b, p9.b, p8.b\n"
      "mov x28, %x[row_offset]\n"
      "mov x27, %x[out]\n"
      "mov x26, #0x0\n"
      "whilelt p9.b, x26, %x[width]\n"
      "whilelt p8.b, x26, %x[width]\n"
      "cbnz %x[first], 1f\n"
      "addvl x27, x27, #-2\n"
      "ld1w { z19.s }, p2/Z, [x27]\n"
      "ld1w { z18.s }, p2/Z, [x27, #1, MUL VL]\n"
      "1:"  // K loop: Load row sums: End
      "mov x25, %x[in]\n"
      "add x24, %x[in], x16, LSL #3\n"
      "ldr x23, [x25, #0x0]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      "add x24, x24, #0x10\n"
      "mov x12, #0x0\n"
      "cbz x9, 3f\n"
      "2:"  // K loop: Charge: Loop
      ".inst 0x25246140  // dup p0.b, p8.b/Z, p10.b[w12]\n"
      ".inst 0xe01c02e0  // ld1b { za0h.b[x12] }, p0/Z, [x23, x28]\n"
      ".inst 0x252c6140  // dup p0.b, p8.b/Z, p10.b[w12, #1]\n"
      ".inst 0x25646141  // dup p1.b, p8.b/Z, p10.b[w12, #4]\n"
      ".inst 0xe01c02c1  // ld1b { za0h.b[x12, #1] }, p0/Z, [x22, x28]\n"
      ".inst 0x256c6140  // dup p0.b, p8.b/Z, p10.b[w12, #5]\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c06a4  // ld1b { za0h.b[x12, #4] }, p1/Z, [x21, x28]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      ".inst 0xe01c0285  // ld1b { za0h.b[x12, #5] }, p0/Z, [x20, x28]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      "add x24, x24, #0x10\n"
      "add x12, x12, #0x8\n"
      "cmp x12, x9, LSL #2\n"
      "blt 2b\n"
      "3:"  // K loop: Charge: End
      ".inst 0x25246140  // dup p0.b, p8.b/Z, p10.b[w12]\n"
      ".inst 0xe01c02e0  // ld1b { za0h.b[x12] }, p0/Z, [x23, x28]\n"
      ".inst 0x252c6140  // dup p0.b, p8.b/Z, p10.b[w12, #1]\n"
      ".inst 0x25646141  // dup p1.b, p8.b/Z, p10.b[w12, #4]\n"
      ".inst 0xe01c02c1  // ld1b { za0h.b[x12, #1] }, p0/Z, [x22, x28]\n"
      ".inst 0x256c6140  // dup p0.b, p8.b/Z, p10.b[w12, #5]\n"
      "mov x25, %x[in]\n"
      ".inst 0xe01c06a4  // ld1b { za0h.b[x12, #4] }, p1/Z, [x21, x28]\n"
      "add x24, %x[in], x16, LSL #3\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c0285  // ld1b { za0h.b[x12, #5] }, p0/Z, [x20, x28]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      "add x24, x24, #0x10\n"
      "incb x28\n"
      "incb x26\n"
      "cbz x13, 9f\n"
      "mov x19, x13\n"
      "4:"  // K loop: Main loop
      "whilelt p8.b, x26, %x[width]\n"
      "mov x13, #0x0\n"
      "mov x12, #0x0\n"
      "cbz x9, 6f\n"
      "5:"  // K loop: Main loop: First: Loop
      ".inst 0x25356140  // dup p0.b, p8.b/Z, p10.b[w13, #2]\n"
      ".inst 0xe01c22e2  // ld1b { za0h.b[x13, #2] }, p0/Z, [x23, x28]\n"
      ".inst 0x253d6140  // dup p0.b, p8.b/Z, p10.b[w13, #3]\n"
      ".inst 0x25756141  // dup p1.b, p8.b/Z, p10.b[w13, #6]\n"
      ".inst 0xe01c22c3  // ld1b { za0h.b[x13, #3] }, p0/Z, [x22, x28]\n"
      ".inst 0x257d6140  // dup p0.b, p8.b/Z, p10.b[w13, #7]\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c26a6  // ld1b { za0h.b[x13, #6] }, p1/Z, [x21, x28]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      ".inst 0xe01c2287  // ld1b { za0h.b[x13, #7] }, p0/Z, [x20, x28]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      ".inst 0xc0828811  // mova z17.s, p2/M, za0v.s[x12]\n"
      ".inst 0xc0828890  // mova z16.s, p2/M, za1v.s[x12]\n"
      "add x24, x24, #0x10\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8360  // st1w { za0v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      "sdot z19.s, z17.b, z20.b\n"
      ".inst 0xc0828831  // mova z17.s, p2/M, za0v.s[x12, #1]\n"
      "sdot z18.s, z16.b, z20.b\n"
      ".inst 0xc08288b0  // mova z16.s, p2/M, za1v.s[x12, #1]\n"
      ".inst 0xe0b08364  // st1w { za1v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      ".inst 0x25706d21  // dup p1.s, p11.s/Z, p9.s[w12, #1]\n"
      ".inst 0x25706d20  // dup p0.s, p11.s/Z, p9.s[w12, #1]\n"
      "add x13, x13, #0x8\n"
      ".inst 0xe0af8761  // st1w { za0v.s[x12, #1] }, p1/Z, [x27, x15, LSL #2]\n"
      "sdot z19.s, z17.b, z20.b\n"
      "sdot z18.s, z16.b, z20.b\n"
      ".inst 0xe0ae8365  // st1w { za1v.s[x12, #1] }, p0/Z, [x27, x14, LSL #2]\n"
      "addvl x27, x27, #4\n"
      "add x12, x12, #0x2\n"
      "cmp x12, x9\n"
      "blt 5b\n"
      "6:"  // K loop: Main loop: First: Tail
      "mov x25, %x[in]\n"
      "add x24, %x[in], x16, LSL #3\n"
      ".inst 0x25356140  // dup p0.b, p8.b/Z, p10.b[w13, #2]\n"
      ".inst 0xe01c22e2  // ld1b { za0h.b[x13, #2] }, p0/Z, [x23, x28]\n"
      ".inst 0x253d6140  // dup p0.b, p8.b/Z, p10.b[w13, #3]\n"
      ".inst 0x25756141  // dup p1.b, p8.b/Z, p10.b[w13, #6]\n"
      ".inst 0xe01c22c3  // ld1b { za0h.b[x13, #3] }, p0/Z, [x22, x28]\n"
      ".inst 0x257d6140  // dup p0.b, p8.b/Z, p10.b[w13, #7]\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c26a6  // ld1b { za0h.b[x13, #6] }, p1/Z, [x21, x28]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      ".inst 0xe01c2287  // ld1b { za0h.b[x13, #7] }, p0/Z, [x20, x28]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      ".inst 0xc0828811  // mova z17.s, p2/M, za0v.s[x12]\n"
      ".inst 0xc0828890  // mova z16.s, p2/M, za1v.s[x12]\n"
      "add x24, x24, #0x10\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8360  // st1w { za0v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      "sdot z19.s, z17.b, z20.b\n"
      ".inst 0xc0828831  // mova z17.s, p2/M, za0v.s[x12, #1]\n"
      "sdot z18.s, z16.b, z20.b\n"
      ".inst 0xc08288b0  // mova z16.s, p2/M, za1v.s[x12, #1]\n"
      ".inst 0xe0b08364  // st1w { za1v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      ".inst 0x25706d21  // dup p1.s, p11.s/Z, p9.s[w12, #1]\n"
      ".inst 0x25706d20  // dup p0.s, p11.s/Z, p9.s[w12, #1]\n"
      "whilelt p9.b, x26, %x[width]\n"
      ".inst 0xe0af8761  // st1w { za0v.s[x12, #1] }, p1/Z, [x27, x15, LSL #2]\n"
      "sdot z19.s, z17.b, z20.b\n"
      "incb x26\n"
      "sdot z18.s, z16.b, z20.b\n"
      "incb x28\n"
      ".inst 0xe0ae8365  // st1w { za1v.s[x12, #1] }, p0/Z, [x27, x14, LSL #2]\n"
      "addvl x27, x27, #4\n"
      "whilelt p8.b, x26, %x[width]\n"
      "mov x13, #0x0\n"
      "mov x12, #0x0\n"
      "cbz x9, 8f\n"
      "7:"  // K loop: Main loop: Second: Loop
      ".inst 0x25256140  // dup p0.b, p8.b/Z, p10.b[w13]\n"
      ".inst 0xe01c22e0  // ld1b { za0h.b[x13] }, p0/Z, [x23, x28]\n"
      ".inst 0x252d6140  // dup p0.b, p8.b/Z, p10.b[w13, #1]\n"
      ".inst 0x25656141  // dup p1.b, p8.b/Z, p10.b[w13, #4]\n"
      ".inst 0xe01c22c1  // ld1b { za0h.b[x13, #1] }, p0/Z, [x22, x28]\n"
      ".inst 0x256d6140  // dup p0.b, p8.b/Z, p10.b[w13, #5]\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c26a4  // ld1b { za0h.b[x13, #4] }, p1/Z, [x21, x28]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      ".inst 0xe01c2285  // ld1b { za0h.b[x13, #5] }, p0/Z, [x20, x28]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      ".inst 0xc0828911  // mova z17.s, p2/M, za2v.s[x12]\n"
      ".inst 0xc0828990  // mova z16.s, p2/M, za3v.s[x12]\n"
      "add x24, x24, #0x10\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8368  // st1w { za2v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      "sdot z19.s, z17.b, z20.b\n"
      ".inst 0xc0828931  // mova z17.s, p2/M, za2v.s[x12, #1]\n"
      "sdot z18.s, z16.b, z20.b\n"
      ".inst 0xc08289b0  // mova z16.s, p2/M, za3v.s[x12, #1]\n"
      ".inst 0xe0b0836c  // st1w { za3v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      ".inst 0x25706d21  // dup p1.s, p11.s/Z, p9.s[w12, #1]\n"
      ".inst 0x25706d20  // dup p0.s, p11.s/Z, p9.s[w12, #1]\n"
      "add x13, x13, #0x8\n"
      ".inst 0xe0af8769  // st1w { za2v.s[x12, #1] }, p1/Z, [x27, x15, LSL #2]\n"
      "sdot z19.s, z17.b, z20.b\n"
      "sdot z18.s, z16.b, z20.b\n"
      ".inst 0xe0ae836d  // st1w { za3v.s[x12, #1] }, p0/Z, [x27, x14, LSL #2]\n"
      "addvl x27, x27, #4\n"
      "add x12, x12, #0x2\n"
      "cmp x12, x9\n"
      "blt 7b\n"
      "8:"  // K loop: Main loop: Second: Tail
      "mov x25, %x[in]\n"
      "add x24, %x[in], x16, LSL #3\n"
      ".inst 0x25256140  // dup p0.b, p8.b/Z, p10.b[w13]\n"
      ".inst 0xe01c22e0  // ld1b { za0h.b[x13] }, p0/Z, [x23, x28]\n"
      ".inst 0x252d6140  // dup p0.b, p8.b/Z, p10.b[w13, #1]\n"
      ".inst 0x25656141  // dup p1.b, p8.b/Z, p10.b[w13, #4]\n"
      ".inst 0xe01c22c1  // ld1b { za0h.b[x13, #1] }, p0/Z, [x22, x28]\n"
      ".inst 0x256d6140  // dup p0.b, p8.b/Z, p10.b[w13, #5]\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c26a4  // ld1b { za0h.b[x13, #4] }, p1/Z, [x21, x28]\n"
      "ldr x22, [x24, #0x0]\n"
      "ldr x21, [x25, #0x8]\n"
      ".inst 0xe01c2285  // ld1b { za0h.b[x13, #5] }, p0/Z, [x20, x28]\n"
      "ldr x20, [x24, #0x8]\n"
      "add x25, x25, #0x10\n"
      ".inst 0xc0828911  // mova z17.s, p2/M, za2v.s[x12]\n"
      ".inst 0xc0828990  // mova z16.s, p2/M, za3v.s[x12]\n"
      "add x24, x24, #0x10\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8368  // st1w { za2v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      "sdot z19.s, z17.b, z20.b\n"
      ".inst 0xc0828931  // mova z17.s, p2/M, za2v.s[x12, #1]\n"
      "sdot z18.s, z16.b, z20.b\n"
      ".inst 0xc08289b0  // mova z16.s, p2/M, za3v.s[x12, #1]\n"
      ".inst 0xe0b0836c  // st1w { za3v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      ".inst 0x25706d21  // dup p1.s, p11.s/Z, p9.s[w12, #1]\n"
      ".inst 0x25706d20  // dup p0.s, p11.s/Z, p9.s[w12, #1]\n"
      "whilelt p9.b, x26, %x[width]\n"
      ".inst 0xe0af8769  // st1w { za2v.s[x12, #1] }, p1/Z, [x27, x15, LSL #2]\n"
      "sdot z19.s, z17.b, z20.b\n"
      "incb x26\n"
      "sdot z18.s, z16.b, z20.b\n"
      "incb x28\n"
      ".inst 0xe0ae836d  // st1w { za3v.s[x12, #1] }, p0/Z, [x27, x14, LSL #2]\n"
      "addvl x27, x27, #4\n"
      "subs x19, x19, #0x1\n"
      "bgt 4b\n"
      "9:"  // K loop: Tails
      "cbnz x11, 12f\n"
      "mov x25, %x[in]\n"
      "whilelt p8.b, x26, %x[width]\n"
      "mov x13, #0x0\n"
      "mov x12, #0x0\n"
      "10:"  // K loop: Tails: Even: First
      ".inst 0xc0828811  // mova z17.s, p2/M, za0v.s[x12]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8360  // st1w { za0v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0xc0828890  // mova z16.s, p2/M, za1v.s[x12]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0x25356141  // dup p1.b, p8.b/Z, p10.b[w13, #2]\n"
      ".inst 0xe0b08364  // st1w { za1v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      ".inst 0x253d6140  // dup p0.b, p8.b/Z, p10.b[w13, #3]\n"
      "sdot z19.s, z17.b, z20.b\n"
      "sdot z18.s, z16.b, z20.b\n"
      "ldr x23, [x25, #0x0]\n"
      ".inst 0xe01c26e2  // ld1b { za0h.b[x13, #2] }, p1/Z, [x23, x28]\n"
      "ldr x22, [x25, x16, LSL #0x3]\n"
      "addvl x27, x27, #2\n"
      ".inst 0xe01c22c3  // ld1b { za0h.b[x13, #3] }, p0/Z, [x22, x28]\n"
      "add x25, x25, #0x8\n"
      "add x13, x13, #0x4\n"
      "add x12, x12, #0x1\n"
      "cmp x12, x16\n"
      "blt 10b\n"
      "whilelt p9.b, x26, %x[width]\n"
      "whilelt p8.b, x26, %x[width]\n"
      "mov x19, #0x0\n"
      "mov x12, #0x0\n"
      "11:"  // K loop: Tails: Even: Second
      ".inst 0xc0828911  // mova z17.s, p2/M, za2v.s[x12]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8368  // st1w { za2v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0xc0828990  // mova z16.s, p2/M, za3v.s[x12]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      "add x19, x19, #0x4\n"
      ".inst 0xe0b0836c  // st1w { za3v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      "addvl x27, x27, #2\n"
      "sdot z19.s, z17.b, z20.b\n"
      "sdot z18.s, z16.b, z20.b\n"
      "add x12, x12, #0x1\n"
      "cmp x12, x10\n"
      "blt 11b\n"
      "whilelt p9.b, x26, %x[width]\n"
      "b 14f\n"
      "12:"  // K loop: Tails: Odd
      "mov x12, #0x0\n"
      "13:"  // K loop: Tails: Odd: Loop
      ".inst 0xc0828811  // mova z17.s, p2/M, za0v.s[x12]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0bf8360  // st1w { za0v.s[x12] }, p0/Z, [x27, XZR, LSL #2]\n"
      ".inst 0xc0828890  // mova z16.s, p2/M, za1v.s[x12]\n"
      ".inst 0x25306d20  // dup p0.s, p11.s/Z, p9.s[w12]\n"
      ".inst 0xe0b08364  // st1w { za1v.s[x12] }, p0/Z, [x27, x16, LSL #2]\n"
      "addvl x27, x27, #2\n"
      "sdot z19.s, z17.b, z20.b\n"
      "sdot z18.s, z16.b, z20.b\n"
      "add x12, x12, #0x1\n"
      "cmp x12, x10\n"
      "blt 13b\n"
      "14:"  // K loop: End
      "st1w { z19.s }, p2, [x27]\n"
      "st1w { z18.s }, p2, [x27, #1, MUL VL]\n"
      "addvl x27, x27, #2\n"
      "mov %x[out], x27\n"
      ".inst 0xd503467f  // SMSTOP\n"
      : [out] "+&r" (out)
      : [first] "r" (first), [height] "r" (height), [in] "r" (in), [row_offset] "r" (row_offset), [width] "r" (width)
      : "cc", "memory", "p0", "p1", "p2", "p8", "p9", "p10", "p11", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25", "z26", "z27", "z28", "z29", "z30", "z31"
    );
}

#endif  // defined(__ARM_FEATURE_SVE)
