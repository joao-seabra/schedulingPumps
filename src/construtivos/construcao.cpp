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
    for(int i = 0; i < inst.nHoras; i++){
      base = nivelReservatorio - r.getConsumo(i);

      if(ativas && base + fluxoTotal > r.getVolumeMax()){
        ativas = false;
      }

      if(base < r.getLimInferior()){
        ativas = true;
      }

      for(int b : r.getBombasAcopladas()){
        if(s.getVetorSolucao().at(i + (b * inst.nHoras)) != ativas){
          s.movimentoInversao(i + (b * inst.nHoras));
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
  int tam = inst.nHoras * inst.nBombas;
  
  TipoSolucao vetorAleatorio;
  
  vetorAleatorio.assign(tam, 0); 

  int n = inteiroAleatorio(1, tam - 1);
  for(int i = 0; i < n; i++){
    vetorAleatorio[i] = inteiroAleatorio(0, 60);
  }
  embaralhaVetor(vetorAleatorio);

  if (!s.alteraVetorSolucao(vetorAleatorio)){
    std::cerr << "\nVetor de solução aleatória não copiado" << std::endl;
  }
  

  return s.funcaoAvaliacao();
}
