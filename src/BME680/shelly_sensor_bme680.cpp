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

#include "shelly_sensor_bme680.hpp"

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_bme680.h"

#include <math.h>

namespace shelly {

static void bme680_output_cb(int ev, void *ev_data, void *arg) {
  const struct mgos_bsec_output *out = (struct mgos_bsec_output *) ev_data;
  auto *sensor = static_cast<BME680Sensor*>(arg);
  double ts = out->temp.time_stamp / 1000000000.0;
  float ps_hpa = out->ps.signal / 100.0f;
  if (out->iaq.time_stamp > 0) {
    LOG(LL_INFO,
        ("%.2f IAQ %.2f (acc %d) T %.2f RH %.2f P %.2f hPa VOC %.2f CO2 %.2f", ts,
         out->iaq.signal, out->iaq.accuracy, out->temp.signal, out->rh.signal,
         ps_hpa, out->voc.signal, out->co2.signal));
  } else {
    LOG(LL_INFO, ("%.2f T %.2f RH %.2f P %.2f hPa", ts,
                  out->temp.signal, out->rh.signal, ps_hpa));
  }
  sensor->out_ = *out;
  (void) ev;
}

BME680Sensor::BME680Sensor() {
  if(!mgos_sys_config_get_bme680_enable()) {
    LOG(LL_WARN, ("BME680 disabled. Set bme680.enable to true in mos.yml"));
    return;
  }
  mgos_event_add_handler(MGOS_EV_BME680_BSEC_OUTPUT, bme680_output_cb, this);
}

BME680Sensor::~BME680Sensor() {
}

StatusOr<float> BME680Sensor::GetTemperature() {
  if (out_.temp.time_stamp == 0) {
    return Status(-1, "Cannot read temperature from bme680 sensor") ;
  } 
  float t = out_.temp.signal;
  LOG(LL_DEBUG, ("bme280 readings: t %.3f", t));

  return t;
}

StatusOr<float> BME680Sensor::GetPressure() {
  if (out_.ps.time_stamp == 0) {
    return Status(-1, "Cannot read pressure from bme680 sensor") ;
  } 
  float p = out_.ps.signal / 100.0f;
  LOG(LL_DEBUG, ("bme280 readings: p %.3f", p));

  return p;
}

StatusOr<float> BME680Sensor::GetHumidity() {
 if (out_.rh.time_stamp == 0) {
    return Status(-1, "Cannot read humidity from bme680 sensor") ;
  } 
  float rh = out_.rh.signal;
  LOG(LL_DEBUG, ("bme280 readings: rh %.3f", rh));

  return rh;
}

}  // namespace shelly
