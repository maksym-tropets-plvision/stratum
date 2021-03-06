// Copyright 2018 Google LLC
// Copyright 2018-present Open Networking Foundation
// Copyright 2019 Dell EMC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stratum/hal/lib/phal/optics_adapter.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "stratum/glue/status/status.h"
#include "stratum/hal/lib/common/utils.h"

namespace stratum {
namespace hal {
namespace phal {

OpticsAdapter::OpticsAdapter(AttributeDatabaseInterface* attribute_db_interface)
    : Adapter(ABSL_DIE_IF_NULL(attribute_db_interface)) {}

/*!
 * \brief OpticsAdapter::GetOpticalTransceiverInfo gets the information about
 * a optical transceiver module by querying TAI for the given module_id and
 * netif_id. This method is expected to return error if there is no module is
 * inserted in the given module_id yet.
 */
::util::Status OpticsAdapter::GetOpticalTransceiverInfo(
    uint64 module_id, uint32 netif_id, OpticalChannelInfo* oc_info) {

  std::vector<Path> paths = {
    {PathEntry("optical_cards", module_id, false, false, true)}
  };

  ASSIGN_OR_RETURN(auto phaldb, Get(paths));

  const uint64 frequency = phaldb->optical_cards(module_id).frequency();
  oc_info->mutable_frequency()->set_value(frequency);

  const float input_power = phaldb->optical_cards(module_id).input_power();
  oc_info->mutable_input_power()->set_instant(input_power);

  const float output_power = phaldb->optical_cards(module_id).output_power();
  oc_info->mutable_output_power()->set_instant(output_power);

  const float target_output_power
      = phaldb->optical_cards(module_id).target_output_power();
  oc_info->mutable_target_output_power()->set_value(target_output_power);

  uint64 operational_mode = phaldb->optical_cards(module_id).operational_mode();
  oc_info->mutable_operational_mode()->set_value(operational_mode);

  return ::util::OkStatus();
}

/*!
 * \brief OpticsAdapter::SetOpticalTransceiverInfo sets the data from oc_info
 * into a optical transceiver module by querying TAI for the given module_id
 * and netif_id. This method is expected to return error if there is no module
 * is inserted in the given module_id yet.
 */
::util::Status OpticsAdapter::SetOpticalTransceiverInfo(
    uint64 module_id, uint32 netif_id, const OpticalChannelInfo& oc_info) {
  AttributeValueMap attrs;
  std::vector<PathEntry> path;

  path = {PathEntry("optical_cards", module_id),
                                 PathEntry("frequency")};
  if (oc_info.has_frequency())
    attrs[path] = oc_info.frequency().value();

  path = {PathEntry("optical_cards", module_id),
                                 PathEntry("target_output_power")};
  if (oc_info.has_target_output_power())
    attrs[path] = oc_info.target_output_power().value();

  path = {PathEntry("optical_cards", module_id),
                                 PathEntry("operational_mode")};
  if (oc_info.has_operational_mode())
    attrs[path] = oc_info.operational_mode().value();

  return Set(attrs);
}

}  // namespace phal
}  // namespace hal
}  // namespace stratum
