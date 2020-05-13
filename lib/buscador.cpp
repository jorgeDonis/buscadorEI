#include "buscador.h"
#include <vector>

using namespace std;

const float Buscador::DEFAULT_B = 0.75;
const float Buscador::DEFAULT_C = 2;
const float Buscador::DEFAULT_K1 = 1.2;

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

void ResultadoRI::copy_vals(const ResultadoRI& res)
{
    vSimilitud = res.vSimilitud;
    idDoc = res.idDoc;
    nombreDoc = res.nombreDoc;
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

bool ResultadoRI::operator<(const ResultadoRI &lhs) const
{
    if (numPregunta == lhs.numPregunta)
        return (vSimilitud < lhs.vSimilitud);
    else
        return (numPregunta > lhs.numPregunta);
}

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion)
{
    formSimilitud = f;
    b = Buscador::DEFAULT_B;
    c = Buscador::DEFAULT_C;
    k1 = Buscador::DEFAULT_K1;
}

void Buscador::copy_vals(const Buscador& bus)
{
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

bool Buscador::Buscar(const int& numDocumentos)
{
    return false; //TODO
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin)
{
    return false; //TODO
}

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos)
{
    //TODO optimizar
    size_t docs_impresos = 0;
    priority_queue<ResultadoRI> copiaCola = docsOrdenados;
    while (docsOrdenados.size() != 0 || docs_impresos == numDocumentos)
    {
        ResultadoRI res = docsOrdenados.top();
        docsOrdenados.pop();
        cout << res.NumPregunta() << " ";
        if (formSimilitud)
            cout << "BM25 ";
        else
            cout << "DFR ";
        cout << res.NombreDoc() << " ";
        cout << docs_impresos << " ";
        cout << res.VSimilitud() << " ";
        if (!res.NumPregunta())
            cout << Pregunta() << " ";
        else
            cout << "ConjuntoDePreguntas";
        docs_impresos++;
    }
    docsOrdenados = copiaCola;
}

