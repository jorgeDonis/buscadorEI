#include "buscador.h"

class Debugger
{
public:
    static void debug()
    {
        IndexadorHash *i = new IndexadorHash("./resources/StopWordsEspanyol.txt", ".     ,:",
                                             false, false, "./resources/guardadoIndicePrueba", 0, false, false);
        i->IndexarDirectorio("./resources/materiales_buscador/CorpusTime/Documentos");
        i->GuardarIndexacion();
        delete i;
        Buscador b("./resources/guardadoIndicePrueba", 0);
        b.IndexarPregunta("KENNEDY ADMINISTRATION PRESSURE ON NGO DINH DIEMSTOP SUPPRESSING THE BUDDHISTS .");
        b.Buscar();
        b.ImprimirResultadoBusqueda();
    }
};


int main() 
{
    Debugger::debug();
    return 0;
}