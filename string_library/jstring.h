#ifndef JSTRING_H
#define JSTRING_H

#define SIZEOF_SIGNED_CHAR 1 
#define SIZEOF_UNSIGNED_CHAR 1 
#define SIZEOF_SIGNED_SHORT 2
#define SIZEOF_UNSIGNED_SHORT 2
#define SIZEOF_SIGNED_INT 4 
#define SIZEOF_UNSIGNED_INT 4 
#define SIZEOF_SIGNED_LONG_LONG 8 
#define SIZEOF_UNSIGNED_LONG_LONG 8 
    
/* NOTE: goal is to have a header only string library that can compile
 * to c89 with 0 dependencies (including C standard library)
 */

/* NOTE: list of string functions to implement:
 * - compare two strings
 *   - equals convenience function as well (i.e. are the two strings
 *   equal).
 * - concatenate strings
 * - copy (can copy substring of one string to substring of other string)
 * - begins with (substring or char)
 * - ends with (substring or char)
 * - begins with (substring or char)
 * - format (create a string from a given format string? this seems
 *   complicated, but is obv very useful. actually you know what, you
 *   can do it bro wtheck ofc u can) 
 * - index of: get index of a char or substring
 * - last index of
 * - insert: insert char or substring at an index
 * - is_empty: simple convenience function I guess? is this useful at
 *   all?
 * - join: concatenates strings or list of strings with a given
 *   "separator" in between each string (i.e. a list with ", " between
 *   each string or smn could be a use case)
 * - remove: remove instances of char or substring or characters at a 
 *   given index -> a few possible versions of this one, make them all
 * - replace: replace chars at a given index or replace instances of
 *   char or substring with char or substring
 * - split: like strtok but actually good basically
 * - to upper/to lower
 * - trim: remove whitespace from beginning or end, or like all whitespace
 *   idk that could be usefu ltoo
 * 
 * so there ya go! good luck :) - josh
 *
 */

/* NOTE: have a big memory arena that all the strings are stored in
 * so that no dynamic allocation occurs... the user program gives this
 * memory tho so that this library doesn't need to worry about how 
 * allocation is done, it just knows how much memory it has and doesn't
 * use more than that PERIOD
 */

/* TODO: need some construct for what a string is */
/* TODO: ITERATE! start with smn stupid and explore the problem space */

#endif
