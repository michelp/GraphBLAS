function C = flipedge (G, varargin)
%FLIPEDGE Flip edge directions.
% C = flipedge (G) returns a directed gbgraph C, which contains the same edges
% as the directed gbgraph G, but with reversed directions.
%
% C = flipedge (G, S, T) reverse the direction of edges (i,j) where i is in
% the list S, and j is in the list T.
%
% C = flipedge (G, I) is the same as C = flipedge (G, I, I).
%
% See also digraph/flipedge.

% TODO tests

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
% http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

if (~isdirected (G))
    error ('G must be a directed graph') ;
end

if (nargin == 1)
    % C = flipedge (G), flip all edges
    C = gbgraph (G', 'directed', false) ;
    return ;
elseif (nargin == 2)
    S = varargin {2} ;
    T = varargin {2} ;
elseif (nargin == 3)
    S = varargin {2} ;
    T = varargin {3} ;
else
    error ('usage: C = flipedge (G, S, T)') ;
end

if (~iscolumn (S))
    S = S (:) ;
end

if (~iscolumn (T))
    T = T (:) ;
end

n = numnodes (G) ;
type = gb.type (G) ;
[I, J, X] = gb.extracttuples (G) ;

% find all edges to reverse
[~, ia, ib] = intersect ([I J], [S T], 'rows') ;

% swap I(ia) and J(ia)
s = I (ia) ;
I (ia) = J (ia) ;
J (ia) = s ;

% rebuild the graph
C = gbgraph (gb.build (I, J, X, n, n, type), 'directed', false) ;
