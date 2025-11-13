#ifndef BASE_H
#define BASE_H

#include <stdlib.h>

#define MEOW() printf("MEOW\n")

#define global   static
#define local    static
#define function static

#define TYPED_CALLOC(NMEMB, TYPE) \
    (TYPE *) calloc((NMEMB), sizeof(TYPE));

#define VERIFIED(code) \
    || ({code; false;})
//
// #define VERIFIED(code, retval) \
//     || ({code; return (retval); false;})

#define VERIFY(cond, code) \
    if (!(cond)) {code};

#define DIE() \
    *(int *) 0;

#define POSASERTED() \
    VERIFIED(ERROR_MSG("possaserted killed program, the action was completed unsuccessfully"); DIE();)

#define POSASSERT(cond) \
    VERIFY(cond, ERROR_MSG("possasert killed program, the cond (" #cond ") was false"); DIE();)

#define FREE(ptr)   \
    free((ptr));      \
    (ptr) = nullptr;

#endif // BASE_H