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
const short Estado::DEFAULT_CHARS[256] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
const string Estado::URL_ALLOWED_DELI = "_:/.?&-=#@";
size_t EstadoChar::iterador_salida;

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

void Tokenizador::Tokenizar(const char* str, const size_t len, list<string>& tokens)
{
    size_t izquierda = 0;
    size_t derecha = 0;
    while (derecha != len)
    {
        while (izquierda != len && is_delimiter(str[izquierda]))
            izquierda++;
        derecha = izquierda;
        string token = "";
        while (derecha != len && !is_delimiter(str[derecha]))
        {
            token += str[derecha];
            derecha++;
        }
        tokens.push_back(token);
        izquierda = derecha + 1;
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
        Tokenizar_especial(str.c_str(), str.length(), tokens);
        if (!espacio_delimitador)
        {
            delimiters.erase(delimiters.find(" "));
            delimiters_set[32] = 0;
        }
    }
    else
        Tokenizar(str.c_str(), str.length(), tokens);
    if (pasarAminuscSinAcentos)
        for (string& token : tokens)
            minusc_sin_acentos(token);
}

bool Tokenizador::Tokenizar(const string& input_filename, const string& output_filename)
{
    int ifd = open(input_filename.c_str(), O_RDONLY, (mode_t)0600);
    if (!ifd)
    {
        cerr << "ERROR: error al leer el fichero" << input_filename << endl;
        return false;
    }
    struct stat fileInfo = {0};
    fstat(ifd, &fileInfo);
    char *input_map = (char *)mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, ifd, 0);

    size_t len = fileInfo.st_size + 1;
    int ofd = open(output_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (!ofd)
    {
        cerr << "ERROR: error al crear el fichero" << output_filename << endl;
        return false;   
    }
    lseek(ofd, len, SEEK_SET);
    write(ofd, "", 1);

    char *output_map = (char *)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, ofd, 0);

    Tokenizar_fichero(input_map, output_map, len - 1);

    if (msync(output_map, len, MS_SYNC) == -1)
    {
        cerr << "ERROR: no se pudo sincronizar con el disco" << endl;
        return false;
    }
    munmap(output_map, len);
    close(ofd);
    munmap((char *)input_map, fileInfo.st_size);
    close(ifd);
    truncate(output_filename.c_str(), EstadoChar::iterador_salida - 1);
    return true;
}

bool Tokenizador::Tokenizar(const string& i)
{
    return Tokenizar(i, i + ".tk");
}

