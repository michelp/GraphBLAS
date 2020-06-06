function C = bitshift (A, B, assumedtype)
%BITSHIFT bitwise left and right shift.
% C = bitshift (A,B) computes the bitwise shift of A; if B > 0 then A is
% shifted left by B bits, and if B < 0 then A is shifted right by -B bits.
% If either A or B are scalars, they are expanded to the pattern of the
% other matrix.  C has the pattern of A (after expansion, if needed).
%
% With a third parameter, C = bitshift (A,B,assumedtype) provides a data
% type to convert A to if it is a floating-point type.  If A already has
% an integer type, then it is not modified.  Otherwise, A is converted to
% assumedtype, which can be 'int8', 'int16', 'int32', 'int64', 'uint8',
% 'uint16', 'uint32' or 'uint64'.  The default is 'uint64'.
%
% The input matrices must be real, and may be either GraphBLAS and/or
% MATLAB matrices, in any combination.  C is returned as a GraphBLAS
% matrix.  The type of C is the same as A, after any conversion to
% assumedtype, if needed.
%
% Example:
%
%   TODO example
%
% See also GrB/bitor, GrB/bitand, GrB/bitxor, GrB/bitcmp, GrB/bitget,
% GrB/bitset, GrB/bitclr.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

if (nargin < 3)
    assumedtype = 'uint64' ;
end

C = GrB (gb_bitwise ('bitshift', A, B, assumedtype)) ;

