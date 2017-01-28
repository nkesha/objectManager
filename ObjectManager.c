//-----------------------------------------
// NAME: chris Nkeshimana 
// STUDENT NUMBER: 7735006
// COURSE: COMP 2160
// INSTRUCTOR: Michael Zapp
// ASSIGNMENT: Assignment 4
// 
// REMARKS: Object Manager
//
//-----------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ObjectManager.h"


typedef struct NODE Node;
typedef struct INDEX Index;

unsigned char * bufferOne;
unsigned char * bufferTwo;

//buffer to be used.
unsigned char *bufferInUse = NULL;

Node* compact();

 
static int num_nodes = 0;

struct INDEX
{
    int refID;
    int size;
    int count;
    int start;
};

struct NODE
{
    Index index;
    Node *next;
};

//-------------------------------------------------------------------------------------
// VARIABLES
//-------------------------------------------------------------------------------------

static Node *top = NULL;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

static void validateState()
{
    
#ifndef NDEBUG

    Node *curr = top;
    Node *prev = NULL;
    int count_nodes = 0;
    
#endif

    if (num_nodes == 0)
        assert( top == NULL);
        
    else if (num_nodes == 1)
        assert ( top ->next == NULL);
    
    else
        assert (top != NULL && top->next != NULL);
        
    //since we are using 2 buffers the refID should definetely be in order.
    
#ifndef NDEBUG
    while (curr)
    {
        count_nodes++;
        prev = curr;
        curr = curr->next;
        
        if (curr)
        {
            //check if the refID are in order
            assert (prev->index.refID < curr->index.refID);
            prev = curr;
        }
        
    }
    assert (count_nodes == num_nodes);
#endif
}



//-----------------------------------------------------------------------
//              INSERT OBJECT
//----------------------------------------------------------------------


Ref insertObject( const int size )
{
    
    
    Ref result = NULL_REF;
    Node *newNode = NULL;
    Node *curr = top;
    Node *prev = NULL;
    
    validateState();
    assert (size > 0);
    
    newNode = (Node*) malloc (sizeof(Node));
    assert(newNode != NULL);
    
    
    if (newNode && size > 0)
    {
        
        assert( curr != NULL || ( curr == NULL && top == NULL ) );
        
        if (curr == NULL && top == NULL)
        {
            
            //first item, let me check the space.
            
            if (size <= MEMORY_SIZE)
            {
                
                newNode->index.refID = 1;
                newNode->index.size = size;
                newNode->index.count = 1;
                newNode->index.start = 0;
                
                top = newNode;
                newNode->next = NULL;
                
                //Success here.
                result = newNode->index.refID;
                
                num_nodes++;
                
            }
            else 
            {
                printf ("Exceeds Memory");
                result = NULL_REF;
            }
            
            // IF NOT SUCCESS DO WE CALL GC (on the 1st ITEM)?
        }
        else
        {
            assert (curr != NULL);
            
            //now we need to figure out where to add
            while ( curr != NULL )
            {
                prev = curr;
                curr = curr->next;
            }
            
            
            if (prev == NULL)
            {
                //inserting at the top of the list
                assert (curr == top);
                
                //need to check if there is enough space remaining
                if ((curr->index.start + curr->index.size + size) <= MEMORY_SIZE)
                {
                    
                    //then you can create the new track index for that object
                    
                    newNode->index.refID = curr->index.refID + 1;
                    newNode->index.size = size;
                    newNode->index.count = 1;
                    newNode->index.start = curr->index.start + curr->index.size;
                
                    newNode->next = NULL;
                    curr->next = newNode;
                    
                    //success here return refID
                    result = newNode->index.refID;
                    num_nodes++;
                }
                // IF NOT SUCCESS DO WE CALL GC (on the 2 ITEMS)?

            }
            else 
            {
                assert(curr != top);
                
                //inserting at the end of table( condition ici ).
                
                if ((prev->index.start + prev->index.size + size) <= MEMORY_SIZE)
                {
                    
                    newNode->index.refID = prev->index.refID + 1;
                    newNode->index.size = size;
                    newNode->index.count = 1;
                    newNode->index.start = prev->index.start + prev->index.size;
                
                    newNode->next = NULL;
                    prev->next = newNode;
                    
                    //Success here
                    result = newNode->index.refID;
                    num_nodes++;
                    
                }
                else
                {
                    printf("Let me Try and Compact memory for you\n");
					
					printf("\nNumber of objects = %d", num_nodes);
					printf("\nNumber of current bytes in Use = %d", (prev->index.start + prev->index.size));
                    
                    Node *lastNode = NULL;
                    
                    //call G.C here  (should return a pointer to the last Node in index(for the newBuffer)
                    
                    lastNode = compact();
                    
                    dumpPool();
                    
                    if(lastNode != NULL)
                    {
                        if ((lastNode->index.start + lastNode->index.size + size) <= MEMORY_SIZE)
                        {
                            
                            newNode->index.refID = lastNode->index.refID + 1;
                            newNode->index.size = size;
                            newNode->index.count = 1;
                            newNode->index.start = lastNode->index.start + lastNode->index.size;
                            
                            
                            lastNode->next = newNode;
                            newNode->next = NULL;
                    
                    
                            //success
                            result = newNode->index.refID;
							num_nodes++;
                            
                        }
                        else
                        {
                            result = NULL_REF;
                        }
                        
                    }
                    else 
                    {
                        validateState();
                        printf ("NO SPACE AFTER COMPACTING");
                        exit(0);
                    }
                }
            }
        }
        
        validateState ();
    }
    else 
    {
        free (newNode);
        newNode = NULL;
    }
    
    return result;
}


