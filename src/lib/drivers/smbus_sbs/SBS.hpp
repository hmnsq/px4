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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c_spi_buses.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/drivers/smbus/SMBus.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/battery_info.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/battery_status.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/atmosphere/atmosphere.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBS_MEASUREMENT_INTERVAL_US             	100_ms          ///< time in microseconds, measure at 10Hz (not part of SBS spec)

ਸਾਂਚਾ<ਵਰਗ T>
ਵਰਗ SMBUS_SBS_ਅਧਾਰ_Class : ਜਨਤਕ I2CSPIDriver<T>
{
ਜਨਤਕ:
	SMBUS_SBS_ਅਧਾਰ_Class(ਅਚਲ I2CSPIDriverConfig &config, SMBus *interface);
	SMBUS_SBS_ਅਧਾਰ_Class();

	~SMBUS_SBS_ਅਧਾਰ_Class();

	ਸਥਿਰ ਖਾਲੀ print_usage();

	ਮਿੱਤਰ SMBus;

	ਪੂਰਨ populate_smbus_ਅੰਕੜੇ(battery_ਸਥਿਤੀ_s &ਸੁਨੇਹਾ_2, battery_info_s &battery_info);

	ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_Impl(); // Can be overridden by derived implimentation

	ਆਭਾਸੀ ਖਾਲੀ custom_ਵਿਧੀ(ਅਚਲ BusCLIArguments &cli) = 0; //Has be overriden by derived implimentation

	/**
	 * @brief Returns the SBS serial number of the battery device.
	 * @return Returns the SBS serial number of the battery device.
	 */
	uint16_t get_serial_ਸੰਖਿਆ();

	/**
	* @brief Read info from battery on startup.
	* @return Returns PX4_OK on success, PX4_ERROR on failure. Can be overridden by derived implimentation
	*/
	ਆਭਾਸੀ ਪੂਰਨ get_startup_info();

	/**
	 * @brief Gets the SBS manufacture date of the battery.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	ਪੂਰਨ manufacture_date();

	/**
	 * @brief Gets the SBS manufacturer name of the battery device.
	 * @param manufacturer_name Pointer to a buffer into which the manufacturer name is to be written.
	 * @param max_length The maximum number of bytes to attempt to read from the manufacturer name register,
	 *                   including the null character that is appended to the end.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	ਪੂਰਨ manufacturer_name(uint8_t *manufacturer_name, ਅਚਲ uint8_t ਲੰਮਾਈ);

	/**
	 * @brief Enables or disables the cell under voltage protection emergency shut off.
	 */

	ਖਾਲੀ suspend();

	ਖਾਲੀ resume();
	ਗਿਣਤੀ {
		BATT_SMBUS_ਆਰਜ਼ੀ                         =	0x08,           ///< temperature register
		BATT_SMBUS_VOLTAGE                      =       0x09,           ///< voltage register
		BATT_SMBUS_CURRENT                      =       0x0A,           ///< current register
		BATT_SMBUS_ਔਸਤਨ_CURRENT              =       0x0B,           ///< average current register
		BATT_SMBUS_MAX_ERROR                    =       0x0C,           ///< max error
		BATT_SMBUS_RELATIVE_SOC                 =       0x0D,           ///< Relative State Of Charge
		BATT_SMBUS_ABSOLUTE_SOC                 =       0x0E,           ///< Absolute State of charge
		BATT_SMBUS_ਬਚਿਆ_ਸਮਰੱਥਾ           =       0x0F,           ///< predicted remaining battery capacity as a percentage
		BATT_SMBUS_ਭਰਿਆ_CHARGE_ਸਮਰੱਥਾ         =       0x10,           ///< capacity when fully charged
		BATT_SMBUS_ਲੜੀਵਾਰ_ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ            =       0x11,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_ਔਸਤਨ_ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ        =       0x12,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_ਚੱਕਰ_ਗਿਣਤੀ                  =       0x17,           ///< number of cycles the battery has experienced
		BATT_SMBUS_DESIGN_ਸਮਰੱਥਾ              =       0x18,           ///< design capacity register
		BATT_SMBUS_DESIGN_VOLTAGE               =       0x19,           ///< design voltage register
		BATT_SMBUS_MANUFACTURER_NAME            =       0x20,           ///< manufacturer name
		BATT_SMBUS_MANUFACTURER_NAME_ਮਾਪ       =       21,             ///< manufacturer name data size (not part of SBS spec)
		BATT_SMBUS_DEVICE_NAME			=	0x21,		///< character string that contains the battery's name
		BATT_SMBUS_MANUFACTURE_DATE             =       0x1B,           ///< manufacture date register
		BATT_SMBUS_SERIAL_ਸੰਖਿਆ                =       0x1C,           ///< serial number register
		BATT_SMBUS_MANUFACTURER_ACCESS          =       0x00,
		BATT_SMBUS_MANUFACTURER_ਅੰਕੜੇ            =       0x23
	} SBS_REGISTERS;

ਸੁਰੱਖਿਅਤ:

	SMBus *_interface;

	perf_counter_t ਚੱਕਰ{perf_alloc(PC_ELAPSED, "batmon_cycle")}; // TODO

	orb_advert_t _battery_info_topic{ਖਾਲੀ_ਸੂਚਕ};
	orb_advert_t _batt_topic{ਖਾਲੀ_ਸੂਚਕ};

	/** @param _cell_count Number of series cell (retrieved from cell_count PX4 params) */
	uint8_t cell_ਗਿਣਤੀ_2{0};

	/** @param _batt_capacity Battery design capacity in mAh (0 means unknown). */
	uint16_t batt_ਸਮਰੱਥਾ{0};

	/** @param _batt_startup_capacity Battery remaining capacity in mAh on startup. */
	uint16_t batt_startup_ਸਮਰੱਥਾ{0};

	/** @param _cycle_count The number of cycles the battery has experienced. */
	uint16_t ਚੱਕਰ_ਗਿਣਤੀ{0};

	/** @param _serial_number Serial number register. */
	uint16_t serial_ਸੰਖਿਆ{0};

	/** @param _manufacturer_name Name of the battery manufacturer. */
	ਅੱਖਰ _manufacturer_name[BATT_SMBUS_MANUFACTURER_NAME_ਮਾਪ + 1] {};	// Plus one for terminator

	/** @param _manufacture_date Date of the battery manufacturing. */
	uint16_t _manufacture_date{0};

	/** @param _state_of_health state of health as read on connection  */
	ਦਸ਼ਮਲਵ state_ਦਾ_health{0.f}; // Not part of SBS, can be calculated from FullChargeCapacity
	// and DesignCapacity

	ਖਾਲੀ print_man_info();

};

