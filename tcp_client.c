
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SIZE_STR 255
#define SIZE_COMMAND 7


typedef  struct
{
    uint8_t  led1 : 1;
    uint8_t  led2 : 1;
    uint8_t  led3 : 1;
    uint8_t  reserv : 1;
} LEDS;



static int32_t connect_server(char *cmd);
static int32_t parsing_connect(char *cmd, char **ip, char **port);
static void create_socke(void);
static int32_t parsing_command(char *cmd);
static void create_command(char *cmd, LEDS *leds);


int main(int argc, char *argv[])
{
    int attempt = 0;
    char cmd[SIZE_STR] = "";


    do
    {
        attempt++;
        printf("> ");
        fgets(cmd, SIZE_STR - 1, stdin);
        if (!connect_server(cmd))
        {
            attempt = 0;
        }
    }
    while ( attempt < 3 );

    return 0;
}


static int32_t connect_server(char *cmd)
{
    struct sockaddr_in serv_addr;
    int32_t  sock_fd = 0;
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
            sock_fd = socket(AF_INET, SOCK_STREAM, 0);

            if (sock_fd == -1)
            {
                printf("Socket не создался...\n");

                exit(0);
            }
            else
            {
                printf("Socket успешно создан.\n");
            }

            bzero(&serv_addr, sizeof(serv_addr));

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr(ip);
            printf("port: %d\n", atoi(port));
            serv_addr.sin_port = htons(atoi(port));

            if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
            {
                printf("Соединение с сервером не установлено...\n");

                exit(0);
            }
            else
            {
                printf("Сеодинение с сервером установлено.\n");
            }

            fgets(cmd, SIZE_STR - 1, stdin);
            parsing_command(cmd);
            close(sock_fd);
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


static void create_socke(void)
{}


int32_t parsing_command(char *cmd)
{
    int32_t retval = -10;
    char *check = NULL;
    LEDS  leds = {0, 0, 0};


    check = strtok(cmd, " ");

    if (!strncmp(check, "ifconfig", 8))
    {
        // TODO: запросить у сервера сетеве настройки
        printf("%s", check);
        retval = 0;
    }
    else if (!strncmp(check, "./start_leds", 12))
    {
        // TODO: отправить на сервер команду включить светодиоды
        create_command(check, &leds);
        printf("%d\n", leds.led1);
        printf("%d\n", leds.led2);
        printf("%d\n", leds.led3);
    }
    else if (!strncmp(check, "./stop_leds", 11))
    {
        // TODO: отправить на сервер команду выключить светодиоды
        create_command(check, &leds);
        printf("%d\n", leds.led1);
        printf("%d\n", leds.led2);
        printf("%d\n", leds.led3);
    }
    else
    {
        retval = 1;
    }

    return  retval;
}


static void create_command(char *cmd, LEDS *leds)
{
    while (cmd != NULL)
    {
        if (!strcmp(cmd, "LED1"))
        {
            leds->led1 = 1;
        }

        if (!strcmp(cmd, "LED2"))
        {
            leds->led2 = 1;
        }

        if (!strcmp(cmd, "LED3"))
        {
            leds->led3 = 1;
        }

        cmd = strtok(NULL, " \n");
    }
}

