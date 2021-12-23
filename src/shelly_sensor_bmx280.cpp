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

#ifdef HAVE_BMX280

#include "shelly_sensor_bmx280.hpp"

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_bme280.h"

#include <math.h>

namespace shelly {

BMP280Sensor::BMP280Sensor(int bus_num, uint8_t i2caddr) {
  if(bus_num != 0) {
    LOG(LL_ERROR, ("Creating sensor only suppored on i2c0 bus"));
    return;
  }

  bme280_ = mgos_bme280_i2c_create(i2caddr);

  if (!bme280_) {
    LOG(LL_ERROR, ("no bme280 sensor created."));
  } 
}

BMP280Sensor::~BMP280Sensor() {
}

StatusOr<float> BMP280Sensor::GetTemperature() {
  if (!bme280_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
    return Status(-1, "bme280 sensor not initialized") ;
  }
  double t = mgos_bme280_read_temperature(bme280_);
 
  if (t == NAN) {
    return Status(-1, "Cannot read bme280 sensor") ;
  } 
  LOG(LL_DEBUG, ("bme280 readings: t %.3f", t));

  return (float) t;
}

StatusOr<float> BMP280Sensor::GetPressure() {
  if (!bme280_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  double p = mgos_bme280_read_pressure(bme280_);
 
  if (p == NAN) {
    return Status(-1, "Cannot read bme280 sensor") ;
  } 
  LOG(LL_DEBUG, ("bme280 readings: p %.3f", p));

  return (float) p;
}

BME280Sensor::BME280Sensor(int bus_num, uint8_t i2caddr) : BMP280Sensor(bus_num, i2caddr) {
}

BME280Sensor::~BME280Sensor() {
} 

StatusOr<float> BME280Sensor::GetHumidity() {
  if (!bme280_) {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  double h = mgos_bme280_read_humidity(bme280_);
 
  if (h == NAN) {
    return Status(-1, "Cannot read bme280 sensor") ;
  } 
  LOG(LL_DEBUG, ("bme280 readings: h %.3f", h));

  return (float) h;
}

}  // namespace shelly

#endif