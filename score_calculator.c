#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char treasure_id[50];
    char username[32];
    float latitude;
    float longitude;
    char clue[256];
    int value;
} Treasure;

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "--hunt") != 0) {
        fprintf(stderr, "Usage: %s --hunt <hunt_id>\n", argv[0]);
        return 1;
    }

    char *hunt_id = argv[2];
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/treasures.dat", hunt_id);

    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Error opening treasures.dat");
        return 1;
    }

    struct {
        char username[32];
        int total;
    } users[100];

    int n_users = 0;
    Treasure t;

    while (fread(&t, sizeof(t), 1, f) == 1) {
        int found = 0;
        for (int i = 0; i < n_users; ++i) {
            if (strcmp(users[i].username, t.username) == 0) {
                users[i].total += t.value;
                found = 1;
                break;
            }
        }
        if (!found && n_users < 100) {
            strcpy(users[n_users].username, t.username);
            users[n_users].total = t.value;
            n_users++;
        }
    }

    fclose(f);

    printf("Scores for hunt %s:\n", hunt_id);
    for (int i = 0; i < n_users; ++i) {
        printf("  %s: %d\n", users[i].username, users[i].total);
    }

    return 0;
}
