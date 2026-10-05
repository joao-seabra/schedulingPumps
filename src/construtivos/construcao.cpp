#include "construcao.h"

#include <vector>

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/util.h"

double solucaoBangBang(const Instancia &inst, Solucao &s){
  double nivelReservatorio, fluxoTotal, base;
  bool ativas;
  for(const Reservatorio &r : inst.reservatorios){
    fluxoTotal = 0;
    for(int b : r.getBombasAcopladas()){
      fluxoTotal += inst.bombas.at(b).getFluxo();
    }  

    nivelReservatorio = r.getVolumeInicial();
    ativas = false;
    for(int i = 0; i < 24; i++){
      base = nivelReservatorio - r.getConsumo(i);

      if(ativas && base + fluxoTotal > r.getVolumeMax()){
        ativas = false;
      }

      if(base < r.getLimInferior()){
        ativas = true;
      }

      for(int b : r.getBombasAcopladas()){
        if(s.getVetorSolucao().at(i + (b * 24)) != ativas){
          s.movimentoInversao(i + (b * 24));
        }
      }
      if(ativas){
        nivelReservatorio = base + fluxoTotal;
      }
      else{
        nivelReservatorio = base;
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
