/*
	Name: Utkarsh Verma
	Student ID: 1001663173

*/
// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2020 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports ten arguments(changed to 10 as said in the pdf file of assignment)

int main()
{
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  char *history[15] ; // array for storing history of commands
  int i;
  int signal=0;
  for(i=0;i<15;i++)
  {
	history[i]=(char*) malloc( MAX_COMMAND_SIZE );
  }
  int history_index=0;
  int pid_history[15]; //array for storing list of pids
  int pid_index=0;
  //all the possible filePath where to look for commands
  char *filePath1="/bin/";
  char *filePath2="/usr/bin/";
  char *filePath3="/usr/local/bin/";
  pid_t pid;
  while( 1 )
  {
    
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
	
    while(!fgets (cmd_str, MAX_COMMAND_SIZE, stdin));
	//checking for new line or when no input is given
	if(cmd_str[0]=='\n' || cmd_str[0]==' ' ||cmd_str[0]=='\t')
	{
		continue;
	}

	//saving the history of commands here
	strncpy(history[history_index],cmd_str,255);
	history_index++;
	//checking if number of commands in history_index is exceeding 15
	if(history_index>14)
	{ 
		history_index=0;
		signal=1;
	}
	
	//if !n is entered then we will execute the nth command in the history index
	//this is done by parsing out ! from !n and then loading the nth command in cmd_str
	if(cmd_str[0]=='!')
	{
		
		int cmdNum=atoi(&cmd_str[1]); //converting char into integer using atoi
		//checking if we don't have the nth command in the array
		if(cmdNum-1>history_index)
		{
			strcpy(cmd_str,"gbg");  //setting it equal to a garbage command
			printf("Command not found in history\n");
		}
		else
		{
			strcpy(cmd_str,history[cmdNum-1]);
		}
	
	}
	
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );    

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;
	
    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
	//if quit or exit is entered then we will terminate the program with status 0
	if((strcmp(token[0],"exit")==0)|| (strcmp(token[0],"quit")==0))
    {
		exit(0);
    }
	//checking when no input is given
	//along with that we are also checking for the gbg command set up in the history
	//this will help us to skip the part of the loop so that we don't get "Command not found"
	//and will only print "command not in history"
    else if(token[0]==NULL || (strcmp(token[0],"gbg")==0))
    {
		continue;
    }
	//comparing with both cd and NULL just in case if we don't have anything after NULL
	
    else if((strcmp(token[0],"cd")==0) && token[1]!=NULL)
    {
		if(chdir(token[1])==-1)
		{
			printf("No such file or directory\n");
		}	
    }
	//This will display the list of all pids stored in our pids array
    else if(strcmp(token[0],"showpids")==0)
    {
		int i;
		int index=pid_index;
		for(i=0;i<index;i++)
		{ 
			printf("%d. %d \n",i+1, pid_history[i]);
		}
    }
	//This will show us the history of last 15 commands
    else if(strcmp(token[0],"history")==0)
    {
		int i;	
		int index=history_index;
		//if we have less than 15 commands then we will run the code in the if 
		//it will also check for signal. If we have filled the whole array that is 15 commands
		//then the signal will be set to 1 and we won't be able the if part
		if(index<15 && signal==0)
		{
			for(i=0;i<index;i++)
			{
				printf("%d. %s\n",(i+1),(history[i]));
				//checking if we have more than 15 commands in the array
			}
		}
		//if we have full list of 15 commands in the history array then we will enter the else part
		else
		{
			for(i=0;i<15;i++)
			{
				printf("%d. %s\n",(i+1),history[i]);
				//checking if we have more than 15 commands in the array
					
			}
			if(history_index>14)
			{ 
				history_index=0;
				signal=1;
			}
		}
    }
    
	else
	{	
		pid=fork(); //creating a child process to run other linux commands
		
		//checking for any errors if the fork fails
		if(pid<0)
		{
			perror("Fork Failed");
		}
		//child process
		else if(pid==0)
		{
			char cmd[100]; //an array to hold the file path where we look for the code to execute
			// copying/overwriting the first file path that is bin to the array cmd
			strcpy(cmd,filePath1);
			//concatenting the command entered by the user
			strcat(cmd,token[0]);
			//looking for the command and executing the command in the given file path
			//if it doesn't fail then the rest of code in this block will not run
			execvp(cmd,token);
			
			// copying/overwriting the first file path that is usr/bin to the array cmd
			strcpy(cmd,filePath2);
			//concatenting the command entered by the user
			strcat(cmd,token[0]);
			//looking for the command and executing the command in the given file path
			//if it doesn't fail then the rest of code in this block will not run
			
			execvp(cmd,token);
			
			// copying/overwriting the first file path that is usr/bin to the array cmd
			strcpy(cmd,filePath3);
			//concatenting the command entered by the user
			strcat(cmd,token[0]);
			//looking for the command and executing the command in the given file path
			//if it doesn't fail then the rest of code in this block will not run
			execvp(cmd,token);
			
			//this print statement will only be printed when all of the above exec statement fails to execute 
			//this also means the given command is not a valid command as we could not find it in any of the directory
			printf(" %s: Command not found!\n",token[0]);
			//exit will help get out of the process invalid command is entered
			exit(0);
		}
		//parent proceess
		else
		{
			int status;
			//waiting for the child process to end
			wait(&status);
		  
			//the pid of the parent will also be stored in the pid array here
			pid_history[pid_index]=pid;
			pid_index++;
			
		}

		
	}
    free( working_root );

  }
  return 0;
}