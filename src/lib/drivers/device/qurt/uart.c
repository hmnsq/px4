
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

	якщо (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

ціле qurt_uart_get_port(const char *dev)
{
	якщо (dev != NULL) {
		// Convert device string into a uart port number
		char *endptr = NULL;
		ціле port_number = strtol(dev, &endptr, 10);

		якщо ((port_number == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			повернути -1;

		} інакше {
			повернути port_number;
		}
	}

	повернути -1;
}

ціле qurt_uart_open(const char *dev, speed_t speed)
{
	ціле port_number = qurt_uart_get_port(dev);

	якщо (_callbacks_configured && (port_number >= 0)) {

		повернути _open_uart((uint8_t) port_number, speed);

	} інакше {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	повернути -1;
}

ціле qurt_uart_write(ціле fd, const char *buf, size_t len)
{
	якщо (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		повернути -1;
	}

	якщо (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		повернути -1;
	}

	якщо (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		повернути -1;
	}

	якщо (_callbacks_configured) {
		повернути _write_uart(fd, buf, len);

	} інакше {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	повернути -1;
}

ціле qurt_uart_read(ціле fd, char *buf, size_t len, uint32_t timeout_us)
{
	якщо (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		повернути -1;
	}

	якщо (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		повернути -1;
	}

	якщо (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		повернути -1;
	}

	якщо (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		ціле read_len = 0;

		do {
			read_len = _read_uart(fd, buf, len);

			якщо (read_len > 0) { break; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} поки (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		повернути read_len;

	} інакше {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	повернути -1;
}
