//------------------------------------------------------------------------------
// GxB_Vector_export_Bitmap: export a bitmap vector
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "GB_export.h"

#define GB_FREE_ALL ;

GrB_Info GxB_Vector_export_Bitmap   // export and free a bitmap vector
(
    GrB_Vector *v,      // handle of vector to export and free
    GrB_Type *type,     // type of vector exported
    GrB_Index *n,       // length of the vector
    GrB_Index *nvals,   // # of entries
    int8_t **vb,        // bitmap, size n
    void **vx,          // values, size n
    const GrB_Descriptor desc
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GxB_Vector_export_Bitmap (&v, &type, &n, &nvals,"
        " &vb, &vx, desc)") ;
    GB_BURBLE_START ("GxB_Vector_export_Bitmap") ;
    GB_RETURN_IF_NULL (v) ;
    GB_GET_DESCRIPTOR (info, desc, xx1, xx2, xx3, xx4, xx5, xx6) ;

    //--------------------------------------------------------------------------
    // finish any pending work
    //--------------------------------------------------------------------------

    GB_MATRIX_WAIT (*v) ;

    //--------------------------------------------------------------------------
    // ensure the vector is bitmap CSC
    //--------------------------------------------------------------------------

    ASSERT ((*v)->is_csc) ;
    GB_OK (GB_convert_any_to_bitmap ((GrB_Matrix) *v, Context)) ;
    ASSERT (GB_IS_BITMAP (*v)) ;

    //--------------------------------------------------------------------------
    // export the vector
    //--------------------------------------------------------------------------

    GrB_Index vdim ;
    info = GB_export ((GrB_Matrix *) v, type, n, &vdim,
        NULL, nvals, NULL, NULL, NULL,
        NULL, NULL, vb, NULL, vx, NULL, NULL, Context) ;
    GB_BURBLE_END ;
    return (info) ;
}
