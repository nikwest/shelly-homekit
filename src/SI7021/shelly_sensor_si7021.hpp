#pragma once


#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "shelly_pressure_sensor.hpp"
#include "mgos_si7021.h"

namespace shelly {

class SI7021Sensor : public TempSensor, public HumiditySensor {
 public:
  SI7021Sensor(int bus_num, uint8_t i2caddr);
  virtual ~SI7021Sensor();

  StatusOr<float> GetTemperature() override;
  StatusOr<float> GetHumidity() override;

 protected:

 private:
  struct mgos_si7021 *si7021_;

};

}  // namespace shelly
