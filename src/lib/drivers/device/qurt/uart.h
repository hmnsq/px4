#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

ຈຳນວນເຕັມ qurt_uart_get_port(const var_na *dev);
ຈຳນວນເຕັມ qurt_uart_open(const var_na *dev, speed_t speed);
ຈຳນວນເຕັມ qurt_uart_write(ຈຳນວນເຕັມ fd, const var_na *buf, size_t len);
ຈຳນວນເຕັມ qurt_uart_read(ຈຳນວນເຕັມ fd, var_na *buf, size_t len, uint32_t timeout_us);

typedef ຈຳນວນເຕັມ (*open_uart_func_t)(uint8_t, speed_t);
typedef ຈຳນວນເຕັມ (*write_uart_func_t)(ຈຳນວນເຕັມ, const void *, size_t);
typedef ຈຳນວນເຕັມ (*read_uart_func_t)(ຈຳນວນເຕັມ,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
