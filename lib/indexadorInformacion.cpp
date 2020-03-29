#include "indexadorInformacion.h"
#include <ctime>

long int InfDoc::DOC_ID = 0;

using namespace std;

ostream& operator<<(ostream& s, const InfTermDoc& p)
{
    s << "ft: " << p.ft;
    for (const int pos : p.posTerm)
        s << "\t" << pos;
    return s;
}

void InfTermDoc::copy_vals(const InfTermDoc& foo)
{
    this->ft = foo.ft;
    for (int pos : foo.posTerm)
        this->posTerm.push_back(pos);
}

InfTermDoc::InfTermDoc(const InfTermDoc& foo)
{
    copy_vals(foo);
}

InfTermDoc& InfTermDoc::operator=(const InfTermDoc& foo)
{
    if (&foo != this)
    {
        this->~InfTermDoc();
        copy_vals(foo);
    }
    return *this;
}

InfTermDoc::~InfTermDoc()
{
    posTerm.clear();
}

ostream& operator<<(ostream& os, const InformacionTermino& it)
{
    os << "Frecuencia total: " << it.ftc << "\tfd: " << it.l_docs.size();
    for (const pair<long int, InfTermDoc>& entry : it.l_docs)
        os << "\tId.Doc: " << entry.first << "\t" << entry.second;
    return os;
}

void InformacionTermino::copy_vals(const InformacionTermino& foo)
{
    this->ftc = foo.ftc;
    for (const pair<long int, InfTermDoc>& entry : foo.l_docs)
        this->l_docs.insert(entry);
}

InformacionTermino::InformacionTermino(const InformacionTermino& foo)
{
    copy_vals(foo);
}

InformacionTermino::~InformacionTermino()
{
    l_docs.clear();
}

InformacionTermino& InformacionTermino::operator=(const InformacionTermino& foo)
{
    if (this != &foo)
    {
        this->~InformacionTermino();
        copy_vals(foo);
    }
    return *this;
}

ostream& operator<<(ostream& os, const InfDoc& id)
{
    os << "idDoc: " << id.idDoc << "\tnumPal: " << id.numPal << "\tnumPalSinParada: "
    << id.numPalSinParada << "\tnumPalDiferentes: " << id.numPalDiferentes << "\ttamBytes: "
    << id.tamBytes;
    return os;
}

void InfDoc::copy_vals(const InfDoc& foo)
{
    this->fechaModificacion = foo.fechaModificacion;
    this->idDoc = foo.idDoc;
    this->numPal = foo.numPal;
    this->numPalDiferentes = foo.numPalDiferentes;
    this->numPalSinParada = foo.numPalSinParada;
    this->tamBytes = foo.tamBytes;
}

InfDoc::InfDoc()
{
    idDoc = InfDoc::DOC_ID++;
    fechaModificacion = time(NULL);
    numPal = numPalDiferentes = numPalSinParada = tamBytes = 0;
}

InfDoc::InfDoc(const InfDoc& foo)
{
    copy_vals(foo);
}

InfDoc& InfDoc::operator=(const InfDoc& foo)
{
    if (this != &foo)
    {
        this->~InfDoc();
        copy_vals(foo);
    }
    return *this;
}

ostream& operator<<(ostream& os, const InfColeccionDocs& icd)
{
    os << "numDocs: " << icd.numDocs << "\tnumTotalPal: " << icd.numTotalPal << "\tnumTotalPalSinParada: " <<
    icd.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << icd.numTotalPalDiferentes << "\ttamBytes" <<
    icd.tamBytes;
    return os;
}

InfColeccionDocs::InfColeccionDocs()
{
    numDocs = numTotalPal = numTotalPalSinParada = numTotalPalDiferentes = tamBytes = 0;
}

void InfColeccionDocs::copy_vals(const InfColeccionDocs& foo)
{
    numDocs = foo.numDocs;
    numTotalPal = foo.numTotalPal;
    numTotalPalSinParada = foo.numTotalPalSinParada;
    numTotalPalDiferentes = foo.numTotalPalDiferentes;
    tamBytes = foo.tamBytes;
}

InfColeccionDocs::InfColeccionDocs(const InfColeccionDocs& foo)
{
    copy_vals(foo);
}

InfColeccionDocs& InfColeccionDocs::operator=(const InfColeccionDocs& foo)
{
    if (&foo != this)
        copy_vals(foo);
    return *this;
}

ostream& operator<<(ostream& os, const InformacionTerminoPregunta& itp)
{
    os << "ft: " << itp.ft;
    for (const int pos : itp.posTerm)
        os << "\t" << pos;
    return os;
}

void InformacionTerminoPregunta::copy_vals(const InformacionTerminoPregunta& foo)
{
    ft = foo.ft;
    for (const int pos : foo.posTerm)
        posTerm.push_back(pos);
}

InformacionTerminoPregunta& InformacionTerminoPregunta::operator=(const InformacionTerminoPregunta& foo)
{
    if (&foo != this)
    {
        this->~InformacionTerminoPregunta();
        copy_vals(foo);
    }
    return *this;
}

ostream& operator<<(ostream& os, const InformacionPregunta& ip)
{
    os << "numTotalPal: " << ip.numTotalPal << "\tnumTotalPalSinParada: " << ip.numTotalPalSinParada <<
    "\t numTotalPalDiferentes: " << ip.numTotalPalDiferentes;
    return os;
}

void InformacionPregunta::copy_vals(const InformacionPregunta& foo)
{
    numTotalPal = foo.numTotalPal;
    numTotalPalSinParada = foo.numTotalPalSinParada;
    numTotalPalDiferentes = foo.numTotalPalDiferentes;
}

InformacionPregunta& InformacionPregunta::operator=(const InformacionPregunta& foo)
{
    if (&foo != this)
        copy_vals(foo);
    return *this;
}
