#ifndef __TOKENIZADOR__
#define __TOKENIZADOR__

#include <string>
#include <iostream>

class Tokenizador
{
    friend std::ostream& operator<<(std::ostream&, const Tokenizador&);
    private:
        std::string delimiters;
        bool casosEspeciales;
        bool pasarAminuscSinAcentos;
};

#endif