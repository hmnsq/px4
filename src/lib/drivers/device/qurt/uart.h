#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

정수 qurt_uart_get_port(const char *dev);
정수 qurt_uart_open(const char *dev, speed_t speed);
정수 qurt_uart_write(정수 fd, const char *buf, size_t len);
정수 qurt_uart_read(정수 fd, char *buf, size_t len, uint32_t timeout_us);

typedef 정수 (*open_uart_func_t)(uint8_t, speed_t);
typedef 정수 (*write_uart_func_t)(정수, const void *, size_t);
typedef 정수 (*read_uart_func_t)(정수,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
