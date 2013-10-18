/*braille5.c  14 March 1996  Adam J. Howell
This program will convert inputed alpha-numeric characters
into graphical (ASCII) braille characters.
This version includes ALL printable single cell class
one braille characters, ecept for contractions.
New for version .5  Now I've added a case for backspace from
keyboard entry.*/

#include <process.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <fcntl.h>
//#include <errno.h>	//needed for file error descriptions, not incl yet
#include <alloc.h>
#include <ctype.h>
#include <dir.h>
#include <io.h>

int	file(void);

char	character(void);

int	braille(char *out, int count, int xpos, int ypos);

int	print_braille(FILE *direction, char *out, int count);

int	print_top_row(FILE *direction, char *ch_to_prn, int count);

int	print_mid_row(FILE *direction, char *ch_to_prn, int count);

int	print_bot_row(FILE *direction, char *ch_to_prn, int count);


/*This first function is not mine, so no help from me.
All code and comments in it are from Borland C++ 3.1 help files.*/

char *current_directory(char *path)
{
	strcpy(path, "X:\\");      // fill string with form of response: X:\
	path[0] = 'A' + getdisk();    // replace X with current drive letter
	getcurdir(0, path+3);  // fill rest of string with current directory
	return(path);
}

int	main(void)
{
int	option, exit_code;

	clrscr();				//clear screen to start program

	printf("This program will convert input characters\n");
	printf("into an ASCII version of braille.");

	do{
		printf("\nWould you like to type in (C)haracters, import a (F)ile, or (Q)uit? [C/F/Q] ");
		option = getch();			//get input one char at a time
		option = toupper(option);	//convert to upper case to simplify code
		printf("%c\n", option);		//echo input

		switch (option)
		{

			case 'C' :	//case for keyboard entry
			{
				/*charout = */character();
//				printf("%x hex entered.\n", charout);	//debuging tool
				break;
			}

			case 'F' :	//case for file entry
			{
				exit_code = file();
				if(exit_code == 1)
					printf("\n");
//					printf("Would you like to try again? ");
				break;
			}

			case 'Q':		//case for quiting
			{
				printf("Program terminated.\n");
				break;
			}

			default:		//if all else fails
			{
				printf("I couldn't read your answer, please try again.\n\n");
			}
		}
	} while(option != 'Q');		//the end of a do{} - while() loop
	return 0;
}

int	file(void)
{
int	handle, bytes, badcount;

	FILE *direction;
	char filename[257], curdir[MAXPATH], *buf, answ;

//This gives a 4 Kbyte limit to file input.  See also below
//	buf = malloc(4096);		//init buf for file input
	memset(buf, 0x00, 4096);	//clear buf to avoid errors

	if (buf == NULL)		//test for malloc() error
	{
		printf("WARNING! - Unable to allocate memory for the input buffer\n");
		return 1;			//return errorlevel
	}

	current_directory(curdir);
	printf("Please enter the path and file name you want to input.\n");
//Make sure the file exists or we get an error and are kicked out.
	printf("I've only reserved 4 Kbytes now.\n");
	printf("The current directory is %s\n", curdir);
	scanf("%s", filename);
	handle = open(filename, O_RDONLY); //my code, file is read-only

	if (handle == -1)		//if file error
	{
		perror("Error ");
		return 1;			//return errorlevel
	}

	if ((bytes = read(handle, buf, 4096)) == -1)	//4 K file limit
	{
		printf("Read Failed.\n");
		return 1;
//		exit(1);			//this line quits execution
	}

	else					//if no error
	{
		printf("Read: %d bytes read.\n", bytes);
		printf("They were :\n%s\n\n", buf); //just here to check for errors.
	}
	close(handle);			//close file

	return 0;
}

char	character(void)
{
char	ch;

	printf("Please enter the characters you would like to convert.\n");
	printf("Press <Esc> to leave.\n");

	while((ch = getch()) != 0x1b)	//while ch != <Esc>
	{
		braille(&ch, 1, wherex(), wherey());	//process one char (ch) at a time
	}

	if(ch == 0x1b)		//test for <Esc> 1b is the hex
	{
		printf("\n\n\n\n<Esc> entered, exiting...\n\n");	//nelines needed to clear printed braille
	}
	return ch;
}

