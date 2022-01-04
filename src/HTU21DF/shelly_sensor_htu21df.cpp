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

#include "shelly_sensor_htu21df.hpp"

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_htu21df.h"

#include <math.h>

namespace shelly {

HTU21DFSensor::HTU21DFSensor(int bus_num, uint8_t i2caddr) {
  struct mgos_i2c *i2c = mgos_i2c_get_bus(bus_num);

  if (!i2c) {
    LOG(LL_ERROR, ("I2C bus missing, set i2c.enable=true in mos.yml"));
    return;
  } 

  htu21df_ = mgos_htu21df_create(i2c, i2caddr);

  if (!htu21df_) {
    LOG(LL_ERROR, ("no htu21df sensor created."));
  } 
}

HTU21DFSensor::~HTU21DFSensor() {
}

StatusOr<float> HTU21DFSensor::GetTemperature() {
  if (!htu21df_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  float t = mgos_htu21df_getTemperature(htu21df_);
 
  if (t == NAN) {
    return Status(-1, "Cannot read htu21df sensor") ;
  } 
  LOG(LL_DEBUG, ("htu21df readings: t %.3f", t));

  return t;
}

StatusOr<float> HTU21DFSensor::GetHumidity() {
  if (!htu21df_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  double h = mgos_htu21df_getHumidity(htu21df_);
 
  if (h == NAN) {
    return Status(-1, "Cannot read htu21df sensor") ;
  } 
  LOG(LL_DEBUG, ("htu21df readings: h %.3f", h));

  return h;
}

}  // namespace shelly
