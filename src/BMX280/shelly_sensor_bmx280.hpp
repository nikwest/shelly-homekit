#pragma once


#include "shelly_sensor.hpp"
#include "shelly_temp_sensor.hpp"
#include "shelly_humidity_sensor.hpp"
#include "shelly_pressure_sensor.hpp"
#include "mgos_bme280.h"

namespace shelly {

class BMP280Sensor : public TempSensor, public PressureSensor {
 public:
  BMP280Sensor(int bus_num, uint8_t i2caddr);
  virtual ~BMP280Sensor();
  Status Init() override;

  StatusOr<float> GetTemperature() override;
  StatusOr<float> GetPressure() override;

  protected:
    struct mgos_bme280 *bme280_;

};

class BME280Sensor : public HumiditySensor, public BMP280Sensor {
 public:
  BME280Sensor(int bus_num, uint8_t i2caddr);
  virtual ~BME280Sensor();

  StatusOr<float> GetHumidity() override;
};

}  // namespace shelly
