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


#include "stratum/hal/lib/tai/typesconverter.h"

#include <cmath>

namespace stratum {
namespace hal {
namespace tai {

/*!
 * \brief TypesConverter::HertzToMegahertz method converts \param hertz to
 * megahertz
 */
::google::protobuf::uint64 TypesConverter::HertzToMegahertz(
    tai_uint64_t hertz) {
  return hertz / kMegahertzInHertz;
}

/*!
 * \brief TypesConverter::MegahertzToHertz method converts \param megahertz to
 * hertz
 */
tai_uint64_t TypesConverter::MegahertzToHertz(
    ::google::protobuf::uint64 megahertz) {
  return megahertz * kMegahertzInHertz;
}

tai_float_t TypesConverter::Decimal64ValueToFloat(const Decimal64 &value) {
  tai_float_t ret = value.digits();
  if (value.precision() != 0) ret /= powf(10, value.precision());

  return ret;
}

/*!
 * \brief TypesConverter::FloatToDecimal64Value method converts \param value
 * that TAI float value to Decimal64 with given in
 * \param precision precision
 * \note The caller takes ownership of the returned object.
 */
Decimal64 *TypesConverter::FloatToDecimal64Value(
    tai_float_t value, ::google::protobuf::uint32 precision) {
  Decimal64 *decimalValue = new Decimal64();

  decimalValue->set_digits(static_cast<::google::protobuf::int64>(
      value * powf(10, precision)));
  decimalValue->set_precision(precision);

  return decimalValue;
}

}  // namespace tai
}  // namespace hal
}  // namespace stratum
