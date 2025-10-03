# multi-user-chat-server

Servidor TCP concorrente aceitando múltiplos clientes desenvolvido em C. Projeto da disciplina Linguagem de Programação 2 com o objetivo aplicar de conceitos de concorrência em uma aplicação real.

## Estrutura do Projeto

```
├── include/
│ ├── client.h
│ ├── libtslog.h
│ └── server.h
│
├── logs/
│
├── tests/ 
│ └── log_sim.c
| └── test_chat.sh
│
├── client/ 
│ └── client.c
│
├── server/ 
│ └── server.c
│
├── libtslog.c
├── Makefile
└── README.md 
```
---

## Como compilar

No terminal:

```bash
make
```

---
## Como executar

Em um terminal:

```bash
./server 8080
```

Em outro terminal:

```bash
./client 127.0.0.1 8080
```
---
## Executar testes automatizados
```bash
chmod +x tests/test_chat.sh
```
```bash
./tests/test_chat.sh
```
---
## Remover compilados e logs:
```bash
make clean
```