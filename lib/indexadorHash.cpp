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
#include <algorithm>

using namespace std;

const string IndexadorHash::NOMBRE_LISTA_FICHEROS = ".ficheros_corpus.lista";
const string IndexadorHash::FICHERO_BINARIO_INDICE = "indice.bin";
const string IndexadorHash::DIRECTORIO_INDICE_DISCO = "./indiceLocal/indice/";
const string IndexadorHash::DIRECTORIO_INDICE_DOCS_DISCO = "./indiceLocal/indiceDocs/";

inline uint32_t IndexadorHash::fnv1a(const char *text, uint32_t hash = Seed)
{
    const unsigned char *ptr = (const unsigned char *)text;
    while (*ptr)
        hash = (*ptr++ ^ hash) * Prime;
    return hash;
}

size_t IndexadorHash::get_file_size(const string& filename)
{
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
    "Directorio donde se almacenara el indice generado: " << index.directorioIndice << endl <<
    "Stemmer utilizado: " << index.tipoStemmer << endl <<
    "Informacion de la coleccion indexada: " << index.informacionColeccionDocs << endl <<
    "Se almacenara parte del indice en disco duro: " << index.almacenarEnDisco << endl <<
    "Se almacenaran las posiciones de los terminos: " << index.almacenarPosTerm << endl;
    return os;
}

IndexadorHash::IndexadorHash()
{
    tok.DelimitadoresPalabra(tok.delimiters);
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
    indiceDisco = foo.indiceDisco;
    indiceDocsDisco = foo.indiceDocsDisco;
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

void IndexadorHash::crear_directorios_indice()
{
    string cmd = "mkdir -p " + DIRECTORIO_INDICE_DISCO;
    system(cmd.c_str());
    cmd = "mkdir -p " + DIRECTORIO_INDICE_DOCS_DISCO;
    system(cmd.c_str());
}

IndexadorHash::IndexadorHash(const std::string &fichStopWords, const std::string &delimitadores,
                             const bool &detectComp, const bool &minuscSinAcentos, const std::string &dirIndice,
                             const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm)
                             : tok(delimitadores, detectComp, minuscSinAcentos)
{
    tipoStemmer = tStemmer;
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
        almacenarEnDisco = almEnDisco;
        almacenarPosTerm = almPosTerm;
    }
    if (almacenarEnDisco)
        crear_directorios_indice();
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
    if (almacenarEnDisco)
    {
        ImprimirIndexacion_disco();
        return;
    }
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
    if (almacenarEnDisco)
        return GuardarIndexacion_disco();
    if (!Tokenizador::is_dir(directorioIndice))
    {
        if (mkdir(directorioIndice.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
        {
            cerr << "ERROR: el directorio " << directorioIndice << " no se pudo crear" << endl;
            return false;
        }
    }
    fstream file(directorioIndice + "/" + FICHERO_BINARIO_INDICE, ios::out | ios::binary);
    if (file.is_open())
    {
        GestorFicheros::guardar(*this, file);
        file.close();
    }
    else
    {
        cerr << "ERROR: no se pudo crear el fichero índice: " << FICHERO_BINARIO_INDICE << endl;
        return false;
    }
    return true;
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion)
{
    if (!Tokenizador::file_exists(directorioIndexacion))
        cerr << "ERROR: el directorio " << directorioIndexacion << " no existe" << endl;
    else if (!Tokenizador::is_dir(directorioIndexacion))
        cerr << "ERROR: " << directorioIndexacion << " no es un directorio" << endl;
    else
    {
        fstream file(directorioIndexacion + "/" + FICHERO_BINARIO_INDICE, ios::in | ios::binary);
        if (file.is_open())
        {
            GestorFicheros::leer(*this, file);
            file.close();
        }
        else
            cerr << "ERROR: no se pudo abrir " << directorioIndexacion << endl;
    }
}

void IndexadorHash::eliminar_doc(const string& nombreDoc)
{
    list<string> tokens_vacios;
    unordered_map<string, InfDoc>::iterator doc_it = indiceDocs.find(nombreDoc);
    long int ID = doc_it->second.idDoc;
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
            it->second.l_docs.erase(ID);
        }
        it++;
    }
    informacionColeccionDocs.numTotalPal -= doc_it->second.numPal;
    informacionColeccionDocs.numTotalPalSinParada -= doc_it->second.numPalSinParada;
    informacionColeccionDocs.tamBytes -= indiceDocs[nombreDoc].tamBytes;
    for (const string& token : tokens_vacios)
    {
        indice.erase(token);
        informacionColeccionDocs.numTotalPalDiferentes--;
    }
    indiceDocs.erase(doc_it);
    informacionColeccionDocs.numDocs--;
}

