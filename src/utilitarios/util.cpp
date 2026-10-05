#include "util.h"

#include "../definicao_problema/solucao.h"
#include "../definicao_problema/instancia.h"

bool validaSolucao(const Instancia &inst, const Solucao &s){
  double nivelReservatorio;
  for(const Reservatorio &r : inst.reservatorios){
    nivelReservatorio = r.getVolumeInicial();
    for(int i = 0; i < 24; i++){
      nivelReservatorio -= r.getConsumo(i);
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

bool validaEImprimeSolucao(const Instancia &inst, const Solucao &s){
  double nivelReservatorio;
  bool result = true;
  int horaInvalida = -1;
  for(const Reservatorio &r : inst.reservatorios){
    nivelReservatorio = r.getVolumeInicial();
    std::cout << "\nReservatório:\n" << r << std::endl;
    std::cout << "\nHora | Volume\n";
    std::cout << std::left;
    for(int i = 0; i < 24; i++){
      nivelReservatorio -= r.getConsumo(i);
      for(int bombaAcoplada : r.getBombasAcopladas()){
        if(s.getVetorSolucao().at(i + (bombaAcoplada * 24))){
          nivelReservatorio += inst.bombas.at(bombaAcoplada).getFluxo();
        }
      }
      std::cout << std::setw(2) << i << "   | " << nivelReservatorio << "\n";
      if(nivelReservatorio < 0 || nivelReservatorio > r.getVolumeMax()){
        result = false;
        horaInvalida = i;
      }
    }
    std::cout << std::endl;
    if(horaInvalida > 0){
      std::cout << "\nUltima Hora inválida: " << horaInvalida;
      horaInvalida = -1;
    }
  }
  
  return result;
}
