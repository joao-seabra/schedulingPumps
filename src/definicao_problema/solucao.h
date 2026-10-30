#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <vector>
#include <iostream>

#include "instancia.h"

#define PESO_ESPECIFICO_AGUA 9810 //N/M3

typedef std::vector<unsigned short> TipoVetorSolucao;
typedef unsigned short TipoSolucao;


class Solucao{
  private:
    Instancia &inst;
    TipoVetorSolucao vetorSolucao;
    double fAvaliacao;
    double custoEnergia;
    int nInterrupcoes;

    static int quant_min;
    static double penalidade1;
    static double penalidade2;
    static double penalidade3;


    double calculaCusto(int &nInterrupcoes);

    void calculaNivel(TipoVetorSolucao &vetorS, double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado);
    double calculaDeltaInversao(int i, int &novo_numInterrupcoes, double &novo_custoEnergia);
    double calculaDeltaMovimento(int i, int multiplicador, int &novo_numInterrupcoes, double &novo_custoEnergia);
    double calculaEnergia(int &nInterrupcoes);
    double calculaLambda(int nInterrupcoes);


    static void setQUANT_MIN(int n) {quant_min = n;}
    static void setPenalidade1(double n) {penalidade1 = n;}
    static void setPenalidade2(double n) {penalidade2 = n;}
    static void setPenalidade3(double n) {penalidade3 = n;}
    double calculaPenalidade3();
  public:
    Solucao(Instancia &_inst, double _penalidade1 = 1000.0, double _penalidade2 = 1000.0, double _penalidade3 = -1, int _quant_min = 60):
    inst(_inst){
      setPenalidade1(_penalidade1);
      setPenalidade2(_penalidade2);

      if(_penalidade3 < -1e3)
        setPenalidade3(calculaPenalidade3());
      else 
        setPenalidade3(_penalidade3);

      if(_quant_min > 60 || _quant_min < 0){
        _quant_min = 60;
      }
      setQUANT_MIN(_quant_min);

      nInterrupcoes = 0;
      zerarVetorSolucao(); //inicializa vetor solução com 0
      funcaoAvaliacao();
    }

    virtual ~Solucao(){}

    
    // GETTERS
    const Instancia& getInst() const { return inst; }
    const TipoVetorSolucao& getVetorSolucao() const { return vetorSolucao; }
    double getFAvaliacao() const { return fAvaliacao; }
    double getCustoEnergia() const { return custoEnergia; }
    int getNInterrupcoes() const { return nInterrupcoes; }
    static double getPenalidade1() { return penalidade1; }
    static double getPenalidade2() { return penalidade2; }
    static double getPenalidade3() { return penalidade3; }
    static double getQUANT_MIN() { return quant_min; }

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

    bool alteraVetorSolucao(TipoVetorSolucao novaSolucao);
    void zerarVetorSolucao();

    friend std::ostream& operator<<(std::ostream& os, const Solucao &s);
    Solucao& operator=(const Solucao& s2);

};





#endif