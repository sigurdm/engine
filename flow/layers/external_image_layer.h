// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_LAYERS_EXTERNAL_IMAGE_LAYER_H_
#define FLUTTER_FLOW_LAYERS_EXTERNAL_IMAGE_LAYER_H_


#include "flutter/flow/layers/layer.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace flow {

class ExternalImageLayer : public Layer {
 public:
  ExternalImageLayer();
  ~ExternalImageLayer() override;

  void set_offset(const SkPoint& offset) { offset_ = offset; }
  void set_size(const SkSize& size) { size_ = size; }
  void set_image_id(uint32_t image_id) { image_id_ = image_id; }

  void Preroll(PrerollContext* context, const SkMatrix& matrix) override;
  void Paint(PaintContext& context) override;

 private:
  SkPoint offset_;
  SkSize size_;
  uint32_t image_id_ = 0;

  FTL_DISALLOW_COPY_AND_ASSIGN(ExternalImageLayer);
};

}  // namespace flow

#endif  // FLUTTER_FLOW_LAYERS_EXTERNAL_IMAGE_LAYER_H_
