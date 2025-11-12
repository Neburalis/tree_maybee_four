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
    if (!(cond)) {code};

#define die() \
    *(int *) 0;

#define posasserted() \
    verified(ERROR_MSG("possaserted killed program, the action was completed unsuccessfully"); die();)

#define posasert(cond) \
    verify(cond, ERROR_MSG("possasert killed program, the cond (" #cond ") was false"); die();)

#define FREE(ptr)   \
    free((ptr));      \
    (ptr) = nullptr;

#endif // BASE_H