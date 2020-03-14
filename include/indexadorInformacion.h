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
        // Añadir cuantos métodos se consideren necesarios para manejar la parte
        //privada de la clase private : int ft;
        // Frecuencia del término en el documento
        std::list<int> posTerm;
        // Solo se almacenará esta información si el campo privado del indexador
        //almacenarPosTerm == true
        //                    // Lista de números de palabra en los que aparece el término en el
        //                    documento.Los números de palabra comenzarán desde
        //                    cero(la primera
        //                             palabra del documento)
        //                        .Se numerarán las palabras de parada.Estará
        //                    ordenada de menor a mayor posición.
};

class InformacionTermino
{
    friend std::ostream& operator<<(std::ostream& s, const InformacionTermino& p);
    public:
        InformacionTermino(const InformacionTermino &);
        InformacionTermino();
        // Inicializa ftc = 0
        ~InformacionTermino();
        // Pone ftc = 0 y vacía l_docs
        InformacionTermino &operator=(const InformacionTermino &);
        // Añadir cuantos métodos se consideren necesarios para manejar la parte
        // privada de la clase private : int ftc; // Frecuencia total del término en la colección
        std::unordered_map<long int, InfTermDoc> l_docs;
        // Tabla Hash que se accederá por el id del documento, devolviendo un
        //objeto de la clase InfTermDoc que contiene toda la información de
        //    aparición del término en el documento
};


#endif