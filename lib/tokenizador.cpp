#include "tokenizador.h"
#include <fstream>
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

void Tokenizador::init_mapa_acentos()
{

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
    delimiters_set.insert(delimiters.begin(), delimiters.end());
    casosEspeciales = Tokenizador::CASOS_ESPECIALES_DEFAULT;
    pasarAminuscSinAcentos = Tokenizador::PASAR_MINUSC_DEFAULT;
}

Tokenizador::Tokenizador(const string& delimitadoresPalabra, const bool& kCasosEspeciales,
                         const bool& minuscSinAcentos)
{
    delimiters = delimitadoresPalabra;
    delimiters_set.insert(delimiters.begin(), delimiters.end());
    remove_duplicate_string(delimiters);
    casosEspeciales = kCasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
}

void Tokenizador::copy_values(const Tokenizador& tokenizador)
{
    this->delimiters = tokenizador.delimiters;
    delimiters_set.insert(delimiters.begin(), delimiters.end());
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

bool Tokenizador::is_delimiter(const char& foo) const
{
    return (delimiters_set.find(foo) != delimiters_set.end());
}



void Tokenizador::Tokenizar(string& str, list<string>& tokens) const
{
    //TODO: casos especiales
    if (pasarAminuscSinAcentos)
    tokens.erase(tokens.begin(), tokens.end());
    string::iterator izquierda = str.begin();
    string::iterator derecha = str.begin();
    while (derecha != str.end())
    {
        while (izquierda != str.end() && is_delimiter(*izquierda))
            izquierda++;
        derecha = izquierda;
        while (derecha != str.end() && !is_delimiter(*derecha))
            derecha++;
        tokens.push_back(string(izquierda, derecha));
        izquierda = derecha + 1;
    }
}

bool Tokenizador::Tokenizar(const string& input_filename, const string& output_filename) const
{
    //TODO: optimizar con https://gist.github.com/marcetcheverry/991042
    ifstream ifs(input_filename);
    list<string> tokens;
    if (ifs.is_open())
    {
        string str;
        ifs.seekg(0, ios::end);
        str.reserve(ifs.tellg());
        ifs.seekg(0, ios::beg);
        str.assign(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
        Tokenizar(str, tokens);
    }
    else
    {
        cerr << "ERROR: No existe el archivo: " << input_filename << endl;
        return false;
    }
    ofstream ofs(output_filename);
    if (ofs.is_open())
    {

    }
    else
    {
        cerr << "ERROR: Error al crear el fichero: " << output_filename << endl;
        return false;
    }
    return true;
}