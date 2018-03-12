// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_LAYERS_NATIVE_WIDGET_LAYER_H_
#define FLUTTER_FLOW_LAYERS_NATIVE_WIDGET_LAYER_H_

#include "flutter/flow/layers/layer.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/gpu/GrTypes.h"

namespace flow {

class NativeWidgetLayer : public Layer {
 public:
  NativeWidgetLayer();
  ~NativeWidgetLayer() override;

  void set_offset(const SkPoint& offset) { offset_ = offset; }
  void set_size(const SkSize& size) { size_ = size; }
  void set_texture_id(int64_t texture_id) { texture_id_ = texture_id; }

  void Preroll(PrerollContext* context, const SkMatrix& matrix) override;
  void Paint(PaintContext& context) const override;
  void UpdateScene(SystemCompositorContext& context) override;

 private:
  SkPoint offset_;
  SkSize size_;
  int64_t texture_id_;

  FXL_DISALLOW_COPY_AND_ASSIGN(NativeWidgetLayer);
};

}  // namespace flow

#endif  // FLUTTER_FLOW_LAYERS_NATIVE_WIDGET_LAYER_H_