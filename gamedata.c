#include "gamedata.h"

#include "stb/stb.h"
#include "stb/stb_ds.h"

#include <assert.h>
#include <limits.h>

int strings_put(struct stringtable *st, const char *str)
{
    ptrdiff_t len;
    string_index index;

    assert(str);
    index = stbds_shgets(st->index, str);
    if (index.key) {
        return index.value;
    }
    len = stbds_arrlen(st->strings);
    if (len < INT_MAX) {
        int pos = (int)len;
        char *value = stb_p_strdup(str);
        if (value) {
            stbds_arrput(st->strings, value);
            stbds_shput(st->index, str, pos);
            return pos;
        }
        return -2;
    }
    return -1;
}

const char *strings_get(const struct stringtable *st, int index)
{
    assert(index >=0 && index < stbds_arrlen(st->strings));
    return st->strings[index];
}
