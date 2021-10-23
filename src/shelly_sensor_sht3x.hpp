#pragma once

#ifdef HAVE_SHT3X

#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "mgos_sht31.h"

namespace shelly {

class SHT3xSensor : public TempSensor, public HumiditySensor {
 public:
  SHT3xSensor(int bus_num, uint8_t i2caddr);
  virtual ~SHT3xSensor();

  StatusOr<float> GetTemperature() override;
  StatusOr<float> GetHumidity() override;

 protected:

 private:
  struct mgos_sht31 *sht31_;

};

}  // namespace shelly

#endif