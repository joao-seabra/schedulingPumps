#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib> 
#include "CLI11.hpp"

#include "definicao_problema/instancia.h"
#include "definicao_problema/solucao.h"
#include "utilitarios/cronometro.h"
#include "utilitarios/util.h"
#include "utilitarios/aleatorio.h"
#include "interfaces/menu.h"

#include "construtivos/construcao.h"
#include "refinamento/descida.h"
#include "meta_heuristicas/multiStart.h"
#include "meta_heuristicas/simulatedAnnealing.h"

int main (int argc, char* argv[]) {

  CLI::App app{"Sintonia do Simulated Annealing via irace"};
  std::string nomeInstancia;
  int seed = 12345;

  //parametros a serem tunados
  int construcao = 1;
  double alpha = 0.99;
  int saMax = 100;
  double beta = 1.1;
  double gamma = 0.95;
  double tempFinal = 0.01;
  int penalidade1 = 1;
  int penalidade2 = 1;
  double penalidade3 = 0.01;

      // Configuração do CLI11
    app.add_option("-i,--instancia", nomeInstancia, "Nome da instância (ex: T1R2B)")->required();
    app.add_option("--seed", seed, "Semente aleatória enviada pelo irace");
    
    // Configurações do construtivo e do SA
    app.add_option("--construcao", construcao, "Construtivo: 1=Bang-Bang, 2=Aleatoria");
    app.add_option("--alpha", alpha, "Taxa de resfriamento do SA");
    app.add_option("--samax", saMax, "Número de iterações por temperatura (SAmax)");
    app.add_option("--beta", beta, "Fator de aquecimento (cálculo Temp Inicial)");
    app.add_option("--gamma", gamma, "Taxa de aceitação (cálculo Temp Inicial)");
    app.add_option("--tempfinal", tempFinal, "Temperatura de parada");
    app.add_option("--penalidade1", penalidade1, "penalidade de excesso");
    app.add_option("--penalidade2", penalidade2, "penalidade de falta");
    app.add_option("--penalidade3", penalidade3, "penalidade de variação do nível");

    // Realiza o parse dos argumentos
    CLI11_PARSE(app, argc, argv);

    // Carrega a instância (mantendo a lógica original do seu código de concatenar pastas)
    const std::string nomeArquivoTWLS = "Instancia/" + nomeInstancia + ".txt";
    const std::string nomeArquivoConsumo = "Instancia/" + nomeInstancia + "CONSUMO.txt";

    Instancia inst = leInstancia(nomeArquivoTWLS, nomeArquivoConsumo);
    Solucao s(inst, penalidade1, penalidade2, penalidade3);
    

    // Inicializa a semente
    semente(seed);


    // 1. Fase de Construção Inicial (Apenas determinísticas puras ou totalmente aleatória)
    if (construcao == 1) {
        solucaoBangBang(inst, s);
    } else if (construcao == 2) {
        solucaoAleatoria(inst, s);
    }
    // 2. Cálculo da Temperatura Inicial
    double tempInicial = temperaturaInicial(inst, s, beta, gamma, saMax);

    // 3. Execução do Simulated Annealing
    double foFinal = simulatedAnnealing(inst, s, alpha, saMax, tempInicial, tempFinal);

    // 4. RETORNO OBRIGATÓRIO PARA O IRACE
    // NENHUM outro std::cout deve ocorrer antes desta linha
    std::cout << foFinal << std::endl;



  return 0;
}

