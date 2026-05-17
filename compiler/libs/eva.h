#if defined(__cplusplus) && !defined(EVA_USING_C_ABI)
#error "C++ is not supported"
#endif

#ifndef EVA_H
#define EVA_H

#include <stdbool.h>
#include <stdio.h>

#define EVA_VALUE_TAG_FIELDS \
    X(eva_string, "string") \
    X(eva_number, "number") \
    X(eva_bool, "bool") \
    X(eva_map, "map") \
    X(eva_list, "list") \
    X(eva_nil, "nil")

#define X(tag, ...) tag,
typedef enum { EVA_VALUE_TAG_FIELDS } EvaValueTag;
#undef X

typedef struct EvaParser {
    size_t status;
    void *parser;
} EvaParser;

typedef struct EvaValue {
    EvaValueTag tag;
    union {
        char *string;
        double number;
        int boolean;
    } data;
} EvaValue;

EvaParser *eva_make_parser(const char *path);

void eva_dump_pointer(EvaValue value);

EvaValue eva_get_value_from_namespace(EvaParser *parser, const char *ns, const char *name);
char eva_check_exist_field_in_namespace(EvaParser *parser, const char *ns, const char *name);

int eva_get_list_length(EvaValue list);
EvaValue eva_get_list_field(EvaValue list, int index);

int eva_get_map_length(EvaValue map);
EvaValue eva_get_map_field(EvaValue map, const char *key);
EvaValue eva_get_all_keys_from_map(EvaValue map);
bool eva_check_exist_field_in_map(EvaValue map, const char *field);


#ifndef __cplusplus

#define eva_make(value) eva_make_parser(value)

#define eva_dump(value) eva_dump_pointer(value)

#define eva_get(value, ns, field) eva_get_value_from_namespace(value, ns, field)
#define eva_exist(value, ns, field) eva_get_value_from_namespace(value, ns, field)

#define eva_listlen(list) eva_get_list_length(list)
#define eva_listget(list, index) eva_get_list_field(list, index)

#define eva_maplen(map) eva_get_map_length(map)
#define eva_mapkeys(map) eva_get_all_keys_from_map(map)
#define eva_mapget(map, key) eva_get_map_field(map, key)
#define eva_mapexist(map, key) eva_check_exist_field_in_map(map, key)

#endif

#endif
