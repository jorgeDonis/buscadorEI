#include  "tokenizador.h"
#include <iostream>
#include <math.h>

using namespace std;

int main()
{
    Tokenizador tokenizador;
    tokenizador.PasarAminuscSinAcentos(false);
    tokenizador.CasosEspeciales(true);
    tokenizador.DelimitadoresPalabra(":_/.,@");
    tokenizador.PasarAminuscSinAcentos(true);

    list<string> tokens;
    string corpus = "shttps:g.es";
    tokenizador.Tokenizar(corpus, tokens);

    cout << "[";
    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        cout << *it << "|";
    cout << "\b]" << endl;
}
