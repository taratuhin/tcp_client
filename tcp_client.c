
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
static int32_t create_socke(char *ip, char *port);
static int32_t parsing_command(char *cmd, int32_t  sock_fd);
static void create_command(char *cmd, LEDS *leds);
static void get_server_info(int32_t sock_fd);
static void leds_on(int32_t sock_fd, LEDS *leds);
static void leds_off(int32_t sock_fd, LEDS *leds);


/**
 *   \brief   Точка входа
 *   \param   argc - количество входных аргументов
 *   \param  *argv - входные аргументы
 *   \return  Код завершения
 */
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


/**
 *   \brief   Соединение с сервером и передача команды
 *   \param  *cmd - команда
 *   \return  Код завершения, 0 - введена корректная команда, 1 - введена не корректная команда
 */
static int32_t connect_server(char *cmd)
{
    int32_t sock_fd = 0;
    int32_t retval = -10;
    char *ip = NULL;
    char *port = NULL;


    switch (parsing_connect(cmd, &ip, &port))
    {
        case 0:
            printf("# ");
            printf("%s\n", cmd);
            printf("%s\n", ip);
            printf("%s\n", port);
            sock_fd = create_socke(ip, port);
            while (1)
            {
                fgets(cmd, SIZE_STR - 1, stdin);
                if (strncmp(cmd, "exit", 4))
                {
                    parsing_command(cmd, sock_fd);
                    printf("# ");
                    bzero(cmd, SIZE_STR);
                }
                else
                {
                    printf("Выход из программы.\n");
                    exit(0);
                }
            }
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


/**
 *   \brief   Разбор команды для соединения с сервером
 *   \param  *cmd - команда
 *   \param **ip - IP
 *   \param **port - порт
 *   \return  Код завершения, 0 - введена корректная команда, 1 - введена не корректная команда
 */
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


/**
 *   \brief   Создание соединения с сервером
 *   \param  *ip - IP
 *   \param  *port - порт
 *   \return  Код завершения, 0 - введена корректная команда, 1 - введена не корректная команда
 */
static int32_t create_socke(char *ip, char *port)
{
    struct sockaddr_in serv_addr;
    int32_t  sock_fd = 0;


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
        printf("# ");
    }

    return sock_fd;
}


/**
 *   \brief   Создание команд серверу
 *   \param  *cmd - команда
 *   \return  Код завершения, 0 - введена корректная команда, 1 - введена не корректная команда
 */
int32_t parsing_command(char *cmd, int32_t sock_fd)
{
    int32_t retval = -10;
    char *check = NULL;
    LEDS  leds = {0, 0, 0};


    check = strtok(cmd, " ");

    if (!strncmp(check, "ifconfig", 8))
    {
        get_server_info(sock_fd);
        retval = 0;
    }
    else if (!strncmp(check, "./start_leds", 12))
    {
        create_command(check, &leds);
        leds_on(sock_fd, &leds);
    }
    else if (!strncmp(check, "./stop_leds", 11))
    {
        create_command(check, &leds);
        leds_off(sock_fd, &leds);
    }
    else
    {
        retval = 1;
    }

    return  retval;
}


/**
 *   \brief   Создание команды серверу
 *   \param  *cmd - команда
 *   \param  *leds - данные для сервера
 *   \return  Нет
 */
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


/**
 *   \brief   Запрос сетевых настроек сервера
 *   \param   sock_fd - дескриптор сокета
 *   \return  Нет
 */
static void get_server_info(int32_t sock_fd)
{
    char buff[255] = "ifconfig";


    write(sock_fd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sock_fd, buff, sizeof(buff));
    printf("%s\n", buff);
}


/**
 *   \brief   Команды на включение светодиодов на сервере
 *   \param   sock_fd - дескриптор сокета
 *   \param  *leds - данные для сервера
 *   \return  Нет
 */
static void leds_on(int32_t sock_fd, LEDS *leds)
{
    char buff[255];

    bzero(buff, sizeof(buff));
    strcpy(buff, "./start_leds");
    if (leds->led1 == 1)
    {
        strcat(buff, " ");
        strcat(buff, "LED1");
    }

    if (leds->led2 == 1)
    {
        strcat(buff, " ");
        strcat(buff, "LED2");
    }

    if (leds->led3 == 1)
    {
        strcat(buff, " ");
        strcat(buff, "LED3");
    }

    write(sock_fd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sock_fd, buff, sizeof(buff));
}


/**
 *   \brief   Команды на включение светодиодов на сервере
 *   \param   sock_fd - дескриптор сокета
 *   \param  *leds - данные для сервера
 *   \return  Нет
 */
static void leds_off(int32_t sock_fd, LEDS *leds)
{
    char buff[255];

    bzero(buff, sizeof(buff));
    strcpy(buff, "./stop_leds");
    if (leds->led1 == 1)
    {
        strcat(buff, " ");
        strcat(buff, "LED1");
    }

    if (leds->led2 == 1)
    {
        strcat(buff, " ");
        strcat(buff, "LED2");
    }

    if (leds->led3 == 1)
    {
        strcat(buff, " ");
        strcat(buff, "LED3");
    }

    write(sock_fd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sock_fd, buff, sizeof(buff));
}

