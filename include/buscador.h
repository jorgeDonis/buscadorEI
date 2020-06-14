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
        ResultadoRI();
        ResultadoRI(const double&, const long int&, const int&);
        ResultadoRI(const double &, const long int &, const int &, string&);
        ResultadoRI(const ResultadoRI&);
        ResultadoRI& operator=(const ResultadoRI&);
        double VSimilitud() const {return vSimilitud;}
        long int IdDoc() const {return idDoc;}
        int NumPregunta() const {return numPregunta;}
        void reducirPregunta() {numPregunta--;}
        void setVSimilitud(const double& sim) { vSimilitud = sim; }
        void setIdDoc(const int& id) { idDoc = id; }
        void setNumPregunta(const int& preg) { numPregunta = preg; }
        bool operator<(const ResultadoRI& lhs) const;
        void sumarSimilitud(const double&);

};

class Buscador: public IndexadorHash
{
    friend class Debugger;
    friend std::ostream& operator<<(std::ostream&, const Buscador&);
    private:
        const static unsigned TOTAL_DOCUMENTOS;
        const static unsigned TOTAL_PREGUNTAS;
        const static float DEFAULT_B, DEFAULT_C, DEFAULT_K1;
        const static int DEFAULT_FORM_SIMILITUD;
        ResultadoRI docsOrdenados[83][423];
        std::unordered_map<long int, std::string> nombresDocs;
        std::unordered_map<long int, std::string> nombresDocsSinRuta;
        std::string busqueda_str; //TODO optimizar con char*
        int formSimilitud;
        bool conjuntoPreguntas;
        unsigned preg_inicial;
        unsigned preg_final;
        double c;
        double k1;
        double b;
        void limpiar_docs_ordenados();
        void precalcular_bm25();
        void precalcular_dfr();
        void precalcular_offline();
        void guardarNombresDocs();
        void copy_vals(const Buscador&);
        void calc_simil_docs(const size_t&);
        void imprimir_res_pregunta(const unsigned &num_pregunta, const int& numDocumentos);
        void imprimir_busqueda_str(const int& numDocumentos);
        void indexar_pregunta(const size_t&, const std::string&);
        Buscador();
    public:
        Buscador(const string&, const int&);
        Buscador(const Buscador&);
        Buscador& operator=(const Buscador&);
        bool Buscar(const int& numDocumentos = 99999);
        bool Buscar(const string&, const int&, const int&, const int&);
        void ImprimirResultadoBusqueda(const int& numDocumentos = 99999);
        bool ImprimirResultadoBusqueda(const int &numDocumentos, const std::string &);
        int DevolverFormulaSimilitud() const { return formSimilitud; }
        bool CambiarFormulaSimilitud(const int& f) {
            if (!f || (f == 1)) { formSimilitud = f; return true; } else return false;
        }
        void CambiarParametrosDFR(const double& kc) { c = kc; }
        double DevolverParametrosDFR() const { return c; }
        void CambiarParametrosBM25(const double& kk1, const double& kb) { k1 = kk1; b = kb; }
        void DevolverParametrosBM25(double& kk1, double& kb) const { kk1 = k1; kb = b; }
};

#endif