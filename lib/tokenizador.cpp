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
    for (size_t i = 0; i < foo.length(); i++)
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
    {
        delimiters_set[(unsigned char) c] = 1;
        delimiters_set_plus_null[(unsigned char) c] = 1;
    }
    delimiters_set_plus_null[(unsigned char) '\000'] = 1;
    casosEspeciales = Tokenizador::CASOS_ESPECIALES_DEFAULT;
    pasarAminuscSinAcentos = Tokenizador::PASAR_MINUSC_DEFAULT;
}


Tokenizador::Tokenizador(const string& delimitadoresPalabra, const bool& kCasosEspeciales,
                         const bool& minuscSinAcentos)
{
    delimiters = delimitadoresPalabra;
    remove_duplicate_string(delimiters);
    for (const char &c : delimiters)
    {
        delimiters_set[(unsigned char) c] = 1;
        delimiters_set_plus_null[(unsigned char) c] = 1;
    }
    delimiters_set_plus_null[(unsigned char) '\000'] = 1;
    casosEspeciales = kCasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
}

void Tokenizador::copy_values(const Tokenizador& tokenizador)
{
    this->delimiters = tokenizador.delimiters;
    for (unsigned i = 0; i < 256; i++)
    {
        delimiters_set_plus_null[i] = tokenizador.delimiters_set_plus_null[i];
        delimiters_set[i] = tokenizador.delimiters_set[i];
    }
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
    {
        this->~Tokenizador();
        copy_values(tokenizador);
    }
    return *this;
}

inline bool Tokenizador::is_delimiter(const char& foo) const
{
    return delimiters_set[(unsigned char) foo];
}

void Tokenizador::DelimitadoresPalabra(const string& delimitadores)
{
    delimiters = delimitadores;
    remove_duplicate_string(delimiters);
    for (int i = 0; i < 256; i++)
    {
        delimiters_set[i] = 0;
        delimiters_set_plus_null[i] = 0;
    }
    for (const char &c : delimiters)
    {
        delimiters_set[(unsigned char) c] = 1;
        delimiters_set_plus_null[(unsigned char)c] = 1;
    }
    delimiters_set_plus_null[(unsigned char) '\000'] = 1;
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& delimitadores_extra)
{
    for (char nuevo_delimitador : delimitadores_extra)
    {
        if (delimiters.find(nuevo_delimitador) == string::npos)
        {
            delimiters += nuevo_delimitador;
            delimiters_set[(unsigned char) nuevo_delimitador] = 1;
            delimiters_set_plus_null[(unsigned char)nuevo_delimitador] = 1;
        }
    }
}

/**
 * @brief Abre el fichero como mmap y tokeniza sobre memoria
 * 
 * @param input_filename Fichero sobre el que se har� mmap
 * @return char* Tiene reservado tama�o igual que input_filename,
 * Contiene tokens separados por 30 (record separator) y termina en \0
 * (fin de cadena).
 * Tendr� que ser liberado en un futuro. Se añade /n como delimitador
 * de-facto
 */
char* Tokenizador::TokenizarFichero(const string& input_filename)
{
    bool hadNewLine = false;
    string oldDelimiters = delimiters;
    if (delimiters.find("\n") == string::npos)
    {
        AnyadirDelimitadoresPalabra("\n");
        hadNewLine = true;
    }
    int ifd = open(input_filename.c_str(), O_RDONLY, (mode_t)0600);
    if (!ifd)
        cerr << "ERROR: error al leer el fichero" << input_filename << endl;
    struct stat fileInfo = {0};
    fstat(ifd, &fileInfo);
    char* input_map = (char*) mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, ifd, 0);

    size_t len = fileInfo.st_size + 1;

    char* output_map = (char*) malloc((sizeof(char) * len) + 10);

    if (casosEspeciales)
        Tokenizar_fichero_especial(input_map, output_map, len - 1);
    else
        Tokenizar_fichero_simple(input_map, output_map, len - 1);

    munmap((char *)input_map, fileInfo.st_size);
    close(ifd);
    if (hadNewLine)
        DelimitadoresPalabra(oldDelimiters);
    return output_map;
}

/**
 * @brief Reserva memoria equivalente a la longitud del string y llama
 * a las mismas funciones que tokenizan ficheros
 * 
 * @param str String a tokenizar
 * @return char* sigue el mismo formato que TokenizarFichero(str). Tiene que ser liberado
 * posteriormente
 */

