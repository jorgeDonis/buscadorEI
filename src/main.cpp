#include "indexadorHash.h"

int main()
{
    Tokenizador tok; 
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(true);
    IndexadorHash indexador("stopwords.txt", " \n.\t,-", false, true, "", 0, false, true);
    indexador.IndexarDirectorio("./corpus_test");
    indexador.ImprimirIndexacion();
    indexador.ListarTerminos("./corpus_test/pagina1.txt");
    return 0;
}


