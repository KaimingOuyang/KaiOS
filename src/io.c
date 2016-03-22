#include <io.h>
#include <tty.h>
#include <stdarg.h>
#include <string.h>
// temporary printf function, need to be modified formally
void printf(const char* str, ...) {
    va_list arg_list;
    va_start(arg_list, str);
    uint32_t len = strlen(str);

    for(uint32_t i = 0; i < len; i++) {
        if(str[i] == '%') {
            if(i + 1 < len) {
                if(str[i + 1] == '%') {
                    tty_output_char('%');
                } else {
                    // %s,%d,%u,%X implementation
                    if(str[i + 1] == 's') { // parser string
                        const char* arg_str;
                        arg_str = va_arg(arg_list, const char*);
                        // common code after memory management is finished
                        uint32_t inner_len = strlen(arg_str);

                        for(uint32_t j = 0; j < inner_len; j++)
                            tty_output_char(arg_str[j]);

                    } else if(str[i + 1] == 'd' || str[i + 1] == 'u') { // parser int
                        uint32_t arg_int = va_arg(arg_list, uint32_t);

                        if(str[i + 1] == 'd') {
                            if((arg_int & (1 << 31)) == 1) {
                                tty_output_char('-');
                                arg_int = ~arg_int + 1;
                            }
                        }

                        if(arg_int == 0)
                            tty_output_char('0');
                        else {
                            // save memory because length is not sure
                            uint32_t int_len = 0;
                            uint32_t int_tmp = arg_int;

                            while(int_tmp != 0) {
                                int_len++;
                                int_tmp /= 10;
                            }

                            int_tmp = arg_int;
                            // should be modified when memory management is finished
                            char int_buffer[int_len];

                            for(int32_t j = int_len; j > 0; j--) {
                                int_buffer[j - 1] = '0' + (char)(int_tmp % 10);
                                int_tmp /= 10;
                            }

                            for(uint32_t j = 0; j < int_len; j++)
                                tty_output_char(int_buffer[j]);
                        }

                        //arg_str = parser_int(va_arg(arg_list, int));
                    } else if(str[i + 1] == 'X') {
                        char hex_str[8];
                        uint32_t int_tmp = va_arg(arg_list, uint32_t);
                        tty_output_char('0');
                        tty_output_char('x');

                        for(uint32_t index_1 = 8; index_1 > 0; index_1--) {
                            if((int_tmp & 0xf) >= 0xa)
                                hex_str[index_1 - 1] = 'A' + (char)(int_tmp & 0xf) - 0xa;
                            else
                                hex_str[index_1 - 1] = '0' + (char)(int_tmp & 0xf);

                            int_tmp >>= 4;
                        }

                        for(uint32_t index_1 = 0; index_1 < 8; index_1++)
                            tty_output_char(hex_str[index_1]);
                    }
                }
            }

            i++;
        } else
            tty_output_char(str[i]);
    }

    update_cursor();
    return;
}

void putchar(const char c) {
    tty_output_char(c);
    update_cursor();
    return;
}
