#pragma once

#ifdef __cplusplus
ବାହ୍ୟ "C" {
#endif

#ଅନ୍ତର୍ଭାଵ <stdbool.h>
#ଅନ୍ତର୍ଭାଵ <termios.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_get_port(ଅଚଳ ଅକ୍ଷର *dev);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_open(ଅଚଳ ଅକ୍ଷର *dev, speed_t speed);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *buf, size_t len);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା qurt_uart_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର *buf, size_t len, uint32_t timeout_us);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*open_uart_func_t)(uint8_t, speed_t);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*write_uart_func_t)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ଶୂନ୍ୟ *, size_t);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*read_uart_func_t)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା,  ଶୂନ୍ୟ *, size_t);

ଶୂନ୍ୟ configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
