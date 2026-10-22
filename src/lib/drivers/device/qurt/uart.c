
#அந்தர்பாவ <px4_log.h>
#அந்தர்பாவ <px4_platform_common/time.h>
#அந்தர்பாவ "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
நிலையான bool _callbacks_configured = false;

நிலையான open_uart_func_t  _open_uart  = வெற்று;
நிலையான write_uart_func_t _write_uart = வெற்று;
நிலையான read_uart_func_t  _read_uart  = வெற்று;

வெறுமை configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	எனில் (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

முழு qurt_uart_get_port(மாறிலி எழுத்து *dev)
{
	எனில் (dev != வெற்று) {
		// Convert device string into a uart port number
		எழுத்து *endptr = வெற்று;
		முழு port_எண் = strtol(dev, &endptr, 10);

		எனில் ((port_எண் == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			திரும்பு -1;

		} இல்லையெனில் {
			திரும்பு port_எண்;
		}
	}

	திரும்பு -1;
}

முழு qurt_uart_open(மாறிலி எழுத்து *dev, speed_t speed)
{
	முழு port_எண் = qurt_uart_get_port(dev);

	எனில் (_callbacks_configured && (port_எண் >= 0)) {

		திரும்பு _open_uart((uint8_t) port_எண், speed);

	} இல்லையெனில் {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	திரும்பு -1;
}

முழு qurt_uart_write(முழு fd, மாறிலி எழுத்து *buf, size_t len)
{
	எனில் (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		திரும்பு -1;
	}

	எனில் (buf == வெற்று) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		திரும்பு -1;
	}

	எனில் (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		திரும்பு -1;
	}

	எனில் (_callbacks_configured) {
		திரும்பு _write_uart(fd, buf, len);

	} இல்லையெனில் {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	திரும்பு -1;
}

முழு qurt_uart_read(முழு fd, எழுத்து *buf, size_t len, uint32_t timeout_us)
{
	எனில் (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		திரும்பு -1;
	}

	எனில் (buf == வெற்று) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		திரும்பு -1;
	}

	எனில் (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		திரும்பு -1;
	}

	எனில் (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		முழு read_len = 0;

		செய் {
			read_len = _read_uart(fd, buf, len);

			எனில் (read_len > 0) { நிறுத்து; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} போது (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		திரும்பு read_len;

	} இல்லையெனில் {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	திரும்பு -1;
}
