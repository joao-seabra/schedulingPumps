#include "util.h"

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"

bool validaSolucao(const Instancia &inst, const Solucao &s){
  double nivelReservatorio;
  for(const Reservatorio &r : inst.reservatorios){
    nivelReservatorio = r.getVolumeInicial();
    for(int i = 0; i < 24; i++){
      for(int bombaAcoplada : r.getBombasAcopladas()){
        if(s.getVetorSolucao().at(i + (bombaAcoplada * 24))){
          nivelReservatorio += inst.bombas.at(bombaAcoplada).getFluxo();
        }
      }
      if(nivelReservatorio < 0 || nivelReservatorio > r.getVolumeMax()){
        return false;
      }
    }
  }
  
  return true;
}

//TODO

bool validaSolucao(const Instancia &inst, const std::vector<bool> &s){
//todo
  std::cout << "Ainda não fiz, talvez precise no futuro" << std::endl;
  return false;
}
