
#pragma once

#include "mgos_sys_config.h"

#include "mgos_hap_service.hpp"
#include "shelly_common.hpp"
#include "shelly_component.hpp"


namespace shelly {

class ShellySensor : public Component, public mgos::hap::Service {
 public:
  ShellySensor(int id, struct mgos_config_sensor *cfg);
  virtual ~ShellySensor();

  // Component interface impl.
  Type type() const override;
  std::string name() const override;
  virtual Status Init() override;
  StatusOr<std::string> GetInfo() const override;
  StatusOr<std::string> GetInfoJSON() const override;
  Status SetConfig(const std::string &config_json,
                   bool *restart_required) override;
  Status SetState(const std::string &state_json) override;
  bool IsIdle() override;

 protected:
   struct mgos_config_sensor *cfg_;

};

}  // namespace shelly