/**
 * @brief Llamar� a indexar_documento(InfDoc) con un nuevo InfDoc
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

inline void IndexadorHash::actualizar_indice(const string& token, InfDoc& infdoc, int posTerm)
{
    infdoc.numPalSinParada++;
    unordered_map<string, InformacionTermino>::iterator it;
    it = indice.find(token);
    if (it == indice.end())
    {
        informacionColeccionDocs.numTotalPalDiferentes++;
        it = indice.emplace(piecewise_construct, forward_as_tuple(token), forward_as_tuple()).first;
    }
    it->second.ftc++;

    unordered_map<long, InfTermDoc>::iterator it_doc;
    it_doc = it->second.l_docs.find(infdoc.idDoc);
    if (it_doc == it->second.l_docs.end())
    {
        infdoc.numPalDiferentes++;
        it_doc = it->second.l_docs.emplace(piecewise_construct,
         forward_as_tuple(infdoc.idDoc), forward_as_tuple()).first;
    }
    it_doc->second.ft++;
    if (almacenarPosTerm)
        it_doc->second.posTerm.push_back(posTerm);
}

/**
 * @brief Tokeniza e indexa el documento. Actualiza informacionColeccionDocs
 * 
 * @param infDoc Informaci�n que ser� actualizada. El ID se mantiene.
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
            while (tokens[tokens_it] != 30)
                {
                    token += tokens[tokens_it];
                    tokens_it++;
                }
            if (token.length() == 0)
                break;
            stemmer.stemmer(token, tipoStemmer);
            tokens_it++;
            infDoc.numPal++;
            posTerm++;
            if (stopWords.find(token) != stopWords.end())
                continue;
            informacionColeccionDocs.numTotalPalSinParada++;
            actualizar_indice(token, infDoc, posTerm);
        }
        infDoc.tamBytes = get_file_size(nombreDoc);
        informacionColeccionDocs.tamBytes += infDoc.tamBytes;
        informacionColeccionDocs.numTotalPal += infDoc.numPal;
        free(tokens);
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
    if (almacenarEnDisco)
        return Indexar_disco(ficheroDocumentos);
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
    string cmd = "find " + directorio + " -type f | sort > " + IndexadorHash::NOMBRE_LISTA_FICHEROS;
    system(cmd.c_str());
    return Indexar(IndexadorHash::NOMBRE_LISTA_FICHEROS);
}

/**
 * @brief Intenta insertar en la tabla indicePregunta [token, infTerminoPregunta]
 * 
 * @param token Insertado
 * @param pos Posici�n del token en la pregunta. Ser� necesario si almacenarPosTerm = true
 */
void IndexadorHash::actualizar_indice_pregunta(const string& token, size_t pos)
{
    unordered_map<string, InformacionTerminoPregunta>::iterator it;
    it = indicePregunta.find(token);
    if (it == indicePregunta.end())
    {
        infPregunta.numTotalPalDiferentes++;
        indicePregunta.emplace(piecewise_construct, forward_as_tuple(token), forward_as_tuple());
    }
    indicePregunta[token].ft++;
    if (almacenarPosTerm)
        indicePregunta[token].posTerm.push_back(pos);
}

void IndexadorHash::VaciarIndicePreg()
{
    infPregunta.numTotalPal = infPregunta.numTotalPalDiferentes = infPregunta.numTotalPalSinParada = 0;
    indicePregunta.clear();
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
    VaciarIndicePreg();
    this->pregunta = pregunta;
    try
    {
        int posTerm = -1;
        char* tokens = tok.TokenizarString(pregunta);
        unsigned tokens_it = 0;
        while (tokens[tokens_it] != '\0')
        {
            string token = "";
            while (tokens[tokens_it] != 30)
                {
                    token += tokens[tokens_it];
                    tokens_it++;
                }
            if (token == "")
                break;
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

bool IndexadorHash::DevuelvePregunta(string& preg) const
{
    if (indicePregunta.size() > 0)
    {
        preg = pregunta;
        return true;
    }
    return false;
}

bool IndexadorHash::DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    unordered_map<string, InformacionTerminoPregunta>::const_iterator it;
    it = indicePregunta.find(word_minusc);
    if (it == indicePregunta.end())
        return false;
    else
        inf = it->second;
    return true;
}

bool IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) const
{
    if (indicePregunta.size() > 0)
    {
        inf = infPregunta;
        return true;
    }
    return false;
}

bool IndexadorHash::Devuelve(const string& word, InformacionTermino& inf)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    if (!almacenarEnDisco)
    {
        unordered_map<string, InformacionTermino>::const_iterator it;
        it = indice.find(word_minusc);
        if (it == indice.end())
            return false;
        inf = it->second;
        return true;
    }
    if (indiceDisco.find(word_minusc) == indiceDisco.end())
        return false;
    inf = GestorFicheros::leerInfoToken(word_minusc);
    return true;
}

