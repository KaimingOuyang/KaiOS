#include <io.h>
#include <tty.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <asmfunc.h>

#define STATUS_REGISTER_A 0x3F0 // read-only
#define STATUS_REGISTER_B 0x3F1 // read-only
#define DIGITAL_OUTPUT_REGISTER 0x3F2
#define TAPE_DRIVE_REGISTER 0x3F3
#define MAIN_STATUS_REGISTER 0x3F4 // read-only
#define DATARATE_SELECT_REGISTER 0x3F4 // write-only
#define DATA_FIFO 0x3F5
#define DIGITAL_INPUT_REGISTER 0x3F7 // read-only
#define CONFIGURATION_CONTROL_REGISTER 0x3F7 // write-only

#define READ_TRACK 2	// generates IRQ6
#define SPECIFY 3      // * set drive parameters
#define SENSE_DRIVE_STATUS 4
#define WRITE_DATA 5      // * write to the disk
#define READ_DATA 6      // * read from the disk
#define RECALIBRATE 7      // * seek to cylinder 0
#define SENSE_INTERRUPT 8      // * ack IRQ6, get status of last command
#define WRITE_DELETED_DATA 9
#define READ_ID 10	// generates IRQ6
#define READ_DELETED_DATA 12
#define FORMAT_TRACK 13     // *
#define SEEK 15     // * seek both heads to cylinder X
#define VERSION 16	// * used during initialization, once
#define SCAN_EQUAL 17
#define PERPENDICULAR_MODE 18	// * used during initialization, once, maybe
#define CONFIGURE 19     // * set controller parameters
#define LOCK 20     // * protect controller params from a reset
#define VERIFY 22
#define SCAN_LOW_OR_EQUAL 25
#define SCAN_HIGH_OR_EQUAL 29

#define MT 0x80
#define MF 0x40
#define SK 0x20
#define SRT 10 // 6ms delay between track seek
#define HLT 5 // 10ms delay for activating head
#define HUT 5 // 80ms delay for deactivating head
#define NDMA 1 // PIO off

#define CY_NUM 18432
#define HE_NUM 9216
#define SE_NUM 512
#define DMA_ADDR 0x1000

volatile bool floppy_irq;

static inline void wait_floppy_irq();
static void prepare_floppy_DMA_read(uint32_t sectorc);
static inline void floppy_send_cmd(uint8_t cmd);
static inline uint8_t floppy_read_byte();
static inline uint8_t floppy_MSR_status();
static inline void floppy_control_motor(bool st);
static uint32_t floppy_calibrate(uint8_t drive);
static uint32_t floppy_seek(uint8_t cyl, uint8_t head, uint8_t drive);
static inline void floppy_check_int(uint8_t* st, uint8_t* cy);
static inline void floppy_specify(uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma);

void io_init() {
    uint8_t st, cy;
    floppy_irq = false;
    _out8(DIGITAL_OUTPUT_REGISTER, 0x00); // reset
    _out8(DIGITAL_OUTPUT_REGISTER, 0x0c); // DMA and normal mode
    wait_floppy_irq();

    for(uint32_t index_1 = 0; index_1 < 4; index_1++)
        floppy_check_int(&st, &cy);

    _out8(CONFIGURATION_CONTROL_REGISTER, 0x00); // rotate speed 500Kbps
    // configure implied seek on, FIFO on, drive polling mode off, threshold = 8, precompensation 0
    floppy_specify(13, 1, 0xf, true);

    floppy_calibrate(0);
    return;
}


uint8_t* floppy_read(uint32_t addr, uint8_t sectorc) {
    uint8_t cylinder = addr / CY_NUM;
    uint8_t head = (addr % CY_NUM)  / HE_NUM;
    uint8_t sector = ((addr % CY_NUM)  % HE_NUM) / SE_NUM + 1;
    uint8_t st, cy;
    floppy_control_motor(true);

    if(floppy_seek(cylinder, head, 0) != 0)
        return 0;

    prepare_floppy_DMA_read(sectorc);
    floppy_send_cmd(MT | MF | SK | READ_DATA);
    floppy_send_cmd(head << 2);
    floppy_send_cmd(cylinder);
    floppy_send_cmd(head);
    floppy_send_cmd(sector);
    floppy_send_cmd(2);
    floppy_send_cmd(sectorc); // read sector count
    floppy_send_cmd(0x1b);
    floppy_send_cmd(0xff);

    wait_floppy_irq();
    floppy_check_int(&st, &cy);

    // result phase
    for(uint32_t index_1 = 0 ; index_1 < 7; index_1++)
        floppy_read_byte();

    floppy_control_motor(false);
    return (uint8_t*) DMA_ADDR;
}

static inline void floppy_send_cmd(uint8_t cmd) {
    for(int index_1 = 0; index_1 < 500; index_1++)
        if(floppy_MSR_status() & 0x80)
            return _out8(DATA_FIFO, cmd);
}

