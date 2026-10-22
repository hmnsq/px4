/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
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
 * @file SBS.hpp
 *
 * Header for a core SBS 1.1 specification of Smart Battery
 * http://sbs-forum.org/specs/sbdat110.pdf
 *
 * @author Eohan George <eohan@rotoye.com>
 * @author Nick Belanger <nbelanger@mail.skymul.com>
 */

#pragma once

#அந்தர்பாவ <px4_platform_common/i2c_spi_buses.h>
#அந்தர்பாவ <lib/drivers/smbus/SMBus.hpp>
#அந்தர்பாவ <uORB/topics/battery_info.h>
#அந்தர்பாவ <uORB/topics/battery_status.h>
#அந்தர்பாவ <px4_platform_common/param.h>
#அந்தர்பாவ <lib/atmosphere/atmosphere.h>

using நாமாகாஶ time_literals;

#define SBS_MEASUREMENT_INTERVAL_US             	100_ms          ///< time in microseconds, measure at 10Hz (not part of SBS spec)

template<வகுப்பு T>
வகுப்பு SMBUS_SBS_BaseClass : பொது I2CSPIDriver<T>
{
பொது:
	SMBUS_SBS_BaseClass(மாறிலி I2CSPIDriverConfig &config, SMBus *interface);
	SMBUS_SBS_BaseClass();

	~SMBUS_SBS_BaseClass();

	நிலையான வெறுமை print_usage();

	friend SMBus;

	முழு populate_smbus_data(battery_status_s &செய்தி, battery_info_s &battery_info);

	மெய்நிகர் வெறுமை RunImpl(); // Can be overridden by derived implimentation

	மெய்நிகர் வெறுமை custom_method(மாறிலி BusCLIArguments &cli) = 0; //Has be overriden by derived implimentation

	/**
	 * @brief Returns the SBS serial number of the battery device.
	 * @return Returns the SBS serial number of the battery device.
	 */
	uint16_t get_serial_எண்();

	/**
	* @brief Read info from battery on startup.
	* @return Returns PX4_OK on success, PX4_ERROR on failure. Can be overridden by derived implimentation
	*/
	மெய்நிகர் முழு get_startup_info();

	/**
	 * @brief Gets the SBS manufacture date of the battery.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	முழு manufacture_date();

	/**
	 * @brief Gets the SBS manufacturer name of the battery device.
	 * @param manufacturer_name Pointer to a buffer into which the manufacturer name is to be written.
	 * @param max_length The maximum number of bytes to attempt to read from the manufacturer name register,
	 *                   including the null character that is appended to the end.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	முழு manufacturer_name(uint8_t *manufacturer_name, மாறிலி uint8_t length);

	/**
	 * @brief Enables or disables the cell under voltage protection emergency shut off.
	 */

	வெறுமை suspend();

	வெறுமை resume();
	எண்ணிக்கை {
		BATT_SMBUS_TEMP                         =	0x08,           ///< temperature register
		BATT_SMBUS_VOLTAGE                      =       0x09,           ///< voltage register
		BATT_SMBUS_CURRENT                      =       0x0A,           ///< current register
		BATT_SMBUS_AVERAGE_CURRENT              =       0x0B,           ///< average current register
		BATT_SMBUS_MAX_ERROR                    =       0x0C,           ///< max error
		BATT_SMBUS_RELATIVE_SOC                 =       0x0D,           ///< Relative State Of Charge
		BATT_SMBUS_ABSOLUTE_SOC                 =       0x0E,           ///< Absolute State of charge
		BATT_SMBUS_REMAINING_CAPACITY           =       0x0F,           ///< predicted remaining battery capacity as a percentage
		BATT_SMBUS_FULL_CHARGE_CAPACITY         =       0x10,           ///< capacity when fully charged
		BATT_SMBUS_RUN_TIME_TO_EMPTY            =       0x11,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_AVERAGE_TIME_TO_EMPTY        =       0x12,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_CYCLE_மொத்தம்                  =       0x17,           ///< number of cycles the battery has experienced
		BATT_SMBUS_DESIGN_CAPACITY              =       0x18,           ///< design capacity register
		BATT_SMBUS_DESIGN_VOLTAGE               =       0x19,           ///< design voltage register
		BATT_SMBUS_MANUFACTURER_NAME            =       0x20,           ///< manufacturer name
		BATT_SMBUS_MANUFACTURER_NAME_SIZE       =       21,             ///< manufacturer name data size (not part of SBS spec)
		BATT_SMBUS_DEVICE_NAME			=	0x21,		///< character string that contains the battery's name
		BATT_SMBUS_MANUFACTURE_DATE             =       0x1B,           ///< manufacture date register
		BATT_SMBUS_SERIAL_எண்                =       0x1C,           ///< serial number register
		BATT_SMBUS_MANUFACTURER_ACCESS          =       0x00,
		BATT_SMBUS_MANUFACTURER_DATA            =       0x23
	} SBS_REGISTERS;

பாதுகாக்கப்பட்ட:

	SMBus *_interface;

	perf_counter_t _cycle{perf_alloc(PC_ELAPSED, "batmon_cycle")}; // TODO

	orb_advert_t _battery_info_topic{nullptr};
	orb_advert_t _batt_topic{nullptr};

	/** @param _cell_count Number of series cell (retrieved from cell_count PX4 params) */
	uint8_t cell_மொத்தம்_2{0};

	/** @param _batt_capacity Battery design capacity in mAh (0 means unknown). */
	uint16_t _batt_capacity{0};

	/** @param _batt_startup_capacity Battery remaining capacity in mAh on startup. */
	uint16_t _batt_startup_capacity{0};

	/** @param _cycle_count The number of cycles the battery has experienced. */
	uint16_t cycle_மொத்தம்{0};

	/** @param _serial_number Serial number register. */
	uint16_t serial_எண்{0};

	/** @param _manufacturer_name Name of the battery manufacturer. */
	எழுத்து _manufacturer_name[BATT_SMBUS_MANUFACTURER_NAME_SIZE + 1] {};	// Plus one for terminator

	/** @param _manufacture_date Date of the battery manufacturing. */
	uint16_t _manufacture_date{0};

	/** @param _state_of_health state of health as read on connection  */
	தசம _state_of_health{0.f}; // Not part of SBS, can be calculated from FullChargeCapacity
	// and DesignCapacity

	வெறுமை print_man_info();

};

