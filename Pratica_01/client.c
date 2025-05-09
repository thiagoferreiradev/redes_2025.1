#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 7658 // Porta que o servidor vai abrir o socket.
#define MSG_MAX 1024 // Tamanho máximo da mensagem.
#define IP_SIZE 16 // 15 para o padrão IPV4, 1 para '\0'.

// Limpa o stdin, lendo qualquer caractere que tenha sobrado até '\n' ou EOF.
void
clean_stdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Lê uma string de tamanho 'size' usando fgets.
void
read_string(char *input, int size)
{
    if (fgets(input, size, stdin) != NULL) {
        // Se não encontrou '\n' na string, é porque sobraram caracteres no stdin.
        if (strchr(input, '\n') == NULL) {
            clean_stdin();
        } else {
            // Remove o '\n' da string.
            input[strcspn(input, "\n")] = '\0';
        }
    } else {
        fprintf(stderr, "Erro na leitura.\n");
        exit(EXIT_FAILURE);
    }
}

// Verifica se o IP é um IPV4 válido.
int
is_valid_ip(const char *ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}

// Verifica se a porta é válida, usando o intervalo 1–65535.
int
is_valid_port(int port)
{
    return port > 0 && port <= 65535;
}

// Função para ler os argumentos (ip e porta) caso não sejam passados via linha de comando.
void
read_args(char *ip, int *port)
{
    char op = ' ';

    // Requisita um IP até que a entrada seja válida.
    do {
        printf("Insira o IP\n<< ");
        read_string(ip, IP_SIZE);

        if (!is_valid_ip(ip)) {
            printf("IP inválido. Tente novamente.\n");
        }

    } while (!is_valid_ip(ip));

    printf("Deseja alterar a porta? (s/n)\n<< ");

    op = getchar();

    // Requisita uma porta até que a entrada seja válida.
    if (op == 's' || op == 'S') {
        do {
            printf("Insira a porta (1–65535)\n<< ");

            if (scanf("%d", port) != 1) {
                clean_stdin();
                *port = -1;
                printf("Porta inválida. Tente novamente.\n");
            }
        } while (!is_valid_port(*port));
    }
}

void
usage()
{
    printf("Uso: client [OPÇÕES]\n");
    printf("Opções:\n");
    printf("    -i [ip] - Define o IP que será conectado.\n");
    printf("    -p [porta] - Define a porta que será conectada, padrão: 7658.\n\n");
    printf("Exemplo de uso:\n");
    printf("client -i 127.0.0.1 -p 80\n");
    printf("client -i 127.0.0.1     porta padrão 7658.\n");
    printf("client                  o programa irá requisitar o ip e a porta interativamente.");
}

int
main(int argc, char **argv)
{
    char ip[IP_SIZE];
    int port = PORT, opt = 0;
    int sock_fd;

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

                break;
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
        read_args(ip, &port);
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
    while (1) {
        printf(">> ");
        read_string(send_buffer, MSG_MAX);

        // Caso a mensagem seja "exit", encerra.
        if (strcmp(send_buffer, "exit") == 0) {
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