static inline uint8_t floppy_read_byte() {
    for(int index_1 = 0; index_1 < 500; index_1++)
        if(floppy_MSR_status() & 0x80)
            return _in8(DATA_FIFO);

    return -1;
}

static inline uint8_t floppy_MSR_status() {
    return _in8(MAIN_STATUS_REGISTER);
}

static inline void floppy_check_int(uint8_t* st, uint8_t* cy) {
    floppy_send_cmd(SENSE_INTERRUPT);
    *st = floppy_read_byte();
    *cy = floppy_read_byte();
    return;
}

static inline void floppy_specify(uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma) {
    uint32_t data = 0;
    floppy_send_cmd(SPECIFY);
    data = ((stepr & 0xf) << 4) | (unloadt & 0xf);
    floppy_send_cmd(data);
    data = (loadt) << 1 | (dma == true) ? 1 : 0;
    floppy_send_cmd(data);
    return;
}

static uint32_t floppy_seek(uint8_t cyl, uint8_t head, uint8_t drive) {
    uint8_t st0, cyl0;

    if(drive >= 4)
        return -1;

    for(int i = 0; i < 10; i++) {
        floppy_send_cmd(SEEK);
        floppy_send_cmd(head << 2 | drive);
        floppy_send_cmd(cyl);

        wait_floppy_irq();
        floppy_check_int(&st0, &cyl0);

        if(cyl0 == cyl)
            return 0;
    }

    return -1;
}

static uint32_t floppy_calibrate(uint8_t drive) {
    uint8_t st0, cy;

    if(drive >= 4)
        return -2;

    floppy_control_motor(true);

    for(uint32_t i = 0; i < 10; i++) {
        floppy_send_cmd(RECALIBRATE);
        floppy_send_cmd(drive);
        wait_floppy_irq();
        floppy_check_int(&st0, &cy);

        if(!cy) {
            floppy_control_motor(false);
            return 0;
        }
    }

    floppy_control_motor(false);
    return -1;
}

static inline void floppy_control_motor(bool st) {
    if(st == true) {
        _out8(DIGITAL_OUTPUT_REGISTER, 0x1c);

        for(uint32_t index_1 = 0; index_1 < 1000000; index_1++);
    } else
        _out8(DIGITAL_OUTPUT_REGISTER, 0x0c);

    return;
}

static void prepare_floppy_DMA_read(uint32_t sectorc) {
    uint32_t bytes = sectorc * 512;
    _out8(0x0a, 0x06); // mask channel 2

    _out8(0x0c, 0xff); // reset flip-flop
    _out8(0x04, 0); // buffer address low byte 0x001000
    _out8(0x81, 0); // buffer address page 0x80? *****
    _out8(0x04, 0x10); // buffer address high byte

    _out8(0x0c, 0xff); // reset flip-flop
    _out8(0x05, bytes & 0xff); // counter low byte
    _out8(0x0b, 0x46); // read not auto
    _out8(0x05, (bytes >> 8) & 0xff); // counter high byte

    _out8(0x0a, 0x02); // unmask channel 2
    return;
}

static inline void wait_floppy_irq() {
    while(!floppy_irq); // wait IRQ6

    floppy_irq = false;
}



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
                    // %s,%d,%u,%X,%h implementation
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
                    } else if(str[i + 1] == 'h') {
                        uint8_t hex_tmp = va_arg(arg_list, uint32_t);
                        char hex_str[3];

                        if((hex_tmp & 0xf) >= 0xa)
                            hex_str[1] = 'A' + (char)(hex_tmp & 0xf) - 0xa;
                        else
                            hex_str[1] = '0' + (char)(hex_tmp & 0xf);

                        hex_tmp >>= 4;

                        if((hex_tmp & 0xf) >= 0xa)
                            hex_str[0] = 'A' + (char)(hex_tmp & 0xf) - 0xa;
                        else
                            hex_str[0] = '0' + (char)(hex_tmp & 0xf);

                        hex_str[2] = '\0';
                        printf(hex_str);
                    }
                }
            }

            i++;
        } else if(str[i] == '\t') {
            // escape character \t
            for(uint32_t index_1 = 0; index_1 < 4; index_1++)
                tty_output_char(' ');
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


/*
static inline void wait_MSR_RQM_DIO() {
    uint8_t data;

    while(1) {
        data = _in8(MAIN_STATUS_REGISTER);

        if((data & 0xc0) == 0x80)
            break;
    }

    return;
}

static inline void wait_MSR_RQM_NDMA() {
    uint8_t data;

    while(1) {
        data = _in8(MAIN_STATUS_REGISTER);

        if((data & 0xa0) == 0xa0)
            break;
    }

    return;
}
*/
