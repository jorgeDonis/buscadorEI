#include  "tokenizador.h"
#include <iostream>

using namespace std;

int main()
{
    Tokenizador tokenizador = Tokenizador();
    string prueba = "así ";
    list<string> tokens = list<string>();
    tokenizador.Tokenizar(prueba, tokens);
    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        cout << *it << endl;
    return 0;
}
