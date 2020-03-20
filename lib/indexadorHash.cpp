#include "indexadorHash.h" 
#include <fstream>
#include "unistd.h"

using namespace std;

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

IndexadorHash::IndexadorHash() : indice(), indicePregunta(), stopWords(), informacionColeccionDocs(), tok(), stemmer()
{
    pregunta = "";
    ficheroStopWords = "";
    directorioIndice = "";
    tipoStemmer = 0;
    almacenarEnDisco = almacenarPosTerm = false;
}

bool IndexadorHash::leer_fichero_stopwords(const string& filename)
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
            stopWords.insert(line);
        file.close();
    }
    else
    {
        cerr << "ERROR: no se pudo abrir " << filename << endl;
        return false;
    }
    return true;
}

IndexadorHash::IndexadorHash(const std::string &fichStopWords, const std::string &delimitadores,
                             const bool &detectComp, const bool &minuscSinAcentos, const std::string &dirIndice,
                             const int &tStemmer, const bool &almEnDisco, const bool &almPosTerm)
                             : indice(), indicePregunta(), stopWords(), informacionColeccionDocs(), tok(delimitadores, detectComp, minuscSinAcentos)
                               , stemmer()
{
    if (leer_fichero_stopwords(fichStopWords))
    {
        if (dirIndice == "")
        {
            char* wd;
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

