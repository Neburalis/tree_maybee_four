#ifndef BASE_H
#define BASE_H

#include <stdlib.h>

#define MEOW() printf("MEOW\n")

#define global   static
#define local    static
#define function static

#define typed_calloc(NMEMB, TYPE) \
    (TYPE *) calloc((NMEMB), sizeof(TYPE));

#define verified(code) \
    || ({code; false;})

#define verify(cond, code) \
    if (!(cond)) (code);

#define die() \
    *(int *) 0;

#define posasserted() \
    verified(die())

#define posasert(cond) \
    verify(cond, die())

#endif // BASE_H