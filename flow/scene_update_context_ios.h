// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_SCENE_UPDATE_CONTEXT_IOS_H_
#define FLUTTER_FLOW_SCENE_UPDATE_CONTEXT_IOS_H_

#import <UIKit/UIKit.h>
#include "flutter/flow/compositor_context.h"

namespace flow {

class CALayer {};
class Layer;

class SceneUpdateContext {
 public:
  explicit SceneUpdateContext(CALayer* rootNode);

  class Entity {
   public:
    Entity(SceneUpdateContext& context);
    ~Entity();

    SceneUpdateContext& context() { return context_; }
    CALayer* entity_node() { return entity_node_; }

   private:
    SceneUpdateContext& context_;
    Entity* const previous_entity_;

    CALayer* entity_node_;
  };

  class ClipRect : public Entity {
   public:
    ClipRect(SceneUpdateContext& context,
         const SkRect& rect);
    ~ClipRect();
  };

  class Frame : public Entity {
   public:
    Frame(SceneUpdateContext& context,
          const SkRRect& rrect,
          SkColor color,
          float elevation);
    ~Frame();

    void AddPaintedLayer(Layer* layer);

   private:
    const SkRRect& rrect_;
    SkColor const color_;

    std::vector<Layer*> paint_layers_;
    SkRect paint_bounds_;
  };

    private:
  Entity* top_entity_ = nullptr;
  const CALayer* top_entity_node_;


};

} // namespace flow

#endif // FLUTTER_FLOW_SCENE_UPDATE_CONTEXT_IOS_H_