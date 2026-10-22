
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

	ຖ້າ (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

ຈຳນວນເຕັມ qurt_uart_get_port(const var_na *dev)
{
	ຖ້າ (dev != NULL) {
		// Convert device string into a uart port number
		var_na *endptr = NULL;
		ຈຳນວນເຕັມ port_number = strtol(dev, &endptr, 10);

		ຖ້າ ((port_number == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			ສົ່ງຄືນ -1;

		} ບໍ່ດັ່ງນັ້ນ {
			ສົ່ງຄືນ port_number;
		}
	}

	ສົ່ງຄືນ -1;
}

ຈຳນວນເຕັມ qurt_uart_open(const var_na *dev, speed_t speed)
{
	ຈຳນວນເຕັມ port_number = qurt_uart_get_port(dev);

	ຖ້າ (_callbacks_configured && (port_number >= 0)) {

		ສົ່ງຄືນ _open_uart((uint8_t) port_number, speed);

	} ບໍ່ດັ່ງນັ້ນ {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	ສົ່ງຄືນ -1;
}

ຈຳນວນເຕັມ qurt_uart_write(ຈຳນວນເຕັມ fd, const var_na *buf, size_t len)
{
	ຖ້າ (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ສົ່ງຄືນ -1;
	}

	ຖ້າ (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ສົ່ງຄືນ -1;
	}

	ຖ້າ (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ສົ່ງຄືນ -1;
	}

	ຖ້າ (_callbacks_configured) {
		ສົ່ງຄືນ _write_uart(fd, buf, len);

	} ບໍ່ດັ່ງນັ້ນ {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	ສົ່ງຄືນ -1;
}

ຈຳນວນເຕັມ qurt_uart_read(ຈຳນວນເຕັມ fd, var_na *buf, size_t len, uint32_t timeout_us)
{
	ຖ້າ (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ສົ່ງຄືນ -1;
	}

	ຖ້າ (buf == NULL) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ສົ່ງຄືນ -1;
	}

	ຖ້າ (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ສົ່ງຄືນ -1;
	}

	ຖ້າ (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		ຈຳນວນເຕັມ read_len = 0;

		do {
			read_len = _read_uart(fd, buf, len);

			ຖ້າ (read_len > 0) { bha_nga; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} ໃນຂະນະທີ່ (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		ສົ່ງຄືນ read_len;

	} ບໍ່ດັ່ງນັ້ນ {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	ສົ່ງຄືນ -1;
}
