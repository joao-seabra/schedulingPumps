#include "construcao.h"

#include <vector>

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/util.h"

double solucaoBangBang(const Instancia &inst, Solucao &s){
  double nivelReservatorio, fluxoTotal, base;
  bool ativas;
  s.zerarVetorSolucao();
  for(const Reservatorio &r : inst.reservatorios){
    fluxoTotal = 0;
    for(int b : r.getBombasAcopladas()){
      fluxoTotal += inst.bombas.at(b).getFluxo() * QUANT_MIN; //fluxo total em M3/min
    }  

    nivelReservatorio = r.getVolumeInicial();
    ativas = false;
    for(int i = 0; i < inst.nHoras; i++){
      base = nivelReservatorio - r.getConsumo(i);

      for(int k = 1; k <= (60 / QUANT_MIN); k++){
        if(ativas && base + (fluxoTotal * k) > r.getVolumeMax()){
          ativas = false;
        }
        if(base < r.getLimInferior()){
          ativas = true;
        }
        
        for(int b : r.getBombasAcopladas()){
          if(ativas){
            s.movimento(i + (b * inst.nHoras), +1);
          }
        }
        if(ativas){
          nivelReservatorio = base + (fluxoTotal * k);
        }
        
        else{
          nivelReservatorio = base;
        }
      }


      
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
    vetorAleatorio[i] = inteiroAleatorio(0, 60);
  }
  embaralhaVetor(vetorAleatorio);

  if (!s.alteraVetorSolucao(vetorAleatorio)){
    std::cerr << "\nVetor de solução aleatória não copiado" << std::endl;
  }
  

  return s.funcaoAvaliacao();
}
