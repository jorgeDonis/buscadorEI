#include "indexadorHash.h"

int main()
{
    Tokenizador tok; 
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(true);
    IndexadorHash indexador("stopwords.txt", " \n.\t,-", false, true, "", 0, false, true);
    indexador.IndexarDirectorio("./corpus_test");
    indexador.ImprimirIndexacion();
    indexador.GuardarIndexacion();
    IndexadorHash indexador2("/home/jorge/buscadorEI");
    indexador2.ImprimirIndexacion();
    return 0;
}


