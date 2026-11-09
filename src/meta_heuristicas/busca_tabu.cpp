#include "busca_tabu.h"

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../refinamento/descida.h"
#include "../utilitarios/aleatorio.h"

#include <deque>
#include <vector>

typedef struct tipoTabu{
    int posicao;
    int multiplicador;
    int hashSolucao;
}tipoTabu;

int hashTabu(const Solucao &s){
    const int p = 7; // numero primo
    int hash = 0;
    int i = 1;
    for(const auto &valor : s.getVetorSolucao()){
        hash += (valor * i * p) % (int)2e32;
        i++;
    }
    return hash;
}


bool estaNaListaTabu(int posicao, int multiplicador, int &hashEncontrado,
                      const std::deque<tipoTabu> &listaTabu)
{
    for (const auto &movimento : listaTabu) {
        if (movimento.posicao == posicao && movimento.multiplicador == multiplicador) {
            hashEncontrado = movimento.hashSolucao;
            return true;
        }
    }
    return false;
}

bool criterioAspiracao(const Solucao &sLinha, double foEstrela, int hashEncontrado, double chanceAceitacao){
    if(sLinha.getFAvaliacao() < foEstrela) 
        return true; // aceita uma solução melhor
    if(hashEncontrado != hashTabu(sLinha)){
        double aleatorio = realAleatorio(0.0, 1);
        if(aleatorio <= chanceAceitacao)
            return true;
    }
    return false;
}

void atualizaListaTabu(std::deque<tipoTabu> &listaTabu, int tamanhoMax, 
                        Solucao &s, int posicao, int multiplicador)
{
    // Lista tabu de tamanho fixo
    if(listaTabu.size() >= tamanhoMax){
        listaTabu.pop_front();
    }
    // proíbe o retorno para uma solução já vista
    listaTabu.push_back(tipoTabu{posicao, -multiplicador, hashTabu(s)});

}

double vizinhoMelhorTabu(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador, 
                        double foEstrela, std::deque<tipoTabu> &listaTabu)
{
    int n = s.getVetorSolucao().size();
    melhorI = 0;
    melhorMultiplicador = 0;
    double melhorFo = std::numeric_limits<double>::max();
    
    Solucao sLinha = s;
    double foLinha = s.getFAvaliacao();

    int hash = 0;

    int multiplicador = 1;
    for(int i = 0; i < n; i++){
        int valorI = sLinha.getVetorSolucao().at(i); // Usado para conferir se o movimento é válido
        for(int j = 0; j < 2; j++){
            multiplicador *= -1;

            //calculando previamente o efeito do movimento naquela posicao da solução
            int proximoValor = valorI + multiplicador * Solucao::getQUANT_MIN();
            if(proximoValor > 60) 
                proximoValor = 60;
            if(proximoValor < 0)
                proximoValor = 0;
            
            if(proximoValor == valorI){
                continue; // movimento sem alteração
            }

            sLinha.movimento(i, multiplicador);
            foLinha = sLinha.getFAvaliacao();

            if((!estaNaListaTabu(i, multiplicador, hash, listaTabu)) || criterioAspiracao(sLinha, foEstrela, hash, 0.6)){
                if(foLinha < melhorFo){
                    melhorI = i;
                    melhorMultiplicador = multiplicador;
                    melhorFo = foLinha;
                }
            }
            sLinha.movimento(i, -multiplicador); // desfaz o movimento
            foLinha = sLinha.getFAvaliacao();
        }
    }

    return melhorFo;
}

double busca_tabu(Instancia &inst, Solucao &s, int tamanhoLista, int tabuMax){
    Solucao sEstrela = s;
    double fo = s.getFAvaliacao();
    double foEstrela = sEstrela.getFAvaliacao();

    std::deque<tipoTabu> listaTabu;

    int iterBT = 0;
    int melhorIter = 0;
    const int iterMaxAbsoluto = 100000; // evita ciclos infinitos

    int melhorI, melhorMultiplicador;
    while ((iterBT - melhorIter) < tabuMax && iterBT < iterMaxAbsoluto){
        iterBT++;
        fo = vizinhoMelhorTabu(inst, s, melhorI, melhorMultiplicador, foEstrela, listaTabu);
        
        if(melhorMultiplicador == 0){
            listaTabu.clear(); 
            continue; // Reset da lista tabu quando não encontra vizinho válido
        }
        
        s.movimento(melhorI, melhorMultiplicador);
        atualizaListaTabu(listaTabu, tamanhoLista, s, melhorI, melhorMultiplicador);

        if(fo < foEstrela){
            foEstrela = fo;
            sEstrela = s;
            melhorIter = iterBT;
        }

    }

    s = sEstrela;

    return s.funcaoAvaliacao();    
}

