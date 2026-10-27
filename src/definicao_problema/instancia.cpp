#include "instancia.h"

#include <iomanip>
#include <fstream>

#include <cctype>
#include <cstdlib>




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
  
  int nHoras;
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
        if(refReservatorio + 1 > result.nReservatorios || refReservatorio < 0){
          std::cerr << "\nErro na leitura da bomba " << result.nBombas + 1 << std::endl;
          exit(1);
        }
        result.bombas.push_back(Bomba(potencia, fluxo / 60, htopo, eficiencia, refReservatorio));
        result.nBombas++;
        break;
      }
      case 't':{
        arquivoTWLS >> tarifaPico >> tarifaNormal;
        result.tarifaNormal = tarifaNormal / 60;
        result.tarifaPico = tarifaPico / 60;
        break;
      }
      case 'n':{
        arquivoTWLS >> nHoras;
        result.nHoras = nHoras;
        break;
      }
      case 'v':{
        if(!result.tarifaPorHora.empty()){
          std::cerr << "Vetor de tarifa por hora já preenchido" << std::endl;
          exit(1);
        }
        double tarifaMax = 0, tarifaMin = __DBL_MAX__;
        double tarifa;
        for(int i = 0; i < result.nHoras; i++){
          if(!(arquivoTWLS >> tarifa)){
            std::cerr << "Erro ao ler tarifas" << std::endl;
            exit(1);
          }
          tarifa = tarifa / 60;
          if(tarifa > tarifaMax) tarifaMax = tarifa;
          if(tarifa < tarifaMin) tarifaMin = tarifa;
          result.tarifaPorHora.push_back(tarifa);
        }
        
        int tarifasLidas = result.tarifaPorHora.size();
        if(tarifasLidas != result.nHoras || tarifasLidas == 0){
          std::cerr << "Número inválido de tarifas no vetor" << std::endl;
          exit(1);
        }

        result.tarifaNormal = tarifaMin;
        result.tarifaPico = tarifaMax;
        break;
      }
      case '#':
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

  if(nDados != result.nHoras * result.nReservatorios){
    std::cerr << "Arquivo de consumo deve conter exatamente " << result.nHoras 
              <<" valores por reservatório(lidos: " << nDados << ")\n";
    std::exit(1);
  }

  double dado;
  for(int i = 0; i < result.nReservatorios; i++){
    for(int j = 0; j < result.nHoras; j++){
      arquivoConsumo >> dado;
      result.reservatorios.at(i).adicionarConsumo(dado);
    }
  }

  if(result.tarifaPorHora.empty()){
    result.tarifaPorHora.assign(result.nHoras, result.tarifaNormal);
    for(int i = 0; i < result.nHoras; i++){
      int horaDoDia = i % 24;
      if(horaDoDia >= 18 && horaDoDia < 21) 
        result.tarifaPorHora.at(i) = tarifaPico;

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
  os << "Horas que serao planejadas: " << inst.nHoras << std::endl;
  os << std::fixed << std::setprecision(3)
      << "Tarifa de pico: R$" << inst.tarifaPico * 60 << " Tarifa Normal: R$" << inst.tarifaNormal * 60;


  return os;
}
