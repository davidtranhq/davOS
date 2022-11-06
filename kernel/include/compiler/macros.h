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

#endif