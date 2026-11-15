
#include <stdlib.h>
#include "banker.h"



void banker_free(Banker *banker) {
    if (banker == NULL)
        return;


    for (int i = 0; i < banker->customers; i++) {
        if (banker->maximum != NULL)
            free(banker->maximum[i]);

        if (banker->allocation != NULL)
            free(banker->allocation[i]);

        if (banker->need != NULL)
            free(banker->need[i]);
    }

    free(banker->maximum);
    free(banker->allocation);
    free(banker->need);
    free(banker->available);

    *banker = (Banker){0};

}



int banker_init(Banker *banker, int customers, int resources) {
    if (banker == NULL || customers <= 0 || resources <= 0) // checagem
        return 0;

    *banker = (Banker){0};

    banker->customers = customers;
    banker->resources = resources;

    banker->available = calloc(resources, sizeof(int));
    banker->maximum = calloc(customers, sizeof(int *));
    banker->allocation = calloc(customers, sizeof(int *));
    banker->need = calloc(customers, sizeof(int *));

    if (!banker->available || !banker->maximum ||
        !banker->allocation || !banker->need) {
        banker_free(banker);
        return 0;
    }

    for (int i = 0; i < customers; i++) {
        banker->maximum[i] = calloc(resources, sizeof(int));
        banker->allocation[i] = calloc(resources, sizeof(int));
        banker->need[i] = calloc(resources, sizeof(int));

        if (!banker->maximum[i] || !banker->allocation[i] ||
            !banker->need[i]) {
            banker_free(banker);
            return 0;
        }
    }

    return 1;
}
