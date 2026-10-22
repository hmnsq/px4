
#ଅନ୍ତର୍ଭାଵ <px4_log.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/time.h>
#ଅନ୍ତର୍ଭାଵ "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
ସ୍ଥିର bool _callbacks_configured = false;

ସ୍ଥିର open_uart_func_t  _open_uart  = ଖାଲି;
ସ୍ଥିର write_uart_func_t _write_uart = ଖାଲି;
ସ୍ଥିର read_uart_func_t  _read_uart  = ଖାଲି;

ଶୂନ୍ୟ configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	ଯଦି (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_get_port(ଅଚଳ ଅକ୍ଷର *dev)
{
	ଯଦି (dev != ଖାଲି) {
		// Convert device string into a uart port number
		ଅକ୍ଷର *endptr = ଖାଲି;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା port_ସଂଖ୍ୟା = strtol(dev, &endptr, 10);

		ଯଦି ((port_ସଂଖ୍ୟା == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			ଫେରନ୍ତୁ -1;

		} ଅନ୍ୟଥା {
			ଫେରନ୍ତୁ port_ସଂଖ୍ୟା;
		}
	}

	ଫେରନ୍ତୁ -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_open(ଅଚଳ ଅକ୍ଷର *dev, speed_t speed)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା port_ସଂଖ୍ୟା = qurt_uart_get_port(dev);

	ଯଦି (_callbacks_configured && (port_ସଂଖ୍ୟା >= 0)) {

		ଫେରନ୍ତୁ _open_uart((uint8_t) port_ସଂଖ୍ୟା, speed);

	} ଅନ୍ୟଥା {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	ଫେରନ୍ତୁ -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *buf, size_t len)
{
	ଯଦି (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (buf == ଖାଲି) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (_callbacks_configured) {
		ଫେରନ୍ତୁ _write_uart(fd, buf, len);

	} ଅନ୍ୟଥା {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	ଫେରନ୍ତୁ -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର *buf, size_t len, uint32_t timeout_us)
{
	ଯଦି (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (buf == ଖାଲି) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_len = 0;

		କର {
			read_len = _read_uart(fd, buf, len);

			ଯଦି (read_len > 0) { ଭାଙ୍ଗ; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} ଯେତେବେଳେ (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		ଫେରନ୍ତୁ read_len;

	} ଅନ୍ୟଥା {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	ଫେରନ୍ତୁ -1;
}
