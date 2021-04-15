#pragma once

typedef enum crblock_t {
	CRVERSION,
	CRREGION,
	CRPRICES,
	CRRESOURCES,
	CRBUILDING,
	CRSHIP,
	CRUNIT,
	CRITEMS,
	CRSKILLS,
	CRMESSAGE,
	CRSPELLS,
	CREFFECTS,
} crblock_t;

typedef struct stringtable {
	char **strings; /* stb array */
} stringtable;

typedef struct crblock {
	long keys[3];
	enum crblock_t type;
} crblock;

typedef struct crdata {
	stringtable strings;
	crblock *data; /* stb array */
} crdata;

