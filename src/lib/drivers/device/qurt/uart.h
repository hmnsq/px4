#pragma once

#ifdef __cplusplus
बाह्य "C" {
#endif

#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <termios.h>

पूर्णाङ्क qurt_uart_get_port(ध्रुव वर्ण *dev);
पूर्णाङ्क qurt_uart_open(ध्रुव वर्ण *dev, speed_t speed);
पूर्णाङ्क qurt_uart_write(पूर्णाङ्क fd, ध्रुव वर्ण *buf, माप_प्रकार len);
पूर्णाङ्क qurt_uart_read(पूर्णाङ्क fd, वर्ण *buf, माप_प्रकार len, uint32_t timeout_us);

प्रकारनिर्देश पूर्णाङ्क (*open_uart_func_t)(uint8_t, speed_t);
प्रकारनिर्देश पूर्णाङ्क (*write_uart_func_t)(पूर्णाङ्क, ध्रुव शून्य *, माप_प्रकार);
प्रकारनिर्देश पूर्णाङ्क (*read_uart_func_t)(पूर्णाङ्क,  शून्य *, माप_प्रकार);

शून्य configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
