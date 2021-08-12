syn region dirtyStr start=/"/ end=/"/ skip=/\\"/ contains=dirtyEscape
syn match dirtyNum /\<-\=\d\+\(.\d\+\)\=\>/
syn match dirtyBool /\<[TF]\>/
syn match dirtySym /[@$?|~%]/
syn match dirtySym /:)/
syn match dirtyErr /:(/
syn match dirtyOp /[=+<>\#!]/
syn match dirtyOp /+=/
syn match dirtyOp /-=/
syn match dirtyOp /@>/
syn match dirtyOp /@^/
syn match dirtyOp /\~>/
syn match dirtyOp /\.\./
syn match dirtyOp /||/
syn match dirtyOp /&&/
syn match dirtyScope /[{}[\]()]/
syn match dirtyCall /\w\+\(\s*(.*)\)\@=/
syn match dirtyFunc /\(\~\s*\)\@<=\w\+\(\s*(.*)\)\@=/
syn keyword dirtyTodo contained TODO
syn match dirtyShebang /\%^#!.*/
syn match dirtyComment /--.*$/ contains=dirtyTodo
syn match dirtyEscape /\\n/ contained
syn match dirtyEscape /\\x\x\{2}/ contained
syn match dirtyEscape /\\u\x\{4}/ contained
syn match dirtyEscape /\\U\x\{6}/ contained
syn region dirtyCommentBlock start=/---/ end=/---/ contains=dirtyTodo

hi def link dirtyTodo Todo
hi def link dirtySym Statement
hi def link dirtyErr ErrorMsg
hi def link dirtyStr String
hi def link dirtyNum Number
hi def link dirtyBool Number
hi def link dirtyOp Operator
" hi def link dirtyCall PreProc
" hi def link dirtyFunc Function
hi def link dirtyScope Function
hi def link dirtyShebang Comment
hi def link dirtyEscape SpecialChar
hi def link dirtyComment Comment
hi def link dirtyCommentBlock Comment
