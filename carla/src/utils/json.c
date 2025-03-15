#include "json.h"
#include <stdio.h>
#include <string.h>

JValue json_string(const char* value) {
  JValue val = { .type = JType_String, .u.string = strdup(value) };
  return val;
}

JValue json_number(double value) {
  JValue val = { .type = JType_Number, .u.number = value };
  return val;
}

JValue json_boolean(bool value) {
  JValue val = { .type = JType_Boolean, .u.boolean = value };
  return val;
}

JValue json_null(void) {
  JValue val = { .type = JType_Null };
  return val;
}

JValue json_array(JValue* elements, int count) {
  JArray* arr = malloc(sizeof(JArray));
  arr->elements = malloc(sizeof(JValue) * count);
  memcpy(arr->elements, elements, sizeof(JValue) * count);
  arr->count = count;
  JValue val = { .type = JType_Array, .u.array = arr };
  return val;
}

JValue json_object(JPair* pairs, int count) {
  JObject* obj = malloc(sizeof(JObject));
  obj->pairs = malloc(sizeof(JPair) * count);
  for (int i = 0; i < count; i++) {
    obj->pairs[i].key = strdup(pairs[i].key);
    obj->pairs[i].value = pairs[i].value;
  }
  obj->count = count;
  JValue val = { .type = JType_Object, .u.object = obj };
  return val;
}

void json_stringify(char** json_str, JValue value) {
  char buffer[1024] = {0};
  size_t offset = 0;

  switch (value.type) {
    case JType_String:
      sprintf(buffer + offset, "\"%s\"", value.u.string);
      break;
    case JType_Number:
      sprintf(buffer + offset, "%.17g", value.u.number); 
      break;
    case JType_Boolean:
      sprintf(buffer + offset, "%s", value.u.boolean ? "true" : "false");
      break;
    case JType_Null:
      sprintf(buffer + offset, "null");
      break;
    case JType_Object: {
      JObject* obj = value.u.object;
      offset += sprintf(buffer + offset, "{");
      for (int i = 0; i < obj->count; i++) {
        offset += sprintf(buffer + offset, "\"%s\":", obj->pairs[i].key);
        char* sub_str = NULL;
        json_stringify(&sub_str, obj->pairs[i].value);
        offset += sprintf(buffer + offset, "%s", sub_str);
        if (i < obj->count - 1) offset += sprintf(buffer + offset, ",");
        free(sub_str);
      }
      offset += sprintf(buffer + offset, "}");
      break;
    }
    case JType_Array: {
      JArray* arr = value.u.array;
      offset += sprintf(buffer + offset, "[");
      for (int i = 0; i < arr->count; i++) {
        char* sub_str = NULL;
        json_stringify(&sub_str, arr->elements[i]);
        offset += sprintf(buffer + offset, "%s", sub_str);
        if (i < arr->count - 1) offset += sprintf(buffer + offset, ",");
        free(sub_str);
      }
      offset += sprintf(buffer + offset, "]");
      break;
    }
  }

  *json_str = strdup(buffer);
}

void json_free(JValue value) {
  switch (value.type) {
    case JType_String:
      free(value.u.string);
      break;
    case JType_Object: {
      JObject* obj = value.u.object;
      for (int i = 0; i < obj->count; i++) {
        free(obj->pairs[i].key);
        json_free(obj->pairs[i].value);
      }
      free(obj->pairs);
      free(obj);
      break;
    }
    case JType_Array: {
      JArray* arr = value.u.array;
      for (int i = 0; i < arr->count; i++) {
        json_free(arr->elements[i]);
      }
      free(arr->elements);
      free(arr);
      break;
    }
    default:
      break;
  }
}