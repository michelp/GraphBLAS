//------------------------------------------------------------------------------
// GB_concat: concatenate an array of matrices into a single matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#define GB_FREE_WORK                    \
    GB_WERK_POP (Tile_cols, int64_t) ;  \
    GB_WERK_POP (Tile_rows, int64_t) ;

#define GB_FREE_ALL                     \
    GB_FREE_WORK ;                      \
    GB_phbix_free (C) ;

#include "GB_concat.h"

GrB_Info GB_concat                  // concatenate a 2D array of matrices
(
    GrB_Matrix C,                   // input/output matrix for results
    const GrB_Matrix *Tiles,        // 2D row-major array of size m-by-n
    const GrB_Index m,
    const GrB_Index n,
    GB_Context Context
)
{ 

    //--------------------------------------------------------------------------
    // allocate workspace
    //--------------------------------------------------------------------------

    GrB_Info info ;
    GB_WERK_DECLARE (Tile_rows, int64_t) ;
    GB_WERK_DECLARE (Tile_cols, int64_t) ;
    GB_WERK_PUSH (Tile_rows, m+1, int64_t) ;
    GB_WERK_PUSH (Tile_cols, n+1, int64_t) ;
    if (Tile_rows == NULL || Tile_cols == NULL)
    { 
        // out of memory
        GB_FREE_ALL ;
        return (GrB_OUT_OF_MEMORY) ;
    }

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    ASSERT_MATRIX_OK (C, "C input for GB_concat", GB0) ;
    for (int64_t k = 0 ; k < m*n ; k++)
    {
        GrB_Matrix A = Tiles [k] ;
        GB_RETURN_IF_NULL_OR_FAULTY (A) ;
        ASSERT_MATRIX_OK (A, "Tile[k] input for GB_concat", GB0) ;
        GB_MATRIX_WAIT (A) ;
    }

    //--------------------------------------------------------------------------
    // check the sizes and types of each tile
    //--------------------------------------------------------------------------

    bool C_is_full = true ;
    bool csc = C->is_csc ;
    GrB_Type ctype = C->type ;

    for (int64_t i = 0 ; i < m ; i++)
    {
        GrB_Matrix A = GB_TILE (Tiles, i, 0) ;
        Tile_rows [i] = GB_NROWS (A) ;
    }

    for (int64_t j = 0 ; j < n ; j++)
    {
        GrB_Matrix A = GB_TILE (Tiles, 0, j) ;
        Tile_cols [j] = GB_NCOLS (A) ;
    }

    int64_t cnz = 0 ;
    int64_t k = 0 ;
    for (int64_t i = 0 ; i < m ; i++)
    {
        for (int64_t j = 0 ; j < n ; j++)
        {
            GrB_Matrix A = GB_TILE (Tiles, i, j) ;
            // C is full only if all A(i,j) are entirely dense 
            C_is_full = C_is_full && GB_is_dense (A) ;
            int64_t nrows = GB_NROWS (A) ;
            int64_t ncols = GB_NCOLS (A) ;
            cnz += GB_NNZ (A) ;
            if (GB_IS_HYPERSPARSE (A))
            {
                k += A->nvec ;
            }
            else
            {
                k += csc ? ncols : nrows ;
            }
            GrB_Type atype = A->type ;
            if (!GB_Type_compatible (ctype, atype))
            {
                GB_FREE_ALL ;
                int64_t offset = GB_Global_print_one_based_get ( ) ? 1 : 0 ;
                GB_ERROR (GrB_DOMAIN_MISMATCH,
                    "Input matrix Tiles{%ld,%ld} of type [%s]\n"
                    "cannot be typecast to output of type [%s]\n",
                    i+offset, j+offset, atype->name, ctype->name) ;
            }
            int64_t tile_rows = Tile_rows [i] ;
            if (tile_rows != nrows)
            {
                GB_FREE_ALL ;
                int64_t offset = GB_Global_print_one_based_get ( ) ? 1 : 0 ;
                GB_ERROR (GrB_DIMENSION_MISMATCH,
                    "Input matrix Tiles{%ld,%ld} is %ld-by-%ld; its row\n"
                    "dimension must match all other matrices Tiles{%ld,:},"
                    " which is %ld\n", i+offset, j+offset, nrows, ncols,
                    i+offset, tile_rows) ;
            }
            int64_t tile_cols = Tile_cols [j] ;
            if (tile_cols != ncols)
            {
                GB_FREE_ALL ;
                int64_t offset = GB_Global_print_one_based_get ( ) ? 1 : 0 ;
                GB_ERROR (GrB_DIMENSION_MISMATCH,
                    "Input matrix Tiles{%ld,%ld} is %ld-by-%ld; its column\n"
                    "dimension must match all other matrices Tiles{:,%ld},"
                    " which is %ld\n", i+offset, j+offset, nrows, ncols,
                    j+offset, tile_cols) ;
            }
        }
    }

    // replace Tile_rows and Tile_cols with their cumulative sum
    GB_cumsum (Tile_rows, m, NULL, 1, Context) ;
    GB_cumsum (Tile_cols, n, NULL, 1, Context) ;
    int64_t cnrows = Tile_rows [m] ;
    int64_t cncols = Tile_cols [n] ;
    if (cnrows != GB_NROWS (C) || cncols != GB_NCOLS (C))
    {
        GB_FREE_ALL ;
        GB_ERROR (GrB_DIMENSION_MISMATCH,
            "C is %ld-by-%ld but Tiles{:,:} is %ld-by-%ld\n",
            GB_NROWS (C), GB_NCOLS (C), cnrows, cncols) ;
    }

    //--------------------------------------------------------------------------
    // C = concatenate (Tiles)
    //--------------------------------------------------------------------------

    if (C_is_full)
    {
        // construct C as full
        printf ("C full\n") ; GB_GOTCHA ;
        GB_OK (GB_concat_full (C, Tiles, m, n, Tile_rows, Tile_cols, Context)) ;
    }
    else if (GB_convert_sparse_to_bitmap_test (C->bitmap_switch, cnz, cnrows,
        cncols))
    {
        // construct C as bitmap
        printf ("C bitmap\n") ; GB_GOTCHA ;
        GB_OK (GB_concat_bitmap (C, cnz, Tiles, m, n, Tile_rows, Tile_cols,
            Context)) ;
    }
    else if (GB_convert_sparse_to_hyper_test (C->hyper_switch, k, C->vdim))
    {
        // construct C as hypersparse
        GB_OK (GB_concat_hyper (C, cnz, Tiles, m, n, Tile_rows, Tile_cols,
            Context)) ;
    }
    else
    {
        // construct C as sparse
        GB_OK (GB_concat_sparse (C, cnz, Tiles, m, n, Tile_rows, Tile_cols,
            Context)) ;
    }

    //--------------------------------------------------------------------------
    // conform C to its desired format and return result
    //--------------------------------------------------------------------------

    GB_FREE_WORK ;
    ASSERT_MATRIX_OK (C, "C before conform for GxB_Matrix_concat", GB0) ;
    GB_OK (GB_conform (C, Context)) ;
    ASSERT_MATRIX_OK (C, "C output for GxB_Matrix_concat", GB0) ;
    return (GrB_SUCCESS) ;
}

