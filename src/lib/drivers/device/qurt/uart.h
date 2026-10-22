#pragma once

#ifdef __cplusplus
ബാഹ്യ "C" {
#endif

#അന്തര്ഭാവ <stdbool.h>
#അന്തര്ഭാവ <termios.h>

പൂർണ്ണസംഖ്യ qurt_uart_get_port(സ്ഥിരം അക്ഷരം *dev);
പൂർണ്ണസംഖ്യ qurt_uart_open(സ്ഥിരം അക്ഷരം *dev, speed_t speed);
പൂർണ്ണസംഖ്യ qurt_uart_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *buf, size_t len);
പൂർണ്ണസംഖ്യ qurt_uart_read(പൂർണ്ണസംഖ്യ fd, അക്ഷരം *buf, size_t len, uint32_t timeout_us);

തരനിർവചനം പൂർണ്ണസംഖ്യ (*open_uart_func_t)(uint8_t, speed_t);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*write_uart_func_t)(പൂർണ്ണസംഖ്യ, സ്ഥിരം ശൂന്യം *, size_t);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*read_uart_func_t)(പൂർണ്ണസംഖ്യ,  ശൂന്യം *, size_t);

ശൂന്യം configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
