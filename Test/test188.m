% function test188
%TEST188 test concat

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
% SPDX-License-Identifier: Apache-2.0

fprintf ('test188 ----------- C = concat (Tiles)\n') ;

[~, ~, ~, types, ~, ~] = GB_spec_opsall ;
types = types.all ;

rng ('default') ;
d = 0.2 ;

n1 = 2 ;
n2 = 4 ;

for ka = 1:length (types)
    fprintf ('.') ;
    atype = types {ka} ;
    A1 = GB_spec_random (n1, n1, d, 128, atype) ;
    A2 = GB_spec_random (n1, n2, d, 128, atype) ;
    A3 = GB_spec_random (n2, n1, d, 128, atype) ;
    A4 = GB_spec_random (n2, n2, d, 128, atype) ;

    Tiles = cell (2,2) ;
    Tiles {1,1} = A1 ;
    Tiles {1,2} = A2 ;
    Tiles {2,1} = A3 ;
    Tiles {2,2} = A4 ;

    for kc = 1:length (types)
        ctype = types {kc} ;
        for fmt = 0:1
            fprintf ('atype %s ctype %s fmt %d\n', atype, ctype, fmt) ;
            C1 = GB_mex_concat  (Tiles, ctype, fmt) ;
            C2 = GB_spec_concat (Tiles, ctype) ;
            C1
            C2
            GB_spec_compare (C1, C2) ;
        end
    end
end

fprintf ('\n') ;
GrB.burble (0) ;
fprintf ('test188: all tests passed\n') ;
