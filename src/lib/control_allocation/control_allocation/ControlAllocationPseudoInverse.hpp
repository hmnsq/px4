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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "ControlAllocation.hpp"

ವರ್ಗ Control_ಹಂಚಿಕೆ_Pseudo_Inverse: ಸಾರ್ವಜನಿಕ Control_ಹಂಚಿಕೆ
{
ಸಾರ್ವಜನಿಕ:
	Control_ಹಂಚಿಕೆ_Pseudo_Inverse() = ಪೂರ್ವನಿಯೋಜಿತ;
	ವಾಸ್ತವಿಕ ~Control_ಹಂಚಿಕೆ_Pseudo_Inverse() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಶೂನ್ಯ allocate() override;
	ಶೂನ್ಯ ಗಣ_Effectiveness_ಮಾತೃಕೆ(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> &effectiveness,
				    ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_trim, ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &linearization_ಬಿಂದು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators,
				    ತಾರ್ಕಿಕ update_normalization_scale) override;
	ಶೂನ್ಯ ಗಣ_Metric_ಹಂಚಿಕೆ(ತಾರ್ಕಿಕ metric_ಹಂಚಿಕೆ_2) { metric_ಹಂಚಿಕೆ = metric_ಹಂಚಿಕೆ_2; }

	/**
	 * Zero the rows of axes that are (nearly) dependent on higher-priority axes
	 * (thrust z, roll, pitch, thrust x, thrust y, yaw), making them explicitly unachievable
	 * instead of inverting a near-singular matrix.
	 *
	 * @return bitmask over ControlAxis of the dropped axes
	 */
	ಸ್ಥಿರ uint8_t dropDependentAxes(ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> &effectiveness);

ಸಂರಕ್ಷಿತ:
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_ACTUATORS, ಅಂಕೆ_AXES> _mix;

	ತಾರ್ಕಿಕ _mix_update_needed{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ metric_ಹಂಚಿಕೆ{ಸುಳ್ಳು};

	/**
	 * Recalculate pseudo inverse if required.
	 *
	 */
	ಶೂನ್ಯ updatePseudoInverse();

ಖಾಸಗಿ:
	/**
	 * Minimum independence for a control axis to be kept: squared norm of the unit axis row
	 * after removing its projection onto the higher-priority axes (sin^2 of the angle to their
	 * span, unit-free). 1e-2 (~5.7 deg) bounds the pseudo-inverse gain along the axis to ~10x.
	 */
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kMinAxisIndependence = 1e-2f;

	ಶೂನ್ಯ normalize_Control_ಹಂಚಿಕೆ_ಮಾತೃಕೆ();
	ಶೂನ್ಯ update_Control_ಹಂಚಿಕೆ_ಮಾತೃಕೆ_Scale();
	ತಾರ್ಕಿಕ _normalization_needs_update{ಸುಳ್ಳು};
};
