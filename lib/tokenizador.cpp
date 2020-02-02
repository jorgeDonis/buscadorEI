#include "tokenizador.h"
#include <algorithm>

using namespace std;

const bool Tokenizador::CASOS_ESPECIALES_DEFAULT = true;
const bool Tokenizador::PASAR_MINUSC_DEFAULT = false;
const string Tokenizador::DEFAULT_DELIMITERS = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";

ostream& operator<<(ostream& os, const Tokenizador& tokenizador)
{
    os << "DELIMITADORES: " << tokenizador.delimiters << " TRATA CASOS ESPECIALES: ";
    os << tokenizador.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: ";
    os << tokenizador.pasarAminuscSinAcentos;
    return os;
}

void remove_duplicate_string(string& str)
{
    if (str.length() > 1)
    {
        string::iterator it = str.begin();
        string::iterator found_it;
        while (it != str.end())
        {
            found_it = it;
            while (found_it != str.end())
            {
                found_it = find(it + 1, str.end(), *it);
                if (found_it != str.end())
                    str.erase(found_it);
            }
            it++;
        }
    }
}

Tokenizador::Tokenizador()
{
    delimiters = Tokenizador::DEFAULT_DELIMITERS;
    casosEspeciales = Tokenizador::CASOS_ESPECIALES_DEFAULT;
    pasarAminuscSinAcentos = Tokenizador::PASAR_MINUSC_DEFAULT;
}

Tokenizador::Tokenizador(const string& delimitadoresPalabra, const bool& kCasosEspeciales,
                         const bool& minuscSinAcentos)
{
    delimiters = delimitadoresPalabra;
    remove_duplicate_string(delimiters);
    casosEspeciales = kCasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
}

void Tokenizador::copy_values(const Tokenizador& tokenizador)
{
    this->delimiters = tokenizador.delimiters;
    this->pasarAminuscSinAcentos = tokenizador.pasarAminuscSinAcentos;
    this->casosEspeciales = tokenizador.casosEspeciales;
}

Tokenizador::Tokenizador(const Tokenizador& tokenizador)
{
    copy_values(tokenizador);
}

Tokenizador& Tokenizador::operator=(const Tokenizador& tokenizador)
{
    if (this != &tokenizador)
        copy_values(tokenizador);
    return *this;
}