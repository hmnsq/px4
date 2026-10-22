#هدايت once

#جيڪڏهن_وصف_ٿيل __cplusplus
خارجي "C" {
#جيڪڏهن_پورو

#ضمن <stdbool.h>
#ضمن <termios.h>

صحيح qurt_uart_get_پورٽ(ثابت حرف *dev);
صحيح qurt_uart_open(ثابت حرف *dev, speed_t رفتار);
صحيح qurt_uart_write(صحيح fd, ثابت حرف *buf, size_t len);
صحيح qurt_uart_read(صحيح fd, حرف *buf, size_t len, uint32_t timeout_us);

تعريفنوع صحيح (*open_uart_func_t)(uint8_t, speed_t);
تعريفنوع صحيح (*write_uart_func_t)(صحيح, ثابت فراغ *, size_t);
تعريفنوع صحيح (*read_uart_func_t)(صحيح,  فراغ *, size_t);

فراغ configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#جيڪڏهن_وصف_ٿيل __cplusplus
}
#جيڪڏهن_پورو // __cplusplus
