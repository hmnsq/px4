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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _DEVICE_DEVICE_HPP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEVICE_DEVICE_HPP

#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>

/*
 * Includes here should only cover the needs of the framework definitions.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/spi.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_sensor.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEVICE_LOG(FMT, ...) PX4_LOG_NAMED(_name, FMT, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEVICE_DEBUG(FMT, ...) PX4_LOG_NAMED_COND(_name, _debug_enabled, FMT, ##__VA_ARGS__)

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
ਨਾਮਾਕਾਸ਼ device
{

/**
 * Fallback sensor classification from a device id when no start-line classification exists.
 *
 * I2C/SPI: Internal bus → internal, External or Shared bus → external (Shared cannot be
 * inferred; onboard chips on a Shared bus must call set_external(false) via -I/-s).
 * UAVCAN, serial, MAVLink → external. Simulation → internal.
 */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ device_is_external(uint32_t device_id);

/**
 * Fundamental base class for all physical drivers (I2C, SPI).
 *
 * This class provides the basic driver template for I2C and SPI devices
 */
ਵਰਗ __EXPORT Device
{
ਜਨਤਕ:

	// no copy, assignment, move, move assignment
	Device(ਅਚਲ Device &) = ਮਿਟਾਓ;
	Device &ਸੰਚਾਲਕ=(ਅਚਲ Device &) = ਮਿਟਾਓ;
	Device(Device &&) = ਮਿਟਾਓ;
	Device &ਸੰਚਾਲਕ=(Device &&) = ਮਿਟਾਓ;

	/**
	 * Destructor.
	 *
	 * Public so that anonymous devices can be destroyed.
	 */
	ਆਭਾਸੀ ~Device() = ਮੂਲ;

	/*
	 * Direct access methods.
	 */

	/**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
	ਆਭਾਸੀ ਪੂਰਨ	init() { ਵਾਪਸ PX4_OK; }

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
	ਆਭਾਸੀ ਪੂਰਨ	ਪਠ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2) { ਵਾਪਸ -ENODEV; }

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
	ਆਭਾਸੀ ਪੂਰਨ	ਲਿਖ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2) { ਵਾਪਸ -ENODEV; }

	/**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
	ਆਭਾਸੀ uint8_t read_reg(ਬਿਨਾਨਿਸ਼ਾਨੀ reg) { ਵਾਪਸ -ENODEV; }

	/**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
	ਆਭਾਸੀ ਪੂਰਨ write_reg(ਬਿਨਾਨਿਸ਼ਾਨੀ reg, uint8_t value) { ਵਾਪਸ -ENODEV; }

	/**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
	ਆਭਾਸੀ ਪੂਰਨ	ioctl(ਬਿਨਾਨਿਸ਼ਾਨੀ operation, ਬਿਨਾਨਿਸ਼ਾਨੀ &arg) { ਵਾਪਸ -ENODEV; }

	/** Device bus types for DEVID */
	ਗਿਣਤੀ DeviceBusType {
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
	ਬਣਤਰ DeviceStructure {
		DeviceBusType bus_type : 3;
		uint8_t bus: 5;    // which instance of the bus type
		uint8_t ਪਤਾ;   // address on the bus (eg. I2C address)
		uint8_t devtype;   // device class specific device type
	};

	ਸੰਘ DeviceId {
		ਬਣਤਰ DeviceStructure devid_s;
		uint32_t devid{0};
	};

	uint32_t get_device_id() ਅਚਲ { ਵਾਪਸ _device_id.devid; }

	/**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
	DeviceBusType get_device_bus_type() ਅਚਲ { ਵਾਪਸ _device_id.devid_s.bus_type; }
	ਖਾਲੀ          ਸੈੱਟ_device_bus_type(DeviceBusType bus_type) { _device_id.devid_s.bus_type = bus_type; }

	ਸਥਿਰ ਅਚਲ ਅੱਖਰ *get_device_bus_string(DeviceBusType bus)
	{
		ਚੋਣ (bus) {
		ਹਾਲਤ DeviceBusType_I2C:
			ਵਾਪਸ "I2C";

		ਹਾਲਤ DeviceBusType_SPI:
			ਵਾਪਸ "SPI";

		ਹਾਲਤ DeviceBusType_UAVCAN:
			ਵਾਪਸ "UAVCAN";

		ਹਾਲਤ DeviceBusType_SIMULATION:
			ਵਾਪਸ "SIMULATION";

		ਹਾਲਤ DeviceBusType_SERIAL:
			ਵਾਪਸ "SERIAL";

		ਹਾਲਤ DeviceBusType_MAVLINK:
			ਵਾਪਸ "MAVLINK";

		ਹਾਲਤ DeviceBusType_UNKNOWN:
		ਮੂਲ:
			ਵਾਪਸ "UNKNOWN";
		}
	}

	/**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
	uint8_t get_device_bus() ਅਚਲ { ਵਾਪਸ _device_id.devid_s.bus; }
	ਖਾਲੀ    ਸੈੱਟ_device_bus(uint8_t bus) { _device_id.devid_s.bus = bus; }

	/**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
	uint8_t	get_device_ਪਤਾ() ਅਚਲ { ਵਾਪਸ _device_id.devid_s.ਪਤਾ; }
	ਖਾਲੀ	ਸੈੱਟ_device_ਪਤਾ(ਪੂਰਨ ਪਤਾ) { _device_id.devid_s.ਪਤਾ = ਪਤਾ; }

	/**
	 * Return the device type
	 *
	 * @return The device type
	 */
	uint8_t	get_device_type() ਅਚਲ { ਵਾਪਸ _device_id.devid_s.devtype; }
	ਖਾਲੀ	ਸੈੱਟ_device_type(uint8_t devtype) { _device_id.devid_s.devtype = devtype; }

	/**
	 * Print decoded device id string to a buffer.
	 *
	 * @param buffer                        buffer to write to
	 * @param length                        buffer length
	 * @param id	                        The device id.
	 * @param return                        number of bytes written
	 */
	ਸਥਿਰ ਪੂਰਨ device_id_print_ਬਫਰ(ਅੱਖਰ *ਬਫਰ_2, ਪੂਰਨ ਲੰਮਾਈ, uint32_t id)
	{
		DeviceId dev_id{};
		dev_id.devid = id;

		ਪੂਰਨ ਅੰਕ_written = snprintf(ਬਫਰ_2, ਲੰਮਾਈ, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
					   get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.ਪਤਾ);

		ਬਫਰ_2[ਲੰਮਾਈ - 1] = 0; // ensure 0-termination

		ਵਾਪਸ ਅੰਕ_written;
	}

	/**
	 * Is the sensor external to the flight controller board?
	 *
	 * I2C/SPI drivers set this from -I/-s vs -X/-S. Otherwise falls back to device_is_external().
	 */
	ਤਾਰਕਿਕ external() ਅਚਲ { ਵਾਪਸ _external_declared ? _external : device_is_external(_device_id.devid); }

	ਖਾਲੀ ਸੈੱਟ_external(ਤਾਰਕਿਕ external) { _external = external; _external_declared = ਸੱਚ; }

ਸੁਰੱਖਿਅਤ:
	ਸੰਘ DeviceId	_device_id {};            	/**< device identifier information */

	ਤਾਰਕਿਕ		_external{ਝੂਠ};
	ਤਾਰਕਿਕ		_external_declared{ਝੂਠ};

	ਅਚਲ ਅੱਖਰ	*_name{ਖਾਲੀ_ਸੂਚਕ};		/**< driver name */
	ਤਾਰਕਿਕ		_debug_enabled{ਝੂਠ};		/**< if true, debug messages are printed */

	Device() = ਮਿਟਾਓ;
	ਸਪਸ਼ਟ Device(ਅਚਲ ਅੱਖਰ *name) : _name(name) {}

	Device(uint8_t devtype, ਅਚਲ ਅੱਖਰ *name, DeviceBusType bus_type, uint8_t bus, uint8_t ਪਤਾ) : _name(name)
	{
		ਸੈੱਟ_device_type(devtype);
		ਸੈੱਟ_device_bus_type(bus_type);
		ਸੈੱਟ_device_bus(bus);
		ਸੈੱਟ_device_ਪਤਾ(ਪਤਾ);
	}

};

/*
 * Inline so that no caller has to link the kernel-only drivers__device library: in a
 * protected build that would pull the kernel into the userspace image.
 */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ device_is_external(uint32_t device_id)
{
	Device::DeviceId id{};
	id.devid = device_id;

	ਚੋਣ (id.devid_s.bus_type) {
	ਹਾਲਤ Device::DeviceBusType_I2C:
#ਜੇ defined(CONFIG_I2C)
		ਵਾਪਸ px4_i2c_bus_topology(id.devid_s.bus) != BusTopology::ਅੰਦਰੂਨੀ;
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ ਸੱਚ;
#ਜੇ_ਅੰਤ // CONFIG_I2C

	ਹਾਲਤ Device::DeviceBusType_SPI:
#ਜੇ defined(CONFIG_SPI)
		ਵਾਪਸ px4_spi_bus_topology(id.devid_s.bus) != BusTopology::ਅੰਦਰੂਨੀ;
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ ਸੱਚ;
#ਜੇ_ਅੰਤ // CONFIG_SPI

	ਹਾਲਤ Device::DeviceBusType_SIMULATION:
		ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ Device::DeviceBusType_UAVCAN:
	ਹਾਲਤ Device::DeviceBusType_SERIAL:
	ਹਾਲਤ Device::DeviceBusType_MAVLINK:
	ਹਾਲਤ Device::DeviceBusType_UNKNOWN:
		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਸੱਚ;
}

} // namespace device

#ਜੇ_ਅੰਤ /* _DEVICE_DEVICE_HPP */
