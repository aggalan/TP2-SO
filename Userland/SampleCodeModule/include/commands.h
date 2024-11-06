// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define EOF -1

void cmd_help(char *args);
void cmd_tests();
void cmd_eliminator(char *args);
void cmd_time(char *args);
void cmd_setFont(char *args);
void cmd_kill(char *args);
void cmd_annihilate(char *args);
void cmd_block(char *args);
void cmd_unblock(char *args);
void cmd_changeprio(char *args);
void mm_test(char *args);
void process_test(char *args);
void prio_test(char *args);
void pipe_test(char *args);
void cmd_set_font(char *args);
void cmd_print_registers();
void cmd_ps();
void sync_test(char *args);
void busy_wait();
void call_div0();
void call_invalid_op();
char re_size(char *buffer);
void cat_process(char *args);
void filter_process(char *args);
void wc_process(char *args);
void cat();
int is_vowel(char c);
void filter();
void wc();
void phylo_init_process(char *args);

#endif