int	braille(char *out, int count, int xpos, int ypos)
{
//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
char	top[284] = "    *     * ** ** **    *   * *   *        *  *-                             -*     *  **  * **  * * -*  ** ** *  ** ** *   *  * *  *  ** ** *  ** ** *   *  * *  *   * ** ** *   * *  **  *  *  *                                                                                * *  **  *";
char	mid[284] = "   *   *  * *     *     ** **                  - * *  *  ** ** *  ** ** *   *- *  * *  **  *  *      -*      *  * *  ** ** *  **    *      *  * *  ** ** *  **    *  **     *  * *  ** **  *  *                                                                                  *  ** **  *";
char	bot[284] = "   **    **  *  * ** *  ** **  * **  * **  * * -**    *      *  * *  ** ** * - *  *  * ** *   *      -                           *  *  *  *  *  *  *  *  *  *  ** **  * ** ** **  *  *  *     *                                                                                   *  *  *   ";

int	pos = 0, y = 0, badcount = 0;

	if(out == NULL)		//test for bogus pointer
	{
		return 1;			//for return value
	}

	if(wherey() > 21)	//if (@ bottom of screen)
	{				//this little loop is for character entry only ! file.
		clrscr();		//clear the screen
		xpos = 1;		//set x position to 1
		ypos = 1;		//set y position to 1
	}

	for(; count > 0; count--, out++)
	{
		if(*out == 0x0d || *out == 0x0a)	//test for a carriage return
		{
			printf("\n\n\n\n");		//execute a CR (not for file)
			ypos += 4;			//increment y position
			xpos = 1;				//put x @ first position on line
			continue;				//skip the rest of the if()
		}

		if(*out == 0x8)		//backspace
		{
			if(xpos < 3)
               	continue;
			gotoxy(xpos - 3, ypos);
			printf("   ");
			gotoxy(xpos - 3, ypos + 1);
			printf("   ");
			gotoxy(xpos - 3, ypos + 2);
			printf("   ");
			gotoxy(xpos - 3, ypos + 3);
			printf("   ");
			gotoxy(xpos - 3, ypos);
			continue;
		}

		if(*out == 0x0)	//this is the first # of a 2 digit
		{				//keyboard scan code. I need this
			out++;		//to make the arrow keys work
			continue;
		}

		if(*out == 0x4b)	//left arrow
		{
			gotoxy(xpos -=3, ypos);
			continue;
		}

		if(*out == 0x4d)		//right arrow
		{
			gotoxy(xpos += 3, ypos);
			continue;
		}

		if(*out == 0x48)		//up arrow
		{
			gotoxy(xpos, ypos -= 4);
			continue;
		}

		if(*out == 0x50)		//down arrow
		{
			gotoxy(xpos, ypos += 4);
			continue;
		}

		if(*out >= 'a' && *out <= 'z')	//if(lower case)
			*out = toupper(*out);	//convert to upper case
		gotoxy(xpos, ypos);
		printf("%c", *out);			//this line is used to echo the chars
		gotoxy(xpos, ypos + 1);		//this simulates a newline, but wont overwrite prev. braille

		if(*out < ' ' || *out > '~')	//make sure char is in our array
		{
			printf("\n\n\nWARNING! - Character is out of range! : %x hex\n", *out);
			badcount++;		//add up bogus chars
			continue;			//if not alpha break out of for() loop
		}

		y = (*out - 32) * 3;	//subtract 32 and convert to array position

		for (pos = y; pos < (y + 2); pos++)	//this loop prints top row
		{
			printf("%c", top[pos]);
		}
		gotoxy(xpos, ypos + 2);	//simulates newline w/o overwriting old braille

		for (pos = y; pos < (y + 2); pos++)	//this loop prints mid row
		{
			printf("%c", mid[pos]);
		}
		gotoxy(xpos, ypos + 3);	//simulates a newline

		for (pos = y; pos < (y + 2); pos++)	//this loop prints bottom row
		{
			printf("%c", bot[pos]);
		}
		gotoxy(xpos + 3, ypos);	//simulates a newline

/*		if(count > 1)			//test if it's a file
		{
			if(xpos < 78)		//if !@ end of row
			{
				xpos += 3;	//increment x position
			}
			else				//if @ end or row
			{
				printf("Press any key to continue...\n");
				xpos = 1;		//set x position at start of row
				ypos += 5;	//increment y position to after the braille just printed
				getch();		//wait for keypress before continuing
			}
		}
*/
		if(ypos > 20)		//if @ bottom of screen
		{
			clrscr();		//clear screen
			ypos = 1;		//start at top of new screen
			xpos = 1;		//start at begining of line
		}
	}
	return badcount;	 	//return the # of bad characters
}

