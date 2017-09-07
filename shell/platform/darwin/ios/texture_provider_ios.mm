// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/darwin/ios/texture_provider_ios.h"

namespace shell {

TextureProviderIOS::TextureProviderIOS() = default;

TextureProviderIOS::~TextureProviderIOS() = default;

GrBackendTexture TextureProviderIOS::BindTexture(GrContext* gr_context, uint32_t texture_id) {
  if (texture_id != 87) {
    return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
  }
  if (bufferCount == -1) {
    NSLog(@"Setting up media");
    NSURL* url = [NSURL URLWithString:@"https://dwknz3zfy9iu1.cloudfront.net/uscenes_h-264_uhd_test.mp4"];
    asset_ = [[AVURLAsset URLAssetWithURL:url options:nil] retain];
    if (!asset_) {
      return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
    }
    AVAssetTrack* videoTrack = [asset_ tracks][0];
    NSDictionary* decoderOutputOptions = @{(NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange)};
    track_output_ = [[AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:videoTrack
                                                                outputSettings:decoderOutputOptions] retain];
    reader_ = [[AVAssetReader assetReaderWithAsset:asset_ error:nil] retain];
    if (!reader_) {
      NSLog(@"Failed to create reader");
      return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
    }
    [reader_ addOutput:track_output_];
    [reader_ startReading];
    CVReturn cacheCreated = CVOpenGLESTextureCacheCreate(nil, nil, [EAGLContext currentContext], nil, &texture_cache_);
    if (cacheCreated != kCVReturnSuccess) {
      NSLog(@"Failed to create texture cache");
      return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
    }
    NSLog(@"Media setup complete");
    bufferCount = 0;
  }
  if (bufferCount < 0) {
    NSLog(@"Failed to setup media");
    return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
  }
  NSLog(@"Getting buffer %d", bufferCount);
  CMSampleBufferRef buffer = [track_output_ copyNextSampleBuffer];
  if (!buffer) {
    NSLog(@"End of stream");
    return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
  }
  bufferCount++;
  __block CVOpenGLESTextureRef texture;
  __block CVReturn textureCreated;
  __block CMVideoDimensions dimensions;
  CMSampleBufferCallBlockForEachSample(buffer, ^OSStatus(CMSampleBufferRef sampleBuffer, CMItemCount count) {
    sampleCount++;
    //CMTime duration = CMSampleBufferGetOutputDuration(sampleBuffer);
    CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
    dimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    textureCreated = CVOpenGLESTextureCacheCreateTextureFromImage(nil,
                                                                           texture_cache_,
                                                                           imageBuffer,
                                                                           nil,
                                                                           GL_TEXTURE_2D,
                                                                           GL_RGBA,
                                                                           dimensions.width,
                                                                           dimensions.height,
                                                                           GL_RGBA,
                                                                           GL_UNSIGNED_BYTE,
                                                                           0,
                                                                           &texture);
    if (textureCreated == kCVReturnSuccess) {
      NSLog(@"Texture id: %d", CVOpenGLESTextureGetName(texture));
    } else {
      NSLog(@"Failed to create and cache texture %d", textureCreated);
    }
    //CFRelease(buffer);
    return 0;
  });
  if (textureCreated) {
    return GrBackendTexture(dimensions.width, dimensions.height, kRGBA_8888_GrPixelConfig, { CVOpenGLESTextureGetTarget(texture), CVOpenGLESTextureGetName(texture) });
  } else {
    return GrBackendTexture(0, 0, kUnknown_GrPixelConfig, {});
  }
}
}  // namespace shell
