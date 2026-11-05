#include "descida.h"


#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/util.h"

#include <numeric>

double vizinhoRandomico(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador);

double vizinhoPrimeiraMelhora(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador);

double randomDescent(Instancia &inst, Solucao &s, unsigned int itermax){
  double foVizinho;
  unsigned int iter = 0;

  int melhorI, melhorMultiplicador;

  while(iter < itermax){
    iter++;
    foVizinho = vizinhoRandomico(inst, s, melhorI, melhorMultiplicador);
    if(foVizinho < s.getFAvaliacao()){
      iter = 0;
      s.movimento(melhorI, melhorMultiplicador);
    }
  }
  

  return s.funcaoAvaliacao();
}

double vizinhoRandomico(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador){

  int i = inteiroAleatorio(0, s.getVetorSolucao().size() - 1);
  int multiplicador = inteiroAleatorio(0, 1);
  multiplicador = !multiplicador ? -1 : multiplicador;

  double delta = s.calculaDeltaMovimento(i, multiplicador);
  
  melhorI = i;
  melhorMultiplicador = multiplicador;

  return s.getFAvaliacao() + delta;

}


double vizinhoPrimeiraMelhora(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador){
  int n = s.getVetorSolucao().size();

  double fo = s.getFAvaliacao();
  double delta = 0;
  bool melhorou = false;

  std::vector<int> ordem(n);
  std::iota(ordem.begin(), ordem.end(), 0);
  embaralhaVetor(ordem);

  int multiplicador = 1;
  for(int i = 0; i < n && !melhorou; i++){
    for(int j = 0; j < 2 && !melhorou; j++){
      multiplicador *= -1;
      delta = s.calculaDeltaMovimento(ordem[i], multiplicador);
      if(delta < -1e-9){
        melhorou = true;
        melhorI = ordem[i];
        melhorMultiplicador = multiplicador;
      }
    }
  }

  return delta + fo;

}

double firstImprovement(Instancia &inst, Solucao &s){

  double foVizinho;
  int melhorI, melhorMultiplicador;
  bool melhorou = false;

  do{
    melhorou = false;

    foVizinho = vizinhoPrimeiraMelhora(inst, s, melhorI, melhorMultiplicador);
    if(foVizinho < s.getFAvaliacao()){
      s.movimento(melhorI, melhorMultiplicador);
      melhorou = true;
    }

  }while(melhorou);
  
  return s.funcaoAvaliacao();

}

