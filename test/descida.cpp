#include "descida.h"


#include "instancia.h"
#include "solucao.h"
#include "aleatorio.h"
#include "util.h"

#include <numeric>

double vizinhoRandomico(Instancia &inst, Solucao &s, int &melhorI);
double vizinhoRandomico(Instancia &inst, Solucao &s, int &melhorI, int &melhorJ);

double vizinhoPrimeiraMelhora(Instancia &inst, Solucao &s, int &melhorI);

double randomDescent(Instancia &inst, Solucao &s, unsigned int itermax){
  double foVizinho;
  unsigned int iter = 0;

  int melhorI;
  // int melhorJ;

  while(iter < itermax){
    iter++;
    foVizinho = vizinhoRandomico(inst, s, melhorI);
    // foVizinho = vizinhoRandomico(inst, s, melhorI, melhorJ);
    if(foVizinho < s.getFAvaliacao()){
      iter = 0;
      s.movimentoInversao(melhorI);
      // s.movimentoTroca(melhorI, melhorJ);
    }
  }
  

  return s.getFAvaliacao();
}

double vizinhoRandomico(Instancia &inst, Solucao &s, int &melhorI){

  int i = inteiroAleatorio(0, s.getVetorSolucao().size() - 1);

  double delta = s.calculaDeltaInversao(i);
  
  melhorI = i;

  return s.getFAvaliacao() + delta;

}

double vizinhoRandomico(Instancia &inst, Solucao &s, int &melhorI, int &melhorJ){
  int n = s.getVetorSolucao().size();

  int i, j;
  i = inteiroAleatorio(0, n-1);
  do{
    j = inteiroAleatorio(0, n-1);
  }while(j==i);

  double delta = s.calculaDeltaTroca(i, j);

  melhorI = i;
  melhorJ = j;

  return s.getFAvaliacao() + delta;

}

double vizinhoPrimeiraMelhora(Instancia &inst, Solucao &s, int &melhorI){
  int n = s.getVetorSolucao().size();

  double fo = s.getFAvaliacao();
  double delta = 0;
  bool melhorou = false;

  std::vector<int> ordem(n);
  std::iota(ordem.begin(), ordem.end(), 0);
  embaralhaVetor(ordem);

  for(int i = 0; i < n && !melhorou; i++){
    delta = s.calculaDeltaInversao(ordem[i]);
    if(delta < 0){
      melhorou = true;
      melhorI = ordem[i];
    }
  }

  return delta + fo;

}

double firstImprovement(Instancia &inst, Solucao &s){

  double foVizinho;
  int melhorI;
  bool melhorou = false;

  do{
    melhorou = false;

    foVizinho = vizinhoPrimeiraMelhora(inst, s, melhorI);
    if(foVizinho < s.getFAvaliacao()){
      s.movimentoInversao(melhorI);
      melhorou = true;
    }

  }while(melhorou);
  
  return s.getFAvaliacao();

}

