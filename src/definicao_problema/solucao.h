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
    double penalidade1;
    double penalidade2;
    double penalidade3;

    double Fa;
    double Fb; // constante de normalização
    double alpha; // peso para o custo de energia
    double beta; // peso para o número de manutenções

    double calculaFa(); // Limite para o custo de energia
    double calculaFb(); // limite para o número de switches de bomba

    double compoeFA(double energia, int nInterrupcoes, double excedente, double falta, double variacao);

    double calculaCusto(int &nInterrupcoes);

    void calculaNivel(TipoVetorSolucao &vetorS, double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado);
    double calculaDeltaMovimento(int i, int multiplicador, int &novo_numInterrupcoes, double &novo_custoEnergia);
    double calculaEnergia(int &nInterrupcoes);
    double calculaLambda(int nInterrupcoes);


    static void setQUANT_MIN(int n) {quant_min = n;}
    double calculaPenalidadeDura();
    double calculaPenalidade3();
  public:
    Solucao(Instancia &_inst, int _quant_min = -1, double _penalidade1 = -1, double _penalidade2 = -1, double _penalidade3 = -1, double _alpha = 0.8):
    inst(_inst), penalidade1(_penalidade1), penalidade2(_penalidade2){
      
      alpha = (_alpha <= 1 && _alpha >= 0) ? _alpha : 0.8;
      beta = 1 - alpha;
      Fa = calculaFa();
      Fb = calculaFb();

      if(_penalidade1 < -1e-3)
        _penalidade1 = calculaPenalidadeDura();
      penalidade1 = _penalidade1;

      if(_penalidade2 < -1e-3)
        _penalidade2 = calculaPenalidadeDura();
      penalidade2 = _penalidade2;

      if(_penalidade3 < -1e-3)
        // _penalidade3 = calculaPenalidadeDura();
        _penalidade3 = calculaPenalidade3();
      penalidade3 = _penalidade3;

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
    double getPenalidade1() const { return penalidade1; }
    double getPenalidade2() const { return penalidade2; }
    double getPenalidade3() const { return penalidade3; }
    double getAlpha() const { return alpha; }
    double getBeta() const { return beta; }
    double getF_a() const { return Fa; }
    double getF_b() const { return Fb; }
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

    double calculaDeltaMovimento(int i, int multiplicador);

    void movimento(int i, int multiplicador);
    void movimentoLigarBomba(int i);
    void movimentoDesligarBomba(int i);

    bool alteraVetorSolucao(TipoVetorSolucao novaSolucao);
    void zerarVetorSolucao();

    friend std::ostream& operator<<(std::ostream& os, const Solucao &s);
    Solucao& operator=(const Solucao& s2);

};





#endif