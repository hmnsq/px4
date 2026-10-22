
#شامل <px4_log.h>
#شامل <px4_platform_common/time.h>
#شامل "uart.h"

#تعریف_کرو UART_READ_POLL_INTERVAL_US 500

// Static variables
ساکن bool _callbacks_configured = false;

ساکن open_uart_func_t  _open_uart  = تہی;
ساکن write_uart_func_t _write_uart = تہی;
ساکن read_uart_func_t  _read_uart  = تہی;

خالی configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	اگر (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

صحیح qurt_uart_get_پورٹ(ثابت حرف *dev)
{
	اگر (dev != تہی) {
		// Convert device string into a uart port number
		حرف *endptr = تہی;
		صحیح پورٹ_نمبر = strtol(dev, &endptr, 10);

		اگر ((پورٹ_نمبر == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			واپس -1;

		} ورنہ {
			واپس پورٹ_نمبر;
		}
	}

	واپس -1;
}

صحیح qurt_uart_open(ثابت حرف *dev, speed_t رفتار)
{
	صحیح پورٹ_نمبر = qurt_uart_get_پورٹ(dev);

	اگر (_callbacks_configured && (پورٹ_نمبر >= 0)) {

		واپس _open_uart((uint8_t) پورٹ_نمبر, رفتار);

	} ورنہ {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	واپس -1;
}

صحیح qurt_uart_write(صحیح fd, ثابت حرف *buf, size_t len)
{
	اگر (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		واپس -1;
	}

	اگر (buf == تہی) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		واپس -1;
	}

	اگر (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		واپس -1;
	}

	اگر (_callbacks_configured) {
		واپس _write_uart(fd, buf, len);

	} ورنہ {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	واپس -1;
}

صحیح qurt_uart_read(صحیح fd, حرف *buf, size_t len, uint32_t timeout_us)
{
	اگر (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		واپس -1;
	}

	اگر (buf == تہی) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		واپس -1;
	}

	اگر (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		واپس -1;
	}

	اگر (_callbacks_configured) {
		uint32_t interval_شمار = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		صحیح read_len = 0;

		کرو {
			read_len = _read_uart(fd, buf, len);

			اگر (read_len > 0) { توڑو; }

			interval_شمار--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} جب (interval_شمار);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		واپس read_len;

	} ورنہ {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	واپس -1;
}
