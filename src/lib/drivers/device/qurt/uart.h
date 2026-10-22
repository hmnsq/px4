#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

จำนวนเต็ม qurt_uart_get_port(const var_na *dev);
จำนวนเต็ม qurt_uart_open(const var_na *dev, speed_t speed);
จำนวนเต็ม qurt_uart_write(จำนวนเต็ม fd, const var_na *buf, size_t len);
จำนวนเต็ม qurt_uart_read(จำนวนเต็ม fd, var_na *buf, size_t len, uint32_t timeout_us);

typedef จำนวนเต็ม (*open_uart_func_t)(uint8_t, speed_t);
typedef จำนวนเต็ม (*write_uart_func_t)(จำนวนเต็ม, const void *, size_t);
typedef จำนวนเต็ม (*read_uart_func_t)(จำนวนเต็ม,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
