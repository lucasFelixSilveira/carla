#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "../utils/vector.h"
#include "../utils/strings.h"
#include "../utils/symbols.h"
#include "parser.h"
#include "types.h"
#include "llvm.h"
#include "std.h"

/**
  IMPORTANT: Auto-cast do resultado de uma expressão para o tipo correspondente ao da variável na qual é receptora
    - tipo: int64 x = msg[0] ;
      sendo msg []ascii
    
  TODO: Implement ASSIGNMENT_FIELD to NUMBERS and IDENTIFIERS in the expression place
*/

int
exponence(int x, int y)
{
  int result = 1;

  for(int i = 0; i < y; i++)
    {
      result *= x;
    }

  return result;
}

#define GET(vector, index)       ((PNode*)vector->items)[index]
#define GET_T(T, vector, index)  ((T*)vector->items)[index]
#define GETNP(T, vector, index)  ((T*)vector.items)[index]
#define COMPOP(buffer, operator) strcmp (buffer, operator) == 0
#define STATIC_TYPE_LENGTH 4

#define BEGIN_SWITCH(str)    { char *_switch_str = str;
#define CASE(val)            if( strcmp (_switch_str, (val)) == 0 ) {
#define BREAK_CASE(val)      } else if( strcmp (_switch_str, (val)) == 0 ) {
#define DEFAULT              } else {
#define END_SWITCH           }
#define BREAK                }

#define COMP_PRECOMPILATION(output, comp) llvm_alloca_t (output, "bool"); \
                                          llvm_store_l (output, 8, comp ? 1 : 0, (var - 1)); \
                                          llvm_load_number (output, 8, (var-1));

#define MATH_PRECOMPILATION(output, expr) llvm_alloca_t (output, "int64"); \
                                          llvm_store_l (output, 64, expr, (var - 1)); \
                                          llvm_load_number (output, 64, (var-1));

unsigned int var = 0;
unsigned int label_id = 0;
unsigned int carla_id = 0;
unsigned int class = 0;
int complement = 0;
int tab = 0;
int calli = 0;
int debugid = 0;
int lambda_counter = 0;
char *lambda_name;
int struct_i = 0;
char *last_load;

Vector structies;

ExprCache cache[1024];
struct ARG {
  unsigned int llvm;
  char *arg_type;
  int index;
} args[128];
int alen = 0;
int clen = 0;

typedef struct {
  unsigned int root;
} IfStatement;

typedef struct {
  char *name;
  char *type;
} BoundedField;

int
comp_get(Vector *vec, char *content) {

  for(
    int i = 0;
    i < vec->length;
    i++
  ) {
      Complement comp = GET_T(Complement, vec, i);
      if( strcmp (comp.string, content) == 0 )
        return comp.id;
    }

  return -1;
}

char
is_comp(char *operator)
{
   BEGIN_SWITCH(operator)
    CASE("==")       return 1;
    BREAK_CASE("!=") return 1;
    BREAK_CASE(">=") return 1;
    BREAK_CASE("<=") return 1;
    BREAK_CASE("<")  return 1;
    BREAK_CASE(">")  return 1;
    BREAK
  END_SWITCH

  return 0;
}

void
genT(char **tabs)
{
  if( tab == 0 )
    (*tabs)[0] = 0x0;
  else if( tab >= 1 )
    sprintf (*tabs, "  ");
}

Fn
find_fn(PNode call, Vector *vars, char *__struct)
{
  char *instance = call.data.internal_struct.instance_id;
  int result = 0;
  if( call.type == NODE_INTERNAL )
    return std_fn (call);
    
  char in = strcmp (__struct, call.data.internal_struct.__struct) == 0;
  
  char *to_find = call.data.super;
  if( call.type == NODE_INTERNAL_STRUCT )
    {
      to_find = (char*)malloc (128);
      sprintf (to_find, "%s.%s", call.data.internal_struct.__struct,  call.data.internal_struct.fn);
    }

  Variable var;
  VECTOR_CONTAINS(Variable, vars, id, to_find, &result);
  if( result )
    {
      VECTOR_FIND(Variable, vars, id, to_find, &var);

      if( var.v_type != Function )
        {
          printf ("%s IT IS NOT A FUNCTION!", var.id);
          exit(0);
        }

      if(! (var.public || in) )
        {
          printf ("%s IT IS A PRIVATE METHOD!", var.id);
          exit(0);
        }

      return (Fn) {
        .id  = (call.type == NODE_INTERNAL_STRUCT) 
               ? call.data.internal_struct.fn 
               : call.data.super,
        .lib = (call.type == NODE_INTERNAL_STRUCT) 
               ? call.data.internal_struct.__struct 
               : "SUPER",
        .type = var.type,
        .instance = instance,
        .args = ""
      };
    }
  else
    {
      printf ("%s NOT FOUND!", to_find);
      exit(0);
    }
}

/*
  LLVM util functions
*/

int
llvm_sizeof (char *type)
{
  if( type[0] == 'i' || type[0] == 'u' )
    {
      char *bits = substr (type, 1, strlen (type) - 1);
      int bytes = atoi (bits) / 8;
      return bytes;
    }

  if( strcmp (type, "ptr") == 0 )
    return 8;

  return startsWith(type, "%.carla.struct.") ? 8 : 0;
}

char *
llvm_type (char *type)
{

  char result = 0;
  VECTOR_CONTAINS (Structies, &structies, struct_id, type, &result);

  if( result )
    {
      Structies __struct;
      VECTOR_FIND (Structies, &structies, struct_id, type, &__struct);
      char *prefix = "%.carla.struct.";
      char *buffer = malloc (strlen (prefix) + 5);
      sprintf (buffer, "%s%d", prefix, __struct.carla_id);
      free (prefix);
      return buffer;
    }

  if( strcmp (type, "llvm<bool>") == 0 )
    return "i1";

  const char ptrsufix = '*';
  if( type[strlen (type) - 1] == ptrsufix )
    return "ptr";

  const char arrayprefix = '[';
  if( type[0] == arrayprefix )
    return "ptr";

  char *__type = (char*)malloc (128);
  char *prefix = "int";

  integer_by_bits: {
    size_t len = strlen (prefix);
    for( char i = 0; i < len; i++ )
      {
        if( prefix[i] != type[i] )
          break;
        if( i == (len - 1) )
          {
            char *bits = substr (type, len, strlen (type) - (len - 1));
            char *result = (char*)malloc (strlen (type));
            sprintf (result, "i%s", bits);
            free (bits);
            free (__type);
            return result;
          }
      }
    if( prefix[0] == 'u' )
      goto __next_step_type;
  }

  prefix = "uint";
  goto integer_by_bits;
  __next_step_type: {}

  if( strcmp (type, "void") == 0 )
    {
      memcpy (__type, type, strlen (type));
      __type[4] = 0;
      return __type;
    }

  char *matrix[STATIC_TYPE_LENGTH][16] = {
    { "ascii", "i8" },
    { "bool", "i8" },
    { "char", "i8" },
    { "byte",  "i8" }
  };

  for( int i = 0; i < STATIC_TYPE_LENGTH; i++ )
    {
      if( strcmp (matrix[i][0], type) != 0 )
        continue;

      memcpy (__type, matrix[i][1], strlen (matrix[i][1]));
      __type[strlen (matrix[i][1])] = 0;
      return __type;
    }

  return strdup ("void");
}

