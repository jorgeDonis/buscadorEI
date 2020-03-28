#include "indexadorHash.h" 
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


using namespace std;

string IndexadorHash::NOMBRE_FICHERO_MAPA_INDICE = "indice.mapa";
const string IndexadorHash::NOMBRE_LISTA_FICHEROS = ".ficheros_corpus.lista";

size_t IndexadorHash::get_file_size(const string& filename)
{
    off_t file_size;
    char *buffer;
    struct stat stbuf;
    int fd;

    fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
    {
        cerr << "ERROR: no existe el fichero " << filename << endl;
        return -1;
    }
    fstat(fd, &stbuf);
    return stbuf.st_size;
}

time_t IndexadorHash::ultima_modificacion(const string& filename)
{
    struct stat attrib;
    stat(filename.c_str(), &attrib);
    return attrib.st_atim.tv_sec;
}

ostream& operator<<(ostream& os, const IndexadorHash& index)
{
    os << "Fichero con el listado de palabras de parada: " << index.ficheroStopWords << endl <<
    "Tokenizador: "<< index.tok << endl << 
    "Directorio donde se alamcenara el indice generado: " << index.directorioIndice << endl <<
    "Stemmer utilizado: " << index.tipoStemmer << endl <<
    "Informacion de la coleccion indexada: " << index.informacionColeccionDocs << endl <<
    "Se almacenara parte del indice en disco duro: " << index.almacenarEnDisco << endl <<
    "Se almacenaran las posiciones del terminos: " << index.almacenarPosTerm;
    return os;
}

IndexadorHash::IndexadorHash() : indice(), indiceDocs(), indicePregunta(), stopWords(), 
                                 informacionColeccionDocs(), tok(), stemmer(), infPregunta()
{
    pregunta = "";
    ficheroStopWords = "";
    directorioIndice = "";
    tipoStemmer = 0;
    almacenarEnDisco = almacenarPosTerm = false;
}

bool IndexadorHash::leer_fichero_stopwords(const string& filename, const bool minusc)
{
    if (!Tokenizador::file_exists(filename))
    {
        cerr << "ERROR: no existe el fichero " << filename << endl;
        return false;
    }
    ifstream file(filename);
    if (file.is_open())
    {
        ficheroStopWords = filename;
        string line;
        while (getline(file, line))
        {
            if (minusc)
                tok.minusc_sin_acentos(line);
            stemmer.stemmer(line, tipoStemmer);
            stopWords.insert(line);
        }
        file.close();
    }
    else
    {
        cerr << "ERROR: no se pudo abrir " << filename << endl;
        return false;
    }
    return true;
}

void IndexadorHash::copy_vals(const IndexadorHash& foo)
{
    indice = foo.indice;
    indiceDocs = foo.indiceDocs;
    informacionColeccionDocs = foo.informacionColeccionDocs;
    pregunta = foo.pregunta;
    indicePregunta = foo.indicePregunta;
    stopWords = foo.stopWords;
    ficheroStopWords = foo.ficheroStopWords;
    tok = foo.tok;
    directorioIndice = foo.directorioIndice;
    tipoStemmer = foo.tipoStemmer;
    almacenarEnDisco = foo.almacenarEnDisco;
    almacenarPosTerm = foo.almacenarPosTerm;
}

IndexadorHash::IndexadorHash(const std::string &fichStopWords, const std::string &delimitadores,
                             const bool &detectComp, const bool &minuscSinAcentos, const std::string &dirIndice,
                             const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm)
                             : indice(), indicePregunta(), stopWords(), informacionColeccionDocs(), tok(delimitadores, detectComp, minuscSinAcentos)
                             , stemmer(), indiceDocs(), infPregunta()
{
    if (leer_fichero_stopwords(fichStopWords, minuscSinAcentos))
    {
        if (dirIndice == "")
        {
            char wd[PATH_MAX];
            getcwd(wd, MAX_PATHNAME_LEN);
            directorioIndice = wd;
        }
        else
            directorioIndice = dirIndice;
        tipoStemmer = tStemmer;
        almacenarEnDisco = almEnDisco;
        almacenarPosTerm = almPosTerm;
    }
}

IndexadorHash& IndexadorHash::operator=(const IndexadorHash& foo)
{
    if (&foo != this)
    {
        this->~IndexadorHash();
        copy_vals(foo);
    }
    return *this;
}

