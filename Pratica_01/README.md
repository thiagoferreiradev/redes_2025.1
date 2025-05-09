# Prática 01

A prática 01 consiste em um servidor de eco (e um cliente para testá-lo). \
O cliente envia uma mensagem e o servidor retorna a mesma mensagem, contendo a exata hora que ela foi recebida.

Exemplo:
```
<< Mensagem
>> [17:56:28] Mensagem

```

- Requisitos para compilar o projeto
    - Sistema Unix-like (Linux, MacOS, *BSD)
    - gcc (recomendado) ou outro compilador que suporte C99 e API POSIX.
    - make

E então:
```make``` ou  ```make CC=clang CFLAGS="-std=c99 -Wall"``` (via clang)

Irá gerar os binários server e client.