#ifndef UTIL_H
#define UTIL_H

#include <vector>

class Solucao;
class Instancia;

bool validaSolucao(const Instancia &inst, const Solucao &s);
bool validaEImprimeSolucao(const Instancia &inst, const Solucao &s);


#endif