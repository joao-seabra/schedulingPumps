#include "construcao.h"

#include <vector>

#include "solucao.h"
#include "instancia.h"
#include "aleatorio.h"
#include "util.h"

double solucaoBangBang(const Instancia &inst, Solucao &s){
  double nivelReservatorio = inst.reservatorio.getVolumeInicial();

  bool ativas = false;

  for(int i = 0; i < 24; i++){
    nivelReservatorio -= inst.consumo.at(i); //consome agua naquela hora

    if(ativas){
      for(int j = 0; j < inst.nBombas; j++){
        if(!s.getVetorSolucao().at((i + (j * 24)))){
          s.movimentoInversao((i + (j * 24)));
        }
        nivelReservatorio += inst.bombas.at(j).getFluxo(); //bombea agua caso a bomba esteja ativa
      }
    }
    else{
      for(int j = 0; j < inst.nBombas; j++){
        if(s.getVetorSolucao().at((i + (j * 24)))){
          s.movimentoInversao((i + (j * 24)));
        }
      }
    }

    if(nivelReservatorio < inst.reservatorio.getLimInferior()){
      ativas = true;
      for(int j = 0; j < inst.nBombas; j++){
        if(!s.getVetorSolucao().at((i + (j * 24)))){
          nivelReservatorio += inst.bombas.at(j).getFluxo();
          s.movimentoInversao((i + (j * 24)));
        }
      }
    }

    else if(nivelReservatorio > inst.reservatorio.getVolumeMax()){
      ativas = false;
      for(int j = 0; j < inst.nBombas; j++){
        if(s.getVetorSolucao().at((i + (j * 24)))){
          s.movimentoInversao((i + (j * 24)));
        }
      }
    }

  }
  
  s.funcaoAvaliacao();

  return s.getFAvaliacao();
}

double solucaoAleatoria(const Instancia &inst, Solucao &s){
  int tam = 24 * inst.nBombas;
  
  std::vector<bool> vetorAleatorio;
  
  //mudo o vetor inteiro pra false, acho que não precisa, é tudo aleatório mesmo ne
  vetorAleatorio.assign(tam, false); 

  int n = inteiroAleatorio(1, tam - 1);
  for(int i = 0; i < n; i++){
    vetorAleatorio[i] = !vetorAleatorio[i];
  }
  embaralhaVetor(vetorAleatorio);

  if (!s.alteraVetorSolucao(vetorAleatorio)){
    std::cerr << "\nVetor de solução aleatória não copiado" << std::endl;
  }
  

  return s.getFAvaliacao();
}
