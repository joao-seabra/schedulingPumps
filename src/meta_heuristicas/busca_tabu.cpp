#include "busca_tabu.h"

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../refinamento/descida.h"
#include "../utilitarios/aleatorio.h"

#include <deque>
#include <vector>


bool estaNaListaTabu(int posicao, int multiplicador,
                      const std::deque<std::pair<int, int>> &listaTabu)
{
    for (const auto &movimento : listaTabu) {
        if (movimento.first == posicao && movimento.second == multiplicador) {
            return true;
        }
    }
    return false;
}

void atualizaListaTabu(std::deque<std::pair<int, int>> &listaTabu, int tamanhoMax, 
                        int posicao, int multiplicador)
{
    // Lista tabu de tamanho fixo
    if(listaTabu.size() >= tamanhoMax){
        listaTabu.pop_front();
    }
    // proíbe o retorno para uma solução já vista
    listaTabu.push_back(std::pair<int, int>(posicao, -multiplicador));

}

double vizinhoMelhorTabu(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador, 
                        std::deque<std::pair<int, int>> &listaTabu)
{
    int n = s.getVetorSolucao().size();
    melhorI = 0;
    melhorMultiplicador = 0;
    double melhorFo = std::numeric_limits<double>::max();
    double f1 = s.getFAvaliacao();

    int hash = 0;

    int multiplicador = 1;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < 2; j++){
            multiplicador *= -1;
            f1 = s.getFAvaliacao() + s.calculaDeltaMovimento(i, multiplicador);
            if(!estaNaListaTabu(i, multiplicador, listaTabu)){
                    if(f1 < melhorFo){
                        melhorI = i;
                        melhorMultiplicador = multiplicador;
                        melhorFo = f1;
                    }
                }
        }
    }

    return melhorFo;
}

double busca_tabu(Instancia &inst, Solucao &s, int tamanhoLista, int tabuMax){
    Solucao sEstrela = s;
    double fo = s.getFAvaliacao();
    double foEstrela = sEstrela.getFAvaliacao();

    std::deque<std::pair<int, int>> listaTabu;

    int iterBT = 0;
    int melhorIter = 0;
    const int iterMaxAbsoluto = 100000; // evita ciclos infinitos

    int melhorI, melhorMultiplicador;
    while ((iterBT - melhorIter) < tabuMax && iterBT < iterMaxAbsoluto){
        iterBT++;
        fo = vizinhoMelhorTabu(inst, s, melhorI, melhorMultiplicador, listaTabu);
        s.movimento(melhorI, melhorMultiplicador);
        atualizaListaTabu(listaTabu, tamanhoLista, melhorI, melhorMultiplicador);

        if(fo < foEstrela){
            foEstrela = fo;
            sEstrela = s;
            melhorIter = iterBT;
        }

    }

    s = sEstrela;

    return s.funcaoAvaliacao();    
}

