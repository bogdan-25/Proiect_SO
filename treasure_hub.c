#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>

#define BUFFER_SIZE 256

int main()
{
    char input[BUFFER_SIZE];
    int pipe_cmd[2]; 
    int pipe_output[2];
    pid_t monitor_pid = -1;

    printf("Treasure Hub\n");
    printf("Type 'start_monitor' to run.\n");

    while (1)
    {
        printf(">> ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "start_monitor") == 0)
        {
            if (monitor_pid > 0) {
                printf("Monitor already running.\n");
                continue;
            }

            if (pipe(pipe_cmd) == -1 || pipe(pipe_output) == -1) {
                perror("pipe");
                exit(1);
            }

            monitor_pid = fork();
            if (monitor_pid == 0)
            {
                dup2(pipe_cmd[0], STDIN_FILENO);
                dup2(pipe_output[1], STDOUT_FILENO);
                dup2(pipe_output[1], STDERR_FILENO);
                close(pipe_cmd[0]); close(pipe_cmd[1]);
                close(pipe_output[0]); close(pipe_output[1]);
                execl("./monitor", "monitor", NULL);
                perror("execl monitor failed");
                exit(1);
            }
            else if (monitor_pid < 0)
            {
                perror("fork failed");
                exit(1);
            }
            close(pipe_cmd[0]); 
            close(pipe_output[1]);

            printf("Monitor started successfully. Command list:\n");
            printf("- list_hunts\n");
            printf("- list_treasures <hunt_id>\n");
            printf("- view_treasure <hunt_id> <treasure_id>\n");
            printf("- calculate_score <hunt_id>\n");
            printf("- stop_monitor\n");
            printf("- exit\n");
        }
        else if (strcmp(input, "stop_monitor") == 0)
        {
            if (monitor_pid > 0) {
                dprintf(pipe_cmd[1], "stop_monitor\n");
                waitpid(monitor_pid, NULL, 0);
                close(pipe_cmd[1]);
                close(pipe_output[0]);
                monitor_pid = -1;
                printf("Monitor stopped.\n");
            } else {
                printf("Monitor is not running.\n");
            }
        }
        else if (strcmp(input, "exit") == 0)
        {
            if (monitor_pid > 0) {
                printf("Please stop the monitor first using 'stop_monitor'.\n");
            } else {
                printf("Exiting Treasure Hub.\n");
                break;
            }
        }
        else if (monitor_pid > 0)
        {
            dprintf(pipe_cmd[1], "%s\n", input);
            fd_set readfds;
            struct timeval timeout;
            FD_ZERO(&readfds);
            FD_SET(pipe_output[0], &readfds);
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            if (select(pipe_output[0] + 1, &readfds, NULL, NULL, &timeout) > 0) {
                char buf[BUFFER_SIZE];
                ssize_t n;
                while ((n = read(pipe_output[0], buf, sizeof(buf) - 1)) > 0) {
                    buf[n] = '\0';
                    printf("%s", buf);
                    if (n < BUFFER_SIZE - 1) break;
                }
            } else {
                printf("Error talking to monitor.\n");
            }
        }
        else
        {
            printf("Please start the monitor first using 'start_monitor'.\n");
        }
    }

    return 0;
}
