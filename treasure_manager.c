#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>

typedef struct {
    int treasure_id;
    char username[32];
    float latitude;
    float longitude;
    char clue[256];
    int value;
} Treasure;

void add_treasure(char *hunt_id);
void list_treasures(char *hunt_id);
void view_treasure(char *hunt_id, int treasure_id);
void remove_treasure(char *hunt_id, int treasure_id);
void remove_hunt(char *hunt_id);
void log_operation(char *hunt_id, char *message);
void create_symlink(char *hunt_id);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n"
                        "--add <hunt_id>\n"
                        "--list <hunt_id>\n"
                        "--view <hunt_id><treasure_id>\n"
                        "--remove_treasure <hunt_id> <treasure_id>\n"
                        "--remove_hunt <hunt_id>\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "--add") == 0) {
        add_treasure(argv[2]);
    } else if (strcmp(argv[1], "--list") == 0) {
        list_treasures(argv[2]);
    } else if (strcmp(argv[1], "--view") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Missing treasure ID\n");
            exit(EXIT_FAILURE);
        }
        view_treasure(argv[2], atoi(argv[3]));
    } else if (strcmp(argv[1], "--remove_treasure") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Missing treasure ID\n");
            exit(EXIT_FAILURE);
        }
        remove_treasure(argv[2], atoi(argv[3]));
    } else if (strcmp(argv[1], "--remove_hunt") == 0) {
        remove_hunt(argv[2]);
    } else {
        fprintf(stderr, "Unknown command\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}


void log_operation(char *hunt_id, char *message) {
    char log_path[500];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt", hunt_id);

    int fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open (logged_hunt)");
        return;
    }

    write(fd, message, strlen(message));
    close(fd);
}

void create_symlink(char *hunt_id) {
    char target[500];
    char link_name[500];

    snprintf(target, sizeof(target), "%s/logged_hunt", hunt_id);
    snprintf(link_name, sizeof(link_name), "logged_hunt-%s", hunt_id);

    // Remove if exists
    unlink(link_name);

    if (symlink(target, link_name) == -1) {
        perror("symlink");
    }
}

void add_treasure(char* hunt_id) {
    char dir_path[500];
    char file_path[500];
    char log_path[500];
    Treasure t;

    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    snprintf(file_path, sizeof(file_path) + 14, "%s/treasures.dat", dir_path);
    snprintf(log_path, sizeof(log_path) + 12, "%s/logged_hunt", dir_path);

    if (mkdir(dir_path, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }


    printf("Enter Treasure ID: ");
    scanf("%d", &t.treasure_id);
    getchar();

    printf("Enter Username: ");
    fgets(t.username, 32, stdin);
    t.username[strcspn(t.username, "\n")] = 0;

    printf("Enter Latitude: ");
    scanf("%f", &t.latitude);

    printf("Enter Longitude: ");
    scanf("%f", &t.longitude);
    getchar(); 

    printf("Enter Clue: ");
    fgets(t.clue, 256, stdin);
    t.clue[strcspn(t.clue, "\n")] = 0;

    printf("Enter Value: ");
    scanf("%d", &t.value);

    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open (treasures.dat)");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &t, sizeof(Treasure)) != sizeof(Treasure)) {
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);


    char log_msg[256];
    snprintf(log_msg, sizeof(log_msg), "Added treasure ID %d by user %s\n", t.treasure_id, t.username);
    log_operation(hunt_id, log_msg);

    create_symlink(hunt_id);

    printf("Treasure added\n");
}

void list_treasures(char *hunt_id) {
    char file_path[500];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);

    struct stat st;
    if (stat(file_path, &st) == -1) {
        perror("stat");
        return;
    }

    printf("Hunt: %s\n"
           "File Size: %ld bytes\n"
           "Last Modified: %s", hunt_id, st.st_size, ctime(&st.st_mtime));
    
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("open (treasures.dat)");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("\nTreasure ID: %d\n"
               "Username: %s\n"
               "Coordinates: %.2f, %.2f\n"
               "Clue: %s\n"
               "Value: %d\n", t.treasure_id, t.username, t.latitude, t.longitude, t.clue, t.value);
    }
    close(fd);
}

void view_treasure(char *hunt_id, int treasure_id) {
    char file_path[500];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);

    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("open (treasures.dat)");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.treasure_id == treasure_id) {
            printf("Treasure ID: %d\n", t.treasure_id);
            printf("Username: %s\n", t.username);
            printf("Coordinates: %.2f, %.2f\n", t.latitude, t.longitude);
            printf("Clue: %s\n", t.clue);
            printf("Value: %d\n", t.value);
            close(fd);
            return;
        }
    }

    printf("Treasure with ID %d not found.\n", treasure_id);
    close(fd);
}

void remove_treasure(char *hunt_id, int treasure_id) {
    char file_path[500];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);

    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("open (treasures.dat)");
        return;
    }

    // Temporary file to store updated treasures
    char temp_path[500];
    snprintf(temp_path, sizeof(temp_path), "%s/temp_treasures.dat", hunt_id);
    int temp_fd = open(temp_path, O_WRONLY | O_CREAT, 0644);
    if (temp_fd == -1) {
        perror("open (temp file)");
        close(fd);
        return;
    }

    Treasure t;
    int treasure_found = 0;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.treasure_id == treasure_id) {
            treasure_found = 1;  // Skipping the treasure to remove it
            continue;
        }
        write(temp_fd, &t, sizeof(Treasure));
    }

    if (!treasure_found) {
        printf("Treasure with ID %d not found.\n", treasure_id);
    } else {
        printf("Treasure with ID %d removed successfully.\n", treasure_id);
        // Replace the old file with the new file
        remove(file_path);
        rename(temp_path, file_path);
    }

    close(fd);
    close(temp_fd);
}

void remove_hunt(char *hunt_id) {
    char file_path[500];

    // Remove known files in the hunt directory
    const char *files[] = {
        "treasures.dat",
        "logged_hunt",
        "temp_treasures.dat" // optional, cleanup if it exists
    };

    for (int i = 0; i < sizeof(files)/sizeof(files[0]); i++) {
        snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, files[i]);
        unlink(file_path);  // silently try to remove
    }

    // Remove the hunt directory
    if (rmdir(hunt_id) == -1) {
        perror("rmdir");
        return;
    }

    // Remove symlink
    char link_name[500];
    snprintf(link_name, sizeof(link_name), "logged_hunt-%s", hunt_id);
    if (unlink(link_name) == -1) {
        if (errno != ENOENT)
            perror("unlink (symlink)");
    }

    printf("Hunt '%s' removed.\n", hunt_id);
}
