#ifndef INSTANCIA_H
#define INSTANCIA_H

#include <vector>
#include <iostream>

#include "bomba.h"
#include "reservatorio.h"

typedef struct Instancia{
  std::vector<Reservatorio> reservatorios;
  int nReservatorios = 0;
  std::vector<Bomba> bombas;
  int nBombas = 0;

  double tarifaPico = 0.0;
  double tarifaNormal = 0.0;
} Instancia;

/*Lê um arquivo txt com as informações do twls no formato:
  TARIFA_PICO TARIFA_FORA_PICO
  TIPO CAPACIDADE_RESERVATORIO VOLUME_INICIAL

  TIPO EFICIENCIA POTENCIA FLUXO H_TOPO REFERENCIA_AO_RESERVATORIO

  Lê um arquivo txt com as informações do consumo no formato:
  N
  DADO1
  DADO2
  DADON
*/
Instancia leInstancia(const std::string &nomeArquivoTWLS, const std::string &nomeArquivoConsumo);

std::ostream& operator<<(std::ostream& os, Instancia &inst);


#endif