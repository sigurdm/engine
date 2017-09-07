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

  void set_rect(const SkRect& rect) { rect_ = rect; }
  void set_texture_id(uint32_t texture_id) { texture_id_ = texture_id; }

  void Paint(PaintContext& context) override;

 private:
  SkRect rect_;
  uint32_t texture_id_ = 0;

  FTL_DISALLOW_COPY_AND_ASSIGN(TextureLayer);
};

}  // namespace flow

#endif  // FLUTTER_FLOW_LAYERS_TEXTURE_LAYER_H_