ਸਾਂਚਾ<ਵਰਗ T>
SMBUS_SBS_ਅਧਾਰ_Class<T>::SMBUS_SBS_ਅਧਾਰ_Class(ਅਚਲ I2CSPIDriverConfig &config, SMBus *interface):
	I2CSPIDriver<T>(config),
	_interface(interface)
{
	battery_info_s battery_info{};
	battery_ਸਥਿਤੀ_s new_report = {};
	ਪੂਰਨ SBS_instance_ਸੰਖਿਆ = 0;
	_battery_info_topic = orb_advertise_multi(ORB_ID(battery_info), &battery_info, &SBS_instance_ਸੰਖਿਆ);
	_batt_topic = orb_advertise_multi(ORB_ID(battery_ਸਥਿਤੀ), &new_report, &SBS_instance_ਸੰਖਿਆ);
	_interface->init();
}

ਸਾਂਚਾ<ਵਰਗ T>
SMBUS_SBS_ਅਧਾਰ_Class<T>::~SMBUS_SBS_ਅਧਾਰ_Class()
{
	orb_unadvertise(_batt_topic);
	perf_free(ਚੱਕਰ); // TODO

	ਜੇ (_interface != ਖਾਲੀ_ਸੂਚਕ) {
		ਮਿਟਾਓ _interface;
	}

}

ਸਾਂਚਾ<ਵਰਗ T>
ਖਾਲੀ SMBUS_SBS_ਅਧਾਰ_Class<T>::suspend()
{
	ਇਹ->ਸਮਾਂ_ਸਾਰਣੀ_Clear();
}

ਸਾਂਚਾ<ਵਰਗ T>
ਖਾਲੀ SMBUS_SBS_ਅਧਾਰ_Class<T>::resume()
{
	ਇਹ->ਸਮਾਂ_ਸਾਰਣੀ_On_Interval(SBS_MEASUREMENT_INTERVAL_US);
}

ਸਾਂਚਾ<ਵਰਗ T>
uint16_t SMBUS_SBS_ਅਧਾਰ_Class<T>::get_serial_ਸੰਖਿਆ()
{
	uint16_t serial_ਅੰਕ = 0;

	ਜੇ (_interface->read_ਸ਼ਬਦ(BATT_SMBUS_SERIAL_ਸੰਖਿਆ, serial_ਅੰਕ) == PX4_OK) {
		ਵਾਪਸ serial_ਅੰਕ;
	}

	ਵਾਪਸ PX4_ERROR;
}

