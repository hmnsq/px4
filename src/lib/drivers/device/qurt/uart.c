
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "uart.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_READ_POLL_INTERVAL_US 500

// Static variables
ਸਥਿਰ bool _callbacks_configured = false;

ਸਥਿਰ open_uart_func_t  _open_uart  = ਨਿਰਾ;
ਸਥਿਰ write_uart_func_t _write_uart = ਨਿਰਾ;
ਸਥਿਰ read_uart_func_t  _read_uart  = ਨਿਰਾ;

ਖਾਲੀ configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	ਜੇ (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

ਪੂਰਨ qurt_uart_get_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ *dev)
{
	ਜੇ (dev != ਨਿਰਾ) {
		// Convert device string into a uart port number
		ਅੱਖਰ *endptr = ਨਿਰਾ;
		ਪੂਰਨ ਪੋਰਟ_ਸੰਖਿਆ = strtol(dev, &endptr, 10);

		ਜੇ ((ਪੋਰਟ_ਸੰਖਿਆ == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			ਵਾਪਸ -1;

		} ਨਹੀਂਤਾਂ {
			ਵਾਪਸ ਪੋਰਟ_ਸੰਖਿਆ;
		}
	}

	ਵਾਪਸ -1;
}

ਪੂਰਨ qurt_uart_open(ਅਚਲ ਅੱਖਰ *dev, speed_t ਰਫਤਾਰ)
{
	ਪੂਰਨ ਪੋਰਟ_ਸੰਖਿਆ = qurt_uart_get_ਪੋਰਟ(dev);

	ਜੇ (_callbacks_configured && (ਪੋਰਟ_ਸੰਖਿਆ >= 0)) {

		ਵਾਪਸ _open_uart((uint8_t) ਪੋਰਟ_ਸੰਖਿਆ, ਰਫਤਾਰ);

	} ਨਹੀਂਤਾਂ {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	ਵਾਪਸ -1;
}

ਪੂਰਨ qurt_uart_write(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *buf, size_t len)
{
	ਜੇ (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ਵਾਪਸ -1;
	}

	ਜੇ (buf == ਨਿਰਾ) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ਵਾਪਸ -1;
	}

	ਜੇ (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ਵਾਪਸ -1;
	}

	ਜੇ (_callbacks_configured) {
		ਵਾਪਸ _write_uart(fd, buf, len);

	} ਨਹੀਂਤਾਂ {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	ਵਾਪਸ -1;
}

ਪੂਰਨ qurt_uart_read(ਪੂਰਨ fd, ਅੱਖਰ *buf, size_t len, uint32_t timeout_us)
{
	ਜੇ (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		ਵਾਪਸ -1;
	}

	ਜੇ (buf == ਨਿਰਾ) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		ਵਾਪਸ -1;
	}

	ਜੇ (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		ਵਾਪਸ -1;
	}

	ਜੇ (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		ਪੂਰਨ read_len = 0;

		ਕਰੋ {
			read_len = _read_uart(fd, buf, len);

			ਜੇ (read_len > 0) { ਰੋਕੋ; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} ਜਦੋਂ (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		ਵਾਪਸ read_len;

	} ਨਹੀਂਤਾਂ {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	ਵਾਪਸ -1;
}
