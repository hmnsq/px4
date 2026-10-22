
#ضمن <px4_log.h>
#ضمن <px4_platform_common/time.h>
#ضمن "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
ساكن bool _callbacks_configured = false;

ساكن open_uart_func_t  _open_uart  = عدم;
ساكن write_uart_func_t _write_uart = عدم;
ساكن read_uart_func_t  _read_uart  = عدم;

فراغ configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	إذا (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

صحيح qurt_uart_get_port(ثابت حرف *dev)
{
	إذا (dev != عدم) {
		// Convert device string into a uart port number
		حرف *endptr = عدم;
		صحيح port_ژمار = strtol(dev, &endptr, 10);

		إذا ((port_ژمار == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			أرجع -1;

		} وإلا {
			أرجع port_ژمار;
		}
	}

	أرجع -1;
}

صحيح qurt_uart_open(ثابت حرف *dev, speed_t speed)
{
	صحيح port_ژمار = qurt_uart_get_port(dev);

	إذا (_callbacks_configured && (port_ژمار >= 0)) {

		أرجع _open_uart((uint8_t) port_ژمار, speed);

	} وإلا {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	أرجع -1;
}

صحيح qurt_uart_write(صحيح fd, ثابت حرف *buf, size_t len)
{
	إذا (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		أرجع -1;
	}

	إذا (buf == عدم) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		أرجع -1;
	}

	إذا (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		أرجع -1;
	}

	إذا (_callbacks_configured) {
		أرجع _write_uart(fd, buf, len);

	} وإلا {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	أرجع -1;
}

صحيح qurt_uart_read(صحيح fd, حرف *buf, size_t len, uint32_t timeout_us)
{
	إذا (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		أرجع -1;
	}

	إذا (buf == عدم) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		أرجع -1;
	}

	إذا (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		أرجع -1;
	}

	إذا (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		صحيح read_len = 0;

		افعل {
			read_len = _read_uart(fd, buf, len);

			إذا (read_len > 0) { اكسر; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} طالما (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		أرجع read_len;

	} وإلا {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	أرجع -1;
}
