#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "buscador.h"
#include "indexadorHash.h"

using namespace std;

double getcputime(void)
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
int main()
{
	IndexadorHash b("/home/jorge/Desktop/EI/practica/buscadorEI/resources/StopWordsEspanyol.txt", ".,: ", false, false, "./indicePruebaEspanyol", 0, false, false);
	b.IndexarDirectorio("/home/jorge/Desktop/EI/practica/buscadorEI/resources/materiales_buscador/CorpusTime/Documentos");
	b.GuardarIndexacion();
	Buscador a("./indicePruebaEspanyol", 0);

	a.Buscar("/home/jorge/Desktop/EI/practica/buscadorEI/resources/materiales_buscador/CorpusTime/Preguntas/", 3, 2, 2);
	a.ImprimirResultadoBusqueda(3, "/home/jorge/Desktop/EI/practica/buscadorEI/resources/salidas/busqueda_sal.txt");
}
