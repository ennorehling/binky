#pragma once

typedef struct string_index {
    char *key;
    int value;
} string_index;

typedef struct stringtable {
    string_index *map; /* stbds_sh map */
} stringtable;

int strings_put(struct stringtable *st, const char *str);
const char *strings_get(struct stringtable *st, int index);
const char *strings_find(struct stringtable *st, const char *str);
void strings_free(struct stringtable *st);

enum value_t {
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_LOCATION,
} value_t;

typedef struct attribute {
    enum value_t type;
    const char *key;
    union {
        int number;
        char *string;
        short location[3];
    } value;
} attribute;

typedef enum crblock_t {
    CROBJECT,
    CRSTRINGS,
} crblock_t;

typedef struct crblock {
    unsigned int nkeys;
    int keys[3];
    const char *name;
	enum crblock_t type;
    union {
        attribute *attributes; /* CROBJECT, stbds_arr array */
        char **strings; /* CRSTRINGS, stbds_arr array */
    } data;
} crblock;

typedef struct gamedata {
	struct stringtable *strings;
    struct crblock **blocks; /* stbds_arr array */
} gamedata;

int gamedata_turn(struct gamedata *gd);
void gamedata_free(struct gamedata *gd);
