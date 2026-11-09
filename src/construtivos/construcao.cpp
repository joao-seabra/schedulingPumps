#include "construcao.h"

#include <vector>

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/util.h"

double solucaoBangBang(const Instancia &inst, Solucao &s){
  double nivelReservatorio, fluxoTotal, base, volumeBombeado;
  bool ativas;
  s.zerarVetorSolucao();
  for(const Reservatorio &r : inst.reservatorios){
    fluxoTotal = 0;
    for(int b : r.getBombasAcopladas()){
      fluxoTotal += inst.bombas.at(b).getFluxo() * Solucao::getQUANT_MIN(); //fluxo total em M3/min
    }  

    nivelReservatorio = r.getVolumeInicial();
    ativas = false;
    for(int i = 0; i < inst.nHoras; i++){
      base = nivelReservatorio - r.getConsumo(i);
      volumeBombeado = 0;

      for(int k = 1; k <= (60 / Solucao::getQUANT_MIN()); k++){
        if(ativas && base + fluxoTotal + volumeBombeado > r.getVolumeMax()){
          ativas = false;
          break;
        }
        if(base + volumeBombeado < r.getLimInferior()){
          ativas = true;
        }
        
        if(ativas){
          for(int b : r.getBombasAcopladas()){
            s.movimento(i + (b * inst.nHoras), +1);
          }
        }
        
        if(ativas){
          volumeBombeado += fluxoTotal;
        }    

      }

      nivelReservatorio = base + volumeBombeado;

      
    }

  }

  return s.funcaoAvaliacao();
}

double solucaoAleatoria(const Instancia &inst, Solucao &s){
  int tam = inst.nHoras * inst.nBombas;
  
  TipoVetorSolucao vetorAleatorio;
  
  vetorAleatorio.assign(tam, 0); 

  int n = inteiroAleatorio(1, tam - 1);
  for(int i = 0; i < n; i++){
    vetorAleatorio[i] = inteiroAleatorio(0, (int)(60 / Solucao::getQUANT_MIN())) * Solucao::getQUANT_MIN();
  }
  embaralhaVetor(vetorAleatorio);

  if (!s.alteraVetorSolucao(vetorAleatorio)){
    std::cerr << "\nVetor de solução aleatória não copiado" << std::endl;
  }
  

  return s.funcaoAvaliacao();
}
