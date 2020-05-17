#include "buscador.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <regex>

using namespace std;

const float Buscador::DEFAULT_B = 0.75;
const float Buscador::DEFAULT_C = 2;
const float Buscador::DEFAULT_K1 = 1.2;
const int Buscador::DEFAULT_FORM_SIMILITUD = 0;
const size_t Buscador::TOTAL_PREGUNTAS = 86;

ostream& operator<<(ostream& os, const ResultadoRI& res){
    os << res.vSimilitud << "\t\t" << res.idDoc << "\t" << res.numPregunta << endl;
    return os;
}

ostream& operator<<(ostream& os, const Buscador& bus)
{
    string preg;
    os << "Buscador: " << endl;
    if (bus.DevuelvePregunta(preg))
        os << "\tPregunta indexada: " << preg << endl;
    else
        os << "\tNo hay ninguna pregunta indexada" << endl;
    os << "\tDatos del idnexador: " << endl << (IndexadorHash) bus;
    return os;
}

ResultadoRI::ResultadoRI(const double & kvSimilitud, const long int & kidDoc, const int & np)
{
    vSimilitud = kvSimilitud;
    this->idDoc = kidDoc;
    numPregunta = np;
}

ResultadoRI::ResultadoRI(const double &kvSimilitud, const long int &kidDoc, const int &np, string& nombreFichero)
{
    vSimilitud = kvSimilitud;
    this->idDoc = kidDoc;
    numPregunta = np;
}

void ResultadoRI::copy_vals(const ResultadoRI& res)
{
    vSimilitud = res.vSimilitud;
    idDoc = res.idDoc;
    numPregunta = res.numPregunta;
}

ResultadoRI::ResultadoRI(const ResultadoRI& res)
{
    copy_vals(res);
}

ResultadoRI& ResultadoRI::operator=(const ResultadoRI& res)
{
    if (this != &res)
    {
        this->~ResultadoRI();
        copy_vals(res);
    }
    return *this;
}

/**
 * @brief Funciona al revés para que ordene de mayor a menor. No tengo claro si
 * funciona bien con varios numPregunta
 * @param lhs 
 * @return true 
 * @return false 
 */
bool ResultadoRI::operator<(const ResultadoRI &lhs) const
{
    if (numPregunta == lhs.numPregunta)
        return (vSimilitud > lhs.vSimilitud);
    else
        return (numPregunta < lhs.numPregunta);
}

Buscador::Buscador()
{
    formSimilitud = Buscador::DEFAULT_FORM_SIMILITUD;
    k1 = Buscador::DEFAULT_K1;
    b = Buscador::DEFAULT_B;
    c = Buscador::DEFAULT_C;
}

/**
 * @brief Actualizará para cada InfTermDoc indexado el miembro dfr_parcial.
 * Éste se corresponde con w_i,d 
 */
inline void Buscador::precalcular_dfr()
{
    double avr_ld = (double) informacionColeccionDocs.numTotalPalSinParada / informacionColeccionDocs.numDocs;
    for (auto& it_indice : indice)
    {
        size_t n_t = it_indice.second.l_docs.size();
        double lambda = (double) it_indice.second.ftc / informacionColeccionDocs.numDocs;
        for (auto& it_ldocs : it_indice.second.l_docs)
        {
            double ftd = it_ldocs.second.ft;
            size_t ld = indiceDocs[nombresDocs[it_ldocs.first]].numPalSinParada;
            double ftd_norm = ftd * log2(1 + c * avr_ld / (double) ld);
            double first_factor = log2(1 + lambda) + ftd_norm * log2((1 + lambda) / (double) lambda);
            double second_factor = ((double) (it_indice.second.ftc + 1)) / (n_t * (ftd_norm + 1));
            it_ldocs.second.dfr_parcial = first_factor * second_factor;           
        }
    }
}

inline void Buscador::precalcular_bm25()
{

}

/**
 * @brief Actualiza los valores de similitud para todos los InfTermDoc
 * indexados, es decir, las variables dfr_parcial y bm25_parcial. En el caso
 * de DFR sólo se calcula el primer factor de la fórmula. Para bm25 se calcula todo
 * el sumando.
 */
void Buscador::precalcular_offline()
{
    precalcular_dfr();
    precalcular_bm25();
}

