# Prática 01

A prática 01 consiste em um servidor de eco (e um cliente para testá-lo). \
O cliente envia uma mensagem e o servidor retorna a mesma mensagem, contendo o timestamp em que ela foi recebida.

Exemplo:
```
<< Mensagem
>> [09-05-2025 17:56:28] Mensagem

```

- Requisitos para compilar o projeto
    - Sistema Unix-like (Linux, MacOS, *BSD) ou Windows via WSL
    - gcc (recomendado) ou outro compilador que suporte C99 e API POSIX.
    - make

E então:
```make``` ou  ```make CC=clang CFLAGS="-std=c99 -Wall"``` (via clang)

Irá gerar os binários server e client.