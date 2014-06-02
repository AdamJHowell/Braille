/*
braille5.c  14 March 1996  Adam J. Howell
This program will convert inputed alpha-numeric characters into graphical (ASCII) braille characters.
This version includes ALL printable single cell class one braille characters, except for contractions.
New for version .5  I have added a case for backspace from keyboard entry.
*/


#include <string>
#include <iostream>
#include <fstream>


using namespace std;


const char DELIMITER = '\n';						// The delimiter that I use with getline.
const int IGNORE = 4096;						// I use this when clearing the input buffer.


int fileOpen( void );
char charInput( void );
int braille( char *out, int count, int xpos, int ypos );
int print_braille( FILE *direction, char *out, int count );
int print_top_row( FILE *direction, char *ch_to_prn, int count );
int print_mid_row( FILE *direction, char *ch_to_prn, int count );
int print_bot_row( FILE *direction, char *ch_to_prn, int count );


int main( void )
{
	int option = 0;

	printf( "This program will convert input characters\n" );
	printf( "into an ASCII version of braille." );

	do{
		cout << "\nWould you like to type in (C)haracters, import a (F)ile, or (Q)uit? [C/F/Q] " << endl;
		option = getch();			//get input one char at a time
		option = toupper( option );		//convert to upper case to simplify code
		// Test code.
		cout << option << endl;			//echo input

		switch( option )
		{

			case 'C' :			//case for keyboard entry
			{
				charInput();
				// Test code.
				//cout << charout << " endtered." << endl;
				break;
			}

			case 'F' :			//case for file entry
			{
				if( fileOpen() > 0 )
				{
					cout << endl;
					//cout << "Would you like to try again? ";
				break;
				}
			}

			case 'Q':			//case for quiting
			{
				cout << "Program terminated." << endl;
				break;
			}

			default:			//if all else fails
			{
				cout << "I couldn't read your answer, please try again.\n" << endl;
			}
		}
	} while(option != 'Q');				//the end of a do{} - while() loop
	return 0;
} // End main()


// Function name:	fileOpen()
// Purpose:		This function will open a file to use in the editor.
// Parameters:		none
// Returns:		An integer indicating success or fail.
// Preconditions:	none
// Postconditions:	none
int fileOpen( void )
{
	int handle = 0;
	int bytes = 0;
	int badcount = 0;
	string userFile = "";

	cout << "Please enter the name of the file you wish to input." << endl;
	cin >> userFile;
	// Clear the input buffer.
	cin.ignore( IGNORE, '\n' );
	cout << endl;
	ifstream inFile( userFile );

	// Test for file error.
	if ( inFile.fail() )
	{
		// Announce that we could not open the file.
		cout << "\nCould not open file \"" << userFile << "\" for reading." << endl;
		system( "PAUSE" );
		return 1;
	}
	else
	{
		// do things here...
	}
	// Close the file handle.
	inFile.close();

	return 0;
} // End fileOpen()


// Function name:	charInput()
// Purpose:		This function will read one character from the user.
// Parameters:		none
// Returns:		A single character.
// Preconditions:	none
// Postconditions:	none
char	charInput(void)
{
	char ch = '\n';

	printf("Please enter the characters you would like to convert.\n");
	printf("Press <Esc> to leave.\n");

	while( ( ch = getch() ) != 0x1b )		//while ch != <Esc>
	{
		braille( &ch, 1, wherex(), wherey() );	//process one char (ch) at a time
	}

	if(ch == 0x1b)		//test for <Esc> 1b is the hex
	{
		printf("\n\n\n\n<Esc> entered, exiting...\n\n");	//nelines needed to clear printed braille
	}
	return ch;
} // End charInput()


