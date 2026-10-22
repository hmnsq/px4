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

#هدايت once

#ضمن <px4_platform_common/i2c_spi_buses.h>
#ضمن <lib/drivers/smbus/SMBus.hpp>
#ضمن <uORB/topics/battery_info.h>
#ضمن <uORB/topics/battery_status.h>
#ضمن <px4_platform_common/param.h>
#ضمن <lib/atmosphere/atmosphere.h>

استعمال نطاق وقت_literals;

#وصف_ڪريو SBS_MEASUREMENT_INTERVAL_US             	100_ms          ///< time in microseconds, measure at 10Hz (not part of SBS spec)

سانچو<صنف T>
صنف SMBUS_SBS_بنياد_Class : عام I2CSPIDriver<T>
{
عام:
	SMBUS_SBS_بنياد_Class(ثابت I2CSPIDriverConfig &config, SMBus *interface);
	SMBUS_SBS_بنياد_Class();

	~SMBUS_SBS_بنياد_Class();

	ساكن فراغ print_usage();

	دوست SMBus;

	صحيح populate_smbus_معلومات(battery_حالت_s &نياپو, battery_info_s &battery_info);

	مجازي فراغ لاڳيتو_Impl(); // Can be overridden by derived implimentation

	مجازي فراغ custom_طريقو(ثابت BusCLIArguments &cli) = 0; //Has be overriden by derived implimentation

	/**
	 * @brief Returns the SBS serial number of the battery device.
	 * @return Returns the SBS serial number of the battery device.
	 */
	uint16_t get_serial_عدد();

	/**
	* @brief Read info from battery on startup.
	* @return Returns PX4_OK on success, PX4_ERROR on failure. Can be overridden by derived implimentation
	*/
	مجازي صحيح get_startup_info();

	/**
	 * @brief Gets the SBS manufacture date of the battery.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	صحيح manufacture_date();

	/**
	 * @brief Gets the SBS manufacturer name of the battery device.
	 * @param manufacturer_name Pointer to a buffer into which the manufacturer name is to be written.
	 * @param max_length The maximum number of bytes to attempt to read from the manufacturer name register,
	 *                   including the null character that is appended to the end.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	صحيح manufacturer_name(uint8_t *manufacturer_name, ثابت uint8_t ڊيگهائي);

	/**
	 * @brief Enables or disables the cell under voltage protection emergency shut off.
	 */

	فراغ suspend();

	فراغ resume();
	تعداد {
		BATT_SMBUS_عارضي                         =	0x08,           ///< temperature register
		BATT_SMBUS_VOLTAGE                      =       0x09,           ///< voltage register
		BATT_SMBUS_CURRENT                      =       0x0A,           ///< current register
		BATT_SMBUS_اوسط_CURRENT              =       0x0B,           ///< average current register
		BATT_SMBUS_MAX_ERROR                    =       0x0C,           ///< max error
		BATT_SMBUS_RELATIVE_SOC                 =       0x0D,           ///< Relative State Of Charge
		BATT_SMBUS_ABSOLUTE_SOC                 =       0x0E,           ///< Absolute State of charge
		BATT_SMBUS_بچيل_صلاحيت           =       0x0F,           ///< predicted remaining battery capacity as a percentage
		BATT_SMBUS_ڀريل_CHARGE_صلاحيت         =       0x10,           ///< capacity when fully charged
		BATT_SMBUS_لاڳيتو_وقت_منزل_خالي_جايون            =       0x11,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_اوسط_وقت_منزل_خالي_جايون        =       0x12,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_چڪر_ڳڻپ                  =       0x17,           ///< number of cycles the battery has experienced
		BATT_SMBUS_DESIGN_صلاحيت              =       0x18,           ///< design capacity register
		BATT_SMBUS_DESIGN_VOLTAGE               =       0x19,           ///< design voltage register
		BATT_SMBUS_MANUFACTURER_NAME            =       0x20,           ///< manufacturer name
		BATT_SMBUS_MANUFACTURER_NAME_سائيز       =       21,             ///< manufacturer name data size (not part of SBS spec)
		BATT_SMBUS_DEVICE_NAME			=	0x21,		///< character string that contains the battery's name
		BATT_SMBUS_MANUFACTURE_DATE             =       0x1B,           ///< manufacture date register
		BATT_SMBUS_SERIAL_عدد                =       0x1C,           ///< serial number register
		BATT_SMBUS_MANUFACTURER_ACCESS          =       0x00,
		BATT_SMBUS_MANUFACTURER_معلومات            =       0x23
	} SBS_REGISTERS;

محمي:

	SMBus *_interface;

	perf_counter_t چڪر{perf_alloc(PC_ELAPSED, "batmon_cycle")}; // TODO

	orb_advert_t _battery_info_topic{خالي_اشارو};
	orb_advert_t _batt_topic{خالي_اشارو};

	/** @param _cell_count Number of series cell (retrieved from cell_count PX4 params) */
	uint8_t cell_ڳڻپ_2{0};

	/** @param _batt_capacity Battery design capacity in mAh (0 means unknown). */
	uint16_t batt_صلاحيت{0};

	/** @param _batt_startup_capacity Battery remaining capacity in mAh on startup. */
	uint16_t batt_startup_صلاحيت{0};

	/** @param _cycle_count The number of cycles the battery has experienced. */
	uint16_t چڪر_ڳڻپ{0};

	/** @param _serial_number Serial number register. */
	uint16_t serial_عدد{0};

	/** @param _manufacturer_name Name of the battery manufacturer. */
	حرف _manufacturer_name[BATT_SMBUS_MANUFACTURER_NAME_سائيز + 1] {};	// Plus one for terminator

	/** @param _manufacture_date Date of the battery manufacturing. */
	uint16_t _manufacture_date{0};

	/** @param _state_of_health state of health as read on connection  */
	عائم state_جو_health{0.f}; // Not part of SBS, can be calculated from FullChargeCapacity
	// and DesignCapacity

	فراغ print_man_info();

};

