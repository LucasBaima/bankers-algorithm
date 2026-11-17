
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"

static char *read_line(FILE *file) {
    size_t capacity = 128, length = 0;
    char *line = malloc(capacity);

    if (!line)
        return NULL;

    int c;

    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (length + 1 >= capacity) {
            if (capacity > SIZE_MAX / 2) {
                free(line);
                return NULL;
            }

            capacity *= 2;
            char *new_line = realloc(line, capacity);

            if (!new_line) {
                free(line);
                return NULL;
            }

            line = new_line;
        }

        line[length++] = (char)c;
    }

    if (c == EOF && length == 0) {
        free(line);
        return NULL;
    }

    line[length] = '\0';
    return line;
}

static void spaces(const char **p) {
    while (isspace((unsigned char)**p))
        ++*p;
}

static int number(const char **p, int *value) {
    spaces(p);

    if (!isdigit((unsigned char)**p))
        return 0;

    errno = 0;
    char *end;
    long n = strtol(*p, &end, 10);

    if (errno == ERANGE || n > INT_MAX || end == *p)
        return 0;

    *value = (int)n;
    *p = end;

    return 1;
}

static int at_end(const char *p) {
    spaces(&p);
    return *p == '\0';
}

static int customer_values(const char *line, int *values, int n) {
    const char *p = line;

    for (int i = 0; i < n; i++) {
        if (!number(&p, &values[i]))
            return 0;

        spaces(&p);

        if (i + 1 < n && *p++ != ',')
            return 0;
    }

    return at_end(p);
}

static int command_values(const char *line, int *customer,
                          int *values, int n, char *kind) {
    const char *p = line;
    spaces(&p);

    if (p[0] == '*' && at_end(p + 1)) {
        *kind = '*';
        return 1;
    }

    if (p[0] != 'R' ||
        (p[1] != 'Q' && p[1] != 'L') ||
        !isspace((unsigned char)p[2]))
        return 0;

    *kind = p[1];
    p += 2;

    if (!number(&p, customer))
        return 0;

    for (int i = 0; i < n; i++) {
        if (!isspace((unsigned char)*p) ||
            !number(&p, &values[i]))
            return 0;
    }

    return at_end(p);
}

static int count_customers(FILE *file) {
    int count = 0;
    char *line;

    while ((line = read_line(file)) != NULL) {
        if (at_end(line)) {
            free(line);
            return -1;
        }

        count++;
        free(line);
    }

    if (ferror(file) || count == 0)
        return -1;

    rewind(file);
    return count;
}

static void print_values(FILE *output, const int *values, int n) {
    for (int j = 0; j < n; j++)
        fprintf(output, " %d", values[j]);

    fprintf(output, "\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s RECURSO1 [RECURSO2 ...]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int resources = argc - 1;
    int *total = malloc((size_t)resources * sizeof(int));

    if (!total) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        return EXIT_FAILURE;
    }

    for (int j = 0; j < resources; j++) {
        const char *p = argv[j + 1];

        if (!number(&p, &total[j]) || !at_end(p)) {
            fprintf(stderr, "Erro: recurso invalido.\n");
            free(total);
            return EXIT_FAILURE;
        }
    }

    FILE *customer_file = fopen("customer.txt", "r");

    if (!customer_file) {
        perror("Erro ao abrir customer.txt");
        free(total);
        return EXIT_FAILURE;
    }

    int customers = count_customers(customer_file);
    Banker banker = {0};

    if (customers <= 0 ||
        !banker_init(&banker, customers, resources)) {
        fprintf(stderr, "Erro: clientes ou memoria invalidos.\n");
        fclose(customer_file);
        free(total);
        return EXIT_FAILURE;
    }

    for (int j = 0; j < resources; j++)
        banker.available[j] = total[j];

    int success = 1;

    for (int i = 0; i < customers; i++) {
        char *line = read_line(customer_file);

        if (!line ||
            !customer_values(line, banker.maximum[i], resources)) {
            fprintf(stderr, "Erro: customer.txt linha %d.\n", i + 1);
            success = 0;
        } else {
            for (int j = 0; j < resources; j++) {
                banker.need[i][j] = banker.maximum[i][j];

                if (banker.maximum[i][j] > total[j]) {
                    fprintf(stderr,
                            "Erro: cliente %d excede recursos totais.\n", i);
                    success = 0;
                }
            }
        }

        free(line);

        if (!success)
            break;
    }

    fclose(customer_file);
    free(total);

    if (!success) {
        banker_free(&banker);
        return EXIT_FAILURE;
    }

    FILE *commands = fopen("commands.txt", "r");

    if (!commands) {
        perror("Erro ao abrir commands.txt");
        banker_free(&banker);
        return EXIT_FAILURE;
    }

    FILE *result = fopen("result.txt", "w");

    if (!result) {
        perror("Erro ao criar result.txt");
        fclose(commands);
        banker_free(&banker);
        return EXIT_FAILURE;
    }

    int *values = malloc((size_t)resources * sizeof(int));

    if (!values) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        success = 0;
    }

    char *line;
    int line_number = 0;

    while (success && (line = read_line(commands)) != NULL) {
        line_number++;

        int customer = 0;
        char kind = 0;

        if (!command_values(line, &customer, values,
                            resources, &kind) ||
            (kind != '*' && customer >= customers)) {
            fprintf(stderr, "Erro: commands.txt linha %d.\n",
                    line_number);
            success = 0;
        } else if (kind == '*') {
            banker_print(&banker, result);
        } else if (kind == 'Q') {
            int status = banker_request(&banker, customer, values);

            if (status == -4) {
                fprintf(stderr, "Erro na verificacao de seguranca.\n");
                success = 0;
            } else if (status == 1) {
                fprintf(result, "Allocate to customer %d the resources",
                        customer);
                print_values(result, values, resources);
            } else if (status == -1) {
                fprintf(result, "The customer %d request", customer);
                for (int j = 0; j < resources; j++)
                    fprintf(result, " %d", values[j]);
                fprintf(result,
                        " was denied because exceed its maximum allocation\n");
            } else if (status == -2) {
                fprintf(result, "The resources");
                for (int j = 0; j < resources; j++)
                    fprintf(result, " %d", banker.available[j]);
                fprintf(result,
                        " are not enough to customer %d request", customer);
                print_values(result, values, resources);
            } else {
                fprintf(result, "The customer %d request", customer);
                for (int j = 0; j < resources; j++)
                    fprintf(result, " %d", values[j]);
                fprintf(result,
                        " was denied because result in an unsafe state\n");
            }
        } else if (!banker_release(&banker, customer, values)) {
            fprintf(stderr, "Erro: liberacao invalida.\n");
            success = 0;
        } else {
            fprintf(result, "Release from customer %d the resources",
                    customer);
            print_values(result, values, resources);
        }

        free(line);
    }

    if (ferror(commands) || ferror(result)) {
        fprintf(stderr, "Erro de leitura ou escrita.\n");
        success = 0;
    }

    free(values);
    fclose(commands);

    if (fclose(result) != 0)
        success = 0;

    banker_free(&banker);

    if (!success)
        remove("result.txt");

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