void
llvm_load_number(FILE *output, int bits, unsigned int to_load)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  fprintf (output, "%s%c%d = load i%d, ptr %c%d, align %d\n",
          tabs, '%',
          var++,
          bits, '%',
          to_load,
          (bits / 8)
  );
  free (tabs);
}

void
llvm_alloca(FILE *output, PNode node)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  char *type = llvm_type (node.data.definition.type);
  fprintf (output, "%s%c%d = alloca %s, align %d\n",
          tabs, '%',
          var++,
          type,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_alloca_t(FILE *output, char *__type)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  char *type = llvm_type (__type);
  fprintf (output, "%s%c%d = alloca %s, align %d\n",
          tabs, '%',
          var++,
          type,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

char *
backPtr(char *type)
{
  if( isType (type) )
    return type;

  if( type[0] == '[' )
    {
      uint16_t len = 0;
      while(type[len++] != ']');
      return substr (type, len, (strlen (type) - len));
    }

  if( type[strlen (type) - 1] == '*' )
    {
      return substr (type, 0, (strlen (type) - 1));
    }

  return "void";
}

void
llvm_store(FILE *output, PNode node, int __src_var__, int __dst_var__)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  char *type = llvm_type (node.data.definition.type);
  fprintf (output, "%sstore %s %c%d, ptr %c%d, align %d\n",
          tabs,
          type, '%',
          __src_var__, '%',
          __dst_var__,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_store_l(FILE *output, int bits, int original, uint32_t dist)
{
  int number = original;
  if( original > (exponence(2, bits) / 2) - 1 )
    number = -(((exponence(2, bits) / 2)) - (number - ((exponence(2, bits) / 2))));

  char *tabs = (char*)malloc (8);
  genT (&tabs);
  fprintf (output, "%sstore i%d %d, ptr %c%d, align %d\n",
          tabs,
          bits,
          number, '%',
          dist,
          (bits / 8)
  );
  free (tabs);
}

Variable
llvm_get(Vector *vec, char *id)
{
  for(
    int i = 0;
    i < vec->length;
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( strcmp (var.id, id) == 0 )
        return var;
    }

  printf ("%s NOT FOUND!", id);
  exit (0);
}

uint32_t
llvm_get_i(Vector *vec, char *id)
{
  for(
    int i = 0;
    i < vec->length;
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( strcmp (var.id, id) == 0 )
        return i;
    }

  printf ("%s NOT FOUND!", id);
  exit (0);
}

Variable
llvm_get_f(Vector *vec, unsigned int id)
{
  for(
    int i = 0;
    i < vec->length;
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( var.llvm == id )
        return var;
    }

  printf ("%d NOT FOUND!", id);
  exit (0);
}

void
llvm_resize(FILE *output, unsigned int collect, char *type, Variable changable)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);

  fprintf (output, "%s%c%d = ",
          tabs, '%',
          var++
  );
  free (tabs);

  if( llvm_sizeof (llvm_type (type)) > llvm_sizeof (llvm_type (changable.type)) )
    fprintf (output, "sext ");
  else
    fprintf (output, "trunc ");

  fprintf (output, "%s %c%d to %s\n",
          llvm_type (changable.type), '%',
          collect,
          llvm_type (type)
  );

}

void
llvm_operation(FILE *output, char *operator, unsigned int left, unsigned int right)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);

  fprintf (output, "%s%c%d = ",
          tabs, '%',
          var++
  );

  BEGIN_SWITCH(operator)
    CASE("+") fprintf (output, "add nsw ");
    BREAK_CASE("-") fprintf (output, "sub nsw ");
    BREAK_CASE("*") fprintf (output, "mul nsw ");

    BREAK_CASE("%") fprintf (output, "srem ");
    BREAK_CASE("/") fprintf (output, "sdiv ");
    BREAK
  END_SWITCH

  fprintf (output, "i64 %c%d, %c%d\n", '%',
          left, '%',
          right
  );
}

void
llvm_label(FILE *output, char *name)
{
  fprintf (output, ".l%d.carla.%s:\n",
          label_id++,
          name
  );
}

void
llvm_label_id(FILE *output, unsigned int id, char *name)
{
  fprintf (output, ".l%d.carla.%s:\n",
          id,
          name
  );
}

void
llvm_br(FILE *output, unsigned int id, char *name)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  fprintf (output, "%sbr label %c.l%d.carla.%s\n",
          tabs, '%',
          id,
          name
  );
  free (tabs);
}

void
llvm_icmpbr(FILE *output, unsigned int icmp, unsigned int general_id, char *true_name, char *false_name)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  fprintf (output, "%sbr i1 %c%d, label %c.l%d.carla.%s, label %c.l%d.carla.%s\n",
          tabs, '%',
          icmp, '%',
          general_id,
          true_name, '%',
          general_id,
          false_name
  );
  free (tabs);
}

void
llvm_resize_bits(FILE *output, unsigned int collect, char *type, char *change)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);

  fprintf (output, "%s%c%d = ",
          tabs, '%',
          var++
  );
  free (tabs);

  if(
    ( llvm_sizeof (llvm_type (type)) > llvm_sizeof (llvm_type (change)) )
    || strcmp (change, "llvm<bool>") == 0
  )
    fprintf (output, "sext ");
  else
    fprintf (output, "trunc ");

  fprintf (output, "%s %c%d to %s\n",
          llvm_type (change), '%',
          collect,
          llvm_type (type)
  );

}

void
llvm_comp_bits(FILE *output, char bits, char *operator, unsigned int left, unsigned int right, char resize)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);

  fprintf (output, "%s%c%d = icmp ",
          tabs, '%',
          var++
  );

  BEGIN_SWITCH(operator)
    CASE("==") fprintf (output, "eq ");
    BREAK_CASE("!=") fprintf (output, "ne ");
    BREAK_CASE(">=") fprintf (output, "sge ");
    BREAK_CASE("<=") fprintf (output, "sle ");
    BREAK_CASE("<") fprintf (output, "slt ");
    BREAK_CASE(">") fprintf (output, "sgt ");
    BREAK
  END_SWITCH

  fprintf (output, "i%d %c%d, %c%d\n",
          bits,'%',
          left, '%',
          right
  );

  if( resize )
    llvm_resize_bits (output, (var-1), "int8", "llvm<bool>");
}

void
llvm_comp(FILE *output, char *operator, unsigned int left, unsigned int right)
{ llvm_comp_bits (output, 64, operator, left, right, 1); }

