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

#அந்தர்பாவ <cstdio>

/*
 * Includes here should only cover the needs of the framework definitions.
 */
#அந்தர்பாவ <px4_platform_common/i2c.h>
#அந்தர்பாவ <px4_platform_common/log.h>
#அந்தர்பாவ <px4_platform_common/px4_config.h>
#அந்தர்பாவ <px4_platform_common/posix.h>
#அந்தர்பாவ <px4_platform_common/spi.h>

#அந்தர்பாவ <drivers/drv_sensor.h>

#define DEVICE_LOG(FMT, ...) PX4_LOG_NAMED(_name, FMT, ##__VA_ARGS__)
#define DEVICE_DEBUG(FMT, ...) PX4_LOG_NAMED_COND(_name, _debug_enabled, FMT, ##__VA_ARGS__)

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
நாமாகாஶ device
{

/**
 * Fallback sensor classification from a device id when no start-line classification exists.
 *
 * I2C/SPI: Internal bus → internal, External or Shared bus → external (Shared cannot be
 * inferred; onboard chips on a Shared bus must call set_external(false) via -I/-s).
 * UAVCAN, serial, MAVLink → external. Simulation → internal.
 */
உள்வரி தருக்க device_is_external(uint32_t device_id);

/**
 * Fundamental base class for all physical drivers (I2C, SPI).
 *
 * This class provides the basic driver template for I2C and SPI devices
 */
வகுப்பு __EXPORT Device
{
பொது:

	// no copy, assignment, move, move assignment
	Device(மாறிலி Device &) = delete;
	Device &operator=(மாறிலி Device &) = delete;
	Device(Device &&) = delete;
	Device &operator=(Device &&) = delete;

	/**
	 * Destructor.
	 *
	 * Public so that anonymous devices can be destroyed.
	 */
	மெய்நிகர் ~Device() = இயல்பு;

	/*
	 * Direct access methods.
	 */

	/**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
	மெய்நிகர் முழு	init() { திரும்பு PX4_OK; }

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
	மெய்நிகர் முழு	பட(unsigned address, வெறுமை *data, unsigned மொத்தம்) { திரும்பு -ENODEV; }

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
	மெய்நிகர் முழு	லிக(unsigned address, வெறுமை *data, unsigned மொத்தம்) { திரும்பு -ENODEV; }

	/**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
	மெய்நிகர் uint8_t read_reg(unsigned reg) { திரும்பு -ENODEV; }

	/**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
	மெய்நிகர் முழு write_reg(unsigned reg, uint8_t value) { திரும்பு -ENODEV; }

	/**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
	மெய்நிகர் முழு	ioctl(unsigned operation, unsigned &arg) { திரும்பு -ENODEV; }

	/** Device bus types for DEVID */
	எண்ணிக்கை DeviceBusType {
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
	கட்டமைப்பு DeviceStructure {
		DeviceBusType bus_type : 3;
		uint8_t bus: 5;    // which instance of the bus type
		uint8_t address;   // address on the bus (eg. I2C address)
		uint8_t devtype;   // device class specific device type
	};

	ஒன்றிப்பு DeviceId {
		கட்டமைப்பு DeviceStructure devid_s;
		uint32_t devid{0};
	};

	uint32_t get_device_id() மாறிலி { திரும்பு _device_id.devid; }

	/**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
	DeviceBusType get_device_bus_type() மாறிலி { திரும்பு _device_id.devid_s.bus_type; }
	வெறுமை          set_device_bus_type(DeviceBusType bus_type) { _device_id.devid_s.bus_type = bus_type; }

	நிலையான மாறிலி எழுத்து *get_device_bus_string(DeviceBusType bus)
	{
		தேர்வு (bus) {
		நிலை DeviceBusType_I2C:
			திரும்பு "I2C";

		நிலை DeviceBusType_SPI:
			திரும்பு "SPI";

		நிலை DeviceBusType_UAVCAN:
			திரும்பு "UAVCAN";

		நிலை DeviceBusType_SIMULATION:
			திரும்பு "SIMULATION";

		நிலை DeviceBusType_SERIAL:
			திரும்பு "SERIAL";

		நிலை DeviceBusType_MAVLINK:
			திரும்பு "MAVLINK";

		நிலை DeviceBusType_UNKNOWN:
		இயல்பு:
			திரும்பு "UNKNOWN";
		}
	}

	/**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
	uint8_t get_device_bus() மாறிலி { திரும்பு _device_id.devid_s.bus; }
	வெறுமை    set_device_bus(uint8_t bus) { _device_id.devid_s.bus = bus; }

	/**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
	uint8_t	get_device_address() மாறிலி { திரும்பு _device_id.devid_s.address; }
	வெறுமை	set_device_address(முழு address) { _device_id.devid_s.address = address; }

	/**
	 * Return the device type
	 *
	 * @return The device type
	 */
	uint8_t	get_device_type() மாறிலி { திரும்பு _device_id.devid_s.devtype; }
	வெறுமை	set_device_type(uint8_t devtype) { _device_id.devid_s.devtype = devtype; }

	/**
	 * Print decoded device id string to a buffer.
	 *
	 * @param buffer                        buffer to write to
	 * @param length                        buffer length
	 * @param id	                        The device id.
	 * @param return                        number of bytes written
	 */
	நிலையான முழு device_id_print_buffer(எழுத்து *buffer, முழு length, uint32_t id)
	{
		DeviceId dev_id{};
		dev_id.devid = id;

		முழு இலக்கம்_written = snprintf(buffer, length, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
					   get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.address);

		buffer[length - 1] = 0; // ensure 0-termination

		திரும்பு இலக்கம்_written;
	}

	/**
	 * Is the sensor external to the flight controller board?
	 *
	 * I2C/SPI drivers set this from -I/-s vs -X/-S. Otherwise falls back to device_is_external().
	 */
	தருக்க external() மாறிலி { திரும்பு _external_declared ? _external : device_is_external(_device_id.devid); }

	வெறுமை set_external(தருக்க external) { _external = external; _external_declared = உண்மை; }

பாதுகாக்கப்பட்ட:
	ஒன்றிப்பு DeviceId	_device_id {};            	/**< device identifier information */

	தருக்க		_external{பொய்};
	தருக்க		_external_declared{பொய்};

	மாறிலி எழுத்து	*_name{nullptr};		/**< driver name */
	தருக்க		_debug_enabled{பொய்};		/**< if true, debug messages are printed */

	Device() = delete;
	explicit Device(மாறிலி எழுத்து *name) : _name(name) {}

	Device(uint8_t devtype, மாறிலி எழுத்து *name, DeviceBusType bus_type, uint8_t bus, uint8_t address) : _name(name)
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
உள்வரி தருக்க device_is_external(uint32_t device_id)
{
	Device::DeviceId id{};
	id.devid = device_id;

	தேர்வு (id.devid_s.bus_type) {
	நிலை Device::DeviceBusType_I2C:
#எனில் defined(CONFIG_I2C)
		திரும்பு px4_i2c_bus_topology(id.devid_s.bus) != BusTopology::Internal;
#இல்லையெனில்
		திரும்பு உண்மை;
#endif // CONFIG_I2C

	நிலை Device::DeviceBusType_SPI:
#எனில் defined(CONFIG_SPI)
		திரும்பு px4_spi_bus_topology(id.devid_s.bus) != BusTopology::Internal;
#இல்லையெனில்
		திரும்பு உண்மை;
#endif // CONFIG_SPI

	நிலை Device::DeviceBusType_SIMULATION:
		திரும்பு பொய்;

	நிலை Device::DeviceBusType_UAVCAN:
	நிலை Device::DeviceBusType_SERIAL:
	நிலை Device::DeviceBusType_MAVLINK:
	நிலை Device::DeviceBusType_UNKNOWN:
		திரும்பு உண்மை;
	}

	திரும்பு உண்மை;
}

} // namespace device

#endif /* _DEVICE_DEVICE_HPP */
