#include "../../include/filesystem/file.hpp"

/**
 * Restituisce la dimensione del file in byte
 * 
 * @return: dimensione file in byte
 */
const uintmax_t File::getSize()
{
    return this->size;
}

/**
 * stampa nome e dimensione del file con indentazione appropriata
 * @indent: numero di spazi
 */
const void File::ls(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        std::cout << " ";
    }

    std::cout << this->getName() << " " << this->getSize() << std::endl;
}

/**
 * restituisce il tipo dell’istanza (Directory o File) codificato come intero ?
 * 
 * @return: tipo oggetto
 */
const int File::mType()
{
    return 1;
}

std::shared_ptr<File> File::makeFile(std::string name, uintmax_t size)
{
    return std::shared_ptr<File>(new File(name, size));
}