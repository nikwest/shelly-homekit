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
#include "shelly_input_pin.hpp"
//#include "shelly_sys_led_btn.hpp"
//#include "shelly_sensor_sht3x.hpp"
//#include "shelly_sensor_htu21df.hpp"
#ifdef HAVE_BMX280
#include "shelly_sensor_bmx280.hpp"
#endif

#include "shelly_main.hpp"
#include "custom16_pcf857x_output.hpp"
#include "custom16_pcf857x_input.hpp"

#include "mgos_pcf857x.h"
#include "shelly_temp_sensor_ow.hpp"

#ifndef MAX_TS_NUM
#define MAX_TS_NUM 1
#endif
namespace shelly {

static bool create_failed = false;
static std::unique_ptr<Onewire> s_onewire;

void CreatePeripherals(std::vector<std::unique_ptr<Input>> *inputs,
                       std::vector<std::unique_ptr<Output>> *outputs,
                       std::vector<std::unique_ptr<PowerMeter>> *pms,
                       std::unique_ptr<TempSensor> *sys_temp) {

//  sys_temp->reset(new HTU21DFSensor(0, 0x40));
#ifdef HAVE_BMX280
  sys_temp->reset(new BME280Sensor(1, 0x76));
#endif

#ifdef PCF8575
  struct mgos_pcf857x *dout, *din;

  if (!(dout = mgos_pcf8575_create(mgos_i2c_get_global(), 0x20, -1))) {
    LOG(LL_ERROR, ("Could not create ouptput PCF857X at 0x20"));
    create_failed = true;
  }
  if (!(din = mgos_pcf8575_create(mgos_i2c_get_global(), 0x21, 14))) {
    LOG(LL_ERROR, ("Could not create input PCF857X at 0x21"));
    create_failed = true;
  }

  for(int i = 0; i<16; i++) {
    outputs->emplace_back(new custom16::OutputPCF857xPin(i+1, dout, i, 1));
    auto *in = new custom16::InputPCF857xPin(i+1, din, i, 1, MGOS_GPIO_PULL_UP, (i==0));
    if(i==0) {
      in->AddHandler(std::bind(&HandleInputResetSequence, in, 21, _1, _2));
    }
    in->Init();
    inputs->emplace_back(in);
  }
#endif
#ifdef PCF8574
  struct mgos_pcf857x *dout0, *dout1, *din0, *din1;

  if (!(dout0 = mgos_pcf8574_create(mgos_i2c_get_global(), 0x39, -1))) {
    LOG(LL_ERROR, ("Could not create ouptput PCF857X at 0x39"));
    create_failed = true;
  }
  if (!(dout1 = mgos_pcf8574_create(mgos_i2c_get_global(), 0x38, -1))) {
    LOG(LL_ERROR, ("Could not create ouptput PCF857X at 0x38"));
    create_failed = true;
  }
  if (!(din0 = mgos_pcf8574_create(mgos_i2c_get_global(), 0x3e, 17))) {
    LOG(LL_ERROR, ("Could not create input PCF857X at 0x3e"));
    create_failed = true;
  }
  if (!(din1 = mgos_pcf8574_create(mgos_i2c_get_global(), 0x3f, 5))) {
    LOG(LL_ERROR, ("Could not create input PCF857X at 0x3f"));
    create_failed = true;
  }

  for(int i = 0; i<8; i++) {
    outputs->emplace_back(new custom16::OutputPCF857xPin(i+1, dout0, i, 1));
    auto *in = new custom16::InputPCF857xPin(i+1, din0, i, 1, MGOS_GPIO_PULL_UP, (i==0));
    if(i==0) {
      in->AddHandler(std::bind(&HandleInputResetSequence, in, 21, _1, _2));
    }
    in->Init();
    inputs->emplace_back(in);
  }
  for(int i = 0; i<8; i++) {
    outputs->emplace_back(new custom16::OutputPCF857xPin(i+9, dout1, i, 1));
    auto *in = new custom16::InputPCF857xPin(i+9, din1, i, 1, MGOS_GPIO_PULL_UP, (i==0));
    in->Init();
    inputs->emplace_back(in);
  }
#endif
//  InitSysLED(LED_GPIO, LED_ON);

  s_onewire.reset(new Onewire(27));
  if (s_onewire->DiscoverAll().empty()) {
    s_onewire.reset();
  }
  (void) pms;
}

void CreateComponents(std::vector<std::unique_ptr<Component>> *comps,
                      std::vector<std::unique_ptr<mgos::hap::Accessory>> *accs,
                      HAPAccessoryServerRef *svr) {
  
  // if(create_failed) {
  //   return;
  // }

  CreateHAPSwitch(1, mgos_sys_config_get_sw1(), mgos_sys_config_get_in1(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(2, mgos_sys_config_get_sw2(), mgos_sys_config_get_in2(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(3, mgos_sys_config_get_sw3(), mgos_sys_config_get_in3(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(4, mgos_sys_config_get_sw4(), mgos_sys_config_get_in4(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(5, mgos_sys_config_get_sw5(), mgos_sys_config_get_in5(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(6, mgos_sys_config_get_sw6(), mgos_sys_config_get_in6(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(7, mgos_sys_config_get_sw7(), mgos_sys_config_get_in7(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(8, mgos_sys_config_get_sw8(), mgos_sys_config_get_in8(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(9, mgos_sys_config_get_sw9(), mgos_sys_config_get_in9(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(10, mgos_sys_config_get_sw10(), mgos_sys_config_get_in10(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(11, mgos_sys_config_get_sw11(), mgos_sys_config_get_in11(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(12, mgos_sys_config_get_sw12(), mgos_sys_config_get_in12(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(13, mgos_sys_config_get_sw13(), mgos_sys_config_get_in13(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(14, mgos_sys_config_get_sw14(), mgos_sys_config_get_in14(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(15, mgos_sys_config_get_sw15(), mgos_sys_config_get_in15(),
                  comps, accs, svr, false /* to_pri_acc */);
  CreateHAPSwitch(16, mgos_sys_config_get_sw16(), mgos_sys_config_get_in16(),
                  comps, accs, svr, false /* to_pri_acc */);
  
    // Sensor Discovery
  std::vector<std::unique_ptr<TempSensor>> sensors;
  if (s_onewire != nullptr) {
    sensors = s_onewire->DiscoverAll();
  }

  if (!sensors.empty()) {
    struct mgos_config_ts *ts_cfgs[MAX_TS_NUM] = {
        (struct mgos_config_ts *) mgos_sys_config_get_ts1(),
    };

    for (size_t i = 0; i < std::min((size_t) MAX_TS_NUM, sensors.size()); i++) {
      auto *ts_cfg = ts_cfgs[i];
      CreateHAPTemperatureSensor(i + 1, std::move(sensors[i]), ts_cfg, comps,
                                 accs, svr);
    }
  }
}

}  // namespace shelly
