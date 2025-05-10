#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "utils.h"

void
usage()
{
    printf("Uso: client [OPÇÕES]\n");
    printf("Opções:\n");
    printf("    -i [ip] - Define o IP que será conectado, padrão: 127.0.0.1.\n");
    printf("    -p [porta] - Define a porta que será conectada, padrão: 7658.\n\n");
    printf("Exemplo de uso:\n");
    printf("client -i 127.0.0.1 -p 80\n");
    printf("client -i 127.0.0.1     porta padrão 7658.\n");
    printf("client                  o programa irá requisitar o ip e a porta interativamente.");
}

int
main(int argc, char **argv)
{
    char ip[IP_SIZE] = IP;
    int port = PORT, opt = 0;
    int sock_fd;

    int ip_done = 0, port_done = 0;

    struct sockaddr_in server_addr;
    char send_buffer[MSG_MAX], recv_buffer[MSG_MAX];

    if (argc > 1) {
        // Parser de argumentos via linha de comando.
        while ((opt = getopt(argc, argv, "i:p:")) != -1) {
            switch (opt) {
            case 'i':
                if (!is_valid_ip(optarg)) {
                    fprintf(stderr, "IP inválido! (%s)\n", optarg);
                    exit(EXIT_FAILURE);
                }

                snprintf(ip, IP_SIZE, "%s", optarg);

                ip_done = 1;

                break;
            case 'p':
                port = atoi(optarg);

                if (!is_valid_port(port)) {
                    fprintf(stderr, "Porta inválida! (%d)\n", port);
                    exit(EXIT_FAILURE);
                }

                port_done = 1;

                break;
            default:
                usage();
                exit(EXIT_FAILURE);
                break;
            }
        }
    }

    if (!ip_done) {
        read_ip(ip);
    }

    if (!port_done) {
        read_port(&port);
    }

    printf("Conectando a %s:%d\n", ip, port);

    // Cria o socket TCP IPV4.
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
        fprintf(stderr, "Não foi possível criar o socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do servidor.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);

    // Não precisa verificar o IP, pois is_valid_ip() garante que o programa só aceitará IPV4 válidos.
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // Tenta conectar ao servidor, encerra o programa se não for possível.
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Não foi possível conectar à: %s:%d: %s\n", ip, port, strerror(errno));
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Conectado com sucesso! Digite \"sair\" para encerrar.\n");

    // Loop de mensagens.
    for (;;) {
        printf(">> ");
        read_string(send_buffer, MSG_MAX);

        // Caso a mensagem seja "sair", encerra.
        if (strcmp(send_buffer, "sair") == 0) {
            printf("A conexão será encerrada.\n");
            break;
        }

        // Envia mensagem lida ao servidor.
        if (write(sock_fd, send_buffer, strlen(send_buffer)) < 0) {
            fprintf(stderr, "Não foi possível enviar a mensagem: %s\n", strerror(errno));
            break;
        }

        // Aguarda a resposta do servidor.
        int nread = read(sock_fd, recv_buffer, MSG_MAX);

        if (nread < 0) {
            fprintf(stderr, "Não foi possível ler a resposta: %s\n", strerror(errno));
            break;
        }

        if (nread == 0) {
            printf("O servidor fechou a conexão..\n");
            break;
        }

        recv_buffer[nread] = '\0';

        // Exibe a mensagem recebida do servidor.
        printf("<< %s\n", recv_buffer);
        printf("---------------------------\n");
    }

    // Fecha o socket antes de encerrar.
    close(sock_fd);

    return 0;
}