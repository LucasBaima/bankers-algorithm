
#ifndef BANKER_H
#define BANKER_H

typedef struct {
    int customers;
    int resources;

    int *available;
    int **maximum;
    int **allocation;
    int **need;
} Banker; // data

int banker_init(Banker *banker, int customers, int resources); //prepara a estrutura e aloca a memória.
void banker_free(Banker *banker);

#endif
