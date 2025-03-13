#include <stdio.h>
#include <stdlib.h>
#include "../utils/vector.h"
#include "../utils/carla.h"
#define GET(type, vector, index) ((type*)vector->items)[index]
#if _WIN32
# define PATH_SEPARATOR '\\'
#include <windows.h>
void remove_file(char *path);
char* 
getAbsolute() 
{
  char *path = (char*)malloc(1024);
  DWORD result = GetModuleFileName(NULL, path, MAX_PATH);
  remove_file(path);
  return path;
}
#else
# define PATH_SEPARATOR '/'
#include <unistd.h>
void remove_file(char *path);
char* 
getAbsolute() 
{
  char *path = (char*)malloc(1024);
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path)-1);
  path[len] = '\0'; 
  remove_file(path);
  return path;
}
#endif

void 
remove_file(char *path) 
{
  char *last_slash = strrchr(path, '/'); 
  
  if (last_slash == NULL) {
    last_slash = strrchr(path, '\\'); 
  }

  if (last_slash != NULL) {
    *last_slash = '\0';
  }
}

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
      

      char *origin_path = (char*)malloc (1024);
      if( library.is )
        {
          sprintf (origin_path, "%s%clib%c%s%c%s.ll", 
                  getAbsolute(),
                  PATH_SEPARATOR,
                  PATH_SEPARATOR,
                  library.lib,
                  PATH_SEPARATOR,
                  library.fn
          );
        }
      else
        {
          sprintf (origin_path, "%s%clib%c%s%cinit.ll", 
                  getAbsolute(),
                  PATH_SEPARATOR,
                  PATH_SEPARATOR,
                  library.lib,
                  PATH_SEPARATOR
          );
        }

      FILE *origin = fopen (origin_path, "r");

      char c;
      while((c = fgetc(origin)) != EOF)
        fprintf(llvm, "%c", c);

      if(! library.is )
        fprintf (llvm, "\n; %s Initialized", library.lib);
      fprintf (llvm, "\n");
    }
}