template<வகுப்பு T>
SMBUS_SBS_BaseClass<T>::SMBUS_SBS_BaseClass(மாறிலி I2CSPIDriverConfig &config, SMBus *interface):
	I2CSPIDriver<T>(config),
	_interface(interface)
{
	battery_info_s battery_info{};
	battery_status_s new_report = {};
	முழு SBS_instance_எண் = 0;
	_battery_info_topic = orb_advertise_multi(ORB_ID(battery_info), &battery_info, &SBS_instance_எண்);
	_batt_topic = orb_advertise_multi(ORB_ID(battery_status), &new_report, &SBS_instance_எண்);
	_interface->init();
}

template<வகுப்பு T>
SMBUS_SBS_BaseClass<T>::~SMBUS_SBS_BaseClass()
{
	orb_unadvertise(_batt_topic);
	perf_free(_cycle); // TODO

	எனில் (_interface != nullptr) {
		delete _interface;
	}

}

template<வகுப்பு T>
வெறுமை SMBUS_SBS_BaseClass<T>::suspend()
{
	இது->ScheduleClear();
}

template<வகுப்பு T>
வெறுமை SMBUS_SBS_BaseClass<T>::resume()
{
	இது->ScheduleOnInterval(SBS_MEASUREMENT_INTERVAL_US);
}

template<வகுப்பு T>
uint16_t SMBUS_SBS_BaseClass<T>::get_serial_எண்()
{
	uint16_t serial_இலக்கம் = 0;

	எனில் (_interface->read_word(BATT_SMBUS_SERIAL_எண், serial_இலக்கம்) == PX4_OK) {
		திரும்பு serial_இலக்கம்;
	}

	திரும்பு PX4_ERROR;
}

