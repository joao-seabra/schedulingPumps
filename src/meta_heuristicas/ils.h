#ifndef ILS_H
#define ILS_H

class Solucao;
class Instancia;

double ils(Instancia &inst, Solucao &s, int vezesMax, int ilsMax);

#endif