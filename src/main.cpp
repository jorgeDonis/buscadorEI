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
            IndexadorHash indexador("stopwords.txt", " \n.\t,-@", false, true, "./indice_save", 1, false, true);
            indexador.IndexarDirectorio("./corpus_test");
            indexador.GuardarIndexacion();
            IndexadorHash copia("./indice_save");
            cout << "ORIGINAL" << endl;
            indexador.ImprimirIndexacion();
            cout << "COPIA" << endl;
            copia.ImprimirIndexacion();
        }
};

int main()
{
    Debugger::debug();
    return 0;
}



