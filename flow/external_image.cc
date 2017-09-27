// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/external_image.h"

namespace flow {

static std::map<int, ExternalImage*> mapping = {};

int ExternalImage::RegisterExternalImage(ExternalImage* image) {
    int id = mapping.size() + 1;
    mapping[id] = image;
    image->image_id_ = id;
    return id;
}

void ExternalImage::DisposeExternalImage(int id) {
  ExternalImage* image = mapping[id];
  mapping.erase(id);
  delete image;
}

ExternalImage* ExternalImage::GetExternalImage(int id) {
  return mapping[id];
}

}
