#include "indexadorHash.h"

int main()
{
    Tokenizador tok; 
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(true);
    IndexadorHash indexador("stopwords.txt", " \n.\t,-", false, true, "", 0, false, true);
    indexador.IndexarPregunta("es a pregunta buena buena buenísima genial");
    indexador.ImprimirIndexacionPregunta();
    return 0;
}


