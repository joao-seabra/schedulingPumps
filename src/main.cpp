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

  const double penalidade1 = 51;
  const double penalidade2 = 186;
  const double penalidade3 = 0.012;

  const std::string nomeArquivoTWLS("Instancia/T1R2B.txt");
  const std::string nomeArquivoConsumo("Instancia/T1R2BCONSUMO.txt");

  Instancia inst = leInstancia(nomeArquivoTWLS, nomeArquivoConsumo);
  Solucao s(inst, penalidade1, penalidade2, penalidade3);

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

          int saMAX = 348;
          double tempInicial = temperaturaInicial(inst, s, 1.1, 0.96, saMAX);
          std::cout << std::fixed << std::setprecision(2) 
                    << "\nTemperatura Inicial: " << tempInicial << std::endl;
          simulatedAnnealing(inst, s, 0.924, saMAX, tempInicial, 0.35);

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
