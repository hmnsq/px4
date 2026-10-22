#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

целое qurt_uart_get_port(const char *dev);
целое qurt_uart_open(const char *dev, speed_t speed);
целое qurt_uart_write(целое fd, const char *buf, size_t len);
целое qurt_uart_read(целое fd, char *buf, size_t len, uint32_t timeout_us);

typedef целое (*open_uart_func_t)(uint8_t, speed_t);
typedef целое (*write_uart_func_t)(целое, const void *, size_t);
typedef целое (*read_uart_func_t)(целое,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
