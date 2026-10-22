#pragma once

#ifdef __cplusplus
বাহ্যিক "C" {
#endif

#অন্তর্ভাব <stdbool.h>
#অন্তর্ভাব <termios.h>

পূর্ণসংখ্যা qurt_uart_get_port(ধ্রুবক অক্ষর *dev);
পূর্ণসংখ্যা qurt_uart_open(ধ্রুবক অক্ষর *dev, speed_t speed);
পূর্ণসংখ্যা qurt_uart_write(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *buf, size_t len);
পূর্ণসংখ্যা qurt_uart_read(পূর্ণসংখ্যা fd, অক্ষর *buf, size_t len, uint32_t timeout_us);

প্রকারলেখ পূর্ণসংখ্যা (*open_uart_func_t)(uint8_t, speed_t);
প্রকারলেখ পূর্ণসংখ্যা (*write_uart_func_t)(পূর্ণসংখ্যা, ধ্রুবক শূন্য *, size_t);
প্রকারলেখ পূর্ণসংখ্যা (*read_uart_func_t)(পূর্ণসংখ্যা,  শূন্য *, size_t);

শূন্য configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
