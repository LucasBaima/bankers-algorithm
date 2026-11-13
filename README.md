# Algoritmo do Banqueiro em C

Implementação 5 da disciplina de Infraestrutura de Software.

## Sobre o projeto

O programa implementa o Algoritmo do Banqueiro (Banker's Algorithm), utilizado em sistemas operacionais para evitar situações de deadlock durante a alocação de recursos.

A implementação tem como objetiv controlar solicitações e liberações de recursos feitas por diferentes clientes, verificando se uma nova alocação mantém o sistema em um estado seguro.


## Funcionamento previsto

O programa trabalha com quatro estruturas principais:

- **Maximum:** quantidade máxima de recursos que cada cliente pode solicitar.
- **Allocation:** recursos atualmente alocados para cada cliente.
- **Need:** recursos que cada cliente ainda pode precisar.
- **Available:** recursos disponíveis no sistema.

Os dados serão recebidos por arquivos de texto, enquanto as quantidades iniciais de recursos serão informadas por argumentos de linha de comando.

Os comandos previstos são `RQ` (solicitação), `RL` (liberação) e `*` (exibição do estado atual).

## Organização

O código está organizado no diretório `jlobg/`:

- `main.c` — inicialização e controle da execução.
- `banker.c` — operações e verificações do algoritmo.
- `banker.h` — estruturas e declarações compartilhadas.
- `Makefile` — compilação do programa.

## Compilação e execução

A compilação deverá ser realizada em ambiente Linux, Unix ou macOS, utilizando:

```bash
make
```

O executável gerado terá o nome `banker`.

Exemplo de execução:

```bash
./banker 10 5 7
```

O programa utilizará os arquivos `customer.txt` e `commands.txt` e produzirá a saída em `result.txt`.

## Estado do desenvolvimento

Projeto em desenvolvimento. A organização inicial dos arquivos foi definida e a implementação das estruturas de dados e das operações do algoritmo será realizada nas próximas etapas.