#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#if _WIN32
# include <direct.h>
# include <io.h>
# include <windows.h>
# define CHMOD _chmod
# define MKDIR _mkdir
# define FILE_MODE _S_IWRITE | _S_IREAD
# define PATH_SEPARATOR "\\"
# define SUFIX ".exe"
#else
# include <unistd.h>
# include <sys/types.h>
# include <utime.h>
# define CHMOD chmod
# define MKDIR mkdir
# define PATH_SEPARATOR "/"
# define SUFIX ""
#endif

int
isCarla(char *__file__)
{
  // char sufix[6] = { 'a', 'l', 'r', 'a', 'c', '.' }; // .CARLA
  char sufix[3] = { 'l', 'c', '.' }; // .cl
  size_t length = strlen (__file__);
  int i = 0;
  for(; i < sizeof (sufix); i++ ) 
    {
      if( sufix[i] != __file__[length - (i + 1)] )
        {
          printf ("\"%s\" is not a Carla file.", __file__);
          return 0;
        }
    }
  return 1;
}

void 
carla_project(char **output_file) 
{
  char cwd[1024];
  getcwd (cwd, sizeof (cwd));
  char *dir_name = (char*)malloc (1024 + 32);

  // Target folder to save files
  sprintf (dir_name, "%s%s%s", cwd, PATH_SEPARATOR, "target");
  MKDIR (dir_name);

  // Output folder to .exe and LLVM file
  sprintf (dir_name, "%s%s%s%s%s", cwd, PATH_SEPARATOR, "target", PATH_SEPARATOR, "out");
  MKDIR (dir_name);

  // Output LLVM file directory
  sprintf (dir_name, "%s%s%s%s%s%s%s", cwd, PATH_SEPARATOR, "target", PATH_SEPARATOR, "out", PATH_SEPARATOR, "ir.ll");
  memcpy (*output_file, dir_name, strlen (dir_name));

  // Output folder to .exe and llvm file
  sprintf (dir_name, "%s%s%s%s%s", cwd, PATH_SEPARATOR, "target", PATH_SEPARATOR, "logs");
  MKDIR (dir_name);

  free (dir_name);
}