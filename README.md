# buscadorEI
Buscador sobre un corpus estático

## Indexador

El propósito es leer una lista de tokens (tokenizada con *Tokenizador*) y generar un índice que contenga
distintas frecuencias de aparación en relación con los términos y el corpus. El índice ha de ser leído con posterioridad
de manera muy rápida, con lo que se emplea una tabla hash.
