#include "busca_tabu.h"

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../refinamento/descida.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/cronometro.h"

#include <deque>
#include <vector>
#include <functional>

typedef struct tipoTabu{
    int posicao;
    int multiplicador;
    uint64_t hashSolucao;
}tipoTabu;

uint64_t hashTabu(const Solucao &s){
    uint64_t hash = 14695981039346656037ULL; //offset do hash
    uint64_t prime = 1099511628211ULL; //numero primo
    
    for(const auto &valor : s.getVetorSolucao()){
        hash ^= static_cast<uint64_t>(valor);
        hash *= prime;
    }
    return hash;
}


bool estaNaListaTabu(int posicao, int multiplicador, uint64_t &hashEncontrado,
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

bool criterioAspiracao(const Solucao &sLinha, double foEstrela, uint64_t hashEncontrado, double chanceAceitacao){
    if(sLinha.getFAvaliacao() < foEstrela - 1e-9) {
        // subtrair 1e-9 evita aceitar soluções somente por causa de erros de arredondamento 
        return true; // aceita uma solução melhor
    }
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

    uint64_t hash = 0;

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

double busca_tabu(Instancia &inst, Solucao &s, int tamanhoLista, double tempoMax){
    Cronometro cron;
    Solucao sEstrela = s;
    double fo = s.getFAvaliacao();
    double foEstrela = sEstrela.getFAvaliacao();

    std::deque<tipoTabu> listaTabu;


    int melhorI, melhorMultiplicador;
    while (cron.segundosDecorridos() < tempoMax){
        fo = vizinhoMelhorTabu(inst, s, melhorI, melhorMultiplicador, foEstrela, listaTabu);
        
        if(melhorMultiplicador == 0){
            listaTabu.clear(); 
            continue; // Reset da lista tabu quando não encontra vizinho válido
        }
        
        atualizaListaTabu(listaTabu, tamanhoLista, s, melhorI, melhorMultiplicador);
        s.movimento(melhorI, melhorMultiplicador);

        if(fo < foEstrela){
            foEstrela = fo;
            sEstrela = s;
        }

    }

    s = sEstrela;

    return s.funcaoAvaliacao();    
}

