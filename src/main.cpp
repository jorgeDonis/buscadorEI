#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "tokenizador.h"
using namespace std;


int main()
{
        Tokenizador a("\t ,;:.-+/*_`'{}[]()!?&#\"\\<>\n@", true, true);
        a.Tokenizar("corpus.txt");
        return 0;
}
