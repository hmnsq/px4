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

#अन्तर्भाव <px4_platform_common/i2c_spi_buses.h>
#अन्तर्भाव <lib/drivers/smbus/SMBus.hpp>
#अन्तर्भाव <uORB/topics/battery_info.h>
#अन्तर्भाव <uORB/topics/battery_status.h>
#अन्तर्भाव <px4_platform_common/param.h>
#अन्तर्भाव <lib/atmosphere/atmosphere.h>

using नामाकाश time_literals;

#define SBS_MEASUREMENT_INTERVAL_US             	100_ms          ///< time in microseconds, measure at 10Hz (not part of SBS spec)

template<वर्ग T>
वर्ग SMBUS_SBS_BaseClass : सार्वजनिक I2CSPIDriver<T>
{
सार्वजनिक:
	SMBUS_SBS_BaseClass(ध्रुव I2CSPIDriverConfig &config, SMBus *interface);
	SMBUS_SBS_BaseClass();

	~SMBUS_SBS_BaseClass();

	स्थिर शून्य print_usage();

	friend SMBus;

	पूर्णाङ्क populate_smbus_data(battery_status_s &संदेश, battery_info_s &battery_info);

	आभास शून्य RunImpl(); // Can be overridden by derived implimentation

	आभास शून्य custom_method(ध्रुव BusCLIArguments &cli) = 0; //Has be overriden by derived implimentation

	/**
	 * @brief Returns the SBS serial number of the battery device.
	 * @return Returns the SBS serial number of the battery device.
	 */
	uint16_t get_serial_संख्या();

	/**
	* @brief Read info from battery on startup.
	* @return Returns PX4_OK on success, PX4_ERROR on failure. Can be overridden by derived implimentation
	*/
	आभास पूर्णाङ्क get_startup_info();

	/**
	 * @brief Gets the SBS manufacture date of the battery.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	पूर्णाङ्क manufacture_date();

	/**
	 * @brief Gets the SBS manufacturer name of the battery device.
	 * @param manufacturer_name Pointer to a buffer into which the manufacturer name is to be written.
	 * @param max_length The maximum number of bytes to attempt to read from the manufacturer name register,
	 *                   including the null character that is appended to the end.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	पूर्णाङ्क manufacturer_name(uint8_t *manufacturer_name, ध्रुव uint8_t length);

	/**
	 * @brief Enables or disables the cell under voltage protection emergency shut off.
	 */

	शून्य suspend();

	शून्य resume();
	गणना {
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
		BATT_SMBUS_CYCLE_मोजणी                  =       0x17,           ///< number of cycles the battery has experienced
		BATT_SMBUS_DESIGN_CAPACITY              =       0x18,           ///< design capacity register
		BATT_SMBUS_DESIGN_VOLTAGE               =       0x19,           ///< design voltage register
		BATT_SMBUS_MANUFACTURER_NAME            =       0x20,           ///< manufacturer name
		BATT_SMBUS_MANUFACTURER_NAME_SIZE       =       21,             ///< manufacturer name data size (not part of SBS spec)
		BATT_SMBUS_DEVICE_NAME			=	0x21,		///< character string that contains the battery's name
		BATT_SMBUS_MANUFACTURE_DATE             =       0x1B,           ///< manufacture date register
		BATT_SMBUS_SERIAL_संख्या                =       0x1C,           ///< serial number register
		BATT_SMBUS_MANUFACTURER_ACCESS          =       0x00,
		BATT_SMBUS_MANUFACTURER_DATA            =       0x23
	} SBS_REGISTERS;

रक्षित:

	SMBus *_interface;

	perf_counter_t _cycle{perf_alloc(PC_ELAPSED, "batmon_cycle")}; // TODO

	orb_advert_t _battery_info_topic{nullptr};
	orb_advert_t _batt_topic{nullptr};

	/** @param _cell_count Number of series cell (retrieved from cell_count PX4 params) */
	uint8_t cell_मोजणी_2{0};

	/** @param _batt_capacity Battery design capacity in mAh (0 means unknown). */
	uint16_t _batt_capacity{0};

	/** @param _batt_startup_capacity Battery remaining capacity in mAh on startup. */
	uint16_t _batt_startup_capacity{0};

	/** @param _cycle_count The number of cycles the battery has experienced. */
	uint16_t cycle_मोजणी{0};

	/** @param _serial_number Serial number register. */
	uint16_t serial_संख्या{0};

	/** @param _manufacturer_name Name of the battery manufacturer. */
	वर्ण _manufacturer_name[BATT_SMBUS_MANUFACTURER_NAME_SIZE + 1] {};	// Plus one for terminator

	/** @param _manufacture_date Date of the battery manufacturing. */
	uint16_t _manufacture_date{0};

	/** @param _state_of_health state of health as read on connection  */
	भिन्न _state_of_health{0.f}; // Not part of SBS, can be calculated from FullChargeCapacity
	// and DesignCapacity

	शून्य print_man_info();

};

template<वर्ग T>
SMBUS_SBS_BaseClass<T>::SMBUS_SBS_BaseClass(ध्रुव I2CSPIDriverConfig &config, SMBus *interface):
	I2CSPIDriver<T>(config),
	_interface(interface)
{
	battery_info_s battery_info{};
	battery_status_s new_report = {};
	पूर्णाङ्क SBS_instance_संख्या = 0;
	_battery_info_topic = orb_advertise_multi(ORB_ID(battery_info), &battery_info, &SBS_instance_संख्या);
	_batt_topic = orb_advertise_multi(ORB_ID(battery_status), &new_report, &SBS_instance_संख्या);
	_interface->init();
}

