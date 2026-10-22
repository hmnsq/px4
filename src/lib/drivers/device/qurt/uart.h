#సూచన once

#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు

#అన్తర్భావ <stdbool.h>
#అన్తర్భావ <termios.h>

పూర్ణాంకం qurt_uart_get_పోర్టు(స్థిరరాశి అక్షరం *dev);
పూర్ణాంకం qurt_uart_open(స్థిరరాశి అక్షరం *dev, speed_t వేగం);
పూర్ణాంకం qurt_uart_write(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *buf, size_t len);
పూర్ణాంకం qurt_uart_read(పూర్ణాంకం fd, అక్షరం *buf, size_t len, uint32_t timeout_us);

రకనిర్వచనం పూర్ణాంకం (*open_uart_func_t)(uint8_t, speed_t);
రకనిర్వచనం పూర్ణాంకం (*write_uart_func_t)(పూర్ణాంకం, స్థిరరాశి శూన్య *, size_t);
రకనిర్వచనం పూర్ణాంకం (*read_uart_func_t)(పూర్ణాంకం,  శూన్య *, size_t);

శూన్య configure_uart_callbacks(open_uart_func_t, write_uart_func_t, read_uart_func_t);

#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు // __cplusplus
