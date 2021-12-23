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

#include "shelly_hap_garage_door_opener.hpp"
#include "shelly_hap_temperature_sensor.hpp"
#include "shelly_input_pin.hpp"
#include "shelly_main.hpp"
#include "shelly_sensor_sht3x.hpp"
#include "shelly_sensor_bmx280.hpp"
#include "shelly_sensor_htu21df.hpp"
//#include "shelly_sensor_si7021.hpp"

namespace shelly {

void CreatePeripherals(std::vector<std::unique_ptr<Input>> *inputs,
                       std::vector<std::unique_ptr<Output>> *outputs,
                       std::vector<std::unique_ptr<PowerMeter>> *pms,
                       std::unique_ptr<TempSensor> *sys_temp) {
  outputs->emplace_back(new OutputPin(1, 15, 1));
//  outputs->emplace_back(new OutputPin(2, 13, 1));
  // outputs->emplace_back(new OutputPin(3, 2, 1));
  // outputs->emplace_back(new OutputPin(4, 0, 1));
  auto *in1 = new InputPin(1, 12, 1, MGOS_GPIO_PULL_UP, true);
  in1->AddHandler(std::bind(&HandleInputResetSequence, in1, 4, _1, _2));
  in1->Init();
  inputs->emplace_back(in1);
  // auto *in2 = new InputPin(2, 14, 1, MGOS_GPIO_PULL_UP, false);
  // in2->Init();
  // inputs->emplace_back(in2);
  // auto *in3 = new InputPin(3, 1, 1, MGOS_GPIO_PULL_UP, false);
  // in3->Init();
  // inputs->emplace_back(in3);
  // auto *in4 = new InputPin(4, 3, 1, MGOS_GPIO_PULL_UP, false);
  // in4->Init();
  // inputs->emplace_back(in4);

// #ifdef HAVE_HTU21
//   sys_temp->reset(new HTU21DFSensor(0, 0x40));
// #endif
// #ifdef HAVE_BMX280
//   sys_temp->reset(new BME280Sensor(0, 0x76));
// #endif
// #ifdef HAVE_SHT3X
//   sys_temp->reset(new SHT3xSensor(0, 0x45));
// #endif
  (void) pms;
}

void CreateComponents(std::vector<std::unique_ptr<Component>> *comps,
                      std::vector<std::unique_ptr<mgos::hap::Accessory>> *accs,
                      HAPAccessoryServerRef *svr) {
  
  // CreateHAPSwitch(1, mgos_sys_config_get_sw1(), mgos_sys_config_get_in1(),
  //                 comps, accs, svr, false /* to_pri_acc */);
  // // CreateHAPSwitch(2, mgos_sys_config_get_sw2(), mgos_sys_config_get_in2(),
  //                 comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSensor(1, mgos_sys_config_get_sensor1(),
                comps, accs, svr, true /* to_pri_acc */);
  // CreateHAPSwitch(3, mgos_sys_config_get_sw3(), mgos_sys_config_get_in3(),
  //                 comps, accs, svr, false /* to_pri_acc */);
  // CreateHAPSwitch(4, mgos_sys_config_get_sw4(), mgos_sys_config_get_in4(),
  //                 comps, accs, svr, false /* to_pri_acc */);

}

}  // namespace shelly
