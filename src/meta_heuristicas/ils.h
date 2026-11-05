#ifndef ILS_H
#define ILS_H

typedef class Solucao;
typedef class Instancia;

double ils(Instancia &inst, Solucao &s, int vezesMax, int ilsMax);

#endif