// Function name:	braille()
// Purpose:		This function will print Braille characters on screen.
// Parameters:		A char to display, a counter, the horizontal position, and the vertical position.
// Returns:		An integer indicating number of bad characters.
// Preconditions:	none
// Postconditions:	none
int	braille(char *out, int count, int xpos, int ypos)
{
	//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
	char top[285] = "    *     * ** ** **    *   * *   *        *  *-                             -*     *  **  * **  * * -*  ** ** *  ** ** *   *  * *  *  ** ** *  ** ** *   *  * *  *   * ** ** *   * *  **  *  *  *                                                                                * *  **  *";
	char mid[285] = "   *   *  * *     *     ** **                  - * *  *  ** ** *  ** ** *   *- *  * *  **  *  *      -*      *  * *  ** ** *  **    *      *  * *  ** ** *  **    *  **     *  * *  ** **  *  *                                                                                  *  ** **  *";
	char bot[285] = "   **    **  *  * ** *  ** **  * **  * **  * * -**    *      *  * *  ** ** * - *  *  * ** *   *      -                           *  *  *  *  *  *  *  *  *  *  ** **  * ** ** **  *  *  *     *                                                                                   *  *  *   ";

	int pos = 0;
	int y = 0;
	int badcount = 0;

	if( out == NULL )		//test for bogus pointer
	{
		return 1;		//for return value
	}

	if( wherey() > 21 )		//if (@ bottom of screen)
	{				//this little loop is for character entry only ! file.
		clrscr();		//clear the screen
		xpos = 1;		//set x position to 1
		ypos = 1;		//set y position to 1
	}

	for( ; count > 0; count--, out++ )
	{
		if( *out == 0x0d || *out == 0x0a )	//test for a carriage return
		{
			printf( "\n\n\n\n" );		//execute a CR (not for file)
			ypos += 4;			//increment y position
			xpos = 1;			//put x @ first position on line
			continue;			//skip the rest of the if()
		}

		if( *out == 0x8 )			//backspace
		{
			if( xpos < 3 )
               	continue;
			gotoxy( xpos - 3, ypos );
			printf( "   " );
			gotoxy( xpos - 3, ypos + 1 );
			printf( "   " );
			gotoxy( xpos - 3, ypos + 2 );
			printf( "   " );
			gotoxy( xpos - 3, ypos + 3 );
			printf( "   " );
			gotoxy( xpos - 3, ypos );
			continue;
		}

		if( *out == 0x0 )		//this is the first # of a 2 digit keyboard scan code. I need this to make the arrow keys work.
		{
			out++;
			continue;
		}

		if( *out == 0x4b )		//left arrow
		{
			gotoxy(xpos -=3, ypos);
			continue;
		}

		if( *out == 0x4d )		//right arrow
		{
			gotoxy(xpos += 3, ypos);
			continue;
		}

		if( *out == 0x48 )		//up arrow
		{
			gotoxy(xpos, ypos -= 4);
			continue;
		}

		if( *out == 0x50 )		//down arrow
		{
			gotoxy(xpos, ypos += 4);
			continue;
		}

		if( *out >= 'a' && *out <= 'z' )	// if(lower case)
			*out = toupper( *out );		// Convert to upper case.
		gotoxy( xpos, ypos );			// Move into position.
		printf( "%c", *out );			// This line is used to echo the chars.
		gotoxy( xpos, ypos + 1 );		// This simulates a newline, but wont overwrite prev. braille

		if( *out < ' ' || *out > '~' )		// Make sure char is in our array
		{
			printf("\n\n\nWARNING! - Character is out of range! : %x hex\n", *out);
			badcount++;			// Add up unprintable chars.
			continue;			// if not alpha break out of for() loop.
		}

		y = ( *out - 32 ) * 3;		//subtract 32 and convert to array position

		for ( pos = y; pos < (y + 2); pos++ )	//this loop prints top row
		{
			printf( "%c", top[pos] );
		}
		gotoxy( xpos, ypos + 2 );	//simulates newline w/o overwriting old braille

		for ( pos = y; pos < (y + 2); pos++ )	//this loop prints mid row
		{
			printf( "%c", mid[pos] );
		}
		gotoxy( xpos, ypos + 3 );	//simulates a newline

		for ( pos = y; pos < (y + 2); pos++ )	//this loop prints bottom row
		{
			printf( "%c", bot[pos] );
		}
		gotoxy( xpos + 3, ypos );	//simulates a newline

/*		if(count > 1)			//test if it's a file
		{
			if(xpos < 78)		//if !@ end of row
			{
				xpos += 3;	//increment x position
			}
			else			//if @ end or row
			{
				printf("Press any key to continue...\n");
				xpos = 1;	//set x position at start of row
				ypos += 5;	//increment y position to after the braille just printed
				getch();	//wait for keypress before continuing
			}
		}
*/
		if( ypos > 20 )			//if @ bottom of screen
		{
			clrscr();		//clear screen
			ypos = 1;		//start at top of new screen
			xpos = 1;		//start at begining of line
		}
	}
	return badcount;	 		//return the # of bad characters
} // End brailled()


/*
This is where it gets fun. The printer doesn't respond to gotoxy(), like in function braille(), so I have to write a whole new function just for the printer. It's not as easy as it looks.
Right now, I don't have a case for a carriage return.
*/

