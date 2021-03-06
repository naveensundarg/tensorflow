/* Copyright 2015 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMMON_RUNTIME_GPU_GPU_DEBUG_ALLOCATOR_H_
#define TENSORFLOW_COMMON_RUNTIME_GPU_GPU_DEBUG_ALLOCATOR_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "tensorflow/stream_executor/stream_executor.h"
#include "tensorflow/core/common_runtime/gpu/visitable_allocator.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow/core/platform/port.h"

namespace tensorflow {

// An allocator that wraps a GPU allocator and adds debugging
// functionality that verifies that users do not write outside their
// allocated memory.
class GPUDebugAllocator : public VisitableAllocator {
 public:
  explicit GPUDebugAllocator(VisitableAllocator* allocator, int device_id);
  ~GPUDebugAllocator() override;
  string Name() override { return "gpu_debug"; }
  void* AllocateRaw(size_t alignment, size_t num_bytes) override;
  void DeallocateRaw(void* ptr) override;
  void AddAllocVisitor(Visitor visitor) override;
  void AddFreeVisitor(Visitor visitor) override;
  bool TracksAllocationSizes() override;
  size_t RequestedSize(void* ptr) override;
  size_t AllocatedSize(void* ptr) override;
  int64 AllocationId(void* ptr) override;

  // For testing.
  bool CheckHeader(void* ptr);
  bool CheckFooter(void* ptr);

 private:
  VisitableAllocator* base_allocator_ = nullptr;  // owned

  perftools::gputools::StreamExecutor* stream_exec_;  // Not owned.

  TF_DISALLOW_COPY_AND_ASSIGN(GPUDebugAllocator);
};

// An allocator that wraps a GPU allocator and resets the memory on
// allocation and free to 'NaN', helping to identify cases where the
// user forgets to initialize the memory.
class GPUNanResetAllocator : public VisitableAllocator {
 public:
  explicit GPUNanResetAllocator(VisitableAllocator* allocator, int device_id);
  ~GPUNanResetAllocator() override;
  string Name() override { return "gpu_nan_reset"; }
  void* AllocateRaw(size_t alignment, size_t num_bytes) override;
  void DeallocateRaw(void* ptr) override;
  void AddAllocVisitor(Visitor visitor) override;
  void AddFreeVisitor(Visitor visitor) override;
  size_t RequestedSize(void* ptr) override;
  size_t AllocatedSize(void* ptr) override;

 private:
  VisitableAllocator* base_allocator_ = nullptr;  // owned

  perftools::gputools::StreamExecutor* stream_exec_;  // Not owned.

  TF_DISALLOW_COPY_AND_ASSIGN(GPUNanResetAllocator);
};

}  // namespace tensorflow

#endif  // TENSORFLOW_COMMON_RUNTIME_GPU_GPU_DEBUG_ALLOCATOR_H_
