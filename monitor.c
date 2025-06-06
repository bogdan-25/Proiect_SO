#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

typedef struct{
  char treasure_id[50];
  char username[32];
  float latitude;
  float longitude;
  char clue[256];
  int value;
} Treasure;

void call_manager(const char *fmt, ...)
{
    char cmd[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(cmd, sizeof(cmd), fmt, ap);
    va_end(ap);

    char *argv[16];
    int argc = 0;
    char *tok = strtok(cmd, " ");
    while (tok && argc < 15) {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    } else if (pid == 0) {
        // child
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(1);
    }
    close(pipefd[1]);
    int status;
    waitpid(pid, &status, 0);

    char buf[256];
    ssize_t n;
    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    close(pipefd[0]);

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Command failed with exit code %d\n", WEXITSTATUS(status));
    }
}

int main()
{
    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;

        char cmd[32], arg1[64], arg2[64];
        int num_tokens = sscanf(line, "%s %s %s", cmd, arg1, arg2);

        if (strcmp(cmd, "list_hunts") == 0 && num_tokens == 1) {
            call_manager("./treasure_manager --list_hunts");
        }
        else if (strcmp(cmd, "list_treasures") == 0 && num_tokens == 2) {
            call_manager("./treasure_manager --list %s", arg1);
        }
        else if (strcmp(cmd, "view_treasure") == 0 && num_tokens == 3) {
            call_manager("./treasure_manager --view %s %s", arg1, arg2);
        }
	else if (strcmp(cmd, "calculate_score") == 0 && num_tokens == 2) {
	  call_manager("./score_calculator --hunt %s", arg1);
	}
	else if (strcmp(cmd, "stop_monitor") == 0) {
            printf("Stopping monitor...\n");
            break;
        }
        else {
            printf("Invalid or incomplete command: %s\n", line);
	    fflush(stdout);
        }
    }

    return 0;
}
