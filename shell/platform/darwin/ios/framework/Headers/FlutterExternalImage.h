// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLUTTEREXTERNALIMAGE_H_
#define FLUTTER_FLUTTEREXTERNALIMAGE_H_

#import <CoreMedia/CoreMedia.h>
#import <Foundation/Foundation.h>

#include "FlutterMacros.h"

NS_ASSUME_NONNULL_BEGIN

FLUTTER_EXPORT
@protocol FlutterExternalImage<NSObject>
- (BOOL)hasNewImage;
- (CVPixelBufferRef)getImage;
@end

FLUTTER_EXPORT
@protocol FlutterExternalImageRegistry<NSObject>
- (NSUInteger)registerExternalImage:(NSObject<FlutterExternalImage>*)image;
- (void)unregisterExternalImage:(NSUInteger)imageId;
@end

NS_ASSUME_NONNULL_END

#endif  // FLUTTER_EXTERNALIMAGE_H_
