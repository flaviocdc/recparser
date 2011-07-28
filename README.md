MONGA - Parser recursivo e geração de codigo LLVM 
=================================================

Projeto da disciplina Compiladores 2, ministrada por Fabio Mascarenhas em 2011/1.

Podemos dividir o projeto nas seguintes partes:

* Parser recursivo (impl. a mão) e type checker
* Geração do *control flow graph* (CFG) a partir da *AST* gerada pelo parser
* Transformação dos *CFGs* para formato *single static assignment* (SSA)
* Geração de código *LLVM*   

Dentro da pasta `exemplos` existem arquivos que foram sendo usados durante a confecção do projeto para testar diversas funcionalidades necessárias a implementação. *Em sua grande maioria os algoritmos podem fazer pouco ou nenhum sentido.* Para exemplos melhores, ver o diretório `exemplos-reais`

Hacking
-------

* parser e type checker - `ast/`
* geração do cfg - `cfg/`
* transformação ssa - `ssa/`

Para compilar o projeto basta um `make`.
O build gera 2 executáveis:

* `compiler <arquivo>` - executa o parse, monta a AST e depois *pretty print* a AST
* `cfg-llvm <arquivo>` - *parse -> ast -> cfg -> ssa*

Utilitários:

* `ssa.sh <arquivo>` - usa o clang/llvm para *cuspir* LLVM-IR (excelente para verificar se a saída do `cfg-llvm`)

Utilização
----------

Infelizmente não tive tempo de implementar chamadas de função externas (*printf*, ...), então a única maneira de verificar a saída de um algoritmo é a seguinte:

* criar um arquivo (ex: fib.m) que tenha uma função "int monga()" definida. O resultado dessa função será impresso na tela posteriormente.
* executar `./monga.sh fib.m` . Isso irá gerar um arquivo `gen/fib` que pode ser executado.
* executar `gen/fib`
* live happilly ever after
