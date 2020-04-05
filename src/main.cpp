#include "indexadorHash.h"

class Debugger
{
    public:
        static void debug()
        {
            Tokenizador tok; 
            tok.CasosEspeciales(true);
            tok.PasarAminuscSinAcentos(true);
            IndexadorHash indexador("stopwords.txt", " \n.\t,-", false, true, "", 0, false, true);
            indexador.IndexarDirectorio("./corpus_test");
            indexador.ImprimirIndexacion();
            InformacionTermino infter = indexador.indice["linea"];
            ofstream fichero_salida("salida.bin", ios::binary | ios::out);
            GestorFicheros gestor;
            gestor.guardar(inftermdoc, fichero_salida);
            fichero_salida.close();
            ifstream fichero_entrada("salida.bin", ios::binary | ios::in);
            InfTermDoc inftermCopia;
            gestor.leer(inftermCopia, fichero_entrada);
            fichero_entrada.close();
            cout << "inftermdoc :" << inftermdoc << endl;
            cout << "inftermCopia :" << inftermCopia << endl;
        }
};

int main()
{
    Debugger::debug();
    return 0;
}



