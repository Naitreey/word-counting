/*-----------word_counting.c--------------*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
/*---for time---*/
#include <time.h>
/*--------------*/

#define MWLEN 30	//the maximum size of "sane" English words

/*-----------------function prototypes-------------------*/

/* description: open `filename' for reading.
 * return value: Return pointer to file when success.
 * note: Gather details about opening a file in this function*/
static FILE * OpenFile(char *filename);
/* description: input a single word from `pfile' into `buffer'.
 * return value: Valid input return `true', otherwise `false'.
 * note: Deal with details about filtering words among ascii sequences*/
static bool InputWord(FILE *pfile, char *buffer);
/* description: show the counting of a word
 * note: the `item' object has information needed*/
static void DisplayCounts(Item item);
/* description: convert string to lowercase*/
static void ToLower(char *str);

/*------------------------main()-----------------------*/

int main(int argc, char *argv[])
{
	FILE *pfile;
	char buffer[MWLEN];
	Item item;
	Tree wordtree;

	if( argc!=2 )
	{
		fprintf(stderr, "%s: wrong usage.\n" , argv[0]);
		exit(EXIT_FAILURE);
	}

	pfile=OpenFile(argv[1]);

	InitializeTree(&wordtree);

	/*---for time---*/
	clock_t begin, end;
	begin=clock();
	/*--------------*/

	while( InputWord(pfile, buffer) )
	{
		ToLower(buffer);
		item=MakeItem(buffer);
		AddItem(&item, &wordtree);
	}

	/*---for time---*/
	end=clock();
	/*--------------*/

	fclose(pfile);

	Traverse(&wordtree, DisplayCounts);

	DeleteAll(&wordtree);

	/*---for time---*/
	printf( "time: %f\n" , (double)(end-begin)/CLOCKS_PER_SEC );	//clock_t type is integral type
	/*--------------*/

	return 0;
}

/*-------------------function definitions----------------*/

static FILE * OpenFile(char *filename)
{
	FILE *pfile;
	if( (pfile=fopen(filename, "r"))==NULL )
	{
		fprintf(stderr, "error in opening file %s for reading.\n" , filename);
		exit(EXIT_FAILURE);
	}
	else
	{
		return pfile;
	}
}

static bool InputWord(FILE *pfile, char *buffer)
{
	char ch;
	int i;
	/*find the first alphabetic char*/
	while( !isalpha(ch=getc(pfile)) )
	{
		if( ch==EOF )	//special treatment for EOF
		{
			break;
		}
		else
		{
			continue;
		}
	}
	/*fill buffer, until `i' reaching maximum or encountering non-alphabetic char. Don't forget the first char which is generated in the while loop above*/
	for( i=0 ; i<MWLEN-1 && isalpha(ch) ; i++ )
	{
		buffer[i]=ch;
		ch=getc(pfile);
	}
	buffer[i]='\0';		//terminate string with null
	/*if the non-alphabetic char is EOF, signal the main() to end input*/
	if( ch!=EOF )
	{
		return true;
	}
	else
	{
		return false;
	}
}

static void ToLower(char *str)
{
	while( *str!='\0' )
	{
		*str=tolower(*str);
		str++;
	}
}

static void DisplayCounts(Item item)
{
	/*here for uniformity, we construct format string dynamically at runtime using sprintf()*/
	char format[10];
	sprintf(format, "%%%ds:%%-d\n" , MWLEN);
	printf( format , item.pword, item.count );
}
