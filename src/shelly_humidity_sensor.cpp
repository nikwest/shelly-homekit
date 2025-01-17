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

#include "shelly_humidity_sensor.hpp"

namespace shelly {


#if MGOS_HAVE_PROMETHEUS_METRICS
#include "mgos_prometheus_metrics.h"

static void metrics_shelly_humidity(struct mg_connection *nc, void *user_data) {
  HumiditySensor* sensor = (HumiditySensor*) user_data;

  const auto &humidity = sensor->GetHumidity();
  if(humidity.ok()) {
    mgos_prometheus_metrics_printf(
        nc, GAUGE, "shelly_humidity", "Humidity in % (Percent)",
        "%.2f", humidity.ValueOrDie());

  }
  (void) user_data;
}
#endif // MGOS_HAVE_PROMETHEUS_METRICS


HumiditySensor::HumiditySensor() {
  #if MGOS_HAVE_PROMETHEUS_METRICS
    mgos_prometheus_metrics_add_handler(metrics_shelly_humidity, this);
  #endif
}

HumiditySensor::~HumiditySensor() {
}

void HumiditySensor::SetNotifier(Notifier notifier) {
  notifier_ = notifier;
}

}  // namespace shelly
