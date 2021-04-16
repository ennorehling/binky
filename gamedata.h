#pragma once

typedef enum crblock_t {
	CROBJECT,
	CRSTRINGS,
} crblock_t;

typedef struct string_index {
    char *key;
    int value;
} string_index;

typedef struct stringtable {
    string_index *index; /* stbds_sh map */
	char **strings; /* stbds_arr array */
} stringtable;

int strings_put(struct stringtable *st, const char *str);
const char *strings_get(const struct stringtable *st, int index);

enum value_t {
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_LOCATION,
} value_t;

typedef struct attribute {
    enum value_t type;
    int ikey;
    union {
        long number;
        char *string;
        short location[3];
    } value;
} attribute;

typedef struct crblock {
    unsigned int nkeys;
    int keys[3];
    int iname;
	enum crblock_t type;
    union {
        attribute *attributes; /* stbds_arr array */
    } data;
} crblock;

typedef struct gamedata {
	stringtable strings;
	crblock **blocks; /* stbds_arr array */
} gamedata;

const char *block_name(gamedata *gd, crblock *block);