void IndexadorHash::ImprimirIndexacion() const
{
    cout << "Terminos indexados: " << endl;
    unordered_map<string, InformacionTermino>::const_iterator it = indice.begin();
    while (it != indice.end())
    {
        cout << it->first << "\t" << it->second << endl;
        it++;
    }
    unordered_map<string, InfDoc>::const_iterator it_doc = indiceDocs.begin();
    while (it_doc != indiceDocs.end())
    {
        cout << it_doc->first << "\t" << it_doc->second << endl;
        it_doc++;
    }
}

bool IndexadorHash::GuardarIndexacion() const
{
    if (!Tokenizador::is_dir(directorioIndice))
    {
        if (mkdir(directorioIndice.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
        {
            cerr << "ERROR: el directorio " << directorioIndice << " no se pudo crear" << endl;
            return false;
        }
    }
    return false; //TODO IMPLEMENTAR GUARDAR INFORMACION
}

void IndexadorHash::eliminar_doc(const string& nombreDoc)
{
    list<string> tokens_vacios;
    long int ID = indiceDocs.find(nombreDoc)->second.idDoc;
    unordered_map<string, InformacionTermino>::iterator it;
    it = indice.begin();
    while (it != indice.end())
    {
        unordered_map<long int, InfTermDoc>::iterator it_doc = it->second.l_docs.find(ID);
        if (it_doc != it->second.l_docs.end())
        {
            it->second.ftc -= it_doc->second.ft;
            if (!it->second.ftc)
                tokens_vacios.push_back(it->first);
            informacionColeccionDocs.numTotalPal -= it_doc->second.ft;
            informacionColeccionDocs.numTotalPalSinParada -= it_doc->second.ft;
            it->second.l_docs.erase(ID);
        }
        it++;
    }
    informacionColeccionDocs.tamBytes -= indiceDocs[nombreDoc].tamBytes;
    for (const string& token : tokens_vacios)
        indice.erase(token);
}

/**
 * @brief Llamarï¿½ a indexar_documento(InfDoc) con un nuevo InfDoc
 * Registra una nueva fila en indiceDocs. 
 * 
 * @param nombreDoc Nombre del documento
 * @return true Sin fallos
 * @return false Falta de memoria al instertar en indiceDocs o
 * al llamar a indexar_documento(InfDoc)
 */
bool IndexadorHash::indexar_documento(const string& nombreDoc)
{
    pair<unordered_map<string, InfDoc>::iterator, bool> it;
    InfDoc infdoc;
    try
    {
        it = indiceDocs.insert(pair<string, InfDoc>(nombreDoc, infdoc));
        informacionColeccionDocs.numDocs++;
    }
    catch (bad_alloc& e)
    {
        cerr << "ERROR: falta de memoria al insertar en indiceDocs" << endl;
        return false;
    }
    return indexar_documento(it.first->second, nombreDoc);
}

void IndexadorHash::actualizar_infdoc(const string& token, InfDoc& infdoc)
{
    infdoc.numPalSinParada++;
    if (!infdoc.existe_token(token))
    {
        infdoc.numPalDiferentes++;
        infdoc.tokens.insert(token);
    }
}

void IndexadorHash::actualizar_indice(const string& token, const InfDoc& infdoc, int posTerm)
{
    unordered_map<string, InformacionTermino>::iterator it;
    it = indice.find(token);
    if (it == indice.end())
    {
        informacionColeccionDocs.numTotalPalDiferentes++;
        indice.emplace(token, InformacionTermino());
    }
    indice[token].ftc++;
    indice[token].l_docs[infdoc.idDoc].ft++;
    if (almacenarPosTerm)
        indice[token].l_docs[infdoc.idDoc].posTerm.push_back(posTerm);
}

/**
 * @brief Tokeniza e indexa el documento. Actualiza informacionColeccionDocs
 * 
 * @param infDoc Informaciï¿½n que serï¿½ actualizada. El ID se mantiene.
 * @param nombreDoc Nombre del documento
 * @return true Sin fallos
 * @return false Falta de memoria al insertar en indice
 */
bool IndexadorHash::indexar_documento(InfDoc& infDoc, const string& nombreDoc)
{
    try
    {
        int posTerm = -1;
        char* tokens = tok.TokenizarFichero(nombreDoc);
        unsigned tokens_it = 0;
        while (tokens[tokens_it] != '\0')
        {
            string token = "";
            while (tokens[tokens_it] != '\n')
                {
                    token += tokens[tokens_it];
                    tokens_it++;
                }
            stemmer.stemmer(token, tipoStemmer);
            tokens_it++;
            infDoc.numPal++;
            informacionColeccionDocs.numTotalPal++;
            posTerm++;
            if (stopWords.find(token) != stopWords.end())
                continue;
            informacionColeccionDocs.numTotalPalSinParada++;
            actualizar_infdoc(token, infDoc);
            actualizar_indice(token, infDoc, posTerm);
        }
        infDoc.tamBytes = get_file_size(nombreDoc);
        informacionColeccionDocs.tamBytes += infDoc.tamBytes;
        delete[] tokens;
    }
    catch (bad_alloc& e)
    {
        cerr << "ERROR: falta de memoria al indexar " << nombreDoc << endl;
        return false;
    }
    return true;
}

bool IndexadorHash::Indexar(const string& ficheroDocumentos)
{
    ifstream in_file(ficheroDocumentos);
    if (in_file.is_open())
    {
        string nombreDoc;
        while (getline(in_file, nombreDoc))
        {
            unordered_map<string, InfDoc>::iterator iterador = indiceDocs.find(nombreDoc);
            if (iterador != indiceDocs.end())
            {
                if (ultima_modificacion(nombreDoc) > iterador->second.fechaModificacion)
                {
                    eliminar_doc(nombreDoc);
                    indexar_documento(iterador->second, nombreDoc);
                }
            }
            else
                indexar_documento(nombreDoc);
        }
        in_file.close();
    }
    else
    {
        cerr << "ERROR: no existe el fichero " << ficheroDocumentos << endl;
        return false;
    }
    return true;
}

bool IndexadorHash::IndexarDirectorio(const string& directorio)
{
    if (!Tokenizador::file_exists(directorio))
    {
        cerr << "ERROR: el directorio " << directorio << " no existe" << endl;
        return false;
    }
    if (!Tokenizador::is_dir(directorio))
    {
        cerr << "ERROR: " << directorio << " no es un directorio" << endl;
        return false;
    }
    string cmd = "find " + directorio + " -type f > " + IndexadorHash::NOMBRE_LISTA_FICHEROS;
    system(cmd.c_str());
    return Indexar(IndexadorHash::NOMBRE_LISTA_FICHEROS);
}

/**
 * @brief Intenta insertar en la tabla indicePregunta [token, infTerminoPregunta]
 * 
 * @param token Insertado
 * @param pos Posición del token en la pregunta. Será necesario si almacenarPosTerm = true
 */
void IndexadorHash::actualizar_indice_pregunta(const string& token, size_t pos)
{
    unordered_map<string, InformacionTerminoPregunta>::iterator it;
    it = indicePregunta.find(token);
    if (it == indicePregunta.end())
    {
        infPregunta.numTotalPalDiferentes++;
        indicePregunta.emplace(token, InformacionTerminoPregunta());
    }
    indicePregunta[token].ft++;
    if (almacenarPosTerm)
        indicePregunta[token].posTerm.push_back(pos);
}

/**
 * @brief Actualiza pregunta, indicePregunta e infPregunta
 * 
 * @param pregunta 
 * @return true 
 * @return false si falta memoria
 */
bool IndexadorHash::IndexarPregunta(const string& pregunta)
{
    this->pregunta = pregunta;
    try
    {
        int posTerm = -1;
        char* tokens = tok.TokenizarString(pregunta + "\n");
        unsigned tokens_it = 0;
        while (tokens[tokens_it] != '\0')
        {
            string token = "";
            while (tokens[tokens_it] != '\n')
                {
                    token += tokens[tokens_it];
                    tokens_it++;
                }
            stemmer.stemmer(token, tipoStemmer);
            tokens_it++;
            infPregunta.numTotalPal++;
            posTerm++;
            if (stopWords.find(token) != stopWords.end())
                continue;
            infPregunta.numTotalPalSinParada++;
            actualizar_indice_pregunta(token, posTerm);
        }
        delete[] tokens;
    }
    catch (bad_alloc& e)
    {
        cerr << "ERROR: falta de memoria al indexar la pregunta" << endl;
        return false;
    }
    return true;
}

void IndexadorHash::ImprimirPregunta() const
{
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

void IndexadorHash::ImprimirIndexacionPregunta() const
{
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Terminos indexados en la pregunta: " << endl;
    unordered_map<string, InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin();
    while (it != indicePregunta.end())
    {
        cout << it->first << "\t" << it->second << endl;
        it++;
    }
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

