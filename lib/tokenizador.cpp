#include "tokenizador.h"
#include <fstream>
#include <algorithm>

using namespace std;

const bool Tokenizador::CASOS_ESPECIALES_DEFAULT = true;
const bool Tokenizador::PASAR_MINUSC_DEFAULT = false;
const string Tokenizador::DEFAULT_DELIMITERS = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
const short Tokenizador::MAPA_ACENTOS[256] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 
    62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 
    115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 
    94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 
    115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 
    125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 
    145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 
    155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 
    175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 
    185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 
    205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 
    215, 216, 217, 218, 219, 220, 221, 222, 223, 97, 
    97, 226, 227, 228, 229, 230, 231, 101, 101, 234, 
    235, 105, 105, 238, 239, 240, 241, 111, 111, 244, 
    245, 246, 247, 248, 117, 117, 251, 252, 253, 254, 
    255, 
};

void Tokenizador::minusc_sin_acentos(string& foo)
{
    for (string::iterator it = foo.begin(); it != foo.end(); ++it)
        *it = Tokenizador::MAPA_ACENTOS[*it];
}

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
    //TODO: pasar a minusculas sin acentos
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
        ifs.close();
    }
    else
    {
        cerr << "ERROR: No existe el archivo: " << input_filename << endl;
        return false;
    }
    ofstream ofs(output_filename);
    if (ofs.is_open())
    {
        for (string token : tokens)
            ofs << token << endl;
        ofs.close();
    }
    else
    {
        cerr << "ERROR: Error al crear el fichero: " << output_filename << endl;
        return false;
    }
    return true;
}