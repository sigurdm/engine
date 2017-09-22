// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/darwin/ios/platform_view_ios.h"
#include "flutter/shell/platform/darwin/ios/ios_external_image_gl.h"

#import <AVFoundation/AVFoundation.h>
#import <QuartzCore/CAEAGLLayer.h>

#include <utility>

#include "flutter/common/threads.h"
#include "flutter/fml/trace_event.h"
#include "flutter/flow/external_image.h"
#include "flutter/shell/gpu/gpu_rasterizer.h"
#include "flutter/shell/platform/darwin/common/process_info_mac.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/vsync_waiter_ios.h"
#include "lib/ftl/synchronization/waitable_event.h"

namespace shell {

AVPlayer* player;
AVPlayerItemVideoOutput* videoOutput;

PlatformViewIOS::PlatformViewIOS(CALayer* layer)
    : PlatformView(std::make_unique<GPURasterizer>(std::make_unique<ProcessInfoMac>())),
      ios_surface_(IOSSurface::Create(surface_config_, layer)),
      weak_factory_(this) {
}

PlatformViewIOS::~PlatformViewIOS() = default;

void PlatformViewIOS::SetupPlayer() {
  IOSExternalImageGL* image = new IOSExternalImageGL(this);
  flow::ExternalImage::registerExternalImage(image);
  player = [[AVPlayer alloc] init];
  FTL_LOG(INFO) << "Player: " << player;
  NSDictionary *pixBuffAttributes = @{
    (id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA/*420YpCbCr8BiPlanarVideoRange*/),
    (id)kCVPixelBufferIOSurfacePropertiesKey: @{}
  };
  videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:pixBuffAttributes];
  AVPlayerItem *item = [AVPlayerItem playerItemWithURL:[NSURL URLWithString:@"http://www.sample-videos.com/video/mp4/720/big_buck_bunny_720p_20mb.mp4"]];
  AVAsset *asset = [item asset];
  FTL_LOG(INFO) << "Asset: " << asset;
  [asset loadValuesAsynchronouslyForKeys:@[@"tracks"] completionHandler:^{
      FTL_LOG(INFO) << "Here A";
      if ([asset statusOfValueForKey:@"tracks" error:nil] == AVKeyValueStatusLoaded) {
          NSArray *tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
          if ([tracks count] > 0) {
            FTL_LOG(INFO) << "Here B";
              AVAssetTrack *videoTrack = [tracks objectAtIndex:0];
              [videoTrack loadValuesAsynchronouslyForKeys:@[@"preferredTransform"] completionHandler:^{
                  FTL_LOG(INFO) << "Here C";
                  if ([videoTrack statusOfValueForKey:@"preferredTransform" error:nil] == AVKeyValueStatusLoaded) {
                      dispatch_async(dispatch_get_main_queue(), ^{
                          FTL_LOG(INFO) << "Here D";
                          [item addOutput:videoOutput];
                          [player replaceCurrentItemWithPlayerItem:item];
                          [player play];
                          FTL_LOG(INFO) << "Here E";
                      });
                  }
              }];
          }
      }
  }];
}

CVPixelBufferRef PlatformViewIOS::GetPixelBuffer(int image_id) {
  CMTime outputItemTime = [videoOutput itemTimeForHostTime:CACurrentMediaTime()];
  if ([videoOutput hasNewPixelBufferForItemTime:outputItemTime]) {
    CVPixelBufferRef ref = [videoOutput copyPixelBufferForItemTime:outputItemTime itemTimeForDisplay:NULL];
    return ref;
  }
  FTL_LOG(INFO) << "No pixel buffer for current time";
  return nullptr;
}

void PlatformViewIOS::Attach() {
  Attach(NULL);
}

void PlatformViewIOS::Attach(ftl::Closure firstFrameCallback) {
  CreateEngine();
  PostAddToShellTask();
  if (firstFrameCallback) {
    firstFrameCallback_ = firstFrameCallback;
    rasterizer_->AddNextFrameCallback([weakSelf = GetWeakPtr()] {
      if (weakSelf) {
        weakSelf->firstFrameCallback_();
        weakSelf->firstFrameCallback_ = nullptr;
      }
    });
  }
}

void PlatformViewIOS::NotifyCreated() {
  PlatformView::NotifyCreated(ios_surface_->CreateGPUSurface());
}

void PlatformViewIOS::ToggleAccessibility(UIView* view, bool enabled) {
  if (enabled) {
    if (!accessibility_bridge_) {
      accessibility_bridge_.reset(new shell::AccessibilityBridge(view, this));
    }
  } else {
    accessibility_bridge_ = nullptr;
  }
  SetSemanticsEnabled(enabled);
}

void PlatformViewIOS::SetupAndLoadFromSource(const std::string& assets_directory,
                                             const std::string& main,
                                             const std::string& packages) {
  blink::Threads::UI()->PostTask(
      [ engine = engine().GetWeakPtr(), assets_directory, main, packages ] {
        if (engine)
          engine->RunBundleAndSource(assets_directory, main, packages);
      });
}

ftl::WeakPtr<PlatformViewIOS> PlatformViewIOS::GetWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

void PlatformViewIOS::UpdateSurfaceSize() {
  blink::Threads::Gpu()->PostTask([self = GetWeakPtr()]() {
    if (self && self->ios_surface_ != nullptr) {
      self->ios_surface_->UpdateStorageSizeIfNecessary();
    }
  });
}

VsyncWaiter* PlatformViewIOS::GetVsyncWaiter() {
  if (!vsync_waiter_) {
    vsync_waiter_ = std::make_unique<VsyncWaiterIOS>();
  }
  return vsync_waiter_.get();
}

bool PlatformViewIOS::ResourceContextMakeCurrent() {
  return ios_surface_ != nullptr ? ios_surface_->ResourceContextMakeCurrent() : false;
}

void PlatformViewIOS::UpdateSemantics(std::vector<blink::SemanticsNode> update) {
  if (accessibility_bridge_)
    accessibility_bridge_->UpdateSemantics(std::move(update));
}

void PlatformViewIOS::HandlePlatformMessage(ftl::RefPtr<blink::PlatformMessage> message) {
  platform_message_router_.HandlePlatformMessage(std::move(message));
}

void PlatformViewIOS::RunFromSource(const std::string& assets_directory,
                                    const std::string& main,
                                    const std::string& packages) {
  auto latch = new ftl::ManualResetWaitableEvent();

  dispatch_async(dispatch_get_main_queue(), ^{
    SetupAndLoadFromSource(assets_directory, main, packages);
    latch->Signal();
  });

  latch->Wait();
  delete latch;
}

}  // namespace shell
