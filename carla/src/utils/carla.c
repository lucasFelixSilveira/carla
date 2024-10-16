#include <stdio.h>
#include <string.h>

/**
 * @param __file__ receive a .cl(or not) file name
*/
int
isCarla(char *__file__)
{
  char sufix[3] = { 'l', 'c', '.' }; // .CL
  size_t length = strlen (__file__);
  int i = 0;
  for(; i < sizeof (sufix); i++) 
    {
      if( sufix[i] != __file__[length - (i + 1)] )
        {
          printf ("\"%s\" is not a Carla file.", __file__);
          return 0;
        }
    }
  return 1;
}