سانچو<صنف T>
SMBUS_SBS_بنياد_Class<T>::SMBUS_SBS_بنياد_Class(ثابت I2CSPIDriverConfig &config, SMBus *interface):
	I2CSPIDriver<T>(config),
	_interface(interface)
{
	battery_info_s battery_info{};
	battery_حالت_s new_report = {};
	صحيح SBS_instance_عدد = 0;
	_battery_info_topic = orb_advertise_multi(ORB_ID(battery_info), &battery_info, &SBS_instance_عدد);
	_batt_topic = orb_advertise_multi(ORB_ID(battery_حالت), &new_report, &SBS_instance_عدد);
	_interface->init();
}

سانچو<صنف T>
SMBUS_SBS_بنياد_Class<T>::~SMBUS_SBS_بنياد_Class()
{
	orb_unadvertise(_batt_topic);
	perf_free(چڪر); // TODO

	إذا (_interface != خالي_اشارو) {
		ميٽيو _interface;
	}

}

سانچو<صنف T>
فراغ SMBUS_SBS_بنياد_Class<T>::suspend()
{
	هذا->وقت_جدول_Clear();
}

سانچو<صنف T>
فراغ SMBUS_SBS_بنياد_Class<T>::resume()
{
	هذا->وقت_جدول_On_Interval(SBS_MEASUREMENT_INTERVAL_US);
}

سانچو<صنف T>
uint16_t SMBUS_SBS_بنياد_Class<T>::get_serial_عدد()
{
	uint16_t serial_انگ = 0;

	إذا (_interface->read_لفظ(BATT_SMBUS_SERIAL_عدد, serial_انگ) == PX4_OK) {
		أرجع serial_انگ;
	}

	أرجع PX4_ERROR;
}

