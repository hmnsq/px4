#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

puur_naa_nka qurt_uart_get_port(const var_na *dev);
puur_naa_nka qurt_uart_open(const var_na *dev, speed_t speed);
puur_naa_nka qurt_uart_write(puur_naa_nka fd, const var_na *buf, size_t len);
puur_naa_nka qurt_uart_read(puur_naa_nka fd, var_na *buf, size_t len, uint32_t timeout_us);

typedef puur_naa_nka (*open_uart_func_t)(uint8_t, speed_t);
typedef puur_naa_nka (*write_uart_func_t)(puur_naa_nka, const void *, size_t);
typedef puur_naa_nka (*read_uart_func_t)(puur_naa_nka,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