bool IndexadorHash::Devuelve(const string& word, const string& nomDoc, InfTermDoc& infDoc)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    if (!almacenarEnDisco)
    {
        unordered_map<string, InformacionTermino>::const_iterator it;
        it = indice.find(word_minusc);
        if (it == indice.end())
            return false;
        else
        {
            unordered_map<string, InfDoc>::const_iterator doc_it;
            doc_it = indiceDocs.find(nomDoc);
            if (doc_it == indiceDocs.end())
                return false;
            long ID = doc_it->second.idDoc;
            unordered_map<long, InfTermDoc>::const_iterator term_doc_it;
            term_doc_it = it->second.l_docs.find(ID);
            if (term_doc_it == it->second.l_docs.end())
                return false;
            infDoc = term_doc_it->second;
        }
        return true;
    }
    if (indiceDisco.find(word_minusc) == indiceDisco.end()
        || indiceDocsDisco.find(nomDoc) == indiceDocsDisco.end())
        return false;
    InformacionTermino infoTerm = GestorFicheros::leerInfoToken(word_minusc);
    InfDoc infoDoc = GestorFicheros::leerInfoDoc(nomDoc);
    unordered_map<long int, InfTermDoc>::const_iterator it = infoTerm.l_docs.find(infoDoc.idDoc);
    if (it == infoTerm.l_docs.end())
        return false;
    infDoc = it->second;
    return true;
}

bool IndexadorHash::Existe(const std::string &word)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    if (!almacenarEnDisco)
        return indice.find(word_minusc) != indice.end();
    return indiceDisco.find(word_minusc) != indiceDisco.end();
}

bool IndexadorHash::Borra(const std::string &word)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    if (!almacenarEnDisco)
    {
        unordered_map<string, InformacionTermino>::const_iterator it;
        it = indice.find(word_minusc);
        if (it == indice.end())
            return false;
        indice.erase(it);
        return true;
    }
    if (indiceDisco.find(word) == indiceDisco.end())
        return false;
    indiceDisco.erase(word_minusc);
    string cmd = "rm " + DIRECTORIO_INDICE_DISCO + word_minusc;
    system(cmd.c_str());
    return true;
}

bool IndexadorHash::BorraDoc(const std::string &nomDoc)
{
    if (almacenarEnDisco)
        return BorraDoc_disco(nomDoc);
    unordered_map<string, InfDoc>::const_iterator it = indiceDocs.find(nomDoc);
    if (it == indiceDocs.end())
        return false;
    eliminar_doc(nomDoc);
    return true;
}

bool IndexadorHash::Actualiza(const std::string &word, const InformacionTermino &inf)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    if (almacenarEnDisco)
    {
        unordered_map<string, InformacionTermino>::const_iterator it;
        it = indice.find(word_minusc);
        if (it == indice.end())
            return false;
        indice[word_minusc] = inf;
        return true;
    }
    if (indiceDisco.find(word_minusc) == indiceDisco.end())
        return false;
    GestorFicheros::guardarInfoToken(word_minusc, inf);
    return true;
}

bool IndexadorHash::Inserta(const std::string &word, const InformacionTermino &inf)
{
    string word_minusc = word;
    Tokenizador::minusc_sin_acentos(word_minusc);
    stemmer.stemmer(word_minusc, tipoStemmer);
    if (!almacenarEnDisco)
    {
        unordered_map<string, InformacionTermino>::const_iterator it;
        it = indice.find(word_minusc);
        if (it != indice.end())
            return false;
        indice[word_minusc] = inf;
        return true;
    }
    if (indiceDisco.find(word_minusc) != indiceDisco.end())
        return false;
    GestorFicheros::guardarInfoToken(word_minusc, inf);
    return true;
}

void IndexadorHash::ListarPalParada() const
{
    ifstream file(ficheroStopWords);
    if (file.is_open())
    {
        string word;
        while (getline(file, word))
            cout << word << endl;
        file.close();
    }
    else
        cerr << "ERROR: no se pudo abrir el fichero de stopwords: " << ficheroStopWords << endl;
}

void IndexadorHash::ListarTerminos() const
{
    if (!almacenarEnDisco)
    {
        unordered_map<string, InformacionTermino>::const_iterator it;
        it = indice.begin();
        while (it != indice.end())
        {
            cout << it->first << "\t" << it->second << endl;
            it++;
        }
    }
    else
    {
        for (const string& token : indiceDisco)
        {
            InformacionTermino infoToken = GestorFicheros::leerInfoToken(token);
            cout << token << "\t" << infoToken << endl;
        }
    }
}

