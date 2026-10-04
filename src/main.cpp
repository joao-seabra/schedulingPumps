#include <iostream>
#include <iomanip>
#include <vector>

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


void imprimeSolucao(const Instancia &inst, Solucao &s);

int main(){
  semente(1000);

  double PENALIDADE1 = 100;
  double PENALIDADE2 = 200;
  double PENALIDADE3 = 0.05;

  // const std::string nomeArquivoTWLS("Instancia/T1R2B.txt");
  // const std::string nomeArquivoConsumo("Instancia/T1R2BCONSUMO.txt");

  const std::string nomeArquivoTWLS("Instancia/T2R2B.txt");
  const std::string nomeArquivoConsumo("Instancia/T2R2BCONSUMO.txt");

  Instancia inst = leInstancia(nomeArquivoTWLS, nomeArquivoConsumo);
  Solucao s(inst, PENALIDADE1, PENALIDADE2, PENALIDADE3);

  std::cout << "\nInstância:\n" << inst << std::endl;
  std::cout << "\nSolução:\n";
  imprimeSolucao(inst, s);

  Cronometro *cron = nullptr;
  int escolha = 0;
  do {
    escolha = menuPrincipal();
    if (escolha != 0 && escolha != 1) {
        std::cout << "\nLembre-se de gerar uma solucao" <<
                "inicial caso necessario (opcao 1)!\n";
        //continue;
    }

    switch (escolha) {
      case 1: { // Geração de uma solução inicial
        switch (menuSolucaoInicial()) {
          case 1: //Solução bang bang
          cron = new Cronometro();
          solucaoBangBang(inst, s);
          std::cout << "\nSolucao construida pelo método Bang-Bang:\n";
          break;
          case 2: //solução aleatoria
          cron = new Cronometro();
          solucaoAleatoria(inst, s);
          std::cout << "\nSolucao aleatória:\n";
          break;
        }

        if(cron != nullptr){
            std::cout << std ::fixed << std::setprecision(8)
                      << "Tempo de execução = " << cron->segundosDecorridos()
                      << "s\n";
          delete cron;
          cron = nullptr;
        }
        imprimeSolucao(inst, s);
        break;
      }

      case 2: { // Descida Randômica
          std::cout << "\nDescida Randômica:\n";
          cron = new Cronometro();
          randomDescent(inst, s, 0.7 * 24 * inst.nBombas);
          if(cron != nullptr){
            std::cout << std ::fixed << std::setprecision(8)
                      << "Tempo de execução = " << cron->segundosDecorridos()
                      << "s\n";
            delete cron;
            cron = nullptr;
          }
          imprimeSolucao(inst, s);
          break;
      }

      case 3: { // First improvement
          std::cout << "\nPrimeira melhora:\n";
          cron = new Cronometro();
          firstImprovement(inst, s);
          if(cron != nullptr){
            std::cout << std ::fixed << std::setprecision(8)
                      << "Tempo de execução = " << cron->segundosDecorridos()
                      << "s\n";;
            delete cron;
            cron = nullptr;
          }
          imprimeSolucao(inst, s);
          break;
      }

      case 4: { // multi-start
          std::cout << "\nMulti-Start:\n";
          cron = new Cronometro();
          multistart(inst, s, 100);
          if(cron != nullptr){
            std::cout << std ::fixed << std::setprecision(8)
                      << "Tempo de execução = " << cron->segundosDecorridos()
                      << "s\n";
            delete cron;
            cron = nullptr;
          }
          imprimeSolucao(inst, s);
          break;
      }

      case 5: { // Simulated Annealing
          std::cout << "\nSimulated Annealing:\n";
          cron = new Cronometro();

          double tempInicial = temperaturaInicial(inst, s, 1.1, 0.98, 500);
          std::cout << std::fixed << std::setprecision(2) 
                    << "\nTemperatura Inicial: " << tempInicial << std::endl;
          simulatedAnnealing(inst, s, 0.998, 10 * inst.nBombas * 24, tempInicial, 0.01);

          if(cron != nullptr){
            std::cout << std ::fixed << std::setprecision(8)
                      << "Tempo de execução = " << cron->segundosDecorridos()
                      << "s\n";
            delete cron;
            cron = nullptr;
          }
          imprimeSolucao(inst, s);
          break;
      }

      case 6: { // Imprimir solução atual;
        std::cout << "\nSolucao atual:\n";
        imprimeSolucao(inst, s);
        break;
      }

      case 7: { // Imprimir informações da Instância
          std::cout << "\nInstância:\n" << inst << std::endl;
          break;
      }

      default:
          break;
    }
  } while (escolha != 0);



  return 0;
}

void imprimeSolucao(const Instancia &inst, Solucao &s){
  double falta, excedente, variacaoNivel;
  s.calculaNivel(excedente, falta, variacaoNivel);
  std::cout << std::fixed << std::setprecision(2);
  std::cout << s << std::endl;
  std::cout << "\nCusto de energia elétrica de s:" << s.getCustoEnergia() << std::endl;
  std::cout << "\nExcedente de s:" << excedente << std::endl;
  std::cout << "\nPenalidade por excedente de s:" << s.getPenalidade1() << std::endl;
  std::cout << "\nFalta de s:" << falta << std::endl;
  std::cout << "\nPenalidade por falta de s:" << s.getPenalidade2() << std::endl;
  std::cout << "\nVariacao em relação ao volume inicial:" << variacaoNivel << std::endl;
  std::cout << "\nPenalidade por variação de volume:" << s.getPenalidade3() << std::endl;

  if (validaSolucao(inst, s)) std::cout << "\nSolução válida\n";
  else std::cout << "\nSoulução Inválida\n";

}
