#include <stdio.h>
#include <stdlib.h>
#include "../utils/vector.h"
#include "../utils/carla.h"
#define GET(type, vector, index) ((type*)vector->items)[index]
#if _WIN32
# define PATH_SEPARATOR '\\'
#else
# define PATH_SEPARATOR '/'
#endif

void
putin(FILE *llvm, Vector *libs, char exec_dir[])
{
  char *exec_path = exec_dir;
  int i = strlen (exec_dir) - 1;

  while (i >= 0 && exec_path[i] != PATH_SEPARATOR)
    i--;
  if (i >= 0) 
    exec_path[i] = '\0';

  fprintf (llvm, "\n; Extern functions\n");

  size_t j = 0;
  for(; j < libs->length; j++ )
    {
      Library library = GET(Library, libs, j);
      if(! library.is )
        continue;
        
      char *origin_path = (char*)malloc (1024);
      sprintf (origin_path, "%s%clib%c%s%c%s.ll", 
              exec_path,
              PATH_SEPARATOR,
              PATH_SEPARATOR,
              library.lib,
              PATH_SEPARATOR,
              library.fn
      );
      FILE *origin = fopen (origin_path, "r");

      char c;
      while((c = fgetc(origin)) != EOF)
        fprintf(llvm, "%c", c);

      fprintf (llvm, "\n");
    }
}