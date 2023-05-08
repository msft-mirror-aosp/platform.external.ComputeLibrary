/*
 * Copyright (c) 2016-2020 Arm Limited.
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
#include "arm_compute/core/CL/CLHelpers.h"
#include "arm_compute/core/CL/CLKernelLibrary.h"
#include "arm_compute/core/CL/ICLTensor.h"
#include "arm_compute/core/CL/OpenCL.h"
#include "arm_compute/core/Helpers.h"
#include "arm_compute/core/Validate.h"

#include "src/core/CL/kernels/CLAbsoluteDifferenceKernel.h"
#include "src/core/helpers/WindowHelpers.h"

#include <set>
#include <string>

using namespace arm_compute;

CLAbsoluteDifferenceKernel::CLAbsoluteDifferenceKernel()
    : _input1(nullptr), _input2(nullptr), _output(nullptr)
{
}

void CLAbsoluteDifferenceKernel::configure(const ICLTensor *input1, const ICLTensor *input2, ICLTensor *output)
{
    configure(CLKernelLibrary::get().get_compile_context(), input1, input2, output);
}

void CLAbsoluteDifferenceKernel::configure(const CLCompileContext &compile_context, const ICLTensor *input1, const ICLTensor *input2, ICLTensor *output)
{
    ARM_COMPUTE_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(input1, 1, DataType::U8, DataType::S16);
    ARM_COMPUTE_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(input2, 1, DataType::U8, DataType::S16);
    ARM_COMPUTE_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(output, 1, DataType::U8, DataType::S16);
    ARM_COMPUTE_ERROR_ON_MSG(output->info()->data_type() == DataType::U8 && (input1->info()->data_type() != DataType::U8 || input2->info()->data_type() != DataType::U8),
                             "The output image can only be U8 if both input images are U8");

    _input1 = input1;
    _input2 = input2;
    _output = output;

    // Set kernel build options
    std::set<std::string> build_opts;
    build_opts.insert("-DDATA_TYPE_IN1=" + get_cl_type_from_data_type(input1->info()->data_type()));
    build_opts.insert("-DDATA_TYPE_IN2=" + get_cl_type_from_data_type(input2->info()->data_type()));
    build_opts.insert("-DDATA_TYPE_OUT=" + get_cl_type_from_data_type(output->info()->data_type()));

    // Create kernel
    _kernel = create_kernel(compile_context, "absdiff", build_opts);

    // Configure kernel window
    constexpr unsigned int num_elems_processed_per_iteration = 16;

    Window win = calculate_max_window(*input1->info(), Steps(num_elems_processed_per_iteration));

    AccessWindowHorizontal input1_access(input1->info(), 0, num_elems_processed_per_iteration);
    AccessWindowHorizontal input2_access(input2->info(), 0, num_elems_processed_per_iteration);
    AccessWindowHorizontal output_access(output->info(), 0, num_elems_processed_per_iteration);

    update_window_and_padding(win, input1_access, input2_access, output_access);

    ValidRegion valid_region = intersect_valid_regions(input1->info()->valid_region(),
                                                       input2->info()->valid_region());

    output_access.set_valid_region(win, valid_region);

    ICLKernel::configure_internal(win);
}

void CLAbsoluteDifferenceKernel::run(const Window &window, cl::CommandQueue &queue)
{
    ARM_COMPUTE_ERROR_ON_UNCONFIGURED_KERNEL(this);
    ARM_COMPUTE_ERROR_ON_INVALID_SUBWINDOW(ICLKernel::window(), window);

    Window slice = window.first_slice_window_2D();
    do
    {
        unsigned int idx = 0;
        add_2D_tensor_argument(idx, _input1, slice);
        add_2D_tensor_argument(idx, _input2, slice);
        add_2D_tensor_argument(idx, _output, slice);
        enqueue(queue, *this, slice, lws_hint());
    }
    while(window.slide_window_slice_2D(slice));
}
