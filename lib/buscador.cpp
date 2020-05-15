#include "buscador.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

string ResultadoRI::NOMBRE_DOC_DEFAULT = "NULL_DOC_NAME";

const float Buscador::DEFAULT_B = 0.75;
const float Buscador::DEFAULT_C = 2;
const float Buscador::DEFAULT_K1 = 1.2;
const int Buscador::DEFAULT_FORM_SIMILITUD = 0;
const int Buscador::NUM_PREGUNTAS_TOTAL = 83;

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
: nombreDoc(NOMBRE_DOC_DEFAULT)
{
    vSimilitud = kvSimilitud;
    this->idDoc = kidDoc;
    numPregunta = np;
}

ResultadoRI::ResultadoRI(const double &kvSimilitud, const long int &kidDoc, const int &np, string& nombreFichero)
    : nombreDoc(nombreFichero)
{
    vSimilitud = kvSimilitud;
    this->idDoc = kidDoc;
    numPregunta = np;
}

void ResultadoRI::copy_vals(const ResultadoRI& res)
{
    vSimilitud = res.vSimilitud;
    idDoc = res.idDoc;
    nombreDoc = res.nombreDoc;
    numPregunta = res.numPregunta;
}

ResultadoRI::ResultadoRI(const ResultadoRI& res) : nombreDoc(NOMBRE_DOC_DEFAULT)
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

bool ResultadoRI::operator<(const ResultadoRI &lhs) const
{
    if (numPregunta == lhs.numPregunta)
        return (vSimilitud < lhs.vSimilitud);
    else
        return (numPregunta > lhs.numPregunta);
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
void Buscador::precalcular_dfr()
{
    for (const pair<string, InformacionTermino>& it_indice : indice)
    {
        double lambda = (double) it_indice.second.ftc / informacionColeccionDocs.numDocs;
        for (const pair<unsigned long, InfTermDoc>& it_ldocs : it_indice.second.l_docs)
        {
            double avr_ld = informacionColeccionDocs.numTotalPalSinParada / informacionColeccionDocs.numDocs;
            double ftd = it_ldocs.second.ft;
            size_t ld = indiceDocs[nombresDocs[it_ldocs.first]].numPalSinParada;
            double ftd_norm = ftd * log2(1 + (c * avr_ld) / (double) ld);
            
        }
    }
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

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion)
{
    docsOrdenados = vector<ResultadoRI>(informacionColeccionDocs.numDocs, ResultadoRI(0, -1, 0));
    numDocsBuscados = 0;
    numDocsImprimir = 0;
    formSimilitud = f;
    b = Buscador::DEFAULT_B;
    c = Buscador::DEFAULT_C;
    k1 = Buscador::DEFAULT_K1;
}

void Buscador::copy_vals(const Buscador& bus)
{
    numDocsBuscados = bus.numDocsBuscados;
    numDocsImprimir = bus.numDocsImprimir;
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
 * @brief Actualizará docsOrdenados y numDocsOrdenados con 
 * varios ResultadoRI's . Además ordenará el vector asociado
 * a esa pregunta. La pregunta tiene que estar indexada en
 * indicePregunta
 * 
 * @param num_pregunta 
 */
void Buscador::buscar_pregunta(const size_t& num_pregunta)
{
    unordered_map<long int, pair<double, string&>> resultados_parciales;
    for (const pair<string, InformacionTerminoPregunta>& entrada_indice_pregunta : indicePregunta)
    {
        const unordered_map<string, InformacionTermino>::const_iterator it_indice_tok 
        = indice.find(entrada_indice_pregunta.first);
        for (const pair<long int, InfTermDoc>& entrada_l_docs : it_indice_tok->second.l_docs)
        {
            unordered_map<long int, pair<double, string&>>::iterator parciales_it =
            resultados_parciales.find(entrada_l_docs.first);
            double sim_parcial;
            if (formSimilitud == 0)
            {
                sim_parcial = entrada_indice_pregunta.second.ft / (double) indicePregunta.size();
                sim_parcial *= entrada_l_docs.second.dfr_parcial;
            }
            else
                sim_parcial = entrada_l_docs.second.bm25_parcial;
            if (parciales_it == resultados_parciales.end())
                resultados_parciales.emplace(make_pair(sim_parcial, nombresDocs[entrada_l_docs.first]));
            else
                parciales_it->second.first += sim_parcial;
        }
    }
    for (const pair<long int, pair<double, string&>>& fila_similitud : resultados_parciales)
    {
        docsOrdenados[numDocsBuscados] = 
        ResultadoRI(fila_similitud.second.first, fila_similitud.first, num_pregunta, fila_similitud.second.second);
        numDocsBuscados++;
    }
    sort(docsOrdenados.begin(), docsOrdenados.begin() + numDocsBuscados);
}

bool Buscador::Buscar(const int& numDocumentos)
{
    if (indicePregunta.size())
    {
        numDocsBuscados = 0;
        buscar_pregunta(0);
        return true;
    }
    return false;
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin)
{
    return false; //TODO
}

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos)
{
    //TODO hacer
    size_t docs_impresos = 0;
    while (docsOrdenados.size() != 0 || docs_impresos == numDocumentos)
    {
        // ResultadoRI res = docsOrdenados.top();
        // docsOrdenados.pop();
        // cout << res.NumPregunta() << " ";
        // if (formSimilitud)
        //     cout << "BM25 ";
        // else
        //     cout << "DFR ";
        // cout << res.NombreDoc() << " ";
        // cout << docs_impresos << " ";
        // cout << res.VSimilitud() << " ";
        // if (!res.NumPregunta())
        //     cout << Pregunta() << " ";
        // else
        //     cout << "ConjuntoDePreguntas" << endl;
        docs_impresos++;
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