char* Tokenizador::TokenizarString(const string& str)
{
    char* output_map = new char[str.size() + 20]; //este 20 es por si falta memoria
    if (casosEspeciales)
        Tokenizar_fichero_especial(str.c_str(), output_map, str.size());
    else
        Tokenizar_fichero_simple(str.c_str(), output_map, str.size());

    return output_map;
}

bool Estado::es_decimal(const char c)
{
    return (c >= 48 && c <= 57);
}

//
//VERSION QUE USA PUNTEROS (para ficheros) Y NO USA REPRESENTACIONES INTERMEDIAS
//

void Tokenizador::Tokenizar_fichero_especial(const char* mapa_entrada, char* mapa_salida, const size_t len)
{
    EstadoChar estado(this, mapa_entrada, mapa_salida, len);
    while (true)
    {
        if (pasarAminuscSinAcentos)
            estado.current_char = MAPA_ACENTOS[(unsigned char) mapa_entrada[estado.iterador_entrada_derecha]];
        else
            estado.current_char = mapa_entrada[estado.iterador_entrada_derecha];
        if (estado.iterador_entrada_derecha == len - 1)
        {
            if (estado.current_char == '$' || estado.current_char == '%')
            {
                estado.escribir_token(estado.iterador_entrada_izquierda, estado.iterador_entrada_derecha - 1);
                estado.mapa_salida[estado.iterador_salida] = estado.current_char;
                estado.iterador_salida++;
                estado.mapa_salida[estado.iterador_salida] = 30;
                estado.iterador_salida++;
                mapa_salida[estado.iterador_salida] = '\0';
                return;
            }
            if (is_delimiter(estado.current_char) && estado.estado != URL)
                estado.iterador_entrada_derecha--;
            estado.escribir_token(estado.iterador_entrada_izquierda, estado.iterador_entrada_derecha);
            mapa_salida[estado.iterador_salida] = '\0';
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

void EstadoChar::escribir_token(const size_t inicio, const size_t final)
{
    if (inicio <= final)
    {
        for (size_t i = inicio; i <= final; i++)
        {
            if (tokenizador->PasarAminuscSinAcentos())
                mapa_salida[iterador_salida] = Tokenizador::MAPA_ACENTOS[(unsigned char) mapa_entrada[i]];
            else
                mapa_salida[iterador_salida] = mapa_entrada[i];
            iterador_salida++;
        }
        mapa_salida[iterador_salida] = 30;
        iterador_salida++;
    }
}

bool EstadoChar::char_not_surround(const char & c) const
{
    if (current_char == c)
    {
        if ((iterador_entrada_derecha == len - 1 ||
             !tokenizador->is_delimiter(mapa_entrada[iterador_entrada_derecha + 1]) &&
            (iterador_entrada_derecha == 0 ||
             !tokenizador->is_delimiter(mapa_entrada[iterador_entrada_derecha - 1]))))
                return true;
    }
    return false;
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

bool EstadoChar::es_URL() const
{
    if (casos_activos[URL_ac])
    {
        if (current_char == ':')
        {
            string s(mapa_entrada, iterador_entrada_izquierda,
                     iterador_entrada_derecha - iterador_entrada_izquierda);
            Tokenizador::minusc_sin_acentos(s);
            return (!s.find("http") || !s.find("https") || !s.find("ftp"));
        }
    }
    return false;
}

bool EstadoChar::es_decimal()
{
    if (casos_activos[decimal_ac] && (((current_char == '.' || current_char == ',')
            && !tokenizador->is_delimiter(mapa_entrada[iterador_entrada_derecha + 1]))
            || current_char == '%' || current_char == '$'))
    {
        for (size_t i = iterador_entrada_izquierda; i <= iterador_entrada_derecha; i++)
            if (!Estado::es_decimal((unsigned char) mapa_entrada[i]) && mapa_entrada[i] != '.'
                && mapa_entrada[i] != ',')
                return false;
        size_t it = iterador_entrada_derecha;
        while (it != len - 1)
        {
            char it_char = mapa_entrada[it];
            if (!tokenizador->is_delimiter(it_char) || it_char == ',' || it_char == '.')
            {
                if (!Estado::es_decimal(it_char) && it_char != '.' && it_char != ',')
                {
                    if (!(it_char == '$' || it_char == '%') 
                    || !(tokenizador->is_delimiter(mapa_entrada[it + 1])))
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

bool EstadoChar::es_email() const
{
    if (casos_activos[email_ac] && char_not_surround('@'))
    {
        string s(mapa_entrada, iterador_entrada_izquierda, 
                 iterador_entrada_derecha - iterador_entrada_izquierda);
        if (s.find('@') == s.length() - 1)
        {
            size_t it = iterador_entrada_derecha + 1; 
            char it_char = mapa_entrada[it];
            while (it != len - 1 && (it_char == '.' || it_char == '_' || it_char == '-' ||
                   it_char == '@' || !tokenizador->is_delimiter(it_char)))
            {
                if (it_char == '@')
                    return false;
                it++;
                it_char = mapa_entrada[it];
            }
        }
        else
            return false;
    }
    else
        return false;
    return true;
}

bool EstadoChar::es_acronimo() const
{
    return casos_activos[acronimo_ac] && char_not_surround('.');
}

bool EstadoChar::es_multipalabra() const
{
    return casos_activos[multipalabra_ac] && char_not_surround('-');
}

void EstadoChar::siguiente_decimal()
{
    if (current_char == '%')
    {
        estado = pctg;
        escribir_token(iterador_entrada_izquierda, iterador_entrada_derecha - 1);
        mapa_salida[iterador_salida] = '%';
        iterador_salida++;
        mapa_salida[iterador_salida] = 30;
        iterador_salida++;
        iterador_entrada_izquierda = iterador_entrada_derecha + 1;
    }
    else if (current_char == '$')
    {
        estado = dollar;
        escribir_token(iterador_entrada_izquierda, iterador_entrada_derecha - 1);
        mapa_salida[iterador_salida] = '$';
        iterador_salida++;
        mapa_salida[iterador_salida] = 30;
        iterador_salida++;
        iterador_entrada_izquierda = iterador_entrada_derecha + 1;
    }
    else if (!tokenizador->is_delimiter(current_char) ||
             char_not_surround('.') || char_not_surround(','))
        estado = decimal;
    else if (!tokenizador->is_delimiter(current_char) &&
             !Estado::es_decimal(current_char))
        estado = acronimo;
    else
        estado = _default;
}

void EstadoChar::siguiente_default()
{
    if (es_URL())
        estado = URL;
    else if (es_decimal())
    {
        if (current_char == '%' || current_char == '$')
            siguiente_decimal();
        else
            estado = decimal;
        if (iterador_entrada_derecha == 0 ||
            tokenizador->is_delimiter(mapa_entrada[iterador_entrada_derecha - 1]))
        {
            mapa_salida[iterador_salida] = '0';
            iterador_salida++;
        }
    }
    else if (iterador_entrada_derecha != 0 && iterador_entrada_derecha != len - 1)
    {
        if (es_email())
            estado = email;
        else if (es_acronimo())
            estado = acronimo;
        else if (es_multipalabra())
            estado = multipalabra;
        else
            estado = _default;
    }
}

void EstadoChar::siguiente()
{
    switch (estado)
    {
    case _default:
        if (!Estado::DEFAULT_CHARS[current_char])
            siguiente_default();
        break;
    case URL:
        if (tokenizador->is_delimiter(current_char) && Estado::URL_ALLOWED_DELI.find(current_char) == string::npos)
            estado = _default;
        break;
    case decimal:
        siguiente_decimal();
        break;
    case email:
        if (tokenizador->is_delimiter(current_char) && !char_not_surround('.') && !char_not_surround('_') && !char_not_surround('-'))
            estado = _default;
        break;
    case acronimo:
        if (tokenizador->is_delimiter(current_char) && !char_not_surround('.'))
            estado = _default;
        break;
    case multipalabra:
        if (tokenizador->is_delimiter(current_char) && !char_not_surround('-'))
            estado = _default;
        break;
    case pctg:
        siguiente_default();
        break;
    case dollar:
        siguiente_default();
        break;
    }
}

inline void Tokenizador::Tokenizar_fichero_simple(const char* mapa_entrada, char* mapa_salida, const size_t len)
{
    size_t it_entrada, it_salida;
    it_entrada = it_salida = 0;
    char c = mapa_entrada[0];
    while (it_entrada < len - 1) //OJO ESTE - 1 NO ESTOY SEGURO TODO
    {
        while (is_delimiter(c))
        {
            it_entrada++;
            c = mapa_entrada[it_entrada];
        }
        while (!delimiters_set_plus_null[(unsigned char) c])
        {
            if (pasarAminuscSinAcentos)
                mapa_salida[it_salida] = MAPA_ACENTOS[(unsigned char) c];
            else
                mapa_salida[it_salida] = c;
            it_salida++;
            it_entrada++;
            c = mapa_entrada[it_entrada];
        }
        mapa_salida[it_salida] = 30;
        it_salida++;
    }
    mapa_salida[it_salida] = '\0';
}
