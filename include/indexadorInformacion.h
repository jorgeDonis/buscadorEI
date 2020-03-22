#ifndef __INDEXADOR_INFORMACION__
#define __INDEXADOR_INFORMACION__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>


class InfTermDoc
{
    friend class IndexadorHash;
    friend std::ostream& operator<<(std::ostream& s, const InfTermDoc& p);
    private:
        int ft;
        std::list<int> posTerm;
        void copy_vals(const InfTermDoc&);
    public:
        InfTermDoc() {ft = 0;}
        InfTermDoc(const InfTermDoc &);
        ~InfTermDoc();
        InfTermDoc& operator=(const InfTermDoc&);
};

class InformacionTermino
{
    friend class IndexadorHash;
    friend std::ostream& operator<<(std::ostream& s, const InformacionTermino& p);
    private:
        int ftc;
        std::unordered_map<long int, InfTermDoc> l_docs;
        void copy_vals(const InformacionTermino&);
    public:
        InformacionTermino() : l_docs() {ftc = 0;}
        InformacionTermino(const InformacionTermino&);
        ~InformacionTermino();
        InformacionTermino& operator=(const InformacionTermino&);
};

class InfDoc
{
    friend class IndexadorHash;
    static long int DOC_ID;
    friend std::ostream& operator<<(std::ostream&, const InfDoc&);
    private:
        std::unordered_set<std::string> tokens;
        long int idDoc;
        int numPal;
        int numPalSinParada;
        int numPalDiferentes;
        int tamBytes;
        time_t fechaModificacion;
        void copy_vals(const InfDoc&);
    public:
        bool existe_token(const std::string& token) const;
        InfDoc();
        InfDoc(const InfDoc&);
        ~InfDoc() {InfDoc::DOC_ID--;}
        InfDoc& operator=(const InfDoc&);
};

class InfColeccionDocs
{
    friend class IndexadorHash;
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
    friend class IndexadorHash;
    friend std::ostream& operator<<(std::ostream&, const InformacionTerminoPregunta&);
    private:
        int ft;
        std::list <int> posTerm;
        void copy_vals(const InformacionTerminoPregunta&);
    public:
        InformacionTerminoPregunta() {ft = 0;}
        InformacionTerminoPregunta(const InformacionTerminoPregunta& foo) {copy_vals(foo);}
        ~InformacionTerminoPregunta() {posTerm.clear();}
        InformacionTerminoPregunta& operator=(const InformacionTerminoPregunta&);
};

class InformacionPregunta
{
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