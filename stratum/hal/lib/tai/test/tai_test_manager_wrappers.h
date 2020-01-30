/*
 * Copyright 2018 Google LLC
 * Copyright 2018-present Open Networking Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STRATUM_HAL_LIB_TAI_TEST_TAI_TEST_MANAGER_WRAPPERS_H_
#define STRATUM_HAL_LIB_TAI_TEST_TAI_TEST_MANAGER_WRAPPERS_H_

#include "stratum/hal/lib/tai/tai_manager.h"

#include <memory>
#include <utility>

#include "stratum/hal/lib/tai/test/tai_wrapper_mock.h"
#include "absl/memory/memory.h"

namespace stratum {
namespace hal {
namespace tai {

class TAIManagerTestWrapper : public TAIManager {
 public:
  explicit TAIManagerTestWrapper(
      std::unique_ptr<tai_wrapper_mock> wrapper_mock) {
    SetTaiWrapper(std::move(wrapper_mock));
  }
};

}  // namespace tai
}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_TAI_TEST_TAI_TEST_MANAGER_WRAPPERS_H_
