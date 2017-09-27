// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_EXTERNAL_IMAGE_H_
#define FLUTTER_FLOW_EXTERNAL_IMAGE_H_

#include <map>
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "flutter/common/threads.h"
#include "lib/ftl/synchronization/waitable_event.h"

namespace flow {

class ExternalImage {
  public:
    static int RegisterExternalImage(ExternalImage* image);
    static void DisposeExternalImage(int id);
    static ExternalImage* GetExternalImage(int id);

    virtual ~ExternalImage() {};

    // Called from GPU thread.
    virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext* grContext) = 0;

    int image_id() { return image_id_; }

  private:
   int image_id_;
};

} // namespace flow

#endif // FLUTTER_FLOW_EXTERNAL_IMAGE_H_