bool IndexadorHash::ListarTerminos(const std::string &nomDoc) const
{
    if (!almacenarEnDisco)
    {
        unordered_map<string, InfDoc>::const_iterator it;
        it = indiceDocs.find(nomDoc);
        if (it == indiceDocs.end())
            return false;
        long id_doc = it->second.idDoc;
        for (unordered_map<string, InformacionTermino>::const_iterator it_ind = indice.begin();
            it_ind != indice.end(); ++it_ind)
        {
            if (it_ind->second.l_docs.find(id_doc) != it_ind->second.l_docs.end())
                cout << it_ind->first << "\t" << it_ind->second << endl;
        }
        return true;
    }
    InfDoc infoDoc = GestorFicheros::leerInfoDoc(nomDoc);
    if (indiceDocsDisco.find(nomDoc) == indiceDocsDisco.end())
        return false;
    for (const string& token : indiceDisco)
    {
        InformacionTermino infoToken = GestorFicheros::leerInfoToken(token);
        unordered_map<long int, InfTermDoc>::const_iterator it = infoToken.l_docs.find(infoDoc.idDoc);
        if (it != infoToken.l_docs.end())
            cout << token << "\t" << infoToken << endl;
    }
    return true;
}

bool IndexadorHash::ListarDocs(const std::string& nomDoc) const
{
    if (!almacenarEnDisco)
    {
        unordered_map<string, InfDoc>::const_iterator it;
        it = indiceDocs.find(nomDoc);
        if (it == indiceDocs.end())
            return false;
        cout << nomDoc << "\t" << it->second << endl;
        return true;
    }
    if (indiceDocsDisco.find(nomDoc) == indiceDocsDisco.end())
        return false;
    InfDoc infoDoc = GestorFicheros::leerInfoDoc(nomDoc);
    cout << nomDoc << "\t" << infoDoc << endl;
    return true;
}

/**
 * @brief Guardar� primero un int con ft, luego un unsigned long int con el tama�o en bytes
 * de la lista y posteriormente todos los int de posTerm
 * 
 * @param inftermdoc 
 */
void GestorFicheros::guardar(const InfTermDoc& inftermdoc, fstream& fichero_salida)
{
    fichero_salida.write((const char*) &inftermdoc.ft, sizeof(int));
    unsigned long int pos_size = inftermdoc.posTerm.size() * sizeof(int);
    fichero_salida.write((const char*) &pos_size, sizeof(unsigned long int));
    int* posiciones = (int*) malloc(pos_size);
    size_t i = 0;
    for (const int& pos : inftermdoc.posTerm)
    {
        posiciones[i] = pos;
        i++;
    }
    fichero_salida.write((const char*) posiciones, pos_size);
    free(posiciones);
}

void GestorFicheros::leer(InfTermDoc& inftermdoc, fstream& fichero_entrada)
{
    fichero_entrada.read((char*) &inftermdoc.ft, sizeof(int));
    unsigned long int pos_size;
    fichero_entrada.read((char*) &pos_size, sizeof(unsigned long int));
    size_t pos_elements = pos_size / sizeof(int);
    int* posiciones = (int*) malloc(pos_size);
    fichero_entrada.read((char*) posiciones, pos_size);
    for (size_t i = 0; i < pos_elements; i++)
        inftermdoc.posTerm.push_back(posiciones[i]);
    free(posiciones);
}

void GestorFicheros::guardar(const InformacionTermino& informaciontermino, fstream& fichero_salida)
{
    fichero_salida.write((const char*) &informaciontermino.ftc, sizeof(int));
    unsigned long int l_docs_size = informaciontermino.l_docs.size();
    fichero_salida.write((const char*) &l_docs_size, sizeof(unsigned long int)); //escribo el numero de entradas
    for (const pair<long int, InfTermDoc>& entry : informaciontermino.l_docs)
    {
        fichero_salida.write((const char*) &entry.first, sizeof(long int));
        guardar(entry.second, fichero_salida);
    }
}

void GestorFicheros::leer(InformacionTermino& informaciontermino, fstream& fichero_entrada)
{
    fichero_entrada.read(( char*) &informaciontermino.ftc, sizeof(int));
    unsigned long int l_docs_size;
    fichero_entrada.read(( char*) &l_docs_size, sizeof(unsigned long int));
    for (size_t i = 0; i < l_docs_size; i++)
    {
        long int id_doc;
        InfTermDoc inftermdoc;
        fichero_entrada.read((char*) &id_doc, sizeof(long int));
        leer(inftermdoc, fichero_entrada);
        informaciontermino.l_docs.emplace(id_doc, inftermdoc);
    }
}

void GestorFicheros::guardar(const std::unordered_map<std::string, InformacionTermino>& indice, std::fstream& fichero_salida)
{
    unsigned long int numero_entradas = indice.size();
    fichero_salida.write((const char*) &numero_entradas, sizeof(unsigned long int));
    for (const pair<string, InformacionTermino>& entrada : indice)
    {
        int tam_string = entrada.first.length() + 1;
        fichero_salida.write((const char*) &tam_string, sizeof(int));
        fichero_salida.write((const char*) entrada.first.c_str(), tam_string * sizeof(char));
        guardar(entrada.second, fichero_salida);
    }
}

