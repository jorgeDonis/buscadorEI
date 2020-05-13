#ifndef __INDEXADOR_HASH__
#define __INDEXADOR_HASH__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "indexadorInformacion.h"
#include "tokenizador.h"
#include "stemmer.h"
#include <fstream>
#include <sstream>

class IndexadorHash
{
    friend std::ostream& operator<<(std::ostream&, const IndexadorHash&);
    friend class GestorFicheros;
    friend class Debugger;
    private:
        static size_t get_file_size(const std::string&);
        const static std::string NOMBRE_LISTA_FICHEROS;
        const static size_t MAX_PATHNAME_LEN = 256;
        const static std::string FICHERO_BINARIO_INDICE;
        IndexadorHash();
        //VERSIÓN QUE USA ALMACENAMIENTO SECUNDARIO
        static const uint32_t Prime = 0x01000193; //   16777619
        static const uint32_t Seed = 0x811C9DC5;  // 2166136261
        static inline uint32_t fnv1a(const char *, uint32_t);
        static const std::string DIRECTORIO_INDICE_DISCO;
        static const std::string DIRECTORIO_INDICE_DOCS_DISCO;
        std::unordered_set<std::string> indiceDisco;
        std::unordered_set<std::string> indiceDocsDisco;
        static void crear_directorios_indice();
        void ImprimirIndexacion_disco() const;
        bool GuardarIndexacion_disco() const;
        bool BorraDoc_disco(const std::string& nomDoc);
        void actualizar_indice_disco(const string &token, InfDoc &, int);
        bool indexar_documento_disco(const string &);
        bool indexar_documento_disco(InfDoc &, const string &);
        bool Indexar_disco(const std::string &ficheroDocumentos);
        //FIN VERSIÓN QUE USA ALMACENAMIENTO SECUNDARIO
        std::unordered_map<std::string, InformacionTermino> indice;
        std::unordered_map<std::string, InfDoc> indiceDocs;
        std::unordered_map<std::string, InformacionTerminoPregunta> indicePregunta;
        std::unordered_set<std::string> stopWords;
        Tokenizador tok;
        std::string pregunta;
        std::string ficheroStopWords;
        std::string directorioIndice;
        InfColeccionDocs informacionColeccionDocs;
        InformacionPregunta infPregunta;
        int tipoStemmer;
        bool almacenarEnDisco;
        bool almacenarPosTerm;
        stemmerPorter stemmer;
        void actualizar_indice_pregunta(const std::string&, size_t);
        inline void actualizar_indice(const string& token, InfDoc&, int);
        bool indexar_documento(const string&);
        bool indexar_documento(InfDoc&, const string&);
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
        std::string Pregunta() const { return pregunta; }
        bool Devuelve(const std::string& word, InformacionTermino& inf);
        bool Devuelve(const std::string& word, const std::string& nomDoc, InfTermDoc& InfDoc);
        bool Existe(const std::string& word);
        bool Borra(const std::string& word);
        bool BorraDoc(const std::string& nomDoc);
        void VaciarIndiceDocs() {indiceDocs.clear();}
        void VaciarIndicePreg();
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
    public:
        static void guardar(const std::string& foo, std::fstream& fichero_salida)
        {
            unsigned long int tam_string = foo.size() + 1;
            fichero_salida.write((const char*) &tam_string, sizeof(unsigned long int));
            fichero_salida.write(foo.c_str(), sizeof(char)* tam_string);
        }
        static void leer(std::string& foo, std::fstream& fichero_entrada)
        {
            unsigned long int tam_string;
            fichero_entrada.read((char*) &tam_string, sizeof(unsigned long int));
            char* foo_str = (char*) malloc(sizeof(char) * tam_string);
            fichero_entrada.read(foo_str, sizeof(char) * tam_string);
            foo = string(foo_str);
            free(foo_str);
        }
        static void guardar(const InfTermDoc&, std::fstream&);
        static void leer(InfTermDoc&, std::fstream&);
        static void guardar(const InformacionTermino&, std::fstream&);
        static void leer(InformacionTermino&, std::fstream&);
        static void guardar(const std::unordered_map<std::string, InformacionTermino>&, std::fstream&);
        static void leer(std::unordered_map<std::string, InformacionTermino>&, std::fstream&);
        static void guardar(const std::unordered_map<std::string, InfDoc>&, std::fstream&);
        static void leer(std::unordered_map<std::string, InfDoc>&, std::fstream&);
        static void guardar(const InformacionTerminoPregunta&, std::fstream&);
        static void leer(InformacionTerminoPregunta&, std::fstream&);
        static void guardar(const std::unordered_map<std::string, InformacionTerminoPregunta>&, std::fstream&);
        static void leer(std::unordered_map<std::string, InformacionTerminoPregunta>&, std::fstream&);
        static void guardar(const std::unordered_set<std::string>&, std::fstream&);
        static void leer(std::unordered_set<std::string>&, std::fstream&);
        static void guardar(const Tokenizador&, std::fstream&);
        static void leer(Tokenizador&, std::fstream&);
        static void guardar(const IndexadorHash&, std::fstream&);
        static void leer(IndexadorHash&, std::fstream&);
        static InformacionTermino leerInfoToken(const std::string&);
        static InfDoc leerInfoDoc(const std::string &);
        static void guardarInfoToken(const std::string&, const InformacionTermino &);
        static void guardarInfoDoc(const std::string&, const InfDoc &info);
};


#endif