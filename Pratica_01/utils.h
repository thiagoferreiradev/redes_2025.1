#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define IP "127.0.0.1"
#define PORT 7658 // Porta que o servidor vai abrir o socket.
#define MSG_MAX 1024 // Tamanho máximo da mensagem.
#define IP_SIZE 16 // 15 para o padrão IPV4, 1 para '\0'.
#define TS_SIZE 21      // Espaço para timestamp formatado. "DD-MM-YYYY HH:MM:SS + '\0"
#define BACKLOG 5       // Número máximo de conexões pendentes

// Limpa o stdin, lendo qualquer caractere que tenha sobrado até '\n' ou EOF.
void clean_stdin();

// Lê uma string de tamanho 'size' usando fgets.
void read_string(char *input, int size);

// Funções para ler os argumentos (ip e porta) caso não sejam passados via linha de comando.
void read_ip(char *ip);
void read_port(int *port);

// Verifica se o IP é um IPV4 válido.
int is_valid_ip(const char *ip);

// Verifica se a porta é válida, usando o intervalo 1–65535.
int is_valid_port(int port);

#endif