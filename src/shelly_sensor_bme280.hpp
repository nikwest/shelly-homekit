#pragma once

#ifdef HAVE_BME280

#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "shelly_pressure_sensor.hpp"
#include "mgos_bme280.h"

namespace shelly {

class BME280Sensor : public TempSensor, public HumiditySensor, public PressureSensor {
 public:
  BME280Sensor(int bus_num, uint8_t i2caddr);
  virtual ~BME280Sensor();

  StatusOr<float> GetTemperature() override;
  StatusOr<float> GetHumidity() override;
  StatusOr<float> GetPressure() override;

 protected:

 private:
  struct mgos_bme280 *bme280_;

};

}  // namespace shelly

#endif