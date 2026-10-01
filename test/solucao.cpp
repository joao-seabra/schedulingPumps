#include "solucao.h"

double calculaLambda(int nInterrupcoes);
double calculaCustoEnergiaInstantaneo(const Bomba &bomba, double tarifa);


double Solucao::calculaCusto(int &nInterrupcoes){
  double custoEnergia = 0;

  nInterrupcoes = 0;

  for(int i = 0; i < 18; i++){ //percorre o vetor em cada hora de não pico
    for(int j = 0; j < inst.nBombas; j++){
      if(vetorSolucao.at((i + (j * 24)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo(inst.bombas.at(j), inst.tarifaNormal);
      }
      if(vetorSolucao.at((i + (j * 24))) != vetorSolucao.at((i + 1 + (j * 24)))){
        //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
        nInterrupcoes++;
      }
    }
  }

  for(int i = 18; i < 21; i++){ //percorre o vetor em cada hora de pico
    for(int j = 0; j < inst.nBombas; j++){
      if(vetorSolucao.at((i + (j * 24)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo(inst.bombas.at(j), inst.tarifaPico);
      }
      if(vetorSolucao.at((i + (j * 24))) != vetorSolucao.at((i + 1 + (j * 24)))){
        //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
        nInterrupcoes++;
      }
    }
  }

  for(int i = 21; i < 24; i++){
    for(int j = 0; j < inst.nBombas; j++){
      if(vetorSolucao.at((i + (j * 24)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo(inst.bombas.at(j), inst.tarifaNormal);
      }
      if(i < 23){
        if(vetorSolucao.at((i + (j * 24))) != vetorSolucao.at((i + 1 + (j * 24)))){
        //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
        nInterrupcoes++;
        }
      }
      
    }
  }


  return custoEnergia / calculaLambda(nInterrupcoes);

}

double Solucao::calculaCusto(){
  int tempInterrupcoes = 0;
  double result = calculaCusto(tempInterrupcoes);
  return result;
}


double calculaCustoEnergiaInstantaneo(const Bomba &bomba, double tarifa){
  double vazao = bomba.getFluxo() / 3600.0; // m3/h para m3/s
  double potenciaW = PESO_ESPECIFICO_AGUA * vazao * bomba.getAlturaDeTopo() / bomba.getEficiencia();
  double potenciakW = potenciaW / 1000.0;
  return potenciakW * tarifa;
}

double calculaLambda(int nInterrupcoes){
  if (nInterrupcoes >= 36) return 0.1;
  return (double) 1 - 0.00036 * (nInterrupcoes * nInterrupcoes) - 0.0122 * nInterrupcoes;
}

double Solucao::funcaoAvaliacao(){
  double custo = calculaCusto(nInterrupcoes);
  calculaNivel(excedente, falta);

  fAvaliacao = custo + penalidade1 * excedente + penalidade2 * falta;
  return fAvaliacao;
}

void Solucao::calculaNivel(double &excedente, double &falta){
  /*Calcula qual foi o excesso e qual foi a falta do 
    reservatorio com aquela solucao hora a hora*/
  double nivel = inst.reservatorio.getVolumeInicial();
  double excessoHora, faltaHora;
  double totalExcedente = 0, totalFalta = 0;

  for(int i = 0; i < 24; i++){
    nivel -= inst.consumo.at(i);
    for(int j = 0; j < inst.nBombas; j++){
      if(vetorSolucao.at(i + (j * 24))){
        nivel += inst.bombas.at(j).getFluxo();
      }
    }

    //calcula quao abaixo do nível o reservatorio esta naquele instante
    faltaHora = inst.reservatorio.getLimInferior() - nivel;
    if(faltaHora > 0){
      totalFalta += faltaHora;
    }

    //calcula quao acima do nível o reservatorio esta naquele instante
    excessoHora = nivel - inst.reservatorio.getVolumeMax();
    // excessoHora = nivel - inst.reservatorio.getLimSuperior();
    if(excessoHora > 0){
      totalExcedente += excessoHora;
    }

  }

  excedente = totalExcedente;
  falta = totalFalta;

}

void Solucao::movimentoInversao(int i){
  vetorSolucao[i] = !vetorSolucao[i];
  funcaoAvaliacao();
}

void Solucao::movimentoTroca(int i, int j){
  if(i != j && vetorSolucao[i] != vetorSolucao[j]){
    vetorSolucao[i] = !vetorSolucao[i];
    vetorSolucao[j] = !vetorSolucao[j];
    funcaoAvaliacao();
  }
}

double Solucao::calculaDeltaInversao(int i){
    double foAntigo = fAvaliacao;

    vetorSolucao[i] = !vetorSolucao[i];
    double foNovo = funcaoAvaliacao();

    // desfaz o movimento
    vetorSolucao[i] = !vetorSolucao[i]; 
    funcaoAvaliacao();

    return foNovo - foAntigo;
}

double Solucao::calculaDeltaTroca(int i, int j){
  if(i == j) return 0;

  double foAntigo = fAvaliacao;

  vetorSolucao[i] = !vetorSolucao[i];
  vetorSolucao[j] = !vetorSolucao[j];
  double foNovo = funcaoAvaliacao();

  //desfaz o movimento
  vetorSolucao[i] = !vetorSolucao[i];
  vetorSolucao[j] = !vetorSolucao[j];
  funcaoAvaliacao();

  return foNovo - foAntigo;
}


bool Solucao::alteraVetorSolucao(std::vector<bool> novaSolucao){
  if(novaSolucao.size() != vetorSolucao.size()){
    return false;
  }

  vetorSolucao = novaSolucao;
  funcaoAvaliacao();

  return true;
}

void Solucao::copia(const Solucao &s){
  inst = s.inst;
  alteraVetorSolucao(s.vetorSolucao);
  fAvaliacao = s.fAvaliacao;
  falta = s.falta;
  excedente = s.excedente;
  nInterrupcoes = s.nInterrupcoes;
  penalidade1 = s.penalidade1;
  penalidade2 = s.penalidade2;

}

std::ostream& operator<<(std::ostream& os, const Solucao &s){
  os << "Vetor Solução:" << std::endl;

  os << std::left;
  for(int i = 0; i < s.inst.nBombas; i++){
    for(int j = 0; j < 24; j++){
      os << std::setw(2) << j << " ";
    }

  }
  os << std::endl;

  os << std::left;
  for(bool b : s.vetorSolucao){
    os << std::setw(2) << b << " ";
  }

  os << std::endl << "\nFunção objetivo: " << s.fAvaliacao << "\nNúmero de interrupções: " << s.nInterrupcoes;

  return os;
}
