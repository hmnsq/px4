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

/**
 * @file ControlAllocationPseudoInverse.hpp
 *
 * Simple Control Allocation Algorithm
 *
 * It computes the pseudo-inverse of the effectiveness matrix
 * Actuator saturation is handled by simple clipping, do not
 * expect good performance in case of actuator saturation.
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "ControlAllocation.hpp"

ଶ୍ରେଣୀ ControlAllocationPseudoInverse: ସର୍ବସାଧାରଣ ControlAllocation
{
ସର୍ବସାଧାରଣ:
	ControlAllocationPseudoInverse() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	ଆଭାସୀ ~ControlAllocationPseudoInverse() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଶୂନ୍ୟ allocate() override;
	ଶୂନ୍ୟ setEffectivenessMatrix(ଅଚଳ matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS> &effectiveness,
				    ଅଚଳ ActuatorVector &actuator_trim, ଅଚଳ ActuatorVector &linearization_point, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_actuators,
				    ତାର୍କିକ update_normalization_scale) override;
	ଶୂନ୍ୟ setMetricAllocation(ତାର୍କିକ metric_allocation) { _metric_allocation = metric_allocation; }

	/**
	 * Zero the rows of axes that are (nearly) dependent on higher-priority axes
	 * (thrust z, roll, pitch, thrust x, thrust y, yaw), making them explicitly unachievable
	 * instead of inverting a near-singular matrix.
	 *
	 * @return bitmask over ControlAxis of the dropped axes
	 */
	ସ୍ଥିର uint8_t dropDependentAxes(matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS> &effectiveness);

ସୁରକ୍ଷିତ:
	matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_ACTUATORS, ଅଙ୍କ_AXES> _mix;

	ତାର୍କିକ _mix_update_needed{ମିଥ୍ୟା};
	ତାର୍କିକ _metric_allocation{ମିଥ୍ୟା};

	/**
	 * Recalculate pseudo inverse if required.
	 *
	 */
	ଶୂନ୍ୟ updatePseudoInverse();

ବ୍ୟକ୍ତିଗତ:
	/**
	 * Minimum independence for a control axis to be kept: squared norm of the unit axis row
	 * after removing its projection onto the higher-priority axes (sin^2 of the angle to their
	 * span, unit-free). 1e-2 (~5.7 deg) bounds the pseudo-inverse gain along the axis to ~10x.
	 */
	ସ୍ଥିର constexpr ଦଶମିକ kMinAxisIndependence = 1e-2f;

	ଶୂନ୍ୟ normalizeControlAllocationMatrix();
	ଶୂନ୍ୟ updateControlAllocationMatrixScale();
	ତାର୍କିକ _normalization_needs_update{ମିଥ୍ୟା};
};
