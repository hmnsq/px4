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

#నిర్వచితం_కాకపోతే _DEVICE_DEVICE_HPP
#నిర్వచించు _DEVICE_DEVICE_HPP

#అన్తర్భావ <cstdio>

/*
 * Includes here should only cover the needs of the framework definitions.
 */
#అన్తర్భావ <px4_platform_common/i2c.h>
#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <px4_platform_common/spi.h>

#అన్తర్భావ <drivers/drv_sensor.h>

#నిర్వచించు DEVICE_LOG(FMT, ...) PX4_LOG_NAMED(_name, FMT, ##__VA_ARGS__)
#నిర్వచించు DEVICE_DEBUG(FMT, ...) PX4_LOG_NAMED_COND(_name, _debug_enabled, FMT, ##__VA_ARGS__)

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
నామాకాశ device
{

/**
 * Fallback sensor classification from a device id when no start-line classification exists.
 *
 * I2C/SPI: Internal bus → internal, External or Shared bus → external (Shared cannot be
 * inferred; onboard chips on a Shared bus must call set_external(false) via -I/-s).
 * UAVCAN, serial, MAVLink → external. Simulation → internal.
 */
అంతఃవరుస తార్కిక device_is_external(uint32_t device_id);

/**
 * Fundamental base class for all physical drivers (I2C, SPI).
 *
 * This class provides the basic driver template for I2C and SPI devices
 */
తరగతి __EXPORT Device
{
బహిరంగ:

	// no copy, assignment, move, move assignment
	Device(స్థిరరాశి Device &) = తొలగించు;
	Device &క్రియాకారకం=(స్థిరరాశి Device &) = తొలగించు;
	Device(Device &&) = తొలగించు;
	Device &క్రియాకారకం=(Device &&) = తొలగించు;

	/**
	 * Destructor.
	 *
	 * Public so that anonymous devices can be destroyed.
	 */
	మాయా ~Device() = అప్రమేయ;

	/*
	 * Direct access methods.
	 */

	/**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
	మాయా పూర్ణాంకం	init() { తిరిగి PX4_OK; }

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
	మాయా పూర్ణాంకం	పఠ(గుర్తులేని చిరునామా, శూన్య *దత్తాంశం, గుర్తులేని లెక్క) { తిరిగి -ENODEV; }

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
	మాయా పూర్ణాంకం	లిఖ(గుర్తులేని చిరునామా, శూన్య *దత్తాంశం, గుర్తులేని లెక్క) { తిరిగి -ENODEV; }

	/**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
	మాయా uint8_t read_reg(గుర్తులేని reg) { తిరిగి -ENODEV; }

	/**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
	మాయా పూర్ణాంకం write_reg(గుర్తులేని reg, uint8_t value) { తిరిగి -ENODEV; }

	/**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
	మాయా పూర్ణాంకం	ioctl(గుర్తులేని operation, గుర్తులేని &arg) { తిరిగి -ENODEV; }

	/** Device bus types for DEVID */
	గణన DeviceBusType {
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
	నిర్మాణం DeviceStructure {
		DeviceBusType bus_type : 3;
		uint8_t bus: 5;    // which instance of the bus type
		uint8_t చిరునామా;   // address on the bus (eg. I2C address)
		uint8_t devtype;   // device class specific device type
	};

	సంఘం DeviceId {
		నిర్మాణం DeviceStructure devid_s;
		uint32_t devid{0};
	};

	uint32_t get_device_id() స్థిరరాశి { తిరిగి _device_id.devid; }

	/**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
	DeviceBusType get_device_bus_type() స్థిరరాశి { తిరిగి _device_id.devid_s.bus_type; }
	శూన్య          సమితి_device_bus_type(DeviceBusType bus_type) { _device_id.devid_s.bus_type = bus_type; }

	స్థిర స్థిరరాశి అక్షరం *get_device_bus_string(DeviceBusType bus)
	{
		ఎంపిక (bus) {
		స్థితి DeviceBusType_I2C:
			తిరిగి "I2C";

		స్థితి DeviceBusType_SPI:
			తిరిగి "SPI";

		స్థితి DeviceBusType_UAVCAN:
			తిరిగి "UAVCAN";

		స్థితి DeviceBusType_SIMULATION:
			తిరిగి "SIMULATION";

		స్థితి DeviceBusType_SERIAL:
			తిరిగి "SERIAL";

		స్థితి DeviceBusType_MAVLINK:
			తిరిగి "MAVLINK";

		స్థితి DeviceBusType_UNKNOWN:
		అప్రమేయ:
			తిరిగి "UNKNOWN";
		}
	}

	/**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
	uint8_t get_device_bus() స్థిరరాశి { తిరిగి _device_id.devid_s.bus; }
	శూన్య    సమితి_device_bus(uint8_t bus) { _device_id.devid_s.bus = bus; }

	/**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
	uint8_t	get_device_చిరునామా() స్థిరరాశి { తిరిగి _device_id.devid_s.చిరునామా; }
	శూన్య	సమితి_device_చిరునామా(పూర్ణాంకం చిరునామా) { _device_id.devid_s.చిరునామా = చిరునామా; }

	/**
	 * Return the device type
	 *
	 * @return The device type
	 */
	uint8_t	get_device_type() స్థిరరాశి { తిరిగి _device_id.devid_s.devtype; }
	శూన్య	సమితి_device_type(uint8_t devtype) { _device_id.devid_s.devtype = devtype; }

	/**
	 * Print decoded device id string to a buffer.
	 *
	 * @param buffer                        buffer to write to
	 * @param length                        buffer length
	 * @param id	                        The device id.
	 * @param return                        number of bytes written
	 */
	స్థిర పూర్ణాంకం device_id_print_బఫరు(అక్షరం *బఫరు_2, పూర్ణాంకం నిడివి, uint32_t id)
	{
		DeviceId dev_id{};
		dev_id.devid = id;

		పూర్ణాంకం అంకె_written = snprintf(బఫరు_2, నిడివి, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
					   get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus, dev_id.devid_s.చిరునామా);

		బఫరు_2[నిడివి - 1] = 0; // ensure 0-termination

		తిరిగి అంకె_written;
	}

	/**
	 * Is the sensor external to the flight controller board?
	 *
	 * I2C/SPI drivers set this from -I/-s vs -X/-S. Otherwise falls back to device_is_external().
	 */
	తార్కిక external() స్థిరరాశి { తిరిగి _external_declared ? _external : device_is_external(_device_id.devid); }

	శూన్య సమితి_external(తార్కిక external) { _external = external; _external_declared = నిజం; }

రక్షిత:
	సంఘం DeviceId	_device_id {};            	/**< device identifier information */

	తార్కిక		_external{అబద్ధం};
	తార్కిక		_external_declared{అబద్ధం};

	స్థిరరాశి అక్షరం	*_name{శూన్యసూచిక};		/**< driver name */
	తార్కిక		_debug_enabled{అబద్ధం};		/**< if true, debug messages are printed */

	Device() = తొలగించు;
	స్పష్టమైన Device(స్థిరరాశి అక్షరం *name) : _name(name) {}

	Device(uint8_t devtype, స్థిరరాశి అక్షరం *name, DeviceBusType bus_type, uint8_t bus, uint8_t చిరునామా) : _name(name)
	{
		సమితి_device_type(devtype);
		సమితి_device_bus_type(bus_type);
		సమితి_device_bus(bus);
		సమితి_device_చిరునామా(చిరునామా);
	}

};

/*
 * Inline so that no caller has to link the kernel-only drivers__device library: in a
 * protected build that would pull the kernel into the userspace image.
 */
అంతఃవరుస తార్కిక device_is_external(uint32_t device_id)
{
	Device::DeviceId id{};
	id.devid = device_id;

	ఎంపిక (id.devid_s.bus_type) {
	స్థితి Device::DeviceBusType_I2C:
#ఒకవేళ defined(CONFIG_I2C)
		తిరిగి px4_i2c_bus_topology(id.devid_s.bus) != BusTopology::అంతర్గత;
#లేకపోతే
		తిరిగి నిజం;
#అయితే_ముగింపు // CONFIG_I2C

	స్థితి Device::DeviceBusType_SPI:
#ఒకవేళ defined(CONFIG_SPI)
		తిరిగి px4_spi_bus_topology(id.devid_s.bus) != BusTopology::అంతర్గత;
#లేకపోతే
		తిరిగి నిజం;
#అయితే_ముగింపు // CONFIG_SPI

	స్థితి Device::DeviceBusType_SIMULATION:
		తిరిగి అబద్ధం;

	స్థితి Device::DeviceBusType_UAVCAN:
	స్థితి Device::DeviceBusType_SERIAL:
	స్థితి Device::DeviceBusType_MAVLINK:
	స్థితి Device::DeviceBusType_UNKNOWN:
		తిరిగి నిజం;
	}

	తిరిగి నిజం;
}

} // namespace device

#అయితే_ముగింపు /* _DEVICE_DEVICE_HPP */
