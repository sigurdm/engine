// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/scene_update_context_ios.h"

namespace flow {

SceneUpdateContext::SceneUpdateContext(CALayer* rootNode) : top_entity_node_(rootNode) {}

SceneUpdateContext::Entity::Entity(SceneUpdateContext& context)
    : context_(context),
      previous_entity_(context.top_entity_),
      entity_node_(nullptr) {
//      entity_node_([[CALayer alloc] init]) {
  if (previous_entity_) {
   // [previous_entity_->entity_node_ addSubLayer:entity_node_];
  } else {
  //  [top_entity_node_ addSubLayer:entity_node_];
  }
  context.top_entity_ = this;
}

SceneUpdateContext::Entity::~Entity() {
  FXL_DCHECK(context_.top_entity_ == this);
  context_.top_entity_ = previous_entity_;
}

SceneUpdateContext::ClipRect::ClipRect(SceneUpdateContext& context,
                               const SkRect& rect)
    : Entity(context) {
//        entity_node().frame = CGRectMake(rect.x(), rect.y(), rect.width(), rect.height());
//        entity_node().masksToBounds = true;
}

SceneUpdateContext::ClipRect::~ClipRect() = default;

SceneUpdateContext::Frame::Frame(SceneUpdateContext& context,
                                 const SkRRect& rrect,
                                 SkColor color,
                                 float elevation)
    : Entity(context),
      rrect_(rrect),
      color_(color),
      paint_bounds_(SkRect::MakeEmpty()) {
 // if (elevation != 0.0)
//    entity_node().SetTranslation(0.f, 0.f, elevation);
}

SceneUpdateContext::Frame::~Frame() {
//  context().CreateFrame(entity_node(), rrect_, color_, paint_bounds_,
//                        std::move(paint_layers_));
}

void SceneUpdateContext::Frame::AddPaintedLayer(Layer* layer) {
  FXL_DCHECK(layer->needs_painting());
  paint_layers_.push_back(layer);
  paint_bounds_.join(layer->paint_bounds());
}

}  // namespace flow
