

#include "shelly_hap_sensor.hpp"

#include "mgos.hpp"
#include "mgos_hap_service.hpp"
#include "mgos_hap_chars.hpp"

namespace shelly {

namespace hap {

// Eve Service type for atmospheric pressure sensor.
// E863F00A-079E-48FF-8F27-9C2605A29F52
const HAPUUID kHAPServiceType_EveAtmosphericPressureSensor = {
   0x52, 0x9F, 0xA2, 0x05, 0x26, 0x9C, 0x27, 0x8F,
    0xFF, 0x48, 0x9E, 0x07, 0x0A, 0xF0, 0x63, 0xE8,
};

// Eve current atmospheric pressure. Float value in hPa.
// E863F10F-079E-48FF-8F27-9C2605A29F52
const HAPUUID kHAPCharacteristic_EveCurrentAtmosphericPressure = {
    0x52, 0x9F, 0xA2, 0x05, 0x26, 0x9C, 0x27, 0x8F,
    0xFF, 0x48, 0x9E, 0x07, 0x0F, 0xF1, 0x63, 0xE8,
};

Sensor::Sensor(int id, struct mgos_config_sensor *cfg)
    : ShellySensor(id, cfg) {
}

Sensor::~Sensor() {
}

Status Sensor::Init(std::unique_ptr<mgos::hap::Accessory> *acc) {
  auto sensor = ShellySensor::Init();
  if (!sensor.ok()) return sensor;

  const int id1 = id() - 1;  // IDs used to start at 0, preserve compat.
  uint16_t iid =
      SHELLY_HAP_IID_BASE_SENSOR + (SHELLY_HAP_IID_STEP_SENSOR * id1);
  if(temp_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_TemperatureSensor, kHAPServiceDebugDescription_TemperatureSensor)
    );
    // Name
    s->AddNameChar(iid++, cfg_->name);
    s->AddChar(new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristicType_CurrentTemperature, 0.0, 100.0, 0.1,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto temperature = temp_->GetTemperature();
        if (!temperature.ok()) return kHAPError_Busy;
        *value = temperature.ValueOrDie();
        LOG(LL_ERROR, ("Reading temp: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_CurrentTemperature));

    acc->get()->AddService(s);
  }
  if(humidity_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_HumiditySensor, kHAPCharacteristicDebugDescription_CurrentRelativeHumidity)
    );
    // Name
    s->AddNameChar(iid++, cfg_->name);
    s->AddChar(new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristicType_CurrentRelativeHumidity, 0.0F, 100.0F, 0.1F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto humidity = humidity_->GetHumidity();
        if (!humidity.ok()) return kHAPError_Busy;
        *value = humidity.ValueOrDie();
        LOG(LL_ERROR, ("Reading humidity: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_CurrentRelativeHumidity));

    acc->get()->AddService(s);
  }
 
if(pressure_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_EveAtmosphericPressureSensor, "eve-atmospheric-pressure")
    );
    // Name
    s->AddNameChar(iid++, cfg_->name);
    s->AddChar(new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristic_EveCurrentAtmosphericPressure, 0.0F, 2000.0F, 0.1F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto pressure = pressure_->GetPressure();
        if (!pressure.ok()) return kHAPError_Busy;
        *value = pressure.ValueOrDie();
        LOG(LL_ERROR, ("Reading pressure: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      "eve-atmospheric-pressure"));

    acc->get()->AddService(s);
  }

  return Status::OK();
}

// Status Sensor::AddToAcc(mgos::hap::Accessory *acc) {
//   for (auto &s : services_) {
//     LOG(LL_INFO, ("Adding service iid: %d", s->iid()));
//     acc->AddService(s);
//   }
//   return Status::OK();
// }

}  // namespace hap
}  // namespace shelly
