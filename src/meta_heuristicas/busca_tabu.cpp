#include "busca_tabu.h"

#include "../definicao_problema/instancia.h"
#include "../definicao_problema/solucao.h"
#include "../refinamento/descida.h"
#include "../utilitarios/aleatorio.h"
#include "../utilitarios/cronometro.h"

#include <deque>
#include <vector>
#include <functional>
#include <algorithm>

typedef struct tipoTabu{
    int duracaoTabu;
    int multiplicador;
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


bool estaNaListaTabu(int iter, int posicao, int multiplicador,
                      const std::vector<tipoTabu> &listaTabu)
{
    if(listaTabu.at(posicao).duracaoTabu >= iter){
        if(listaTabu.at(posicao).multiplicador == multiplicador)
            return true;
    }
    return false;
}

bool criterioAspiracao(const Solucao &sLinha, double foEstrela, const std::deque<uint64_t> listaHash){
    if(sLinha.getFAvaliacao() < foEstrela - 1e-9) {
        // subtrair 1e-9 evita aceitar soluções somente por causa de erros de arredondamento 
        return true; // aceita uma solução melhor
    }
    if(std::find(listaHash.begin(), listaHash.end(), hashTabu(sLinha)) == listaHash.end()){
        // procura hash na lista, se não estiver a solução sLinha é aceita
        return true;
    }
    return false;
}

void atualizaListaTabu(std::vector<tipoTabu> &listaTabu, std::deque<uint64_t> &hash, size_t hashMax, int duracaoTabu, 
                        Solucao &s, int posicao, int multiplicador)
{
    if(hash.size() >= hashMax){
        hash.pop_front();
    }

    hash.push_back(hashTabu(s));
    // proíbe o retorno para uma solução já vista
    listaTabu.at(posicao) = tipoTabu{duracaoTabu, -multiplicador};

}

double vizinhoMelhorTabu(Instancia &inst, Solucao &s, int &melhorI, int &melhorMultiplicador, 
                        double foEstrela, int iter, std::vector<tipoTabu> &listaTabu, 
                        std::deque<uint64_t> listaHash)
{
    int n = s.getVetorSolucao().size();
    melhorI = 0;
    melhorMultiplicador = 0;
    double melhorFo = std::numeric_limits<double>::max();
    
    Solucao sLinha = s;
    double foLinha = s.getFAvaliacao();

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

            if((!estaNaListaTabu(iter, i, multiplicador, listaTabu)) || criterioAspiracao(sLinha, foEstrela, listaHash)){
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

    std::vector<tipoTabu> listaTabu(s.getVetorSolucao().size());
    std::deque<uint64_t> hashsVizitados(tamanhoLista);


    int iter = 0;
    int duracaoTabu;
    int melhorI, melhorMultiplicador;
    while (cron.segundosDecorridos() < tempoMax){
        fo = vizinhoMelhorTabu(inst, s, melhorI, melhorMultiplicador, foEstrela, iter, listaTabu, hashsVizitados);
        
        if(melhorMultiplicador == 0){
            listaTabu.clear(); 
            continue; // Reset da lista tabu quando não encontra vizinho válido
        }
        duracaoTabu = inteiroAleatorio((int)(0.8 * tamanhoLista), (int)(1.2 * tamanhoLista));
        atualizaListaTabu(listaTabu, hashsVizitados, tamanhoLista, iter + duracaoTabu, s, melhorI, melhorMultiplicador);
        s.movimento(melhorI, melhorMultiplicador);

        if(fo < foEstrela){
            foEstrela = fo;
            sEstrela = s;
        }
        iter++;
    }

    s = sEstrela;

    return s.funcaoAvaliacao();    
}