void
llvm_load(FILE *output, Variable toLoad)
{
  char *tabs = (char*)malloc (8);
  last_load = toLoad.type;
  char *type = llvm_type (toLoad.type);
  genT (&tabs);
  fprintf (output, "%s%c%d = load %s, ptr %c%d, align %d\n",
          tabs, '%',
          var++,
          type, '%',
          toLoad.llvm,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_getelementptr(FILE *output, ExprCache vec)
{
  char *tabs = (char*)malloc (8);
  char *type = llvm_type (vec.info.access.type);
  genT (&tabs);
  fprintf (output, "%s%c%d = getelementptr inbounds %s, %s %c%d, i64 %c%d\n",
          tabs, '%',
          var,
          type,
          type, '%',
          vec.info.access.load_vec, '%',
          (var-1)
  );
  var++;
  free (type);
  free (tabs);
}

void 
llvm_access_field(FILE *output, unsigned int struct_id, unsigned int struct_ptr, int field)
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  fprintf (output, "%s%c%d = getelementptr %c.carla.struct.%d, ptr %c%d, i32 0, i32 %d\n",
          tabs, '%',
          var, '%',
          struct_id, '%', 
          struct_ptr, 
          field
  );
  var++;
  free (tabs);
}

void
llvm_load_field(FILE *output, Variable field, unsigned int access) 
{
  char *tabs = (char*)malloc (8);
  genT (&tabs);
  fprintf (output, "%s%c%d = load %s, ptr %c%d, align %d\n", 
          tabs, '%', 
          var++,
          llvm_type (field.type), '%',
          access, 
          llvm_sizeof (llvm_type (field.type))
  );
  free (tabs);
}

void
llGenerate(FILE *output, Vector *pTree)
{

  Vector vars              = vector_init (sizeof (Variable));
  Vector scopes            = vector_init (sizeof (ScopeType));
  Vector retStack          = vector_init (sizeof (RetStack));
  Vector complements       = vector_init (sizeof (Complement));
  Vector ifstatements_root = vector_init (sizeof (IfStatement));
  Vector bounded_vec       = vector_init (sizeof (BoundedField));
  
  structies                = vector_init (sizeof (Structies));
  char *struct_type        = (char*)malloc (1024);
  struct_type[0] = 0x0;


  char in_struct = 0;
  char in_struct_fields = 0;
  char *struct_name;
  int struct_fields_len = 0;

  int i = 0;
  for(; i < pTree->length; i++ )
    {
      PNode branch = GET(pTree, i);

      goto jmp_expr;
      wait_expr:
        {
          while(1)
            {
              PNode next = GET(pTree, i + 1);

              switch (next.type)
                {
                  case NODE_FIELD_ACCESS:
                    { i++;
                      Variable str;
                      VECTOR_FIND (Variable, &vars, id, next.data.value, &str);

                      Structies result;
                      VECTOR_FIND (Structies, &structies, struct_id, str.type, &result);

                      PNode next_n = GET(pTree, i + 1);
                      i++;
                      
                      switch(next_n.type)
                        {
                          case NODE_SINGLE:
                            { 
                              printf ("\n\n; [CARLA DEBUG]: %s\n\n", "");
                              Variable field;
                              BoundedField bound;
                              char _result = 0;
                              VECTOR_CONTAINS (Variable, &vars, id, next_n.data.single.data.value, &_result);
                              if( _result ) 
                                {
                                  VECTOR_FIND_AND (Variable, &vars, id, next_n.data.single.data.value, __struct, struct_name, &field);
                                  llvm_access_field (output, result.carla_id, str.llvm, field.struct_i); 
                                  llvm_load_field (output, field, (var-1));
                                }
                              else
                                { 
                                  int resultG = 0;
                                  VECTOR_FIND_GET_INDEX (BoundedField, &bounded_vec, name, next_n.data.single.data.value, &bound, &resultG);
                                  llvm_access_field (output, result.carla_id, str.llvm, resultG); 
                                  llvm_load_field (output, (Variable) {
                                    .type = bound.type
                                  }, (var-1));
                                }
                              ExprCache resolve = cache[--clen];
                              switch(resolve.type)
                                {
                                  case VAR_DECLARATION:
                                    {
                                      llvm_store (output, (PNode) {
                                        .data.definition.type = (_result) ? field.type : bound.type,
                                      }, (var-1), resolve.info.var.llvm);
                                      goto finish;
                                    } break;
                                  default: break;
                                }
                            } break;

                            case NODE_ASSIGNMENT:
                              { Variable field;
                                VECTOR_FIND_AND (Variable, &vars, id, next_n.data.value, __struct, str.type, &field);
                                llvm_access_field (output, result.carla_id, str.llvm, field.struct_i); 
                              
                                cache[clen++] = (ExprCache) {
                                  .type       = ASSIGNMENT_FIELD,
                                  .info.llvm  = (var-1)
                                };

                                continue;
                              } break;

                            default: break;
                          }
                    } break;

                  case NODE_ACCESS:
                    {
                      i++;
                      Variable access = llvm_get (&vars, next.data.value);

                      llvm_load (output, access);
                      cache[clen++] = (ExprCache) {
                        .type = ACCESS_EXPR,
                        .info = {
                          .access = {
                            .need_load = 1,
                            .load_vec  = (var-1),
                            .type      = access.type,
                            .id        = access.id
                          }
                        }
                      };

                    } break;

                  case NODE_INTERNAL:
                  case NODE_INTERNAL_SUPER:
                  case NODE_INTERNAL_STRUCT:
                    {
                      i++;
                      fprintf (output, "; [CARLA DEBUG]: Function added to stack: %s.\n", find_fn (next, &vars, struct_name).id);
                      cache[clen++] = (ExprCache) {
                        .type = FUNCTION_CALL,
                        .info = {
                          .fn_call = find_fn (next, &vars, struct_name)
                        }
                      };
                      calli++;
                    } break;

                  case NODE_OPERATION:
                    {
                      i++;

                      Token left_data       = next.data.operation.left;
                      Token operator_data   = next.data.operation.operation;
                      Token right_data      = next.data.operation.right;
                      char left_int         = left_data.type  == Integer;
                      char left_identifier  = left_data.type  == Identifier;
                      char right_int        = right_data.type == Integer;
                      char right_identifier = right_data.type == Identifier;


                      if( left_int && right_int )
                        {
                          int left = atoi (left_data.buffer);
                          int right = atoi (right_data.buffer);

                          int bytes = 0;

                          BEGIN_SWITCH(operator_data.buffer)
                            CASE("==")       bytes = 1; COMP_PRECOMPILATION(output, left == right);
                            BREAK_CASE("!=") bytes = 1; COMP_PRECOMPILATION(output, left != right);
                            BREAK_CASE(">=") bytes = 1; COMP_PRECOMPILATION(output, left >= right);
                            BREAK_CASE("<=") bytes = 1; COMP_PRECOMPILATION(output, left <= right);

                            /* MATH precompilation */
                            BREAK_CASE("+") bytes = 1; MATH_PRECOMPILATION(output, left + right);
                            BREAK_CASE("-") bytes = 1; MATH_PRECOMPILATION(output, left - right);
                            BREAK_CASE("/") bytes = 1; MATH_PRECOMPILATION(output, left / right);
                            BREAK_CASE("*") bytes = 1; MATH_PRECOMPILATION(output, left * right);
                            BREAK_CASE("%") bytes = 1; MATH_PRECOMPILATION(output, left % right);
                            BREAK
                          END_SWITCH

                          fprintf (output, "; [CARLA DEBUG]: Pre-compiled obvious operation\n");

                          if( clen > 0 )
                            {
                              ExprCache resolve = cache[clen - 1];

                              char *type = (resolve.type == RETURN_KEY) ?
                                resolve.info.node.data.definition.type
                                : (resolve.type == VAR_DECLARATION) ?
                                  resolve.info.var.node.data.definition.type
                                  : "int64";

                              char *buffer = (char*)malloc (16);
                              sprintf (buffer, "int%d", bytes * 8);

                              if( llvm_sizeof (llvm_type (type)) != bytes )
                                llvm_resize_bits (output, (var-1), type, buffer);

                              free (buffer);

                              if( resolve.type == FUNCTION_CALL )
                                {
                                  args[alen++] = (struct ARG) {
                                    .llvm = (var - 1),
                                    .arg_type = type,
                                    .index = calli
                                  };
                                }
                            }
                        }
                      else
                        {
                          if( left_identifier )
                            {
                              Variable id = llvm_get (&vars, left_data.buffer);
                              llvm_load (output, id);
                              if( llvm_sizeof (llvm_type (id.type)) != 8 )
                                llvm_resize_bits (output, (var-1), "int64", id.type);
                            }
                          else if( left_int )
                            {
                              int left = atoi (left_data.buffer);
                              llvm_alloca_t (output, "int64");
                              llvm_store_l (output, 64, left, (var-1));
                              llvm_load_number (output, 64, (var-1));
                            }

                          unsigned int left_var = (var-1);

                          if( right_identifier )
                            {
                              Variable id = llvm_get (&vars, right_data.buffer);
                              llvm_load (output, id);
                              if( llvm_sizeof (llvm_type (id.type)) != 8 )
                                llvm_resize_bits (output, (var-1), "int64", id.type);
                            }
                          else if( right_int )
                            {
                              int right = atoi (right_data.buffer);
                              llvm_alloca_t (output, "int64");
                              llvm_store_l (output, 64, right, (var-1));
                              llvm_load_number (output, 64, (var-1));
                            }

                          unsigned int right_var = (var-1);

                         switch(operator_data.type)
                          { case MathOP:
                              {
                                llvm_alloca_t (output, "int64");
                                llvm_operation (output, operator_data.buffer, left_var, right_var);
                                llvm_store (output, (PNode) {
                                  .data.definition.type = "int64"
                                } , (var - 1), (var - 2));
                                llvm_load_number (output, 64, (var-2));
                              } break;

                            case ComparationOP:
                              {
                                fprintf (output, "; where\n");
                                llvm_alloca_t (output, "bool");
                                llvm_comp (output, operator_data.buffer, left_var, right_var);
                                llvm_store (output, (PNode) {
                                  .data.definition.type = "bool"
                                } , (var - 1), (var - 3));
                                llvm_load_number (output, 8, (var-3));
                              } break;
                            default: break;
                          }

                          ExprCache resolve = cache[clen - 1];
                          if( resolve.type == VAR_DECLARATION )
                            {
                              llvm_store (output, (PNode) {
                                .data.definition.type = resolve.info.var.node.data.definition.type
                              } , (var - 1), resolve.info.var.llvm);
                              break;

                              if( GET(pTree, i).type == NODE_EEXPR  )
                                clen--;
                            }

                          if( resolve.type == ACCESS_EXPR )
                            {
                              llvm_getelementptr (output, resolve);
                              llvm_load (output, (Variable) {
                                .llvm = (var-1),
                                .type = resolve.info.access.type
                              });
                              clen--;
                              break;
                            }

                          if( resolve.type == IF_KEY || resolve.type == ELIF_KEY )
                            {
                              llvm_store (output, (PNode) {
                                .data.definition.type = "bool"
                              }, (var-1), resolve.info.var.llvm);
                              break;
                            }

                          if( resolve.type == FUNCTION_CALL )
                            {
                              args[alen++] = (struct ARG) {
                                .llvm = (var - 1),
                                .arg_type = (operator_data.type == MathOP) ? "int64" : "bool",
                                .index = calli
                              };
                              break;
                            }
                        }

                    } break;

                  case NODE_SINGLE:
                    {
                      i++;
                      ExprCache resolve = cache[--clen];
                      switch(next.data.single.type)
                        {
                          case NODE_NUMBER:
                            {
                              switch (resolve.type)
                                {
                                  case RETURN_KEY:
                                    {
                                      llvm_alloca_t (output, resolve.info.node.data.definition.type);
                                      llvm_store_l (output, llvm_sizeof (llvm_type (resolve.info.node.data.definition.type)) * 8, (int)next.data.single.data.number, (var - 1));
                                      llvm_load (output, (Variable) { .llvm = (var-1), .type = resolve.info.node.data.definition.type });
                                    } break;

                                  case VAR_DECLARATION:
                                    {
                                      llvm_store_l (output, llvm_sizeof (llvm_type (resolve.info.var.node.data.definition.type)) * 8, (int)next.data.single.data.number, resolve.info.var.llvm);
                                    } break;

                                  case ACCESS_EXPR:
                                    {
                                      llvm_alloca_t (output, "int64");
                                      llvm_store_l (output, 64, (int)next.data.single.data.number, (var - 1));
                                      llvm_load (output, (Variable) { .llvm = (var-1), .type = "int64"});
                                    } break;

                                  default:
                                    {
                                      llvm_alloca_t (output, "int64");
                                      llvm_store_l (output, 64, (int)next.data.single.data.number, (var - 1));
                                    } break;

                                }
                            } break;
                          case NODE_TEXT: {
                            switch (resolve.type)
                              {
                                case FUNCTION_CALL:
                                  llvm_alloca_t (output, "[]byte");
                                  break;
                                default: break;
                              }

                            int already_exist = comp_get (&complements, next.data.single.data.value);
                            if( already_exist == -1  )
                              {
                                vector_push (&complements, ((void*)(&(Complement) {
                                  .id = complement,
                                  .string = strdup (next.data.single.data.value),
                                  .type = 0
                                })));
                                already_exist = (complement++);
                              }

                            char *tabs = (char*)malloc (8);
                            genT (&tabs);

                            fprintf (output, "%s%c%d = getelementptr inbounds [%d x i8], ptr @.carla.static.str.%d, i32 0, i32 0\n",
                                    tabs, '%',
                                    var++,
                                    ((int)strlen (next.data.single.data.value) + 1),
                                    already_exist
                            );

                            free (tabs);

                            if( resolve.type != RETURN_KEY )
                              {
                                llvm_store (output, (PNode) {
                                  .data.definition.type = "[]byte"
                                } , (var - 1), (var - 2));
                              }

                          } break;
                          case NODE_ID:
                            {
                              Variable id = llvm_get (&vars, next.data.value);
                              llvm_load (output, id);

                              switch (resolve.type)
                                {
                                  case VAR_DECLARATION:
                                  case ACCESS_EXPR:
                                  case RETURN_KEY:
                                    {
                                      char *type = (resolve.type == RETURN_KEY) ?
                                        resolve.info.node.data.definition.type
                                        : (resolve.type == VAR_DECLARATION) ?
                                          resolve.info.var.node.data.definition.type
                                          : "int64";

                                      char isInteger = types_int (type) && types_int (id.type);
                                      if(! isInteger )
                                        break;

                                      if( llvm_sizeof (llvm_type (type)) != llvm_sizeof (llvm_type (id.type)) )
                                          llvm_resize (output, (var - 1), type, id);

                                      switch (resolve.type)
                                        {
                                          case VAR_DECLARATION:
                                            clen++; break;
                                          default: break;
                                        }
                                    } break;

                                  default: break;
                                }
                            } break;
                          default: break;
                        }

                      switch (resolve.type)
                        {
                          case ACCESS_EXPR:
                            {
                              llvm_getelementptr (output, resolve);
                              llvm_load (output, (Variable) { .llvm = (var-1), .type = backPtr (resolve.info.access.type) });

                              if( clen > 0 && cache[clen - 1].type == FUNCTION_CALL )
                                {
                                  args[alen++] = (struct ARG) {
                                    .llvm = (var - 1),
                                    .arg_type = backPtr (resolve.info.access.type),
                                    .index = calli
                                  };
                                }
                            } break;

                          case FUNCTION_CALL:
                            {
                              if( next.data.single.type == NODE_NUMBER )
                                {
                                  llvm_load (output, (Variable) { .llvm = (var-1), .type = "int64" });
                                  fprintf (output, "; [CARLA DEBUG]: The argument is a number\n");
                                }

                              if( next.data.single.type == NODE_TEXT )
                                {
                                  llvm_load (output, (Variable) { .llvm = (var-2), .type = "[]byte" });
                                  fprintf (output, "; [CARLA DEBUG]: The argument is a text\n");
                                }

                              args[alen++] = (struct ARG) {
                                .llvm = (var - 1),
                                .arg_type =
                                  (next.data.single.type == NODE_NUMBER) ?
                                    "int64" :
                                    (next.data.single.type == NODE_TEXT) ?
                                      "[]byte" :
                                      llvm_get (&vars, next.data.single.data.value).type,
                                .index = calli
                              };

                              clen++;
                            } break;

                          case RETURN_KEY:
                            { clen++; } break;

                          case VAR_DECLARATION:
                            {
                              if( next.type == NODE_OPERATION ) {
                                int bits = 8;
                                if( next.data.operation.operation.type != ComparationOP )
                                  bits = 64;

                                llvm_store_l (output, bits, (var-1), (var-3));
                              }
                            } break;

                          default: break;
                        }

                    } break;

                  case NODE_CLOSE:
                    {
                      i++;
                      ExprCache resolve = cache[--clen];
                      if( resolve.type != FUNCTION_CALL )
                        goto __llvm_gen_error__;

                      char *tabs = (char*)malloc (8);
                      genT (&tabs);

                      Variable _var;
                      if( resolve.info.fn_call.instance != NULL )
                        {
                          VECTOR_FIND (Variable, &vars, id, resolve.info.fn_call.instance, &_var);
                          llvm_load (output, _var);
                        }

                      if( strcmp (resolve.info.fn_call.type, "void") != 0 )
                        {
                          fprintf (output, "%s%c%d = ",
                                  tabs, '%',
                                  var++
                          );
                        }
                      else
                        fprintf (output, "%s", tabs);
                      free (tabs);

                      char o = 0;
                      if( strcmp (resolve.info.fn_call.lib, "SUPER") == 0 )
                        {
                          fprintf (output, "call %s @%s(",
                                  llvm_type (resolve.info.fn_call.type),
                                  resolve.info.fn_call.id
                          );
                        }
                      else
                        {

                          fprintf (output, "call %s @%s.%s(",
                                  llvm_type (resolve.info.fn_call.type),
                                  resolve.info.fn_call.lib,
                                  resolve.info.fn_call.id
                          );

                          if( resolve.info.fn_call.instance != NULL ) 
                            {
                              fprintf(output, "%s %c%d",
                                llvm_type (_var.type), '%',
                                (strcmp (resolve.info.fn_call.type, "void") == 0) ? (var - 1) : (var - 2)
                              );
                              o = 1;
                            } 
                        }

                      int j = 0;
                      int cpy_alen = alen;
                      for(; j < cpy_alen && args[j].index == calli; j++ )
                        {
                          fprintf (output, "%s%s %c%d",
                                  (!j) ? (o) ? ", " : "" : ", ",
                                  llvm_type (args[j].arg_type), '%',
                                  args[j].llvm
                          );
                          alen--;
                        }


                      calli--;
                      fprintf (output, ")\n");

                      if( clen > 0 && cache[clen - 1].type == FUNCTION_CALL )
                        {
                          char *type = llvm_type (resolve.info.fn_call.type);
                          args[alen++] = (struct ARG) {
                            .llvm = (var - 1),
                            .arg_type = resolve.info.fn_call.type,
                            .index = calli
                          };
                        }

                      fprintf (output, "; [CARLA DEBUG]: %d items remaining in the stack.\n", alen);
                    } break;

                  default:
                    goto finish;
                }

              continue;
              finish: { break; }
            }

            if( clen > 0 )
              {
                ExprCache resolve = cache[clen - 1];
                switch(resolve.type)
                  {
                    case RETURN_KEY:
                      {
                        clen--;

                        char *tabs = (char*)malloc (8);
                        genT (&tabs);
                        fprintf (output, "%sret %s %c%d\n",
                                tabs,
                                llvm_type (resolve.info.node.data.definition.type), '%',
                                (var - 1)
                        );
                        free (tabs);
                        continue;
                      }

                    case VAR_DECLARATION:
                      {
                        clen--;
                        llvm_store (output, resolve.info.var.node, (var - 1), resolve.info.var.llvm);
                      } break;

                    case ELIF_KEY:
                    case IF_KEY:
                      {
                        clen--;
                        PNode next = GET(pTree, i + 1);

                        switch(next.type)
                        {
                          case NODE_BEGIN:
                            { llvm_load_number (output, 8, resolve.info.var.llvm);

                              unsigned int __carla_boolean = (var-1);

                              llvm_alloca_t (output, "bool");
                              llvm_store_l (output, 8, 0, (var-1));
                              llvm_load_number (output, 8, (var-1));
                              unsigned int __carla_false = (var-1);

                              llvm_comp_bits (output, 8, "!=", __carla_boolean, __carla_false, 0);

                              unsigned int begin = label_id;
                              llvm_icmpbr (output, (var-1), begin, "that", "else");
                              llvm_label (output, "that");

                              scope_t t = If_scope;
                              if( resolve.type == ELIF_KEY )
                                t = Elif_scope;

                              ScopeType stype = (ScopeType) {
                                .type = t,
                                .label_id = begin,
                                ._if.false = __carla_false,
                              };

                              tab++;

                              vector_push (&scopes, ((void*)&stype));

                            } break;
                          // case NODE_THEN: break;
                          default: break;
                        }
                      } break;

                    default: break;
                  }
              }
          continue;
        }
      jmp_expr: {}

      switch(branch.type)
        {
          /* type id (; | =) */
          case NODE_DEFINITION:
            {

              PNode lamb;
              if( (lamb = GET(pTree, i+1)).type == NODE_LAMBDA )
                {
                  i += 2;

                  char *type = llvm_type (branch.data.definition.type);
                  char *tabs = (char*)malloc (8);
                  genT (&tabs);
                  tab++;

                  char *prefix = (char*)malloc (128);
                  prefix[0] = 0;
                  if( in_struct )
                    sprintf (prefix, "%s.", struct_name);

                  fprintf (output, "\n%sdefine %s @%s%s(",
                          tabs,
                          type,
                          prefix,
                          branch.data.definition.id
                  );
                  free (tabs);

                  char *name = branch.data.definition.id;

                  if( in_struct ) 
                    {
                      name = (char*)malloc (128);
                      sprintf (name, "%s.%s", struct_name, branch.data.definition.id);
                    }

                  vector_push (&vars, ((void*)&(Variable) {
                    .v_type = Function,
                    .type   = branch.data.definition.type,
                    .public = (! in_struct ) ? 1 : lamb.data.our,
                    .llvm   = 0,
                    .tab    = tab,
                    .id     = name
                  }));

                  lambda_counter += 1;
                  lambda_name = branch.data.definition.id;
                  vector_push (&retStack, ((void*)&(RetStack) {
                    .type = branch.data.definition.type
                  }));

                  int j = 0;
                  PNode arg;
                  if( GET(pTree, i).type == NODE_CLOSE && GET(pTree, i+1).type == NODE_BEGIN )
                    {
                      i += 1;
                      goto only_open;
                    }

                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    {
                      switch(arg.type)
                        {
                          case NODE_DEFINITION:
                            {
                              char *prefix = (char*)malloc (3);
                              if( j > 0 )
                                sprintf (prefix, ", ");
                              else memset (prefix, 0, 3);
                              char *type = llvm_type (arg.data.definition.type);
                              fprintf (output, "%s%s %c%d",
                                      prefix,
                                      type, '%',
                                      var++
                              );
                              free (prefix);
                            } break;

                          default: {
                            goto __llvm_gen_error__;
                          } break;
                        }
                    }

                  only_open: {}
                  int q = j;

                  fprintf (output, ") {\n");
                  if( strcmp (branch.data.definition.id, "main") == 0 )
                    {
                      fprintf (output, "entry:\n");
                    }
                  var++;
                  tab++;

                  j = 0;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    {
                      vector_push (&vars, ((void*)&(Variable) {
                        .v_type = Normal,
                        .type   = arg.data.definition.type,
                        .llvm   = var,
                        .tab    = tab,
                        .id     = arg.data.definition.id
                      }));
                      llvm_alloca (output, arg);
                    }

                  j = 0;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    llvm_store (output, arg, var - (q * 2) - 1 + j, var - q + j);

                  free (type);
                  i += q;

                  ScopeType stype = (ScopeType) {
                    .type = Lambda
                  };
                  vector_push (&scopes, ((void*)&stype));

                  continue;
                }

              if(! in_struct_fields ) 
                {
                  if(! branch.data.definition.is_bound ) 
                    vector_push (&vars, ((void*)&(Variable) {
                      .v_type  = Normal,
                      .bounded = branch.data.definition.is_bound,
                      .type    = branch.data.definition.type,
                      .llvm    = var,
                      .tab     = tab,
                      .id      = branch.data.definition.id
                    }));

                  if(! branch.data.definition.is_bound ) 
                    llvm_alloca (output, branch);
                  else 
                    {
                      vector_push (&bounded_vec, ((void*)&(BoundedField) {
                        .name = branch.data.definition.id,
                        .type = branch.data.definition.type
                      }));
                    }

                  if(! branch.data.definition.hopeful )
                    continue;

                  cache[clen++] = (ExprCache) {
                    .type = VAR_DECLARATION,
                    .info = {
                      .var = {
                        .llvm = (var - 1),
                        .node = branch
                      }
                    }
                  };

                  goto wait_expr;
                }

                int first = 1;

              sprintf (struct_type, "%s%s%s", 
                      struct_type,
                      (struct_fields_len > 0) ? ", " : "",
                      llvm_type (branch.data.definition.type)
              );
              struct_fields_len++;

              vector_push (&vars, ((void*)&(Variable) {
                .__struct = strdup (struct_name),
                .tab      = -1,
                .v_type   = StructField,
                .struct_i = struct_i++,
                .type     = branch.data.definition.type,
                .id       = branch.data.definition.id
              }));

            } break;

          case NODE_END:
            {

              if( in_struct_fields )
                {
                  in_struct_fields = !in_struct_fields;
                  fprintf_start (output, struct_type);
                  free (struct_type);
                  struct_name = "";
                  struct_i = 0;
                  continue;
                }

              int index = scopes.length - 1;
              ScopeType scope = GETNP(ScopeType, scopes, index);
              char jump_to_expr = 0;

              tab -= (
                  scope.type    == For
                  || scope.type == For_complex
                ) ? 0 : 1;
              char *tabs = (char*)malloc (8);
              genT (&tabs);
              if( tab == 0 )
                var = 0;

              switch(scope.type)
                {
                  case Lambda:
                    {
                      if( lambda_counter > 0 ) 
                        {
                          int last = retStack.length - 1;
                          BEGIN_SWITCH(lambda_name)
                            CASE("main")
                              char *type = GETNP(RetStack, retStack, last).type;
                              fprintf (output, "; [CARLA DEBUG]: Default success return, in case the return is not done manually.\n  ret %s 0\n", llvm_type (type));
                              vector_remove (&retStack, last);
                            DEFAULT
                              char *type = GETNP(RetStack, retStack, last).type;
                              if( strcmp (type, "void") == 0 ) 
                                {
                                  fprintf (output, "  ret void\n");
                                }
                              vector_remove (&retStack, last);
                            BREAK
                          END_SWITCH
                          lambda_counter--;
                        }

                      fprintf (output, "}\n; [CARLA DEBUG]: Tab n: %d", tab);
                      if( tab != 0 ) 
                        tab = 0;

                      var = 0;
                    } break;

                  case Elif_scope:
                  case If_scope:
                    {
                      fprintf (output, "; [CARLA DEBUG]: End of ID if statement %d\n", scope.label_id);
                      unsigned int root_id = scope.label_id;

                      int last = ifstatements_root.length - 1;
                      if( scope.type == Elif_scope )
                        {
                          IfStatement ifStatement = GETNP(IfStatement, ifstatements_root, last);
                          root_id = ifStatement.root;
                        }

                      llvm_br (output, root_id, "end_root");

                      PNode _else = GET(pTree, i + 1);

                      llvm_label_id (output, scope.label_id, "else");
                      var++;

                      if( scope.type == If_scope )
                        {
                          fprintf (output, "; [CARLA DEBUG]: Added to root vector %d\n", scope.label_id);
                          vector_push (&ifstatements_root, ((void*)&(IfStatement) { .root = scope.label_id }));
                          last++;
                        }

                      if( _else.type != NODE_ELSE )
                        {
                          IfStatement ifStatement = GETNP(IfStatement, ifstatements_root, last);
                          root_id = ifStatement.root;
                          vector_remove (&ifstatements_root, last);

                          llvm_br (output, root_id, "end_root");
                          llvm_label_id (output, root_id, "end_root");
                          var++;
                          break;
                        }

                      i++;

                      PNode _else_if = GET(pTree, i + 1);

                      if( _else_if.type == NODE_IF )
                        {
                          i++;

                          fprintf (output, "; [CARLA DEBUG]: Else if begin\n");
                          llvm_alloca_t (output, "bool");

                          cache[clen++] = (ExprCache) {
                            .type = ELIF_KEY,
                            .info = {
                              .var = {
                                .llvm = (var - 1),
                              }
                            }
                          };

                          jump_to_expr = 1;
                          break;
                        }


                      ScopeType stype = (ScopeType) {
                        .type = Else_scope,
                        .label_id = root_id
                      };
                      tab++;

                      vector_push (&scopes, ((void*)&stype));

                    } break;

                  case Else_scope:
                    {
                      fprintf (output, "; [CARLA DEBUG]: End of ID else statement %d\n", scope.label_id);

                      int last = ifstatements_root.length - 1;
                      IfStatement ifStatement = GETNP(IfStatement, ifstatements_root, last);
                      int root_id = ifStatement.root;

                      llvm_br (output, root_id, "end_root");
                      llvm_label_id (output, root_id, "end_root");
                      var++;

                      vector_remove (&ifstatements_root, last);

                    } break;

                  case For:
                    {
                      fprintf (output, "; [CARLA DEBUG]: End of ID iteration %d\n", scope.label_id);

                      llvm_alloca_t (output, "int64");
                      llvm_store_l (output, 64, 1, (var-1));
                      llvm_load_number (output, 64, (var-1));
                      unsigned int right_llvm = (var-1);

                      Variable left = llvm_get_f (&vars, scope.operation.left);
                      fprintf (output, "; [CARLA DEBUG]: Found variable: %s\n", left.id);
                      int bytes = llvm_sizeof (llvm_type (left.type));
                      llvm_load_number (output, bytes * 8, scope.operation.left);
                      if( bytes != 8 )
                        llvm_resize_bits (output, (var-1), "int64", left.type);

                      llvm_operation (output, scope.operation.operator, (var-1), right_llvm);

                      if( bytes != 8 )
                        llvm_resize_bits (output, (var-1), left.type, "int64");

                      llvm_store (output, (PNode) {
                        .data.definition.type = left.type
                      } , (var - 1), scope.operation.left);
                      llvm_br (output, scope.label_id, "for_begin");
                      llvm_label_id (output, scope.label_id, "for_end");
                      var++;

                      tab--;
                    } break;
                  case For_complex:
                    {

                      fprintf (output, "; [CARLA DEBUG]: End of ID iteration %d\n", scope.label_id);

                      char bits = llvm_sizeof (llvm_type (scope.complex.type)) * 8;

                      llvm_alloca_t (output, "int64");
                      llvm_store_l (output, 64, 1, (var-1));
                      llvm_load_number (output, 64, (var-1));
                      unsigned int one = (var-1);

                      llvm_load_number (output, bits, scope.complex.left);
                      if( bits != 64 )
                        llvm_resize_bits (output, (var-1), "int64", scope.complex.type);

                      unsigned int iter = (var-1);

                      llvm_icmpbr (output, scope.complex.comp, scope.label_id, "for_decrement", "for_increment");

                      llvm_label_id (output, scope.label_id, "for_decrement");
                      llvm_operation (output, "-", iter, one);

                      if ( bits != 64 )
                        llvm_resize_bits (output, (var-1), scope.complex.type, "int64");

                      llvm_store (output, (PNode) {
                        .data.definition.type = scope.complex.type
                      } , (var-1), scope.complex.left);

                      llvm_br (output, scope.label_id, "for_begin");

                      llvm_label_id (output, scope.label_id, "for_increment");
                      llvm_operation (output, "+", iter, one);

                      if ( bits != 64 )
                        llvm_resize_bits (output, (var-1), scope.complex.type, "int64");

                      llvm_store (output, (PNode) {
                        .data.definition.type = scope.complex.type
                      } , (var-1), scope.complex.left);

                      llvm_br (output, scope.label_id, "for_begin");
                      llvm_label_id (output, scope.label_id, "for_end");
                      var++;

                      tab--;
                    } break;
                  default: break;
                }

              vector_remove (&scopes, index);

              int last = (vars.length - 1);
              Variable __var;
              while( last > 0 && (__var = GETNP(Variable, vars, last)).v_type == Normal && __var.tab > tab )
                if( __var.v_type != StructField )
                  vector_remove (&vars, last--);

              free (tabs);

              if( jump_to_expr )
                goto wait_expr;
            } break;

          case NODE_RET:
            {

              char remove = 0;
              unsigned int j = i + 1;
              int extra_begin = 0;
              int counter = 0;
              PNode arg;
              while(1)
                {
                  arg = GET(pTree, j++);

                  if( arg.type == NODE_BEGIN )
                    extra_begin++;

                  if( arg.type == NODE_END )
                    {
                      if( extra_begin == 0 )
                        break;
                      
                      extra_begin--;
                    }
                  
                  if( arg.type == NODE_RET ) 
                    counter++;
                };

              unsigned int last = retStack.length - 1;
              remove = counter == 0;
              
              PNode node = {
                .data = {
                  .definition = {
                    .type = GETNP(RetStack, retStack, last).type
                  }
                }
              };

              if( remove && tab == 2)
                {
                  vector_remove(&retStack, last);
                  lambda_counter--;
                }

              cache[clen++] = (ExprCache) {
                .type = RETURN_KEY,
                .info = {
                  .node = node
                }
              };

              goto wait_expr;

            } break;

          case NODE_IF:
            { fprintf (output, "; [CARLA DEBUG]: If begin\n");
              llvm_alloca_t (output, "bool");

              cache[clen++] = (ExprCache) {
                .type = IF_KEY,
                .info = {
                  .var = {
                    .llvm = (var - 1),
                  }
                }
              };

              goto wait_expr;
            } break;

          case NODE_STRUCT:
            { fprintf (output, "; [CARLA DEBUG]: Struct %s\n", branch.data.value);

              in_struct = 1;
              struct_name = branch.data.value;
              vector_push (&structies, ((void*)&(Structies) {
                .carla_id = carla_id++,
                .struct_id = struct_name
              }));
            } break;

          case NODE_END_IMPLEMENT:
            { in_struct = 0;
              PNode our_or_fields = GET(pTree, i + 1);
              i++;

              switch(our_or_fields.type)
                { case NODE_OUR:
                    { PNode fields = GET(pTree, i + 1);
                      i++;
                      if( fields.type != NODE_BEGIN )
                        goto __llvm_gen_error__;

                      sprintf (struct_type, "\n%c.carla.struct.%d = type { ", '%', (carla_id-1));
                      int iteration = 0;
                      for(; iteration < bounded_vec.length; iteration++) 
                        {
                          BoundedField field = GETNP(BoundedField, bounded_vec, iteration);
                          char result = 0; 
                          VECTOR_CONTAINS (Variable, &vars, id, field.name, &result);

                          if( result )
                            continue;
                            
                          sprintf (struct_type, "%s%s%s", 
                                  struct_type,
                                  (struct_i == 0) ? "" : ", ",
                                  llvm_type (field.type)
                          );

                          vector_push (&vars, ((void*)&(Variable) {
                            .__struct = strdup (struct_name),
                            .tab      = -1,
                            .v_type   = StructField,
                            .struct_i = struct_i++,
                            .type     = field.type,
                            .id       = field.name
                          }));
                          
                        }
                      in_struct_fields = 1;
                    } break;
                  
                  case NODE_BEGIN:
                    { in_struct_fields = 1;
                      sprintf (struct_type, "\n%c.carla.struct.%d = type { ", '%', (carla_id-1));
                      int iteration = 0;
                      for(; iteration < bounded_vec.length; iteration++) 
                        {
                          BoundedField field = GETNP(BoundedField, bounded_vec, iteration);
                          char result = 0; 
                          VECTOR_CONTAINS (Variable, &vars, id, field.name, &result);

                          if( result )
                            continue;
                            
                          sprintf (struct_type, "%s%s%s", 
                                  struct_type,
                                  (struct_i == 0) ? "" : ", ",
                                  llvm_type (field.type)
                          );

                          vector_push (&vars, ((void*)&(Variable) {
                            .__struct = strdup (struct_name),
                            .tab      = -1,
                            .v_type   = StructField,
                            .struct_i = struct_i++,
                            .type     = field.type,
                            .id       = field.name
                          }));
                          
                        }
                      continue;
                    } break;

                  case NODE_EEXPR: {
                    struct_name = "";
                    continue;
                  }
                  default: goto __llvm_gen_error__;
                }


            } break;

          case NODE_FOR:
            { PNode definition = GET(pTree, i + 1);
              if( definition.type == NODE_DEFINITION && definition.data.definition.iter )
                {
                  fprintf (output, "; [CARLA DEBUG]: For iteration\n");

                  PNode iterator = GET(pTree, i + 2);
                  PNode open = GET(pTree, i + 3);
                  if(
                    (
                      (
                        iterator.type == NODE_OPERATION
                        && iterator.data.operation.operation.type == Iter
                      ) || (
                        iterator.type == NODE_SINGLE
                        && iterator.data.single.type == NODE_ID
                      )
                    )
                    && open.type == NODE_BEGIN
                  ) {
                      i += 3;

                      llvm_alloca (output, definition);
                      unsigned int iteration = (var-1);
                      vector_push (&vars, ((void*)&(Variable) {
                        .v_type = Normal,
                        .type   = definition.data.definition.type,
                        .llvm   = iteration,
                        .tab    = ++tab,
                        .id     = definition.data.definition.id
                      }));

                      switch(iterator.type)
                        { case NODE_OPERATION:
                            {

                              Token left = iterator.data.operation.left;
                              Token right = iterator.data.operation.right;

                              int left_isint = left.type == Integer;
                              int left_identifier = left.type == Identifier;
                              int right_isint = right.type == Integer;
                              int right_identifier = right.type == Identifier;

                              char bits = llvm_sizeof (llvm_type (definition.data.definition.type)) * 8;

                              if( left_isint && right_isint && types_int (definition.data.definition.type) )
                                {
                                  int left_v = atoi (left.buffer);
                                  int right_v = atoi (right.buffer);

                                  char *operator = (left_v < right_v) ? "<" : ">";

                                  llvm_alloca_t (output, definition.data.definition.type);
                                  llvm_store_l (output, bits, right_v, (var-1));
                                  llvm_load_number (output, bits, (var-1));
                                  unsigned int right_llvm = (var-1);

                                  llvm_store_l (output, bits, left_v, iteration);
                                  llvm_br (output, label_id, "for_begin");

                                  llvm_label (output, "for_begin");
                                  var++;

                                  llvm_load_number (output, bits, iteration);
                                  llvm_comp_bits (output, bits, operator, (var-1), right_llvm, 0);
                                  llvm_icmpbr (output, (var-1), (label_id - 1), "for_body", "for_end");

                                  llvm_label_id (output, (label_id - 1), "for_body");
                                  var++;

                                  ScopeType stype = (ScopeType) {
                                    .type = For,
                                    .label_id = (label_id - 1),
                                    .operation.left = iteration,
                                    .operation.operator = (left_v < right_v) ? "+" : "-"
                                  };

                                  vector_push (&scopes, ((void*)&stype));
                                }
                              else
                                {
                                  if( left_identifier )
                                    {
                                      Variable id = llvm_get (&vars, left.buffer);
                                      llvm_load (output, id);
                                      if( llvm_sizeof (llvm_type (id.type)) * 8 != bits )
                                        llvm_resize_bits (output, (var-1), definition.data.definition.type, id.type);
                                    }
                                  else if( left_isint )
                                    {
                                      int _left = atoi (left.buffer);
                                      llvm_alloca_t (output, definition.data.definition.type);
                                      llvm_store_l (output, bits, _left, (var-1));
                                      llvm_load_number (output, bits, (var-1));
                                    }

                                  unsigned int left_var = (var-1);

                                  if( right_identifier )
                                    {
                                      Variable id = llvm_get (&vars, right.buffer);
                                      llvm_load (output, id);
                                      if( llvm_sizeof (llvm_type (id.type)) * 8 != bits )
                                        llvm_resize_bits (output, (var-1), definition.data.definition.type, id.type);
                                    }
                                  else if( right_isint )
                                    {
                                      int _right = atoi (right.buffer);
                                      llvm_alloca_t (output, definition.data.definition.type);
                                      llvm_store_l (output, bits, _right, (var-1));
                                      llvm_load_number (output, bits, (var-1));
                                    }

                                  unsigned int right_var = (var-1);

                                  llvm_comp_bits (output, bits, ">", left_var, right_var, 0);
                                  unsigned int comp = (var-1);

                                  llvm_store (output, (PNode) {
                                    .data.definition.type = definition.data.definition.type
                                  }, left_var, iteration);

                                  llvm_br (output, label_id, "for_begin");
                                  llvm_label (output, "for_begin");
                                  var++;

                                  unsigned int begin = (label_id - 1);

                                  llvm_load_number (output, bits, iteration);
                                  unsigned int iter = (var-1);

                                  llvm_icmpbr (output, comp, begin, "for_check.grant", "for_check.less");

                                  /* left > right = LEFT IS GRANT THAN RIGHT*/
                                  llvm_label_id (output, begin, "for_check.grant");

                                  llvm_comp_bits (output, bits, ">", iter, right_var, 0);
                                  llvm_icmpbr (output, (var-1), begin, "for_body", "for_end");

                                  /* !(left > right) = LEFT IS LESS THAN RIGHT*/
                                  llvm_label_id (output, begin, "for_check.less");

                                  llvm_comp_bits (output, bits, "<", iter, right_var, 0);
                                  llvm_icmpbr (output, (var-1), begin, "for_body", "for_end");

                                  llvm_label_id (output, begin, "for_body");
                                  var++;

                                  ScopeType stype = (ScopeType) {
                                    .type = For_complex,
                                    .label_id = begin,
                                    .complex.left = iteration,
                                    .complex.comp = comp,
                                    .complex.type = definition.data.definition.type
                                  };

                                  vector_push (&scopes, ((void*)&stype));
                                }
                            } break;
                          default: break;
                        }
                    }
                }
            } break;

          case NODE_FIELD_ACCESS:
          case NODE_INTERNAL:
          case NODE_INTERNAL_SUPER:
          case NODE_INTERNAL_STRUCT:
            {
              i--;
              goto wait_expr;
            }
          default: break;
        }
    }
  
  goto __ignore_error;
  __llvm_gen_error__: {
    printf ("Fail to generate the LLVM code\n");
  }
  __ignore_error: {}


  int j = 0;
  for(; j < vars.length; j++ )
    {
      free (GETNP(Variable, vars, j).type);
      free (GETNP(Variable, vars, j).id);
    }
  vector_free (&vars);
  j = 0;
  fprintf (output, "\n");
  for(; j < complements.length; j++ )
    {
      const int t = GETNP(Complement, complements, j).type;
      const int string = 0;
      fprintf (output, "@.carla.static.%s.%d = ",
              (t == string) ? "str" : "unknown",
              j
      );

      switch (t)
        { case 0:
            {
              const char *content = GETNP(Complement, complements, j).string;
              fprintf (output, "private constant [%d x i8] c\"%s\\00\", align 1\n",
                      ((int)strlen (content) + 1),
                      content
              );
            } break;
          default: break;
        }

      free (GETNP(Complement, complements, j).string);
    }
  vector_free (&scopes);

}