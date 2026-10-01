#ifndef UTIL_H
#define UTIL_H

#include <vector>

class Solucao;
class Instancia;

bool validaSolucao(const Instancia &inst, const Solucao &s);
bool validaSolucao(const Instancia &inst, const std::vector<bool> &s);


#endif