ਸਾਂਚਾ<ਵਰਗ T>
ਪੂਰਨ SMBUS_SBS_ਅਧਾਰ_Class<T>::get_startup_info()
{
	ਪੂਰਨ ret = PX4_OK;

	ret |= _interface->ਖੰਡ_read(BATT_SMBUS_MANUFACTURER_NAME, _manufacturer_name, BATT_SMBUS_MANUFACTURER_NAME_ਮਾਪ,
				      ਸੱਚ);
	_manufacturer_name[ਆਕਾਰ(_manufacturer_name) - 1] = '\0';

	uint16_t serial_ਅੰਕ;
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_SERIAL_ਸੰਖਿਆ, serial_ਅੰਕ);

	uint16_t ਬਚਿਆ_cap;
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਬਚਿਆ_ਸਮਰੱਥਾ, ਬਚਿਆ_cap);

	uint16_t ਚੱਕਰ_ਗਿਣਤੀ_2;
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਚੱਕਰ_ਗਿਣਤੀ, ਚੱਕਰ_ਗਿਣਤੀ_2);

	uint16_t ਭਰਿਆ_cap;
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਭਰਿਆ_CHARGE_ਸਮਰੱਥਾ, ਭਰਿਆ_cap);

	uint16_t manufacture_date;
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_MANUFACTURE_DATE, manufacture_date);

	ਜੇ (!ret) {
		serial_ਸੰਖਿਆ = serial_ਅੰਕ;
		batt_startup_ਸਮਰੱਥਾ = (uint16_t)((ਦਸ਼ਮਲਵ)ਬਚਿਆ_cap);
		ਚੱਕਰ_ਗਿਣਤੀ = ਚੱਕਰ_ਗਿਣਤੀ_2;
		batt_ਸਮਰੱਥਾ = (uint16_t)((ਦਸ਼ਮਲਵ)ਭਰਿਆ_cap);
		_manufacture_date = manufacture_date;
	}

	ਵਾਪਸ ret;
}

ਸਾਂਚਾ<ਵਰਗ T>
ਪੂਰਨ SMBUS_SBS_ਅਧਾਰ_Class<T>::populate_smbus_ਅੰਕੜੇ(battery_ਸਥਿਤੀ_s &ਅੰਕੜੇ, battery_info_s &battery_info)
{

	// Temporary variable for storing SMBUS reads.
	uint16_t ਨਤੀਜਾ;

	ਪੂਰਨ ret = _interface->read_ਸ਼ਬਦ(BATT_SMBUS_VOLTAGE, ਨਤੀਜਾ);

	// Convert millivolts to volts.
	ਅੰਕੜੇ.voltage_v = ((ਦਸ਼ਮਲਵ)ਨਤੀਜਾ) * 0.001f;

	// Read current.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_CURRENT, ਨਤੀਜਾ);

	ਅੰਕੜੇ.current_a = (-1.0f * ((ਦਸ਼ਮਲਵ)(*(int16_t *)&ਨਤੀਜਾ)) * 0.001f);

	// Read remaining capacity.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_RELATIVE_SOC, ਨਤੀਜਾ);
	ਅੰਕੜੇ.ਬਚਿਆ = (ਦਸ਼ਮਲਵ)ਨਤੀਜਾ * 0.01f;

	// Read remaining capacity.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਬਚਿਆ_ਸਮਰੱਥਾ, ਨਤੀਜਾ);
	ਅੰਕੜੇ.discharged_mah = batt_startup_ਸਮਰੱਥਾ - ਨਤੀਜਾ;

	// Read full capacity.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਭਰਿਆ_CHARGE_ਸਮਰੱਥਾ, ਨਤੀਜਾ);
	ਅੰਕੜੇ.ਸਮਰੱਥਾ = ਨਤੀਜਾ;

	// Read cycle count.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਚੱਕਰ_ਗਿਣਤੀ, ਨਤੀਜਾ);
	ਅੰਕੜੇ.ਚੱਕਰ_ਗਿਣਤੀ_2 = ਨਤੀਜਾ;

	// Read serial number.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_SERIAL_ਸੰਖਿਆ, ਨਤੀਜਾ);
	snprintf(battery_info.serial_ਸੰਖਿਆ_2, ਆਕਾਰ(battery_info.serial_ਸੰਖਿਆ_2), "%" PRIu16, ਨਤੀਜਾ);

	// Read battery temperature and covert to Celsius.
	ret |= _interface->read_ਸ਼ਬਦ(BATT_SMBUS_ਆਰਜ਼ੀ, ਨਤੀਜਾ);
	ਅੰਕੜੇ.temperature = ((ਦਸ਼ਮਲਵ)ਨਤੀਜਾ * 0.1f) + atmosphere::kAbsoluteNullCelsius;

	ਵਾਪਸ ret;

}

ਸਾਂਚਾ<ਵਰਗ T>
ਖਾਲੀ SMBUS_SBS_ਅਧਾਰ_Class<T>::ਲੜੀਵਾਰ_Impl()
{

	// Get the current time.
	uint64_t now = hrt_absolute_ਸਮਾਂ();

	// Read data from sensor.
	battery_ਸਥਿਤੀ_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = now;

	new_report.connected = ਸੱਚ;

	battery_info_s battery_info{};
	battery_info.timestamp = now;
	battery_info.id = new_report.id;

	ਪੂਰਨ ret = populate_smbus_ਅੰਕੜੇ(new_report, battery_info);

	new_report.cell_ਗਿਣਤੀ = cell_ਗਿਣਤੀ_2;

	// Only publish if no errors.
	ਜੇ (!ret) {
		orb_publish(ORB_ID(battery_ਸਥਿਤੀ), _batt_topic, &new_report);
		orb_publish(ORB_ID(battery_info), _battery_info_topic, &battery_info);
	}
}
