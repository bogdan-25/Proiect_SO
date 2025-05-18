#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#define TMP_FILE "cli.txt"

typedef struct{
  char treasure_id[50];
  char username[32];
  float latitude;
  float longitude;
  char clue[256];
  int value;
} Treasure;

void call_manager(const char *fmt, ...){
  char cmd[512];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(cmd, sizeof(cmd), fmt, ap);
  va_end(ap);
  system(cmd);
}
int main()
{
  while (1)
    {
      FILE *filepath = fopen(TMP_FILE, "r");
      if (!filepath)
	{
	  sleep(1);
	  continue;
	}

      char line[256];
      if (fgets(line, 256, filepath))
	{
	  line[strcspn(line, "\n")] = 0;

	  char cmd[32], arg1[64], arg2[64];
	  int num_tokens = sscanf(line, "%s %s %s", cmd, arg1, arg2);

	  if (strcmp(cmd, "list_hunts") == 0)
	    {
	      call_manager("./treasure_manager --list_hunts");
	    }
	  else if (strcmp(cmd, "list_treasures") == 0 && num_tokens == 2)
	    {
	      call_manager("./treasure_manager --list %s", arg1);
	    }
	  else if (strcmp(cmd, "view_treasure") == 0 && num_tokens == 3)
	    {
	      call_manager("./treasure_manager --view %s %s", arg1, arg2);                
	    }
	  else if (strcmp(cmd, "stop_monitor") == 0)
	    {
	      printf("Stopping monitor...\n");
	      break;
	    }
	  else if (strcmp(cmd, "exit") == 0)
	    {
	      printf("Cannot exit: Monitor is still active. Please stop it first using 'stop_monitor'.\n");
	    }
	  else
	    {
	      printf("Invalid command: %s\n", line);
	    }

	  // Clear the file after reading command
	  fclose(filepath);
	  filepath = fopen(TMP_FILE, "w");
	  if (filepath) fclose(filepath);
        }
      else
	{
	  fclose(filepath);
	}

      sleep(1);
    }
  return 0;
}
