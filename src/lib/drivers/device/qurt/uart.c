
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

	ᯀᯮᯝ (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

puur_naa_nka qurt_uart_get_port(const var_na *dev)
{
	ᯀᯮᯝ (dev != NULL) {
		// Convert device string into a uart port number
		var_na *endptr = NULL;
		puur_naa_nka port_number = strtol(dev, &endptr, 10);

		ᯀᯮᯝ ((port_number == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			ᯀᯮᯞ -1;

		} ᯀᯮᯝᯂᯪ {
			ᯀᯮᯞ port_number;
		}
	}

	ᯀᯮᯞ -1;
}

puur_naa_nka qurt_uart_open(const var_na *dev, speed_t speed)
{
	puur_naa_nka port_number = qurt_uart_get_port(dev);

	ᯀᯮᯝ (_callbacks_configured && (port_number >= 0)) {

		ᯀᯮᯞ _open_uart((uint8_t) port_number, speed);

	} ᯀᯮᯝᯂᯪ {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	ᯀᯮᯞ -1;
}

puur_naa_nka qurt_uart_write(puur_naa_nka fd, const var_na *buf, size_t len)
{
	ᯀᯮᯝ (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ᯀᯮᯞ -1;
	}

	ᯀᯮᯝ (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ᯀᯮᯞ -1;
	}

	ᯀᯮᯝ (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ᯀᯮᯞ -1;
	}

	ᯀᯮᯝ (_callbacks_configured) {
		ᯀᯮᯞ _write_uart(fd, buf, len);

	} ᯀᯮᯝᯂᯪ {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	ᯀᯮᯞ -1;
}

puur_naa_nka qurt_uart_read(puur_naa_nka fd, var_na *buf, size_t len, uint32_t timeout_us)
{
	ᯀᯮᯝ (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ᯀᯮᯞ -1;
	}

	ᯀᯮᯝ (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ᯀᯮᯞ -1;
	}

	ᯀᯮᯝ (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ᯀᯮᯞ -1;
	}

	ᯀᯮᯝ (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		puur_naa_nka read_len = 0;

		do {
			read_len = _read_uart(fd, buf, len);

			ᯀᯮᯝ (read_len > 0) { bha_nga; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} ᯀᯬᯝ (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		ᯀᯮᯞ read_len;

	} ᯀᯮᯝᯂᯪ {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	ᯀᯮᯞ -1;
}
