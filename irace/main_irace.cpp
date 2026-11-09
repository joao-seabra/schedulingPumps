#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> 
#include "CLI11.hpp"

#include "../src/definicao_problema/instancia.h"
#include "../src/definicao_problema/solucao.h"
#include "../src/utilitarios/cronometro.h"
#include "../src/utilitarios/util.h"
#include "../src/utilitarios/aleatorio.h"
#include "../src/interfaces/menu.h"

#include "../src/construtivos/construcao.h"
#include "../src/refinamento/descida.h"
#include "../src/meta_heuristicas/multiStart.h"
#include "../src/meta_heuristicas/simulatedAnnealing.h"
#include "../src/meta_heuristicas/ils.h"
#include "../src/meta_heuristicas/busca_tabu.h"

int main (int argc, char* argv[]) {

  CLI::App app{"Sintonia do Simulated Annealing via irace"};
  std::string nomeInstancia;
  int seed = 12345;

  //parametros a serem tunados
  int construcao = 1;
  int metodoUsado = 1;
  double alpha = 0.99;
  int saMax = 100;
  double beta = 1.1;
  double gamma = 0.95;
  double tempFinal = 0.01;

  int ils_vezes_max = 5;
  int ils_nivel_max = 8;

  int tamanho_lista_tabu = 7;

  int penalidade1 = -1;
  int penalidade2 = -1;
  double penalidade3 = -1;
  double tempoMAX = 2.5;
  int quant_min = 10;

      // Configuração do CLI11
    app.add_option("-i,--instancia", nomeInstancia, "Nome da instância (ex: T1R2B)")->required();
    app.add_option("--seed", seed, "Semente aleatória enviada pelo irace");
    
    // Configurações do construtivo e do SA
    app.add_option("--construcao", construcao, "Construtivo: 1=Bang-Bang, 2=Aleatoria");
    app.add_option("--metodo", metodoUsado, 
      "MetaHeurística: 1=Simulated Annealing, 2=Multi start, 3=ILS, 4=Busca Tabu");

    app.add_option("--alpha", alpha, "Taxa de resfriamento do SA");
    app.add_option("--samax", saMax, "Número de iterações por temperatura (SAmax)");
    app.add_option("--beta", beta, "Fator de aquecimento (cálculo Temp Inicial)");
    app.add_option("--gamma", gamma, "Taxa de aceitação (cálculo Temp Inicial)");
    app.add_option("--tempfinal", tempFinal, "Temperatura de parada");
    
    app.add_option("--ils_vezes_max", ils_vezes_max, "Numero de vezes que o ils se mantem no mesmo nível");
    app.add_option("--ils_nivel_max", ils_nivel_max, "Nível máximo do ils");
    
    app.add_option("--tamanho_lista_tabu", tamanho_lista_tabu, "Tamanho da lista tabu mantida");


    app.add_option("--penalidade1", penalidade1, "penalidade de excesso");
    app.add_option("--penalidade2", penalidade2, "penalidade de falta");
    app.add_option("--penalidade3", penalidade3, "penalidade de variação do nível");
    app.add_option("--quant_min", quant_min, "quantidade mínima em minutos que uma bomba fica ligada");
    app.add_option("--tempo_max", tempoMAX, "Critério de parada por tempo (s) para as metaheurísticas");

    // Realiza o parse dos argumentos
    CLI11_PARSE(app, argc, argv);

    // Carrega a instância (mantendo a lógica original do seu código de concatenar pastas)
    const std::string nomeArquivoTWLS = "Instancia/" + nomeInstancia + ".txt";
    const std::string nomeArquivoConsumo = "Instancia/" + nomeInstancia + "CONSUMO.txt";

    Instancia inst = leInstancia(nomeArquivoTWLS, nomeArquivoConsumo);
    Solucao s(inst, penalidade1, penalidade2, penalidade3, quant_min);
    

    // Inicializa a semente
    semente(seed);

    // 1. Fase de Construção Inicial (Apenas determinísticas puras ou totalmente aleatória)
    if (construcao == 1) {
      solucaoBangBang(inst, s);
    } else {
      solucaoAleatoria(inst, s);
    }
    // 2. Escolha da metaHeuristica
    // "MetaHeurística: 1=Simulated Annealing, 2=Multi start, 3=ILS, 4=Busca Tabu"

    double foFinal = s.getFAvaliacao();
    switch (metodoUsado)
    {
    case 1:
      double tempInicial = temperaturaInicial(inst, s, beta, gamma, saMax);
      foFinal = simulatedAnnealing(inst, s, alpha, saMax, tempoMAX, tempInicial, tempFinal);
      break;
    case 2:
      multistart(inst, s, tempoMAX);
      break;
    case 3:
      ils(inst, s, ils_vezes_max, ils_nivel_max, tempoMAX);
      break;
    case 4:
      busca_tabu(inst, s, tamanho_lista_tabu, tempoMAX);
      break;
    default:
      break;
    }

    // 3. RETORNO OBRIGATÓRIO PARA O IRACE
    // NENHUM outro std::cout deve ocorrer antes desta linha
    std::cout << foFinal << std::endl;



  return 0;
}

