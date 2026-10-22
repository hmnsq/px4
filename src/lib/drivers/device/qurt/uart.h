#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

ціле qurt_uart_get_port(const char *dev);
ціле qurt_uart_open(const char *dev, speed_t speed);
ціле qurt_uart_write(ціле fd, const char *buf, size_t len);
ціле qurt_uart_read(ціле fd, char *buf, size_t len, uint32_t timeout_us);

typedef ціле (*open_uart_func_t)(uint8_t, speed_t);
typedef ціле (*write_uart_func_t)(ціле, const void *, size_t);
typedef ціле (*read_uart_func_t)(ціле,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