سانچو<صنف T>
صحيح SMBUS_SBS_بنياد_Class<T>::get_startup_info()
{
	صحيح ret = PX4_OK;

	ret |= _interface->بلاڪ_read(BATT_SMBUS_MANUFACTURER_NAME, _manufacturer_name, BATT_SMBUS_MANUFACTURER_NAME_سائيز,
				      سچ);
	_manufacturer_name[حجم(_manufacturer_name) - 1] = '\0';

	uint16_t serial_انگ;
	ret |= _interface->read_لفظ(BATT_SMBUS_SERIAL_عدد, serial_انگ);

	uint16_t بچيل_cap;
	ret |= _interface->read_لفظ(BATT_SMBUS_بچيل_صلاحيت, بچيل_cap);

	uint16_t چڪر_ڳڻپ_2;
	ret |= _interface->read_لفظ(BATT_SMBUS_چڪر_ڳڻپ, چڪر_ڳڻپ_2);

	uint16_t ڀريل_cap;
	ret |= _interface->read_لفظ(BATT_SMBUS_ڀريل_CHARGE_صلاحيت, ڀريل_cap);

	uint16_t manufacture_date;
	ret |= _interface->read_لفظ(BATT_SMBUS_MANUFACTURE_DATE, manufacture_date);

	إذا (!ret) {
		serial_عدد = serial_انگ;
		batt_startup_صلاحيت = (uint16_t)((عائم)بچيل_cap);
		چڪر_ڳڻپ = چڪر_ڳڻپ_2;
		batt_صلاحيت = (uint16_t)((عائم)ڀريل_cap);
		_manufacture_date = manufacture_date;
	}

	أرجع ret;
}

سانچو<صنف T>
صحيح SMBUS_SBS_بنياد_Class<T>::populate_smbus_معلومات(battery_حالت_s &معلومات, battery_info_s &battery_info)
{

	// Temporary variable for storing SMBUS reads.
	uint16_t نتيجو;

	صحيح ret = _interface->read_لفظ(BATT_SMBUS_VOLTAGE, نتيجو);

	// Convert millivolts to volts.
	معلومات.voltage_v = ((عائم)نتيجو) * 0.001f;

	// Read current.
	ret |= _interface->read_لفظ(BATT_SMBUS_CURRENT, نتيجو);

	معلومات.current_a = (-1.0f * ((عائم)(*(int16_t *)&نتيجو)) * 0.001f);

	// Read remaining capacity.
	ret |= _interface->read_لفظ(BATT_SMBUS_RELATIVE_SOC, نتيجو);
	معلومات.بچيل = (عائم)نتيجو * 0.01f;

	// Read remaining capacity.
	ret |= _interface->read_لفظ(BATT_SMBUS_بچيل_صلاحيت, نتيجو);
	معلومات.discharged_mah = batt_startup_صلاحيت - نتيجو;

	// Read full capacity.
	ret |= _interface->read_لفظ(BATT_SMBUS_ڀريل_CHARGE_صلاحيت, نتيجو);
	معلومات.صلاحيت = نتيجو;

	// Read cycle count.
	ret |= _interface->read_لفظ(BATT_SMBUS_چڪر_ڳڻپ, نتيجو);
	معلومات.چڪر_ڳڻپ_2 = نتيجو;

	// Read serial number.
	ret |= _interface->read_لفظ(BATT_SMBUS_SERIAL_عدد, نتيجو);
	snprintf(battery_info.serial_عدد_2, حجم(battery_info.serial_عدد_2), "%" PRIu16, نتيجو);

	// Read battery temperature and covert to Celsius.
	ret |= _interface->read_لفظ(BATT_SMBUS_عارضي, نتيجو);
	معلومات.temperature = ((عائم)نتيجو * 0.1f) + atmosphere::kAbsoluteNullCelsius;

	أرجع ret;

}

سانچو<صنف T>
فراغ SMBUS_SBS_بنياد_Class<T>::لاڳيتو_Impl()
{

	// Get the current time.
	uint64_t now = hrt_absolute_وقت();

	// Read data from sensor.
	battery_حالت_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = now;

	new_report.connected = سچ;

	battery_info_s battery_info{};
	battery_info.timestamp = now;
	battery_info.id = new_report.id;

	صحيح ret = populate_smbus_معلومات(new_report, battery_info);

	new_report.cell_ڳڻپ = cell_ڳڻپ_2;

	// Only publish if no errors.
	إذا (!ret) {
		orb_publish(ORB_ID(battery_حالت), _batt_topic, &new_report);
		orb_publish(ORB_ID(battery_info), _battery_info_topic, &battery_info);
	}
}
