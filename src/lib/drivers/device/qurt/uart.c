
#अन्तर्भाव <px4_log.h>
#अन्तर्भाव <px4_platform_common/time.h>
#अन्तर्भाव "uart.h"

#define UART_READ_POLL_INTERVAL_US 500

// Static variables
स्थिर bool _callbacks_configured = false;

स्थिर open_uart_func_t  _open_uart  = रिक्त;
स्थिर write_uart_func_t _write_uart = रिक्त;
स्थिर read_uart_func_t  _read_uart  = रिक्त;

शून्य configure_uart_callbacks(open_uart_func_t open_func,
			      write_uart_func_t write_func,
			      read_uart_func_t read_func)
{
	_open_uart  = open_func;
	_write_uart = write_func;
	_read_uart  = read_func;

	जर (_open_uart && _write_uart && _read_uart) {
		_callbacks_configured = true;
	}
}

पूर्णांक qurt_uart_get_port(स्थिरांक अक्षर *dev)
{
	जर (dev != रिक्त) {
		// Convert device string into a uart port number
		अक्षर *endptr = रिक्त;
		पूर्णांक port_संख्या = म_से_दीर्घ(dev, &endptr, 10);

		जर ((port_संख्या == 0) && (endptr == dev)) {
			PX4_ERR("Could not convert %s into a valid uart port number", dev);
			परत -1;

		} अन्यथा {
			परत port_संख्या;
		}
	}

	परत -1;
}

पूर्णांक qurt_uart_open(स्थिरांक अक्षर *dev, speed_t speed)
{
	पूर्णांक port_संख्या = qurt_uart_get_port(dev);

	जर (_callbacks_configured && (port_संख्या >= 0)) {

		परत _open_uart((uint8_t) port_संख्या, speed);

	} अन्यथा {
		PX4_ERR("Cannot open uart until callbacks have been configured");
	}

	परत -1;
}

पूर्णांक qurt_uart_write(पूर्णांक fd, स्थिरांक अक्षर *buf, माप_प्रकार len)
{
	जर (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		परत -1;
	}

	जर (buf == रिक्त) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		परत -1;
	}

	जर (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		परत -1;
	}

	जर (_callbacks_configured) {
		परत _write_uart(fd, buf, len);

	} अन्यथा {
		PX4_ERR("Cannot write to uart until callbacks have been configured");
	}

	परत -1;
}

पूर्णांक qurt_uart_read(पूर्णांक fd, अक्षर *buf, माप_प्रकार len, uint32_t timeout_us)
{
	जर (fd < 0) {
		PX4_ERR("invalid fd %d for %s", fd, __FUNCTION__);
		परत -1;
	}

	जर (buf == रिक्त) {
		PX4_ERR("NULL buffer pointer in %s", fd, __FUNCTION__);
		परत -1;
	}

	जर (len == 0) {
		PX4_ERR("Zero length buffer in %s", __FUNCTION__);
		परत -1;
	}

	जर (_callbacks_configured) {
		uint32_t interval_counter = (timeout_us + (UART_READ_POLL_INTERVAL_US - 1)) / UART_READ_POLL_INTERVAL_US;
		// PX4_INFO("UART interval counter = %d", interval_counter);
		पूर्णांक read_len = 0;

		करा {
			read_len = _read_uart(fd, buf, len);

			जर (read_len > 0) { थांबा; }

			interval_counter--;
			px4_usleep(UART_READ_POLL_INTERVAL_US);
		} पर्यंत (interval_counter);

		// if (read_len <= 0) PX4_INFO("Warning, UART read timed out");
		// else PX4_INFO("UART read %d bytes, counter = %d", read_len, interval_counter);
		परत read_len;

	} अन्यथा {
		PX4_ERR("Cannot read from uart until callbacks have been configured");
	}

	परत -1;
}
