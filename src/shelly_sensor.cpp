
#include "shelly_sensor.hpp"

#include "mgos.hpp"
#include "mgos_hap_accessory.hpp"
#include "mgos_hap_chars.hpp"

#include "shelly_main.hpp"

#ifdef HAVE_BMX280
#include "shelly_sensor_bmx280.hpp"
#endif

#ifdef HAVE_HTU21DF
#include "shelly_sensor_htu21df.hpp"
#endif

#ifdef HAVE_SHT3X
#include "shelly_sensor_sht3x.hpp"
#endif

#ifdef HAVE_SI7021
#include "shelly_sensor_si7021.hpp"
#endif

#ifdef HAVE_BME680
#include "shelly_sensor_bme680.hpp"
#endif

namespace shelly {

ShellySensor::ShellySensor(int id, struct mgos_config_se *cfg)
    : Component(id),
      cfg_(cfg) {
}

ShellySensor::~ShellySensor() {
  if(temp_ != nullptr) {
    delete temp_;
  } else if(humidity_ != nullptr) {
    delete humidity_;
  } else if(pressure_ != nullptr) {
    delete pressure_;
  } else if(co2_ != nullptr) {
    delete co2_;
  } 
}

Component::Type ShellySensor::type() const {
  return Type::kSensor;
}

std::string ShellySensor::name() const {
  return cfg_->name;
}

StatusOr<std::string> ShellySensor::GetInfo() const {

  return mgos::SPrintf("sensor");
}

StatusOr<std::string> ShellySensor::GetInfoJSON() const {
  std::string res = mgos::JSONPrintStringf(
      "{id: %d, type: %d, model: %d, name: %Q",
      id(), type(), cfg_->model, (cfg_->name ? cfg_->name : ""));
  if(temp_) {
    auto temperature = temp_->GetTemperature();
    if (temperature.ok()) {
      std::string t = mgos::JSONPrintStringf(", tvalue: %.1f, tunit: %d", temperature.ValueOrDie(), cfg_->tunit);
      res.append(t);
    }
  }
  if(humidity_) {
    auto humidity = humidity_->GetHumidity();
    if (humidity.ok()) {
      std::string t = mgos::JSONPrintStringf(", hvalue: %.0f", humidity.ValueOrDie());
      res.append(t);
    }
  }
  if(pressure_) {
    auto pressure = pressure_->GetPressure();
    if (pressure.ok()) {
      std::string t = mgos::JSONPrintStringf(", pvalue: %.0f", pressure.ValueOrDie());
      res.append(t);
    }
  }
  if(co2_) {
    auto co2 = co2_->GetCO2Level();
    if (co2.ok()) {
      std::string t = mgos::JSONPrintStringf(", co2value: %.0f", co2.ValueOrDie());
      res.append(t);
    }
  }
  if(air_) {
    auto iaq = air_->GetIAQLevel();
    if (iaq.ok()) {
      std::string t = mgos::JSONPrintStringf(", iaqvalue: %.1f", iaq.ValueOrDie());
      res.append(t);
    }
    auto voc = air_->GetVOCLevel();
    if (voc.ok()) {
      std::string t = mgos::JSONPrintStringf(", vocvalue: %.1f", voc.ValueOrDie());
      res.append(t);
    }
  }

  res.append("}");
  return res;
}

Status ShellySensor::SetConfig(const std::string &config_json,
                               bool *restart_required) {
  struct mgos_config_se cfg = *cfg_;
  cfg.name = nullptr;
  json_scanf(
      config_json.c_str(), config_json.size(),
      "{name: %Q}",
      &cfg.name);
  mgos::ScopedCPtr name_owner((void *) cfg.name);
  // Validation.
  if (cfg.name != nullptr && strlen(cfg.name) > 64) {
    return mgos::Errorf(STATUS_INVALID_ARGUMENT, "invalid %s",
                        "name (too long, max 64)");
  }
  if (cfg.tunit < 0 || cfg.tunit > 1) {
     return mgos::Errorf(STATUS_INVALID_ARGUMENT, "invalid unit");
  }
  
  // Now copy over.
  if (cfg_->name != nullptr && strcmp(cfg_->name, cfg.name) != 0) {
    mgos_conf_set_str(&cfg_->name, cfg.name);
    *restart_required = true;
  }
  if (cfg_->tunit != cfg.tunit) {
    cfg_->tunit = cfg.tunit;
  }
  return Status::OK();
}

Status ShellySensor::SetState(const std::string &state_json) {
  // no state
  return Status::OK();
}

bool ShellySensor::IsIdle() {
  return true;
}

Status ShellySensor::Init() {
  if (!cfg_->enable) {
    LOG(LL_INFO, ("'%s' is disabled", cfg_->name));
    return Status::OK();
  }
  switch (cfg_->model) {
  case kBMP280:
    #ifdef HAVE_BMX280
    {
      auto* sensor(new BMP280Sensor(cfg_->i2c_bus,  cfg_->i2c_addr));
      temp_ = sensor;
      humidity_ = nullptr;
      pressure_ = sensor;
      co2_ = nullptr;
      air_ = nullptr;
    }
    #else 
    //  #warning "HAVE_BMX280 not enabled"
    #endif
    break;
  case kHTU21:
    #ifdef HAVE_HTU21DF
    {
      auto* sensor(new HTU21DFSensor(cfg_->i2c_bus,  cfg_->i2c_addr));
      temp_ = sensor;
      humidity_ = sensor;
      pressure_ = nullptr;
      co2_ = nullptr;
      air_ = nullptr;
   }
    #else 
    //  #warning "HAVE_HTU21DF not enabled"
    #endif
    break;  
  case kSHT3X:
    #ifdef HAVE_SHT3X
    {
      auto* sensor(new SHT3xSensor(cfg_->i2c_bus,  cfg_->i2c_addr));
      temp_ = sensor;
      humidity_ = sensor;
      pressure_= nullptr;
      co2_ = nullptr;
      air_ = nullptr;
    }
    #else 
    //  #warning "HAVE_SHT3X not enabled"
    #endif
    break;
  case kSI7021:
    #ifdef HAVE_SI7021
    {
      auto* sensor(new SI7021Sensor(cfg_->i2c_bus,  cfg_->i2c_addr));
      temp_ = sensor;
      humidity_ = sensor;
      pressure_ = nullptr;
      co2_ = nullptr;
      air_ = nullptr;
   }
    #else 
    //  #warning "HAVE_SI7021 not enabled"
    #endif
    break;  
  case kBME680:
    #ifdef HAVE_BME680
    if(mgos_sys_config_get_bme680_enable()) {
      auto *sensor(new BME680Sensor());
      temp_ = sensor;
      humidity_ = sensor;
      pressure_ = sensor;
      co2_ = sensor;
      air_ = sensor;
    } else {
      LOG(LL_WARN, ("bme680.enabled is set to false in mos.yml, cannot add sensor"));
    }
    #else 
    //  #warning "HAVE_BME680 not enabled"
    #endif
    break;  
  default:
    LOG(LL_INFO, ("Unsupported sensor model '%i'", cfg_->model));
    break;
  }
  
  LOG(LL_INFO, ("Exporting '%s': ", cfg_->name));
  return Status::OK();
}

}  // namespace shelly
