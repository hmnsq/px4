#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

ចំនួនគត់ qurt_uart_get_port(const var_na *dev);
ចំនួនគត់ qurt_uart_open(const var_na *dev, speed_t speed);
ចំនួនគត់ qurt_uart_write(ចំនួនគត់ fd, const var_na *buf, size_t len);
ចំនួនគត់ qurt_uart_read(ចំនួនគត់ fd, var_na *buf, size_t len, uint32_t timeout_us);

typedef ចំនួនគត់ (*open_uart_func_t)(uint8_t, speed_t);
typedef ចំនួនគត់ (*write_uart_func_t)(ចំនួនគត់, const void *, size_t);
typedef ចំនួនគត់ (*read_uart_func_t)(ចំនួនគត់,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
