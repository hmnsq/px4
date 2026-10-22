#pragma once

#ifdef __cplusplus
বাহ্য "C" {
#endif

#অন্তর্ভাব <stdbool.h>
#অন্তর্ভাব <termios.h>

পূর্ণাঙ্ক qurt_uart_get_port(ধ্রুব বর্ণ *dev);
পূর্ণাঙ্ক qurt_uart_open(ধ্রুব বর্ণ *dev, speed_t speed);
পূর্ণাঙ্ক qurt_uart_write(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *buf, size_t len);
পূর্ণাঙ্ক qurt_uart_read(পূর্ণাঙ্ক fd, বর্ণ *buf, size_t len, uint32_t timeout_us);

প্রকারনির্দেশ পূর্ণাঙ্ক (*open_uart_func_t)(uint8_t, speed_t);
প্রকারনির্দেশ পূর্ণাঙ্ক (*write_uart_func_t)(পূর্ণাঙ্ক, ধ্রুব শূন্য *, size_t);
প্রকারনির্দেশ পূর্ণাঙ্ক (*read_uart_func_t)(পূর্ণাঙ্ক,  শূন্য *, size_t);

শূন্য configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
