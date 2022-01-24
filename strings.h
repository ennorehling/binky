#if defined(_WIN32) && (_MSC_VER >= 1300)
#define stb_p_stricmp    _stricmp
#define stb_p_strnicmp   _strnicmp
#define stb_p_strdup     _strdup
#else
#define stb_p_strdup     strdup
#define stb_p_stricmp    stricmp
#define stb_p_strnicmp   strnicmp
#endif

