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

#include <algorithm>

#include "shelly_hap_input.hpp"
#include "shelly_hap_stateless_switch.hpp"
#include "shelly_hap_temperature_sensor.hpp"
#include "shelly_input_pin.hpp"
#include "shelly_main.hpp"
#include "shelly_temp_sensor_ow.hpp"
#include "shelly_temp_sensor_ntc.hpp"

#ifndef MAX_TS_NUM
#define MAX_TS_NUM 3
#endif

namespace shelly {

static std::unique_ptr<Onewire> s_onewire;

void CreatePeripherals(std::vector<std::unique_ptr<Input>> *inputs,
                       std::vector<std::unique_ptr<Output>> *outputs,
                       std::vector<std::unique_ptr<PowerMeter>> *pms,
                       std::unique_ptr<TempSensor> *sys_temp) {

  outputs->emplace_back(new OutputPin(1, 12, 1));
  auto *in = new InputPin(1, 14, 1, MGOS_GPIO_PULL_NONE, true);
  in->AddHandler(std::bind(&HandleInputResetSequence, in, 12, _1, _2));
  in->Init();
  inputs->emplace_back(in);

  s_onewire.reset(new Onewire(13));
  // if (s_onewire->DiscoverAll().empty()) {
  //   s_onewire.reset();
  // }

  //sys_temp->reset(new TempSensorSDNT1608X103F3950(0, 1.0f, 37400.0f));

  (void) pms;
}

void CreateComponents(std::vector<std::unique_ptr<Component>> *comps,
                      std::vector<std::unique_ptr<mgos::hap::Accessory>> *accs,
                      HAPAccessoryServerRef *svr) {
  
  // Sensor Discovery
  std::vector<std::unique_ptr<TempSensor>> sensors;
  if (s_onewire != nullptr) {
    sensors = s_onewire->DiscoverAll();
  }

  // Single switch with non-detached input and no sensors = only one accessory.
  bool to_pri_acc = (sensors.empty() && (mgos_sys_config_get_sw1_in_mode() !=
                                         (int) InMode::kDetached));
  CreateHAPSwitch(1, mgos_sys_config_get_sw1(), mgos_sys_config_get_in1(),
                  comps, accs, svr, to_pri_acc);

  if (!sensors.empty()) {
    struct mgos_config_ts *ts_cfgs[MAX_TS_NUM] = {
        (struct mgos_config_ts *) mgos_sys_config_get_ts1(),
        (struct mgos_config_ts *) mgos_sys_config_get_ts2(),
//        (struct mgos_config_ts *) mgos_sys_config_get_ts3(),
    };

    for (size_t i = 0; i < std::min((size_t) MAX_TS_NUM, sensors.size()); i++) {
      auto *ts_cfg = ts_cfgs[i];
      CreateHAPTemperatureSensor(i + 1, std::move(sensors[i]), ts_cfg, comps, accs, svr);
    }
  }         
}

}  // namespace shelly
