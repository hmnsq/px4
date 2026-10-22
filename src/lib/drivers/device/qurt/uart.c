
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

	만약 (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

정수 qurt_uart_get_port(const char *dev)
{
	만약 (dev != NULL) {
		// Convert device string into a uart port number
		char *endptr = NULL;
		정수 port_number = strtol(dev, &endptr, 10);

		만약 ((port_number == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			반환 -1;

		} 아니면 {
			반환 port_number;
		}
	}

	반환 -1;
}

정수 qurt_uart_open(const char *dev, speed_t speed)
{
	정수 port_number = qurt_uart_get_port(dev);

	만약 (_callbacks_configured && (port_number >= 0)) {

		반환 _open_uart((uint8_t) port_number, speed);

	} 아니면 {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	반환 -1;
}

정수 qurt_uart_write(정수 fd, const char *buf, size_t len)
{
	만약 (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		반환 -1;
	}

	만약 (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		반환 -1;
	}

	만약 (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		반환 -1;
	}

	만약 (_callbacks_configured) {
		반환 _write_uart(fd, buf, len);

	} 아니면 {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	반환 -1;
}

정수 qurt_uart_read(정수 fd, char *buf, size_t len, uint32_t timeout_us)
{
	만약 (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		반환 -1;
	}

	만약 (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		반환 -1;
	}

	만약 (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		반환 -1;
	}

	만약 (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		정수 read_len = 0;

		do {
			read_len = _read_uart(fd, buf, len);

			만약 (read_len > 0) { break; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} 동안 (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		반환 read_len;

	} 아니면 {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	반환 -1;
}
