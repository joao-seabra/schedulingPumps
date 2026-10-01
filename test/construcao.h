#ifndef BANGBANG_H
#define BANGBANG_H

class Instancia;
class Solucao;

/*Resolve o problema desativando as bombas quando o reservatório passa de um limite superior
e ativando novamente quando passa de um limite inferior*/

double solucaoBangBang(const Instancia &inst, Solucao &s);
double solucaoAleatoria(const Instancia &inst, Solucao &s);


#endif