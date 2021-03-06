// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_CONTENT_HANDLER_RASTERIZER_H_
#define FLUTTER_CONTENT_HANDLER_RASTERIZER_H_

#include <memory>

#include "apps/mozart/services/scenic/session.fidl.h"
#include "flutter/flow/layers/layer_tree.h"
#include "lib/ftl/functional/closure.h"
#include "lib/ftl/macros.h"
#include "magenta/system/ulib/mx/include/mx/eventpair.h"

namespace flutter_runner {

class Rasterizer {
 public:
  virtual ~Rasterizer();

  static std::unique_ptr<Rasterizer> Create();

  virtual void SetScene(
      fidl::InterfaceHandle<scenic::SceneManager> scene_manager,
      mx::eventpair import_token,
      ftl::Closure metrics_changed_callback) = 0;

  virtual void Draw(std::unique_ptr<flow::LayerTree> layer_tree,
                    ftl::Closure callback) = 0;
};

}  // namespace flutter_runner

#endif  // FLUTTER_CONTENT_HANDLER_RASTERIZER_H_
