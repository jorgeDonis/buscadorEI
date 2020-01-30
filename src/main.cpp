#include  "tokenizador.h"
#include <iostream>

using namespace std;

int main()
{
    Tokenizador tokenizador("abcd abdc", false, true);
    cout << tokenizador << endl;
    return 0;
}
