#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMD_FILE "cli.txt"

int main()
{
    char input[256];
    pid_t monitor_pid = -1;
    FILE *cmd_filepath;

    printf("Treasure Hub\n");
    printf("Type 'start_monitor' to run.\n");

    while (1)
      {
        printf(">> ");
        fgets(input, 256, stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "start_monitor") == 0)
	  {
            monitor_pid = fork();
            if (monitor_pid == 0)
	      {
                execl("./monitor", "monitor", NULL);
                perror("start monitor failed");
                exit(1);
	      }
	    else if (monitor_pid < 0)
	      {
                perror("fork failed");
                exit(1);
	      }

            cmd_filepath = fopen(CMD_FILE, "w");
            if (cmd_filepath == NULL)
	      {
                perror("fopen failed");
                exit(1);
	      }
	    
            fclose(cmd_filepath);
            printf("Monitor started successfully. Command list:\n");
            printf("- list_hunts\n");
            printf("- list_treasures <hunt_id>\n");
            printf("- view_treasure <hunt_id> <treasure_id>\n");
            printf("- stop_monitor\n");
            printf("- exit\n");
	  }
	else if (strcmp(input, "stop_monitor") == 0)
	  {
            cmd_filepath = fopen(CMD_FILE, "w");
            if (cmd_filepath)
	      {
                fprintf(cmd_filepath, "stop_monitor\n");
                fclose(cmd_filepath);
	      }

            if (monitor_pid > 0)
	      {
                waitpid(monitor_pid, NULL, 0);
                printf("Monitor stopped.\n");
		monitor_pid=-1;
	      }
	  }
	else if (strcmp(input, "exit") == 0)
	  {
            if (monitor_pid > 0)
	      {
                printf("Please stop the monitor before exiting using 'stop_monitor'.\n");
	      }
	    else
	      {
                printf("Exiting Treasure Hub.\n");
                break;
	      }
	  }
	else if (monitor_pid > 0)
	  {
            cmd_filepath = fopen(CMD_FILE, "w");
            if (!cmd_filepath)
	      {
                perror("fopen");
                continue;
	      }
            fprintf(cmd_filepath, "%s\n", input);
            fclose(cmd_filepath);
            sleep(1);
	  }
	else
	  {
            printf("Please start the monitor first using 'start_monitor'.\n");
	  }
      }

    return 0;
}
