#include <stdio.h>
#include "json.h"
#include "errors.h"
#include "../frontend/lexer.h"

void 
errors_common(
  FILE *logs,
  TokenLocation local,
  CarlaErrors error,
  char *buffer,
  char *msg
) 
{
  JPair cPairs[2];
  cPairs[0] = (JPair) { "type", json_string ("error") };
  
  JPair ePairs[4];
  ePairs[0] = (JPair) { "message",  json_string (msg) };
  ePairs[1] = (JPair) { "code",     json_number (error) };
  ePairs[2] = (JPair) { "buffer",   json_string (buffer) };
  ePairs[3] = (JPair) { "location", json_array  ((JValue[]){
    json_number (local.posY), 
    json_number (local.posX)
  }, 2) };
  
  cPairs[1] = (JPair) { "error", json_object (ePairs, 4) };
  
  JValue data = json_object (cPairs, 2);
  char *json;

  json_stringify (&json, data);
  fprintf (logs, "%s", json);
  exit (1);
}

void 
errors_tip(
  FILE *logs,
  TokenLocation local,
  CarlaErrors error,
  char *buffer,
  char *msg,
  char *can,
  CarlaTips tip
) 
{
  JPair cPairs[2];
  cPairs[0] = (JPair) { "type", json_string ("error") };
  
  JPair ePairs[6];
  ePairs[0] = (JPair) { "message",  json_string (msg) };
  ePairs[1] = (JPair) { "code",     json_number (error) };
  ePairs[2] = (JPair) { "buffer",   json_string (buffer) };
  ePairs[3] = (JPair) { "can",      json_string (can) };
  ePairs[4] = (JPair) { "tip",      json_number (tip) };
  ePairs[5] = (JPair) { "location", json_array  ((JValue[]){
    json_number (local.posY), 
    json_number (local.posX)
  }, 2) };
  
  cPairs[1] = (JPair) { "error", json_object (ePairs, 6) };
  
  JValue data = json_object (cPairs, 2);
  char *json;

  json_stringify (&json, data);
  fprintf (logs, "%s", json);
  exit (1);
}

void 
errors_uses(
  FILE *logs,
  TokenLocation usesLocal,
  TokenLocation local,
  CarlaErrors error,
  char *uses,
  char *buffer,
  char *msg,
  char *can,
  CarlaTips tip
) 
{
  JPair cPairs[2];
  cPairs[0] = (JPair) { "type", json_string ("error") };
  
  JPair ePairs[7];
  ePairs[0] = (JPair) { "message",  json_string (msg) };
  ePairs[1] = (JPair) { "code",     json_number (error) };
  ePairs[2] = (JPair) { "buffer",   json_string (buffer) };
  ePairs[3] = (JPair) { "can",      json_string (can) };
  ePairs[4] = (JPair) { "tip",      json_number (tip) };
  ePairs[5] = (JPair) { "location", json_array  ((JValue[]){
    json_number (local.posY), 
    json_number (local.posX)
  }, 2) };
  
  JPair uPairs[2];
  uPairs[0] = (JPair) { "buffer", json_string (uses) };
  uPairs[1] = (JPair) { "location", json_array  ((JValue[]){
    json_number (usesLocal.posY), 
    json_number (usesLocal.posX)
  }, 2) };

  ePairs[6] = (JPair) { "uses", json_object (uPairs, 2) };
  cPairs[1] = (JPair) { "error", json_object (ePairs, 7) };
  
  JValue data = json_object (cPairs, 2);
  char *json;

  json_stringify (&json, data);
  fprintf (logs, "%s", json);
  exit (1);
}