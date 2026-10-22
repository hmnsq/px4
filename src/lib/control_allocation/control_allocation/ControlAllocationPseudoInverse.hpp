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

#ضمن "ControlAllocation.hpp"

מחלקה ControlAllocationPseudoInverse: عام ControlAllocation
{
عام:
	ControlAllocationPseudoInverse() = افتراضي;
	virtual ~ControlAllocationPseudoInverse() = افتراضي;

	فراغ allocate() override;
	فراغ setEffectivenessMatrix(ثابت matrix::Matrix<عائم, NUM_AXES, NUM_ACTUATORS> &effectiveness,
				    ثابت ActuatorVector &actuator_trim, ثابت ActuatorVector &linearization_point, صحيح num_actuators,
				    منطقي update_normalization_scale) override;
	فراغ setMetricAllocation(منطقي metric_allocation) { _metric_allocation = metric_allocation; }

	/**
	 * Zero the rows of axes that are (nearly) dependent on higher-priority axes
	 * (thrust z, roll, pitch, thrust x, thrust y, yaw), making them explicitly unachievable
	 * instead of inverting a near-singular matrix.
	 *
	 * @return bitmask over ControlAxis of the dropped axes
	 */
	ساكن uint8_t dropDependentAxes(matrix::Matrix<عائم, NUM_AXES, NUM_ACTUATORS> &effectiveness);

محمي:
	matrix::Matrix<عائم, NUM_ACTUATORS, NUM_AXES> _mix;

	منطقي _mix_update_needed{שקר};
	منطقي _metric_allocation{שקר};

	/**
	 * Recalculate pseudo inverse if required.
	 *
	 */
	فراغ updatePseudoInverse();

خاص:
	/**
	 * Minimum independence for a control axis to be kept: squared norm of the unit axis row
	 * after removing its projection onto the higher-priority axes (sin^2 of the angle to their
	 * span, unit-free). 1e-2 (~5.7 deg) bounds the pseudo-inverse gain along the axis to ~10x.
	 */
	ساكن constexpr عائم kMinAxisIndependence = 1e-2f;

	فراغ normalizeControlAllocationMatrix();
	فراغ updateControlAllocationMatrixScale();
	منطقي _normalization_needs_update{שקר};
};