void Buscador::guardarNombresDocs()
{
    for (const auto& it_ind_docs : indiceDocs)
    {
        // regex expr(".*\\/(.*)\\..*");
        // smatch matches;
        // regex_match(it_ind_docs.first.begin(), it_ind_docs.first.end(), matches, expr);
        // nombresDocs[it_ind_docs.second.idDoc] = matches[1];
        nombresDocs[it_ind_docs.second.idDoc] = it_ind_docs.first;
    }
}

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion)
{
    docsOrdenados = vector<ResultadoRI>(informacionColeccionDocs.numDocs * TOTAL_PREGUNTAS, ResultadoRI(0, -1, 0));
    numDocsBuscados = 0;
    formSimilitud = f;
    b = Buscador::DEFAULT_B;
    c = Buscador::DEFAULT_C;
    k1 = Buscador::DEFAULT_K1;
    guardarNombresDocs();
    precalcular_offline();
}

void Buscador::copy_vals(const Buscador& bus)
{
    nombresDocs = bus.nombresDocs;
    numDocsBuscados = bus.numDocsBuscados;
    docsOrdenados = bus.docsOrdenados;
    formSimilitud = bus.formSimilitud;
    b = bus.b;
    c = bus.c;
    k1 = bus.k1;
}

Buscador::Buscador(const Buscador& bus) : IndexadorHash(bus.DevolverDirIndice())
{
    copy_vals(bus);
}

Buscador& Buscador::operator=(const Buscador& bus)
{
    if (this != &bus)
    {
        this->~Buscador();
        copy_vals(bus);
    }
    return *this;
}


/**
 * @brief Calcula los valores de similitud de los documentos con tokens
 * presentes en la query con la misma. Añade ResultadoRI's al vector docsOrdenados. No ordena
 * el vector.
 * @param num_pregunta 0 si sólo es una
 */
inline void Buscador::calc_simil_docs(const size_t& num_pregunta)
{
    unordered_map<long int, double> resultados_parciales;
    for (const auto& entrada_indice_pregunta : indicePregunta)
    {
        const unordered_map<string, InformacionTermino>::const_iterator it_indice_tok 
        = indice.find(entrada_indice_pregunta.first);
        if (it_indice_tok != indice.end())
        {
            for (const auto& entrada_l_docs : it_indice_tok->second.l_docs)
            {
                unordered_map<long int, double>::iterator parciales_it =
                resultados_parciales.find(entrada_l_docs.first);
                double sim_parcial;
                if (formSimilitud == 0)
                {
                    sim_parcial = entrada_indice_pregunta.second.ft / (double) infPregunta.numTotalPalSinParada;
                    sim_parcial *= entrada_l_docs.second.dfr_parcial;
                }
                else
                    sim_parcial = entrada_l_docs.second.bm25_parcial;
                if (parciales_it == resultados_parciales.end())
                    resultados_parciales.emplace(entrada_l_docs.first, sim_parcial);
                else
                    parciales_it->second += sim_parcial;
            }
        }
    }
    for (const auto& fila_similitud : resultados_parciales)
    {
        docsOrdenados[numDocsBuscados] = 
        ResultadoRI(fila_similitud.second, fila_similitud.first, num_pregunta);
        numDocsBuscados++;
    }
}

bool Buscador::Buscar(const int& numDocumentos)
{
    if (indicePregunta.size())
    {
        numDocsBuscados = 0;
        calc_simil_docs(0);
        sort(docsOrdenados.begin(), docsOrdenados.begin() + numDocsBuscados);
        numDocsBuscados = min(numDocsBuscados, (size_t) numDocumentos);
        return true;
    }
    cerr << "ERROR: no hay ninguna pregunta indexada" << endl;
    return false;
}

/**
 * @brief Indexa la pregunta contenida en el fichero dirPreguntas/num_pregunta.txt
 * 
 * @param num_pregunta 
 * @param dirPreguntas 
 */
