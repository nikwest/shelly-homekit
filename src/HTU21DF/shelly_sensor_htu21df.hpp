#pragma once

#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "shelly_pressure_sensor.hpp"
#include "mgos_htu21df.h"

namespace shelly {

class HTU21DFSensor : public TempSensor, public HumiditySensor {
 public:
  HTU21DFSensor(int bus_num, uint8_t i2caddr);
  virtual ~HTU21DFSensor();

  StatusOr<float> GetTemperature() override;
  StatusOr<float> GetHumidity() override;

 protected:

 private:
  struct mgos_htu21df *htu21df_;

};

}  // namespace shelly
