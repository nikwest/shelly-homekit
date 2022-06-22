/*
 * Copyright (c) Shelly-HomeKit Contributors
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "shelly_fan_pwm.hpp"

#include "mgos.h"
#include "mgos_pwm.h"
#include "mgos_iram.h"
#include <math.h>

#define FAN_PWM_FREQ 400
#define TIMER_INTERVAL 1000

namespace shelly {

#if MGOS_HAVE_PROMETHEUS_METRICS
#include "mgos_prometheus_metrics.h"

static void metrics_shelly_fan(struct mg_connection *nc, void *user_data) {
    Fan* fan = (Fan*) user_data;
    StatusOr<int> st = fan->GetRPM();
    if(st.ok()) {
      mgos_prometheus_metrics_printf(
        nc, GAUGE, "fan_rpm", "Fan rpm",
        "%d", st.ValueOrDie());
    }
}
#endif // MGOS_HAVE_PROMETHEUS_METRICS

  Fan::Fan(int pwm_pin, int rpm_pin) : 
    pwm_pin_(pwm_pin),
    rpm_pin_(rpm_pin),
    update_timer_(std::bind(&Fan::UpdateCB, this)) {
    max_temp_ = 100;
    min_temp_ = 50;  
    rpm_ = mgos::Errorf(STATUS_UNAVAILABLE, "RPM not available");
  }
  Fan::Fan(int pwm_pin) :  Fan(pwm_pin, -1) {
  }
  Fan::~Fan() {
    if (rpm_pin_ >= 0) {
      mgos_gpio_disable_int(rpm_pin_);
      mgos_gpio_remove_int_handler(rpm_pin_, nullptr, nullptr);
    }
  }

  Status Fan::Init() {
    mgos_gpio_set_mode(pwm_pin_, MGOS_GPIO_MODE_OUTPUT_OD);
    mgos_pwm_set(pwm_pin_, FAN_PWM_FREQ, 0.0);
    if(rpm_pin_ >= 0) {
      mgos_gpio_set_mode(rpm_pin_, MGOS_GPIO_MODE_INPUT);
      mgos_gpio_setup_input(rpm_pin_, MGOS_GPIO_PULL_UP);
      if(!mgos_gpio_set_int_handler_isr(rpm_pin_, MGOS_GPIO_INT_EDGE_NEG,
                                   Fan::GPIOIntHandler, (void *) &count_)) {
        return mgos::Errorf(STATUS_UNAVAILABLE, "Failed to set rpm interrupt for fan");
      }
      mgos_gpio_enable_int(rpm_pin_);
      update_timer_.Reset(TIMER_INTERVAL, MGOS_TIMER_REPEAT);
      #if MGOS_HAVE_PROMETHEUS_METRICS
        mgos_prometheus_metrics_add_handler(metrics_shelly_fan, this);
      #endif
    }
    return Status::OK();
  }


  StatusOr<int> Fan::GetRPM() {
    return rpm_;
  }

  void Fan::SetMaxTemp(int temp) {
    max_temp_ = temp;
  }

  void Fan::SetMinTemp(int temp) {
    min_temp_ = temp;
  }

  void Fan::Adjust(int temp) {
    int off_temp = (pwm_ == 0.0) ? min_temp_ : min_temp_ - 2; // hysterese
    if(temp < off_temp) {
      pwm_ = 0.0;
    } else if(temp > max_temp_) {
      pwm_ = 1.0;
    } else {
      pwm_ = fmaxf(0.15, (float) (temp - min_temp_) / (max_temp_ - min_temp_));
    }
    mgos_pwm_set(pwm_pin_, FAN_PWM_FREQ, pwm_);
    LOG(LL_INFO, ("Temp: %d, PWM: %.2f, RPM: %d", temp, pwm_, (rpm_.ok() ? rpm_.ValueOrDie() : -1)));
  }

  void Fan::UpdateCB() {
    rpm_ = count_ * 30000 / TIMER_INTERVAL;
    count_ = 0;
  }

// static
  IRAM void Fan::GPIOIntHandler(int pin, void *arg) {
    (*((uint32_t *) arg))++;
    (void) pin;
  }
}