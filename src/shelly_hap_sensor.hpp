

#pragma once

#include <memory>
#include <vector>

#include "mgos_hap.hpp"
#include "mgos_sys_config.h"
#include "mgos_timers.h"

#include "shelly_common.hpp"
#include "shelly_component.hpp"
#include "shelly_sensor.hpp"

namespace shelly {
namespace hap {

class Sensor : public ShellySensor {
 public:
  Sensor(int id, struct mgos_config_sensor *cfg);
  virtual ~Sensor();

  Status Init();

 protected:
  HAPError FloatStateCharRead(HAPAccessoryServerRef *,
                             const HAPFloatCharacteristicReadRequest *,
                             float *value);

  float temp_ = 0.0;
};

}  // namespace hap
}  // namespace shelly
