#include "menu.h"

#include <iostream>

int menuPrincipal()
{
    int escolha;
    do {
        std::cout << "\n*******************Menu Principal************************* \n"
                   << "ATENCAO: Necessario gerar solucao inicial antes de refinar\n"
                   << "                1. Gere solucao inicial \n"
                   << "                2. Descida randomica \n"
                   << "                3. Descida com Primeiro de Melhora \n"
                   << "                4. Multi-Start \n"
                   << "                5. Simulated Annealing \n"

                   << "                6. Imprimir solução atual \n"
                   << "                7. Imprimir Informações da Instância \n"
                   << "                8. Imprimir Informações dos níveis dos reservatorios \n"
                   << "                0. Sair \n"
                   << "                Escolha: ";
        std::cin >> escolha;
    } while (escolha < 0 || escolha > 8);
    return escolha;
}

int menuSolucaoInicial()
{
    int escolha;
    do {
        std::cout << "\n************Geracao da Solucao Inicial**************** \n"
                   << "                1. Bang-Bang \n"
                   << "                2. Aleatoria \n"
                   << "                Escolha: ";
        std::cin >> escolha;
    } while (escolha < 1 || escolha > 2);
    return escolha;
}
