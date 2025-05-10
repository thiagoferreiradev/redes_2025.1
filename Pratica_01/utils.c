#include "utils.h"

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

// Funções para ler os argumentos (ip e porta) caso não sejam passados via linha de comando.
void
read_ip(char *ip)
{
    char op[2] = "";

    printf("Deseja alterar o ip? (s/n)\n<< ");

    read_string(op, sizeof(op));

    if (tolower(op[0]) == 's') {
        do {
            printf("Insira o IP\n<< ");
            read_string(ip, IP_SIZE);

            if (!is_valid_ip(ip)) {
                printf("IP inválido. Tente novamente.\n");
            }

        } while (!is_valid_ip(ip));
    }
}

void
read_port(int *port)
{
    char op[2] = "";

    printf("Deseja alterar a porta? (s/n)\n<< ");

    read_string(op, sizeof(op));

    if (tolower(op[0]) == 's') {
        char buffer[100];
        char *endptr;
        long p = 0;

        do {
            printf("Insira a porta (1–65535)\n<< ");

            read_string(buffer, sizeof(buffer));

            errno = 0; // Reseta errno antes da conversão.

            p = strtol(buffer, &endptr, 10);

            if (errno == ERANGE || p < 1 || p > 65535) {
                printf("Porta fora do intervalo válido. Tente novamente.\n");
            } else if (endptr == buffer || *endptr != '\0') {
                printf("A entrada não é um número inteiro válido. Tente novamente.\n");
            } else {
                *port = (int)p;
                break;
            }
        } while (1);
    }
}