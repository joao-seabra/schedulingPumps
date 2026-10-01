#include "instancia.h"

#include <iomanip>
#include <fstream>


Instancia leInstancia(const std::string &nomeArquivoTWLS, const std::string &nomeArquivoConsumo){
  std::ifstream arquivoTWLS(nomeArquivoTWLS);
  if(!arquivoTWLS){
    std::cerr << "O arquivo " << nomeArquivoTWLS
                   << " nao pode ser aberto.\n";
        std::exit(EXIT_FAILURE);
  }

  int nBombas;
  double capacidade, volumeInicial, tarifaPico, tarifaNormal;

  arquivoTWLS >> capacidade >> volumeInicial >> nBombas >> tarifaPico >> tarifaNormal;

  Instancia result;
  result.nBombas = nBombas;
  result.tarifaNormal = tarifaNormal;
  result.tarifaPico = tarifaPico;
  result.reservatorio = Reservatorio(capacidade, volumeInicial);

  double eficiencia, potencia, fluxo, htopo;
  for(int i = 0; i < result.nBombas; i++){
    arquivoTWLS >> eficiencia >> potencia >> fluxo >> htopo;
    result.bombas.push_back(Bomba(potencia, fluxo, htopo, eficiencia));
  }

  std::ifstream arquivoConsumo(nomeArquivoConsumo);
  if(!arquivoConsumo){
    std::cerr << "O arquivo " << nomeArquivoConsumo
    << " nao pode ser aberto.\n";
    std::exit(EXIT_FAILURE);
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
  std::exit(EXIT_FAILURE);
}

  return result;

}


std::ostream& operator<<(std::ostream& os, Instancia &inst){
  os << "Reservatorio: " << inst.reservatorio << std::endl;
  os << "Numero de bombas: " << inst.nBombas << std::endl;
  os << "Bombas:\n";
  for(Bomba b : inst.bombas){
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
