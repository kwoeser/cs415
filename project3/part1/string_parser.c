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
	if (buf == NULL){ 
		exit(EXIT_FAILURE);
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
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/


	// create command_line variable to be filled and returned
	command_line cmd;
	char* copy_buf;
	char* saveptr = NULL;
	int index = 0;

	// malloc memory for token array inside command_line variable
	copy_buf = strdup(buf);
	cmd.num_token = count_token(buf, delim);
	cmd.command_list = malloc(sizeof(char*) * (cmd.num_token+1));

	// remove the \n at the end of the line, count the number of tokens 
	strtok_r(copy_buf, "\n", &saveptr);
	char* token = strtok_r(copy_buf, delim, &saveptr);


	while(token!=NULL){
	    cmd.command_list[index] = strdup(token);
	    index++;
	    token = strtok_r(NULL, delim, &saveptr);
	}

	cmd.command_list[index] = NULL;
	free(copy_buf);

	return cmd;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
	for(int i = 0; i < command->num_token; i++){
	       free(command->command_list[i]);
	}
	free(command->command_list);	
}
