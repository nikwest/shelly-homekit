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

#ifdef HAVE_SHT3X

#include "shelly_sensor_sht3x.hpp"

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_sht31.h"

#include <math.h>

namespace shelly {

SHT3xSensor::SHT3xSensor() {
  struct mgos_i2c *i2c = mgos_i2c_get_global();
  uint8_t i2caddr = 69;

  LOG(LL_ERROR, ("Creating sensor"));

  if (!i2c) {
    LOG(LL_ERROR, ("I2C bus missing, set i2c.enable=true in mos.yml"));
    return;
  } 

  sht31_ = mgos_sht31_create(i2c, i2caddr);
  if (!sht31_) {
    LOG(LL_ERROR, ("no sht31 sensor created."));
  } 
}

SHT3xSensor::~SHT3xSensor() {
}

StatusOr<float> SHT3xSensor::GetTemperature() {
  if (!sht31_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  float t = mgos_sht31_getTemperature(sht31_);
 
  if (t == NAN) {
    return Status(-1, "Cannot read sht3x sensor") ;
  } 
  LOG(LL_DEBUG, ("SHT3x readings: t %.3f", t));

  return t;
}

StatusOr<float> SHT3xSensor::GetHumidity() {
  if (!sht31_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  float h = mgos_sht31_getHumidity(sht31_);
 
  if (h == NAN) {
    return Status(-1, "Cannot read sht3x sensor") ;
  } 
  LOG(LL_DEBUG, ("SHT3x readings: h %.2f", h));

  return h;
}

}  // namespace shelly

#endif