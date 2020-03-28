#include "indexadorHash.h"

int main()
{
    Tokenizador tok; 
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(true);
    IndexadorHash indexador("stopwords.txt", " \n.\t,-", false, true, "", 0, false, true);
    indexador.IndexarDirectorio("corpus_test");
    cout << indexador << endl;
    indexador.ImprimirIndexacion();
    return 0;
}


