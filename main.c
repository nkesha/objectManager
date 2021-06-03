
// NAME: chris Nkeshimana 





#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ObjectManager.h"

#define SIZES 5
const int  sizeArray[SIZES] = {100, 200, 300, 400, 523288};

//-------------------------------------------------------------------------------------
// VARIABLES
//--------------------------------------------------------------------------------------

static int testsFailed;
static int testsExecuted;



//-------------------------------------------------------------------------------------
// PROTOTYPES
//--------------------------------------------------------------------------------------

void testInsert ();
void testFunc ();
void testAdd_drop ();
void testReturn (Ref one, Ref two);

void initTests ();

//-------------------------------------------------------------------------------------
// FUNCTIONS
//--------------------------------------------------------------------------------------


int main( int argc, char *argv[] )
{
    initPool();
    initTests ();
    testInsert();
	testAdd_drop ();
    testFunc();
    destroyPool();


    printf("Number of tests passed: %d\n", (testsExecuted - testsFailed));
    printf("Number of tests failed: %d\n", testsFailed);
}



void testInsert()
{
	printf( "\nTESTING INSERTION\n");
    //Fill the buffer completely
    for (int i = 0; i < SIZES; i++)
    {
        if (insertObject(sizeArray[i]) != (i+1))
        {
            printf( "FAILED: did not insert object  %d into the table.\n", sizeArray[i] );
        }
    }
	dumpPool();
}

void testAdd_drop ()
{
	printf( "\nTESTING ADDREFERENCE && DROPREFERENCE\n");
	//add end of table && first element
	
	addReference(5);
	addReference(1);
	
	dumpPool();
	
	dropReference(1);
	dropReference(5);
	
	printf("\nINDEX SHOULD BE UNCHANGED\n");
	
	dumpPool();
	
	
}
void testFunc ()
{
	printf( "\nTESTING COMBINATION OF FUNCTIONALITY\n");
    //now I have 5 objects filling all the space.
	
    dropReference (5);
    testReturn (insertObject(sizeArray[4]), 5);
    dumpPool();
	
	//what if G.C can't find a spot
	
	testReturn (insertObject(sizeArray[0]), 0);
	
	//addReference testing
	addReference(5);
	addReference(1);
	
	//dropReference testing
	dropReference(5);
	dropReference(1);
	
	dumpPool();
	
}


void initTests ()
{
    testsFailed = 0;
    testsExecuted = 0;
}


void testReturn (Ref one, Ref two)
{
    if (one == two)
    {
        printf("Passed\n");
    }
    else 
    {
        printf("Failed\n");
        testsFailed++;
    }
    testsExecuted++;
}






