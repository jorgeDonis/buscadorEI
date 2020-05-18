#ifndef __BUSCADOR__
#define __BUSCADOR__

#include "indexadorHash.h"
#include <iostream>
#include <vector>
#include <string>

class ResultadoRI
{
    friend class Debugger;
    friend std::ostream& operator<<(std::ostream& os, const ResultadoRI& res);
    private:
        double vSimilitud;
        long int idDoc;
        int numPregunta;
        void copy_vals(const ResultadoRI&);
    public:
        ResultadoRI(const double&, const long int&, const int&);
        ResultadoRI(const double &, const long int &, const int &, string&);
        ResultadoRI(const ResultadoRI&);
        ResultadoRI& operator=(const ResultadoRI&);
        double VSimilitud() const {return vSimilitud;}
        long int IdDoc() const {return idDoc;}
        int NumPregunta() const {return numPregunta;}
        bool operator<(const ResultadoRI& lhs) const;

};

class Buscador: public IndexadorHash
{
    friend class Debugger;
    friend std::ostream& operator<<(std::ostream&, const Buscador&);
    private:
        const static size_t TOTAL_PREGUNTAS;
        const static float DEFAULT_B, DEFAULT_C, DEFAULT_K1;
        const static int DEFAULT_FORM_SIMILITUD;
        std::vector<ResultadoRI> docsOrdenados; //TODO transformar en lista de listas si va lento
        std::unordered_map<long int, std::string> nombresDocs; //TODO hacer otra tabla hash con el nombre sin extension si va lento
        std::unordered_map<long int, std::string> nombresDocsSinRuta;
        size_t numDocsBuscados;
        std::string busqueda_str;
        int formSimilitud;
        double c;
        double k1;
        double b;
        void precalcular_bm25();
        void precalcular_dfr();
        void precalcular_offline();
        void guardarNombresDocs();
        void copy_vals(const Buscador&);
        void calc_simil_docs(const size_t&);
        void imprimir_busqueda_str(const int& numDocumentos = 99999);
        void indexar_pregunta(const size_t&, const std::string&);
        Buscador();
    public:
        Buscador(const string&, const int&);
        Buscador(const Buscador&);
        Buscador& operator=(const Buscador&);
        bool Buscar(const int& numDocumentos = 99999);
        bool Buscar(const string&, const int&, const int&, const int&);
        void ImprimirResultadoBusqueda(const int& numDocumentos = 99999);
        bool ImprimirResultadoBusqueda(const std::string&, const int &numDocumentos = 99999);
        int DevolverFormulaSimilitud() const {return formSimilitud;}
        bool CambiarFormulaSimilitud(const int& f) {
            if (!f || (f == 1)) {formSimilitud = f; return true;} else return false;
        }
        void CambiarParametrosDFR(const double& kc) { c = kc; }
        double DevolverParametrosDFR() const { return c; }
        void CambiarParametrosBM25(const double& kk1, const double& kb) { k1 = kk1; b = kb; }
        void DevolverParametrosBM25(double& kk1, double& kb) const { kk1 = k1; kb = b; }
};

#endif