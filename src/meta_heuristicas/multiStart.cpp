#include "multiStart.h"

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../utilitarios/aleatorio.h"

#include "../refinamento/descida.h"
#include "../construtivos/construcao.h"
#include "../utilitarios/util.h"
#include "../utilitarios/cronometro.h"

double multistart(Instancia &inst, Solucao &s, double tempoMax){
  Cronometro cron;
  Solucao sEstrela(s);

  if(s.getVetorSolucao().empty()){
    solucaoBangBang(inst, s);
  }

  else s.funcaoAvaliacao();

  int iter = 0;
  while(cron.segundosDecorridos() < tempoMax){
    iter++;
    solucaoAleatoria(inst, sEstrela);
    randomDescent(inst, sEstrela, 100);

     if(sEstrela.getFAvaliacao() < s.getFAvaliacao()){
       s = sEstrela; 
       iter = 0;
     }

  }

  return s.getFAvaliacao();
}

