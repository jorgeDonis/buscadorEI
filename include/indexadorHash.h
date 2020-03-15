#ifndef __INDEXADOR_HASH__
#define __INDEXADOR_HASH__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "indexadorInformacion.h"
#include "tokenizador.h"

class IndexadorHash
{
    friend std::ostream& operator<<(std::ostream&, const IndexadorHash&);
    private:
        IndexadorHash();
        std::unordered_map<std::string, InformacionTermino> indice;
        InfColeccionDocs informacionColeccionDocs;
        std::string pregunta;
        std::unordered_map<std::string, InformacionTerminoPregunta> indicePregunta;
        std::unordered_set<std::string> stopWords;
        std::string ficheroStopWords;
        Tokenizador tok;
        std::string directorioIndice;
        int tipoStemmer;
        bool almacenarEnDisco;
        bool almacenarPosTerm;
    public:
        IndexadorHash(const std::string& fichStopWords, const std::string& delimitadores,
                      const bool& detectComp, const bool& minuscSinAcentos, const std::string&
                      dirIndice, const int& tStemmer, const bool& almEnDisco, const bool&
                      almPosTerm);
        IndexadorHash(const std::string& directorioIndexacion);
        IndexadorHash(const IndexadorHash&);
        ~IndexadorHash();
        IndexadorHash& operator= (const IndexadorHash&);
        bool Indexar(const std::string& ficheroDocumentos);
        bool GuardarIndexacion() const;
        bool RecuperarIndexacion (const std::string& directorioIndexacion);
        bool IndexarPregunta(const std::string& preg);
        bool DevuelvePregunta(std::string& preg) const;
        bool DevuelvePregunta(InformacionPregunta& inf) const;
        void ImprimirIndexacionPregunta();
        void ImprimirPregunta();
        bool Devuelve(const std::string& word, InformacionTermino& inf) const;
        bool Devuelve(const std::string& word, const std::string& nomDoc, InfTermDoc& InfDoc) const;
        bool Existe(const std::string& word) const;
        bool Borra(const std::string& word);
        bool BorraDoc(const std::string& nomDoc);
        void VaciarIndiceDocs();
        void VaciarIndicePreg();
        bool Actualiza(const std::string& word, const InformacionTermino& inf);
        bool Inserta(const std::string& word, const InformacionTermino& inf);
        int NumPalIndexadas() const;
        std::string DevolverFichPalParada () const;
        void ListarPalParada() const;
        int NumPalParada() const;
        std::string DevolverDelimitadores () const;
        bool DevolverCasosEspeciales () const;
        bool DevolverPasarAminuscSinAcentos () const;
        bool DevolverAlmacenarPosTerm () const;
        std::string DevolverDirIndice () const;
        int DevolverTipoStemming () const;
        bool DevolverAlmEnDisco () const;
        void ListarInfColeccDocs() const;
        void ListarTerminos() const;
        bool ListarTerminos(const std::string& nomDoc) const;
        void ListarDocs() const;
        bool ListarDocs(const std::string& nomDoc) const;
};

#endif