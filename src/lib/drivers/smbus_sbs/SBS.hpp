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

#సూచన once

#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>
#అన్తర్భావ <lib/drivers/smbus/SMBus.hpp>
#అన్తర్భావ <uORB/topics/battery_info.h>
#అన్తర్భావ <uORB/topics/battery_status.h>
#అన్తర్భావ <px4_platform_common/param.h>
#అన్తర్భావ <lib/atmosphere/atmosphere.h>

ఉపయోగించి నామాకాశ సమయం_literals;

#నిర్వచించు SBS_MEASUREMENT_INTERVAL_US             	100_ms          ///< time in microseconds, measure at 10Hz (not part of SBS spec)

మూస<తరగతి T>
తరగతి SMBUS_SBS_పునాది_Class : బహిరంగ I2CSPIDriver<T>
{
బహిరంగ:
	SMBUS_SBS_పునాది_Class(స్థిరరాశి I2CSPIDriverConfig &config, SMBus *interface);
	SMBUS_SBS_పునాది_Class();

	~SMBUS_SBS_పునాది_Class();

	స్థిర శూన్య print_usage();

	మిత్ర SMBus;

	పూర్ణాంకం populate_smbus_దత్తాంశం(battery_స్థితి_s &సందేశం_2, battery_info_s &battery_info);

	మాయా శూన్య నిరంతరం_Impl(); // Can be overridden by derived implimentation

	మాయా శూన్య custom_పద్ధతి(స్థిరరాశి BusCLIArguments &cli) = 0; //Has be overriden by derived implimentation

	/**
	 * @brief Returns the SBS serial number of the battery device.
	 * @return Returns the SBS serial number of the battery device.
	 */
	uint16_t get_serial_సంఖ్య();

	/**
	* @brief Read info from battery on startup.
	* @return Returns PX4_OK on success, PX4_ERROR on failure. Can be overridden by derived implimentation
	*/
	మాయా పూర్ణాంకం get_startup_info();

	/**
	 * @brief Gets the SBS manufacture date of the battery.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	పూర్ణాంకం manufacture_date();

	/**
	 * @brief Gets the SBS manufacturer name of the battery device.
	 * @param manufacturer_name Pointer to a buffer into which the manufacturer name is to be written.
	 * @param max_length The maximum number of bytes to attempt to read from the manufacturer name register,
	 *                   including the null character that is appended to the end.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	పూర్ణాంకం manufacturer_name(uint8_t *manufacturer_name, స్థిరరాశి uint8_t నిడివి);

	/**
	 * @brief Enables or disables the cell under voltage protection emergency shut off.
	 */

	శూన్య suspend();

	శూన్య resume();
	గణన {
		BATT_SMBUS_తాత్కాలిక                         =	0x08,           ///< temperature register
		BATT_SMBUS_VOLTAGE                      =       0x09,           ///< voltage register
		BATT_SMBUS_CURRENT                      =       0x0A,           ///< current register
		BATT_SMBUS_సరాసరి_CURRENT              =       0x0B,           ///< average current register
		BATT_SMBUS_MAX_ERROR                    =       0x0C,           ///< max error
		BATT_SMBUS_RELATIVE_SOC                 =       0x0D,           ///< Relative State Of Charge
		BATT_SMBUS_ABSOLUTE_SOC                 =       0x0E,           ///< Absolute State of charge
		BATT_SMBUS_మిగిలిన_సామర్థ్యం           =       0x0F,           ///< predicted remaining battery capacity as a percentage
		BATT_SMBUS_నిండిన_CHARGE_సామర్థ్యం         =       0x10,           ///< capacity when fully charged
		BATT_SMBUS_నిరంతరం_సమయం_గమ్యం_ఖాళీలు            =       0x11,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_సరాసరి_సమయం_గమ్యం_ఖాళీలు        =       0x12,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_చక్రం_లెక్క                  =       0x17,           ///< number of cycles the battery has experienced
		BATT_SMBUS_DESIGN_సామర్థ్యం              =       0x18,           ///< design capacity register
		BATT_SMBUS_DESIGN_VOLTAGE               =       0x19,           ///< design voltage register
		BATT_SMBUS_MANUFACTURER_NAME            =       0x20,           ///< manufacturer name
		BATT_SMBUS_MANUFACTURER_NAME_కొలత       =       21,             ///< manufacturer name data size (not part of SBS spec)
		BATT_SMBUS_DEVICE_NAME			=	0x21,		///< character string that contains the battery's name
		BATT_SMBUS_MANUFACTURE_DATE             =       0x1B,           ///< manufacture date register
		BATT_SMBUS_SERIAL_సంఖ్య                =       0x1C,           ///< serial number register
		BATT_SMBUS_MANUFACTURER_ACCESS          =       0x00,
		BATT_SMBUS_MANUFACTURER_దత్తాంశం            =       0x23
	} SBS_REGISTERS;

రక్షిత:

	SMBus *_interface;

	perf_counter_t చక్రం{perf_alloc(PC_ELAPSED, "batmon_cycle")}; // TODO

	orb_advert_t _battery_info_topic{శూన్యసూచిక};
	orb_advert_t _batt_topic{శూన్యసూచిక};

	/** @param _cell_count Number of series cell (retrieved from cell_count PX4 params) */
	uint8_t cell_లెక్క_2{0};

	/** @param _batt_capacity Battery design capacity in mAh (0 means unknown). */
	uint16_t batt_సామర్థ్యం{0};

	/** @param _batt_startup_capacity Battery remaining capacity in mAh on startup. */
	uint16_t batt_startup_సామర్థ్యం{0};

	/** @param _cycle_count The number of cycles the battery has experienced. */
	uint16_t చక్రం_లెక్క{0};

	/** @param _serial_number Serial number register. */
	uint16_t serial_సంఖ్య{0};

	/** @param _manufacturer_name Name of the battery manufacturer. */
	అక్షరం _manufacturer_name[BATT_SMBUS_MANUFACTURER_NAME_కొలత + 1] {};	// Plus one for terminator

	/** @param _manufacture_date Date of the battery manufacturing. */
	uint16_t _manufacture_date{0};

	/** @param _state_of_health state of health as read on connection  */
	దశాంశం state_యొక్క_health{0.f}; // Not part of SBS, can be calculated from FullChargeCapacity
	// and DesignCapacity

	శూన్య print_man_info();

};

