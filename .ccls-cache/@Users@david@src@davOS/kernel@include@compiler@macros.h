/**
 * Useful compiler macros
 */

#ifndef DAVOS_COMPILER_MACROS_H_INCLUDED
#define DAVOS_COMPILER_MACROS_H_INCLUDED

#ifdef __cplusplus
    #define BEGIN_CDECLS extern "C" {
    #define END_CDECLS }
#else
    #define BEGIN_CDECLS
    #define END_CDECLS
#endif

#define IS_32_BIT INTPTR_MAX == INT32_MAX
#define IS_64_BIT INTPTR_MAX == INT64_MAX

#endif