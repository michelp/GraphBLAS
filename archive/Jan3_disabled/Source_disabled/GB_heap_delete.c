//------------------------------------------------------------------------------
// GB_heap_delete: delete an element in the middle of a Heap
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// OLD METHOD, disabled.
#if 0

#include "GB.h"
#include "GB_heap.h"

void GB_heap_delete
(
    int64_t p,                  // node that needs to be deleted
    GB_Element *GB_RESTRICT Heap,  // Heap [1..nheap]
    int64_t *GB_RESTRICT nheap     // the number of nodes in the Heap;
                                // decremented on output
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    ASSERT (Heap != NULL && (*nheap) >= 0) ;
    ASSERT (p >= 0 && p <= (*nheap)) ;

    //--------------------------------------------------------------------------
    // delete node p from the Heap
    //--------------------------------------------------------------------------

    // move the last node to node p and decrement the # of nodes in the Heap
    Heap [p] = Heap [(*nheap)--] ;

    // heapify node p (safely does nothing if node p was the one just deleted)
    GB_heapify (p, Heap, (*nheap)) ;
}

#endif