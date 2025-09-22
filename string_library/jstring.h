#ifndef JSTRING_H
#define JSTRING_H

static void (*jstring_log)(const char*, ...);

/* NOTE: this assertion sets off the address sanitizer */
#define JSTRING_ASSERT(expression, msg) \
if(!(expression)) \
{ \
	jstring_log("JSTRING ASSERTION: %s. file: %s. line: %d", \
			msg, __FILE__, __LINE__); \
	char halt = *((char*)0);  \
}

/* NOTE: this is how you do a static assert without having to include C 
 * standard headers and stuff so this is how we doin' it 
 */
#define JSTRING_STATIC_ASSERT(expression, message) \
typedef char static_assertion_##message[(expression)?1:-1]

JSTRING_STATIC_ASSERT(sizeof(signed char) == 1, 
		asserting_signed_char_size_is_one_byte);
JSTRING_STATIC_ASSERT(sizeof(signed short) == 2, 
		asserting_signed_short_size_is_two_bytes);
JSTRING_STATIC_ASSERT(sizeof(signed int) == 4, 
		asserting_signed_int_size_is_four_bytes);
JSTRING_STATIC_ASSERT(sizeof(signed long long) == 8, 
	asserting_signed_long_long_size_is_eight_bytes);
JSTRING_STATIC_ASSERT(sizeof(unsigned char) == 1, 
		asserting_unsigned_char_size_is_one_byte);
JSTRING_STATIC_ASSERT(sizeof(unsigned short) == 2, 
		asserting_unsigned_short_size_is_two_bytes);
JSTRING_STATIC_ASSERT(sizeof(unsigned int) == 4, 
		asserting_unsigned_int_size_is_four_bytes);
JSTRING_STATIC_ASSERT(sizeof(unsigned long long) == 8, 
		asserting_unsigned_long_long_size_is_eight_bytes);
JSTRING_STATIC_ASSERT(sizeof(float) == 4, 
		asserting_float_size_is_four_bytes);
JSTRING_STATIC_ASSERT(sizeof(double) == 8, 
		asserting_double_size_is_eight_bytes);

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef unsigned char u8;
typedef unsigned char b8; /* for "boolean" stuff, this is more so I can 
						   * remember what stuff is is used for booleans
						   */
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define true 1
#define false 0 

static void jstring_log_stub(const char*, ...) { }
static void (*jstring_log)(const char*, ...) = jstring_log_stub;
    

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

typedef struct {
	u32 length;
	u32 capacity;
	char *data;
} jstring;

static jstring_memory jstring_temporary_memory_info = {0};

/* NOTE: basically user is just saying, "hey you get to use this memory, 
 * I will not touch it" 
 */
static b8 jstring_memory_activate(u64 size, void *address)
{
	if(jstring_temporary_memory_info.activated)
	{
		jstring_log("jstring_memory_activate: jstring memory has already" 
					" been activated! returning 0.");
		return false;
	}
	jstring_temporary_memory_info.size = size;
	jstring_temporary_memory_info.offset = 0;
	jstring_temporary_memory_info.address = address;
	jstring_temporary_memory_info.activated = true;

	jstring_log(
		"jstring_memory_activate: jstring memory activated "
		"- %u bytes @%p", 
		jstring_temporary_memory_info.size,
		jstring_temporary_memory_info.address);
	return true;
}

