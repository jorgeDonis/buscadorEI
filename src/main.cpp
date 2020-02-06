#include  "tokenizador.h"
#include <iostream>

using namespace std;

int main()
{
    Tokenizador tokenizador;
    string prueba = "puede ser\"que no vaya tan horrendamente";
    list<string> tokens = list<string>();
    tokenizador.Tokenizar(prueba, tokens);
    cout << "[";
    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        cout << *it << "|";
    cout << "\b]" << endl;
    return 0;
}
