#pragma once

#ifdef __cplusplus
बाह्य "C" {
#endif

#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <termios.h>

पूर्णांक qurt_uart_get_port(स्थिरांक अक्षर *dev);
पूर्णांक qurt_uart_open(स्थिरांक अक्षर *dev, speed_t speed);
पूर्णांक qurt_uart_write(पूर्णांक fd, स्थिरांक अक्षर *buf, माप_प्रकार len);
पूर्णांक qurt_uart_read(पूर्णांक fd, अक्षर *buf, माप_प्रकार len, uint32_t timeout_us);

प्रकारव्याख्या पूर्णांक (*open_uart_func_t)(uint8_t, speed_t);
प्रकारव्याख्या पूर्णांक (*write_uart_func_t)(पूर्णांक, स्थिरांक शून्य *, माप_प्रकार);
प्रकारव्याख्या पूर्णांक (*read_uart_func_t)(पूर्णांक,  शून्य *, माप_प्रकार);

शून्य configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#ifdef __cplusplus
}
#endif // __cplusplus
