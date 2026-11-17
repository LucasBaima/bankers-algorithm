

#ifndef BANKER_H
#define BANKER_H
#include <stdio.h>

typedef struct {
    int customers;
    int resources;

    int *available;
    int **maximum;
    int **allocation;
    int **need;
} Banker; // data 

int banker_init(Banker *banker, int customers, int resources);  //prepara a estrutura e aloca a memória.
void banker_free(Banker *banker);

int banker_safe(const Banker *banker);

int banker_request(Banker *banker, int customer,
                   const int *request);

int banker_release(Banker *banker, int customer,
                   const int *release);

void banker_print(const Banker *banker, FILE *output);


#endif
