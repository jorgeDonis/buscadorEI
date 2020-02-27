#include  "tokenizador.h"
#include <iostream>
#include <math.h>

using namespace std;



int main()
{
    Tokenizador tokenizador("", true,false);

    list<string> tokens;
    string corpus = "3..2 4,,,,5 ..35";
    tokenizador.Tokenizar(corpus, tokens);
    cout << "[";
    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        cout << *it << "|";
    cout << "\b]" << endl;
}