void Buscador::indexar_pregunta(const size_t& num_pregunta, const string& dirPreguntas)
{
    string nombre_fichero_pregunta = dirPreguntas + "/" + to_string(num_pregunta) + ".txt";
    ifstream fichero_pregunta(nombre_fichero_pregunta, ios::in | ios::binary | ios::ate);
    size_t file_size = fichero_pregunta.tellg();
    char *memblock = (char *)malloc(file_size);
    fichero_pregunta.seekg(0, ios::beg);
    fichero_pregunta.read(memblock, file_size - 1); // para no leer el /n
    fichero_pregunta.close();
    string pregunta(memblock);
    free(memblock);
    IndexarPregunta(pregunta);
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin)
{
    docsOrdenados = vector<ResultadoRI>(informacionColeccionDocs.numDocs * TOTAL_PREGUNTAS, ResultadoRI(0, -1, 0));
    if (Tokenizador::file_exists(dirPreguntas))
    {
        numDocsBuscados = 0;
        size_t numDocsOrdenados = 0;
        for (size_t num_pregunta = numPregInicio; num_pregunta <= numPregFin; num_pregunta++)
        {
            indexar_pregunta(num_pregunta, dirPreguntas);
            size_t tam_vector_prev = numDocsBuscados;
            calc_simil_docs(num_pregunta);
            size_t nuevos_docs_buscados = numDocsBuscados - tam_vector_prev;
            if (nuevos_docs_buscados > numDocumentos)
            {
                sort(docsOrdenados.begin() + numDocsOrdenados, docsOrdenados.begin() + numDocsBuscados);
                numDocsBuscados = numDocsOrdenados = tam_vector_prev + numDocumentos;
            }
        }
        sort(docsOrdenados.begin() + numDocsOrdenados, docsOrdenados.begin() + numDocsBuscados);
        return true;
    }
    cerr << "ERROR: No existe el directorio " << dirPreguntas << endl;
    return false;
    
}

void Buscador::ImprimirResultadoBusqueda(const int& maxDocsPregunta)
{
    const bool conjuntoPreguntas = docsOrdenados[0].NumPregunta();
    size_t docs_impresos_pregunta = 0;
    size_t docs_impresos_total = 0;
    regex expr(".*\\/(.*)\\..*");
    while (docs_impresos_total != numDocsBuscados)
    {
        const bool maximoSuperado = (docs_impresos_pregunta + 1) == maxDocsPregunta;
        ResultadoRI res = docsOrdenados[docs_impresos_total];
        smatch matches;
        regex_match(nombresDocs[res.IdDoc()], matches, expr);
        string nombreSinRuta = matches[1];
        cout << res.NumPregunta() << " ";
        if (formSimilitud)
            cout << "BM25 ";
        else
            cout << "DFR ";
        cout << nombreSinRuta << " ";
        cout << docs_impresos_pregunta << " ";
        cout << res.VSimilitud() << " ";
        if (conjuntoPreguntas)
            cout << "ConjuntoDePreguntas" << endl;
        else
            cout << Pregunta() << endl;
        docs_impresos_pregunta++;
        docs_impresos_total++;
        const size_t numPreguntaPrev = docsOrdenados[docs_impresos_total - 1].NumPregunta();
        if (maximoSuperado)
        {
            while (docs_impresos_total != numDocsBuscados && 
                   numPreguntaPrev == docsOrdenados[docs_impresos_total].NumPregunta())
                docs_impresos_total++;
        }
        if (numPreguntaPrev != docsOrdenados[docs_impresos_total].NumPregunta())
            docs_impresos_pregunta = 0;
    }
}

bool Buscador::ImprimirResultadoBusqueda(const string& nombreFichero, const int &numDocumentos)
{
    //TODO optimizar con mmap y hacer
    size_t docs_impresos = 0;
    while (docsOrdenados.size() != 0 || docs_impresos == numDocumentos)
    {
        // ResultadoRI res = docsOrdenados.top();
        // docsOrdenados.pop();
        // ficheroSalida << res.NumPregunta() << " ";
        // if (formSimilitud)
        //     ficheroSalida << "BM25 ";
        // else
        //     ficheroSalida << "DFR ";
        // ficheroSalida << res.NombreDoc() << " ";
        // ficheroSalida << docs_impresos << " ";
        // ficheroSalida << res.VSimilitud() << " ";
        // if (!res.NumPregunta())
        //     ficheroSalida << Pregunta() << " ";
        // else
        //     ficheroSalida << "ConjuntoDePreguntas" << endl;
        // docs_impresos++;
    }
    return true;
}
