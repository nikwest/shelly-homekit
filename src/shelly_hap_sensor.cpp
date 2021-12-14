

#include "shelly_hap_sensor.hpp"

#include "mgos.hpp"
#include "mgos_hap_chars.hpp"

namespace shelly {
namespace hap {

Sensor::Sensor(int id, struct mgos_config_sensor *cfg)
    : ShellySensor(id, cfg) {
}

Sensor::~Sensor() {
}

Status Sensor::Init() {
  auto sensor = ShellySensor::Init();
  if (!sensor.ok()) return sensor;

  const int id1 = id() - 1;  // IDs used to start at 0, preserve compat.
  uint16_t iid =
      SHELLY_HAP_IID_BASE_SENSOR + (SHELLY_HAP_IID_STEP_SENSOR * id1);
  svc_.iid = iid++;
  svc_.serviceType = &kHAPServiceType_TemperatureSensor;
  svc_.debugDescription = kHAPServiceDebugDescription_TemperatureSensor;
  // Name
  AddNameChar(iid++, cfg_->name);
  
  AddChar(new mgos::hap::FloatCharacteristic(
    svc_.iid + 2, &kHAPCharacteristicType_CurrentTemperature, 0.0, 100.0, 0.1,
    std::bind(&Sensor::FloatStateCharRead, this, _1, _2, _3),
    true /* supports_notification */, nullptr /* write_handler */,
    kHAPCharacteristicDebugDescription_CurrentTemperature));

  return Status::OK();
}

HAPError Sensor::FloatStateCharRead(HAPAccessoryServerRef *,
                             const HAPFloatCharacteristicReadRequest *,
                             float *value) {
  LOG(LL_ERROR, ("Reading temp: %f", temp_));
  //*value = temp_;
  return kHAPError_None;
}

}  // namespace hap
}  // namespace shelly