void GestorFicheros::leer(std::unordered_map<std::string, InformacionTermino>& indice, std::fstream& fichero_entrada)
{
    unsigned long int numero_entradas;
    fichero_entrada.read((char*) &numero_entradas, sizeof(unsigned long int));
    for (size_t i = 0; i < numero_entradas; i++)
    {
        int tam_string;
        fichero_entrada.read((char*) &tam_string, sizeof(int));
        size_t tam_byte_str = sizeof(char) * tam_string;
        char* token = (char*) malloc(tam_byte_str);
        fichero_entrada.read(token, tam_byte_str);
        InformacionTermino informaciontermino;
        leer(informaciontermino, fichero_entrada);
        indice.emplace(string(token), informaciontermino);
        free(token);
    }
}

void GestorFicheros::guardar(const std::unordered_map<std::string, InfDoc>& indiceDocs, std::fstream& fichero_salida)
{
    unsigned long int numero_entradas = indiceDocs.size();
    fichero_salida.write((const char*) &numero_entradas, sizeof(unsigned long int));
    for (const pair<string, InfDoc>& entrada : indiceDocs)
    {
        int tam_string = entrada.first.length() + 1;
        fichero_salida.write((const char*) &tam_string, sizeof(int));
        fichero_salida.write((const char*) entrada.first.c_str(), tam_string * sizeof(char));
        fichero_salida.write((const char*) &entrada.second, sizeof(InfDoc));
    }
}

void GestorFicheros::leer(std::unordered_map<std::string, InfDoc>& indiceDocs, std::fstream& fichero_entrada)
{
    unsigned long int numero_entradas;
    fichero_entrada.read((char*) &numero_entradas, sizeof(unsigned long int));
    for (size_t i = 0; i < numero_entradas; i++)
    {
        int tam_string;
        fichero_entrada.read((char*) &tam_string, sizeof(int));
        size_t tam_byte_str = sizeof(char) * tam_string;
        char* nombreDoc = (char*) malloc(tam_byte_str);
        fichero_entrada.read(nombreDoc, tam_byte_str);
        InfDoc infDoc;
        fichero_entrada.read((char*) &infDoc, sizeof(InfDoc));
        indiceDocs.emplace(string(nombreDoc), infDoc);
        free(nombreDoc);
    }
}

void GestorFicheros::guardar(const InformacionTerminoPregunta& inf, std::fstream& fichero_salida)
{
    fichero_salida.write((const char*) &inf.ft, sizeof(int));
    unsigned long int numero_pos = inf.posTerm.size();
    fichero_salida.write((const char*) &numero_pos, sizeof(unsigned long int));
    int* posiciones = (int*) malloc(sizeof(int) * numero_pos);
    size_t i = 0;
    for (const int& pos : inf.posTerm)
    {
        posiciones[i] = pos;
        i++;
    }
    fichero_salida.write((const char*) posiciones, sizeof(int) * numero_pos);
    free(posiciones);
}

void GestorFicheros::leer(InformacionTerminoPregunta& inf, std::fstream& fichero_entrada)
{
    fichero_entrada.read((char*) &inf.ft, sizeof(int));
    unsigned long int numero_pos;
    fichero_entrada.read((char*) &numero_pos, sizeof(unsigned long int));
    int* posiciones = (int*) malloc(sizeof(int) * numero_pos);
    fichero_entrada.read((char*) posiciones, sizeof(int) * numero_pos);
    for (size_t i = 0; i < numero_pos; i++)
        inf.posTerm.push_back(posiciones[i]);
    free(posiciones);
}

void GestorFicheros::guardar(const std::unordered_map<std::string, InformacionTerminoPregunta>& indicePregunta, std::fstream& fichero_salida)
{
    unsigned long int numero_entradas = indicePregunta.size();
    fichero_salida.write((const char*) &numero_entradas, sizeof(unsigned long int));
    for (const pair<string, InformacionTerminoPregunta>& entrada : indicePregunta)
    {
        int tam_string = entrada.first.length() + 1;
        fichero_salida.write((const char*) &tam_string, sizeof(int));
        fichero_salida.write((const char*) entrada.first.c_str(), tam_string * sizeof(char));
        guardar(entrada.second, fichero_salida);
    }
}

void GestorFicheros::leer(std::unordered_map<std::string, InformacionTerminoPregunta>& indicePregunta, std::fstream& fichero_entrada)
{
    unsigned long int numero_entradas;
    fichero_entrada.read((char*) &numero_entradas, sizeof(unsigned long int));
    for (size_t i = 0; i < numero_entradas; i++)
    {
        int tam_string;
        fichero_entrada.read((char*) &tam_string, sizeof(int));
        size_t tam_byte_str = sizeof(char) * tam_string;
        char* nombreDoc = (char*) malloc(tam_byte_str);
        fichero_entrada.read(nombreDoc, tam_byte_str);
        InformacionTerminoPregunta infPregunta;
        leer(infPregunta, fichero_entrada);
        indicePregunta.emplace(string(nombreDoc), infPregunta);
        free(nombreDoc);
    }
}

