#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"

void
usage()
{
    printf("Uso: server [OPÇÕES]\n");
    printf("Opções:\n");
    printf("    -p [porta] - Define a porta que será conectada, padrão: 7658.\n\n");
    printf("Exemplo de uso:\n");
    printf("server -p 80\n");
    printf("server                 o programa irá requisitar o ip e a porta interativamente.");
}

int
main(int argc, char **argv)
{
    int server_fd = -1, client_fd = -1, opt = 0, port = PORT;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char msg_buf[MSG_MAX];
    char send_buf[TS_SIZE + MSG_MAX];
    char timestamp[TS_SIZE];

    time_t now;
    struct tm tm_info;

    if (argc > 1) {
        // Parser de argumentos via linha de comando.
        while ((opt = getopt(argc, argv, "p:")) != -1) {
            switch (opt) {
            case 'p':
                port = atoi(optarg);

                if (!is_valid_port(port)) {
                    fprintf(stderr, "Porta inválida! (%d)\n", port);
                    exit(EXIT_FAILURE);
                }

                break;
            default:
                usage();
                exit(EXIT_FAILURE);
                break;
            }
        }
    } else {
        read_port(&port);
    }

    printf("Iniciando servidor na porta %d\n", port);

    // Cria o socket TCP IPV4.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "Não foi possível criar o socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Configura o endereço.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // todas as interfaces
    server_addr.sin_port        = htons(port);

    // Associa o socket ao endereço e a porta.
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Não foi possível associar o socket à porta %d: %s\n", port, strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Seta o socket em modo passivo.
    if (listen(server_fd, BACKLOG) < 0) {
        fprintf(stderr,"Não é possível ouvir a porta %d (backlog %d): %s\n", port, BACKLOG, strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado na porta %d.\n-------------------\n", port);

    for (;;) {
        // Espera a conexão de um cliente.
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd < 0) {
            fprintf(stderr, "Não foi possível aceitar a conexão, tentando novamente. %s\n", strerror(errno));
            continue;
        }

        // Informa endereço do cliente conectado.
        printf("Cliente conectado. Endereço: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Loop de mensagens.
        for (;;) {
            // Leitura de dados enviados pelo cliente.
            ssize_t nread = read(client_fd, msg_buf, MSG_MAX);

            if (nread < 0) {
                fprintf(stderr, "Não foi possível ler a mensagem do cliente %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), strerror(errno));
                break;
            }

            // Cliente fechou a conexão.
            if (nread == 0) {
                printf("Cliente %s:%d desconectado.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            }

            // Garante término de string.
            if (nread < MSG_MAX)
                msg_buf[nread] = '\0';
            else
                msg_buf[MSG_MAX - 1] = '\0';

            // Gera um timestamp no formato DD-MM-YYYY HH:MM:SS.
            now = time(NULL);
            localtime_r(&now, &tm_info);
            strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %H:%M:%S", &tm_info);

            // Exibe a resposta: [timestamp] mensagem.
            int slen = snprintf(send_buf, sizeof(send_buf), "[%s] %s", timestamp, msg_buf);

            if (slen < 0) {
                fprintf(stderr, "Não foi possível formatar a string de resposta.\n");
                break;
            }

            // Envia a mensagem de volta ao cliente.
            if (write(client_fd, send_buf, slen) < 0) {
                fprintf(stderr, "Não foi possível enviar ao cliente %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), strerror(errno));
                break;
            }
        }

        // Fecha o socket do cliente e volta para aceitar novas conexões.
        close(client_fd);
        client_fd = -1;
    }

    // Fecha o file descriptor do servidor.
    close(server_fd);

    return 0;
}