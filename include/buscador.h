#ifndef __BUSCADOR__
#define __BUSCADOR__

#include "indexadorHash.h"
#include <queue>

class ResultadoRI
{
    private:
        double vSimilitud;
};

class Buscador: public IndexadorHash
{
    private:
        std::priority_queue<ResultadoRI> docsOrdenados;
};

#endif