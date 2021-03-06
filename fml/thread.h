// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FML_THREAD_H_
#define FLUTTER_FML_THREAD_H_

#include <atomic>
#include <memory>
#include <thread>

#include "lib/ftl/macros.h"
#include "lib/ftl/tasks/task_runner.h"

namespace fml {

class Thread {
 public:
  explicit Thread(const std::string& name = "");

  ~Thread();

  ftl::RefPtr<ftl::TaskRunner> GetTaskRunner() const;

  void Join();

 private:
  std::unique_ptr<std::thread> thread_;
  ftl::RefPtr<ftl::TaskRunner> task_runner_;
  std::atomic_bool joined_;

  static void SetCurrentThreadName(const std::string& name);

  FTL_DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace fml

#endif  // FLUTTER_FML_THREAD_H_
