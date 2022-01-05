
#pragma once

#include "mgos_sys_config.h"

#include "shelly_common.hpp"
#include "shelly_component.hpp"
#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "shelly_pressure_sensor.hpp"
#include "shelly_co2_sensor.hpp"
#include "shelly_airquality_sensor.hpp"

namespace shelly {

class ShellySensor : public Component {
 public:
 enum {
    kBMP280 = 0,
    kBME280 = 1,
    kHTU21 = 2,
    kSHT3X = 3,
    kSI7021 = 4,
    kBME680 = 5,
    kMax,
  } model;

  ShellySensor(int id, struct mgos_config_se *cfg);
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
   struct mgos_config_se *cfg_;
   TempSensor* temp_;
   PressureSensor* pressure_;
   HumiditySensor* humidity_; 
   CO2Sensor* co2_; 
   AirQualitySensor* air_; 
};

}  // namespace shelly
