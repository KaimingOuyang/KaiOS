#include <io.h>
#include <tty.h>
#include <string.h>
#include <asmfunc.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>
#include <shell.h>
/*
struct PMInfoBlock{
    uint8_t signature[4];
    uint16_t entry_point;
    uint16_t init_point;
    uint16_t BIOS_data_sel;
    uint16_t sel_A0000;
    uint16_t sel_B0000;
    uint16_t sel_B8000;
    uint16_t code_sel;
    uint8_t in_pm_mode;
    uint8_t check_sum;
};
*/
/*
struct PMInfoBlock* head;

void adjust_resolution(uint32_t column, uint32_t row){
    const uint32_t* BIOS_CODE = (uint32_t*) 0x400000;
    load_BIOS_code(BIOS_CODE);
    head = (PMInfoBlock*) scan_BIOS_head();
    head->BIOS_data_sel = 2003 * 8;
    head->sel_A0000 = 2004 * 8;
    head->sel_B0000 = 2005 * 8;
    head->sel_B8000 = 2006 * 8;
    head->code_sel = 2002 * 8;

}
*/


/*
    tty depends on task, so anything tty should do must be done in task
*/

uint8_t tty_default_color;
extern bool insert_mode;
extern struct TaskAdmin* task_admin;
extern struct MemAdmin mem_admin;
extern uint32_t** kernel_page_directory;

static void tty_screen_up(struct Task* task);
static inline uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg);
static inline uint16_t tty_retchar(char c, uint8_t color);
static void tty_output_char_task(const char c, struct Task* task);

static char logo[] =
    "................................................................................"
    "................................................................................"
    "................................................................................"
    "................................................................................"
    "..........*****.....*****...**********...******........******..*****............"
    "............***.....***.........**.........**............**.....**.............."
    "............***.....***.........**.........**............**.....*..............."
    "............***.....***.........**.........**............**....*................"
    "............*.**...*.**.........**.........**............**...**................"
    "............*.**...*.**.........**.........**............**..**................."
    "............*.**...*.**.........**.........**............**..*.................."
    "............*.**...*.**.........**.........**............**.***................."
    "............*.***.*..**.........**.........**............***.**................."
    "............*..**.*..**.........**.........**............**..***................"
    "............*..**.*..**.........**.........**............**...**................"
    "............*..****..**.........**.........**............**....**..............."
    "............*..***...**.........**.........**............**....***.............."
    "............*...**...**.........**.........**........*...**.....**.............."
    "............*...**...**.........**.........**........*...**.....***............."
    "............*...**...**.........**.........**......**....**......**............."
    "..........*****.*..******...**********...************..******...*****..........."
    "................................................................................"
    "................................................................................"
    "................................................................................"
    "................................................................................";

volatile uint32_t logo_time;

void show_logo() {
    logo_time = 0;
    uint16_t* vram = (uint16_t*) VGAHEAD;
    tty_default_color = make_color(COLOR_LIGHT_GREY,COLOR_BLACK);
    for(uint32_t index_1 = 0; index_1 < 80 * 25; index_1++) {
        if(logo[index_1] == '.')
            vram[index_1] = tty_retchar(' ',tty_default_color);
        else
            vram[index_1] = tty_retchar(0x07,tty_default_color);
    }
    while(logo_time != 200);
    //while(1);
    return;
}

void tty_init(struct Task* task) {
    task->tty_pos_cur = 0;
    task->tty_pos_end = 0;
    task->tty_pos_start = 0;
    tty_default_color = make_color(COLOR_LIGHT_GREY,COLOR_BLACK); // should be annotated after whole finished
    tty_buffer_init(task);
    return;
}

void tty_buffer_init(struct Task* task) {
    for(uint32_t y = 0; y < VGAHEIGHT; y++)
        for(uint32_t x = 0; x < VGAWIDTH; x++) {
            uint32_t pos = y * VGAWIDTH + x;
            task->tty_buffer[pos] = tty_retchar(' ', tty_default_color);
        }

    return;
}

