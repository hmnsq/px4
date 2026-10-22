
#અન્તર્ભાવ <px4_log.h>
#અન્તર્ભાવ <px4_platform_common/time.h>
#અન્તર્ભાવ "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
સ્થિર bool _callbacks_configured = false;

સ્થિર open_uart_func_t  _open_uart  = ખાલી;
સ્થિર write_uart_func_t _write_uart = ખાલી;
સ્થિર read_uart_func_t  _read_uart  = ખાલી;

શૂન્ય configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	જો (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

પૂર્ણાંક qurt_uart_get_port(અચલ અક્ષર *dev)
{
	જો (dev != ખાલી) {
		// Convert device string into a uart port number
		અક્ષર *endptr = ખાલી;
		પૂર્ણાંક port_સંખ્યા = strtol(dev, &endptr, 10);

		જો ((port_સંખ્યા == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			પાછા -1;

		} નહીંતર {
			પાછા port_સંખ્યા;
		}
	}

	પાછા -1;
}

પૂર્ણાંક qurt_uart_open(અચલ અક્ષર *dev, speed_t speed)
{
	પૂર્ણાંક port_સંખ્યા = qurt_uart_get_port(dev);

	જો (_callbacks_configured && (port_સંખ્યા >= 0)) {

		પાછા _open_uart((uint8_t) port_સંખ્યા, speed);

	} નહીંતર {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	પાછા -1;
}

પૂર્ણાંક qurt_uart_write(પૂર્ણાંક fd, અચલ અક્ષર *buf, size_t len)
{
	જો (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		પાછા -1;
	}

	જો (buf == ખાલી) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		પાછા -1;
	}

	જો (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		પાછા -1;
	}

	જો (_callbacks_configured) {
		પાછા _write_uart(fd, buf, len);

	} નહીંતર {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	પાછા -1;
}

પૂર્ણાંક qurt_uart_read(પૂર્ણાંક fd, અક્ષર *buf, size_t len, uint32_t timeout_us)
{
	જો (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		પાછા -1;
	}

	જો (buf == ખાલી) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		પાછા -1;
	}

	જો (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		પાછા -1;
	}

	જો (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		પૂર્ણાંક read_len = 0;

		કરો {
			read_len = _read_uart(fd, buf, len);

			જો (read_len > 0) { તોડો; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} જ્યારે (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		પાછા read_len;

	} નહીંતર {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	પાછા -1;
}
