#include "multiStart.h"

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../utilitarios/aleatorio.h"

#include "../refinamento/descida.h"
#include "../construtivos/construcao.h"
#include "../utilitarios/util.h"

double multistart(Instancia &inst, Solucao &s, int iterMax){
  Solucao sEstrela(s);
  /*
  if(s.getVetorSolucao().empty() || !validaSolucao(inst, s)){
    solucaoBangBang(inst, s);
  } */

  if(s.getVetorSolucao().empty()){
    solucaoBangBang(inst, s);
  }

  else s.funcaoAvaliacao();

  int iter = 0;
  while(iter < iterMax){
    iter++;
    solucaoAleatoria(inst, sEstrela);
    randomDescent(inst, sEstrela, 100);

    /*
    if(sEstrela.getFAvaliacao() < s.getFAvaliacao() && validaSolucao(inst, sEstrela)){
      s.copia(sEstrela); 
      iter = 0;
    }
    */
     if(sEstrela.getFAvaliacao() < s.getFAvaliacao()){
       s.copia(sEstrela); 
       iter = 0;
     }

  }

  return s.getFAvaliacao();
}