void GestorFicheros::guardar(const std::unordered_set<std::string>& stopwords, std::fstream& fichero_salida)
{
    unsigned long int num_stopwords = stopwords.size();
    fichero_salida.write((const char*) &num_stopwords, sizeof(unsigned long int));
    for (const string& stopword : stopwords)
    {
        int tam_string = stopword.length() + 1;
        fichero_salida.write((const char*) &tam_string, sizeof(int));
        fichero_salida.write((const char*) stopword.c_str(), tam_string * sizeof(char));
    }
}

void GestorFicheros::leer(std::unordered_set<std::string>& stopwords, std::fstream& fichero_entrada)
{
    unsigned long int num_stopwords;
    fichero_entrada.read((char*) &num_stopwords, sizeof(unsigned long int));
    for (size_t i = 0; i < num_stopwords; i++)
    {
        int tam_string;
        fichero_entrada.read((char*) &tam_string, sizeof(int));
        char* stopword = (char*) malloc (tam_string * sizeof(char));
        fichero_entrada.read((char*) stopword, tam_string * sizeof(char));
        stopwords.emplace(string(stopword));
        free(stopword);
    }
}

void GestorFicheros::guardar(const Tokenizador& tok, std::fstream& fichero_salida)
{
    unsigned long int tam_delimitadores = tok.delimiters.size() + 1;
    fichero_salida.write((const char*) &tam_delimitadores, sizeof(unsigned long int));
    fichero_salida.write((const char*) tok.delimiters.c_str(), sizeof(char) * tam_delimitadores);
    fichero_salida.write((const char*) tok.delimiters_set, sizeof(short) * 256);
    fichero_salida.write((const char *)tok.delimiters_set_plus_null, sizeof(short) * 256);
    fichero_salida.write((const char*) &tok.casosEspeciales, sizeof(bool));
    fichero_salida.write((const char*) &tok.pasarAminuscSinAcentos, sizeof(bool));
}

void GestorFicheros::leer(Tokenizador& tok, std::fstream& fichero_entrada)
{
    unsigned long int tam_delimitadores;
    fichero_entrada.read((char*) &tam_delimitadores, sizeof(unsigned long int));
    char* delimitadores = (char*) malloc(sizeof(char) * tam_delimitadores);
    fichero_entrada.read(delimitadores, sizeof(char) * tam_delimitadores);
    tok.delimiters = string(delimitadores);
    free(delimitadores);
    fichero_entrada.read((char * ) tok.delimiters_set, sizeof(short) * 256);
    fichero_entrada.read((char *)tok.delimiters_set_plus_null, sizeof(short) * 256);
    fichero_entrada.read((char*) &tok.casosEspeciales, sizeof(bool));
    fichero_entrada.read((char*) &tok.pasarAminuscSinAcentos, sizeof(bool));
}

void GestorFicheros::guardar(const IndexadorHash& index, std::fstream& fichero_salida)
{
    guardar(index.indice, fichero_salida);
    guardar(index.indiceDocs, fichero_salida);
    guardar(index.indicePregunta, fichero_salida);
    guardar(index.stopWords, fichero_salida);
    guardar(index.tok, fichero_salida);
    guardar(index.pregunta, fichero_salida);
    guardar(index.ficheroStopWords, fichero_salida);
    guardar(index.directorioIndice, fichero_salida);
    fichero_salida.write((const char*) &index.informacionColeccionDocs, sizeof(InfColeccionDocs));
    fichero_salida.write((const char*) &index.infPregunta, sizeof(InformacionPregunta));
    fichero_salida.write((const char*) &index.tipoStemmer, sizeof(int));
    fichero_salida.write((const char*) &index.almacenarEnDisco, sizeof(bool));
    fichero_salida.write((const char*) &index.almacenarPosTerm, sizeof(bool));
}

void GestorFicheros::leer(IndexadorHash& index, std::fstream& fichero_entrada)
{
    leer(index.indice, fichero_entrada);
    leer(index.indiceDocs, fichero_entrada);
    leer(index.indicePregunta, fichero_entrada);
    leer(index.stopWords, fichero_entrada);
    leer(index.tok, fichero_entrada);
    leer(index.pregunta, fichero_entrada);
    leer(index.ficheroStopWords, fichero_entrada);
    leer(index.directorioIndice, fichero_entrada);
    fichero_entrada.read((char*) &index.informacionColeccionDocs, sizeof(InfColeccionDocs));
    fichero_entrada.read((char*) &index.infPregunta, sizeof(InformacionPregunta));
    fichero_entrada.read((char*) &index.tipoStemmer, sizeof(int));
    fichero_entrada.read((char*) &index.almacenarEnDisco, sizeof(bool));
    fichero_entrada.read((char*) &index.almacenarPosTerm, sizeof(bool));    
}


//VERSIÓN QUE GUARDA EL ÍNDICE EN MEMORIA SECUNDARIA