template<वर्ग T>
SMBUS_SBS_BaseClass<T>::~SMBUS_SBS_BaseClass()
{
	orb_unadvertise(_batt_topic);
	perf_free(_cycle); // TODO

	यदि (_interface != nullptr) {
		delete _interface;
	}

}

template<वर्ग T>
शून्य SMBUS_SBS_BaseClass<T>::suspend()
{
	एतत्->ScheduleClear();
}

template<वर्ग T>
शून्य SMBUS_SBS_BaseClass<T>::resume()
{
	एतत्->ScheduleOnInterval(SBS_MEASUREMENT_INTERVAL_US);
}

template<वर्ग T>
uint16_t SMBUS_SBS_BaseClass<T>::get_serial_संख्या()
{
	uint16_t serial_अंक = 0;

	यदि (_interface->read_word(BATT_SMBUS_SERIAL_संख्या, serial_अंक) == PX4_OK) {
		प्रत्यागम serial_अंक;
	}

	प्रत्यागम PX4_ERROR;
}

template<वर्ग T>
पूर्णाङ्क SMBUS_SBS_BaseClass<T>::get_startup_info()
{
	पूर्णाङ्क ret = PX4_OK;

	ret |= _interface->block_read(BATT_SMBUS_MANUFACTURER_NAME, _manufacturer_name, BATT_SMBUS_MANUFACTURER_NAME_SIZE,
				      सत्य);
	_manufacturer_name[परिमाण(_manufacturer_name) - 1] = '\0';

	uint16_t serial_अंक;
	ret |= _interface->read_word(BATT_SMBUS_SERIAL_संख्या, serial_अंक);

	uint16_t remaining_cap;
	ret |= _interface->read_word(BATT_SMBUS_REMAINING_CAPACITY, remaining_cap);

	uint16_t cycle_मोजणी_2;
	ret |= _interface->read_word(BATT_SMBUS_CYCLE_मोजणी, cycle_मोजणी_2);

	uint16_t full_cap;
	ret |= _interface->read_word(BATT_SMBUS_FULL_CHARGE_CAPACITY, full_cap);

	uint16_t manufacture_date;
	ret |= _interface->read_word(BATT_SMBUS_MANUFACTURE_DATE, manufacture_date);

	यदि (!ret) {
		serial_संख्या = serial_अंक;
		_batt_startup_capacity = (uint16_t)((भिन्न)remaining_cap);
		cycle_मोजणी = cycle_मोजणी_2;
		_batt_capacity = (uint16_t)((भिन्न)full_cap);
		_manufacture_date = manufacture_date;
	}

	प्रत्यागम ret;
}

template<वर्ग T>
पूर्णाङ्क SMBUS_SBS_BaseClass<T>::populate_smbus_data(battery_status_s &data, battery_info_s &battery_info)
{

	// Temporary variable for storing SMBUS reads.
	uint16_t result;

	पूर्णाङ्क ret = _interface->read_word(BATT_SMBUS_VOLTAGE, result);

	// Convert millivolts to volts.
	data.voltage_v = ((भिन्न)result) * 0.001f;

	// Read current.
	ret |= _interface->read_word(BATT_SMBUS_CURRENT, result);

	data.current_a = (-1.0f * ((भिन्न)(*(int16_t *)&result)) * 0.001f);

	// Read remaining capacity.
	ret |= _interface->read_word(BATT_SMBUS_RELATIVE_SOC, result);
	data.remaining = (भिन्न)result * 0.01f;

	// Read remaining capacity.
	ret |= _interface->read_word(BATT_SMBUS_REMAINING_CAPACITY, result);
	data.discharged_mah = _batt_startup_capacity - result;

	// Read full capacity.
	ret |= _interface->read_word(BATT_SMBUS_FULL_CHARGE_CAPACITY, result);
	data.capacity = result;

	// Read cycle count.
	ret |= _interface->read_word(BATT_SMBUS_CYCLE_मोजणी, result);
	data.cycle_मोजणी_2 = result;

	// Read serial number.
	ret |= _interface->read_word(BATT_SMBUS_SERIAL_संख्या, result);
	snprintf(battery_info.serial_संख्या_2, परिमाण(battery_info.serial_संख्या_2), "%" PRIu16, result);

	// Read battery temperature and covert to Celsius.
	ret |= _interface->read_word(BATT_SMBUS_TEMP, result);
	data.temperature = ((भिन्न)result * 0.1f) + atmosphere::kAbsoluteNullCelsius;

	प्रत्यागम ret;

}

template<वर्ग T>
शून्य SMBUS_SBS_BaseClass<T>::RunImpl()
{

	// Get the current time.
	uint64_t now = hrt_absolute_time();

	// Read data from sensor.
	battery_status_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = now;

	new_report.connected = सत्य;

	battery_info_s battery_info{};
	battery_info.timestamp = now;
	battery_info.id = new_report.id;

	पूर्णाङ्क ret = populate_smbus_data(new_report, battery_info);

	new_report.cell_मोजणी = cell_मोजणी_2;

	// Only publish if no errors.
	यदि (!ret) {
		orb_publish(ORB_ID(battery_status), _batt_topic, &new_report);
		orb_publish(ORB_ID(battery_info), _battery_info_topic, &battery_info);
	}
}
