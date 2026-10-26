#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <vector>
#include <iostream>

#include "instancia.h"

#define PESO_ESPECIFICO_AGUA 9810 //N/M3
#define QUANT_MIN 5 //quantidade mínima de minutos que uma bomba fica ligada

typedef std::vector<unsigned short> TipoSolucao;

class Solucao{
  private:
    Instancia &inst;
    TipoSolucao vetorSolucao;
    double fAvaliacao;
    double custoEnergia;
    int nInterrupcoes;
    double penalidade1;
    double penalidade2;
    double penalidade3;


    double calculaCusto(int &nInterrupcoes);

    void calculaNivel(TipoSolucao &vetorS, double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado);
    double calculaDeltaInversao(int i, int &novo_numInterrupcoes, double &novo_custoEnergia);
    double calculaDeltaMovimento(int i, int multiplicador, int &novo_numInterrupcoes, double &novo_custoEnergia);
    double calculaEnergia(int &nInterrupcoes);
    double calculaLambda(int nInterrupcoes);
    double calculaPenalidade3();
  public:
    Solucao(Instancia &_inst, double _penalidade1 = 1000.0, double _penalidade2 = 1000.0, double _penalidade3 = 0.0):
    inst(_inst), penalidade1(_penalidade1), penalidade2(_penalidade2), penalidade3(_penalidade3){
      penalidade3 += calculaPenalidade3();
      nInterrupcoes = 0;
      zerarVetorSolucao(); //inicializa vetor solução com 0
      funcaoAvaliacao();
    }

    virtual ~Solucao(){}

    
    // GETTERS
    const Instancia& getInst() const { return inst; }
    const TipoSolucao& getVetorSolucao() const { return vetorSolucao; }
    double getFAvaliacao() const { return fAvaliacao; }
    double getCustoEnergia() const { return custoEnergia; }
    int getNInterrupcoes() const { return nInterrupcoes; }
    double getPenalidade1() const { return penalidade1; }
    double getPenalidade2() const { return penalidade2; }
    double getPenalidade3() const { return penalidade3; }

    /*
    Métodos para calcular custo, capacidade do reservatorio/viabilidade de dada solução, 
    e métodos para explorar a vizinhança
    */

    double calculaCusto();
    double funcaoAvaliacao();

    void calculaNivel(double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado);
    /*Calcula qual foi o excesso e qual foi a falta do 
    reservatorio com aquela solucao hora a hora*/

    double calculaDeltaInversao(int i);
    double calculaDeltaMovimento(int i, int multiplicador);

    void movimentoInversao(int i);
    void movimento(int i, int multiplicador);
    void movimentoLigarBomba(int i);
    void movimentoDesligarBomba(int i);

    bool alteraVetorSolucao(TipoSolucao novaSolucao);
    void zerarVetorSolucao();

    friend std::ostream& operator<<(std::ostream& os, const Solucao &s);
    Solucao& operator=(const Solucao& s2);

};





#endif