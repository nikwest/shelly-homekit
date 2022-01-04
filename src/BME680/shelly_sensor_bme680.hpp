#pragma once


#include "shelly_sensor.hpp"
#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "shelly_pressure_sensor.hpp"
#include "mgos_bme680.h"

namespace shelly {

class BME680Sensor : public TempSensor, public PressureSensor, public HumiditySensor {
 public:
  BME680Sensor();
  virtual ~BME680Sensor();

  StatusOr<float> GetTemperature() override;
  StatusOr<float> GetPressure() override;
  StatusOr<float> GetHumidity() override;

  struct mgos_bsec_output out_;
};

}  // namespace shelly
