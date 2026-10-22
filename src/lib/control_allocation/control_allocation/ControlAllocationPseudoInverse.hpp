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

#अन्तर्भाव "ControlAllocation.hpp"

वर्ग ControlAllocationPseudoInverse: सार्वजनिक ControlAllocation
{
सार्वजनिक:
	ControlAllocationPseudoInverse() = स्वभाव;
	आभास ~ControlAllocationPseudoInverse() = स्वभाव;

	शून्य allocate() override;
	शून्य setEffectivenessMatrix(ध्रुव matrix::Matrix<भिन्न, अंक_AXES, अंक_ACTUATORS> &effectiveness,
				    ध्रुव ActuatorVector &actuator_trim, ध्रुव ActuatorVector &linearization_point, पूर्णाङ्क अंक_actuators,
				    तार्किक update_normalization_scale) override;
	शून्य setMetricAllocation(तार्किक metric_allocation) { _metric_allocation = metric_allocation; }

	/**
	 * Zero the rows of axes that are (nearly) dependent on higher-priority axes
	 * (thrust z, roll, pitch, thrust x, thrust y, yaw), making them explicitly unachievable
	 * instead of inverting a near-singular matrix.
	 *
	 * @return bitmask over ControlAxis of the dropped axes
	 */
	स्थिर uint8_t dropDependentAxes(matrix::Matrix<भिन्न, अंक_AXES, अंक_ACTUATORS> &effectiveness);

रक्षित:
	matrix::Matrix<भिन्न, अंक_ACTUATORS, अंक_AXES> _mix;

	तार्किक _mix_update_needed{असत्य};
	तार्किक _metric_allocation{असत्य};

	/**
	 * Recalculate pseudo inverse if required.
	 *
	 */
	शून्य updatePseudoInverse();

गुप्त:
	/**
	 * Minimum independence for a control axis to be kept: squared norm of the unit axis row
	 * after removing its projection onto the higher-priority axes (sin^2 of the angle to their
	 * span, unit-free). 1e-2 (~5.7 deg) bounds the pseudo-inverse gain along the axis to ~10x.
	 */
	स्थिर constexpr भिन्न kMinAxisIndependence = 1e-2f;

	शून्य normalizeControlAllocationMatrix();
	शून्य updateControlAllocationMatrixScale();
	तार्किक _normalization_needs_update{असत्य};
};
