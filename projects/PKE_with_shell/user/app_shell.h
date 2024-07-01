#include "user_lib.h"
#include "string.h"
#include "util/types.h"

#define MAX_NAME_LENGTH 64
#define MAX_CMDLINE_ARGS 64
#define MAX_PARA_LENGTH 64
#define MAX_ECHO_LENGTH 64
#define MAX_CAT_LENGTH 64

void read_cmd(char *cmd, char *para);
int do_cmd(char *cmd, char *para);
void shell_cd(char *para);
void shell_mkdir(char *para);
void shell_ls(char *para);
void shell_touch(char *para);
void shell_echo(char *para);
void shell_cat(char *para);
void shell_exec(char *para);
