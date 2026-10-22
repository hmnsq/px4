#pragma once

#ifdef __cplusplus
خارجي "C" {
#endif

#ضمن <stdbool.h>
#ضمن <termios.h>

صحيح qurt_uart_get_port(ثابت حرف *dev);
صحيح qurt_uart_open(ثابت حرف *dev, speed_t speed);
صحيح qurt_uart_write(صحيح fd, ثابت حرف *buf, size_t len);
صحيح qurt_uart_read(صحيح fd, حرف *buf, size_t len, uint32_t timeout_us);

تعريفنوع صحيح (*open_uart_func_t)(uint8_t, speed_t);
تعريفنوع صحيح (*write_uart_func_t)(صحيح, ثابت فراغ *, size_t);
تعريفنوع صحيح (*read_uart_func_t)(صحيح,  فراغ *, size_t);

فراغ configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
