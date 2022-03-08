#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sts/wait.h>

	/*
	 * The main method will take in inputs of the user, readi in by fgets, and is them parsed for the amount of arguments entered.
	 * Once finished, initialze a char matrix to hold those string arguments, re-parse through the input char array and parse through the characters storing them in 
	 * 	it's current char row until it encounters a space or a new character where it will move to the next row in the char matrix for the next string.
	 */

int main(void){		
	int args = 0;    //Number of argumnts
	int hld = 0;     //Holding variable
	int smt = 0;     //split main thread, indicates if the main thread will continue without waiting for the child process
	int relic = 0;   //Sometimes if '&' is used, the program will act as if the next command will be using '&' whether it actually did or not,
	do {             //	acting as a 'relic' of a previous execution, so this is just an indicator. Logically it should interfere with consecutive '&'s with the wait(),
		         //	but interestingly, it doesn't. This was from previous revisions so I don't know if it still matters, but I'm too scared and tired to find out. 
		char input[1024];
		if(relic == 1){
			smt = wait(NULL);
		}
		smt = 0;
		args = 0;
		hld = 0;
		write(1, "EbinShell::>>", 13); //Prompt
		fgets(input, 1024, stdin);
		//Read in the characters entered by the user and count how many words are encountered
		//Something of note is that it will explicitly count an empty string if the first character is a space,
		//	I forgot why I put it here, but I trust my past self that it's to resolve an error
		for(int i = 1; i < 1024; i++){
			if(i-1 == 0 && input[i-1] == 32){
				args++;
				continue;
			}
			if((input[i-1] > 32 && input[i-1] < 127) && (input[i] == 32 || input[i] == 10)){
				args++;
			}
		}
		//Create a char matrix of 'args' amount of strings, each with a size of 100
		char str[args][100];
		int ch[args]; //This will hold the number of characters are in each string, with the index corrosponding with it's associated row of strings
		//Clear the string matrix; I have found that sometimes 'junk data' is left over, which will mess up ch[] count, so I just have to clear it to 0s
		for(int i = 0; i < args; i++){
			for(int j = 0; j < 100; j++){
				str[i][j] = 0;
			}
		}
		int j = 0;                                            //j is used for the indexes of input[]
		for(int i = 0; i < args; i++){                        //Stop once it has read all of the strings from the user, otherwise it will start to read 'junk data'
			for(int k = 0; j < 1024; j++, k++){           //k is for the index of the string row's column.
				if(input j <= 32 || input[j] >= 127){ //If we've reached the end of the word then store k  in ch[]
					ch[i] = k;
					j++;
					break;
				}
				str[i][k] = input[j];                 //Storing characters
			}
		}
		//Check if the word entered is 'exit', compensating the index if there's an empty space by hld
		if(ch[0] == 4 || (ch[0] == 0 && ch[1] == 4)){
			if(ch[0] == 0){
				hld = 1;
			}
			if(str[hld][0] == 101 && str[hld][1] == 120 && str[hld][2] == 105 && str[hld][3] == 116){
				exit(0);
				break;
			}
		}
		hld = 0; //Reset to utilize for other purposes
		//If the last character is '&', indicate that the main thread will 'split' from the child process
		if(ch[args-1] == 1 && str[args-1][0] == 38){
			relic = 0;
			smt = 1;
		}
		//If arguments exists then try for a command, else skip
		if(args != 0){
			void* ptr = malloc(0);
			if(ch[0] == 0){
				hld = 1; //If there's an extra space, hld will be an offset by 1
			}
			if((smt == 1 && args == 1) || (smt == 1 & hld ==1 && args == 2)){
				//This statement is to check if only an '&' is entered, if so then do nothing, skipping execv.
			} else if((hld == 0 || args > 1)){ //If there is no space, then continus, if hld != 0 then it will check if there is more than 1 argument,
				int frk = fork();          //	as that space will take the place of 1. Else it's empty, holding only 1 space character.
				if(frk < 0){
					fprintf(stderr, "Fork Failed");
					exit(1);
				} else if(frk == 0){
					if(args >= 1){ //By boolean logic this is not needed, but I've found that execv needs to be in an if statement to properly display an error message.
						int n = ((args-hld-smt)+1); //The number of arguments, compensating for the end NULL element, space offset, and '&';
						char *pargs[n];             //	this is easy as they are initialized as 0 and are 1 only if an offet or an '&' is encountered.
						ptr = pargs; //Since the child process will immedietly exit, it won't be able to free memory, so it is assigned for the main process to free after waiting.
						pargs[0] = malloc((ch[hld]+1)*sizeof(char));
						pargs[0] = str[hld];                               //Allocate and store the command and the final element NULL
						pargs[n-1] = NULL;
						hld = hld + 1;                                     //Shift starting index for potantial command arguments
						for(int i = hld; i < (n-1)l i++){
							pargs[i] = malloc((ch[i]+1)*sizeof(char)); //Allocate and store remainding arguments, upto n-2
							pargs[i] = str[i];
						}
						execv(pargs[0], pargs);                            //Execute
					}
					fprintf(stderr, "Error: Command could not be found.\n\n");
					exit(1);
				} else if(smt == 0){
					relic = 1;        //Too tired to remember and deal with
					smt = wait(NULL); //Wait
					free(ptr);        //Free the memory that was used by the child process
					                  //Something that bothers me is that I don't know how to (or when) to free memory if the main thread continues without waiting
							  //for the child process to be finished with it.
				}
			}
		}
		for(int i = 0; i < 1024; i++){ //Clear input; could be skipped in certain cases with conditionals, but I'm too 'pooped out' to think about that
			input[i] = 0;
		}
	} while(1==1);
	return 0;
}
