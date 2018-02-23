// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_rasterizer.h"

#include <string>
#include <utility>

#include "flutter/common/threads.h"
#include "flutter/flow/system_compositor_context.h"
#include "flutter/glue/trace_event.h"
#include "flutter/shell/common/picture_serializer.h"
#include "flutter/shell/common/platform_view.h"
#include "flutter/shell/common/shell.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace shell {

GPURasterizer::GPURasterizer(std::unique_ptr<flow::ProcessInfo> info)
    : compositor_context_(std::move(info)), weak_factory_(this) {}

GPURasterizer::~GPURasterizer() = default;

fml::WeakPtr<Rasterizer> GPURasterizer::GetWeakRasterizerPtr() {
  return weak_factory_.GetWeakPtr();
}

void GPURasterizer::Setup(std::unique_ptr<Surface> surface,
                          #if defined(OS_IOS)
                          flow::SystemCompositorContext *systemCompositorContext,
                          #endif
                          fxl::Closure continuation,
                          fxl::AutoResetWaitableEvent* setup_completion_event) {
  surface_ = std::move(surface);
  #if defined(OS_IOS)
  system_compositor_context_ = systemCompositorContext;
  #endif
  compositor_context_.OnGrContextCreated();

  continuation();

  setup_completion_event->Signal();
}

void GPURasterizer::Clear(SkColor color, const SkISize& size) {
  if (surface_ == nullptr) {
    return;
  }

  auto frame = surface_->AcquireFrame(size);

  if (frame == nullptr) {
    return;
  }

  SkCanvas* canvas = frame->SkiaCanvas();

  if (canvas == nullptr) {
    return;
  }

  canvas->clear(color);

  frame->Submit();
}

void GPURasterizer::Teardown(
    fxl::AutoResetWaitableEvent* teardown_completion_event) {
  compositor_context_.OnGrContextDestroyed();
  if (surface_) {
    surface_.reset();
  }
  last_layer_tree_.reset();
  teardown_completion_event->Signal();
}

flow::LayerTree* GPURasterizer::GetLastLayerTree() {
  return last_layer_tree_.get();
}

void GPURasterizer::DrawLastLayerTree() {
  if (!last_layer_tree_ || !surface_) {
    return;
  }
  DrawToSurface(*last_layer_tree_);
}

flow::TextureRegistry& GPURasterizer::GetTextureRegistry() {
  return compositor_context_.texture_registry();
}

void GPURasterizer::Draw(
    fxl::RefPtr<flutter::Pipeline<flow::LayerTree>> pipeline) {
  TRACE_EVENT0("flutter", "GPURasterizer::Draw");

  flutter::Pipeline<flow::LayerTree>::Consumer consumer =
      std::bind(&GPURasterizer::DoDraw, this, std::placeholders::_1);

  // Consume as many pipeline items as possible. But yield the event loop
  // between successive tries.
  switch (pipeline->Consume(consumer)) {
    case flutter::PipelineConsumeResult::MoreAvailable: {
      auto weak_this = weak_factory_.GetWeakPtr();
      blink::Threads::Gpu()->PostTask([weak_this, pipeline]() {
        if (weak_this) {
          weak_this->Draw(pipeline);
        }
      });
      break;
    }
    default:
      break;
  }
}

void GPURasterizer::DoDraw(std::unique_ptr<flow::LayerTree> layer_tree) {
  if (!layer_tree || !surface_) {
    return;
  }

  // There is no way for the compositor to know how long the layer tree
  // construction took. Fortunately, the layer tree does. Grab that time
  // for instrumentation.
  compositor_context_.engine_time().SetLapTime(layer_tree->construction_time());

  DrawToSurface(*layer_tree);

  NotifyNextFrameOnce();

  last_layer_tree_ = std::move(layer_tree);
}

void GPURasterizer::DrawToSurface(flow::LayerTree& layer_tree) {
  auto frame = surface_->AcquireFrame(layer_tree.frame_size());

  if (frame == nullptr) {
    return;
  }

  auto canvas = frame->SkiaCanvas();

  if (canvas == nullptr) {
    return;
  }

  auto compositor_frame =
      compositor_context_.AcquireFrame(surface_->GetContext(),
                                canvas
                                #if defined(OS_IOS)
                                ,system_compositor_context_
                                #endif

      );

  canvas->clear(SK_ColorBLACK);

  layer_tree.Raster(compositor_frame);

  frame->Submit();
}

void GPURasterizer::AddNextFrameCallback(fxl::Closure nextFrameCallback) {
  nextFrameCallback_ = nextFrameCallback;
}

void GPURasterizer::NotifyNextFrameOnce() {
  if (nextFrameCallback_) {
    blink::Threads::Platform()->PostTask([callback = nextFrameCallback_] {
      TRACE_EVENT0("flutter", "GPURasterizer::NotifyNextFrameOnce");
      callback();
    });
    nextFrameCallback_ = nullptr;
  }
}

void GPURasterizer::SetTextureRegistry(flow::TextureRegistry* textureRegistry) {
  compositor_context_.SetTextureRegistry(textureRegistry);
}

}  // namespace shell
