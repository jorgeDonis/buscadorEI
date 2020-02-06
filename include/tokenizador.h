#ifndef __TOKENIZADOR__
#define __TOKENIZADOR__

#include <string>
#include <iostream>
#include <list>
#include <unordered_set>

class Tokenizador
{
    friend std::ostream& operator<<(std::ostream&, const Tokenizador&);
    private:
        static const bool CASOS_ESPECIALES_DEFAULT;
        static const bool PASAR_MINUSC_DEFAULT;
        static const std::string DEFAULT_DELIMITERS;
        static char MAPA_ACENTOS[256];
        static void init_mapa_acentos();
        std::string delimiters;
        //Contiene todos los delimitadores en una tabla hash        
        std::unordered_set<char> delimiters_set;
        bool is_delimiter(const char&) const;
        bool casosEspeciales;
        bool pasarAminuscSinAcentos;
        void copy_values(const Tokenizador&);
    public:
        Tokenizador();
        Tokenizador(const std::string&, const bool&, const bool&);
        Tokenizador(const Tokenizador&);
        Tokenizador& operator=(const Tokenizador&);
        void Tokenizar(std::string&, std::list<std::string>&) const;
        bool Tokenizar(const std::string&, const std::string&) const;
};

#endif