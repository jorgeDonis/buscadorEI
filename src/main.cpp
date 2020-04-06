#include "indexadorHash.h"

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
            ofstream fichero_salida("salida.bin", ios::binary | ios::out);
            GestorFicheros gestor;
            gestor.guardar(indexador, fichero_salida);
            fichero_salida.close();
            ifstream fichero_entrada("salida.bin", ios::binary | ios::in);
            gestor.leer(copia, fichero_entrada);
            fichero_entrada.close();
            cout << endl << "ESCRITURA FICHEROS" << endl;
            cout << "stopwords :" << endl;
            indexador.ImprimirIndexacion();
            cout << endl << "--------------------------------" << endl;
            cout << "stopwordsCopia :" << endl;
            copia.ImprimirIndexacion();
        }
};

int main()
{
    Debugger::debug();
    return 0;
}



