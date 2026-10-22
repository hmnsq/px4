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

#ضمن <gtest/gtest.h>
#ضمن <matrix/math.hpp>
#ضمن <lib/mathlib/mathlib.h>
#ضمن "ObstacleMath.hpp"

استعمال نطاق ميٽرڪس;

TEST(ObstacleMathTest, Project_مفاصلو_On_Horizontal_Plane)
{
	// standard vehicle orientation inputs
	Quatf vehicle_pitch_مٿي_45(Eulerf(0.0f, M_PI_4_F, 0.0f));
	Quatf vehicle_roll_ساڄو_45(Eulerf(M_PI_4_F, 0.0f, 0.0f));

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	عائم مفاصلو = 1.0f;
	عائم sensor_orientation = 0; // radians (forward facing)

	// WHEN: we project the distance onto the horizontal plane
	ObstacleMath::project_مفاصلو_on_horizontal_plane(مفاصلو, sensor_orientation, vehicle_pitch_مٿي_45);

	// THEN: the distance should be scaled correctly
	عائم expected_scale    = sqrtf(2) / 2;
	عائم expected_مفاصلو = 1.0f * expected_scale;

	EXPECT_NEAR(مفاصلو, expected_مفاصلو, 1e-5);

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	مفاصلو = 1.0f;

	ObstacleMath::project_مفاصلو_on_horizontal_plane(مفاصلو, sensor_orientation, vehicle_roll_ساڄو_45);

	// THEN: the distance should be scaled correctly
	expected_scale     = 1.f;
	expected_مفاصلو  = 1.0f * expected_scale;

	EXPECT_NEAR(مفاصلو, expected_مفاصلو, 1e-5);

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	مفاصلو = 1.0f;
	sensor_orientation = M_PI_2_F; // radians (right facing)

	ObstacleMath::project_مفاصلو_on_horizontal_plane(مفاصلو, sensor_orientation, vehicle_roll_ساڄو_45);

	// THEN: the distance should be scaled correctly
	expected_scale     = sqrtf(2) / 2;
	expected_مفاصلو  = 1.0f * expected_scale;

	EXPECT_NEAR(مفاصلو, expected_مفاصلو, 1e-5);

	// GIVEN: a distance, sensor orientation, and quaternion representing the vehicle's orientation
	مفاصلو = 1.0f;

	ObstacleMath::project_مفاصلو_on_horizontal_plane(مفاصلو, sensor_orientation, vehicle_pitch_مٿي_45);

	// THEN: the distance should be scaled correctly
	expected_scale     = 1.f;
	expected_مفاصلو  = 1.0f * expected_scale;

	EXPECT_NEAR(مفاصلو, expected_مفاصلو, 1e-5);
}

TEST(ObstacleMathTest, GetBinAtAngle)
{
	عائم bin_ويڪر = 5.0f;

	// GIVEN: a start bin, bin width, and angle
	عائم angle = 0.0f;

	// WHEN: we calculate the bin index at the angle
	uint16_t bin_اشاريو = ObstacleMath::get_bin_at_angle(bin_ويڪر, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_اشاريو, 0);

	// GIVEN: a start bin, bin width, and angle
	angle = 90.0f;

	// WHEN: we calculate the bin index at the angle
	bin_اشاريو = ObstacleMath::get_bin_at_angle(bin_ويڪر, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_اشاريو, 18);

	// GIVEN: a start bin, bin width, and angle
	angle = -90.0f;

	// WHEN: we calculate the bin index at the angle
	bin_اشاريو = ObstacleMath::get_bin_at_angle(bin_ويڪر, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_اشاريو, 54);

	// GIVEN: a start bin, bin width, and angle
	angle = 450.0f;

	// WHEN: we calculate the bin index at the angle
	bin_اشاريو = ObstacleMath::get_bin_at_angle(bin_ويڪر, angle);

	// THEN: the bin index should be correct
	EXPECT_EQ(bin_اشاريو, 18);
}

TEST(ObstacleMathTest, Get_هيٺاهون_Bound)
{
	// GIVEN: an invalid bin index, non-integer bin width, and a negative non-integer angle offset
	صحيح      bin          = -1;
	عائم    bin_ويڪر    =  7.5f;
	عائم    angle_وٿي = -4.3f;

	// WHEN: we calculate the lower bound angle of the bin
	عائم هيٺاهون_bound = ObstacleMath::get_هيٺاهون_bound_angle(bin, bin_ويڪر, angle_وٿي);

	// THEN: the lower bound angle should be correct. The bin index is wrapped to the end and
	// the angle offset is applied in the counter-clockwise direction.
	EXPECT_FLOAT_EQ(هيٺاهون_bound, 344.45);

}


