
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


//----------------------------------------------------------------------------------


int banker_safe(const Banker *banker) {
    int *work = malloc(banker->resources * sizeof(int));
    int *finish = calloc(banker->customers, sizeof(int));

    if (!work || !finish) {
        free(work);
        free(finish);
        return -1;
    }

    for (int j = 0; j < banker->resources; j++)
        work[j] = banker->available[j];

    int completed = 0;

    while (completed < banker->customers) {
        int progress = 0;

        for (int i = 0; i < banker->customers; i++) {
            if (finish[i])
                continue;

            int possible = 1;

            for (int j = 0; j < banker->resources; j++) {
                if (banker->need[i][j] > work[j]) {
                    possible = 0;
                    break;
                }
            }

            if (!possible)
                continue;

            for (int j = 0; j < banker->resources; j++)
                work[j] += banker->allocation[i][j];

            finish[i] = 1;
            completed++;
            progress = 1;
        }

        if (!progress)
            break;
    }

    free(work);
    free(finish);

    return completed == banker->customers;
}



int banker_request(Banker *banker, int customer,
                   const int *request) {
    if (!banker || !request ||
        customer < 0 || customer >= banker->customers)
        return -1;

    for (int j = 0; j < banker->resources; j++) {
        if (request[j] < 0 ||
            request[j] > banker->need[customer][j])
            return -1;
    }

    for (int j = 0; j < banker->resources; j++) {
        if (request[j] > banker->available[j])
            return -2;
    }

    for (int j = 0; j < banker->resources; j++) {
        banker->available[j] -= request[j];
        banker->allocation[customer][j] += request[j];
        banker->need[customer][j] -= request[j];
    }

    int safe = banker_safe(banker);

    if (safe == 1)
        return 1;

    for (int j = 0; j < banker->resources; j++) {
        banker->available[j] += request[j];
        banker->allocation[customer][j] -= request[j];
        banker->need[customer][j] += request[j];
    }

    return safe == 0 ? -3 : -4;
}




int banker_release(Banker *banker, int customer,
                   const int *release) {
    if (!banker || !release ||
        customer < 0 || customer >= banker->customers)
        return 0;

    for (int j = 0; j < banker->resources; j++) {
        if (release[j] < 0 ||
            release[j] > banker->allocation[customer][j])
            return 0;
    }

    for (int j = 0; j < banker->resources; j++) {
        banker->allocation[customer][j] -= release[j];
        banker->available[j] += release[j];
        banker->need[customer][j] += release[j];
    }

    return 1;
}



void banker_print(const Banker *banker, FILE *output) {
    fprintf(output, "MAXIMUM | ALLOCATION | NEED\n");

    for (int i = 0; i < banker->customers; i++) {
        for (int j = 0; j < banker->resources; j++)
            fprintf(output, "%s%d", j ? " " : "",
                    banker->maximum[i][j]);

        fprintf(output, " | ");

        for (int j = 0; j < banker->resources; j++)
            fprintf(output, "%s%d", j ? " " : "",
                    banker->allocation[i][j]);

        fprintf(output, " | ");

        for (int j = 0; j < banker->resources; j++)
            fprintf(output, "%s%d", j ? " " : "",
                    banker->need[i][j]);

        fprintf(output, "\n");
    }

    fprintf(output, "AVAILABLE");

    for (int j = 0; j < banker->resources; j++)
        fprintf(output, " %d", banker->available[j]);

    fprintf(output, "\n");
}