మూస<తరగతి T>
SMBUS_SBS_పునాది_Class<T>::SMBUS_SBS_పునాది_Class(స్థిరరాశి I2CSPIDriverConfig &config, SMBus *interface):
	I2CSPIDriver<T>(config),
	_interface(interface)
{
	battery_info_s battery_info{};
	battery_స్థితి_s new_report = {};
	పూర్ణాంకం SBS_instance_సంఖ్య = 0;
	_battery_info_topic = orb_advertise_multi(ORB_ID(battery_info), &battery_info, &SBS_instance_సంఖ్య);
	_batt_topic = orb_advertise_multi(ORB_ID(battery_స్థితి), &new_report, &SBS_instance_సంఖ్య);
	_interface->init();
}

మూస<తరగతి T>
SMBUS_SBS_పునాది_Class<T>::~SMBUS_SBS_పునాది_Class()
{
	orb_unadvertise(_batt_topic);
	perf_free(చక్రం); // TODO

	ఒకవేళ (_interface != శూన్యసూచిక) {
		తొలగించు _interface;
	}

}

మూస<తరగతి T>
శూన్య SMBUS_SBS_పునాది_Class<T>::suspend()
{
	ఇది->కాలపట్టిక_Clear();
}

మూస<తరగతి T>
శూన్య SMBUS_SBS_పునాది_Class<T>::resume()
{
	ఇది->కాలపట్టిక_On_Interval(SBS_MEASUREMENT_INTERVAL_US);
}

మూస<తరగతి T>
uint16_t SMBUS_SBS_పునాది_Class<T>::get_serial_సంఖ్య()
{
	uint16_t serial_అంకె = 0;

	ఒకవేళ (_interface->read_మాట(BATT_SMBUS_SERIAL_సంఖ్య, serial_అంకె) == PX4_OK) {
		తిరిగి serial_అంకె;
	}

	తిరిగి PX4_ERROR;
}

