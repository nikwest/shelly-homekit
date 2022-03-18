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

#pragma once

#include "shelly_common.hpp"

namespace shelly {

class CO2Sensor {
 public:
  typedef std::function<void()> Notifier;

  CO2Sensor();
  virtual ~CO2Sensor();
  CO2Sensor(const CO2Sensor &other) = delete;

  virtual StatusOr<float> GetCO2Level() = 0;

 virtual void StartUpdating(int interval UNUSED_ARG) {
  }

  void SetNotifier(Notifier notifier);

 protected:
  Notifier notifier_;
};

}  // namespace shelly