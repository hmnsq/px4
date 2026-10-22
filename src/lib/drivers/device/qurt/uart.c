
#شامل <px4_log.h>
#شامل <px4_platform_common/time.h>
#شامل "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
ایستا bool _callbacks_configured = false;

ایستا open_uart_func_t  _open_uart  = پوچ;
ایستا write_uart_func_t _write_uart = پوچ;
ایستا read_uart_func_t  _read_uart  = پوچ;

تهی configure_uart_callbacks(open_uart_func_t open_func,
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

صحیح qurt_uart_get_port(ثابت نویسه *dev)
{
	اگر (dev != پوچ) {
		// Convert device string into a uart port number
		نویسه *endptr = پوچ;
		صحیح port_عدد = strtol(dev, &endptr, 10);

		اگر ((port_عدد == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			بازگشت -1;

		} وگرنه {
			بازگشت port_عدد;
		}
	}

	بازگشت -1;
}

صحیح qurt_uart_open(ثابت نویسه *dev, speed_t speed)
{
	صحیح port_عدد = qurt_uart_get_port(dev);

	اگر (_callbacks_configured && (port_عدد >= 0)) {

		بازگشت _open_uart((uint8_t) port_عدد, speed);

	} وگرنه {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	بازگشت -1;
}

صحیح qurt_uart_write(صحیح fd, ثابت نویسه *buf, size_t len)
{
	اگر (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		بازگشت -1;
	}

	اگر (buf == پوچ) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		بازگشت -1;
	}

	اگر (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		بازگشت -1;
	}

	اگر (_callbacks_configured) {
		بازگشت _write_uart(fd, buf, len);

	} وگرنه {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	بازگشت -1;
}

صحیح qurt_uart_read(صحیح fd, نویسه *buf, size_t len, uint32_t timeout_us)
{
	اگر (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		بازگشت -1;
	}

	اگر (buf == پوچ) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		بازگشت -1;
	}

	اگر (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		بازگشت -1;
	}

	اگر (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		صحیح read_len = 0;

		انجامبده {
			read_len = _read_uart(fd, buf, len);

			اگر (read_len > 0) { بشکن; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} وقتی (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		بازگشت read_len;

	} وگرنه {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	بازگشت -1;
}
