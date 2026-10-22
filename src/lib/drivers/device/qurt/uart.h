#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

цео qurt_uart_get_port(const char *dev);
цео qurt_uart_open(const char *dev, speed_t speed);
цео qurt_uart_write(цео fd, const char *buf, size_t len);
цео qurt_uart_read(цео fd, char *buf, size_t len, uint32_t timeout_us);

typedef цео (*open_uart_func_t)(uint8_t, speed_t);
typedef цео (*write_uart_func_t)(цео, const void *, size_t);
typedef цео (*read_uart_func_t)(цео,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
