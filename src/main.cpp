#include "buscador.h"
#include <sys/resource.h>

class Debugger
{
private:
    static double getcputime(void)
    {
        struct timeval tim;
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
        tim = ru.ru_utime;
        double t = (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
        tim = ru.ru_stime;
        t += (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
        return t;
    }
public:
    static void debug()
    {
        IndexadorHash *i = new IndexadorHash("./resources/StopWordsEspanyol.txt", ".     ,:",
                                             false, false, "./resources/guardadoIndicePrueba", 0, false, false);
        i->IndexarDirectorio("./resources/materiales_buscador/CorpusTime/Documentos");
        i->GuardarIndexacion();
        delete i;
        Buscador b("./resources/guardadoIndicePrueba", 0);
        double t_0 = getcputime();
        // b.IndexarPregunta("KENNEDY ADMINISTRATION PRESSURE ON NGO DINH DIEMSTOP SUPPRESSING THE BUDDHISTS . ");
        // b.Buscar();
        b.Buscar("./resources/materiales_buscador/CorpusTime/Preguntas", 9999, 1, 83);
        b.ImprimirResultadoBusqueda();
        double t_f = getcputime();
        cout << "Ha tardado " << t_f - t_0 << " segundos" << endl;
    }
};


int main() 
{
    Debugger::debug();
    return 0;
}