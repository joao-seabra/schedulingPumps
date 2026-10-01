#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H



class Instancia;
class Solucao;

double temperaturaInicial(Instancia &inst, Solucao &s, double beta, double gamma, int saMAX);

double simulatedAnnealing(Instancia &inst, Solucao &s, double alpha, int saMAX, double tempInicial, double tempFinal);

#endif