InformacionTermino GestorFicheros::leerInfoToken(const std::string & token)
{
    string nombreFichero = to_string(IndexadorHash::fnv1a(token.c_str()));
    string fichero = IndexadorHash::DIRECTORIO_INDICE_DISCO + nombreFichero;
    fstream fichero_entrada(fichero, ios::binary | ios::in);
    InformacionTermino info;
    leer(info, fichero_entrada);
    fichero_entrada.close();
    return info;
}

InfDoc GestorFicheros::leerInfoDoc(const std::string & nombreDoc)
{
    string nombreDocFormat = to_string(IndexadorHash::fnv1a(nombreDoc.c_str()));
    string fichero = IndexadorHash::DIRECTORIO_INDICE_DOCS_DISCO + nombreDocFormat;
    fstream fichero_entrada(fichero, ios::binary | ios::in);
    InfDoc info;
    fichero_entrada.read((char*) &info, sizeof(InfDoc));
    fichero_entrada.close();
    return info;
}

void GestorFicheros::guardarInfoToken(const string &token, const InformacionTermino &info)
{
    string nombreFichero = to_string(IndexadorHash::fnv1a(token.c_str()));
    string fichero = IndexadorHash::DIRECTORIO_INDICE_DISCO + nombreFichero;
    fstream fichero_salida(fichero, ios::binary | ios::out | ios::trunc);
    guardar(info, fichero_salida);
    fichero_salida.close();
}

void GestorFicheros::guardarInfoDoc(const string& nombreDoc, const InfDoc & info)
{
    string nombreDocFormat = to_string(IndexadorHash::fnv1a(nombreDoc.c_str()));
    string fichero = IndexadorHash::DIRECTORIO_INDICE_DOCS_DISCO + nombreDocFormat;
    fstream fichero_salida(fichero, ios::binary | ios::out | ios::trunc);
    fichero_salida.write((const char*) &info, sizeof(InfDoc));
    fichero_salida.close();
}

void IndexadorHash::ImprimirIndexacion_disco() const
{
    cout << "Terminos indexados: " << endl;
    for (const string& fila : indiceDisco)
    {
        InformacionTermino info = GestorFicheros::leerInfoToken(fila);
        cout << fila << "\t" << info << endl;
    }
    for (const string& fila : indiceDocsDisco)
    {
        InfDoc info = GestorFicheros::leerInfoDoc(fila);
        cout << fila << "\t" << info << endl;
    }
}

bool IndexadorHash::GuardarIndexacion_disco() const
{
    return false;
    // if (!Tokenizador::is_dir(directorioIndice))
    // {
    //     if (mkdir(directorioIndice.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
    //     {
    //         cerr << "ERROR: el directorio " << directorioIndice << " no se pudo crear" << endl;
    //         return false;
    //     }
    // }
    // fstream fichero_salida(directorioIndice + "/" + FICHERO_BINARIO_INDICE, ios::out | ios::binary);
    // if (fichero_salida.is_open())
    // {
    //     unsigned numTokens = indiceDisco.size();
    //     fichero_salida.write((const char*) &numTokens, sizeof(unsigned));
    //     for (const string& token : indiceDisco)
    //     {
    //         InformacionTermino info = GestorFicheros::leerInfoToken(token);
    //         GestorFicheros::guardar(info, fichero_salida);
    //     }
    //     unsigned numDocs = indiceDocsDisco.size();
    //     fichero_salida.write((const char*) &numDocs, sizeof(unsigned));
    //     for (const string& ficheroDoc : indiceDocsDisco)
    //     {
    //         InfDoc info = GestorFicheros::leerInfoDoc(ficheroDoc);
    //         fichero_salida.write((const char*) &info, sizeof(InfDoc));
    //     }
    //     GestorFicheros::guardar(indicePregunta, fichero_salida);
    //     GestorFicheros::guardar(stopWords, fichero_salida);
    //     GestorFicheros::guardar(tok, fichero_salida);
    //     GestorFicheros::guardar(pregunta, fichero_salida);
    //     GestorFicheros::guardar(ficheroStopWords, fichero_salida);
    //     GestorFicheros::guardar(directorioIndice, fichero_salida);
    //     fichero_salida.write((const char *)&informacionColeccionDocs, sizeof(InfColeccionDocs));
    //     fichero_salida.write((const char *)&infPregunta, sizeof(InformacionPregunta));
    //     fichero_salida.write((const char *)&tipoStemmer, sizeof(int));
    //     fichero_salida.write((const char *)&almacenarEnDisco, sizeof(bool));
    //     fichero_salida.write((const char *)&almacenarPosTerm, sizeof(bool));
    //     fichero_salida.close();
    // }
    // else
    // {
    //     cerr << "ERROR: no se pudo crear el fichero índice: " << FICHERO_BINARIO_INDICE << endl;
    //     return false;
    // }
    // return true;
}

