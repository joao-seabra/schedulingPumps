#ifndef DESCIDA_H
#define DESCIDA_H

class Instancia;
class Solucao;

double randomDescent(Instancia &inst, Solucao &s, unsigned int itermax);

double firstImprovement(Instancia &inst, Solucao &s);


#endif