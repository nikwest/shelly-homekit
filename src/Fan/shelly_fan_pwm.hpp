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

#include "mgos_timers.hpp"
#include "shelly_common.hpp"

namespace shelly {

class Fan {
 public:
  Fan(int pwm_pin, int rpm_pin);
  Fan(int pwm_pin);
  ~Fan();

  Status Init();

  StatusOr<int> GetRPM();
  void SetMaxTemp(int temp);
  void SetMinTemp(int temp);
  void Adjust(int temp);

 private:
  int max_temp_;
  int min_temp_;
  int pwm_pin_;
  int rpm_pin_;
  float pwm_ = 0.0;
  StatusOr<int> rpm_;
  mgos::Timer update_timer_;
  volatile uint32_t count_ = 0;
  static void GPIOIntHandler(int pin, void *arg);
  void UpdateCB();
};
} // namespace shelly