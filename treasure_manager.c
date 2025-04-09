#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  char treasureID[20];
  char username[40];
  float latitude;
  float longitude;
  char clueText[100];
  int value;
}treasure;

void add(char* huntID)
{
  int f=open("treasure1.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(f<0)
    {
      perror("Eroare la deschidere fisier");
      exit(-1);
    }
  treasure t;

  printf("Introduceti treasureID:\n");
  scanf("%s",t.treasureID);
  write(f,t.treasureID,sizeof(t.treasureID));

  printf("Introduceti username:\n");
  scanf("%s",t.username);
  write(f,t.username,sizeof(t.username));

  printf("Introduceti latitude:\n");
  scanf("%f",&t.latitude);
  write(f,&t.latitude,sizeof(t.latitude));

  printf("Introduceti longitude:\n");
  scanf("%f",&t.longitude);
  write(f,&t.longitude,sizeof(t.longitude));

  printf("Introduceti cluetext:\n");
  scanf("%s",t.clueText);
  write(f,t.clueText,sizeof(t.clueText));

  printf("Introduceti value:\n");
  scanf("%d",&t.value);
  write(f,&t.value,sizeof(t.value));

  close(f);
}

void list(char* huntID)
{
  printf("%s",huntID);
}

void view(char* huntID,char* treasureID)
{
  int f=open("treasure1.bin", O_RDONLY );
  if(f<0)
    {
      perror("Eroare la deschidere fisier");
      exit(-1);
    }
  treasure t;
  
  read(f,t.treasureID,sizeof(t.treasureID));
  printf("%s",t.treasureID);
  
  read(f,t.username,sizeof(t.username));
  printf("\n%s",t.username);

  read(f,&t.latitude,sizeof(t.latitude));
  printf("\n%f",t.latitude);

  read(f,&t.longitude,sizeof(t.longitude));
  printf("\n%f",t.longitude);

  read(f,t.clueText,sizeof(t.clueText));
  printf("\n%s",t.clueText);

  read(f,&t.value,sizeof(t.value));
  printf("\n%d",t.value);

  close(f);
}

void remove_treasure(char* huntID,char* treasureID)
{
  printf("%s %s",huntID,treasureID);
}

void remove_hunt(char* huntID)
{
  printf("%s",huntID);
}




int main(int argc,char** argv)
{
  if(!strcmp(argv[1],"--add"))
    {
      add(argv[2]);
    }
  if(!strcmp(argv[1],"--list"))
    {
      list(argv[2]);
    }
  if(!strcmp(argv[1],"--view"))
    {
      view(argv[2],argv[3]);
    }
  if(!strcmp(argv[1],"--remove_treasure"))
    {
    }
  if(!strcmp(argv[1],"--remove_hunt"))
    {
    }
  printf("\n");
  return 0;
}
