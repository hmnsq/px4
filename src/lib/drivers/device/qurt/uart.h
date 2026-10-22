#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

整数 qurt_uart_get_port(const char *dev);
整数 qurt_uart_open(const char *dev, speed_t speed);
整数 qurt_uart_write(整数 fd, const char *buf, size_t len);
整数 qurt_uart_read(整数 fd, char *buf, size_t len, uint32_t timeout_us);

typedef 整数 (*open_uart_func_t)(uint8_t, speed_t);
typedef 整数 (*write_uart_func_t)(整数, const void *, size_t);
typedef 整数 (*read_uart_func_t)(整数,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
