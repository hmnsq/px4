#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

цяло qurt_uart_get_port(const char *dev);
цяло qurt_uart_open(const char *dev, speed_t speed);
цяло qurt_uart_write(цяло fd, const char *buf, size_t len);
цяло qurt_uart_read(цяло fd, char *buf, size_t len, uint32_t timeout_us);

typedef цяло (*open_uart_func_t)(uint8_t, speed_t);
typedef цяло (*write_uart_func_t)(цяло, const void *, size_t);
typedef цяло (*read_uart_func_t)(цяло,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
