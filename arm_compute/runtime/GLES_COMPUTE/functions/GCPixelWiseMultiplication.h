/*
 * Copyright (c) 2017-2020 Arm Limited.
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
#ifndef ARM_COMPUTE_GCPIXELWISEMULTIPLICATION_H
#define ARM_COMPUTE_GCPIXELWISEMULTIPLICATION_H

#include "arm_compute/core/Types.h"
#include "arm_compute/runtime/GLES_COMPUTE/IGCSimpleFunction.h"

namespace arm_compute
{
class IGCTensor;

/** Basic function to run @ref GCPixelWiseMultiplicationKernel.
 *
 * @deprecated This function is deprecated and is intended to be removed in 21.05 release
 *
 */
class GCPixelWiseMultiplication : public IGCSimpleFunction
{
public:
    /** Initialise the kernel's inputs, output and convertion policy.
     *
     * @param[in]  input1   First tensor input. Data types supported: F32.
     * @param[in]  input2   Second tensor input. Data types supported: Same as @p input1.
     * @param[out] output   Output tensor. Data types supported: Same as @p input1.
     * @param[in]  scale    Scale to apply after multiplication. Must be a positive value.
     * @param[in]  act_info (Optional) Activation layer information in case of a fused activation. Currently not supported.
     */
    void configure(const IGCTensor *input1, const IGCTensor *input2, IGCTensor *output, float scale, const ActivationLayerInfo &act_info = ActivationLayerInfo());
};
}
#endif /*ARM_COMPUTE_GCPIXELWISEMULTIPLICATION_H */
