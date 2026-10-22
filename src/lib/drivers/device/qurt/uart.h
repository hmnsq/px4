#ہدایت once

#اگر_تعریف_شدہ __cplusplus
خارجی "C" {
#اگر_ختم

#شامل <stdbool.h>
#شامل <termios.h>

صحیح qurt_uart_get_پورٹ(ثابت حرف *dev);
صحیح qurt_uart_open(ثابت حرف *dev, speed_t رفتار);
صحیح qurt_uart_write(صحیح fd, ثابت حرف *buf, size_t len);
صحیح qurt_uart_read(صحیح fd, حرف *buf, size_t len, uint32_t timeout_us);

قسمتعریف صحیح (*open_uart_func_t)(uint8_t, speed_t);
قسمتعریف صحیح (*write_uart_func_t)(صحیح, ثابت خالی *, size_t);
قسمتعریف صحیح (*read_uart_func_t)(صحیح,  خالی *, size_t);

خالی configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#اگر_تعریف_شدہ __cplusplus
}
#اگر_ختم // __cplusplus
