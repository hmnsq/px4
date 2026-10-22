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

#ଅନ୍ତର୍ଭାଵ <cstdio>

/*
 * Includes here should only cover the needs of the framework definitions.
 */
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/i2c.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/spi.h>

#ଅନ୍ତର୍ଭାଵ <drivers/drv_sensor.h>

#define DEVICE_LOG(FMT, ...) PX4_LOG_NAMED(_name, FMT, ##__VA_ARGS__)
#define DEVICE_DEBUG(FMT, ...) PX4_LOG_NAMED_COND(_name, _debug_enabled, FMT, ##__VA_ARGS__)

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
ନାମାକାଶ device
{

/**
 * Fallback sensor classification from a device id when no start-line classification exists.
 *
 * I2C/SPI: Internal bus → internal, External or Shared bus → external (Shared cannot be
 * inferred; onboard chips on a Shared bus must call set_external(false) via -I/-s).
 * UAVCAN, serial, MAVLink → external. Simulation → internal.
 */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ device_is_external(uint32_t device_id);

/**
 * Fundamental base class for all physical drivers (I2C, SPI).
 *
 * This class provides the basic driver template for I2C and SPI devices
 */
ଶ୍ରେଣୀ __EXPORT Device
{
ସର୍ବସାଧାରଣ:

	// no copy, assignment, move, move assignment
	Device(ଅଚଳ Device &) = delete;
	Device &operator=(ଅଚଳ Device &) = delete;
	Device(Device &&) = delete;
	Device &operator=(Device &&) = delete;

	/**
	 * Destructor.
	 *
	 * Public so that anonymous devices can be destroyed.
	 */
	ଆଭାସୀ ~Device() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	/*
	 * Direct access methods.
	 */

	/**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init() { ଫେରନ୍ତୁ PX4_OK; }

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
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ପଠ(unsigned address, ଶୂନ୍ୟ *data, unsigned ଗଣନା_2) { ଫେରନ୍ତୁ -ENODEV; }

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
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଲିଖ(unsigned address, ଶୂନ୍ୟ *data, unsigned ଗଣନା_2) { ଫେରନ୍ତୁ -ENODEV; }

	/**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
	ଆଭାସୀ uint8_t read_reg(unsigned reg) { ଫେରନ୍ତୁ -ENODEV; }

	/**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_reg(unsigned reg, uint8_t value) { ଫେରନ୍ତୁ -ENODEV; }

	/**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ioctl(unsigned operation, unsigned &arg) { ଫେରନ୍ତୁ -ENODEV; }

	/** Device bus types for DEVID */
	ଗଣନା DeviceBusType {
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
	ସଂରଚନା DeviceStructure {
		DeviceBusType bus_type : 3;
		uint8_t bus: 5;    // which instance of the bus type
		uint8_t address;   // address on the bus (eg. I2C address)
		uint8_t devtype;   // device class specific device type
	};

	ସଂଘ DeviceId {
		ସଂରଚନା DeviceStructure devid_s;
		uint32_t devid{0};
	};

	uint32_t get_device_id() ଅଚଳ { ଫେରନ୍ତୁ _device_id.devid; }

	/**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
	DeviceBusType get_device_bus_type() ଅଚଳ { ଫେରନ୍ତୁ _device_id.devid_s.bus_type; }
	ଶୂନ୍ୟ          set_device_bus_type(DeviceBusType bus_type) { _device_id.devid_s.bus_type = bus_type; }

	ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *get_device_bus_string(DeviceBusType bus)
	{
		ବାଛ (bus) {
		ସ୍ଥିତି DeviceBusType_I2C:
			ଫେରନ୍ତୁ "I2C";

		ସ୍ଥିତି DeviceBusType_SPI:
			ଫେରନ୍ତୁ "SPI";

		ସ୍ଥିତି DeviceBusType_UAVCAN:
			ଫେରନ୍ତୁ "UAVCAN";

		ସ୍ଥିତି DeviceBusType_SIMULATION:
			ଫେରନ୍ତୁ "SIMULATION";

		ସ୍ଥିତି DeviceBusType_SERIAL:
			ଫେରନ୍ତୁ "SERIAL";

		ସ୍ଥିତି DeviceBusType_MAVLINK:
			ଫେରନ୍ତୁ "MAVLINK";

		ସ୍ଥିତି DeviceBusType_UNKNOWN:
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ "UNKNOWN";
		}
	}

	/**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
	uint8_t get_device_bus() ଅଚଳ { ଫେରନ୍ତୁ _device_id.devid_s.bus; }
	ଶୂନ୍ୟ    set_device_bus(uint8_t bus) { _device_id.devid_s.bus = bus; }

	/**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
	uint8_t	get_device_address() ଅଚଳ { ଫେରନ୍ତୁ _device_id.devid_s.address; }
	ଶୂନ୍ୟ	set_device_address(ପୂର୍ଣ୍ଣସଂଖ୍ୟା address) { _device_id.devid_s.address = address; }

	/**
	 * Return the device type
	 *
	 * @return The device type
	 */
	uint8_t	get_device_type() ଅଚଳ { ଫେରନ୍ତୁ _device_id.devid_s.devtype; }
	ଶୂନ୍ୟ	set_device_type(uint8_t devtype) { _device_id.devid_s.devtype = devtype; }

	/**
	 * Print decoded device id string to a buffer.
	 *
	 * @param buffer                        buffer to write to
	 * @param length                        buffer length
	 * @param id	                        The device id.
	 * @param return                        number of bytes written
	 */
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା device_id_print_buffer(ଅକ୍ଷର *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length, uint32_t id)
	{
		DeviceId dev_id{};
		dev_id.devid = id;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_written = snprintf(buffer, length, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
					   get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.address);

		buffer[length - 1] = 0; // ensure 0-termination

		ଫେରନ୍ତୁ ଅଙ୍କ_written;
	}

	/**
	 * Is the sensor external to the flight controller board?
	 *
	 * I2C/SPI drivers set this from -I/-s vs -X/-S. Otherwise falls back to device_is_external().
	 */
	ତାର୍କିକ external() ଅଚଳ { ଫେରନ୍ତୁ _external_declared ? _external : device_is_external(_device_id.devid); }

	ଶୂନ୍ୟ set_external(ତାର୍କିକ external) { _external = external; _external_declared = ସତ୍ୟ; }

ସୁରକ୍ଷିତ:
	ସଂଘ DeviceId	_device_id {};            	/**< device identifier information */

	ତାର୍କିକ		_external{ମିଥ୍ୟା};
	ତାର୍କିକ		_external_declared{ମିଥ୍ୟା};

	ଅଚଳ ଅକ୍ଷର	*_name{nullptr};		/**< driver name */
	ତାର୍କିକ		_debug_enabled{ମିଥ୍ୟା};		/**< if true, debug messages are printed */

	Device() = delete;
	explicit Device(ଅଚଳ ଅକ୍ଷର *name) : _name(name) {}

	Device(uint8_t devtype, ଅଚଳ ଅକ୍ଷର *name, DeviceBusType bus_type, uint8_t bus, uint8_t address) : _name(name)
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
ଧାଡ଼ିଭିତରେ ତାର୍କିକ device_is_external(uint32_t device_id)
{
	Device::DeviceId id{};
	id.devid = device_id;

	ବାଛ (id.devid_s.bus_type) {
	ସ୍ଥିତି Device::DeviceBusType_I2C:
#ଯଦି defined(CONFIG_I2C)
		ଫେରନ୍ତୁ px4_i2c_bus_topology(id.devid_s.bus) != BusTopology::Internal;
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ ସତ୍ୟ;
#endif // CONFIG_I2C

	ସ୍ଥିତି Device::DeviceBusType_SPI:
#ଯଦି defined(CONFIG_SPI)
		ଫେରନ୍ତୁ px4_spi_bus_topology(id.devid_s.bus) != BusTopology::Internal;
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ ସତ୍ୟ;
#endif // CONFIG_SPI

	ସ୍ଥିତି Device::DeviceBusType_SIMULATION:
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ସ୍ଥିତି Device::DeviceBusType_UAVCAN:
	ସ୍ଥିତି Device::DeviceBusType_SERIAL:
	ସ୍ଥିତି Device::DeviceBusType_MAVLINK:
	ସ୍ଥିତି Device::DeviceBusType_UNKNOWN:
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

} // namespace device

#endif /* _DEVICE_DEVICE_HPP */
