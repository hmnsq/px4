#pragma once

#ifdef __cplusplus
ಬಾಹ್ಯ "C" {
#endif

#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <termios.h>

ಪೂರ್ಣಾಙ್ಕ qurt_uart_get_port(ಧ್ರುವ ವರ್ಣ *dev);
ಪೂರ್ಣಾಙ್ಕ qurt_uart_open(ಧ್ರುವ ವರ್ಣ *dev, speed_t speed);
ಪೂರ್ಣಾಙ್ಕ qurt_uart_write(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *buf, size_t len);
ಪೂರ್ಣಾಙ್ಕ qurt_uart_read(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ *buf, size_t len, uint32_t timeout_us);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*open_uart_func_t)(uint8_t, speed_t);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*write_uart_func_t)(ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಶೂನ್ಯ *, size_t);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*read_uart_func_t)(ಪೂರ್ಣಾಙ್ಕ,  ಶೂನ್ಯ *, size_t);

ಶೂನ್ಯ configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
