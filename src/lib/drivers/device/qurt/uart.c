
#include <px4_log.h>
#include <px4_platform_common/time.h>
#include "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
static bool _callbacks_configured = false;

static open_uart_func_t  _open_uart  = NULL;
static write_uart_func_t _write_uart = NULL;
static read_uart_func_t  _read_uart  = NULL;

void configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	ако (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

цяло qurt_uart_get_port(const char *dev)
{
	ако (dev != NULL) {
		// Convert device string into a uart port number
		char *endptr = NULL;
		цяло port_number = strtol(dev, &endptr, 10);

		ако ((port_number == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			върни -1;

		} иначе {
			върни port_number;
		}
	}

	върни -1;
}

цяло qurt_uart_open(const char *dev, speed_t speed)
{
	цяло port_number = qurt_uart_get_port(dev);

	ако (_callbacks_configured && (port_number >= 0)) {

		върни _open_uart((uint8_t) port_number, speed);

	} иначе {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	върни -1;
}

цяло qurt_uart_write(цяло fd, const char *buf, size_t len)
{
	ако (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		върни -1;
	}

	ако (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		върни -1;
	}

	ако (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		върни -1;
	}

	ако (_callbacks_configured) {
		върни _write_uart(fd, buf, len);

	} иначе {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	върни -1;
}

цяло qurt_uart_read(цяло fd, char *buf, size_t len, uint32_t timeout_us)
{
	ако (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		върни -1;
	}

	ако (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		върни -1;
	}

	ако (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		върни -1;
	}

	ако (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		цяло read_len = 0;

		do {
			read_len = _read_uart(fd, buf, len);

			ако (read_len > 0) { break; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} докато (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		върни read_len;

	} иначе {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	върни -1;
}
