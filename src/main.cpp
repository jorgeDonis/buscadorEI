#include  "tokenizador.h"
#include <iostream>
#include <math.h>

using namespace std;

int main()
{
    Tokenizador tokenizador;
    // int prueba[100] = {225, 225, 225, 32, 225, 255};
    // char prueba_char[100];
    // for (int i = 0; i < 6; i++)
    //     prueba_char[i] = prueba[i];
    // string foo(prueba_char);
    // cout << "string = " << foo << endl;
    // list<string> tokens = list<string>();
    bool execution_is_right = tokenizador.TokenizarListaFicheros("listaficheros.txt");
    if (execution_is_right)
        cout << "Execution is OK" << endl;
    else
        cout << "Execution is not OK" << endl;
    // cout << "[";
    // for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
    //     cout << *it << "|";
    // cout << "\b]" << endl;
}
