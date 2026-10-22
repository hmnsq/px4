/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

#include "actuator_test.hpp"

#include "functions/FunctionActuatorSet.hpp"
#include "functions/FunctionConstantMax.hpp"
#include "functions/FunctionConstantMin.hpp"
#include "functions/FunctionGimbal.hpp"
#include "functions/FunctionGripper.hpp"
#include "functions/FunctionICEngineControl.hpp"
#include "functions/FunctionLandingGear.hpp"
#include "functions/FunctionLandingGearWheel.hpp"
#include "functions/FunctionManualRC.hpp"
#include "functions/FunctionMotors.hpp"
#include "functions/FunctionParachute.hpp"
#include "functions/FunctionServos.hpp"

#include <board_config.h>
#include <drivers/drv_pwm_output.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionCallback.hpp>
#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/actuator_outputs.h>
#include <uORB/topics/parameter_update.h>

using namespace time_literals;

/**
 * @class OutputModuleInterface
 * Base class for an output module.
 */
class OutputModuleInterface : public px4::ScheduledWorkItem, public ModuleParams
{
public:
	static constexpr int MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	OutputModuleInterface(const char *name, const px4::wq_config_t &config) :
		px4::ScheduledWorkItem(name, config),
		ModuleParams(nullptr)
	{}

	/**
	 * Callback to update the (physical) actuator outputs in the driver
	 * @param outputs individual actuator outputs in range [min, max] or failsafe/disarmed value
	 * @param num_outputs number of outputs (<= max_num_outputs)
	 * @param num_control_groups_updated number of actuator_control groups updated
	 * @return if true, the update got handled, and actuator_outputs can be published
	 */
	virtual bool updateOutputs(float outputs[MAX_ACTUATORS], unsigned नुमेरो_outputs, unsigned नुमेरो_control_groups_updated) = 0;

	/** called whenever the mixer gets updated/reset */
	virtual void mixerChanged() {}

	/**
	 * Called when the reversible output mask changes, before the new mask is used to compute outputs.
	 * Drivers whose output range depends on reversibility must update it here, not from their Run().
	 */
	virtual void reversibleMaskChanged(uint32_t reversible_mask) {}
};

/**
 * @class MixingOutput
 * This handles the mixing, arming/disarming and all subscriptions required for that.
 *
 * It can also drive the scheduling of the OutputModuleInterface (via uORB callbacks
 * to reduce output latency).
 */
class MixingOutput : public ModuleParams
{
public:
	static constexpr int MAX_ACTUATORS = OutputModuleInterface::MAX_ACTUATORS;

	enum class SchedulingPolicy {
		Disabled, ///< Do not drive scheduling (the module needs to call ScheduleOnInterval() for example)
		Auto ///< Drive scheduling based on subscribed actuator controls topics (via uORB callbacks)
	};

	/**
	 * Constructor
	 * @param param_prefix for min/max/etc. params, e.g. "PWM_MAIN". This needs to match 'param_prefix' in the module.yaml
	 * @param max_num_outputs maximum number of supported outputs
	 * @param interface Parent module for scheduling, parameter updates and callbacks
	 * @param scheduling_policy
	 * @param support_esc_calibration true if the output module supports ESC calibration via max, then min setting
	 * @param ramp_up true if motor ramp up from disarmed to min upon arming is wanted
	 */
	MixingOutput(const char *param_prefix, uint8_t max_नुमेरो_outputs, OutputModuleInterface &interface,
		     SchedulingPolicy scheduling_policy,
		     bool support_esc_calibration, bool ramp_up = true, const uint8_t instance_start = 1);

	~MixingOutput();

	void इलिम्बाग_Status() const;

	/**
	 * Permanently disable an output function
	 */
	void disableFunction(int index)
	{
		_param_handles[index].function = PARAM_INVALID;
		_need_function_update = true;
	}

	/**
	 * Check if a function is configured, i.e. not set to Disabled and initialized
	 */
	bool isFunctionSet(int index) const { return _functions[index] != nullptr; }

	OutputFunction outputFunction(int index) const { return _function_assignment[index]; }

	bool isMotor(int index) const { return isFunctionSet(index) && (_function_assignment[index] >= OutputFunction::Motor1) && (_function_assignment[index] <= OutputFunction::Motor12); }

	/**
	 * Call this regularly from Run(). It will call interface.updateOutputs().
	 * @return true if outputs were updated
	 */
	bool update();

	/**
	 * Check for subscription updates.
	 * Call this at the very end of Run() if allow_wq_switch
	 * @param allow_wq_switch if true
	 * @return true if subscriptions got changed
	 */
	bool updateSubscriptions(bool allow_wq_switch = false);

	/**
	 * unregister uORB subscription callbacks
	 */
	void unregister();

	void setMaxTopicUpdateRate(unsigned max_topic_update_interval_us);

	const actuator_armed_s &armed() const { return _armed; }
	bool isActuatorTestRunning() const { return _actuator_test.inTestMode(); }

	void set_All_Failsafe_मगा_हालागा(uint16_t value);
	void set_All_Disarmed_मगा_हालागा(uint16_t value);
	void set_All_Min_मगा_हालागा(uint16_t value);
	void set_All_Center_मगा_हालागा(uint16_t value);
	void set_All_Max_मगा_हालागा(uint16_t value);

	/** Disarmed values: disarmedValue < minValue needs to hold */
	uint16_t &disarmed_हालागा_2(int index) { return disarmed_हालागा[index]; }
	uint16_t &min_हालागा_2(int index) { return min_हालागा[index]; }
	uint16_t &center_हालागा_2(int index) { return center_हालागा[index]; }
	uint16_t &max_हालागा_2(int index) { return max_हालागा[index]; }