TEST(ObstacleMathTest, وٿي_Bin_اشاريو)
{
	// In this test, we want to offset the bin index by a negative and positive angle.
	// We take the output of the first offset and offset it by the same angle in the
	// opposite direction to return back to the original bin index.

	// GIVEN: a bin index, bin width, and a negative angle offset
	uint16_t bin = 0;
	عائم bin_ويڪر = 5.0f;
	عائم angle_وٿي = -120.0f;

	// WHEN: we offset the bin index by the negative angle
	uint16_t new_bin_اشاريو = ObstacleMath::get_وٿي_bin_اشاريو(bin, bin_ويڪر, angle_وٿي);

	// THEN: the new bin index should be correctly offset by the wrapped angle
	EXPECT_EQ(new_bin_اشاريو, 24);

	// GIVEN: the output bin index of the previous offset, bin width, and the same angle
	// offset in positive direction
	bin = 24;
	bin_ويڪر = 5.0f;
	angle_وٿي = 120.0f;

	// WHEN: we offset the bin index by the positive angle
	new_bin_اشاريو = ObstacleMath::get_وٿي_bin_اشاريو(bin, bin_ويڪر, angle_وٿي);

	// THEN: the new bin index should return back to the original bin index
	EXPECT_EQ(new_bin_اشاريو, 0);
}


TEST(ObstacleMathTest, WrapBin)
{
	// GIVEN: a bin index within bounds and the number of bins
	صحيح bin = 0;
	صحيح bin_ڳڻپ = 72;

	// WHEN: we wrap a bin index within the bounds
	صحيح wrapped_bin = ObstacleMath::wrap_bin(bin, bin_ڳڻپ);

	// THEN: the wrapped bin index should stay 0
	EXPECT_EQ(wrapped_bin, 0);

	// GIVEN: a bin index that is out of bounds, and the number of bins
	bin = 73;
	bin_ڳڻپ = 72;

	// WHEN: we wrap a bin index that is larger than the number of bins
	wrapped_bin = ObstacleMath::wrap_bin(bin, bin_ڳڻپ);

	// THEN: the wrapped bin index should be wrapped back to the beginning
	EXPECT_EQ(wrapped_bin, 1);

	// GIVEN: a negative bin index and the number of bins
	bin = -1;
	bin_ڳڻپ = 72;

	// WHEN: we wrap a bin index that is negative
	wrapped_bin = ObstacleMath::wrap_bin(bin, bin_ڳڻپ);

	// THEN: the wrapped bin index should be wrapped back to the end
	EXPECT_EQ(wrapped_bin, 71);
}

TEST(ObstacleMathTest, HandleMissedBins)
{
	// In this test, the current and previous bin are adjacent to the bins that are outside
	// the sensor field of view. The missed bins (0,1,6 & 7) should be populated, and no
	// data should be filled in the bins outside the FOV.

	// GIVEN: measurements, current bin, previous bin, bin width, and field of view offset
	عائم measurements[8] = {0, 0, 1, 0, 0, 2, 0, 0};
	صحيح   current_bin     = 2;
	صحيح   اڳيون_bin    = 5;
	صحيح   bin_ويڪر       = 45.0f;
	عائم fov             = 270.0f;
	عائم fov_وٿي      = 360.0f - fov / 2;

	عائم measurement     = measurements[current_bin];

	// WHEN: we handle missed bins
	صحيح current_bin_وٿي  = ObstacleMath::get_وٿي_bin_اشاريو(current_bin,  bin_ويڪر, fov_وٿي);
	صحيح اڳيون_bin_وٿي = ObstacleMath::get_وٿي_bin_اشاريو(اڳيون_bin, bin_ويڪر, fov_وٿي);

	صحيح شروعات = math::min(current_bin_وٿي, اڳيون_bin_وٿي) + 1;
	صحيح پڇاڙي   = math::max(current_bin_وٿي, اڳيون_bin_وٿي);

	EXPECT_EQ(شروعات, 1);
	EXPECT_EQ(پڇاڙي,   5);

	لكل (uint16_t i = شروعات; i < پڇاڙي; i++) {
		uint16_t bin_اشاريو = ObstacleMath::get_وٿي_bin_اشاريو(i, bin_ويڪر, -fov_وٿي);
		measurements[bin_اشاريو] = measurement;
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
