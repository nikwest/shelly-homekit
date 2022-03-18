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

#include "shelly_co2_sensor.hpp"

namespace shelly {


#if MGOS_HAVE_PROMETHEUS_METRICS
#include "mgos_prometheus_metrics.h"

static void metrics_shelly_co2(struct mg_connection *nc, void *user_data) {
  CO2Sensor* sensor = (CO2Sensor*) user_data;

  const auto &co2 = sensor->GetCO2Level();
  if(co2.ok()) {
    mgos_prometheus_metrics_printf(
        nc, GAUGE, "shelly_co2", "Carbon dioxide contenctration in (ppm)",
        "%.0f", co2.ValueOrDie());

  }
  (void) user_data;
}
#endif // MGOS_HAVE_PROMETHEUS_METRICS


CO2Sensor::CO2Sensor() {
  #if MGOS_HAVE_PROMETHEUS_METRICS
    mgos_prometheus_metrics_add_handler(metrics_shelly_co2, this);
  #endif
}

CO2Sensor::~CO2Sensor() {
}

void CO2Sensor::SetNotifier(Notifier notifier) {
  notifier_ = notifier;
}

}  // namespace shelly
