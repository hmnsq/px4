#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

ကိန်းပြည့် qurt_uart_get_port(const var_na *dev);
ကိန်းပြည့် qurt_uart_open(const var_na *dev, speed_t speed);
ကိန်းပြည့် qurt_uart_write(ကိန်းပြည့် fd, const var_na *buf, size_t len);
ကိန်းပြည့် qurt_uart_read(ကိန်းပြည့် fd, var_na *buf, size_t len, uint32_t timeout_us);

typedef ကိန်းပြည့် (*open_uart_func_t)(uint8_t, speed_t);
typedef ကိန်းပြည့် (*write_uart_func_t)(ကိန်းပြည့်, const void *, size_t);
typedef ကိန်းပြည့် (*read_uart_func_t)(ကိန်းပြည့်,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
