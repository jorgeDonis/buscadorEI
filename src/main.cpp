#include "indexadorHash.h"

int main()
{
    Tokenizador tok;
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(true);
    IndexadorHash indexador("stopwords.txt", " \n.\t,", true, true, "", 0, false, true);
    indexador.IndexarDirectorio("corpus_test");
    return 0;
}


