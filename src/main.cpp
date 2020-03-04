#include <iostream> 
#include <string>
#include <list> 
#include "tokenizador.h"

using namespace std;

void
print_tokens(const list<string>& tokens)
{
        cout << "[";
        for (const string& token : tokens)
                cout << token << "|";
        cout << "\b]" << endl;
}

int
main()
{
        Tokenizador tokenizador;
        tokenizador.CasosEspeciales(true);
        tokenizador.PasarAminuscSinAcentos(true);
        tokenizador.TokenizarDirectorio("./corpus");
        return 0;
}
