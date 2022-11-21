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

#include "shelly_cct_controller.hpp"
#include "shelly_hap_input.hpp"
#include "shelly_hap_light_bulb.hpp"
#include "shelly_input_pin.hpp"
#include "shelly_light_bulb_controller.hpp"
#include "shelly_temp_sensor_ow.hpp"
#include "shelly_temp_sensor_ntc.hpp"
#include "shelly_fan_pwm.hpp"
#include "shelly_main.hpp"

namespace shelly {

static std::unique_ptr<Onewire> s_onewire;
static std::unique_ptr<Fan> s_fan;

void CreatePeripherals(UNUSED_ARG std::vector<std::unique_ptr<Input>> *inputs,
                       std::vector<std::unique_ptr<Output>> *outputs,
                       UNUSED_ARG std::vector<std::unique_ptr<PowerMeter>> *pms,
                      std::unique_ptr<TempSensor> *sys_temp) {
  outputs->emplace_back(new OutputPin(1, 12, 1));
  outputs->emplace_back(new OutputPin(2, 14, 1));

  std::vector<std::unique_ptr<TempSensor>> sensors;
  s_onewire.reset(new Onewire(4));
  sensors = s_onewire->DiscoverAll();
  if (sensors.empty()) {
    LOG(LL_ERROR, ("Could not create onewire temp sensor"));
    s_onewire.reset();
  } else {
    std::unique_ptr<TempSensor> sensor = std::move(sensors[0]);
    if(mgos_sys_config_get_t0_update_interval() > 0) {      
      sensor->StartUpdating(mgos_sys_config_get_t0_update_interval()*1000);
      Fan* fan = new Fan(mgos_sys_config_get_fan0_pwm_pin(), mgos_sys_config_get_fan0_rpm_pin());
      Status status = fan->Init();
      if(status.ok()) {
        fan->SetMinTemp(mgos_sys_config_get_shelly_overheat_off()-20);
        fan->SetMaxTemp(mgos_sys_config_get_shelly_overheat_off());
        TempSensor* s = sensor.get();
        sensor->SetNotifier( [fan, s] {
          StatusOr<float> temp = s->GetTemperature();
          if(temp.ok()) {
            fan->Adjust((int) temp.ValueOrDie());
          }
        });
        s_fan.reset(fan);
      } else {
        LOG(LL_ERROR, ("Fan init failed: %s", status.error_message().c_str()));
      }
    } else {
      LOG(LL_ERROR, ("No onewire temp sensor updating interval given."));
    }
    sys_temp->reset(sensor.release());
  }

  // 1.0V if used with WeMos or other dev boards, 3.3V otherwise
  //sys_temp->reset(new TempSensorSDNT1608X103F3950(0, 1.0f, 37400.0f));

}

void CreateComponents(std::vector<std::unique_ptr<Component>> *comps,
                      std::vector<std::unique_ptr<mgos::hap::Accessory>> *accs,
                      HAPAccessoryServerRef *svr) {
  std::unique_ptr<LightBulbControllerBase> lightbulb_controller;
  std::unique_ptr<hap::LightBulb> hap_light;
  auto *lb_cfg = (struct mgos_config_lb *) mgos_sys_config_get_lb1();

  lightbulb_controller.reset(
      new CCTController(lb_cfg, FindOutput(1), FindOutput(2)));

  hap_light.reset(new hap::LightBulb(
      1, nullptr, std::move(lightbulb_controller), lb_cfg, false));

  if (hap_light == nullptr || !hap_light->Init().ok()) {
    return;
  }

  mgos::hap::Accessory *pri_acc = accs->front().get();
  hap_light->set_primary(true);
  pri_acc->SetCategory(kHAPAccessoryCategory_Lighting);
  pri_acc->AddService(hap_light.get());

  comps->push_back(std::move(hap_light));
}
}  // namespace shelly