//---------------------------------------------------------------------------
//      RETRIEVE OBJECTS
//---------------------------------------------------------------------------

// returns a pointer to the object being requested given by the reference id

void *retrieveObject( const Ref ref )
{
    unsigned char *ptrObject = NULL;
    bool found = false;
    
    validateState();
    
    assert (ref > 0);
	if (ref > 0)
	{
		Node *curr = top;
    
		while(curr && !found)
		{
			if (curr->index.refID == ref)
			{
				//need to check the buffer in use (either BufferOne or Two)
				ptrObject = bufferInUse + curr->index.start; 
            
				found = true;
			}
			curr = curr->next;
		}
		validateState();
	}
	
	return ptrObject;
}


//-----------------------------------------------------------------------
//              ADD REFERENCE
//-----------------------------------------------------------------------


// update our index to indicate that we have another reference to the given object
void addReference( const Ref ref )
{
    validateState();
    
    assert (ref > 0);
    if (ref > 0)
	{	
		bool found = false;
		Node *curr = top;
		int search_count = 0; //how far to make sure we don't go outOfBounds
    
    
		//why you did not check the pointer here ??
		while (curr != NULL && !found)
		{
			if (curr->index.refID == ref)
			{	
				curr->index.count++;
				found = true; 
            
				assert (search_count <= num_nodes);
			} 
			else
			{
				curr = curr->next;
				search_count++;
			}
		}
    
		//post conditions here quite smart here.
		assert( found || (search_count == num_nodes) );
	}
	validateState();
}

//-----------------------------------------------------------------------
//              DROP REFERENCE
//-----------------------------------------------------------------------

void dropReference( const Ref ref )
{
    validateState();
    assert (ref > 0);
	if (ref > 0)
    {
		bool found = false;
		Node *curr = top;
		int search_count = 0; //how far to make sure we don't go outOfBounds
    
    
		//why you did not check the pointer here ??
		while (curr != NULL && !found)
		{
			if (curr->index.refID == ref)
			{
				curr->index.count--;
				found = true; 
            
				assert (search_count <= num_nodes);
			} 
			else
			{
				curr = curr->next;
				search_count++;
			}
		}
    
		//post conditions here quite smart here.
		assert( found || (search_count == num_nodes) ); 
    }
    validateState();

}


//------------------------------------------------------------------------
//      DESTROY POOL
//------------------------------------------------------------------------

// clean up the object manager (before exiting)
void destroyPool()
{
    validateState();
    
    Node *curr = top;
  
    while ( top != NULL )
    {
        top = top->next;
        free( curr );
    
        curr = top;
    
        num_nodes--;
    }
    
    validateState();
    
    free (bufferOne);
    free (bufferTwo);
    
}


//-----------------------------------------------------------------------
//              COMPACT FUNC
//-----------------------------------------------------------------------

// Returns a pointer to the last Node for our newBuffer.

Node* compact()
{
    validateState();
    
    Node *currOne = top;
    Node *prevOne = NULL;
    
    int start = 0; // tracker for the second buffer
	int bytesCollected = 0;
    
    unsigned char *bufferToUse = NULL; //where to transfer the memory

    
    // check which buffer we are dealing with
    if (bufferInUse == bufferOne)
        bufferToUse = bufferTwo;
    
    else
        bufferToUse = bufferOne;
    
	
    while (currOne != NULL)
    {
        //referenced objects
        assert (currOne != NULL);
		
        if (currOne->index.count > 0)
        {
            //copy them in the appropriate buffer.
            
            bufferToUse[start] = bufferInUse[currOne->index.start];
            
            //update here my index table
            currOne->index.start = start;
            
            //update available space in newBuffer
            start = start + currOne->index.size; 
            
            
            
            prevOne = currOne;
            currOne = currOne->next;
    
        }
        //non-referenced object / first in the tracking table.
        
        else if (currOne->index.count <= 0 && prevOne == NULL)
        {
            
            top = currOne->next;
			bytesCollected += currOne->index.size; 
            free(currOne);
        
            currOne = top;
			
			num_nodes--;

        }
        // non-referenced object / not the first element
        
        else if (currOne->index.count <= 0 && prevOne != NULL)
        {
            
            prevOne->next = currOne->next;
			bytesCollected += currOne->index.size; 
            free(currOne);
            
            //update curr here
            currOne = prevOne->next;
			num_nodes--;
        }
    }
    
	if (prevOne != NULL)
	{
		
		printf("\nNumber of bytes collected = %d\n\n", bytesCollected);
    }
    
    validateState();
    
	return prevOne;
}


//-----------------------------------------------------------------------
//              INIT POOL
//----------------------------------------------------------------------

void initPool()
{
    printf ("\nSIZE OF YOUR MEMORY:  %d\n", MEMORY_SIZE);
    bufferOne = (unsigned char *) malloc (sizeof(MEMORY_SIZE));
    bufferTwo = (unsigned char *) malloc (sizeof(MEMORY_SIZE));
    
    bufferInUse = bufferOne;
}

//-----------------------------------------------------------------------
//              DUMP POOL
//-----------------------------------------------------------------------

// This function traverses the index and prints the info in each entry corresponding to a block of allocated memory.
// You should print the block's reference id, it's starting address, and it's size (in bytes).
void dumpPool()
{
    Node *curr = top;
    
    validateState();
    
    while (curr != NULL)
    {

        printf("ID: %d S: %d C: %d St: %d\n", curr->index.refID,
                                                curr->index.size, 
                                                curr->index.count, 
                                                curr->index.start);
        curr = curr->next;
    }
    validateState();
}













