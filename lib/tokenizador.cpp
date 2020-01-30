 #include "tokenizador.h"

 using namespace std;

 ostream& operator<<(ostream& os, const Tokenizador& tokenizador)
 {
     os << "DELIMITADORES: " << tokenizador.delimiters << " TRATA CASOS ESPECIALES: ";
     os << tokenizador.casosEspeciales << " PASAR A MINUSCULAS SIN ACENTOS: ";
     os << tokenizador.pasarAminuscSinAcentos;
     return os;
 }