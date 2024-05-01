// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_AHB_TEXTURE_POOL_VK_H_
#define FLUTTER_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_AHB_TEXTURE_POOL_VK_H_

#include <deque>

#include "impeller/base/thread.h"
#include "impeller/base/timing.h"
#include "impeller/renderer/backend/vulkan/android/ahb_texture_source_vk.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Maintains a bounded pool of hardware buffer backed texture
///             sources that can be used as swapchain images.
///
///             The number of cached entries in the texture pool is capped to a
///             caller specified value. Within this cap, no entry may be older
///             than the caller specified duration.
///
///             If a previously cached entry cannot be obtained from the pool, a
///             new entry is created. The only case where a valid texture source
///             cannot be obtained is due to resource exhaustion.
///
///             Pools are thread-safe.
///
class AHBTexturePoolVK {
 public:
  //----------------------------------------------------------------------------
  /// @brief      Create a new (empty) texture pool.
  ///
  /// @param[in]  context        The context whose allocators will be used to
  ///                            create the resources for the texture sources.
  /// @param[in]  desc           The descriptor of the hardware buffers that
  ///                            will be used to create the backing stores of
  ///                            the texture sources.
  /// @param[in]  max_entries    The maximum entries that will remain cached
  ///                            in the pool.
  /// @param[in]  max_extry_age  The maximum duration an entry will remain
  ///                            cached in the pool.
  ///
  explicit AHBTexturePoolVK(
      std::weak_ptr<Context> context,
      android::HardwareBufferDescriptor desc,
      size_t max_entries = 2u,
      std::chrono::milliseconds max_extry_age = std::chrono::seconds{1});

  ~AHBTexturePoolVK();

  AHBTexturePoolVK(const AHBTexturePoolVK&) = delete;

  AHBTexturePoolVK& operator=(const AHBTexturePoolVK&) = delete;

  //----------------------------------------------------------------------------
  /// @brief      If the pool can create and pool hardware buffer backed texture
  ///             sources. The only reason valid textures cannot be obtained
  ///             from a valid pool is because of resource exhaustion.
  ///
  /// @return     `true` if valid, `false` otherwise.
  ///
  bool IsValid() const;

  //----------------------------------------------------------------------------
  /// @brief      Pops an texture source from the pool. If the pool is empty, a
  ///             new texture source is created and returned.
  ///
  ///             This operation is thread-safe.
  ///
  /// @return     A texture source that can be used as a swapchain image. This
  ///             can be nullptr in case of resource exhaustion.
  ///
  std::shared_ptr<AHBTextureSourceVK> Pop();

  //----------------------------------------------------------------------------
  /// @brief      Push a popped texture back into the pool. This also performs a
  ///             GC.
  ///
  ///             This operation is thread-safe.
  ///
  /// @warning    Only a texture source obtained from the same pool can be
  ///             returned to it. It is user error to mix and match texture
  ///             sources from different pools.
  ///
  /// @param[in]  texture  The texture to be returned to the pool.
  ///
  void Push(std::shared_ptr<AHBTextureSourceVK> texture);

  //----------------------------------------------------------------------------
  /// @brief      Perform an explicit GC of the pool items. This happens
  ///             implicitly when a texture source us pushed into the pool but
  ///             one may be necessary explicitly if there is no push back into
  ///             the pool for a long time.
  ///
  void PerformGC();

 private:
  struct PoolEntry {
    TimePoint last_access_time;
    std::shared_ptr<AHBTextureSourceVK> item;

    explicit PoolEntry(std::shared_ptr<AHBTextureSourceVK> p_item)
        : last_access_time(Clock::now()), item(std::move(p_item)) {}
  };

  const std::weak_ptr<Context> context_;
  const android::HardwareBufferDescriptor desc_;
  const size_t max_entries_;
  const std::chrono::milliseconds max_extry_age_;
  bool is_valid_ = false;
  Mutex pool_mutex_;
  std::deque<PoolEntry> pool_ IPLR_GUARDED_BY(pool_mutex_);

  void PerformGCLocked() IPLR_REQUIRES(pool_mutex_);

  std::shared_ptr<AHBTextureSourceVK> CreateTexture() const;
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_RENDERER_BACKEND_VULKAN_SWAPCHAIN_AHB_AHB_TEXTURE_POOL_VK_H_
