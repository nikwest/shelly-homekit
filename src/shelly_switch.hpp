/*
 * Copyright (c) 2020 Deomid "rojer" Ryabkov
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

#pragma once

#include <memory>
#include <vector>

#include "mgos_sys_config.h"
#include "mgos_timers.hpp"

#include "shelly_common.hpp"
#include "shelly_component.hpp"
#include "shelly_hap_chars.hpp"
#include "shelly_hap_service.hpp"
#include "shelly_input.hpp"
#include "shelly_output.hpp"
#include "shelly_pm.hpp"

namespace shelly {

// Common base for Switch, Outlet and Lock services.
class ShellySwitch : public Component, public hap::Service {
 public:
  enum class InMode {
    kMomentary = 0,
    kToggle = 1,
    kEdge = 2,
    kDetached = 3,
  };

  enum class InitialState {
    kOff = 0,
    kOn = 1,
    kLast = 2,
    kInput = 3,
  };

  ShellySwitch(int id, Input *in, Output *out, PowerMeter *out_pm,
               struct mgos_config_sw *cfg);
  virtual ~ShellySwitch();

  // Component interface impl.
  Type type() const override;
  virtual Status Init() override;
  StatusOr<std::string> GetInfo() const override;
  StatusOr<std::string> GetInfoJSON() const override;
  Status SetConfig(const std::string &config_json,
                   bool *restart_required) override;

  void SetState(bool new_state, const char *source);

 protected:
  void InputEventHandler(Input::Event ev, bool state);

  void AutoOffTimerCB();

  void SaveState();

  Input *const in_;
  Output *const out_;
  PowerMeter *const out_pm_;
  struct mgos_config_sw *cfg_;

  Input::HandlerID handler_id_ = Input::kInvalidHandlerID;
  std::vector<hap::Characteristic *> state_notify_chars_;

  mgos::ScopedTimer auto_off_timer_;

  ShellySwitch(const ShellySwitch &other) = delete;
};

}  // namespace shelly