bool Tokenizador::TokenizarListaFicheros(const string& i)
{
    AnyadirDelimitadoresPalabra("\n");
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


void Tokenizador::Tokenizar_especial(const char* str, const size_t len, list<string>& tokens)
{
    if (!len)
        return;
    string token;
    Estado estado(str, tokens, this);
    estado.str_len = len;
    while (true)
    {
        estado.current_char = str[estado.absolute_iterator];
        token += estado.current_char;
        if (estado.absolute_iterator == len - 1)
        {
            if (estado.current_char == '$' || estado.current_char == '%')
            {
                token.erase(token.end() - 1, token.end());
                tokens.push_back(token);
                tokens.push_back(string(1, estado.current_char));
                return;
            }
            if (is_delimiter(estado.current_char) && estado.estado != URL)
                token.erase(token.end() - 1);
            if (token.length() >= 1)
                tokens.push_back(token);
            return;
        }
        estado.siguiente(token);
        if (estado.estado == _default && is_delimiter(estado.current_char))
        {
            token.erase(token.end() - 1);
            if (token.length() >= 1)
                tokens.push_back(token);
            token.erase(token.begin(), token.end());
        }
        estado.absolute_iterator++;
    }
}

void Estado::set_casos_activos()
{
    for (const char& c : Estado::URL_ALLOWED_DELI)
        if (tokenizador->is_delimiter(c))
            casos_activos[URL_ac] = true;
    if (tokenizador->is_delimiter('.') && tokenizador->is_delimiter(','))
        casos_activos[decimal_ac] = true;
    if (tokenizador->is_delimiter('@'))
        casos_activos[email_ac] = true;
    if (tokenizador->is_delimiter('.'))
        casos_activos[acronimo_ac] = true;
    if (tokenizador->is_delimiter('-'))
        casos_activos[multipalabra_ac] = true;
}

bool Estado::es_URL(const string& token) const
{
    if (casos_activos[URL_ac])
    {
        if (token[token.length() - 1] == ':')
            return (!token.find("http:") || !token.find("https:") || !token.find("ftp:"));
    }
    return false;
}

bool Estado::es_decimal(const char c) const
{
    return (c >= 48 && c <= 57);
}

bool Estado::char_not_surround(const char& c) const
{
    if (current_char == c)
    {
        if ((absolute_iterator == str_len - 1 ||
             !tokenizador->is_delimiter(full_str[absolute_iterator + 1]) &&
            (absolute_iterator == 0 ||
             !tokenizador->is_delimiter(full_str[absolute_iterator - 1]))))
                return true;
    }
    return false;
}

bool Estado::es_decimal(const string& token) const
{
    if (casos_activos[decimal_ac] && (((current_char == '.' || current_char == ',')
            && !tokenizador->is_delimiter(full_str[absolute_iterator + 1]))
            || current_char == '%' || current_char == '$'))
    {
        for (int i = 0; i < token.length() - 1; i++)
            if (!es_decimal((unsigned char) token[i]) && token[i] != '.'
                && token[i] != ',')
                return false;
        size_t it = absolute_iterator;
        while (it != str_len - 1)
        {
            char it_char = full_str[it];
            if (!tokenizador->is_delimiter(it_char) || it_char == ',' || it_char == '.')
            {
                if (!es_decimal(it_char) && it_char != '.' && it_char != ',')
                {
                    if (!(it_char == '$' || it_char == '%') 
                    || !(tokenizador->is_delimiter(full_str[it + 1])))
                        return false;
                }
                it++;
            }
               else break;
        }
        return true;
    }
    return false;
}

bool Estado::es_email(const string& token) const
{
    if (casos_activos[email_ac] && char_not_surround('@'))
    {
        if (token.find('@') == token.length() - 1)
        {
            size_t it = absolute_iterator + 1; 
            char it_char = full_str[it];
            while (it != str_len - 1 && (it_char == '.' || it_char == '_' || it_char == '-' || it_char == '@' ||
                   !tokenizador->is_delimiter(it_char)))
            {
                if (it_char == '@')
                    return false;
                it++;
                it_char = full_str[it];
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
    return casos_activos[acronimo_ac] && char_not_surround('.');
}

bool Estado::es_multipalabra() const
{
    return casos_activos[multipalabra_ac] && char_not_surround('-');
}

void Estado::siguiente_default(string& token)
{
    if (es_URL(token))
        estado = URL;
    else if (es_decimal(token))
    {
        if (current_char == '%' || current_char == '$')
            siguiente_decimal(token);
        else
            estado = decimal;
        if (absolute_iterator == 0 ||
            tokenizador->is_delimiter(full_str[absolute_iterator - 1]))
            token.insert(0, "0");
    }
    else if (absolute_iterator != 0 && absolute_iterator != str_len - 1)
    {
        if (es_email(token))
            estado = email;
        else if (es_acronimo())
            estado = acronimo;
        else if (es_multipalabra())
            estado = multipalabra;
    } 
    else
        estado = _default;
}

void Estado::siguiente_decimal(string& token)
{
    if (current_char == '%')
    {
        estado = pctg;
        token.erase(token.end() - 1, token.end());
        tokens.push_back(token);
        token.erase(token.begin(), token.end());
        tokens.push_back("%");
        absolute_iterator++;
    }
    else if (current_char == '$')
    {
        estado = dollar;
        token.erase(token.end() - 1, token.end());
        tokens.push_back(token);
        token.erase(token.begin(), token.end());
        tokens.push_back("$");
        absolute_iterator++;
    }
    else if (!tokenizador->is_delimiter(current_char) || 
       char_not_surround('.') || char_not_surround(','))
        estado = decimal;
    else if (!tokenizador->is_delimiter(current_char) &&
             !es_decimal(current_char))
        estado = acronimo;
    else
        estado = _default;
}

void Estado::siguiente(string& token)
{
    switch (estado)
    {
        case _default:
            if (!Estado::DEFAULT_CHARS[current_char])
                siguiente_default(token);
            break;
        case URL:
            if (tokenizador->is_delimiter(current_char)
                && Estado::URL_ALLOWED_DELI.find(current_char) == string::npos)
                estado = _default;
            break;
        case decimal:
            siguiente_decimal(token);
            break;
        case email:
            if (tokenizador->is_delimiter(current_char)
                && !char_not_surround('.') && !char_not_surround('_') && !char_not_surround('-'))
                estado = _default;
            break;
        case acronimo:
            if (tokenizador->is_delimiter(current_char)
                && !char_not_surround('.'))
                estado = _default;
            break;
        case multipalabra:
            if (tokenizador->is_delimiter(current_char)
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

void EstadoChar::escribir_token(const size_t inicio, const size_t final)
{
    if (inicio <= final)
    {
        for (size_t i = inicio; i <= final; i++)
        {
            mapa_salida[iterador_salida] = Tokenizador::MAPA_ACENTOS[mapa_entrada[i]];
            iterador_salida++;
        }
        mapa_salida[iterador_salida] = '\n';
        iterador_salida++;
    }
}

void Tokenizador::Tokenizar_fichero(const char* mapa_entrada, char* mapa_salida, const size_t len)
{
    EstadoChar estado(this, mapa_entrada, mapa_salida, len);
    while (true)
    {
        estado.current_char = MAPA_ACENTOS[mapa_entrada[estado.iterador_entrada_derecha]];
        if (estado.iterador_entrada_derecha == len - 1)
        {
            if (estado.current_char == '$' || estado.current_char == '%')
            {
                estado.escribir_token(estado.iterador_entrada_izquierda, estado.iterador_entrada_derecha - 1);
                estado.mapa_salida[estado.iterador_salida] = estado.current_char;
                estado.iterador_salida++;
                estado.mapa_salida[estado.iterador_salida] = '\n';
                estado.iterador_salida++;
                return;
            }
            if (is_delimiter(estado.current_char) && estado.estado != URL)
                estado.iterador_entrada_derecha--;
            estado.escribir_token(estado.iterador_entrada_izquierda, estado.iterador_entrada_derecha);
            return;
        }
        estado.siguiente();
        if (estado.estado == _default && is_delimiter(estado.current_char))
        {
            if (estado.iterador_entrada_derecha)
                estado.escribir_token(estado.iterador_entrada_izquierda, estado.iterador_entrada_derecha - 1);
            estado.iterador_entrada_izquierda = estado.iterador_entrada_derecha + 1;
        }
        estado.iterador_entrada_derecha++;
    }
}

void EstadoChar::set_casos_activos()
{
    for (const char &c : Estado::URL_ALLOWED_DELI)
        if (tokenizador->is_delimiter(c))
            casos_activos[URL_ac] = true;
    if (tokenizador->is_delimiter('.') && tokenizador->is_delimiter(','))
        casos_activos[decimal_ac] = true;
    if (tokenizador->is_delimiter('@'))
        casos_activos[email_ac] = true;
    if (tokenizador->is_delimiter('.'))
        casos_activos[acronimo_ac] = true;
    if (tokenizador->is_delimiter('-'))
        casos_activos[multipalabra_ac] = true;
}

void EstadoChar::siguiente()
{

}
