#ਹਦਾਇਤ once

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <termios.h>

ਪੂਰਨ qurt_uart_get_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ *dev);
ਪੂਰਨ qurt_uart_open(ਅਚਲ ਅੱਖਰ *dev, speed_t ਰਫਤਾਰ);
ਪੂਰਨ qurt_uart_write(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *buf, size_t len);
ਪੂਰਨ qurt_uart_read(ਪੂਰਨ fd, ਅੱਖਰ *buf, size_t len, uint32_t timeout_us);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*open_uart_func_t)(uint8_t, speed_t);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*write_uart_func_t)(ਪੂਰਨ, ਅਚਲ ਖਾਲੀ *, size_t);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*read_uart_func_t)(ਪੂਰਨ,  ਖਾਲੀ *, size_t);

ਖਾਲੀ configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ // __cplusplus