bool IndexadorHash::BorraDoc_disco(const std::string &nomDoc)
{
    if (indiceDocsDisco.find(nomDoc) == indiceDocsDisco.end())
        return false;
    list<string> tokens_vacios;
    InfDoc infoDoc = GestorFicheros::leerInfoDoc(nomDoc);
    for (const string& token : indiceDisco)
    {
        InformacionTermino info = GestorFicheros::leerInfoToken(token);
        unordered_map<long int, InfTermDoc>::iterator it_doc = info.l_docs.find(infoDoc.idDoc);
        if (it_doc != info.l_docs.end())
        {
            info.ftc--;
            if (!info.ftc)
                tokens_vacios.push_back(token);
            info.l_docs.erase(infoDoc.idDoc);
            GestorFicheros::guardarInfoToken(token, info);
        }
    }
    informacionColeccionDocs.numTotalPal -= infoDoc.numPal;
    informacionColeccionDocs.numTotalPalSinParada -= infoDoc.numPalSinParada;
    informacionColeccionDocs.tamBytes -= infoDoc.tamBytes;
    for (const string &token : tokens_vacios)
    {
        string cmd = "rm " + DIRECTORIO_INDICE_DISCO + token;
        system(cmd.c_str());
        indiceDisco.erase(token);
        informacionColeccionDocs.numTotalPalDiferentes--;
    }
    indiceDocsDisco.erase(nomDoc);
    string cmd = "rm " + DIRECTORIO_INDICE_DOCS_DISCO + nomDoc;
    system(cmd.c_str());
    informacionColeccionDocs.numDocs--;
    return true;
}

bool IndexadorHash::indexar_documento_disco(const string &nombreDoc)
{
    indiceDocsDisco.insert(nombreDoc);
    InfDoc infDoc;
    return indexar_documento_disco(infDoc, nombreDoc);
}

void IndexadorHash::actualizar_indice_disco(const string &token, InfDoc &infdoc, int posTerm)
{
    infdoc.numPalSinParada++;
    InformacionTermino infoTerm;
    unordered_set<string>::iterator it;
    it = indiceDisco.find(token);
    if (it == indiceDisco.end())
    {
        informacionColeccionDocs.numTotalPalDiferentes++;
        it = indiceDisco.insert(token).first;
    }
    else
        infoTerm = GestorFicheros::leerInfoToken(token);
    infoTerm.ftc++;

    unordered_map<long, InfTermDoc>::iterator it_doc;
    it_doc = infoTerm.l_docs.find(infdoc.idDoc);
    if (it_doc == infoTerm.l_docs.end())
    {
        infdoc.numPalDiferentes++;
        it_doc = infoTerm.l_docs.emplace(infdoc.idDoc, InfTermDoc()).first;
    }
    it_doc->second.ft++;
    if (almacenarPosTerm)
        it_doc->second.posTerm.push_back(posTerm);
    GestorFicheros::guardarInfoToken(token, infoTerm);
}

bool IndexadorHash::indexar_documento_disco(InfDoc &infDoc, const string &nombreDoc)
{
    try
    {
        int posTerm = -1;
        char *tokens = tok.TokenizarFichero(nombreDoc);
        unsigned tokens_it = 0;
        while (tokens[tokens_it] != '\0')
        {
            string token = "";
            while (tokens[tokens_it] != 30)
            {
                token += tokens[tokens_it];
                tokens_it++;
            }
            if (token.length() == 0)
                break;
            stemmer.stemmer(token, tipoStemmer);
            tokens_it++;
            infDoc.numPal++;
            posTerm++;
            if (stopWords.find(token) != stopWords.end())
                continue;
            informacionColeccionDocs.numTotalPalSinParada++;
            actualizar_indice_disco(token, infDoc, posTerm);
        }
        infDoc.tamBytes = get_file_size(nombreDoc);
        informacionColeccionDocs.tamBytes += infDoc.tamBytes;
        informacionColeccionDocs.numTotalPal += infDoc.numPal;
        free(tokens);
        GestorFicheros::guardarInfoDoc(nombreDoc, infDoc);
    }
    catch (bad_alloc &e)
    {
        cerr << "ERROR: falta de memoria al indexar " << nombreDoc << endl;
        return false;
    }
    return true;
}

bool IndexadorHash::Indexar_disco(const string &ficheroDocumentos)
{
    ifstream in_file(ficheroDocumentos);
    if (in_file.is_open())
    {
        string nombreDoc;
        while (getline(in_file, nombreDoc))
        {
            unordered_set<string>::iterator iterador = indiceDocsDisco.find(nombreDoc);
            if (iterador != indiceDocsDisco.end())
            {
                InfDoc infoDoc = GestorFicheros::leerInfoDoc(nombreDoc);
                if (ultima_modificacion(nombreDoc) > infoDoc.fechaModificacion)
                {
                    BorraDoc_disco(nombreDoc);
                    indexar_documento_disco(infoDoc, nombreDoc);
                }
            }
            else
                indexar_documento_disco(nombreDoc);
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

//FIN VERSIÓN QUE USA ALMACENAMIENTO SECUNDARIO
