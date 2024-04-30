
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define SIZE_STR 255
#define SIZE_COMMAND 7


static int32_t connect(char *cmd);
static int32_t parsing_connect(char *cmd, char **ip, char **port);
static int32_t parsing_command(char *cmd);


int main(int argc, char *argv[])
{
    int attempt = 0;
    char cmd[SIZE_STR] = "";


    do
    {
        attempt++;
        printf("> ");
        fgets(cmd, SIZE_STR - 1, stdin);
        if (!connect(cmd))
        {
            attempt = 0;
        }
    }
    while ( attempt < 3 );

    return 0;
}


static int32_t connect(char *cmd)
{
    int32_t  retval = -10;
    char *ip = NULL;
    char *port = NULL;


    switch (parsing_connect(cmd, &ip, &port))
    {
        case 0:
            printf("# ");
            printf("%s\n", cmd);
            printf("%s\n", ip);
            printf("%s\n", port);
            printf("# ");
            // TODO Установление связи по TCP
            fgets(cmd, SIZE_STR - 1, stdin);
            parsing_command(cmd);
            free(ip);
            free(port);
            retval = 0;

            break;

        default:
            retval = 1;

            break;
    }

    return  retval;
}


static int32_t parsing_connect(char *cmd, char **ip, char **port)
{
    int32_t retval = -10;
    char *check;


    check = strtok(cmd, " ");

    if (!strncmp(check, "telnet", 6))
    {
        check = strtok(NULL, " :");
        *ip = (char * ) calloc(strlen(check) + 1, 1);
        strcpy(*ip, check);
        check = strtok(NULL, "\n");
        *port = (char *) calloc(strlen(check) + 1, 1);
        strcpy(*port, check);
        retval = 0;
    }
    else
    {
        retval = 1;
    }

    return retval;
}


int32_t parsing_command(char *cmd)
{
    int32_t retval = -10;
    char *check = NULL;


    check = strtok(cmd, " ");

    if (strlen(check) == 6)
    {
//        if (!strncmp(check, "telnet", 6))
//        {
//            check = strtok(NULL, " :");
//            *ip = (char * ) calloc(strlen(check) + 1, 1);
//            strcpy(*ip, check);
//            check = strtok(NULL, "\n");
//            *port = (char *) calloc(strlen(check) + 1, 1);
//            strcpy(*port, check);
//            retval = 0;
//        }
//        else
//        {
//            retval = 2;
//        }
    }
    else
    {
        retval = 1;
    }


    return  retval;
}

