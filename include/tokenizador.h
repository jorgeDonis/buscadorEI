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
        static const short MAPA_ACENTOS[256];
        static const std::string DEFAULT_FILELIST_FILENAME;
        static bool is_dir(const std::string&);
        static bool file_exists(const std::string&);
        static void minusc_sin_acentos(std::string&);
        std::string delimiters;
        //delimiters_set[i] = 1 si el caracter i es delimitador. De lo contrario, 0.        
        short delimiters_set[256] = {0};
        bool casosEspeciales;
        bool pasarAminuscSinAcentos;
        void copy_values(const Tokenizador&);
        //para cuando casosEspeciales == true
        void Tokenizar_especial(const char*, const size_t, std::list<std::string>&);
    public:
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
        bool TokenizarFichero(const std::string&, std::list<std::string>&);
        bool EscribirFichero(const std::string &, const std::list<std::string>&) const;
        bool Tokenizar(const std::string&, const std::string&);
        bool Tokenizar(const std::string&);
        bool TokenizarListaFicheros(const std::string&);
        bool TokenizarDirectorio(const std::string&);
};

enum Estados {_default, URL, decimal, email, acronimo, multipalabra, pctg, dollar};
enum EspecialesActivos {URL_ac, decimal_ac, email_ac, acronimo_ac, multipalabra_ac};

//Representa el estado dentro de la máquina de estados finitos
class Estado
{
    private:
        static const short DEFAULT_CHARS[256];
        static const std::string URL_ALLOWED_DELI;
        bool char_not_surround(const char &) const;
        bool es_URL(const std::string&) const;
        bool es_decimal(const char) const;
        bool es_decimal(const std::string&) const;
        bool es_email(const std::string&) const;
        bool es_acronimo() const;
        bool es_multipalabra() const;
        bool casos_activos[5] = { false };
    public:
        char current_char;
        void set_casos_activos();
        Tokenizador* tokenizador;
        const char* full_str;
        size_t str_len;
        size_t absolute_iterator;
        std::list<std::string>& tokens;
        Estados estado;
        Estado(const char* str, std::list<std::string>& tk) : estado(_default), tokens(tk){
            set_casos_activos();
            full_str = str;
        }
        void siguiente(std::string&);
        void siguiente_default(std::string&);
        void siguiente_decimal(std::string&);

};

#endif