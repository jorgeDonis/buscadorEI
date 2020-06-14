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
const unsigned Buscador::TOTAL_PREGUNTAS = 83;
const unsigned Buscador::TOTAL_DOCUMENTOS = 423;

ostream& operator<<(ostream& os, const ResultadoRI& res) {
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

ResultadoRI::ResultadoRI()
{
    idDoc = -1;
    numPregunta = -1;
    vSimilitud = 0;
}

ResultadoRI::ResultadoRI(const double & kvSimilitud, const long int & kidDoc, const int & np)
{
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
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

void ResultadoRI::sumarSimilitud(const double& simAdi)
{
    vSimilitud += simAdi;
}

/**
 * @brief Funciona al rev�s para que ordene de mayor a menor. No tengo claro si
 * funciona bien con varios numPregunta
 * @param lhs 
 * @return true 
 * @return false 
 */
bool ResultadoRI::operator<(const ResultadoRI &lhs) const
{
    if (!vSimilitud)
        return false;
    if (!lhs.vSimilitud)
        return true;
    return vSimilitud > lhs.vSimilitud;
}

Buscador::Buscador()
{
    formSimilitud = Buscador::DEFAULT_FORM_SIMILITUD;
    k1 = Buscador::DEFAULT_K1;
    b = Buscador::DEFAULT_B;
    c = Buscador::DEFAULT_C;
}

/**
 * @brief Actualizar� para cada InfTermDoc indexado el miembro dfr_parcial.
 * �ste se corresponde con w_i,d 
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
    double avgdl = (double)informacionColeccionDocs.numTotalPalSinParada / informacionColeccionDocs.numDocs;
    double N = informacionColeccionDocs.numDocs;
    for (auto& it_indice : indice)
    {
        double nqi = it_indice.second.l_docs.size();
        for (auto& it_ldocs : it_indice.second.l_docs)
        {
            double fqid = it_ldocs.second.ft;
            double D = indiceDocs[nombresDocs[it_ldocs.first]].numPalSinParada;
            double IDF = log2((N - nqi + 0.5) / (nqi + 0.5));
            double second_factor = (fqid * (k1 + 1)) / (fqid + k1 * (1 - b + b * (D / avgdl)));
            it_ldocs.second.bm25_parcial = IDF * second_factor;
        }
    }
}

/**
 * @brief Actualiza los valores de similitud para todos los InfTermDoc
 * indexados, es decir, las variables dfr_parcial y bm25_parcial. En el caso
 * de DFR s�lo se calcula el primer factor de la f�rmula. Para bm25 se calcula todo
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
        regex expr(".*\\/(.*)\\..*");
        smatch matches;
        regex_match(it_ind_docs.first.begin(), it_ind_docs.first.end(), matches, expr);
        nombresDocsSinRuta[it_ind_docs.second.idDoc] = matches[1];
        nombresDocs[it_ind_docs.second.idDoc] = it_ind_docs.first;
    }
}

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion)
{
    formSimilitud = f;
    b = Buscador::DEFAULT_B;
    c = Buscador::DEFAULT_C;
    k1 = Buscador::DEFAULT_K1;
    busqueda_str.reserve(TOTAL_DOCUMENTOS * TOTAL_PREGUNTAS * 40);
    guardarNombresDocs();
    precalcular_offline();
}

void Buscador::copy_vals(const Buscador& bus)
{
    nombresDocsSinRuta = bus.nombresDocsSinRuta;
    nombresDocs = bus.nombresDocs;
    for (unsigned i = 0; i < TOTAL_PREGUNTAS; i++)
        for (unsigned j = 0; j < TOTAL_DOCUMENTOS; j++)
            docsOrdenados[i][j] = bus.docsOrdenados[i][j];
    busqueda_str = bus.busqueda_str;
    conjuntoPreguntas = bus.conjuntoPreguntas;
    preg_inicial = bus.preg_inicial;
    preg_final = bus.preg_final;
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
 * presentes en la query con la misma. A�ade ResultadoRI's al vector correspondiente
 * del vector de vectores docsOrdenados. Sí ordena este vector.
 * @param num_pregunta 0 si s�lo es una. La pregunta 1 ha de ser 0 (se tiene que restar
 * antes de llamar a este método)
 */
inline void Buscador::calc_simil_docs(const size_t& num_pregunta)
{
    for (const auto& entrada_indice_pregunta : indicePregunta)
    {
        const unordered_map<string, InformacionTermino>::const_iterator it_indice_tok 
        = indice.find(entrada_indice_pregunta.first);
        if (it_indice_tok != indice.end())
        {
            for (const auto& entrada_l_docs : it_indice_tok->second.l_docs)
            {
                double sim_parcial;
                if (formSimilitud == 0)
                {
                    sim_parcial = entrada_indice_pregunta.second.ft / (double) infPregunta.numTotalPalSinParada;
                    sim_parcial *= entrada_l_docs.second.dfr_parcial;
                }
                else
                    sim_parcial = entrada_l_docs.second.bm25_parcial;
                docsOrdenados[num_pregunta][entrada_l_docs.first - 1].sumarSimilitud(sim_parcial);
            }
        }
    }
    sort(docsOrdenados[num_pregunta], docsOrdenados[num_pregunta] + TOTAL_DOCUMENTOS);
}

/**
 * @brief Inicializa cada fila de docsOrdenados con similitud a 0 y la pregunta
 * correspondiente a su posición en la matriz. idDoc = apropiado
 */
void Buscador::limpiar_docs_ordenados()
{
    for (unsigned i = 0; i < TOTAL_PREGUNTAS; i++)
        for (unsigned j = 0; j < TOTAL_DOCUMENTOS; j++)
            docsOrdenados[i][j] = ResultadoRI(0, j + 1, i + 1);
}

bool Buscador::Buscar(const int& numDocumentos)
{
    if (indicePregunta.size())
    {
        limpiar_docs_ordenados();
        conjuntoPreguntas = false;
        calc_simil_docs(0);
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
    VaciarIndicePreg();
    char* tokens = tok.TokenizarFichero(nombre_fichero_pregunta);
    indexar_tokens_pregunta(tokens);
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin)
{
    if (Tokenizador::file_exists(dirPreguntas))
    {
        conjuntoPreguntas = true;
        limpiar_docs_ordenados();
        preg_inicial = numPregInicio;
        preg_final = numPregFin;
        for (size_t num_pregunta = numPregInicio; num_pregunta <= numPregFin; num_pregunta++)
        {
            indexar_pregunta(num_pregunta, dirPreguntas);
            calc_simil_docs(num_pregunta - 1);
        }
        return true;
    }
    cerr << "ERROR: No existe el directorio " << dirPreguntas << endl;
    return false;
    
}

/**
 * @brief Modifica la variable de clase busqueda_str
 * 
 */
void Buscador::imprimir_res_pregunta(const unsigned& num_pregunta, const int& maxDocsPregunta)
{
    unsigned docs_imprimidos = 0;
    for (unsigned num_doc = 0; num_doc < TOTAL_DOCUMENTOS; num_doc++)
    {
        ResultadoRI res = docsOrdenados[num_pregunta][num_doc];
        if (res.VSimilitud() == 0 || docs_imprimidos == maxDocsPregunta)
            break;
        docs_imprimidos++;
        string nombreSinRuta = nombresDocsSinRuta[res.IdDoc()];
        busqueda_str += to_string(res.NumPregunta());
        busqueda_str += " ";
        if (formSimilitud)
            busqueda_str += "BM25 ";
        else
            busqueda_str += "DFR ";
        busqueda_str += nombreSinRuta;
        busqueda_str += " ";
        busqueda_str += to_string(num_doc);
        busqueda_str += " ";
        busqueda_str += to_string(res.VSimilitud());
        busqueda_str += " ";
        if (conjuntoPreguntas)
            busqueda_str += "ConjuntoDePreguntas\n";
        else
        {
            busqueda_str += Pregunta();
            busqueda_str += "\n";
        }
    }
}

void Buscador::imprimir_busqueda_str(const int& maxDocsPregunta)
{
    if (!conjuntoPreguntas)
    {
        for (unsigned i = 0; i < TOTAL_DOCUMENTOS; i++)
        {
            if (!docsOrdenados[0][i].VSimilitud())
                break;
            docsOrdenados[0][i].reducirPregunta();
        }
        imprimir_res_pregunta(0, maxDocsPregunta);
    }
    else
        for (unsigned num_pregunta = preg_inicial; num_pregunta <= preg_final; num_pregunta++)
            imprimir_res_pregunta(num_pregunta - 1, maxDocsPregunta);
}

void Buscador::ImprimirResultadoBusqueda(const int& maxDocsPregunta)
{
    busqueda_str.clear();
    imprimir_busqueda_str(maxDocsPregunta);
    cout << busqueda_str;
}

bool Buscador::ImprimirResultadoBusqueda(const int &maxDocsPregunta, const string &nombreFichero)
{
    busqueda_str.clear();
    imprimir_busqueda_str(maxDocsPregunta);
    fstream fichero_salida(nombreFichero, ios::out | ios::binary);
    if (fichero_salida.is_open())
    {
        fichero_salida.write(busqueda_str.c_str(), busqueda_str.length());
        fichero_salida.close();
    }
    else
    {
        cerr << "ERROR: no se pudo crear el fichero " << nombreFichero << endl;
        return false;
    }
    return true;
}
