/****************************************************************************
 *
 *   Copyright (c) 2012-2022 PX4 Development Team. All rights reserved.
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
 * @file Device.hpp
 *
 * Definitions for the generic base classes in the device framework.
 */

#اگر_غیر_تعریف_شدہ _DEVICE_DEVICE_HPP
#تعریف_کرو _DEVICE_DEVICE_HPP

#شامل <cstdio>

/*
 * Includes here should only cover the needs of the framework definitions.
 */
#شامل <px4_platform_common/i2c.h>
#شامل <px4_platform_common/log.h>
#شامل <px4_platform_common/px4_config.h>
#شامل <px4_platform_common/posix.h>
#شامل <px4_platform_common/spi.h>

#شامل <drivers/drv_sensor.h>

#تعریف_کرو DEVICE_LOG(FMT, ...) PX4_LOG_NAMED(_name, FMT, ##__VA_ARGS__)
#تعریف_کرو DEVICE_DEBUG(FMT, ...) PX4_LOG_NAMED_COND(_name, _debug_enabled, FMT, ##__VA_ARGS__)

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
نامخلا device
{

/**
 * Fallback sensor classification from a device id when no start-line classification exists.
 *
 * I2C/SPI: Internal bus → internal, External or Shared bus → external (Shared cannot be
 * inferred; onboard chips on a Shared bus must call set_external(false) via -I/-s).
 * UAVCAN, serial, MAVLink → external. Simulation → internal.
 */
اندرونی منطقی device_is_external(uint32_t device_id);

/**
 * Fundamental base class for all physical drivers (I2C, SPI).
 *
 * This class provides the basic driver template for I2C and SPI devices
 */
جماعت __EXPORT Device
{
عوامی:

	// no copy, assignment, move, move assignment
	Device(ثابت Device &) = حذف;
	Device &عامل=(ثابت Device &) = حذف;
	Device(Device &&) = حذف;
	Device &عامل=(Device &&) = حذف;

	/**
	 * Destructor.
	 *
	 * Public so that anonymous devices can be destroyed.
	 */
	مجازی ~Device() = طےشدہ;

	/*
	 * Direct access methods.
	 */

	/**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
	مجازی صحیح	init() { واپس PX4_OK; }

	/**
	 * Read directly from the device.
	 *
	 * The actual size of each unit quantity is device-specific.
	 *
	 * @param offset	The device address at which to start reading
	 * @param data		The buffer into which the read values should be placed.
	 * @param count		The number of items to read.
	 * @return		The number of items read on success, negative errno otherwise.
	 */
	مجازی صحیح	اقرأ(غیرعلامتی پتہ, خالی *ڈیٹا, غیرعلامتی گنتی) { واپس -ENODEV; }

	/**
	 * Write directly to the device.
	 *
	 * The actual size of each unit quantity is device-specific.
	 *
	 * @param address	The device address at which to start writing.
	 * @param data		The buffer from which values should be read.
	 * @param count		The number of items to write.
	 * @return		The number of items written on success, negative errno otherwise.
	 */
	مجازی صحیح	اكتب(غیرعلامتی پتہ, خالی *ڈیٹا, غیرعلامتی گنتی) { واپس -ENODEV; }

	/**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
	مجازی uint8_t read_reg(غیرعلامتی reg) { واپس -ENODEV; }

	/**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
	مجازی صحیح write_reg(غیرعلامتی reg, uint8_t value) { واپس -ENODEV; }

	/**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
	مجازی صحیح	ioctl(غیرعلامتی operation, غیرعلامتی &arg) { واپس -ENODEV; }

	/** Device bus types for DEVID */
	شمار DeviceBusType {
		DeviceBusType_UNKNOWN = 0,
		DeviceBusType_I2C     = 1,
		DeviceBusType_SPI     = 2,
		DeviceBusType_UAVCAN  = 3,
		DeviceBusType_SIMULATION = 4,
		DeviceBusType_SERIAL = 5,
		DeviceBusType_MAVLINK = 6,
	};

	/*
	  broken out device elements. The bitfields are used to keep
	  the overall value small enough to fit in a float accurately,
	  which makes it possible to transport over the MAVLink
	  parameter protocol without loss of information.
	 */
	ساخت DeviceStructure {
		DeviceBusType bus_type : 3;
		uint8_t bus: 5;    // which instance of the bus type
		uint8_t پتہ;   // address on the bus (eg. I2C address)
		uint8_t devtype;   // device class specific device type
	};

	اتحاد DeviceId {
		ساخت DeviceStructure devid_s;
		uint32_t devid{0};
	};

	uint32_t get_device_id() ثابت { واپس _device_id.devid; }

	/**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
	DeviceBusType get_device_bus_type() ثابت { واپس _device_id.devid_s.bus_type; }
	خالی          سیٹ_device_bus_type(DeviceBusType bus_type) { _device_id.devid_s.bus_type = bus_type; }

	ساکن ثابت حرف *get_device_bus_string(DeviceBusType bus)
	{
		انتخاب (bus) {
		حالت DeviceBusType_I2C:
			واپس "I2C";

		حالت DeviceBusType_SPI:
			واپس "SPI";

		حالت DeviceBusType_UAVCAN:
			واپس "UAVCAN";

		حالت DeviceBusType_SIMULATION:
			واپس "SIMULATION";

		حالت DeviceBusType_SERIAL:
			واپس "SERIAL";

		حالت DeviceBusType_MAVLINK:
			واپس "MAVLINK";

		حالت DeviceBusType_UNKNOWN:
		طےشدہ:
			واپس "UNKNOWN";
		}
	}

	/**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
	uint8_t get_device_bus() ثابت { واپس _device_id.devid_s.bus; }
	خالی    سیٹ_device_bus(uint8_t bus) { _device_id.devid_s.bus = bus; }

	/**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
	uint8_t	get_device_پتہ() ثابت { واپس _device_id.devid_s.پتہ; }
	خالی	سیٹ_device_پتہ(صحیح پتہ) { _device_id.devid_s.پتہ = پتہ; }

	/**
	 * Return the device type
	 *
	 * @return The device type
	 */
	uint8_t	get_device_type() ثابت { واپس _device_id.devid_s.devtype; }
	خالی	سیٹ_device_type(uint8_t devtype) { _device_id.devid_s.devtype = devtype; }

	/**
	 * Print decoded device id string to a buffer.
	 *
	 * @param buffer                        buffer to write to
	 * @param length                        buffer length
	 * @param id	                        The device id.
	 * @param return                        number of bytes written
	 */
	ساکن صحیح device_id_print_بفر(حرف *بفر_2, صحیح لمبائی_2, uint32_t id)
	{
		DeviceId dev_id{};
		dev_id.devid = id;

		صحیح عدد_written = snprintf(بفر_2, لمبائی_2, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
					   get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.پتہ);

		بفر_2[لمبائی_2 - 1] = 0; // ensure 0-termination

		واپس عدد_written;
	}

	/**
	 * Is the sensor external to the flight controller board?
	 *
	 * I2C/SPI drivers set this from -I/-s vs -X/-S. Otherwise falls back to device_is_external().
	 */
	منطقی external() ثابت { واپس _external_declared ? _external : device_is_external(_device_id.devid); }

	خالی سیٹ_external(منطقی external) { _external = external; _external_declared = سچ; }

محفوظ:
	اتحاد DeviceId	_device_id {};            	/**< device identifier information */

	منطقی		_external{جھوٹ};
	منطقی		_external_declared{جھوٹ};

	ثابت حرف	*_name{خالی_اشاریہ};		/**< driver name */
	منطقی		_debug_enabled{جھوٹ};		/**< if true, debug messages are printed */

	Device() = حذف;
	صریح Device(ثابت حرف *name) : _name(name) {}

	Device(uint8_t devtype, ثابت حرف *name, DeviceBusType bus_type, uint8_t bus, uint8_t پتہ) : _name(name)
	{
		سیٹ_device_type(devtype);
		سیٹ_device_bus_type(bus_type);
		سیٹ_device_bus(bus);
		سیٹ_device_پتہ(پتہ);
	}

};

/*
 * Inline so that no caller has to link the kernel-only drivers__device library: in a
 * protected build that would pull the kernel into the userspace image.
 */
اندرونی منطقی device_is_external(uint32_t device_id)
{
	Device::DeviceId id{};
	id.devid = device_id;

	انتخاب (id.devid_s.bus_type) {
	حالت Device::DeviceBusType_I2C:
#اگر defined(CONFIG_I2C)
		واپس px4_i2c_bus_topology(id.devid_s.bus) != BusTopology::اندرونی_2;
#ورنہ
		واپس سچ;
#اگر_ختم // CONFIG_I2C

	حالت Device::DeviceBusType_SPI:
#اگر defined(CONFIG_SPI)
		واپس px4_spi_bus_topology(id.devid_s.bus) != BusTopology::اندرونی_2;
#ورنہ
		واپس سچ;
#اگر_ختم // CONFIG_SPI

	حالت Device::DeviceBusType_SIMULATION:
		واپس جھوٹ;

	حالت Device::DeviceBusType_UAVCAN:
	حالت Device::DeviceBusType_SERIAL:
	حالت Device::DeviceBusType_MAVLINK:
	حالت Device::DeviceBusType_UNKNOWN:
		واپس سچ;
	}

	واپس سچ;
}

} // namespace device

#اگر_ختم /* _DEVICE_DEVICE_HPP */
