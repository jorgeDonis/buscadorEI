#include "indexadorHash.h"
#include <sstream>

class Debugger
{
    public:
        static void print_indice(const unordered_map<string, InformacionTermino >& indice)
        {
            for (const pair<string, InformacionTermino>& entrada : indice)
                cout << "[" << entrada.first << "] : " << entrada.second << endl;
        }
        static void print_indice(const unordered_map<string, InfDoc >& indice)
        {
            for (const pair<string, InfDoc>& entrada : indice)
                cout << "[" << entrada.first << "] : " << entrada.second << endl;
        }
        static void print_indice(const unordered_map<string, InformacionTerminoPregunta >& indice)
        {
            for (const pair<string, InformacionTerminoPregunta>& entrada : indice)
                cout << "[" << entrada.first << "] : " << entrada.second << endl;
        }
        static void print_set(const unordered_set<string>& set)
        {
            for (const string& entrada : set)
                cout << entrada << endl;
        }

        static void debug()
        {
            IndexadorHash indexador("stopwords.txt", " \n.\t,-@", false, true, "", 1, false, true);
            indexador.IndexarDirectorio("./corpus_test");
            IndexadorHash copia;
            fstream fichero("indexador.bin", ios::binary | ios::out);
            GestorFicheros::guardar(indexador, fichero);
            fichero.close();
            fstream fichero_entrada("indexador.bin", ios::binary | ios::in);
            GestorFicheros::leer(copia, fichero_entrada);
            fichero_entrada.close();
            cout << "ORIGINAL" << endl;
            indexador.ImprimirIndexacion();
            cout << "COPIA" << endl;
            copia.ImprimirIndexacion();
            cout << "tam bytes : " << indexador.informacionColeccionDocs.tamBytes << endl;
        }
};

int main()
{
    Debugger::debug();
    return 0;
}



