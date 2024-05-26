/*
 * Detoxy 0.1
 *
 * Detokenizes CBM Basic programs.  
 * Write now, it supports BASIC versions 2.0 and 3.5.  More will be added in the future
 * 
 * compile with:
 *              gcc -O3 detoxy.c -o detox
 *		clang -O3 detoxy.c -o detox 
 *
 * (c) 2024 Derrik Walker v2.0
 * This is licensed for use under the GNU General Public License v2
 *
 * Forked from detox64 by Stian Soreng, https://github.com/vroby65/detox64
 *
 * Changes from original:
 *	- Added all the tokens and other code for BASIC 3.5
 *	- Updated to use getopts for argument processing
 *	- Added an argument for displaying the BASIC version
 *	- A couple bug fixes and a little code clean up
 *
 * 2024-05-11   dwalker		Initial version
 * 2024-05-25	dwalker		Added support for BASIC 3.5 with graphics
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

char *tokens[] = {
	"END", "FOR", "NEXT", "DATA", "INPUT#", "INPUT", "DIM", "READ",
	"LET", "GOTO", "RUN", "IF", "RESTORE", "GOSUB", "RETURN", "REM",
	"STOP", "ON", "WAIT", "LOAD", "SAVE", "VERIFY",	"DEF", "POKE",
	"PRINT#", "PRINT", "CONT", "LIST", "CLR", "CMD", "SYS", "OPEN",
	"CLOSE", "GET", "NEW", "TAB(", "TO", "FN", "SPC(", "THEN", "NOT", 
	"STEP", "+", "-", "*", "/", "^", "AND", "OR", ">", "=", "<", "SGN", 
	"INT", "ABS", "USR", "FRE", "POS", "SQR", "RND", "LOG", "EXP", 
	"COS", "SIN", "TAN", "ATN", "PEEK", "LEN", "STR$", "VAL", "ASC", 
	"CHR$", "LEFT$", "RIGHT$", "MID$", "GO", "RGR", "RCLR", "RLUM", "JOY", 
	"RDOT", "DEC", "HEX$", "ERR$", "INSTR", "ELSE", "RESUME", "TRAP", 
	"TRON", "TROFF", "SOUND", "VOL", "AUTO", "PUDEF", "GRAPHIC", "PAINT", 
	"CHAR", "BOX", "CIRCLE", "GSHAPE", "SSHAPE", "DRAW", "LOCATE", "COLOR", 
	"SCNCLR", "SCALE", "HELP", "DO", "LOOP", "EXIT", "DIRECTORY", "DSAVE", 
	"DLOAD", "HEADER", "SCRATCH", "COLLECT", "COPY", "RENAME", "BACKUP", 
	"DELETE", "RENUMBER", "KEY", "MONITOR", "USING", "UNTIL", "WHILE", "none"
};

unsigned char getByte( FILE *fp ) {

	unsigned char in = 0x00;

	if( fp ) 
		( void )fread( &in, 1, 1, fp );
	return( in );
}

enum {
	INPUT_BAS=1,
	INPUT_P00
};

int help() {
	printf( "Usage: detox64 [-f type] <filename>\n" );
	printf( "-f type: input file format name, possible values are:\n" );
	printf( "     -f bas    BASIC format ( default )\n" );
	printf( "     -f p00    Emulator file\n" );
	printf( " -b: print the detected BASIC Version\n" );
	printf( " -h: help\n" );
	return( 1 );
}

int main( int argc, char **argv ) {

	unsigned int lineno = 0;
	FILE *fp;
	int input_type = 0;
	int print_basic_ver = FALSE;
	int opts;
	opterr = 0;

	while (( opts = getopt( argc, argv, "f:b" )) != -1 )
	
		switch( opts ) {

			case 'b':
				print_basic_ver = TRUE;
				break;

			case 'f':
				if ( strcmp( optarg, "bas" ) == 0 ) 
					input_type = INPUT_BAS;

				else if ( strcmp( optarg, "p00" ) == 0 ) 
					input_type = INPUT_P00;

				else
					return( help() );
				break;

			case 'h':
				return( help() );

			default:
				return( help() );
		}

	fp = fopen( argv[optind], "rb" );

	if( fp == NULL ) {

		printf( "Unable to open input file '%s' for reading\n", argv[optind] );
		return( 2 );
	}

	if( input_type == INPUT_P00 ) {

		char tmp[8];
		(void) fread( tmp, 8, 1, fp );	

		if( strcmp( tmp, "C64File" ) == 0 ) {

			int x=0;

			for( x=0; x<18; x++ )
				(void) getByte( fp );

		} else {
			printf( "File format not recognized\n" );
			fclose( fp );
			return( 1 );
		}
	}

	/* The Load location to find the BASIC version */
	switch ( ( getByte(fp))|(getByte(fp)<<8 ) ) {

		case 0x0801:	/* BASIC 2.0 on the C64 */
			if( print_basic_ver ) 
				printf( "BASIC 2.0\n");
			break;

		case 0x1001:	/* BASIC 3.5 on a TED without high-res graphics */
			if( print_basic_ver ) 
				printf( "BASIC 3.5\n" );
			break;

		case 0x4001:	/* BASIC 3.5 on a TED with high-res graphics */
			if( print_basic_ver ) 
				printf( "BASIC 3.5\n" );
			break;

		default:
			printf( "This is probably not a BASIC file.\n" );
	 		fclose( fp );
		 	return( 1 );
	}

	(void)getByte(fp);
	(void)getByte(fp);

	while( !feof(fp) ) {

		unsigned char ch = 0;

		lineno = getByte(fp)|(getByte(fp)<<8);
		printf( "%d ", lineno );
		ch = getByte( fp );

		while( (!feof(fp)) && (ch != 0x00) ) {

			if( ch > 0x7f ) 
				printf( "%s", tokens[ch&0x7f] );

			else 
				printf( "%c", ch );

			ch = getByte( fp );
		}

		printf( "\n" );

		if( (getByte(fp)|(getByte(fp)<<8)) == 0x00 ) break;
	}

	fclose( fp );
	return( 0 );
}
