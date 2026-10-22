/****************************************************************************
 *
 *   Copyright (C) 2025 PX4 Development Team. All rights reserved.
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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <matrix/math.hpp>
#अन्तर्भाव <lib/mathlib/mathlib.h>
#अन्तर्भाव "ObstacleMath.hpp"

उपयुज्य नामाकाश matrix;

TEST(ObstacleMathTest, ProjectDistanceOnHorizontalPlane)
{
	// standard vehicle orientation inputs
	Quatf vehicle_pitch_ऊर्ध्वम्_45(Eulerf(0.0f, M_PI_4_F, 0.0f));
	Quatf vehicle_roll_दक्षिणः_45(Eulerf(M_PI_4_F, 0.0f, 0.0f));

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	भिन्न distance = 1.0f;
	भिन्न sensor_orientation = 0; // radians (forward facing)

	// WHEN: we project the distance onto the horizontal plane
	ObstacleMath::project_distance_on_horizontal_plane(distance, sensor_orientation, vehicle_pitch_ऊर्ध्वम्_45);

	// THEN: the distance should be scaled correctly
	भिन्न expected_scale    = sqrtf(2) / 2;
	भिन्न expected_distance = 1.0f * expected_scale;

	EXPECT_NEAR(distance, expected_distance, 1e-5);

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	distance = 1.0f;

	ObstacleMath::project_distance_on_horizontal_plane(distance, sensor_orientation, vehicle_roll_दक्षिणः_45);

	// THEN: the distance should be scaled correctly
	expected_scale     = 1.f;
	expected_distance  = 1.0f * expected_scale;

	EXPECT_NEAR(distance, expected_distance, 1e-5);

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	distance = 1.0f;
	sensor_orientation = M_PI_2_F; // radians (right facing)

	ObstacleMath::project_distance_on_horizontal_plane(distance, sensor_orientation, vehicle_roll_दक्षिणः_45);

	// THEN: the distance should be scaled correctly
	expected_scale     = sqrtf(2) / 2;
	expected_distance  = 1.0f * expected_scale;

	EXPECT_NEAR(distance, expected_distance, 1e-5);

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	distance = 1.0f;

	ObstacleMath::project_distance_on_horizontal_plane(distance, sensor_orientation, vehicle_pitch_ऊर्ध्वम्_45);

	// THEN: the distance should be scaled correctly
	expected_scale     = 1.f;
	expected_distance  = 1.0f * expected_scale;

	EXPECT_NEAR(distance, expected_distance, 1e-5);
}

TEST(ObstacleMathTest, GetBinAtAngle)
{
	भिन्न bin_विस्तारः = 5.0f;

	// GIVEN: a start bin, bin width, and angle
	भिन्न angle = 0.0f;

	// WHEN: we calculate the bin index at the angle
	uint16_t bin_सूचकांक = ObstacleMath::get_bin_at_angle(bin_विस्तारः, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_सूचकांक, 0);

	// GIVEN: a start bin, bin width, and angle
	angle = 90.0f;

	// WHEN: we calculate the bin index at the angle
	bin_सूचकांक = ObstacleMath::get_bin_at_angle(bin_विस्तारः, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_सूचकांक, 18);

	// GIVEN: a start bin, bin width, and angle
	angle = -90.0f;

	// WHEN: we calculate the bin index at the angle
	bin_सूचकांक = ObstacleMath::get_bin_at_angle(bin_विस्तारः, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_सूचकांक, 54);

	// GIVEN: a start bin, bin width, and angle
	angle = 450.0f;

	// WHEN: we calculate the bin index at the angle
	bin_सूचकांक = ObstacleMath::get_bin_at_angle(bin_विस्तारः, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_सूचकांक, 18);
}

TEST(ObstacleMathTest, Get_अधस्तनम्_Bound)
{
	// GIVEN: an invalid bin index, non-integer bin width, and a negative non-integer angle offset
	पूर्णाङ्क      bin          = -1;
	भिन्न    bin_विस्तारः    =  7.5f;
	भिन्न    angle_विस्थापनम् = -4.3f;

	// WHEN: we calculate the lower bound angle of the bin
	भिन्न अधस्तनम्_bound = ObstacleMath::get_अधस्तनम्_bound_angle(bin, bin_विस्तारः, angle_विस्थापनम्);

	// THEN: the lower bound angle should be correct. The bin index is wrapped to the end and
	// the angle offset is applied in the counter-clockwise direction.
	EXPECT_FLOAT_EQ(अधस्तनम्_bound, 344.45);

}


TEST(ObstacleMathTest, विस्थापनम्_Bin_सूचकांक)
{
	// In this test, we want to offset the bin index by a negative and positive angle.
	// We take the output of the first offset and offset it by the same angle in the
	// opposite direction to return back to the original bin index.

	// GIVEN: a bin index, bin width, and a negative angle offset
	uint16_t bin = 0;
	भिन्न bin_विस्तारः = 5.0f;
	भिन्न angle_विस्थापनम् = -120.0f;

	// WHEN: we offset the bin index by the negative angle
	uint16_t new_bin_सूचकांक = ObstacleMath::get_विस्थापनम्_bin_सूचकांक(bin, bin_विस्तारः, angle_विस्थापनम्);

	// THEN: the new bin index should be correctly offset by the wrapped angle
	EXPECT_EQ(new_bin_सूचकांक, 24);

	// GIVEN: the output bin index of the previous offset, bin width, and the same angle
	// offset in positive direction
	bin = 24;
	bin_विस्तारः = 5.0f;
	angle_विस्थापनम् = 120.0f;

	// WHEN: we offset the bin index by the positive angle
	new_bin_सूचकांक = ObstacleMath::get_विस्थापनम्_bin_सूचकांक(bin, bin_विस्तारः, angle_विस्थापनम्);

	// THEN: the new bin index should return back to the original bin index
	EXPECT_EQ(new_bin_सूचकांक, 0);
}


TEST(ObstacleMathTest, WrapBin)
{
	// GIVEN: a bin index within bounds and the number of bins
	पूर्णाङ्क bin = 0;
	पूर्णाङ्क bin_गणनम् = 72;

	// WHEN: we wrap a bin index within the bounds
	पूर्णाङ्क wrapped_bin = ObstacleMath::wrap_bin(bin, bin_गणनम्);

	// THEN: the wrapped bin index should stay 0
	EXPECT_EQ(wrapped_bin, 0);

	// GIVEN: a bin index that is out of bounds, and the number of bins
	bin = 73;
	bin_गणनम् = 72;

	// WHEN: we wrap a bin index that is larger than the number of bins
	wrapped_bin = ObstacleMath::wrap_bin(bin, bin_गणनम्);

	// THEN: the wrapped bin index should be wrapped back to the beginning
	EXPECT_EQ(wrapped_bin, 1);

	// GIVEN: a negative bin index and the number of bins
	bin = -1;
	bin_गणनम् = 72;

	// WHEN: we wrap a bin index that is negative
	wrapped_bin = ObstacleMath::wrap_bin(bin, bin_गणनम्);

	// THEN: the wrapped bin index should be wrapped back to the end
	EXPECT_EQ(wrapped_bin, 71);
}

TEST(ObstacleMathTest, HandleMissedBins)
{
	// In this test, the current and previous bin are adjacent to the bins that are outside
	// the sensor field of view. The missed bins (0,1,6 & 7) should be populated, and no
	// data should be filled in the bins outside the FOV.

	// GIVEN: measurements, current bin, previous bin, bin width, and field of view offset
	भिन्न measurements[8] = {0, 0, 1, 0, 0, 2, 0, 0};
	पूर्णाङ्क   current_bin     = 2;
	पूर्णाङ्क   previous_bin    = 5;
	पूर्णाङ्क   bin_विस्तारः       = 45.0f;
	भिन्न fov             = 270.0f;
	भिन्न fov_विस्थापनम्      = 360.0f - fov / 2;

	भिन्न measurement     = measurements[current_bin];

	// WHEN: we handle missed bins
	पूर्णाङ्क current_bin_विस्थापनम्  = ObstacleMath::get_विस्थापनम्_bin_सूचकांक(current_bin,  bin_विस्तारः, fov_विस्थापनम्);
	पूर्णाङ्क previous_bin_विस्थापनम् = ObstacleMath::get_विस्थापनम्_bin_सूचकांक(previous_bin, bin_विस्तारः, fov_विस्थापनम्);

	पूर्णाङ्क start = math::min(current_bin_विस्थापनम्, previous_bin_विस्थापनम्) + 1;
	पूर्णाङ्क अन्तः_2   = math::max(current_bin_विस्थापनम्, previous_bin_विस्थापनम्);

	EXPECT_EQ(start, 1);
	EXPECT_EQ(अन्तः_2,   5);

	प्रति (uint16_t i = start; i < अन्तः_2; i++) {
		uint16_t bin_सूचकांक = ObstacleMath::get_विस्थापनम्_bin_सूचकांक(i, bin_विस्तारः, -fov_विस्थापनम्);
		measurements[bin_सूचकांक] = measurement;
	}

	// THEN: the correct missed bins should be populated with the measurement
	EXPECT_EQ(measurements[0], 1);
	EXPECT_EQ(measurements[1], 1);
	EXPECT_EQ(measurements[2], 1);
	EXPECT_EQ(measurements[3], 0);
	EXPECT_EQ(measurements[4], 0);
	EXPECT_EQ(measurements[5], 2);
	EXPECT_EQ(measurements[6], 1);
	EXPECT_EQ(measurements[7], 1);
}
