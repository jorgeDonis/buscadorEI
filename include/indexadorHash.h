#ifndef __INDEXADOR_HASH__
#define __INDEXADOR_HASH__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "indexadorInformacion.h"
#include "tokenizador.h"
#include "stemmer.h"
#include <fstream>

class IndexadorHash
{
    friend std::ostream& operator<<(std::ostream&, const IndexadorHash&);
    friend class Debugger;
    private:
        static size_t get_file_size(const std::string&);
        static std::string NOMBRE_FICHERO_MAPA_INDICE;
        const static std::string NOMBRE_LISTA_FICHEROS;
        const static size_t MAX_PATHNAME_LEN = 256;
        const static std::string FICHERO_BINARIO_INDICE;
        IndexadorHash();
        std::unordered_map<std::string, InformacionTermino> indice;
        std::unordered_map<std::string, InfDoc> indiceDocs;
        InfColeccionDocs informacionColeccionDocs;
        std::string pregunta;
        std::unordered_map<std::string, InformacionTerminoPregunta> indicePregunta;
        InformacionPregunta infPregunta;
        std::unordered_set<std::string> stopWords;
        std::string ficheroStopWords;
        Tokenizador tok;
        std::string directorioIndice;
        int tipoStemmer;
        stemmerPorter stemmer;
        bool almacenarEnDisco;
        bool almacenarPosTerm;
        void actualizar_indice_pregunta(const std::string&, size_t);
        void actualizar_indice(const string& token, InfDoc&, int);
        bool indexar_documento(const string&);
        bool indexar_documento(InfDoc&, const string&);
        void guardar_mapa_indice() const; //TODO
        void copy_vals(const IndexadorHash&);
        /**
         * @brief Actualiza los atributos ficheroStopWords y stopWords
         * @return true Si la lectura es correcta
         * @return false Si el fichero no existe
         */
        bool leer_fichero_stopwords(const std::string&, const bool);
        /**
         * @brief Elimina toda la informaci�n relacionada con el documento.
         * Actualiza indice, iterando por todos
         * los tokens y reduciendo sus frecuencias. Actualiza informacionColeccionDoc
         */
        void eliminar_doc(const std::string&);
        static time_t ultima_modificacion(const std::string& fichero);
    public:
        IndexadorHash(const std::string &fichStopWords, const std::string &delimitadores,
                      const bool &detectComp, const bool &minuscSinAcentos, const std::string &dirIndice, const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm);
        IndexadorHash(const std::string& directorioIndexacion);
        IndexadorHash(const IndexadorHash& foo) {copy_vals(foo);}
        ~IndexadorHash() {;}
        IndexadorHash& operator= (const IndexadorHash&);
        bool Indexar(const std::string& ficheroDocumentos);
        bool IndexarDirectorio(const std::string&);
        bool GuardarIndexacion() const;
        bool RecuperarIndexacion (const std::string& directorioIndexacion);
        bool IndexarPregunta(const std::string&);
        bool DevuelvePregunta(std::string& preg) const;
        bool DevuelvePregunta(const std::string&, InformacionTerminoPregunta&);
        bool DevuelvePregunta(InformacionPregunta& inf) const;
        void ImprimirIndexacion() const;
        void ImprimirIndexacionPregunta() const;
        void ImprimirPregunta() const;
        bool Devuelve(const std::string& word, InformacionTermino& inf);
        bool Devuelve(const std::string& word, const std::string& nomDoc, InfTermDoc& InfDoc);
        bool Existe(const std::string& word);
        bool Borra(const std::string& word);
        bool BorraDoc(const std::string& nomDoc);
        void VaciarIndiceDocs() {indiceDocs.clear();}
        void VaciarIndicePreg() {indicePregunta.clear();}
        bool Actualiza(const std::string& word, const InformacionTermino& inf);
        bool Inserta(const std::string& word, const InformacionTermino& inf);
        int NumPalIndexadas() const {return indice.size();}
        std::string DevolverFichPalParada () const {return ficheroStopWords;}
        void ListarPalParada() const;
        int NumPalParada() const {return stopWords.size();}
        std::string DevolverDelimitadores () const {return tok.delimiters;}
        bool DevolverCasosEspeciales () const {return tok.casosEspeciales;}
        bool DevolverPasarAminuscSinAcentos () const {return tok.pasarAminuscSinAcentos;}
        bool DevolverAlmacenarPosTerm () const {return almacenarPosTerm;}
        std::string DevolverDirIndice () const {return directorioIndice;}
        int DevolverTipoStemming () const {return tipoStemmer;}
        bool DevolverAlmEnDisco () const {return almacenarEnDisco;}
        void ListarInfColeccDocs() const {cout << informacionColeccionDocs << endl;}
        void ListarTerminos() const;
        bool ListarTerminos(const std::string& nomDoc) const;
        void ListarDocs() const
        {
            for (unordered_map<string, InfDoc>::const_iterator it = indiceDocs.begin();
                it != indiceDocs.end(); ++it)
                cout << it->first << "\t" << it->second << endl;
        }
        bool ListarDocs(const std::string& nomDoc) const;
};

/**
 * @brief Gestiona la lectura y escritura de archivos y permite la serialización de
 * la parte privada del indexador
 */
class GestorFicheros
{
    private:
    public:
        void guardar(const InfTermDoc&, std::ofstream&);
        void leer(InfTermDoc&, std::ifstream&);
        void guardar(const InformacionTermino&, std::ofstream&);
        void leer(InformacionTermino&, std::ifstream&);
};

#endif