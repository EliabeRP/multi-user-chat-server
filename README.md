# multi-user-chat-server

Servidor TCP concorrente aceitando múltiplos clientes desenvolvido em C. Projeto da disciplina Linguagem de Programação 2 com o objetivo aplicar de conceitos de concorrência em uma aplicação real.

## Estrutura do Projeto
```
include/
  libtslog.h
tests/
  log_sim.c
libtslog.c
Makefile
README.md
```
---
## Como compilar

No terminal:

```bash
make
```

---
## Como executar

```bash
./log_sim [nthreads] [nmsgs]
```

---
## Remover compilados e logs:
```bash
make clean
```