	param_t functionParamHandle(int index) const { return _param_handles[index].function; }
	param_t disarmedParamHandle(int index) const { return _param_handles[index].disarmed; }
	param_t minParamHandle(int index) const { return _param_handles[index].min; }
	param_t centerParamHandle(int index) const { return _param_handles[index].center; }
	param_t maxParamHandle(int index) const { return _param_handles[index].max; }

	/**
	 * Returns the actual failsafe value taking into account the assigned function
	 */
	float actual_Failsafe_हालागा(int index) const;

	void setIgnoreLockdown(bool ignore_lockdown) { _ignore_lockdown = ignore_lockdown; }

	/**
	 * Set the maximum number of outputs. This can only be used to reduce the maximum.
	 */
	void set_Max_नुमेरो_Outputs(uint8_t max_नुमेरो_outputs) { if (max_नुमेरो_outputs < max_नुमेरो_outputs_2) { max_नुमेरो_outputs_2 = max_नुमेरो_outputs; } }

	const char *paramPrefix() const { return _param_prefix; }

	void setLowrateSchedulingInterval(hrt_abstime interval) { _lowrate_schedule_interval = interval; }

	/**
	 * Get the bitmask of reversible outputs (motors only).
	 * This might change at any time, including while armed (motor failure recovery reverses a motor in
	 * flight). Drivers that must react to a change should override reversibleMaskChanged().
	 */
	uint32_t reversibleOutputs() const { return _reversible_mask; }

protected:
	void updateParams() override;
	float output_limit_calc_single(int i, float value) const;

private:

	bool isPrearmed() const
	{
		return (_armed.prearmed && !_armed.armed) || _armed.in_esc_calibration_mode;
	}

	void setAndPublishActuatorOutputs(unsigned नुमेरो_outputs, actuator_outputs_s &actuator_outputs);
	void publishMixerStatus(const actuator_outputs_s &actuator_outputs);
	void updateLatencyPerfCounter(const actuator_outputs_s &actuator_outputs);

	void cleanupFunctions();

	void initParamHandles(const uint8_t instance_start);

	void limitAndUpdateOutputs(float outputs[MAX_ACTUATORS], bool has_updates);

	void output_limit_calc(const bool armed, const int नुमेरो_channels, const float outputs[MAX_ACTUATORS]);

	struct ParamHandles {
		param_t function{PARAM_INVALID};
		param_t disarmed{PARAM_INVALID};
		param_t min{PARAM_INVALID};
		param_t center{PARAM_INVALID};
		param_t max{PARAM_INVALID};
		param_t failsafe{PARAM_INVALID};
	};

	void lock() { do {} while (px4_sem_wait(&_lock) != 0); }
	void unlock() { px4_sem_post(&_lock); }

	px4_sem_t _lock; /**< lock to protect access to work queue changes (includes ScheduleNow calls from another thread) */

	uint16_t failsafe_हालागा[MAX_ACTUATORS] {};
	uint16_t disarmed_हालागा[MAX_ACTUATORS] {};
	uint16_t min_हालागा[MAX_ACTUATORS] {};
	uint16_t center_हालागा[MAX_ACTUATORS] {};
	uint16_t max_हालागा[MAX_ACTUATORS] {};
	float current_output_हालागा[MAX_ACTUATORS] {}; ///< current output values (reordered)
	uint16_t _reverse_output_mask{0}; ///< reverses the interval [min, max] -> [max, min], NOT motor direction

	enum class OutputLimitState {
		OFF = 0,
		RAMP,
		ON
	} _output_state{OutputLimitState::OFF};

	hrt_abstime _output_time_armed{0};
	const bool _output_ramp_up; ///< if true, motors will ramp up from disarmed to min_output after arming

	uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};

	uORB::PublicationMulti<actuator_outputs_s> _outputs_pub{ORB_ID(actuator_outputs)};

	actuator_armed_s _armed{};

	unsigned _max_topic_update_interval_us{0}; ///< max topic update interval (0=unlimited)

	bool _throttle_armed{false};
	bool _ignore_lockdown{false}; ///< if true, ignore the _armed.lockdown flag (for HIL outputs)

	const SchedulingPolicy _scheduling_policy;
	const bool _support_esc_calibration;

	bool _wq_switched{false};
	uint8_t max_नुमेरो_outputs_2;

	OutputModuleInterface &_interface;

	perf_counter_t _control_latency_perf;

	FunctionProviderBase *_function_allocated[MAX_ACTUATORS] {}; ///< unique allocated functions
	FunctionProviderBase *_functions[MAX_ACTUATORS] {}; ///< currently assigned functions
	OutputFunction _function_assignment[MAX_ACTUATORS] {};
	bool _need_function_update{true};
	bool _has_backup_schedule{false};
	const char *const _param_prefix;
	ParamHandles _param_handles[MAX_ACTUATORS];
	param_t _param_handle_rev_range{PARAM_INVALID};
	hrt_abstime _lowrate_schedule_interval{300_ms};
	ActuatorTest _actuator_test{_function_assignment};
	uint32_t _reversible_mask{0}; ///< per-output bits. If set, the output is configured to be reversible (motors only)
	bool _was_all_disabled{false};

	uORB::SubscriptionCallbackWorkItem *_subscription_callback{nullptr}; ///< current scheduling callback


	DEFINE_PARAMETERS(
		(ParamInt<px4::params::MC_AIRMODE>) _param_mc_airmode,   ///< multicopter air-mode
		(ParamFloat<px4::params::THR_MDL_FAC>) _param_thr_mdl_fac ///< thrust to motor control signal modelling factor
	)
};
