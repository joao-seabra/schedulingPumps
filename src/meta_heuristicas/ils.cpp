#include "ils.h"


#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../refinamento/descida.h"
#include "../utilitarios/aleatorio.h"

void selecionaVizinho(Solucao &s);

Solucao perturbacao(Instancia &inst, Solucao &s, int nivel){
  Solucao solucaoPerturbada = s;
  int nModificacoes = nivel + 1;
  int cont = 0;
  while (cont < nModificacoes){
    selecionaVizinho(solucaoPerturbada);
    cont++;
  }


  return solucaoPerturbada;
}

void selecionaVizinho(Solucao &s){
  int pos = inteiroAleatorio(1, s.getVetorSolucao().size());
  pos = pos - 1; //evitar overflow do .size() tava dando segmentation fault
  int sinalMultiplicador = inteiroAleatorio(0, 1);
  sinalMultiplicador = (!sinalMultiplicador) ? -1 : 1;

  s.movimento(pos, sinalMultiplicador);
}

double ils(Instancia &inst, Solucao &s, int vezesMax, int nivelMax, int ilsMax){

  int iter = 0, melhorIter = 0;
  int nivel = 1, contNivel = 1;

  firstImprovement(inst, s);
  Solucao sLinha = s;
  
  while(iter - melhorIter < ilsMax){
    iter++;
    sLinha = perturbacao(inst, s, nivel);
    firstImprovement(inst, sLinha);

    if(sLinha.getFAvaliacao() < s.getFAvaliacao()){
      s = sLinha;
      melhorIter = iter;
      nivel = 1;
      contNivel = 1;
    }

    else if(++contNivel > vezesMax) {
      nivel++;
      if(nivel > nivelMax)
        nivel = 1;
      contNivel = 1;
    }
  }

  return s.funcaoAvaliacao();
}
