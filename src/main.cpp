#include  "tokenizador.h"
#include <iostream>
#include <math.h>

using namespace std;

int main()
{
    Tokenizador tokenizador("@.-_", true,false);

    list<string> tokens;
    string corpus = "-UA@iuii.ua.es @p1 p2 ";
    tokenizador.Tokenizar(corpus, tokens);
    cout << "[";
    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        cout << *it << "|";
    cout << "\b]" << endl;
}
