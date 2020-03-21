#ifndef __TOKENIZADOR__
#define __TOKENIZADOR__

#include <string>
#include <iostream>
#include <list>

class Tokenizador
{
    friend std::ostream& operator<<(std::ostream&, const Tokenizador&);
    private:
        static const bool CASOS_ESPECIALES_DEFAULT;
        static const bool PASAR_MINUSC_DEFAULT;
        static const std::string DEFAULT_DELIMITERS;
        static const std::string DEFAULT_FILELIST_FILENAME;
        std::string delimiters;
        //delimiters_set[i] = 1 si el caracter i es delimitador. De lo contrario, 0.        
        short delimiters_set[256] = {0};
        bool casosEspeciales;
        bool pasarAminuscSinAcentos;
        void copy_values(const Tokenizador&);
        //para cuando casosEspeciales == true
        void Tokenizar_especial(const char*, const size_t, std::list<std::string>&);
        void Tokenizar_fichero(const char*, char*, const size_t);
    public:
        static bool is_dir(const std::string&);
        static bool file_exists(const std::string&);
        static void minusc_sin_acentos(std::string&);
        static const short MAPA_ACENTOS[256];
        Tokenizador();
        ~Tokenizador();
        Tokenizador(const std::string&, const bool&, const bool&);
        Tokenizador(const Tokenizador&);
        Tokenizador& operator=(const Tokenizador&);
        bool is_delimiter(const char&) const;
        void DelimitadoresPalabra(const std::string&);
        void AnyadirDelimitadoresPalabra(const std::string&);
        std::string DelimitadoresPalabra() const
        {
            return delimiters;
        }
        void CasosEspeciales(const bool& nuevoCasosEspeciales)
        {
            casosEspeciales = nuevoCasosEspeciales;
        }
        void PasarAminuscSinAcentos(const bool& nuevoPasarAMinusc)
        {
            pasarAminuscSinAcentos = nuevoPasarAMinusc;
        }
        bool PasarAminuscSinAcentos() const
        {
            return pasarAminuscSinAcentos;
        }
        void Tokenizar(const char*, const size_t, std::list<std::string> &);
        void Tokenizar(const std::string&, std::list<std::string>&);
        bool Tokenizar(const std::string&, const std::string&);
        bool Tokenizar(const std::string&);
        bool TokenizarListaFicheros(const std::string&);
        bool TokenizarDirectorio(const std::string&);
};

enum Estados {_default, URL, decimal, email, acronimo, multipalabra, pctg, dollar};
enum EspecialesActivos {URL_ac, decimal_ac, email_ac, acronimo_ac, multipalabra_ac};

//Representa el estado dentro de la m�quina de estados finitos
class Estado
{
    private:
        bool char_not_surround(const char &) const;
        bool es_URL(const std::string&) const;
        bool es_decimal(const std::string&) const;
        bool es_email(const std::string&) const;
        bool es_acronimo() const;
        bool es_multipalabra() const;
        bool casos_activos[5] = { false };
    public:
        static bool es_decimal(const char);
        static const short DEFAULT_CHARS[256];
        static const std::string URL_ALLOWED_DELI;
        char current_char;
        void set_casos_activos();
        Tokenizador* tokenizador;
        const char* full_str;
        size_t str_len;
        size_t absolute_iterator;
        std::list<std::string>& tokens;
        Estados estado;
        Estado(const char* str, std::list<std::string>& tk, Tokenizador* tok) : estado(_default), tokens(tk){
            tokenizador = tok;
            set_casos_activos();
            full_str = str;
            absolute_iterator = 0;
        }
        void siguiente(std::string&);
        void siguiente_default(std::string&);
        void siguiente_decimal(std::string&);
};


//Igual que Estado, pero no usa lista de tokens ni string
class EstadoChar
{
private:
    bool casos_activos[5] = {false};
    bool char_not_surround(const char &) const;
    bool es_URL() const;
    bool es_decimal();
    bool es_email() const;
    bool es_acronimo() const;
    bool es_multipalabra() const;
    void set_casos_activos();
public:
    char current_char;
    Tokenizador* tokenizador;
    const char* mapa_entrada;
    char* mapa_salida;
    size_t len;
    size_t iterador_entrada_izquierda;
    size_t iterador_entrada_derecha;
    static size_t iterador_salida;
    Estados estado;
    void escribir_token(const size_t, const size_t);
    EstadoChar(Tokenizador* tok, const char* map_in, char* map_out, const size_t len)
    {
        estado = _default;
        tokenizador = tok;
        set_casos_activos();
        mapa_entrada = map_in;
        mapa_salida = map_out;
        this->len = len;
        iterador_entrada_izquierda = iterador_entrada_derecha = iterador_salida = 0;
    }
    void siguiente();
    void siguiente_default();
    void siguiente_decimal();
};

#endif