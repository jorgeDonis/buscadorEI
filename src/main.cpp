#include  "tokenizador.h"
#include <iostream>
#include <math.h>

using namespace std;

int main()
{
    // Tokenizador tokenizador;
    // string prueba = "puede ser\"que no vaya tan horrendamente";
    // list<string> tokens = list<string>();
    // tokenizador.Tokenizar(prueba, tokens);
    // cout << "[";
    // for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
    //     cout << *it << "|";
    // cout << "\b]" << endl;
    int prueba[100] = {232, 233, 225, 224, 250, 249};
    char prueba_char[100];
    for (int i = 0; i < 6; i++)
        prueba_char[i] = prueba[i];
    string foo(prueba_char);
    cout << "string = " << foo << endl;
    Tokenizador::minusc_sin_acentos(foo);
    cout << "string = " << foo << endl;
}
