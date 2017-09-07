// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_TEXTURE_PROVIDER_H_
#define FLUTTER_FLOW_TEXTURE_PROVIDER_H_

#include <memory>
#include <vector>

#include "lib/ftl/build_config.h"
#include "lib/ftl/macros.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"

namespace flow {

// Strategy interface through which the platform can bind textures such as video
// with the compositor's GL context. The BindTexture method is invoked once per
// frame on the GPU thread.
class TextureProvider {
 public:
   virtual GrBackendTexture BindTexture(GrContext* grContext, uint32_t texture_id) = 0;
 };

}  // namespace flow

#endif  // FLUTTER_FLOW_TEXTURE_PROVIDER_H_
