/*
 * This app starts a very simple shell and executes some simple cmds.
 * The cmds are stored in the hostfs_root/shellrc
 * The shell loads the file and executes the cmd line by line.                 
 */
#include "user_lib.h"
#include "string.h"
#include "util/types.h"
#include "app_shell.h"

int main(int argc, char *argv[])
{
	printu("Hart 0 running shell\n");
	change_cwd("/RAMDISK0");

	// 默认字符串
	char userinfo[MAX_NAME_LENGTH] = "mylaptop@peacewang";

	// 输入
	char cmd[MAX_CMDLINE_ARGS];
	char para[MAX_PARA_LENGTH];

	// cwd
	char cwd[MAX_PATH_LEN];
	read_cwd(cwd);

	while (1) {
		// 输出引导信息 用户名：路径
		read_cwd(cwd);
		printu("%s:%s$", userinfo, cwd);

		// 读指令
		read_cmd(cmd, para);

		// 执行
		do_cmd(cmd, para);
	}

	exit(0);
	return 0;
}

// 1. 只有一个参数
void read_cmd(char *cmd, char *para)
{
	cmd[0] = '\0';
	para[0] = '\0';
	scanu("%s %s", cmd, para);
	return;
}

int do_cmd(char *cmd, char *para)
{
	if (strcmp(cmd, "cd") == 0) {
		shell_cd(para);
		return 0;
	}
	if (strcmp(cmd, "mkdir") == 0) {
		shell_mkdir(para);
		return 0;
	}
	if (strcmp(cmd, "ls") == 0) {
		shell_ls(para);
		return 0;
	}
	if (strcmp(cmd, "touch") == 0) {
		shell_touch(para);
		return 0;
	}
	if (strcmp(cmd, "echo") == 0) {
		shell_echo(para);
		return 0;
	}
	if (strcmp(cmd, "cat") == 0) {
		shell_cat(para);
		return 0;
	}
	if (strcmp(cmd, "exec") == 0) {
		shell_exec(para);
		return 0;
	}
	printu("%s: cmd not found.\n", cmd);
	return -1;
}

void shell_cd(char *para)
{
	change_cwd(para); // 1 出错，0 正常
	return;
}

void shell_mkdir(char *para)
{
	mkdir_u(para);
	return;
}

void shell_ls(char *para)
{
	int dir_fd = opendir_u(para);
	printu("[name]               [inode_num]\n");
	struct dir dir;
	int width = 20;
	while (readdir_u(dir_fd, &dir) == 0) {
		// we do not have %ms :(
		char name[width + 1];
		memset(name, ' ', width + 1);
		name[width] = '\0';
		if (strlen(dir.name) < width) {
			strcpy(name, dir.name);
			name[strlen(dir.name)] = ' ';
			printu("%s %d\n", name, dir.inum);
		} else
			printu("%s %d\n", dir.name, dir.inum);
	}
	closedir_u(dir_fd);
	return;
}

void shell_touch(char *para)
{
	int fd;
	fd = open(para, O_CREAT);
	printu("file descriptor fd: %d\n", fd);
	close(fd);
	return;
}

void shell_echo(char *para)
{
	int fd = open(para, O_RDWR | O_CREAT);
	char str[MAX_ECHO_LENGTH];
	str[0] = '\0';
	printu("Enter echo string:\n");
	scanu("%s", str);
	write_u(fd, str, strlen(str));
	close(fd);
	return;
}

void shell_cat(char *para)
{
	int fd;
	char buf[MAX_CAT_LENGTH];
	fd = open(para, O_RDWR);
	read_u(fd, buf, MAX_CAT_LENGTH);
	printu("Read content:\n%s\n", buf);
	close(fd);
	return;
}

void shell_exec(char *para)
{
	char str[MAX_PARA_LENGTH];
	printu("Enter exec argument:\n");
	scanu("%s",
	      str); // 我们实现的fork非常脆弱，所以不能在fork后的子进程中进行标准化输入
	int pid = fork();
	if (pid == 0) {
		int ret = exec(para, str);
		if (ret == -1) {
			printu("exec failed\n");
		}
	} else {
		wait(pid);
	}
	return;
}