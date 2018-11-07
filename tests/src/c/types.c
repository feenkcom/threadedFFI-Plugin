/**  Library with functions to test
***    - marshalling of parameters
***    - marshalling of return types
***    - sizeof() of types
***  This library provides functions for each of the basic C types
**/

#include "testLibrary.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

// Macros to define common identity id_type() functions to test arguments and return types
#define id(TYPE) id_with_name(TYPE, TYPE)
#define id_with_name(TYPE, NAME) TYPE id_##NAME(TYPE a){ return a; }

// Macros to define common sum_type() functions to test arguments and return types
#define sum(TYPE) sum_with_name(TYPE, TYPE)
#define sum_with_name(TYPE, NAME) TYPE sum_##NAME(TYPE a, TYPE b){ return a + b; }

// Macros to define common sizeof_type() functions
#define getsizeof(TYPE) sizeof_with_name(TYPE, TYPE)
#define sizeof_with_name(TYPE, NAME) size_t sizeof_##NAME(){ return sizeof(TYPE); }

// Macros to define common functions
#define test_functions(TYPE) test_functions_with_name(TYPE, TYPE)
#define test_functions_with_name(TYPE, NAME) id_with_name(TYPE, NAME) \
sizeof_with_name(TYPE, NAME) \
sum_with_name(TYPE, NAME)

// Floating point numbers
test_functions(float)
test_functions(double)
  
// Characters
test_functions(char)
test_functions_with_name(unsigned char, uchar)

// Signed integers
test_functions(int)
test_functions(int8_t)
test_functions(int16_t)
test_functions(int32_t)
test_functions(int64_t)

// Unsigned integers
test_functions_with_name(unsigned int, uint)
test_functions(uint8_t)
test_functions(uint16_t)
test_functions(uint32_t)
test_functions(uint64_t)
  
// Shorts
test_functions(short)
test_functions_with_name(unsigned short, ushort)

// Longs
test_functions(long)
test_functions_with_name(unsigned long, ulong)
test_functions_with_name(long long, longlong)
test_functions_with_name(unsigned long long, ulonglong)

// size_t
test_functions(size_t)

// Pointers
  
id_with_name(void*, pointer)

// Receive a pointer as argument
// Dereference its value and return it
void *unref_pointer(void **pointer) {
  return *pointer;
}

size_t sizeof_pointer(){ 
  return sizeof(void*);
}

// Strings

// Duplicates a string and returns a pointer to it
// It's the caller's responsability to free it
char* dup_string(char* aString){ 
  int len = strlen(aString);
  char *dst = malloc(len + 1);
  strcpy(dst, aString);
  return dst;
}