/*This is where it gets fun. The printer doesn't respond to gotoxy(),
like in function braille(), so I have to write a whole new function
just for the printer. It's not as easy as it looks.*/
/*Right now, I don't have a case for a carriage return.*/

int	print_braille(FILE *direction, char *out, int count)
{				//*out is the whole buffer, and count is the # of bytes
int	badcount = 0, total;


	if(out == NULL)		//test for bogus pointer
	{
		return 1;			//for return value
	}
/*
	total = count;			//total now has the # of chars in *out
*/
	if(direction == stdout)	//if printing
	{
		printf("Printing... %d characters", total);	//another debugging tool
	}

	while(count > 0)	//*out == buffer, count == bytes
	{
		fprintf(direction, "\n");
		total = print_top_row(direction, out, count);
		print_mid_row(direction, out, total);
		print_bot_row(direction, out, total);

		count -= total;
		out += total;

		if(direction == stdout)
		{
			printf("Press any key to continue...");
			getch();
		}
	}
	return badcount;
}

int	print_top_row(FILE *direction, char *ch_to_prn, int count)
{
//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
char	top[284] = "    *     * ** ** **    *   * *   *        *  *-                             -*     *  **  * **  * * -*  ** ** *  ** ** *   *  * *  *  ** ** *  ** ** *   *  * *  *   * ** ** *   * *  **  *  *  *                                                                                * *  **  *";
int	pos, total;
char *pString;

	for(total = 0, pString = ch_to_prn;
		total < 26 && total < count;
		pString++, total++)	//just to print the ascii
	{
		if(*pString == 0x0a)
		{
			fprintf(direction, "\n");
			break;
		}
		if(*pString >= 'a' && *pString <= 'z')
			*pString = toupper(*pString);	//convert to upper case
		fprintf(direction, "%c  ", *pString);
	}
	fprintf(direction, "\n");

	for(total = 0; total < 26 && total < count; total++, ch_to_prn++)	//used to stop @ end of row
	{
		if(*ch_to_prn == 0x0a)
		{
			total++;	// Increment this so the calling function knows
						// to advance his pointer properly.
			fprintf(direction, "\n");
			return total;
		}
		pos = (*ch_to_prn - 32) * 3;	//subtract 32 and convert to array position
		fprintf(direction, "%c", top[pos]);
		fprintf(direction, "%c ", top[pos+1]);
	}
	fprintf(direction, "\n");

	return total;
}

int	print_mid_row(FILE *direction, char *ch_to_prn, int count)
{
//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
char	mid[284] = "   *   *  * *     *     ** **                  - * *  *  ** ** *  ** ** *   *- *  * *  **  *  *      -*      *  * *  ** ** *  **    *      *  * *  ** ** *  **    *  **     *  * *  ** **  *  *                                                                                  *  ** **  *";
int	pos, total;


	for(total = 0; total < 26 && total < count; total++, ch_to_prn++)	//used to stop @ end of row
	{
		if(*ch_to_prn == 0x0a)
		{
			total++;
			fprintf(direction, "\n");
			return total;
		}
		pos = (*ch_to_prn - 32) * 3;	//subtract 32 and convert to array position
		fprintf(direction, "%c", mid[pos]);
		fprintf(direction, "%c ", mid[pos+1]);
	}
	fprintf(direction, "\n");

	return 0;
}

int	print_bot_row(FILE *direction, char *ch_to_prn, int count)
{
//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
char	bot[284] = "   **    **  *  * ** *  ** **  * **  * **  * * -**    *      *  * *  ** ** * - *  *  * ** *   *      -                           *  *  *  *  *  *  *  *  *  *  ** **  * ** ** **  *  *  *     *                                                                                   *  *  *   ";
int	pos, total;


	for(total = 0; total < 26 && total < count; total++, ch_to_prn++)	//used to stop @ end of row
	{
		if(*ch_to_prn == 0x0a)
		{
			total++;
			fprintf(direction, "\n");
			return total;
		}
		pos = (*ch_to_prn - 32) * 3;	//subtract 32 and convert to array position
		fprintf(direction, "%c", bot[pos]);
		fprintf(direction, "%c ", bot[pos+1]);
	}
	fprintf(direction, "\n");			//add a newlline

	return count - total;				//return the # of unprocessed chars
}
