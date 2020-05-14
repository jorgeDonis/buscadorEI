#include "buscador.h"
#include <vector>
#include <fstream>
#include <algorithm>

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

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion)
{
    vector<ResultadoRI> docsOrdenadosPregunta(informacionColeccionDocs.numDocs, ResultadoRI(0, -1, 0));
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
    for (const pair<string, InformacionTerminoPregunta>& entrada_indice_pregunta : indicePregunta)
    {
        const unordered_map<string, InformacionTermino>::const_iterator it_indice_tok 
        = indice.find(entrada_indice_pregunta.first);
        for (const pair<long int, InfTermDoc>& entrada_l_docs : it_indice_tok->second.l_docs)
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
