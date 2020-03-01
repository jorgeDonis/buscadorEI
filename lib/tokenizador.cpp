#include "tokenizador.h"
#include <fstream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

const bool Tokenizador::CASOS_ESPECIALES_DEFAULT = true;
const bool Tokenizador::PASAR_MINUSC_DEFAULT = false;
const string Tokenizador::DEFAULT_DELIMITERS = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
const string Tokenizador::DEFAULT_FILELIST_FILENAME = ".lista_ficheros";
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
    185, 186, 187, 188, 189, 190, 191, 97, 97, 194, 
    195, 196, 197, 198, 199, 101, 101, 202, 203, 105, 
    105, 206, 207, 208, 241, 111, 111, 212, 213, 214, 
    215, 216, 117, 117, 219, 220, 221, 222, 223, 97, 
    97, 226, 227, 228, 229, 230, 231, 101, 101, 234, 
    235, 105, 105, 238, 239, 240, 241, 111, 111, 244, 
    245, 246, 247, 248, 117, 117, 251, 252, 253, 254, 
    255, 
};
string::const_iterator Estado::absolute_iterator;
Tokenizador Estado::tokenizador;
const string Estado::URL_ALLOWED_DELI = "_:/.?&-=#@";

void Tokenizador::minusc_sin_acentos(string& foo)
{
    for (int i = 0; i < foo.length(); i++)
        foo[i] = Tokenizador::MAPA_ACENTOS[(unsigned char) foo[i]];
}

bool Tokenizador::is_dir(const string& filename)
{
    struct stat buf;
    stat(filename.c_str(), &buf);
    return S_ISDIR(buf.st_mode);
}

bool Tokenizador::file_exists(const string& filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
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
    for (const char& c : delimiters)
        delimiters_set[(unsigned char) c] = 1;
    casosEspeciales = Tokenizador::CASOS_ESPECIALES_DEFAULT;
    pasarAminuscSinAcentos = Tokenizador::PASAR_MINUSC_DEFAULT;
}

Tokenizador::~Tokenizador()
{
    this->delimiters = "";
}

Tokenizador::Tokenizador(const string& delimitadoresPalabra, const bool& kCasosEspeciales,
                         const bool& minuscSinAcentos)
{
    delimiters = delimitadoresPalabra;
    remove_duplicate_string(delimiters);
    for (const char &c : delimiters)
        delimiters_set[(unsigned char) c] = 1;
    casosEspeciales = kCasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
}

void Tokenizador::copy_values(const Tokenizador& tokenizador)
{
    this->delimiters = tokenizador.delimiters;
    for (unsigned i = 0; i < 256; i++)
        delimiters_set[i] = tokenizador.delimiters_set[i];
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
    return delimiters_set[(unsigned char) foo];
}

void Tokenizador::DelimitadoresPalabra(const string& delimitadores)
{
    delimiters = delimitadores;
    remove_duplicate_string(delimiters);
    for (int i = 0; i < 256; i++)
        delimiters_set[i] = 0;
    for (const char &c : delimiters)
        delimiters_set[(unsigned char) c] = 1;
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& delimitadores_extra)
{
    for (char nuevo_delimitador : delimitadores_extra)
    {
        if (delimiters.find(nuevo_delimitador) == string::npos)
        {
            delimiters += nuevo_delimitador;
            delimiters_set[(unsigned char) nuevo_delimitador] = 1;
        }
    }
}

