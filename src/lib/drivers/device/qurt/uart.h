#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <termios.h>

පූර්ණ_සංඛ්‍යා qurt_uart_get_port(const var_na *dev);
පූර්ණ_සංඛ්‍යා qurt_uart_open(const var_na *dev, speed_t speed);
පූර්ණ_සංඛ්‍යා qurt_uart_write(පූර්ණ_සංඛ්‍යා fd, const var_na *buf, size_t len);
පූර්ණ_සංඛ්‍යා qurt_uart_read(පූර්ණ_සංඛ්‍යා fd, var_na *buf, size_t len, uint32_t timeout_us);

typedef පූර්ණ_සංඛ්‍යා (*open_uart_func_t)(uint8_t, speed_t);
typedef පූර්ණ_සංඛ්‍යා (*write_uart_func_t)(පූර්ණ_සංඛ්‍යා, const void *, size_t);
typedef පූර්ණ_සංඛ්‍යා (*read_uart_func_t)(පූර්ණ_සංඛ්‍යා,  void *, size_t);

void configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