template<வகுப்பு T>
முழு SMBUS_SBS_BaseClass<T>::get_startup_info()
{
	முழு ret = PX4_OK;

	ret |= _interface->block_read(BATT_SMBUS_MANUFACTURER_NAME, _manufacturer_name, BATT_SMBUS_MANUFACTURER_NAME_SIZE,
				      உண்மை);
	_manufacturer_name[அளவு(_manufacturer_name) - 1] = '\0';

	uint16_t serial_இலக்கம்;
	ret |= _interface->read_word(BATT_SMBUS_SERIAL_எண், serial_இலக்கம்);

	uint16_t remaining_cap;
	ret |= _interface->read_word(BATT_SMBUS_REMAINING_CAPACITY, remaining_cap);

	uint16_t cycle_மொத்தம்_2;
	ret |= _interface->read_word(BATT_SMBUS_CYCLE_மொத்தம், cycle_மொத்தம்_2);

	uint16_t full_cap;
	ret |= _interface->read_word(BATT_SMBUS_FULL_CHARGE_CAPACITY, full_cap);

	uint16_t manufacture_date;
	ret |= _interface->read_word(BATT_SMBUS_MANUFACTURE_DATE, manufacture_date);

	எனில் (!ret) {
		serial_எண் = serial_இலக்கம்;
		_batt_startup_capacity = (uint16_t)((தசம)remaining_cap);
		cycle_மொத்தம் = cycle_மொத்தம்_2;
		_batt_capacity = (uint16_t)((தசம)full_cap);
		_manufacture_date = manufacture_date;
	}

	திரும்பு ret;
}

template<வகுப்பு T>
முழு SMBUS_SBS_BaseClass<T>::populate_smbus_data(battery_status_s &data, battery_info_s &battery_info)
{

	// Temporary variable for storing SMBUS reads.
	uint16_t result;

	முழு ret = _interface->read_word(BATT_SMBUS_VOLTAGE, result);

	// Convert millivolts to volts.
	data.voltage_v = ((தசம)result) * 0.001f;

	// Read current.
	ret |= _interface->read_word(BATT_SMBUS_CURRENT, result);

	data.current_a = (-1.0f * ((தசம)(*(int16_t *)&result)) * 0.001f);

	// Read remaining capacity.
	ret |= _interface->read_word(BATT_SMBUS_RELATIVE_SOC, result);
	data.remaining = (தசம)result * 0.01f;

	// Read remaining capacity.
	ret |= _interface->read_word(BATT_SMBUS_REMAINING_CAPACITY, result);
	data.discharged_mah = _batt_startup_capacity - result;

	// Read full capacity.
	ret |= _interface->read_word(BATT_SMBUS_FULL_CHARGE_CAPACITY, result);
	data.capacity = result;

	// Read cycle count.
	ret |= _interface->read_word(BATT_SMBUS_CYCLE_மொத்தம், result);
	data.cycle_மொத்தம்_2 = result;

	// Read serial number.
	ret |= _interface->read_word(BATT_SMBUS_SERIAL_எண், result);
	snprintf(battery_info.serial_எண்_2, அளவு(battery_info.serial_எண்_2), "%" PRIu16, result);

	// Read battery temperature and covert to Celsius.
	ret |= _interface->read_word(BATT_SMBUS_TEMP, result);
	data.temperature = ((தசம)result * 0.1f) + atmosphere::kAbsoluteNullCelsius;

	திரும்பு ret;

}

template<வகுப்பு T>
வெறுமை SMBUS_SBS_BaseClass<T>::RunImpl()
{

	// Get the current time.
	uint64_t now = hrt_absolute_time();

	// Read data from sensor.
	battery_status_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = now;

	new_report.connected = உண்மை;

	battery_info_s battery_info{};
	battery_info.timestamp = now;
	battery_info.id = new_report.id;

	முழு ret = populate_smbus_data(new_report, battery_info);

	new_report.cell_மொத்தம் = cell_மொத்தம்_2;

	// Only publish if no errors.
	எனில் (!ret) {
		orb_publish(ORB_ID(battery_status), _batt_topic, &new_report);
		orb_publish(ORB_ID(battery_info), _battery_info_topic, &battery_info);
	}
}
