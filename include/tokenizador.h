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
        static const short MAPA_ACENTOS[256];
        static const std::string DEFAULT_FILELIST_FILENAME;
        static bool is_dir(const std::string&);
        static bool file_exists(const std::string&);
        static void minusc_sin_acentos(std::string&);
        bool is_delimiter(const char&) const;
        std::string delimiters;
        //Contiene todos los delimitadores en una tabla hash        
        std::unordered_set<char> delimiters_set;
        bool casosEspeciales;
        bool pasarAminuscSinAcentos;
        void copy_values(const Tokenizador&);
        //para cuando casosEspeciales == true
        void Tokenizar_especial(std::string &, std::list<std::string> &);
        bool es_URL(const std::string&) const;
    public:
        Tokenizador();
        ~Tokenizador();
        Tokenizador(const std::string&, const bool&, const bool&);
        Tokenizador(const Tokenizador&);
        Tokenizador& operator=(const Tokenizador&);
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
        void Tokenizar(std::string&, std::list<std::string>&);
        bool Tokenizar(const std::string&, const std::string&);
        bool Tokenizar(const std::string&);
        bool TokenizarListaFicheros(const std::string&);
        bool TokenizarDirectorio(const std::string&);
};

class Estado
{
    private:
    public:
        short estado;
        Estado() {estado = 0;}
        Estado(int estado) {this->estado = estado;}
        static void siguiente(const Estado&, std::string& );
};

#endif