
#include "shelly_sensor.hpp"

#include "mgos.hpp"
#include "mgos_hap_accessory.hpp"
#include "mgos_hap_chars.hpp"

#include "shelly_main.hpp"

namespace shelly {

ShellySensor::ShellySensor(int id, struct mgos_config_sensor *cfg)
    : Component(id),
      cfg_(cfg) {
}

ShellySensor::~ShellySensor() {
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
  
  LOG(LL_INFO, ("Exporting '%s': ", cfg_->name));
  return Status::OK();
}

}  // namespace shelly
