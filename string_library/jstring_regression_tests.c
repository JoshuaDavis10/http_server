b32 jstring_create_temporary_test()
{
	const char *tmp = "test string";
	jstring test_string = jstring_create_temporary(tmp, 11);

	u32 index;
	for(index = 0; index < 11; index++)
	{
		if(tmp[index] != 
				((char*)jstring_temporary_memory_info.address)[index])
		{
			printf("\e[1;31mcreated string: %s didn't match expected:"
					" %s\e[0;37m\n", test_string.data, tmp);
			return false;
		}
	}

	printf("\e[1;33mNOTE: \e[0;37m"
			"expecting next log to be a jstring error...\n");

	test_string = jstring_create_temporary(tmp, 15);
	if(test_string.data != 0 ||
			test_string.length != 0 ||
			test_string.capacity != 0)
	{
		printf("\e[1;31mexpected all jstring struct fields to be 0, but"
				" got: data: %p, length: %u, capacity: %u\e[0;37m\n",
				test_string.data, 
				test_string.length, 
				test_string.capacity);
		return false;
	}

	printf("\e[1;33mNOTE: \e[0;37m"
			"expecting next log to be jstring memory reset...\n");
	jstring_memory_reset(
			jstring_temporary_memory_info.size,
			jstring_temporary_memory_info.address);
	return true;
}

/* NOTE: don't write all these regression tests because it's a lot of
 * work for an API that's definitely going to change, but I have a good
 * bit of scaffolding set up for when we do want to add regression tests
 *
 * since we're currently in exploratory phase of jstring API, I'm 
 * not worried about rigorous testing :)
 * - josh (sep 24, 2025)
 */ 

/* TODO:
 * tests for:
 *
 * - jstring_length
 * - compare functions
 *		- jstring_compare_raw
 * 		- jstring_compare_jstring
 * 		- jstring_compare_jstring_and_raw
 *		- jstring_equals_raw
 * 		- jstring_equals_jstring
 * 		- jstring_equals_jstring_and_raw
 * - jstring_create_substring_temporary
 * - jstring_concatenate_jstring
 * - jstring_concatenate_raw
 * - jstring_begins_with
 * - jstring_ends_with
 * - jstring_create_integer
 * - index functions
 *		- jstring_index_of_raw
 * 		- jstring_index_of_jstring
 * 		- jstring_last_index_of_raw
 * 		- jstring_last_index_of_jstring
 * - to upper/lower
 *		- jstring_to_upper_in_place
 *		- jstring_to_lower_in_place
 *		- jstring_to_upper_jstring
 *		- jstring_to_lower_jstring
 * - jstring_insert_chars_at
 * - jstring_insert_jstring_at
 * - remove functions
 *		- jstring_remove_at
 *		- jstring_remove_chars
 *		- jstring_remove_chars_all
 *		- jstring_remove_jstring
 *		- jstring_remove_jstring_all
 * - jstring_replace_at_raw
 * - jstring_replace_at_jstring
 * - jstring_copy_to_buffer
 * - jstring_copy_to_jstring
 */
