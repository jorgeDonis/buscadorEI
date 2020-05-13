#ifndef __BUSCADOR__
#define __BUSCADOR__

#include "indexadorHash.h"
#include <iostream>
#include <queue>
#include <string>

class ResultadoRI
{
    friend std::ostream& operator<<(std::ostream& os, const ResultadoRI& res);
    private:
        double vSimilitud;
        long int idDoc;
        string nombreDoc; //Nombre sin ruta ni extensión
        int numPregunta;
        void copy_vals(const ResultadoRI&);
    public:
        ResultadoRI(const double&, const long int&, const int&);
        ResultadoRI(const ResultadoRI&);
        ResultadoRI& operator=(const ResultadoRI&);
        double VSimilitud() const {return vSimilitud;}
        long int IdDoc() const {return idDoc;}
        string NombreDoc() const {return nombreDoc;}
        int NumPregunta() const {return numPregunta;}
        bool operator<(const ResultadoRI& lhs) const;

};

class Buscador: public IndexadorHash
{
    friend std::ostream& operator<<(std::ostream&, const Buscador&);
    private:
        const static float DEFAULT_B, DEFAULT_C, DEFAULT_K1;
        std::priority_queue<ResultadoRI> docsOrdenados;
        int formSimilitud;
        double c;
        double k1;
        double b;
        void copy_vals(const Buscador&);
        Buscador();
    public:
        Buscador(const string&, const int&);
        Buscador(const Buscador&);
        Buscador& operator=(const Buscador&);
        bool Buscar(const int& numDocumentos = 99999);
        bool Buscar(const string&, const int&, const int&, const int&);
        void ImprimirResultadoBusqueda(const int& numDocumentos = 99999);
};

#endif