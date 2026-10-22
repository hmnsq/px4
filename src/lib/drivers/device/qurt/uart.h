#pragma once

#ifdef __cplusplus
بیرونی "C" {
#endif

#شامل <stdbool.h>
#شامل <termios.h>

صحیح qurt_uart_get_port(ثابت نویسه *dev);
صحیح qurt_uart_open(ثابت نویسه *dev, speed_t speed);
صحیح qurt_uart_write(صحیح fd, ثابت نویسه *buf, size_t len);
صحیح qurt_uart_read(صحیح fd, نویسه *buf, size_t len, uint32_t timeout_us);

تعریفنوع صحیح (*open_uart_func_t)(uint8_t, speed_t);
تعریفنوع صحیح (*write_uart_func_t)(صحیح, ثابت تهی *, size_t);
تعریفنوع صحیح (*read_uart_func_t)(صحیح,  تهی *, size_t);

تهی configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
