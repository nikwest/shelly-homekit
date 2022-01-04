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


#include "shelly_sensor_si7021.hpp"

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_si7021.h"

#include <math.h>

namespace shelly {

SI7021Sensor::SI7021Sensor(int bus_num, uint8_t i2caddr) {
  struct mgos_i2c *i2c = mgos_i2c_get_bus(bus_num);

  if (!i2c) {
    LOG(LL_ERROR, ("I2C bus missing, set i2c.enable=true in mos.yml"));
    return;
  } 

  si7021_ = mgos_si7021_create(i2c, i2caddr);

  if (!si7021_) {
    LOG(LL_ERROR, ("no si7021 sensor created."));
  } 
}

SI7021Sensor::~SI7021Sensor() {
}

StatusOr<float> SI7021Sensor::GetTemperature() {
  if (!si7021_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  float t = mgos_si7021_getTemperature(si7021_);
 
  if (t == NAN) {
    return Status(-1, "Cannot read si7021 sensor") ;
  } 
  LOG(LL_DEBUG, ("si7021 readings: t %.3f", t));

  return t;
}

StatusOr<float> SI7021Sensor::GetHumidity() {
  if (!si7021_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  double h = mgos_si7021_getHumidity(si7021_);
 
  if (h == NAN) {
    return Status(-1, "Cannot read si7021 sensor") ;
  } 
  LOG(LL_DEBUG, ("si7021 readings: h %.3f", h));

  return h;
}

}  // namespace shelly
