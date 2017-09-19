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
    static int registerExternalImage(ExternalImage* image);
    static void disposeExternalImage(int id);
    static ExternalImage* getExternalImage(int id);

    virtual ~ExternalImage() {};

    virtual void UpdateImage() = 0;

    // Called from GPU thread.
    virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext* grContext) = 0;

    bool first_frame_seen() { return first_frame_seen_; }
    void set_first_frame_seen() { first_frame_seen_ = true; }
    bool new_frame_ready() { return new_frame_ready_; }
    void set_new_frame_ready(bool new_frame_ready) { new_frame_ready_ = new_frame_ready; }
    int image_id() { return image_id_; }

  private:
   bool first_frame_seen_ = false;
   bool new_frame_ready_ = false;
   int image_id_;
};

}


#endif // FLUTTER_FLOW_EXTERNAL_IMAGE_H_