void make_title(const char* str, struct Task* task, uint8_t flag) {
    uint32_t len = strlen(str);

    if(flag == 1)
        task->tty_pos_start -= 2;

    for(uint32_t index_1 = 0; index_1 < len; index_1++)
        task->tty_buffer[task->tty_pos_start++] = tty_retchar(str[index_1], tty_default_color);

    task->tty_buffer[task->tty_pos_start++] = tty_retchar('#', tty_default_color);
    task->tty_buffer[task->tty_pos_start++] = tty_retchar(' ', tty_default_color);;
    task->tty_pos_end = task->tty_pos_cur = task->tty_pos_start;
    return;
}

void tty_backspace() {
    if(task_admin->tasks[0].tty_pos_cur > task_admin->tasks[0].tty_pos_start) {
        for(uint32_t i = task_admin->tasks[0].tty_pos_cur; i <= task_admin->tasks[0].tty_pos_end; i++)
            task_admin->tasks[0].tty_buffer[i - 1] = task_admin->tasks[0].tty_buffer[i];

        task_admin->tasks[0].tty_pos_cur--;
        task_admin->tasks[0].tty_pos_end--;
    }

    update_cursor();
    return;
}

void tty_left() {
    if(task_admin->tasks[0].tty_pos_cur > task_admin->tasks[0].tty_pos_start)
        task_admin->tasks[0].tty_pos_cur--;

    update_cursor();
    return;
}

void tty_right() {
    if(task_admin->tasks[0].tty_pos_cur < task_admin->tasks[0].tty_pos_end)
        task_admin->tasks[0].tty_pos_cur++;

    update_cursor();
    return;
}

void tty_home() {
    task_admin->tasks[0].tty_pos_cur = task_admin->tasks[0].tty_pos_start;
    update_cursor();
    return;
}

void tty_end() {
    task_admin->tasks[0].tty_pos_cur = task_admin->tasks[0].tty_pos_end;
    update_cursor();
    return;
}

void tty_delete() {
    if(task_admin->tasks[0].tty_pos_cur != task_admin->tasks[0].tty_pos_end) {
        for(uint32_t i = task_admin->tasks[0].tty_pos_cur + 1; i <= task_admin->tasks[0].tty_pos_end; i++)
            task_admin->tasks[0].tty_buffer[i - 1] = task_admin->tasks[0].tty_buffer[i];

        task_admin->tasks[0].tty_pos_end--;
    }

    return;
}

/*
void tty_up(){

}

void tty_down(){

}
*/

void tty_enter() {
    uint32_t com_len = task_admin->tasks[0].tty_pos_end - task_admin->tasks[0].tty_pos_start;

    if(com_len == 0) {
        printf("\n");
        make_title(MACHINE_TITLE, &task_admin->tasks[0], 0);
    } else {
        char com_buffer[com_len];

        for(uint32_t i = 0; i < com_len; i++)
            com_buffer[i] = task_admin->tasks[0].tty_buffer[task_admin->tasks[0].tty_pos_start + i] & 0x00ff;

        com_buffer[com_len] = '\0';
        command_line_parser(com_buffer);
    }

    update_cursor();
    return;
}



