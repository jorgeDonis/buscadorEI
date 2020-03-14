#ifndef __INDEXADOR_INFORMACION__
#define __INDEXADOR_INFORMACION__

#include <iostream>
#include <unordered_map>
#include <list>

class InfTermDoc
{
    friend std::ostream& operator<<(std::ostream& s, const InfTermDoc& p);

    public:
        InfTermDoc(const InfTermDoc &);
        InfTermDoc();
        // Inicializa ft = 0
        ~InfTermDoc();
        // Pone ft = 0
        InfTermDoc &operator=(const InfTermDoc &);
        // A�adir cuantos m�todos se consideren necesarios para manejar la parte
        //privada de la clase private : int ft;
        // Frecuencia del t�rmino en el documento
        std::list<int> posTerm;
        // Solo se almacenar� esta informaci�n si el campo privado del indexador
        //almacenarPosTerm == true
        //                    // Lista de n�meros de palabra en los que aparece el t�rmino en el
        //                    documento.Los n�meros de palabra comenzar�n desde
        //                    cero(la primera
        //                             palabra del documento)
        //                        .Se numerar�n las palabras de parada.Estar�
        //                    ordenada de menor a mayor posici�n.
};

class InformacionTermino
{
    friend std::ostream& operator<<(std::ostream& s, const InformacionTermino& p);
    public:
        InformacionTermino(const InformacionTermino &);
        InformacionTermino();
        // Inicializa ftc = 0
        ~InformacionTermino();
        // Pone ftc = 0 y vac�a l_docs
        InformacionTermino &operator=(const InformacionTermino &);
        // A�adir cuantos m�todos se consideren necesarios para manejar la parte
        // privada de la clase private : int ftc; // Frecuencia total del t�rmino en la colecci�n
        std::unordered_map<long int, InfTermDoc> l_docs;
        // Tabla Hash que se acceder� por el id del documento, devolviendo un
        //objeto de la clase InfTermDoc que contiene toda la informaci�n de
        //    aparici�n del t�rmino en el documento
};


#endif