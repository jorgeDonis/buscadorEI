#include "indexadorHash.h"

int main()
{
    Tokenizador tok;
    tok.CasosEspeciales(true);
    tok.PasarAminuscSinAcentos(true);
    char* tokens = tok.Tokenizar("corpus_test/pagina1.txt");
    char c = ' ';
    unsigned i = 0;
    while (c != '\000')
    {
        c = tokens[i];
        i++;
        cout << c;
    }
    delete[] tokens;
    return 0;
}


