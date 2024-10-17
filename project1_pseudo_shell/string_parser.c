/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/
	// Check for NULL string
	if (buf == NULL){ 
		return 0;
	}

	char* token;
	char* saveptr;


	token = strtok_r(buf, delim, &saveptr);
	int count;
	for(count = 0; token != NULL; count++){
		token = strtok_r(NULL, delim, &saveptr);
	}

    return count;
}


command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1 create command_line variable to be filled and returned
	*	#2. count the number of tokens with count_token function, set num_token. 
        *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
        *       #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/

	// #1.
	// create command_line variable to be filled and returned
	command_line cmd;

	// init values	
	char *temp_buf = strdup(buf);
	char *token;
	char *saveptr = NULL;

	// #2.
	// remove the \n at the end of the line, count the number of tokens
	cmd.num_token = count_token(buf, delim);
	strtok_r(temp_buf, "\n", &saveptr);

	// #3.
	// malloc memory for token array inside command_line variable
	cmd.command_list = malloc(sizeof(char*) * (cmd.num_token+1));


	// 4.use function strtok_r to find out the tokens 
	token = strtok_r(temp_buf, delim, &saveptr);

    // 5. malloc each index of the array with the length of tokens,
	// fill command_list array with tokens, and fill last spot with NULL.

	int i;
	for(i = 0; token != NULL; i++){
		cmd.command_list[i] = strdup(token);
		token = strtok_r(NULL, delim, &saveptr);
	}
	cmd.command_list[cmd.num_token] = NULL; 

	// #6. return the variable.	
	free(temp_buf);
	return cmd;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/

    for (int i = 0; i < command->num_token; i++) {
        free(command->command_list[i]);
    }
    free(command->command_list);
    
}
