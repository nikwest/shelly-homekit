

#include "shelly_hap_sensor.hpp"

#include "mgos.hpp"
#include "mgos_hap_service.hpp"
#include "mgos_hap_chars.hpp"

#include <math.h>

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

Sensor::Sensor(int id, struct mgos_config_se *cfg)
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
    s->AddNameChar(iid++, "Temperature");
    auto* c = new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristicType_CurrentTemperature, 0.0F, 100.0F, 0.1F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto temperature = temp_->GetTemperature();
        if (!temperature.ok()) return kHAPError_Busy;
        *value = temperature.ValueOrDie();
        LOG(LL_DEBUG, ("Reading temp: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_CurrentTemperature);
    s->AddChar(c);
    temp_->notify_= [c]{
      if(c != nullptr) {
        c->RaiseEvent();
      } 
    };
    acc->get()->AddService(s);
  }
  if(humidity_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_HumiditySensor, kHAPServiceDebugDescription_HumiditySensor)
    );
    // Name
    s->AddNameChar(iid++, "Humidity");
    auto *c = new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristicType_CurrentRelativeHumidity, 0.0F, 100.0F, 0.1F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto humidity = humidity_->GetHumidity();
        if (!humidity.ok()) return kHAPError_Busy;
        *value = humidity.ValueOrDie();
        LOG(LL_DEBUG, ("Reading humidity: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_CurrentRelativeHumidity);
    s->AddChar(c);
    humidity_->notify_= [c]{
      if(c != nullptr) {
        c->RaiseEvent();
      } 
    };
    acc->get()->AddService(s);
  }
 
if(pressure_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_EveAtmosphericPressureSensor, "eve-atmospheric-pressure")
    );
    // Name
    s->AddNameChar(iid++, "Pressure");
    auto* c = new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristic_EveCurrentAtmosphericPressure, 0.0F, 200000.0F, 0.1F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto pressure = pressure_->GetPressure();
        if (!pressure.ok()) return kHAPError_Busy;
        *value = pressure.ValueOrDie();
        LOG(LL_DEBUG, ("Reading pressure: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      "eve-atmospheric-pressure");
    s->AddChar(c);
    pressure_->notify_= [c]{
      if(c != nullptr) {
        c->RaiseEvent();
      } 
    };
    acc->get()->AddService(s);
  }
 
  if(air_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_AirQualitySensor, kHAPServiceDebugDescription_AirQualitySensor)
    );
    // Name
    s->AddNameChar(iid++, "Air Quality");
    auto* c = new mgos::hap::UInt8Characteristic(
      iid++, &kHAPCharacteristicType_AirQuality, 0, 5, 1,
      [this](HAPAccessoryServerRef *, const HAPUInt8CharacteristicReadRequest *,
          uint8_t *value) {
        auto iaq = air_->GetIAQLevel();
        if (!iaq.ok()) return kHAPError_Busy;
        *value = (int) roundf(iaq.ValueOrDie() / 100);
        LOG(LL_DEBUG, ("Reading iaq: %d", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_AirQuality);
    s->AddChar(c);
    auto* c2 = new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristicType_VOCDensity, 0.0F, 1000.0F, 0.1F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto voc = air_->GetVOCLevel();
        if (!voc.ok()) return kHAPError_Busy;
        *value = voc.ValueOrDie();
        LOG(LL_DEBUG, ("Reading voc: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_VOCDensity);
    s->AddChar(c2);
    air_->notify_= [c, c2]{
      if(c != nullptr) {
        c->RaiseEvent();
      } 
      if(c2 != nullptr) {
        c2->RaiseEvent();
      } 
    };

    if(co2_) {
      auto* c = new mgos::hap::FloatCharacteristic(
        iid++, &kHAPCharacteristicType_CarbonDioxideLevel, 0.0F, 10000.0F, 1.0F,
        [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
            float *value) {
          auto co2 = co2_->GetCO2Level();
          if (!co2.ok()) return kHAPError_Busy;
          *value = co2.ValueOrDie();
          LOG(LL_DEBUG, ("Reading co2: %f", *value));
          return kHAPError_None;
        },
        true /* supports_notification */, nullptr /* write_handler */,
        kHAPCharacteristicDebugDescription_CarbonDioxideLevel);
      s->AddChar(c);
    }
        co2_->notify_= [c]{
      if(c != nullptr) {
        c->RaiseEvent();
      } 
    };
    acc->get()->AddService(s);
  } else if(co2_) {
    auto* s(
      new mgos::hap::Service(iid++, &kHAPServiceType_CarbonDioxideSensor, kHAPServiceDebugDescription_CarbonDioxideSensor)
    );
    // Name
    s->AddNameChar(iid++, "CO2");
    s->AddChar(new mgos::hap::UInt8Characteristic(
      iid++, &kHAPCharacteristicType_CarbonDioxideDetected, 0, 1, 1,
      [this](HAPAccessoryServerRef *, const HAPUInt8CharacteristicReadRequest *,
          uint8_t *value) {
        auto co2 = co2_->GetCO2Level();
        if (!co2.ok()) return kHAPError_Busy;
        *value = (co2.ValueOrDie() > mgos_sys_config_get_co2_detected_level()) ? 1 : 0;
        LOG(LL_DEBUG, ("Reading co2 detected: %d", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_CarbonDioxideDetected)
    );

    s->AddChar(new mgos::hap::FloatCharacteristic(
      iid++, &kHAPCharacteristicType_CarbonDioxideLevel, 0.0F, 10000.0F, 1.0F,
      [this](HAPAccessoryServerRef *, const HAPFloatCharacteristicReadRequest *,
          float *value) {
        auto co2 = co2_->GetCO2Level();
        if (!co2.ok()) return kHAPError_Busy;
        *value = co2.ValueOrDie();
        LOG(LL_DEBUG, ("Reading co2: %f", *value));
        return kHAPError_None;
      },
      true /* supports_notification */, nullptr /* write_handler */,
      kHAPCharacteristicDebugDescription_CarbonDioxideLevel)
    );

    acc->get()->AddService(s);
  }

  return Status::OK();
}

}  // namespace hap
}  // namespace shelly
