#pragma once

#ifdef __cplusplus
வெளி "C" {
#endif

#அந்தர்பாவ <stdbool.h>
#அந்தர்பாவ <termios.h>

முழு qurt_uart_get_port(மாறிலி எழுத்து *dev);
முழு qurt_uart_open(மாறிலி எழுத்து *dev, speed_t speed);
முழு qurt_uart_write(முழு fd, மாறிலி எழுத்து *buf, size_t len);
முழு qurt_uart_read(முழு fd, எழுத்து *buf, size_t len, uint32_t timeout_us);

வகைவரை முழு (*open_uart_func_t)(uint8_t, speed_t);
வகைவரை முழு (*write_uart_func_t)(முழு, மாறிலி வெறுமை *, size_t);
வகைவரை முழு (*read_uart_func_t)(முழு,  வெறுமை *, size_t);

வெறுமை configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
