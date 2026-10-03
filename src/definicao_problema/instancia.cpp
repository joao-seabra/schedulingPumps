#include "instancia.h"

#include <iomanip>
#include <fstream>




Instancia leInstancia(const std::string &nomeArquivoTWLS, const std::string &nomeArquivoConsumo){
  std::ifstream arquivoTWLS(nomeArquivoTWLS);
  if(!arquivoTWLS){
    std::cerr << "O arquivo " << nomeArquivoTWLS
                   << " nao pode ser aberto.\n";
        std::exit(1);
  }
  Instancia result;

  char tipo;
  double tarifaPico, tarifaNormal;

  arquivoTWLS >> tarifaPico >> tarifaNormal;

  double capacidade, volumeInicial;

  double eficiencia, potencia, fluxo, htopo;
  int refReservatorio;

  result.nReservatorios = 0;
  result.nBombas = 0;
  result.tarifaNormal = tarifaNormal;
  result.tarifaPico = tarifaPico;

  while(arquivoTWLS >> tipo){
    if(std::tolower(tipo) == 'r'){
      arquivoTWLS >> capacidade >> volumeInicial;
      result.reservatorios.push_back(Reservatorio(capacidade, volumeInicial));
      result.nReservatorios++;
    }
    else if(std::tolower(tipo) == 'b'){
      arquivoTWLS >> eficiencia >> potencia >> fluxo >> htopo >> refReservatorio;
      if(refReservatorio + 1 > result.nReservatorios){
        std::cerr << "\nErro na leitura da bomba " << result.nBombas + 1 << std::endl;
        exit(1);
      }
      result.bombas.push_back(Bomba(potencia, fluxo, htopo, eficiencia, refReservatorio));
      result.nBombas++;
    }
    else{
      std::cerr << "Tipo indefinido no arquivo de INFO" << std::endl;
      exit(1);
    }
  }

  
  int reservatorioReferenciado;
  for(int i = 0; i < result.nBombas; i++){
    reservatorioReferenciado = result.bombas.at(i).getReservatorioAcoplado();
    result.reservatorios.at(reservatorioReferenciado).acoplarBomba(i);
  }

  std::ifstream arquivoConsumo(nomeArquivoConsumo);
  if(!arquivoConsumo){
    std::cerr << "O arquivo " << nomeArquivoConsumo
    << " nao pode ser aberto.\n";
    std::exit(1);
  }
  
  int nDados;
  arquivoConsumo >> nDados;

  result.consumo.reserve(nDados);

  double dado;
  while(arquivoConsumo >> dado){
    result.consumo.push_back(dado);
  }

  if ((int)result.consumo.size() != 24) {
  std::cerr << "Arquivo de consumo deve conter exatamente 24 valores (lidos: "
            << result.consumo.size() << ")\n";
  std::exit(1);
}

  return result;

}


std::ostream& operator<<(std::ostream& os, Instancia &inst){
  os << "Numero de reservatorios: " << inst.nReservatorios << std::endl;
  os << "Reservatorios:\n";
  for(Reservatorio &r : inst.reservatorios){
    os << r << std::endl;
  }
  os << "Numero de bombas: " << inst.nBombas << std::endl;
  os << "Bombas:\n";
  for(Bomba &b : inst.bombas){
    os << b << std::endl;
  }
  os << std::fixed << std::setprecision(3)
      << "Tarifa de pico: R$" << inst.tarifaPico << " Tarifa Normal: R$" << inst.tarifaNormal;
      
  os << "\n\nConsumo:\n";
  os << std::left << std::setprecision(1);
    for(int j = 0; j < 24; j++){
      os << std::setw(4) << j << " ";
    }
  os << std::endl;

  os << std::left;
  for(double d : inst.consumo){
    os << std::setw(4) << d << " ";
  }

  return os;
}
