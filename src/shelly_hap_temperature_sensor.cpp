/*
 * Copyright (c) Shelly-HomeKit Contributors
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "shelly_hap_temperature_sensor.hpp"

namespace shelly {
namespace hap {

TemperatureSensor::TemperatureSensor(int id, Input *in,
                                 struct mgos_config_in_sensor *cfg)
    : SensorBase(id, in, cfg, SHELLY_HAP_IID_BASE_TEMPERATUR_SENSOR,
                 &kHAPServiceType_TemperatureSensor,
                 kHAPServiceDebugDescription_TemperatureSensor) {
}

TemperatureSensor::~TemperatureSensor() {
}

Component::Type TemperatureSensor::type() const {
  return Type::kTemperatureSensor;
}

Status TemperatureSensor::Init() {
  const Status &st = SensorBase::Init();
  if (!st.ok()) return st;
  AddChar(new mgos::hap::FloatCharacteristic(
      svc_.iid + 2, &kHAPCharacteristicType_CurrentTemperature, 0.0, 100.0, 0.1,
      std::bind(&TemperatureSensor::FloatStateCharRead, this, _1, _2, _3),
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_OccupancyDetected));
  return Status::OK();
}

HAPError TemperatureSensor::FloatStateCharRead(HAPAccessoryServerRef *,
                             const HAPFloatCharacteristicReadRequest *,
                             float *value) {
  *value = temp_;
  return kHAPError_None;
}

}  // namespace hap
}  // namespace shelly
