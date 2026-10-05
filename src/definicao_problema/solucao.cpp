#include "solucao.h"

#include <iomanip>

#include "instancia.h"
#include "../utilitarios/cronometro.h"
#include <cmath>

  double calculaCustoEnergiaInstantaneo(const Bomba &bomba, double tarifa);

double Solucao::calculaEnergia(int &nInterrupcoes){
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

  return custoEnergia;

}

double Solucao::calculaCusto(int &nInterrupcoes){
  return ( calculaEnergia(nInterrupcoes) ) / calculaLambda(nInterrupcoes);
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

double Solucao::calculaLambda(int nInterrupcoes){
  if (nInterrupcoes >= 36) return 0.1;
  return (double) 1 - 0.00036 * (nInterrupcoes * nInterrupcoes) - 0.0122 * nInterrupcoes;
}

double Solucao::funcaoAvaliacao(){
  custoEnergia = calculaEnergia(nInterrupcoes);
  double excedente, falta, variacaoNivel;
  calculaNivel(excedente, falta, variacaoNivel);

  fAvaliacao = (custoEnergia / calculaLambda(nInterrupcoes)) + 
  penalidade1 * excedente + penalidade2 * falta + penalidade3 * variacaoNivel;

  return fAvaliacao;
}

void Solucao::calculaNivel(double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado){
  calculaNivel(vetorSolucao, excedenteEncontrado, faltaEncontrada, variacaoDoNivelEncontrado);
}

void Solucao::calculaNivel(std::vector<bool> vetorS, double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado){
  double excessoHora, faltaHora, variacaoDia;
  double totalExcedente = 0, totalFalta = 0, totalVariacao = 0;
  double nivel;
  for(Reservatorio &r : inst.reservatorios){
    nivel = r.getVolumeInicial();
    for(int i = 0; i < 24; i++){
      nivel -= r.getConsumo(i);
      for(int bombaAcoplada : r.getBombasAcopladas()){
        if(vetorS.at(i + (bombaAcoplada * 24))){
          nivel += inst.bombas.at(bombaAcoplada).getFluxo();
        }
      }
    
      faltaHora = r.getLimInferior() - nivel;
      if(faltaHora > 0){
        totalFalta += faltaHora;
      }
  
      excessoHora = nivel - r.getVolumeMax();
      if(excessoHora > 0){
        totalExcedente += excessoHora;
      }
      
      
    }
    // totalVariacao += std::abs(r.getVolumeInicial() - nivel);
    variacaoDia = r.getVolumeInicial() - nivel;
    if(variacaoDia > 0){
      totalVariacao += variacaoDia;
    } 
    
  }

  variacaoDoNivelEncontrado = totalVariacao;
  excedenteEncontrado = totalExcedente;
  faltaEncontrada = totalFalta;
}

void Solucao::movimentoInversao(int i){
  fAvaliacao += calculaDeltaInversao(i, nInterrupcoes, custoEnergia);
  vetorSolucao[i] = !vetorSolucao[i];
}

void Solucao::movimentoTroca(int i, int j){
  if(i != j && vetorSolucao[i] != vetorSolucao[j]){
    movimentoInversao(i);
    movimentoInversao(j);
  }
}

double Solucao::calculaDeltaInversao(int i, int &novo_numInterrupcoes, double &novo_custoEnergia){
  int bomba = i / 24;
  int hora = i % 24;
  
  double custoEnergiaInstantaneo = (hora < 18 || hora >= 21) ? 
  calculaCustoEnergiaInstantaneo(inst.bombas.at(bomba), inst.tarifaNormal) : calculaCustoEnergiaInstantaneo(inst.bombas.at(bomba), inst.tarifaPico);
  
  if(!vetorSolucao.at(i)){
    novo_custoEnergia = custoEnergia + custoEnergiaInstantaneo;
  }
  else{
    novo_custoEnergia = custoEnergia - custoEnergiaInstantaneo;
  }
  

  novo_numInterrupcoes = nInterrupcoes;
  if(hora > 0){
    if(!vetorSolucao.at(i) != vetorSolucao.at(i - 1)){
      //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
      novo_numInterrupcoes++;
    }
    else{
      novo_numInterrupcoes--;
    }
  }
  if(hora < 23){
    if(!vetorSolucao.at(i) != vetorSolucao.at(i + 1)){
      //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
      novo_numInterrupcoes++;
    }
    else{
      novo_numInterrupcoes--;
    }
  }


  //calculo das penalizações
  double deltaExcedente = 0, deltaFalta = 0, deltaNivel = 0;
  std::vector<bool> temp = vetorSolucao;
  temp.at(i) = !temp.at(i);
  calculaNivel(temp, deltaExcedente, deltaFalta, deltaNivel);

  double novaFA = novo_custoEnergia / calculaLambda(novo_numInterrupcoes) 
      + deltaExcedente * penalidade1 + deltaFalta * penalidade2 + deltaNivel * penalidade3;

  return novaFA - fAvaliacao;
}

double Solucao::calculaDeltaInversao(int i){
  int temp1 = 0;
  double temp2 = 0;
  return calculaDeltaInversao(i, temp1, temp2);
}

double Solucao::calculaDeltaTroca(int i, int j){
  if(i != j && vetorSolucao.at(i) != vetorSolucao.at(j)){
    Solucao temp(*this);
    temp.movimentoInversao(i);
    temp.movimentoInversao(j);

    return temp.fAvaliacao - fAvaliacao;
  }
  return 0;
}


bool Solucao::alteraVetorSolucao(std::vector<bool> novaSolucao){
  if(novaSolucao.size() != vetorSolucao.size()){
    return false;
  }

  vetorSolucao = novaSolucao;
  funcaoAvaliacao();

  return true;
}

double custoPorM3(const Bomba &b, double tarifa){
  return calculaCustoEnergiaInstantaneo(b, tarifa) / b.getFluxo();
}

double Solucao::calculaPenalidade3(){
  double media = 0;
  for(const Bomba &b:inst.bombas){
    media += custoPorM3(b, inst.tarifaNormal);
  }
  media /= inst.nBombas;
  media = std::round(media * 1000) / 1000.0;
  return media;
}

Solucao& Solucao::operator=(const Solucao& s2) {
    if (this == &s2) {
        return *this;
    }

    this->vetorSolucao = s2.vetorSolucao;
    this->fAvaliacao = s2.fAvaliacao;
    this->custoEnergia = s2.custoEnergia;
    this->nInterrupcoes = s2.nInterrupcoes;
    this->penalidade1 = s2.penalidade1;
    this->penalidade2 = s2.penalidade2;
    this->penalidade3 = s2.penalidade3;

    return *this;
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
