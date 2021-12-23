

#pragma once

#include <memory>
#include <vector>

#include "mgos_hap.hpp"
#include "mgos_sys_config.h"
#include "mgos_timers.h"
#include "mgos_hap_accessory.hpp"
#include "mgos_hap_service.hpp"

#include "shelly_common.hpp"
#include "shelly_component.hpp"
#include "shelly_sensor.hpp"

namespace shelly {
namespace hap {

class Sensor : public ShellySensor {
 public:
  Sensor(int id, struct mgos_config_sensor *cfg);
  virtual ~Sensor();

  Status Init(std::unique_ptr<mgos::hap::Accessory> *acc);

 // Status AddToAcc(mgos::hap::Accessory *acc);

 protected:
 //  std::vector<std::unique_ptr<mgos::hap::Service>> services_;

};

}  // namespace hap
}  // namespace shelly
