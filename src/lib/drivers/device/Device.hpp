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

#ifndef _DEVICE_DEVICE_HPP
#define _DEVICE_DEVICE_HPP

#ضمن <cstdio>

/*
 * Includes here should only cover the needs of the framework definitions.
 */
#ضمن <px4_platform_common/i2c.h>
#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/posix.h>
#ضمن <px4_platform_common/spi.h>

#ضمن <drivers/drv_sensor.h>

#define DEVICE_LOG(FMT, ...) PX4_LOG_NAMED(_name, FMT, ##__VA_ARGS__)
#define DEVICE_DEBUG(FMT, ...) PX4_LOG_NAMED_COND(_name, _debug_enabled, FMT, ##__VA_ARGS__)

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
نطاق device
{

/**
 * Fallback sensor classification from a device id when no start-line classification exists.
 *
 * I2C/SPI: Internal bus → internal, External or Shared bus → external (Shared cannot be
 * inferred; onboard chips on a Shared bus must call set_external(false) via -I/-s).
 * UAVCAN, serial, MAVLink → external. Simulation → internal.
 */
مضمن منطقي device_is_external(uint32_t device_id);

/**
 * Fundamental base class for all physical drivers (I2C, SPI).
 *
 * This class provides the basic driver template for I2C and SPI devices
 */
صنف __EXPORT Device
{
عام:

	// no copy, assignment, move, move assignment
	Device(ثابت Device &) = delete;
	Device &operator=(ثابت Device &) = delete;
	Device(Device &&) = delete;
	Device &operator=(Device &&) = delete;

	/**
	 * Destructor.
	 *
	 * Public so that anonymous devices can be destroyed.
	 */
	virtual ~Device() = افتراضي;

	/*
	 * Direct access methods.
	 */

	/**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
	virtual صحيح	init() { أرجع PX4_OK; }

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
	virtual صحيح	اقرأ(unsigned address, فراغ *data, unsigned count) { أرجع -ENODEV; }

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
	virtual صحيح	اكتب(unsigned address, فراغ *data, unsigned count) { أرجع -ENODEV; }

	/**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
	virtual uint8_t read_reg(unsigned reg) { أرجع -ENODEV; }

	/**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
	virtual صحيح write_reg(unsigned reg, uint8_t value) { أرجع -ENODEV; }

	/**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
	virtual صحيح	ioctl(unsigned operation, unsigned &arg) { أرجع -ENODEV; }

	/** Device bus types for DEVID */
	تعداد DeviceBusType {
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
	بنية DeviceStructure {
		DeviceBusType bus_type : 3;
		uint8_t bus: 5;    // which instance of the bus type
		uint8_t address;   // address on the bus (eg. I2C address)
		uint8_t devtype;   // device class specific device type
	};

	اتحاد DeviceId {
		بنية DeviceStructure devid_s;
		uint32_t devid{0};
	};

	uint32_t get_device_id() ثابت { أرجع _device_id.devid; }

	/**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
	DeviceBusType get_device_bus_type() ثابت { أرجع _device_id.devid_s.bus_type; }
	فراغ          set_device_bus_type(DeviceBusType bus_type) { _device_id.devid_s.bus_type = bus_type; }

	ساكن ثابت حرف *get_device_bus_string(DeviceBusType bus)
	{
		بدل (bus) {
		حالة DeviceBusType_I2C:
			أرجع "I2C";

		حالة DeviceBusType_SPI:
			أرجع "SPI";

		حالة DeviceBusType_UAVCAN:
			أرجع "UAVCAN";

		حالة DeviceBusType_SIMULATION:
			أرجع "SIMULATION";

		حالة DeviceBusType_SERIAL:
			أرجع "SERIAL";

		حالة DeviceBusType_MAVLINK:
			أرجع "MAVLINK";

		حالة DeviceBusType_UNKNOWN:
		افتراضي:
			أرجع "UNKNOWN";
		}
	}

	/**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
	uint8_t get_device_bus() ثابت { أرجع _device_id.devid_s.bus; }
	فراغ    set_device_bus(uint8_t bus) { _device_id.devid_s.bus = bus; }

	/**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
	uint8_t	get_device_address() ثابت { أرجع _device_id.devid_s.address; }
	فراغ	set_device_address(صحيح address) { _device_id.devid_s.address = address; }

	/**
	 * Return the device type
	 *
	 * @return The device type
	 */
	uint8_t	get_device_type() ثابت { أرجع _device_id.devid_s.devtype; }
	فراغ	set_device_type(uint8_t devtype) { _device_id.devid_s.devtype = devtype; }

	/**
	 * Print decoded device id string to a buffer.
	 *
	 * @param buffer                        buffer to write to
	 * @param length                        buffer length
	 * @param id	                        The device id.
	 * @param return                        number of bytes written
	 */
	ساكن صحيح device_id_print_buffer(حرف *buffer, صحيح length, uint32_t id)
	{
		DeviceId dev_id{};
		dev_id.devid = id;

		صحيح num_written = snprintf(buffer, length, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
					   get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.address);

		buffer[length - 1] = 0; // ensure 0-termination

		أرجع num_written;
	}

	/**
	 * Is the sensor external to the flight controller board?
	 *
	 * I2C/SPI drivers set this from -I/-s vs -X/-S. Otherwise falls back to device_is_external().
	 */
	منطقي external() ثابت { أرجع _external_declared ? _external : device_is_external(_device_id.devid); }

	فراغ set_external(منطقي external) { _external = external; _external_declared = true; }

محمي:
	اتحاد DeviceId	_device_id {};            	/**< device identifier information */

	منطقي		_external{خطأ};
	منطقي		_external_declared{خطأ};

	ثابت حرف	*_name{nullptr};		/**< driver name */
	منطقي		_debug_enabled{خطأ};		/**< if true, debug messages are printed */

	Device() = delete;
	explicit Device(ثابت حرف *name) : _name(name) {}

	Device(uint8_t devtype, ثابت حرف *name, DeviceBusType bus_type, uint8_t bus, uint8_t address) : _name(name)
	{
		set_device_type(devtype);
		set_device_bus_type(bus_type);
		set_device_bus(bus);
		set_device_address(address);
	}

};

/*
 * Inline so that no caller has to link the kernel-only drivers__device library: in a
 * protected build that would pull the kernel into the userspace image.
 */
مضمن منطقي device_is_external(uint32_t device_id)
{
	Device::DeviceId id{};
	id.devid = device_id;

	بدل (id.devid_s.bus_type) {
	حالة Device::DeviceBusType_I2C:
#𐤀𐤌 defined(CONFIG_I2C)
		أرجع px4_i2c_bus_topology(id.devid_s.bus) != BusTopology::Internal;
#وإلا
		أرجع true;
#endif // CONFIG_I2C

	حالة Device::DeviceBusType_SPI:
#𐤀𐤌 defined(CONFIG_SPI)
		أرجع px4_spi_bus_topology(id.devid_s.bus) != BusTopology::Internal;
#وإلا
		أرجع true;
#endif // CONFIG_SPI

	حالة Device::DeviceBusType_SIMULATION:
		أرجع خطأ;

	حالة Device::DeviceBusType_UAVCAN:
	حالة Device::DeviceBusType_SERIAL:
	حالة Device::DeviceBusType_MAVLINK:
	حالة Device::DeviceBusType_UNKNOWN:
		أرجع true;
	}

	أرجع true;
}

} // namespace device

#endif /* _DEVICE_DEVICE_HPP */
