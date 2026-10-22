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

#సూచన once

#అన్తర్భావ "ControlAllocation.hpp"

తరగతి Control_కేటాయింపు_Pseudo_Inverse: బహిరంగ Control_కేటాయింపు
{
బహిరంగ:
	Control_కేటాయింపు_Pseudo_Inverse() = అప్రమేయ;
	మాయా ~Control_కేటాయింపు_Pseudo_Inverse() = అప్రమేయ;

	శూన్య allocate() override;
	శూన్య సమితి_Effectiveness_మాతృక(స్థిరరాశి మాతృక::మాతృక_2<దశాంశం, అంకె_AXES, అంకె_ACTUATORS> &effectiveness,
				    స్థిరరాశి Actuator_సదిశ &actuator_trim, స్థిరరాశి Actuator_సదిశ &linearization_బిందువు, పూర్ణాంకం అంకె_actuators,
				    తార్కిక update_normalization_scale) override;
	శూన్య సమితి_Metric_కేటాయింపు(తార్కిక metric_కేటాయింపు_2) { metric_కేటాయింపు = metric_కేటాయింపు_2; }

	/**
	 * Zero the rows of axes that are (nearly) dependent on higher-priority axes
	 * (thrust z, roll, pitch, thrust x, thrust y, yaw), making them explicitly unachievable
	 * instead of inverting a near-singular matrix.
	 *
	 * @return bitmask over ControlAxis of the dropped axes
	 */
	స్థిర uint8_t dropDependentAxes(మాతృక::మాతృక_2<దశాంశం, అంకె_AXES, అంకె_ACTUATORS> &effectiveness);

రక్షిత:
	మాతృక::మాతృక_2<దశాంశం, అంకె_ACTUATORS, అంకె_AXES> _mix;

	తార్కిక _mix_update_needed{అబద్ధం};
	తార్కిక metric_కేటాయింపు{అబద్ధం};

	/**
	 * Recalculate pseudo inverse if required.
	 *
	 */
	శూన్య updatePseudoInverse();

అంతరంగ:
	/**
	 * Minimum independence for a control axis to be kept: squared norm of the unit axis row
	 * after removing its projection onto the higher-priority axes (sin^2 of the angle to their
	 * span, unit-free). 1e-2 (~5.7 deg) bounds the pseudo-inverse gain along the axis to ~10x.
	 */
	స్థిర constexpr దశాంశం kMinAxisIndependence = 1e-2f;

	శూన్య normalize_Control_కేటాయింపు_మాతృక();
	శూన్య update_Control_కేటాయింపు_మాతృక_Scale();
	తార్కిక _normalization_needs_update{అబద్ధం};
};
