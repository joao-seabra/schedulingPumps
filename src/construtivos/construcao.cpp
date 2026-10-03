#include "construcao.h"

#include <vector>

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/util.h"

double solucaoBangBang(const Instancia &inst, Solucao &s){
  double nivelReservatorio;
  bool ativas;
  for(const Reservatorio &r : inst.reservatorios){
    nivelReservatorio = r.getVolumeInicial();
    ativas = false;
    for(int i = 0; i < 24; i++){
      nivelReservatorio -= r.getConsumo(i);
      if(ativas){
        for(int bombaAcoplada : r.getBombasAcopladas()){
          if(!s.getVetorSolucao().at(i + (bombaAcoplada * 24))){
            s.movimentoInversao(i + (bombaAcoplada * 24));
          }
          nivelReservatorio += inst.bombas.at(bombaAcoplada).getFluxo();
        }
      }
      else{
        for(int bombaAcoplada : r.getBombasAcopladas()){
          if(s.getVetorSolucao().at(i + (bombaAcoplada * 24))){
            s.movimentoInversao(i + (bombaAcoplada * 24));
          }
        }
      }

      if(nivelReservatorio < r.getLimInferior()){
        ativas = true;
        for(int bombaAcoplada : r.getBombasAcopladas()){
          if(!s.getVetorSolucao().at(i + (bombaAcoplada * 24))){
            s.movimentoInversao(i + (bombaAcoplada * 24));
            nivelReservatorio += inst.bombas.at(bombaAcoplada).getFluxo();
          }
        }
      }

      else if(nivelReservatorio > r.getVolumeMax()){
        ativas = false;
        for(int bombaAcoplada : r.getBombasAcopladas()){
          if(s.getVetorSolucao().at(i + (bombaAcoplada * 24))){
            s.movimentoInversao(i + (bombaAcoplada * 24));
          }
        }
      }

    }
  }

  return s.funcaoAvaliacao();
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
  

  return s.funcaoAvaliacao();
}
