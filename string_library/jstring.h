#ifndef JSTRING_H
#define JSTRING_H

static void (*jstring_log)(const char*, ...);

/* TODO: this assertion sets off the address sanitizer, so I'll have to find a way
 * around that in order to actually see the assertion message
 */
#define JSTRING_ASSERT(expression, msg) \
if(!expression) \
{ \
	jstring_log("JSTRING ASSERTION: %s", msg); \
	char halt = *((char*)0); \
}


/* NOTE: this is how you do a static assert without having to include C standard headers and
 * stuff so this is how we doin' it 
 */
#define JSTRING_STATIC_ASSERT(expression, message) \
typedef char static_assertion_##message[(expression)?1:-1]

JSTRING_STATIC_ASSERT(sizeof(signed char) == 1, asserting_signed_char_size_is_one_byte);
JSTRING_STATIC_ASSERT(sizeof(signed short) == 2, asserting_signed_short_size_is_two_bytes);
JSTRING_STATIC_ASSERT(sizeof(signed int) == 4, asserting_signed_int_size_is_four_bytes);
JSTRING_STATIC_ASSERT(sizeof(signed long long) == 8, 
	asserting_signed_long_long_size_is_eight_bytes);
JSTRING_STATIC_ASSERT(sizeof(unsigned char) == 1, asserting_unsigned_char_size_is_one_byte);
JSTRING_STATIC_ASSERT(sizeof(unsigned short) == 2, asserting_unsigned_short_size_is_two_bytes);
JSTRING_STATIC_ASSERT(sizeof(unsigned int) == 4, asserting_unsigned_int_size_is_four_bytes);
JSTRING_STATIC_ASSERT(sizeof(unsigned long long) == 8, 
		asserting_unsigned_long_long_size_is_eight_bytes);
JSTRING_STATIC_ASSERT(sizeof(float) == 4, asserting_float_size_is_four_bytes);
JSTRING_STATIC_ASSERT(sizeof(double) == 8, asserting_double_size_is_eight_bytes);

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef unsigned char u8;
typedef unsigned char b8; /* for "boolean" stuff, this is more so I can remember what stuff is
						   * is used for booleans
						   */
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define true 1
#define false 0 

/* NOTE: if I want to do logging, I had an idea for that:
 * HAVE THE USER PASS function pointers to a logging function that they write?
 * as long as the function they write takes in a format string and variadic args it
 * works? then they can kinda take it from there as far as how they want to print the messages
 * or better yet just turn logging off!
 */
static void jstring_log_stub(const char*, ...) { }
static void (*jstring_log)(const char*, ...) = jstring_log_stub;
    
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

typedef struct {
	u64 size;

	/* current offset past address that we have allocated to */
	u64 offset; 
	void *address;
	b8 activated;
} jstring_memory;

/* NOTE: could have like a free-list type thing, where if strings get "freed" we flag that
 * so that we can fill those empty "blocks" later...
 * simpler would be a linear allocator ? 
 * maybe there's 2 types of strings - permanent, and temporary
 * we have a function that wipes temporary? and temporary is a bump allocator
 * permanent also a bump allocator that gets freed at the end... ?
 * that seems like it covers most use cases as long as there's enough storage for "permanent" 
 * strings. Note also the way the library is setup, permanent strings should be able to be
 * stored in place -> WAIT PROBLEM THO, if we add to strings, concatenate, insert etc... how
 * do we account for that?
 * okay, this is an interesting problem space...
 * maybe we do need like a first-fit free list (or red-black tree for efficiency <- more likely)
 * that allocates double the string size to leave room and then like moves strings if it has to?
 * - I just feel like the more can be done to strings IN PLACE the better
 */ 

/* NOTE: for now I'm going to do just a temporary store that we can clear periodically
 * or whatever to get the functions up and running because I feel like writing code that 
 * does stuff rather than thinking a whole bunch. And I feel like the string processing
 * problem space needs to be fleshed out more in my head before I can make decisions about
 * how to manage the memory around it... does that make sense? 
 */

typedef struct {
	u32 length;
	char *data;
} jstring;

static jstring_memory jstring_temporary_memory_info = {0};

/* NOTE: basically user is just saying, "hey you get to use this memory, I will not touch it" */
static b8 jstring_memory_activate(u64 size, void *address)
{
	if(jstring_temporary_memory_info.activated)
	{
		jstring_log("jstring_memory_activate: jstring memory has already been activated! "
					"returning 0.");
		return false;
	}
	jstring_temporary_memory_info.size = size;
	jstring_temporary_memory_info.offset = 0;
	jstring_temporary_memory_info.address = address;
	jstring_temporary_memory_info.activated = true;

	jstring_log(
		"jstring_memory_activate: jstring memory activated - %u bytes @%p", 
		jstring_temporary_memory_info.size,
		jstring_temporary_memory_info.address);
	return true;
}

static b8 jstring_load_logging_function(void (*func)(const char*, ...))
{
	if(func == 0)
	{
		/* TODO: is there any way to log this failure lol */
		/* probably just document that this is the only failure case */
		return false;
	}
	jstring_log = func;
	/* NOTE: I was gonna have it return false if we already have loaded a logging function
	 * but honestly who cares, load new logging functions mid program i don't care lol
	 */
	return true;
}

/* TODO: ITERATE! start with smn stupid and explore the problem space */
/* NOTE: might be important to have versions of functions that can also just pass a char *
 * so that they don't have to use our string type
 */ 

#endif
