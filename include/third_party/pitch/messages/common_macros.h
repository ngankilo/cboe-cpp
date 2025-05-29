#ifndef _LIBPITCH_PITCH_COMMON_MACROS_H
#define _LIBPITCH_PITCH_COMMON_MACROS_H

/**
 * this macro is used throughout decode_message functions, 
 * which all take range of iterators, and generates begin and end iteratoes
 * based on offsets. It's an internal implementation detail and it's not meant to be
 * called outside libpitch functions
 */
#define _OFFSET_PAIR(O, L) (begin + O), (begin + O + L)
#endif