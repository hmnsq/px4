/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

ਨਾਮਾਕਾਸ਼ ObstacleMath
{

ਗਿਣਤੀ SensorOrientation {
	ROTATION_YAW_0   = 0,     // MAV_SENSOR_ROTATION_NONE
	ROTATION_YAW_45  = 1,	  // MAV_SENSOR_ROTATION_YAW_45
	ROTATION_YAW_90  = 2,	  // MAV_SENSOR_ROTATION_YAW_90
	ROTATION_YAW_135 = 3,	  // MAV_SENSOR_ROTATION_YAW_135
	ROTATION_YAW_180 = 4,	  // MAV_SENSOR_ROTATION_YAW_180
	ROTATION_YAW_225 = 5,	  // MAV_SENSOR_ROTATION_YAW_225
	ROTATION_YAW_270 = 6,	  // MAV_SENSOR_ROTATION_YAW_270
	ROTATION_YAW_315 = 7,	  // MAV_SENSOR_ROTATION_YAW_315
	ROTATION_CUSTOM  = 100,	  // MAV_SENSOR_ROTATION_CUSTOM

	ROTATION_FORWARD_FACING  = 0, // MAV_SENSOR_ROTATION_NONE
	ROTATION_ਸੱਜਾ_FACING    = 2, // MAV_SENSOR_ROTATION_YAW_90
	ROTATION_BACKWARD_FACING = 4, // MAV_SENSOR_ROTATION_YAW_180
	ROTATION_ਖੱਬਾ_FACING     = 6  // MAV_SENSOR_ROTATION_YAW_270
};

/**
 * Converts a sensor orientation to a yaw offset
 * @param orientation sensor orientation
 */
ਦਸ਼ਮਲਵ sensor_orientation_ਮੰਜ਼ਿਲ_yaw_ਵਿੱਥ(ਅਚਲ SensorOrientation orientation, ਅਚਲ ਦਸ਼ਮਲਵ q[4] = ਖਾਲੀ_ਸੂਚਕ);

/**
 * Scales a distance measurement taken in the vehicle body horizontal plane onto the world horizontal plane
 * @param distance measurement which is scaled down
 * @param yaw orientation of the measurement on the body horizontal plane
 * @param q_world_vehicle vehicle attitude quaternion
 */
ਖਾਲੀ project_ਦੂਰੀ_on_horizontal_plane(ਦਸ਼ਮਲਵ &ਦੂਰੀ, ਅਚਲ ਦਸ਼ਮਲਵ yaw, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quatf &q_world_vehicle);

/**
 * Returns bin index at a given angle from the 0th bin
 * @param bin_width width of a bin in degrees
 * @param angle clockwise angle from start bin in degrees
 */
ਪੂਰਨ get_bin_at_angle(ਦਸ਼ਮਲਵ bin_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ angle);

/**
 * Returns lower bound angle of a bin
 * @param bin bin index
 * @param bin_width width of a bin in degrees
 * @param angle_offset clockwise angle offset in degrees
 */
ਦਸ਼ਮਲਵ get_ਹੇਠਲਾ_bound_angle(ਪੂਰਨ bin, ਦਸ਼ਮਲਵ bin_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ angle_ਵਿੱਥ);

/**
 * Returns bin index for the current bin after an angle offset
 * @param bin current bin index
 * @param bin_width width of a bin in degrees
 * @param angle_offset clockwise angle offset in degrees
 */
ਪੂਰਨ get_ਵਿੱਥ_bin_ਸੂਚਕਾਂਕ(ਪੂਰਨ bin, ਦਸ਼ਮਲਵ bin_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ angle_ਵਿੱਥ);

/**
 * Wraps a bin index to the range [0, bin_count)
 * @param bin bin index
 * @param bin_count number of bins
 */
ਪੂਰਨ wrap_bin(ਪੂਰਨ bin, ਪੂਰਨ bin_ਗਿਣਤੀ);

/**
 * Wraps an angle to the range [0, 360)
 * @param angle angle in degrees
 */
ਦਸ਼ਮਲਵ wrap_360(ਅਚਲ ਦਸ਼ਮਲਵ angle);


} // ObstacleMath
