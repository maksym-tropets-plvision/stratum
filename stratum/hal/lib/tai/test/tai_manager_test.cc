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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "stratum/hal/lib/tai/host_interface.h"
#include "stratum/hal/lib/tai/module.h"
#include "stratum/hal/lib/tai/network_interface.h"
#include "stratum/hal/lib/tai/tai_manager.h"
#include "stratum/hal/lib/tai/tai_wrapper.h"
#include "stratum/hal/lib/tai/test/tai_object_mock.h"
#include "stratum/hal/lib/tai/test/tai_test_wrappers.h"
#include "stratum/hal/lib/tai/types_converter.h"

namespace stratum {
namespace hal {
namespace tai {

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::DoAll;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::WithArg;
using ::testing::WithArgs;

MATCHER(IsObjectSuported, "") {
  std::vector<TAIPath> supportedPathes{
      {{TAI_OBJECT_TYPE_MODULE, 0}},

      {{TAI_OBJECT_TYPE_MODULE, 0}, {TAI_OBJECT_TYPE_HOSTIF, 0}},

      {{TAI_OBJECT_TYPE_MODULE, 0}, {TAI_OBJECT_TYPE_NETWORKIF, 1}},

      {{TAI_OBJECT_TYPE_MODULE, 1}},
  };

  for (const auto& supportedPath : supportedPathes) {
    if (supportedPath == arg) return true;
  }
  return false;
}

TEST(TAIManagerTest, CorrectObjectCreation_Test) {
  std::unique_ptr<tai_wrapper_mock> wrapper(new tai_wrapper_mock());

  const TAIPath callPath = {{TAI_OBJECT_TYPE_MODULE, 0},
                            {TAI_OBJECT_TYPE_HOSTIF, 0}};

  EXPECT_CALL(*wrapper.get(), IsObjectValid(_))
      .Times(5)
      .WillOnce(Return(true))
      .WillOnce(Return(true))
      .WillOnce(Return(true))
      .WillOnce(Return(true))
      .WillOnce(Return(false));

  std::unique_ptr<TAIManagerTestWrapper> manager =
      absl::make_unique<TAIManagerTestWrapper>(std::move(wrapper));

  const TAIPath expectPath = {{TAI_OBJECT_TYPE_MODULE, 0},
                              {TAI_OBJECT_TYPE_HOSTIF, 0}};
  EXPECT_TRUE(manager->IsObjectValid({{TAI_OBJECT_TYPE_MODULE, 0}}));

  EXPECT_TRUE(manager->IsObjectValid(
      {{TAI_OBJECT_TYPE_MODULE, 0}, {TAI_OBJECT_TYPE_HOSTIF, 0}}));

  EXPECT_TRUE(manager->IsObjectValid(
      {{TAI_OBJECT_TYPE_MODULE, 0}, {TAI_OBJECT_TYPE_NETWORKIF, 1}}));

  EXPECT_TRUE(manager->IsObjectValid({{TAI_OBJECT_TYPE_MODULE, 1}}));

  EXPECT_FALSE(manager->IsObjectValid(
      {{TAI_OBJECT_TYPE_MODULE, 0}, {TAI_OBJECT_TYPE_NETWORKIF, 0}}));
}

TEST(TAIManagerTest, SetFrequencyValueWithSuccess_Test) {
  const uint64 kFrequency = 45;
  SetRequest_Request set_request;
  set_request.mutable_port()->mutable_frequency()->set_value(kFrequency);
  EXPECT_TRUE(TAIManager::IsRequestSupported(set_request));

  std::unique_ptr<tai_wrapper_mock> wrapper(new tai_wrapper_mock());
  std::shared_ptr<TAIObjectMock> object_mock =
      std::make_shared<TAIObjectMock>();

  tai_attr_metadata_t dummy_tai_metadata = {};

  EXPECT_CALL(*object_mock.get(), SetAttribute(_))
      .Times(1)
      .WillOnce(Return(TAI_STATUS_SUCCESS));

  EXPECT_CALL(*object_mock.get(), GetAlocatedAttributeObject(
                                      TAI_NETWORK_INTERFACE_ATTR_TX_LASER_FREQ))
      .Times(1)
      .WillOnce(Return(TAIAttribute(TAI_NETWORK_INTERFACE_ATTR_TX_LASER_FREQ,
                                    &dummy_tai_metadata)));

  EXPECT_CALL(*wrapper.get(),
              GetObject(TAIPath({{TAI_OBJECT_TYPE_MODULE, 0},
                                 {TAI_OBJECT_TYPE_NETWORKIF, 1}})))
      .Times(1)
      .WillOnce(Return(object_mock));

  std::unique_ptr<TAIManagerTestWrapper> manager =
      absl::make_unique<TAIManagerTestWrapper>(std::move(wrapper));

  auto kStatus = manager->SetValue(set_request, {0, 1});
  EXPECT_TRUE(kStatus.ok());
}

TEST(TAIManagerTest, GetFrequencyValueWithSuccess_Test) {
  tai_attr_metadata_t dummy_tai_metadata = {.objecttype =
                                                TAI_OBJECT_TYPE_NETWORKIF};
  dummy_tai_metadata.attrvaluetype = TAI_ATTR_VALUE_TYPE_U64;
  TAIAttribute dummy_tai_attribute(TAI_NETWORK_INTERFACE_ATTR_TX_LASER_FREQ,
                                   &dummy_tai_metadata);

  const uint64 kFrequency = 2350000;
  dummy_tai_attribute.attr.value.u64 = kFrequency;

  std::unique_ptr<tai_wrapper_mock> wrapper(new tai_wrapper_mock());
  std::shared_ptr<TAIObjectMock> object_mock =
      std::make_shared<TAIObjectMock>();

  EXPECT_CALL(*object_mock.get(), GetAttribute(dummy_tai_attribute.attr.id, _))
      .WillOnce(DoAll(Invoke([](tai_attr_id_t, tai_status_t* return_status) {
                        *return_status = TAI_STATUS_SUCCESS;
                      }),
                      Return(dummy_tai_attribute)));

  EXPECT_CALL(*wrapper.get(),
              GetObject(TAIPath({{TAI_OBJECT_TYPE_MODULE, 0},
                                 {TAI_OBJECT_TYPE_NETWORKIF, 1}})))
      .Times(1)
      .WillOnce(Return(object_mock));

  std::unique_ptr<TAIManagerTestWrapper> manager =
      absl::make_unique<TAIManagerTestWrapper>(std::move(wrapper));

  DataRequest::Request request(DataRequest::Request::default_instance());

  request.mutable_frequency();
  auto valueOrStatus = manager->GetValue(request, {0, 1});
  EXPECT_TRUE(valueOrStatus.ok());

  EXPECT_EQ(valueOrStatus.ConsumeValueOrDie().frequency().value(),
            TypesConverter::HertzToMegahertz(kFrequency));
}

TEST(TAIManagerTest, SetOutputPowerValueWithSuccess_Test) {
  SetRequest_Request set_request;
  const float kValue = 12.34f;
  set_request.clear_port();
  set_request.mutable_port()->mutable_output_power()->set_instant(kValue);

  std::unique_ptr<tai_wrapper_mock> wrapper(new tai_wrapper_mock());
  std::shared_ptr<TAIObjectMock> object_mock =
      std::make_shared<TAIObjectMock>();

  tai_attr_metadata_t dummy_tai_metadata = {};

  EXPECT_CALL(*object_mock.get(), SetAttribute(_))
      .Times(1)
      .WillOnce(Return(TAI_STATUS_SUCCESS));

  EXPECT_CALL(*object_mock.get(), GetAlocatedAttributeObject(
                                      TAI_NETWORK_INTERFACE_ATTR_OUTPUT_POWER))
      .Times(1)
      .WillOnce(Return(TAIAttribute(TAI_NETWORK_INTERFACE_ATTR_OUTPUT_POWER,
                                    &dummy_tai_metadata)));

  EXPECT_CALL(*wrapper.get(),
              GetObject(TAIPath({{TAI_OBJECT_TYPE_MODULE, 0},
                                 {TAI_OBJECT_TYPE_NETWORKIF, 1}})))
      .Times(1)
      .WillOnce(Return(object_mock));

  std::unique_ptr<TAIManagerTestWrapper> manager =
      absl::make_unique<TAIManagerTestWrapper>(std::move(wrapper));

  EXPECT_TRUE(TAIManager::IsRequestSupported(set_request));

  auto kStatus = manager->SetValue(set_request, {0, 1});
  EXPECT_TRUE(kStatus.ok());
}

TEST(TAIManagerTest, GetOutputPowerValueWithSuccess_Test) {
  std::unique_ptr<tai_wrapper_mock> wrapper(new tai_wrapper_mock());
  std::shared_ptr<TAIObjectMock> object_mock =
      std::make_shared<TAIObjectMock>();

  tai_attr_metadata_t dummy_tai_metadata = {.objecttype =
                                                TAI_OBJECT_TYPE_NETWORKIF};
  dummy_tai_metadata.attrvaluetype = TAI_ATTR_VALUE_TYPE_FLT;
  TAIAttribute dummy_tai_attribute(TAI_NETWORK_INTERFACE_ATTR_OUTPUT_POWER,
                                   &dummy_tai_metadata);

  const float kOutputValue = 5.5f;
  dummy_tai_attribute.attr.value.flt = kOutputValue;

  EXPECT_CALL(*object_mock.get(), GetAttribute(dummy_tai_attribute.attr.id, _))
      .WillOnce(DoAll(Invoke([](tai_attr_id_t, tai_status_t* return_status) {
                        *return_status = TAI_STATUS_SUCCESS;
                      }),
                      Return(dummy_tai_attribute)));

  EXPECT_CALL(*wrapper.get(),
              GetObject(TAIPath({{TAI_OBJECT_TYPE_MODULE, 0},
                                 {TAI_OBJECT_TYPE_NETWORKIF, 1}})))
      .Times(1)
      .WillOnce(Return(object_mock));

  std::unique_ptr<TAIManagerTestWrapper> manager =
      absl::make_unique<TAIManagerTestWrapper>(std::move(wrapper));

  // set get the value that was previously setted
  DataRequest::Request request(DataRequest::Request::default_instance());

  request.mutable_output_power();
  auto valueOrStatus = manager->GetValue(request, {0, 1});
  ASSERT_TRUE(valueOrStatus.ok());

  EXPECT_EQ(valueOrStatus.ConsumeValueOrDie().output_power().instant(),
            kOutputValue);
}

TEST(TAIManagerTest, GetInputPowerValueWithSuccess_Test) {
  std::unique_ptr<tai_wrapper_mock> wrapper(new tai_wrapper_mock());
  std::shared_ptr<TAIObjectMock> object_mock =
      std::make_shared<TAIObjectMock>();

  tai_attr_metadata_t dummy_tai_metadata = {.objecttype =
                                                TAI_OBJECT_TYPE_NETWORKIF};
  dummy_tai_metadata.attrvaluetype = TAI_ATTR_VALUE_TYPE_FLT;
  TAIAttribute dummy_tai_attribute(
      TAI_NETWORK_INTERFACE_ATTR_CURRENT_INPUT_POWER, &dummy_tai_metadata);

  const float kInputValue = 5.5f;
  dummy_tai_attribute.attr.value.flt = kInputValue;

  EXPECT_CALL(*object_mock.get(), GetAttribute(dummy_tai_attribute.attr.id, _))
      .WillOnce(DoAll(Invoke([](tai_attr_id_t, tai_status_t* return_status) {
                        *return_status = TAI_STATUS_SUCCESS;
                      }),
                      Return(dummy_tai_attribute)));

  EXPECT_CALL(*wrapper.get(),
              GetObject(TAIPath({{TAI_OBJECT_TYPE_MODULE, 0},
                                 {TAI_OBJECT_TYPE_NETWORKIF, 1}})))
      .Times(1)
      .WillOnce(Return(object_mock));

  std::unique_ptr<TAIManagerTestWrapper> manager =
      absl::make_unique<TAIManagerTestWrapper>(std::move(wrapper));

  DataRequest::Request request(DataRequest::Request::default_instance());

  request.mutable_input_power();
  auto valueOrStatus = manager->GetValue(request, {0, 1});
  // in stub realization there is no default value for input power
  ASSERT_TRUE(valueOrStatus.ok());

  EXPECT_EQ(valueOrStatus.ConsumeValueOrDie().input_power().instant(),
            kInputValue);
}

}  // namespace tai
}  // namespace hal
}  // namespace stratum
