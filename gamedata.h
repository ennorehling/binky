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
	int keys[3];
	enum crblock_t type;
} crblock;

typedef struct gamedata {
	stringtable strings;
	crblock *data; /* stb array */
} gamedata;

