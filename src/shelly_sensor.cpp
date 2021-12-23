
#include "shelly_sensor.hpp"

#include "mgos.hpp"
#include "mgos_hap_accessory.hpp"
#include "mgos_hap_chars.hpp"

#include "shelly_main.hpp"
#include "shelly_sensor_bmx280.hpp"
#include "shelly_sensor_sht3x.hpp"

namespace shelly {

ShellySensor::ShellySensor(int id, struct mgos_config_sensor *cfg)
    : Component(id),
      cfg_(cfg) {
}

ShellySensor::~ShellySensor() {
  temp_.release();
  humidity_.release();
  pressure_.release();
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
      "{id: %d, type: %d, name: %Q",
      id(), type(), (cfg_->name ? cfg_->name : ""));
  
  res.append("}");
  return res;
}

Status ShellySensor::SetConfig(const std::string &config_json,
                               bool *restart_required) {
  struct mgos_config_sensor cfg = *cfg_;
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
  
  // Now copy over.
  if (cfg_->name != nullptr && strcmp(cfg_->name, cfg.name) != 0) {
    mgos_conf_set_str(&cfg_->name, cfg.name);
    *restart_required = true;
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
      temp_.reset(sensor);
      humidity_.reset(nullptr);
      pressure_.reset(sensor);
    }
    #else 
    //  #warning "HAVE_BMX280 not enabled"
    #endif
    break;
  case kSHT3X:
    #ifdef HAVE_SHT3X
    {
      auto* sensor(new SHT3xSensor(cfg_->i2c_bus,  cfg_->i2c_addr));
      temp_.reset(sensor);
      humidity_.reset(sensor);
      pressure_.reset(nullptr);
    }
    #else 
    //  #warning "HAVE_SHT3X not enabled"
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
