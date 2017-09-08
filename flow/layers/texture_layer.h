// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_LAYERS_TEXTURE_LAYER_H_
#define FLUTTER_FLOW_LAYERS_TEXTURE_LAYER_H_

#include "flutter/flow/layers/layer.h"

namespace flow {

class TextureLayer : public Layer {
 public:
  TextureLayer();
  ~TextureLayer() override;

  void set_offset(const SkPoint& offset) { offset_ = offset; }
  void set_size(const SkSize& size) { size_ = size; }
  void set_texture_id(uint32_t texture_id) { texture_id_ = texture_id; }

  void Preroll(PrerollContext* context, const SkMatrix& matrix) override;
  void Paint(PaintContext& context) override;

 private:
  SkPoint offset_;
  SkSize size_;
  uint32_t texture_id_ = 0;

  FTL_DISALLOW_COPY_AND_ASSIGN(TextureLayer);
};

}  // namespace flow

#endif  // FLUTTER_FLOW_LAYERS_TEXTURE_LAYER_H_
