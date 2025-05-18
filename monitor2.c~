#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TMP_FILE "cli.txt"

typedef struct{
    char treasure_id[50];
    char username[32];
    float latitude;
    float longitude;
    char clue[256];
    int value;
} Treasure;

void handle_list_hunts()
{
    DIR *dir = opendir(".");
    if (!dir)
      {
        perror("opendir");
        return;
      }

    struct dirent *entry;
    while ((entry = readdir(dir)))
      {
        if (entry->d_type == DT_DIR && strncmp(entry->d_name, ".", 1) != 0 && strcmp(entry->d_name, "..") != 0)
	  {
            char path[512];
            snprintf(path, sizeof(path), "%s/treasures.dat", entry->d_name);
            FILE *filepath = fopen(path, "rb");
            if (!filepath) continue;

            int count = 0;
            Treasure t;
            while (fread(&t, sizeof(Treasure), 1, filepath) == 1)
                count++;

            fclose(filepath);
            printf("Hunt: %s - %d treasures\n", entry->d_name, count);
	  }
      }
    closedir(dir);
}

void handle_list_treasures(const char *hunt_id)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);

    FILE *filepath = fopen(path, "rb");
    if (!filepath)
      {
        printf("Hunt '%s' not found or cannot open file.\n", hunt_id);
        return;
      }

    Treasure t;
    while (fread(&t, sizeof(Treasure), 1, filepath) == 1)
      {
        printf("Treasure ID: %s\n", t.treasure_id);
      }

    fclose(filepath);
}

void handle_view_treasure(const char *hunt_id, char* treasure_id)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);

    FILE *filepath = fopen(path, "rb");
    if (!filepath)
      {
        printf("Hunt '%s' not found.\n", hunt_id);
        return;
      }

    Treasure t;
    int found = 0;
    while (fread(&t, sizeof(Treasure), 1, filepath) == 1)
      {
      if (strcmp(t.treasure_id,treasure_id)==0)
	{
            printf("Treasure ID: %s\n", t.treasure_id);
            printf("Username: %s\n", t.username);
            printf("Latitude: %.2f\n", t.latitude);
            printf("Longitude: %.2f\n", t.longitude);
            printf("Clue: %s\n", t.clue);
            printf("Value: %d\n", t.value);
            found = 1;
            break;
	}
      }

    if (!found)
        printf("Treasure ID %s not found in hunt '%s'.\n", treasure_id, hunt_id);

    fclose(filepath);
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
                handle_list_hunts();
	      }
	    else if (strcmp(cmd, "list_treasures") == 0 && num_tokens == 2)
	      {
                handle_list_treasures(arg1);
	      }
	    else if (strcmp(cmd, "view_treasure") == 0 && num_tokens == 3)
	      {
                handle_view_treasure(arg1, arg2);                        
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