// Function name:	print_braille()
// Purpose:		This function will print Braille characters on screen.
// Parameters:
// Returns:		An integer indicating number of bad characters.
// Preconditions:	none
// Postconditions:	none
int	print_braille( FILE *direction, char *out, int count )
{			//*out is the whole buffer, and count is the # of bytes
	int badcount = 0;
	int total = 0;


	if( out == NULL )			//test for bogus pointer
	{
		return 1;			//for return value
	}
/*
	total = count;				//total now has the # of chars in *out
*/
	if( direction == stdout )		//if printing
	{
		printf( "Printing... %d characters", total );	//another debugging tool
	}

	while( count > 0 )			//*out == buffer, count == bytes
	{
		fprintf( direction, "\n" );
		total = print_top_row( direction, out, count );
		print_mid_row( direction, out, total );
		print_bot_row( direction, out, total );

		count -= total;
		out += total;

		if( direction == stdout )
		{
			printf( "Press any key to continue..." );
			getch();
		}
	}
	return badcount;	// Since badcount is not currently incremented, this effectively returns 0;
} // End print_braille()


// Function name:	print_top_row()
// Purpose:		This function will print the top row of Braille dots on screen.
// Parameters:
// Returns:		An integer indicating the number of characters printed.
// Preconditions:	none
// Postconditions:	none
int	print_top_row( FILE *direction, char *ch_to_prn, int count )
{
	//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
	char top[285] = "    *     * ** ** **    *   * *   *        *  *-                             -*     *  **  * **  * * -*  ** ** *  ** ** *   *  * *  *  ** ** *  ** ** *   *  * *  *   * ** ** *   * *  **  *  *  *                                                                                * *  **  *";
	int pos = 0;
	int total = 0;
	char *pString;

	for( pString = ch_to_prn; total < 26 && total < count; pString++, total++)	//just to print the ascii
	{
		if( *pString == 0x0a )
		{
			fprintf( direction, "\n" );
			break;
		}
		if( *pString >= 'a' && *pString <= 'z' )
			*pString = toupper( *pString );	//convert to upper case
		fprintf( direction, "%c  ", *pString );
	}
	fprintf( direction, "\n" );

	for( total = 0; total < 26 && total < count; total++, ch_to_prn++ )	//used to stop @ end of row
	{
		if( *ch_to_prn == 0x0a )
		{
			total++;		// Increment this so the calling function knows to advance his pointer properly.
			fprintf( direction, "\n" );
			return total;
		}
		pos = ( *ch_to_prn - 32 ) * 3;	// Subtract 32 and convert to array position.
		fprintf( direction, "%c", top[pos] );
		fprintf( direction, "%c ", top[pos+1] );
	}
	fprintf( direction, "\n" );

	return total;
} // End print_top_row()


// Function name:	print_mid_row()
// Purpose:		This function will print the middle row of Braille dots on screen.
// Parameters:
// Returns:		0.
// Preconditions:	none
// Postconditions:	none
int print_mid_row( FILE *direction, char *ch_to_prn, int count )
{
	//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
	char mid[285] = "   *   *  * *     *     ** **                  - * *  *  ** ** *  ** ** *   *- *  * *  **  *  *      -*      *  * *  ** ** *  **    *      *  * *  ** ** *  **    *  **     *  * *  ** **  *  *                                                                                  *  ** **  *";
	int pos, total;

	for( total = 0; total < 26 && total < count; total++, ch_to_prn++ )	//used to stop @ end of row
	{
		if( *ch_to_prn == 0x0a )
		{
			total++;
			fprintf( direction, "\n" );
			return total;
		}
		pos = ( *ch_to_prn - 32 ) * 3;	//subtract 32 and convert to array position
		fprintf( direction, "%c", mid[pos] );
		fprintf( direction, "%c ", mid[pos+1] );
	}
	fprintf( direction, "\n" );

	return 0;
} // End print_mid_row()


// Function name:	print_mid_row()
// Purpose:		This function will print the middle row of Braille dots on screen.
// Parameters:
// Returns:		The number of unprocessed chars
// Preconditions:	none
// Postconditions:	none
int print_bot_row( FILE *direction, char *ch_to_prn, int count )
{
	//               sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / -0  1  2  3  4  5  6  7  8  9 -:  ;  <  =  >  ?  @  A -B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~ ;
	char bot[285] = "   **    **  *  * ** *  ** **  * **  * **  * * -**    *      *  * *  ** ** * - *  *  * ** *   *      -                           *  *  *  *  *  *  *  *  *  *  ** **  * ** ** **  *  *  *     *                                                                                   *  *  *   ";
	int pos, total;

	for( total = 0; total < 26 && total < count; total++, ch_to_prn++ )	// used to stop @ end of row
	{
		if( *ch_to_prn == 0x0a )
		{
			total++;
			fprintf( direction, "\n" );
			return total;
		}
		pos = ( *ch_to_prn - 32 ) * 3;	// subtract 32 and convert to array position
		fprintf( direction, "%c", bot[pos] );
		fprintf( direction, "%c ", bot[pos+1] );
	}
	fprintf( direction, "\n" );			// add a newlline

	return count - total;				// return the # of unprocessed chars
} // End print_bot_row()
