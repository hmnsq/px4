#pragma once

#ifdef __cplusplus
બાહ્ય "C" {
#endif

#અન્તર્ભાવ <stdbool.h>
#અન્તર્ભાવ <termios.h>

પૂર્ણાંક qurt_uart_get_port(અચલ અક્ષર *dev);
પૂર્ણાંક qurt_uart_open(અચલ અક્ષર *dev, speed_t speed);
પૂર્ણાંક qurt_uart_write(પૂર્ણાંક fd, અચલ અક્ષર *buf, size_t len);
પૂર્ણાંક qurt_uart_read(પૂર્ણાંક fd, અક્ષર *buf, size_t len, uint32_t timeout_us);

પ્રકારવ્યાખ્યા પૂર્ણાંક (*open_uart_func_t)(uint8_t, speed_t);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*write_uart_func_t)(પૂર્ણાંક, અચલ શૂન્ય *, size_t);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*read_uart_func_t)(પૂર્ણાંક,  શૂન્ય *, size_t);

શૂન્ય configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
