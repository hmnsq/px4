#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

số_nguyên qurt_uart_get_port(const char *dev);
số_nguyên qurt_uart_open(const char *dev, speed_t speed);
số_nguyên qurt_uart_write(số_nguyên fd, const char *buf, size_t len);
số_nguyên qurt_uart_read(số_nguyên fd, char *buf, size_t len, uint32_t timeout_us);

typedef số_nguyên (*open_uart_func_t)(uint8_t, speed_t);
typedef số_nguyên (*write_uart_func_t)(số_nguyên, const void *, size_t);
typedef số_nguyên (*read_uart_func_t)(số_nguyên,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
