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
    LOG(LL_DEBUG,
        ("%.2f IAQ %.2f (acc %d) T %.2f RH %.2f P %.2f hPa VOC %.2f CO2 %.2f", ts,
         out->iaq.signal, out->iaq.accuracy, out->temp.signal, out->rh.signal,
         ps_hpa, out->voc.signal, out->co2.signal));
  } else {
    LOG(LL_DEBUG, ("%.2f T %.2f RH %.2f P %.2f hPa", ts,
                  out->temp.signal, out->rh.signal, ps_hpa));
  }
  sensor->out_ = *out;
  sensor->Notify();
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
  mgos_event_remove_handler(MGOS_EV_BME680_BSEC_OUTPUT, bme680_output_cb, this);
}

Status BME680Sensor::Init() {
  return Status::OK();
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

StatusOr<float> BME680Sensor::GetCO2Level() {
  if (out_.co2.time_stamp == 0) {
    return Status(-1, "Cannot read co2 level from bme680 sensor") ;
  } 
  float co2 = out_.co2.signal;
  LOG(LL_DEBUG, ("bme280 readings: co2 %.0f", co2));

  return co2;
}

StatusOr<float> BME680Sensor::GetIAQLevel() {
  if (out_.iaq.time_stamp == 0) {
    return Status(-1, "Cannot read iaq level from bme680 sensor") ;
  } 
  float iaq = out_.iaq.signal;
  LOG(LL_DEBUG, ("bme280 readings: iaq %.0f", iaq));

  return iaq;
}

StatusOr<float> BME680Sensor::GetVOCLevel() {
  if (out_.voc.time_stamp == 0) {
    return Status(-1, "Cannot read voc level from bme680 sensor") ;
  } 
  float voc = out_.voc.signal;
  LOG(LL_DEBUG, ("bme280 readings: voc %.0f", voc));

  return voc;
}

void BME680Sensor::Notify() {
  if(TempSensor::notifier_ != nullptr) {
    TempSensor::notifier_();
  }
  if(HumiditySensor::notifier_ != nullptr) {
    HumiditySensor::notifier_();
  }
  if(PressureSensor::notifier_ != nullptr) {
    PressureSensor::notifier_();
  }
  if(AirQualitySensor::notifier_ != nullptr) {
    AirQualitySensor::notifier_();
  }
  if(CO2Sensor::notifier_ != nullptr) {
    CO2Sensor::notifier_();
  }
}

}  // namespace shelly
