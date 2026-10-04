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
  result.nReservatorios = 0;
  result.nBombas = 0;

  char tipo;
  
  double tarifaPico, tarifaNormal;
  
  double capacidade, volumeInicial;
  double eficiencia, potencia, fluxo, htopo;
  int refReservatorio;


  while(arquivoTWLS >> tipo){
    switch (std::tolower(tipo)){
      case 'r':{
        arquivoTWLS >> capacidade >> volumeInicial;
        result.reservatorios.push_back(Reservatorio(capacidade, volumeInicial));
        result.nReservatorios++;
        break;
      }
      case 'b': {
        arquivoTWLS >> eficiencia >> potencia >> fluxo >> htopo >> refReservatorio;
        if(refReservatorio + 1 > result.nReservatorios){
          std::cerr << "\nErro na leitura da bomba " << result.nBombas + 1 << std::endl;
          exit(1);
        }
        result.bombas.push_back(Bomba(potencia, fluxo, htopo, eficiencia, refReservatorio));
        result.nBombas++;
        break;
      }
      case 't':{
        arquivoTWLS >> tarifaPico >> tarifaNormal;
        result.tarifaNormal = tarifaNormal;
        result.tarifaPico = tarifaPico;
        break;
      }
      case '#':
        arquivoTWLS.ignore(1000, '\n');
        break;
      case '\n':
        arquivoTWLS.ignore(1000, '\n');
        break;
      
      default:{
        std::cerr << "Tipo indefinido no arquivo de INFO" << std::endl;
        exit(1);
        break;
      }
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

  if(nDados / 24 != result.nReservatorios){
    std::cerr << "Arquivo de consumo deve conter exatamente 24 valores por reservatório(lidos: "
            << nDados << ")\n";
    std::exit(1);
  }

  double dado;
  for(int i = 0; i < result.nReservatorios; i++){
    for(int j = 0; j < 24; j++){
      arquivoConsumo >> dado;
      result.reservatorios.at(i).adicionarConsumo(dado);
    }
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


  return os;
}
