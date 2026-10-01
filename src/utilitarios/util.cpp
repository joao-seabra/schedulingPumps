#include "util.h"

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"

bool validaSolucao(const Instancia &inst, const Solucao &s){
  //conferir se o reservatório ultrapassa seu máximo ou chega a um valor negativo
  double nivelReservatorio = inst.reservatorio.getVolumeInicial();

  for(int i = 0; i < 24; i++){
    nivelReservatorio -= inst.consumo.at(i); //consome agua naquela hora
    for(int j = 0; j < inst.nBombas; j++){
      if(s.getVetorSolucao().at((i + (j * 24)))){
        nivelReservatorio += inst.bombas.at(j).getFluxo(); //bombea agua caso a bomba esteja ativa
      }
    }
    if(nivelReservatorio < 0 || nivelReservatorio > inst.reservatorio.getVolumeMax()){
      return false;
    }
  }

  
  return true;
}

bool validaSolucao(const Instancia &inst, const std::vector<bool> &s){
    //conferir se o reservatório ultrapassa seu máximo ou chega a um valor negativo
  double nivelReservatorio = inst.reservatorio.getVolumeInicial();

  for(int i = 0; i < 24; i++){
    nivelReservatorio -= inst.consumo.at(i); //consome agua naquela hora
    for(int j = 0; j < inst.nBombas; j++){
      if(s.at((i + (j * 24)))){
        nivelReservatorio += inst.bombas.at(j).getFluxo(); //bombea agua caso a bomba esteja ativa
      }
    }
    if(nivelReservatorio < 0 || nivelReservatorio > inst.reservatorio.getVolumeMax())
      return false;
  }

  
  return true;
}