void tty_new_terminal(uint32_t id) {
    uint32_t show_id = task_admin->show_id;
    //printf("id:%d", id);

    if(id == show_id)
        return;

    bool on = false;

    if(task_admin->visit[id] == false) {
        task_init(&task_admin->tasks[id], id * 8, id, (uint32_t)kernel_alloc(sizeof(uint16_t) * VGAHEIGHT * VGAWIDTH),
                  KERNEL_DATA_SEGMENT * 8, KERNEL_CODE_SEGMENT * 8, (uint32_t)kernel_alloc(64 * (1 << 10)) + 64 * (1 << 10),
                  (uint32_t)&task_begin, (uint32_t)get_kernel_pagedir());

        _cli();

        if(task_admin->ready_end == NULL) {
            task_admin->ready_end = &task_admin->tasks[id];
            task_admin->ready_head = &task_admin->tasks[id];
        } else {
            task_admin->ready_end->next_ready = &task_admin->tasks[id];
            task_admin->ready_end = &task_admin->tasks[id];
        }

        on = true;

    }

    if(on == false)
        _cli();

    for(uint32_t index_1 = 0; index_1 < VGAHEIGHT * VGAWIDTH; index_1++) {
        task_admin->tasks[show_id].tty_buffer[index_1] = task_admin->tasks[0].tty_buffer[index_1];
        task_admin->tasks[0].tty_buffer[index_1] = task_admin->tasks[id].tty_buffer[index_1];
    }

    task_admin->tasks[show_id].tty_pos_cur = task_admin->tasks[0].tty_pos_cur;
    task_admin->tasks[show_id].tty_pos_start = task_admin->tasks[0].tty_pos_start;
    task_admin->tasks[show_id].tty_pos_end = task_admin->tasks[0].tty_pos_end;
    task_admin->tasks[0].tty_pos_cur = task_admin->tasks[id].tty_pos_cur;
    task_admin->tasks[0].tty_pos_start = task_admin->tasks[id].tty_pos_start;
    task_admin->tasks[0].tty_pos_end = task_admin->tasks[id].tty_pos_end;
    task_admin->show_id = id;
    _sti();
    update_cursor();
    return;

}

void update_cursor() {
    uint16_t position = task_admin->tasks[0].tty_pos_cur;
    // cursor LOW port to vga INDEX register
    _out8(0x3D4, 0x0F);
    _out8(0x3D5, (uint8_t)(position & 0xFF));
    // cursor HIGH port to vga INDEX register
    _out8(0x3D4, 0x0E);
    _out8(0x3D5, (uint8_t)((position >> 8) & 0xFF));
    return;
}

void tty_output_char(const char c) {
    uint32_t show_id = task_admin->show_id;
    uint32_t cur_id = task_admin->running->task_id;

    if(show_id == cur_id) {
        tty_output_char_task(c, &task_admin->tasks[0]);
    } else
        tty_output_char_task(c, &task_admin->tasks[cur_id]);

    return;
}

static void tty_screen_up(struct Task * task) {
    for(uint32_t i = 1; i < VGAHEIGHT; i++)
        for(uint32_t j = 0; j < VGAWIDTH; j++) {
            uint32_t pos = i * VGAWIDTH + j;
            uint32_t rep = (i - 1) * VGAWIDTH + j;
            task->tty_buffer[rep] = task->tty_buffer[pos];
        }

    for(uint32_t j = 0; j < VGAWIDTH; j++) {
        uint32_t pos = (VGAHEIGHT - 1) * VGAWIDTH + j;
        task->tty_buffer[pos] = tty_retchar(' ', tty_default_color);
    }

    task->tty_pos_start -= VGAWIDTH;
    task->tty_pos_end -= VGAWIDTH;
    task->tty_pos_cur -= VGAWIDTH;
    return;
}

static void tty_output_char_task(const char c, struct Task* task) {
    if(c == '\n') {
        uint32_t cur_row;
        cur_row = task->tty_pos_cur / VGAWIDTH;

        if(cur_row == VGAHEIGHT - 1)
            tty_screen_up(task);
        else
            cur_row++;

        task->tty_pos_cur = cur_row * VGAWIDTH;
        task->tty_pos_start = task->tty_pos_end = task->tty_pos_cur;
    } else {
        if(insert_mode)
            for(uint32_t index_1 = task->tty_pos_end; index_1 > task->tty_pos_cur; index_1--)
                task->tty_buffer[index_1] = task->tty_buffer[index_1 - 1];

        task->tty_buffer[task->tty_pos_cur] = tty_retchar(c, tty_default_color);

        if(insert_mode || (task->tty_pos_cur == task->tty_pos_end))
            task->tty_pos_end++;

        task->tty_pos_cur++;

        if(task->tty_pos_end == VGAWIDTH * VGAHEIGHT)
            tty_screen_up(task);
    }
}

static inline uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg) {
    return fg | bg << 4;
}

static inline uint16_t tty_retchar(char c, uint8_t color) {
    return color << 8 | c;
}