static b8 jstring_memory_reset(u64 size, void *address)
{
	if(jstring_temporary_memory_info.activated == false)
	{
		jstring_log(
			"jstring_memory_reset: jstring memory has not been"
			" activated. unable to reset memory. returning 0.");
		return false;
	}
	if(size == 0 && address == 0)
	{
		jstring_temporary_memory_info.offset = 0;
	}
	else
	{
		jstring_temporary_memory_info.size = size;
		jstring_temporary_memory_info.offset = 0;
		jstring_temporary_memory_info.address = address;
	}

	jstring_log(
		"jstring_memory_reset: jstring memory reset "
		" - %u bytes @%p",
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
	return true;
}

static jstring jstring_create_temporary(
		const char *chars, 
		u64 length)
{
	/* TODO: handle these assertion cases? */
	if(chars == 0)
	{
		JSTRING_ASSERT(0, " ");
	}
	if(jstring_temporary_memory_info.size < 
		jstring_temporary_memory_info.offset + ((length + 1) * 2))
	{
		JSTRING_ASSERT(0, " ");
	}
	if(!jstring_temporary_memory_info.activated)
	{
		JSTRING_ASSERT(0, " ");
	}
	jstring result_jstring;
	result_jstring.length = length;
	result_jstring.capacity = 2 * (length + 1);

	char *jstring_temporary_memory_insertion_address = 
		(char*) jstring_temporary_memory_info.address + 
		jstring_temporary_memory_info.offset;

	u32 index;
	for(index = 0; index < length; index++)
	{
		if(chars[index] == '\0')
		{
			JSTRING_ASSERT(0, "hit null terminator before expected");
		}
		jstring_temporary_memory_insertion_address[index] = chars[index];
	}
	if(chars[length] != '\0')
	{
		JSTRING_ASSERT(0, 
				"jstring_create_temporary: null terminator not found"
				" at end of passed string ('chars' param)\n"
				"expected chars[length] == '\0'");
	}
	jstring_temporary_memory_insertion_address[length] = '\0';

	result_jstring.data = jstring_temporary_memory_insertion_address;
	jstring_temporary_memory_info.offset += result_jstring.capacity;

	return result_jstring;
}

static b8 jstring_create_format_string(const char *chars, ...)
{
	/* TODO: signed numbers, unsigned numbers, strings should be the
	 * only format symbols you really need to recognize for now
	 * eventually floats 
	 */
	return true;
}

static i32 jstring_compare_raw(const char *first, const char *second)
{
	u32 index = 0;
	while((first[index] != '\0') && (second[index] != '\0'))
	{
		if(first[index] < second[index])
		{
			return -1;
		}
		if(first[index] > second[index])
		{
			return 1;
		}
		index++;
	}
	if(first[index] < second[index])
	{
		return -1;
	}
	if(first[index] > second[index])
	{
		return 1;
	}
	return 0;
}

static u32 jstring_length(char *string)
{
	char *tmp = string;
	u32 length = 0;
	while(*tmp != '\0')
	{
		tmp++;
		length++;
		JSTRING_ASSERT(length < 4096, "really long string");
	}
	return length;
}

static i32 jstring_compare_jstring(jstring first, jstring second)
{
	if(first.length < second.length)
	{
		return -1;
	}
	if(first.length > second.length)
	{
		return 1;
	}

	char *first_data = first.data;
	char *second_data = second.data;

	u32 index;
	for(index = 0; index < first.length; index++)
	{
		if(first_data[index] < second_data[index])
		{
			return -1;
		}
		if(first_data[index] > second_data[index])
		{
			return 1;
		}
		index++;
	}

	return 0;
}

static jstring jstring_create_substring_temporary(
		jstring string, u32 start, u32 end)
{
	if(jstring_temporary_memory_info.size < 
		jstring_temporary_memory_info.offset + ((end-start+2) * 2))
	{
		JSTRING_ASSERT(0, " ");
	}
	if(!jstring_temporary_memory_info.activated)
	{
		JSTRING_ASSERT(0, " ");
	}
	jstring result_jstring;
	result_jstring.length = start-end+1;
	result_jstring.capacity = 2 * (result_jstring.length + 1);
	char *tmp = jstring_temporary_memory_info.address +
		jstring_temporary_memory_info.offset;
	u32 index;
	for(index = 0; index <= end-start; index++)
	{
		tmp[index] = string.data[index + start];
	}
	result_jstring.data = tmp;
	jstring_temporary_memory_info.offset += result_jstring.capacity;

	return result_jstring;
}

static b8 jstring_concatenate_jstring(
		jstring *to, jstring from)
{
	if(!jstring_temporary_memory_info.activated)
	{
		JSTRING_ASSERT(0, " ");
	}
	if(to->length + from.length < to->capacity)
	{
		/* concatenate in place */
		char *tmp = to->data + to->length;
		u32 index;
		for(index = 0; index <= from.length; index++)
		{
			tmp[index] = from.data[index];
		}
		to->length += from.length;
	}
	else
	{
		/* concatenate by creating new jstring */
		if(jstring_temporary_memory_info.size < 
			jstring_temporary_memory_info.offset + 
			((to->length + from.length + 1) * 2))
		{
			JSTRING_ASSERT(0, " ");
		}
		char *tmp = jstring_temporary_memory_info.address +
			jstring_temporary_memory_info.offset;

		u32 index;
		for(index = 0; index < to->length; index++)
		{
			tmp[index] = to->data[index];
		}
		to->data = tmp;
		tmp += to->length;
		for(index = 0; index <= from.length; index++)
		{
			tmp[index] = from.data[index];
		}
		to->length += from.length;
		to->capacity = 2 * (to->length + 1);
		jstring_temporary_memory_info.offset += to->capacity;
	}
	return true;
}

static b8 jstring_concatenate_raw(
		jstring *to, char *from)
{
	u32 from_length = jstring_length(from);

	if(!jstring_temporary_memory_info.activated)
	{
		JSTRING_ASSERT(0, " ");
	}
	if(to->length + from_length < to->capacity)
	{
		/* concatenate in place */
		char *tmp = to->data + to->length;
		u32 index;
		for(index = 0; index <= from_length; index++)
		{
			tmp[index] = from[index];
		}
		to->length += from_length;
	}
	else
	{
		/* concatenate by creating new jstring */
		if(jstring_temporary_memory_info.size < 
			jstring_temporary_memory_info.offset + 
			((to->length + from_length + 1) * 2))
		{
			JSTRING_ASSERT(0, " ");
		}
		char *tmp = jstring_temporary_memory_info.address +
			jstring_temporary_memory_info.offset;

		u32 index;
		for(index = 0; index < to->length; index++)
		{
			tmp[index] = to->data[index];
		}
		to->data = tmp;
		tmp += to->length;
		for(index = 0; index <= from_length; index++)
		{
			tmp[index] = from[index];
		}
		to->length += from_length;
		to->capacity = 2 * (to->length + 1);
		jstring_temporary_memory_info.offset += to->capacity;
	}
	return true;
}

static b8 jstring_begins_with(jstring string, char *chars)
{
	u32 chars_length = jstring_length(chars);
	if(chars_length > string.length)
	{
		return false;
	}
	u32 index = 0;
	while(chars[index] == string.data[index])
	{
		index++;
		if(index == chars_length)
		{
			return true;
		}
	}

	return false;
}

static b8 jstring_ends_with(jstring string, char *chars)
{
	u32 chars_length = jstring_length(chars);
	if(chars_length > string.length)
	{
		return false;
	}
	u32 index = 0;
	while(chars[index] == 
			string.data[string.length - chars_length + index])
	{
		index++;
		if(index == chars_length)
		{
			return true;
		}
	}

	return false;
}

static jstring jstring_create_integer(u32 number)
{
	u32 digits = 1;
	u32 divisor = 10;
	u32 index;
	char *tmp = jstring_temporary_memory_info.address +
		jstring_temporary_memory_info.offset;
	jstring return_string;
	while(number / divisor != 0)
	{
		digits++;
		divisor *= 10;
	}
	divisor /= 10;
	for(index = 0; index < digits; index++)
	{
		tmp[index] = (number / divisor) + 48;
		number %= divisor;
		divisor /= 10;
	}
	tmp[digits] = '\0';
	return_string.data = tmp;
	return_string.length = digits;
	return_string.capacity = 2 * (return_string.length + 1);
	jstring_temporary_memory_info.offset += return_string.capacity;
	return return_string;
}

static i32 jstring_index_of_raw(jstring string, char *chars)
{
	u32 chars_length = jstring_length(chars);
	u32 index;
	for(index = 0; index <= string.length - chars_length; index++)
	{
		u32 inner_index;
		for(inner_index = 0; inner_index < chars_length; inner_index++)
		{
			if(string.data[inner_index + index] != chars[inner_index])
			{
				break;
			}
			if(inner_index == chars_length - 1)
			{
				return index;
			}
		}
	}
	/* NOTE: return -1 if chars not found in string */
	return -1;
}

/* TODO: you really need better parameter names bro */
static i32 jstring_index_of_jstring(jstring string, jstring check)
{
	u32 index;
	for(index = 0; index <= string.length - check.length; index++)
	{
		u32 inner_index;
		for(inner_index = 0; inner_index < check.length; inner_index++)
		{
			if(string.data[inner_index + index] != 
					check.data[inner_index])
			{
				break;
			}
			if(inner_index == check.length - 1)
			{
				return index;
			}
		}
	}
	/* NOTE: return -1 if check not found in string */
	return -1;
}

static void jstring_to_upper_in_place(jstring *string)
{
	u32 index;
	for(index = 0; index < string->length; index++)
	{
		if((string->data[index] < 123) && (string->data[index] > 96))
		{
			string->data[index] = string->data[index] - 32;
		}
	}
}

static void jstring_to_lower_in_place(jstring *string)
{
	u32 index;
	for(index = 0; index < string->length; index++)
	{
		if((string->data[index] < 91) && (string->data[index] > 64))
		{
			string->data[index] = string->data[index] + 32;
		}
	}
}

static jstring jstring_to_upper_jstring(jstring string)
{
	u32 index;
	jstring return_jstring;
	char *tmp = jstring_temporary_memory_info.address +
		jstring_temporary_memory_info.offset;

	return_jstring.length = string.length;
	return_jstring.capacity = (return_jstring.length + 1) * 2;

	for(index = 0; index <= string.length; index++)
	{
		if((string.data[index] < 123) && (string.data[index] > 96))
		{
			tmp[index] = string.data[index] - 32;
		}
		else
		{
			tmp[index] = string.data[index];
		}
	}

	jstring_temporary_memory_info.offset += return_jstring.capacity;

	return return_jstring;
}

static jstring jstring_to_lower_jstring(jstring string)
{
	u32 index;
	jstring return_jstring;
	char *tmp = jstring_temporary_memory_info.address +
		jstring_temporary_memory_info.offset;

	return_jstring.length = string.length;
	return_jstring.capacity = (return_jstring.length + 1) * 2;

	for(index = 0; index <= string.length; index++)
	{
		if((string.data[index] < 91) && (string.data[index] > 64))
		{
			tmp[index] = string.data[index] + 32;
		}
		else
		{
			tmp[index] = string.data[index];
		}
	}

	jstring_temporary_memory_info.offset += return_jstring.capacity;

	return return_jstring;
}

/* NOTE: convenience function for insert/remove/replace/trim 
 * type functions. really shouldn't be used by "users" of the jstring
 * library, but anything goes lol I want "them" (me) to have full control
 * if needed. I mean users have direct access to the jstring memory since 
 * they allocate it themselves. dangerous but useful. they could write
 * this function themselves
 */
static b8 copy_temporary_memory_chars(
		char *address, 
		u64 to_offset,
		u32 num_chars)
{
	/* don't let it move chars past jstring memory, basically */
	if((address < (char*)jstring_temporary_memory_info.address) ||
		(address > ((char*)jstring_temporary_memory_info.address +
			jstring_temporary_memory_info.size -
			to_offset - num_chars)))
	{
		JSTRING_ASSERT(0, " ");
		return false;
	}

	i32 index;
	/* copy it backwards so that you don't overwrite the data
	 * that you're copying. hope that makes sense
	 */
	for(index = num_chars - 1; index >= 0; index--)
	{
		(address + to_offset)[index] = address[index];
	}
	
	return true;
}

static b8 jstring_insert_chars_at(jstring *to, char *chars, u32 index)
{
	if(index > to->length)
	{
		/* NOTE: this might technically be caught by the next if statement
		 * ? perchance
		 */
		JSTRING_ASSERT(0, " ");
		return false;
	}
	u32 chars_length = jstring_length(chars);

	/* don't need to create a whole new jstring */
	if((to->capacity - to->length) > chars_length)
	{
		copy_temporary_memory_chars(
			to->data + index, chars_length, to->length - index + 1); 
		u32 loop_index;
		for(loop_index = 0; loop_index < chars_length; loop_index++)
		{
			to->data[index + loop_index] = chars[loop_index];
		}
	}
	/* need to create a whole new jstring */
	else
	{
		char *tmp = jstring_temporary_memory_info.address +
			jstring_temporary_memory_info.offset;
		u32 new_length = to->length + chars_length;

		if(jstring_temporary_memory_info.offset + ((new_length + 1) * 2) >
				jstring_temporary_memory_info.size)
		{
			JSTRING_ASSERT(0, " ");
			return false;
		}
		
		u32 loop_index;
		for(loop_index = 0; loop_index < index; loop_index++)
		{
			tmp[loop_index] = to->data[loop_index];
		}
		for(loop_index = 0; loop_index < chars_length; loop_index++)
		{
			tmp[index + loop_index] = chars[loop_index];
		}
		for(
			loop_index = 0; 
			loop_index < (to->length - index + 1);
			loop_index++)
		{
			tmp[index + chars_length + loop_index] = 
				to->data[index + loop_index];
		}

		to->length = new_length;
		to->capacity = (to->length + 1) * 2;
		to->data = tmp;

		jstring_temporary_memory_info.offset += to->capacity;
	}

	return true;
}

static b8 jstring_insert_jstring_at(jstring *to, jstring from, u32 index)
{
	if(index > to->length)
	{
		/* NOTE: this might technically be caught by the next if statement
		 * ? perchance
		 */
		JSTRING_ASSERT(0, " ");
		return false;
	}

	/* don't need to create a whole new jstring */
	if((to->capacity - to->length) > from.length)
	{
		copy_temporary_memory_chars(
			to->data + index, from.length, to->length - index + 1); 
		u32 loop_index;
		for(loop_index = 0; loop_index < from.length; loop_index++)
		{
			to->data[index + loop_index] = from.data[loop_index];
		}
	}
	/* need to create a whole new jstring */
	else
	{
		char *tmp = jstring_temporary_memory_info.address +
			jstring_temporary_memory_info.offset;
		u32 new_length = to->length + from.length;

		if(jstring_temporary_memory_info.offset + ((new_length + 1) * 2) >
				jstring_temporary_memory_info.size)
		{
			JSTRING_ASSERT(0, " ");
			return false;
		}
		
		u32 loop_index;
		for(loop_index = 0; loop_index < index; loop_index++)
		{
			tmp[loop_index] = to->data[loop_index];
		}
		for(loop_index = 0; loop_index < from.length; loop_index++)
		{
			tmp[index + loop_index] = from.data[loop_index];
		}
		for(
			loop_index = 0; 
			loop_index < (to->length - index + 1);
			loop_index++)
		{
			tmp[index + from.length + loop_index] = 
				to->data[index + loop_index];
		}

		to->length = new_length;
		to->capacity = (to->length + 1) * 2;
		to->data = tmp;

		jstring_temporary_memory_info.offset += to->capacity;
	}

	return true;
}

#endif