void Tokenizador::Tokenizar(const string& str, list<string>& tokens)
{
    tokens.erase(tokens.begin(), tokens.end());
    if (casosEspeciales)
    {
        bool espacio_delimitador = delimiters.find(" ") != string::npos;
        if (!espacio_delimitador)
            AnyadirDelimitadoresPalabra(" ");
        Tokenizar_especial(str, tokens);
        if (!espacio_delimitador)
        {
            delimiters.erase(delimiters.find(" "));
            delimiters_set[32] = 0;
        }
    }
    else
    {
        string::const_iterator izquierda = str.begin();
        string::const_iterator derecha = str.begin();
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
    if (pasarAminuscSinAcentos)
        for (string& token : tokens)
            minusc_sin_acentos(token);
}

bool Tokenizador::TokenizarFichero(const string& filename, list<string>& tokens)
{
    int fd = open(filename.c_str(), O_RDONLY, (mode_t)0600);
    if (fd == -1)
        return false;
    struct stat fileInfo = {0};
    fstat(fd, &fileInfo);
    char* map = (char*) mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    string full_str(map);
    munmap(map, fileInfo.st_size);
    close(fd);
    Tokenizar(full_str, tokens);
    return true;
}

bool Tokenizador::EscribirFichero(const string& output_filename, const list<string>& tokens) const
{
    int fd = open(output_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1)
        return false;
    size_t textsize = 1;
    for (const string& token : tokens)
        textsize += token.length() + 1;
    lseek(fd, textsize - 1, SEEK_SET);
    write(fd, "", 1);
    char* map = (char*) mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    unsigned i = 0;
    for (const string& token : tokens)
    {
        for (unsigned j = 0; j < token.length(); j++)
        {
            map[i] = token[j];
            i++;
        }
        map[i] = '\n';
        i++;
    }
    if (msync(map, textsize, MS_SYNC) == -1)
        perror("Could not sync the file to disk");
    munmap(map, textsize);
    close(fd);
    return true;
}

bool Tokenizador::Tokenizar(const string& input_filename, const string& output_filename)
{
    //TODO: optimizar con https://gist.github.com/marcetcheverry/991042
    list<string> tokens;
    if (TokenizarFichero(input_filename, tokens))
    {
        if (!EscribirFichero(output_filename, tokens))
        {
            cerr << "ERROR: Error al crear el fichero: " << output_filename << endl;
            return false;
        }
    }
    else
    {
        cerr << "ERROR: No existe el archivo: " << input_filename << endl;
        return false;
    }
    return true;
}

bool Tokenizador::Tokenizar(const string& i)
{
    return Tokenizar(i, i + ".tk");
}

bool Tokenizador::TokenizarListaFicheros(const string& i)
{
    ifstream ifs(i);
    bool execution_is_right = true;
    if (ifs.is_open())
    {
        string str;
        while (ifs >> str)
        {
            if (!Tokenizador::file_exists(str))
            {
                cerr << "ERROR: El fichero " << str << " no existe" << endl;
                execution_is_right = false;
            }
            else if (Tokenizador::is_dir(str))
            {
                cerr << "ERROR: El fichero " << str << " es un directorio" << endl;
                execution_is_right = false;
            }
            execution_is_right = (execution_is_right && Tokenizar(str));
        }
        ifs.close();
    }
    else
    {
        cerr << "ERROR: No existe el fichero " << i << endl;
        execution_is_right = false;
    }
    return execution_is_right;
}

bool Tokenizador::TokenizarDirectorio(const string& i)
{
    //TODO fallo porque me coje el directorio raiz tambien como si fuera un fichero
    bool execution_is_right = true;
    if (!Tokenizador::file_exists(i))
    {
        cerr << "ERROR: El directorio " << i << " no existe" << endl;
        execution_is_right = false;
    }
    else if (!Tokenizador::is_dir(i))
    {
        cerr << "ERROR: El fichero " << i << " no es un directorio" << endl;
        execution_is_right = false;
    }
    else
    {
        string cmd = "find " + i + " -type f > " + Tokenizador::DEFAULT_FILELIST_FILENAME;
        system(cmd.c_str());
        execution_is_right = (execution_is_right && 
                            TokenizarListaFicheros(Tokenizador::DEFAULT_FILELIST_FILENAME));
    }
    return execution_is_right;
}


void Tokenizador::Tokenizar_especial(const std::string& str, std::list<std::string>& tokens)
{
    Estado::tokenizador = *this;
    string token;
    Estado estado(str, tokens);
    for (string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        Estado::absolute_iterator = it;
        token += *it;
        if (it == str.end() - 1 && *it != '%' && *it != '$')
        {
            if (is_delimiter(*it) && estado.estado != URL)
                token.erase(token.end() - 1);
            if (token.length() >= 1)
                tokens.push_back(token);
            return;
        }
        estado.siguiente(token);
        if (estado.estado == _default && is_delimiter(token[token.length() - 1]))
        {
            token.erase(token.end() - 1);
            if (token.length() >= 1)
                tokens.push_back(token);
            token.erase(token.begin(), token.end());
        }
    }
}

bool Estado::es_URL(const string& token) const
{
    if (Estado::URL_ALLOWED_DELI.find(*Estado::absolute_iterator) != string::npos)
    {
        if (token[token.length() - 1] == ':')
            return (!token.find("http:") || !token.find("https:") || !token.find("ftp:"));
    }
    return false;
}

bool Estado::es_decimal(const unsigned char c) const
{
    return (c >= 48 && c <= 57);
}

bool Estado::char_not_surround(const char& c) const
{
    if (*Estado::absolute_iterator == c)
    {
        if ((Estado::absolute_iterator == full_string.end() - 1 ||
             !Estado::tokenizador.is_delimiter((unsigned char)*next(Estado::absolute_iterator, 1))) &&
            (Estado::absolute_iterator == full_string.begin() ||
             !Estado::tokenizador.is_delimiter((unsigned char)*prev(Estado::absolute_iterator, 1))))
                return true;
    }
    return false;
}

bool Estado::es_decimal(const string& token) const
{
    if (Estado::tokenizador.is_delimiter('.') && Estado::tokenizador.is_delimiter(',')
        && (((token[token.length() - 1] == '.' || token[token.length() - 1] == ',')
            && !Estado::tokenizador.is_delimiter(*next(Estado::absolute_iterator, 1)))
            || token[token.length() - 1] == '%' || token[token.length() - 1] == '$'))
    {
        // if ((Estado::absolute_iterator < full_string.end() - 1 &&
        //      es_decimal((unsigned char)*next(Estado::absolute_iterator, 1))) &&
        //     (Estado::absolute_iterator == full_string.begin() ||
        //      es_decimal((unsigned char)*prev(Estado::absolute_iterator, 1)))
        //      || (Estado::tokenizador.is_delimiter(*prev(Estado::absolute_iterator, 1)))
        //      && es_decimal((unsigned char)*next(Estado::absolute_iterator, 1)))
        //      {
                for (int i = 0; i < token.length() - 1; i++)
                    if (!es_decimal((unsigned char) token[i]) && token[i] != '.'
                        && token[i] != ',')
                        return false;
                string::const_iterator it = Estado::absolute_iterator;
                while (it != Estado::full_string.end() - 1 &&
                       (!Estado::tokenizador.is_delimiter(*it) || *it == ','
                       || *it == '.'))
                {
                    if (!es_decimal((unsigned char)*it) && *it != '.' && *it != ',')
                    {
                        if (!(*it == '$' || *it == '%') || !(it == Estado::full_string.end() - 1
                         || Estado::tokenizador.is_delimiter(*(next(it, 1)))))
                            return false;
                    }
                    it++;
                }
                return true;
            //  }
    }
    return false;
}

bool Estado::es_email(const string& token) const
{
    if (char_not_surround('@') && Estado::tokenizador.is_delimiter('@')
        && Estado::absolute_iterator != full_string.begin()
        && Estado::absolute_iterator != full_string.end() - 1)
    {
        if (token.find('@') == token.length() - 1)
        {
            for (string::const_iterator it = Estado::absolute_iterator + 1; 
                (*it == '.' || *it == '_' || *it == '-' || *it == '@' || !Estado::tokenizador.is_delimiter(*it))
                && it != Estado::full_string.end();
                ++it)
            {
                if (*it == '@')
                    return false;
            }
        }
        else
            return false;
    }
    else
        return false;
    return true;
}

bool Estado::es_acronimo() const
{
    return char_not_surround('.') && Estado::tokenizador.is_delimiter('.')
            && Estado::absolute_iterator != full_string.begin()
            && Estado::absolute_iterator != full_string.end() - 1;

}

bool Estado::es_multipalabra() const
{
    return char_not_surround('-') && Estado::tokenizador.is_delimiter('-')
            && Estado::absolute_iterator != full_string.begin()
            && Estado::absolute_iterator != full_string.end() - 1;
}

void Estado::siguiente_default(string& token)
{
    if (es_URL(token))
        estado = URL;
    else if (es_decimal(token))
    {
        if (*Estado::absolute_iterator == '%' || *Estado::absolute_iterator == '$')
            siguiente_decimal(token);
        else
            estado = decimal;
        if (Estado::absolute_iterator == full_string.begin() ||
            Estado::tokenizador.is_delimiter(*prev(Estado::absolute_iterator, 1)))
            token.insert(0, "0");
    }
    else if (es_email(token))
        estado = email;
    else if (es_acronimo())
        estado = acronimo;
    else if (es_multipalabra())
        estado = multipalabra;
    else
        estado = _default;
}

void Estado::siguiente_decimal(string& token)
{
    if (*Estado::absolute_iterator == '%')
    {
        estado = pctg;
        token.erase(token.end() - 1, token.end());
        tokens.push_back(token);
        token.erase(token.begin(), token.end());
        tokens.push_back("%");
        Estado::absolute_iterator++;
    }
    else if (*Estado::absolute_iterator == '$')
    {
        estado = dollar;
        token.erase(token.end() - 1, token.end());
        tokens.push_back(token);
        token.erase(token.begin(), token.end());
        tokens.push_back("$");
        Estado::absolute_iterator++;
    }
    else if (!Estado::tokenizador.is_delimiter(*Estado::absolute_iterator) || 
       char_not_surround('.') || char_not_surround(','))
        estado = decimal;
    else if (!Estado::tokenizador.is_delimiter(*Estado::absolute_iterator) &&
             !es_decimal((unsigned char)*Estado::absolute_iterator))
        estado = acronimo;
    else
        estado = _default;
}

void Estado::siguiente(string& token)
{
    switch (estado)
    {
        case _default:
            siguiente_default(token);
            break;
        case URL:
            if (Estado::tokenizador.is_delimiter(*Estado::absolute_iterator)
                && Estado::URL_ALLOWED_DELI.find(*Estado::absolute_iterator) == string::npos)
                estado = _default;
            break;
        case decimal:
            siguiente_decimal(token);
            break;
        case email:
            if (Estado::tokenizador.is_delimiter(*Estado::absolute_iterator)
                && !char_not_surround('.') && !char_not_surround('_') && !char_not_surround('-'))
                estado = _default;
            break;
        case acronimo:
            if (Estado::tokenizador.is_delimiter(*Estado::absolute_iterator)
                && !char_not_surround('.'))
                estado = _default;
            break;
        case multipalabra:
            if (Estado::tokenizador.is_delimiter(*Estado::absolute_iterator)
                    && !char_not_surround('-'))
                estado = _default;
            break;
        case pctg:
            siguiente_default(token);
            break;
        case dollar:
            siguiente_default(token);
            break;
    }
}
