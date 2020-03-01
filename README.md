# buscadorEI

Buscador realizado para la asignatura Explotación de la Infromación del Departamento de Lenguajes y Sistemas Informáticos
de la Escuela Politécnica Superior.

Se compone de: 
* **Crawler**: No implementado. Se trabaja sobre un corpus estático. Podría ser **wget**.
* **Tokenizador**: Separa una cadena en distintas palabras o unidades mínimas de información (URL's, multipalabras...).
* **Indexador**: Construye un índice con el corpus que almacena el número de ocurrencias de cada token en cada documento.
* **Buscador**: Encuentra el resultado más relevante basándose en el índice previamente construido.

## Autómata del tokenizador
![tokenizador](https://raw.githubusercontent.com/jorgeDonis/buscadorEI/master/diagrama_tokenizador.png?token=ALLEA2TVUTGQ2ILMM7VBFAS6MEOEO "Autómata tokenizador")
