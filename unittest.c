#include "CuTest.h"

#undef STB_DEFINE
#include "stb/deprecated/stb.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma warning(disable: 4210)

typedef struct suite {
    struct suite *next;
    CuSuite *csuite;
    char *name;
} suite;

static suite *suites;

static void add_suite(CuSuite *(*csuite)(void), const char *name, int argc, char *argv[]) {
    suite *s = 0;
    if (argc > 0) {
        int i;
        for (i = 0; i != argc; ++i) {
            if (strcmp(argv[i], name) == 0) {
                s = malloc(sizeof(suite));
                break;
            }
        }
    }
    else {
        s = malloc(sizeof(suite));
    }
    if (s) {
        s->next = suites;
        s->name = stb_p_strdup(name);
        s->csuite = csuite();
        suites = s;
    }
}

void RunTests(CuSuite *suite, const char *name) {
    CuString *output = CuStringNew();

    CuSuiteRun(suite);
    CuSuiteDetails(suite, output);
    if (suite->failCount) CuSuiteSummary(suite, output);
    printf("%s: %s", name, output->buffer);
    CuStringDelete(output);
}

bool list = false;

#define ADD_SUITE(name) \
    do { \
        CuSuite *get_##name##_suite(void); \
        if (list) printf("%s\n", #name); \
        if (!list || argc>0) add_suite(get_##name##_suite, #name, argc, argv); \
    } while (0)


int RunAllTests(int argc, char *argv[])
{
    ADD_SUITE(gamedata);

    if (suites) {
        CuSuite *summary = CuSuiteNew();
        int fail_count;
        while (suites) {
            suite *s = suites->next;
            RunTests(suites->csuite, suites->name);
            summary->failCount += suites->csuite->failCount;
            summary->count += suites->csuite->count;
            CuSuiteDelete(suites->csuite);
            free(suites->name);
            free(suites);
            suites = s;
        }
        printf("\ntest summary: %d tests, %d failed\n", summary->count, summary->failCount);
        fail_count = summary->failCount;
        CuSuiteDelete(summary);
        return fail_count;
    }
    return 0;
}

int main(int argc, char **argv) {
    ++argv;
    --argc;
    if (argc > 0 && strcmp("--list", argv[0]) == 0) {
        list = true;
        ++argv;
        --argc;
    }
    return RunAllTests(argc, argv);
}
