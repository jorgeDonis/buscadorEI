#include  "tokenizador.h"
#include <iostream>

using namespace std;

int main()
{
    Tokenizador tokenizador("1425 00002019", false, true);
    cout << tokenizador << endl;
    return 0;
}
