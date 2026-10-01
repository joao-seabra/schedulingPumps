#include "simulatedAnnealing.h"

#include <cmath>
#include "aleatorio.h"

double vizinhoSimulatedAnnealing(Instancia &inst, Solucao &s, int &i);
double vizinhoSimulatedAnnealing(Instancia &inst, Solucao &s, int &i, int &j);

double vizinhoSimulatedAnnealing(Instancia &inst, Solucao &s, int &i, int &j){
  int n = s.getVetorSolucao().size();
  i = inteiroAleatorio(0, n - 1);

  do{
    j = inteiroAleatorio(0, n - 1);
  } while(i==j);

  double foVizinho = s.calculaDeltaTroca(i, j) + s.getFAvaliacao();

  return foVizinho;
}


double vizinhoSimulatedAnnealing(Instancia &inst, Solucao &s, int &i){
  i = inteiroAleatorio(0, s.getVetorSolucao().size() - 1);

  double foVizinho = s.calculaDeltaInversao(i) + s.getFAvaliacao();

  return foVizinho;

}

double temperaturaInicial(Instancia &inst, Solucao &s, double beta, double gamma, int saMAX){
  double fo = s.getFAvaliacao();

  int aceitos = 0, i;
  
  double temperaturaInicial = 10;
  double foVizinho, delta, chanceAceitacao;
  
  bool continua = true;
  while(continua){
    aceitos = 0;

    for(int iter = 0; iter < saMAX; iter++){
      foVizinho = vizinhoSimulatedAnnealing(inst, s, i);
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

  int i;
  while(temperatura > tempFinal){
    for(int iter = 0; iter < saMAX; iter++){
      foVizinho = vizinhoSimulatedAnnealing(inst, s, i);

      if(foVizinho < s.getFAvaliacao()){
        s.movimentoInversao(i);
        if(s.getFAvaliacao() < sEstrela.getFAvaliacao()){
          sEstrela.copia(s);
        }
      }
      else{
        delta = foVizinho - s.getFAvaliacao();
        
        aceitacao = realAleatorio(0.0, 1);
        chanceAceitacao = std::exp(-delta/temperatura);

        if(aceitacao < chanceAceitacao){
          s.movimentoInversao(i);
        }
      }
    }

    temperatura *= alpha;
  }
                          
  s.copia(sEstrela);

  return s.getFAvaliacao();

}

