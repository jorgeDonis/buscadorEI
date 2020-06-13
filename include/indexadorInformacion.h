#ifndef __INDEXADOR_INFORMACION__
#define __INDEXADOR_INFORMACION__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>


class InfTermDoc
{
    friend class IndexadorHash;
    friend class GestorFicheros;
    friend class Debugger;
    friend class Buscador;
    friend std::ostream& operator<<(std::ostream& s, const InfTermDoc& p);
    private:
        int ft;
        double dfr_parcial;
        double bm25_parcial;
        std::list<int> posTerm;
        void copy_vals(const InfTermDoc&);
    public:
        InfTermDoc() {ft = 0; dfr_parcial = -1; bm25_parcial = -1;}
        InfTermDoc(const InfTermDoc &);
        InfTermDoc& operator=(const InfTermDoc&);
};

class InformacionTermino
{
    friend class IndexadorHash;
    friend class GestorFicheros;
    friend class Debugger;
    friend class Buscador;
    friend std::ostream& operator<<(std::ostream& s, const InformacionTermino& p);
    private:
        int ftc;
        std::unordered_map<long int, InfTermDoc> l_docs;
        void copy_vals(const InformacionTermino&);
    public:
        InformacionTermino() {ftc = 0;}
        InformacionTermino(const InformacionTermino&);
        InformacionTermino& operator=(const InformacionTermino&);
};

class InfDoc
{
    friend class Buscador;
    friend class IndexadorHash;
    friend class GestorFicheros;
    friend class Debugger;
    static long int DOC_ID;
    friend std::ostream& operator<<(std::ostream&, const InfDoc&);
    private:
        long int idDoc;
        int numPal;
        int numPalSinParada;
        int numPalDiferentes;
        int tamBytes;
        time_t fechaModificacion;
        void copy_vals(const InfDoc&);
    public:
        InfDoc();
        InfDoc(const InfDoc&);
        ~InfDoc() {;}
        InfDoc& operator=(const InfDoc&);
};

class InfColeccionDocs
{
    friend class Buscador;
    friend class IndexadorHash;
    friend class GestorFicheros;
    friend class Debugger;
    friend std::ostream& operator<<(std::ostream&, const InfColeccionDocs&);
    private:
        long int numDocs;
        long int numTotalPal;
        long int numTotalPalSinParada;
        long int numTotalPalDiferentes;
        long int tamBytes;
        void copy_vals(const InfColeccionDocs&);
    public:
        InfColeccionDocs();
        InfColeccionDocs(const InfColeccionDocs&);
        InfColeccionDocs& operator=(const InfColeccionDocs&);
};

class InformacionTerminoPregunta
{
    friend class Buscador;
    friend class GestorFicheros;
    friend class Debugger;
    friend class IndexadorHash;
    friend std::ostream& operator<<(std::ostream&, const InformacionTerminoPregunta&);
    private:
        int ft;
        std::list <int> posTerm;
        void copy_vals(const InformacionTerminoPregunta&);
    public:
        InformacionTerminoPregunta() {ft = 0;}
        ~InformacionTerminoPregunta() {posTerm.clear();}
        InformacionTerminoPregunta(const InformacionTerminoPregunta& foo) {copy_vals(foo);}
        InformacionTerminoPregunta& operator=(const InformacionTerminoPregunta&);
};

class InformacionPregunta
{
    friend class Buscador;
    friend class GestorFicheros;
    friend class Debugger;
    friend class IndexadorHash;
    friend std::ostream& operator<<(std::ostream&, const InformacionPregunta&);
    private:
        long int numTotalPal;
        long int numTotalPalSinParada;
        long int numTotalPalDiferentes;
        void copy_vals(const InformacionPregunta&);
    public:
        InformacionPregunta() {numTotalPal = numTotalPalSinParada = numTotalPalDiferentes = 0;}
        InformacionPregunta(const InformacionPregunta& foo) {copy_vals(foo);}
        InformacionPregunta& operator=(const InformacionPregunta&);
};  

#endif