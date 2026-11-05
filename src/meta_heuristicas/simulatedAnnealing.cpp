#include "simulatedAnnealing.h"

#include <cmath>
#include <assert.h>

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../utilitarios/aleatorio.h"

double vizinhoSimulatedAnnealing(Instancia &inst, Solucao &s, int &i, int &multiplicador);


double vizinhoSimulatedAnnealing(Instancia &inst, Solucao &s, int &i, int &multiplicador){
  i = inteiroAleatorio(0, s.getVetorSolucao().size() - 1);
  int sinalMultiplicador = inteiroAleatorio(0, 1);
  multiplicador = !sinalMultiplicador ? -1 : 1;

  double foVizinho = s.calculaDeltaMovimento(i, multiplicador) + s.getFAvaliacao();

  return foVizinho;

}

double temperaturaInicial(Instancia &inst, Solucao &s, double beta, double gamma, int saMAX){
  double fo = s.getFAvaliacao();

  int aceitos = 0, i, multiplicador;
  
  double temperaturaInicial = 10;
  double foVizinho, delta, chanceAceitacao;
  
  bool continua = true;
  while(continua){
    aceitos = 0;

    for(int iter = 0; iter < saMAX; iter++){
      foVizinho = vizinhoSimulatedAnnealing(inst, s, i, multiplicador);
      delta = foVizinho - fo;

      if(delta < 0){
        aceitos++;
      }
      else{
        chanceAceitacao = std::exp(-delta/temperaturaInicial);
        if(realAleatorio(0.0, 1) < chanceAceitacao){
          aceitos++;
        }
      }
    }

    if(aceitos < gamma * saMAX)  temperaturaInicial *= beta;
    
    else  continua = false;

  }

  return temperaturaInicial;

}

double simulatedAnnealing(Instancia &inst, Solucao &s, double alpha, int saMAX, 
                          double tempInicial, double tempFinal)
{
  Solucao sEstrela(s);
  
  double temperatura = tempInicial;

  double chanceAceitacao, aceitacao, delta;
  double foVizinho = s.getFAvaliacao();

  int i, multiplicador;
  while(temperatura > tempFinal){
    for(int iter = 0; iter < saMAX; iter++){
      foVizinho = vizinhoSimulatedAnnealing(inst, s, i, multiplicador);

      if(foVizinho < s.getFAvaliacao()){
        s.movimento(i, multiplicador);
        if(s.getFAvaliacao() < sEstrela.getFAvaliacao()){
          sEstrela = s;
        }
      }
      else{
        delta = foVizinho - s.getFAvaliacao();
        
        aceitacao = realAleatorio(0.0, 1);
        chanceAceitacao = std::exp(-delta/temperatura);

        if(aceitacao < chanceAceitacao){
          s.movimento(i, multiplicador);
        }
      }
    }

    temperatura *= alpha;
  }
                          
  s = sEstrela;

  return s.funcaoAvaliacao();

}