మూస<తరగతి T>
పూర్ణాంకం SMBUS_SBS_పునాది_Class<T>::get_startup_info()
{
	పూర్ణాంకం ret = PX4_OK;

	ret |= _interface->ఖండం_read(BATT_SMBUS_MANUFACTURER_NAME, _manufacturer_name, BATT_SMBUS_MANUFACTURER_NAME_కొలత,
				      నిజం);
	_manufacturer_name[పరిమాణం(_manufacturer_name) - 1] = '\0';

	uint16_t serial_అంకె;
	ret |= _interface->read_మాట(BATT_SMBUS_SERIAL_సంఖ్య, serial_అంకె);

	uint16_t మిగిలిన_cap;
	ret |= _interface->read_మాట(BATT_SMBUS_మిగిలిన_సామర్థ్యం, మిగిలిన_cap);

	uint16_t చక్రం_లెక్క_2;
	ret |= _interface->read_మాట(BATT_SMBUS_చక్రం_లెక్క, చక్రం_లెక్క_2);

	uint16_t నిండిన_cap;
	ret |= _interface->read_మాట(BATT_SMBUS_నిండిన_CHARGE_సామర్థ్యం, నిండిన_cap);

	uint16_t manufacture_date;
	ret |= _interface->read_మాట(BATT_SMBUS_MANUFACTURE_DATE, manufacture_date);

	ఒకవేళ (!ret) {
		serial_సంఖ్య = serial_అంకె;
		batt_startup_సామర్థ్యం = (uint16_t)((దశాంశం)మిగిలిన_cap);
		చక్రం_లెక్క = చక్రం_లెక్క_2;
		batt_సామర్థ్యం = (uint16_t)((దశాంశం)నిండిన_cap);
		_manufacture_date = manufacture_date;
	}

	తిరిగి ret;
}

మూస<తరగతి T>
పూర్ణాంకం SMBUS_SBS_పునాది_Class<T>::populate_smbus_దత్తాంశం(battery_స్థితి_s &దత్తాంశం, battery_info_s &battery_info)
{

	// Temporary variable for storing SMBUS reads.
	uint16_t ఫలితం;

	పూర్ణాంకం ret = _interface->read_మాట(BATT_SMBUS_VOLTAGE, ఫలితం);

	// Convert millivolts to volts.
	దత్తాంశం.voltage_v = ((దశాంశం)ఫలితం) * 0.001f;

	// Read current.
	ret |= _interface->read_మాట(BATT_SMBUS_CURRENT, ఫలితం);

	దత్తాంశం.current_a = (-1.0f * ((దశాంశం)(*(int16_t *)&ఫలితం)) * 0.001f);

	// Read remaining capacity.
	ret |= _interface->read_మాట(BATT_SMBUS_RELATIVE_SOC, ఫలితం);
	దత్తాంశం.మిగిలిన = (దశాంశం)ఫలితం * 0.01f;

	// Read remaining capacity.
	ret |= _interface->read_మాట(BATT_SMBUS_మిగిలిన_సామర్థ్యం, ఫలితం);
	దత్తాంశం.discharged_mah = batt_startup_సామర్థ్యం - ఫలితం;

	// Read full capacity.
	ret |= _interface->read_మాట(BATT_SMBUS_నిండిన_CHARGE_సామర్థ్యం, ఫలితం);
	దత్తాంశం.సామర్థ్యం = ఫలితం;

	// Read cycle count.
	ret |= _interface->read_మాట(BATT_SMBUS_చక్రం_లెక్క, ఫలితం);
	దత్తాంశం.చక్రం_లెక్క_2 = ఫలితం;

	// Read serial number.
	ret |= _interface->read_మాట(BATT_SMBUS_SERIAL_సంఖ్య, ఫలితం);
	snprintf(battery_info.serial_సంఖ్య_2, పరిమాణం(battery_info.serial_సంఖ్య_2), "%" PRIu16, ఫలితం);

	// Read battery temperature and covert to Celsius.
	ret |= _interface->read_మాట(BATT_SMBUS_తాత్కాలిక, ఫలితం);
	దత్తాంశం.temperature = ((దశాంశం)ఫలితం * 0.1f) + atmosphere::kAbsoluteNullCelsius;

	తిరిగి ret;

}

మూస<తరగతి T>
శూన్య SMBUS_SBS_పునాది_Class<T>::నిరంతరం_Impl()
{

	// Get the current time.
	uint64_t now = hrt_absolute_సమయం();

	// Read data from sensor.
	battery_స్థితి_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = now;

	new_report.connected = నిజం;

	battery_info_s battery_info{};
	battery_info.timestamp = now;
	battery_info.id = new_report.id;

	పూర్ణాంకం ret = populate_smbus_దత్తాంశం(new_report, battery_info);

	new_report.cell_లెక్క = cell_లెక్క_2;

	// Only publish if no errors.
	ఒకవేళ (!ret) {
		orb_publish(ORB_ID(battery_స్థితి), _batt_topic, &new_report);
		orb_publish(ORB_ID(battery_info), _battery_info_topic, &battery_info);
	}
}
