#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <vector>
#include <iostream>

#include "instancia.h"

#define PESO_ESPECIFICO_AGUA 9810 //N/M3

class Solucao{
  private:
    Instancia &inst;
    std::vector<bool> vetorSolucao;
    double fAvaliacao;
    double custoEnergia;
    int nInterrupcoes;
    double penalidade1;
    double penalidade2;


    double calculaCusto(int &nInterrupcoes);

    void calculaNivel(std::vector<bool> vetorS, double &excedenteEncontrado, double &faltaEncontrada);
    double calculaDeltaInversao(int i, int &novo_numInterrupcoes, double &novo_custoEnergia);
    double calculaEnergia(int &nInterrupcoes);
    double calculaLambda(int nInterrupcoes);
  public:
    Solucao(Instancia &_inst, double _penalidade1 = 1000, double _penalidade2 = 1000):
    inst(_inst), penalidade1(_penalidade1), penalidade2(_penalidade2){
      nInterrupcoes = 0;
      vetorSolucao.assign(24 * inst.nBombas, false); //inicializa vetor solução 
      funcaoAvaliacao();
    }

    virtual ~Solucao(){}

    
    // GETTERS
    const Instancia& getInst() const { return inst; }
    const std::vector<bool>& getVetorSolucao() const { return vetorSolucao; }
    double getFAvaliacao() const { return fAvaliacao; }
    double getCustoEnergia() const { return custoEnergia; }
    int getNInterrupcoes() const { return nInterrupcoes; }
    double getPenalidade1() const { return penalidade1; }
    double getPenalidade2() const { return penalidade2; }

    /*
    Métodos para calcular custo, capacidade do reservatorio/viabilidade de dada solução, 
    e métodos para explorar a vizinhança
    */

    double calculaCusto();
    double funcaoAvaliacao();

    void calculaNivel(double &excedenteEncontrado, double &faltaEncontrada);
    /*Calcula qual foi o excesso e qual foi a falta do 
    reservatorio com aquela solucao hora a hora*/

    double calculaDeltaInversao(int i);
    double calculaDeltaTroca(int i, int j);

    void movimentoInversao(int i);
    void movimentoTroca(int i, int j);

    bool alteraVetorSolucao(std::vector<bool> novaSolucao);
    void copia(const Solucao &s);

    friend std::ostream& operator<<(std::ostream& os, const Solucao &s);

};





#endif