#ifndef INSTANCIA_H
#define INSTANCIA_H

#include <vector>
#include <iostream>

#include "bomba.h"
#include "reservatorio.h"

typedef struct Instancia{
  int nBombas = 0;
  Reservatorio reservatorio = Reservatorio(0,0);
  std::vector<Bomba> bombas;
  std::vector<double> consumo;

  double tarifaPico = 0.0;
  double tarifaNormal = 0.0;
} Instancia;

/*Lê um arquivo txt com as informações do twls no formato:
  CAPACIDADE_RESERVATORIO VOLUME_INICIAL N_BOMBAS TARIFA_PICO TARIFA_FORA_PICO

  EFICIENCIA POTENCIA FLUXO H_TOPO

  Lê um arquivo txt com as informações do consumo no formato:
  N
  DADOS
*/
Instancia leInstancia(const std::string &nomeArquivoTWLS, const std::string &nomeArquivoConsumo);

std::ostream& operator<<(std::ostream& os, Instancia &inst);


#endif