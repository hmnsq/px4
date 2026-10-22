#ಸೂಚನೆ once

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <termios.h>

ಪೂರ್ಣಾಂಕ qurt_uart_get_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dev);
ಪೂರ್ಣಾಂಕ qurt_uart_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dev, speed_t ವೇಗ);
ಪೂರ್ಣಾಂಕ qurt_uart_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t len);
ಪೂರ್ಣಾಂಕ qurt_uart_read(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ *buf, size_t len, uint32_t timeout_us);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*open_uart_func_t)(uint8_t, speed_t);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*write_uart_func_t)(ಪೂರ್ಣಾಂಕ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, size_t);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*read_uart_func_t)(ಪೂರ್ಣಾಂಕ,  ಶೂನ್ಯ *, size_t);

ಶೂನ್ಯ configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ // __cplusplus
