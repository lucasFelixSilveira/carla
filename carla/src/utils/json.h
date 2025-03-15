#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  JType_Object,
  JType_Array,
  JType_String,
  JType_Number, 
  JType_Boolean,
  JType_Null
} JType;

typedef struct JValue {
  JType type;
  union {
    char* string;
    double number;
    bool boolean;
    struct JObject* object;
    struct JArray* array;
  } u;
} JValue;

typedef struct {
  char* key;
  JValue value;
} JPair;

typedef struct JObject {
  JPair* pairs;
  int count;
} JObject;

typedef struct JArray {
  JValue* elements;
  int count;
} JArray;

JValue json_string(const char* value);
JValue json_number(double value);
JValue json_boolean(bool value);
JValue json_null(void);
JValue json_array(JValue* elements, int count);
JValue json_object(JPair* pairs, int count);

void json_stringify(char** json_str, JValue value);

void json_free(JValue value);

#endif