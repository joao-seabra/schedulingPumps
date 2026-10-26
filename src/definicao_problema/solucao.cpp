#include "solucao.h"

#include <iomanip>

#include "instancia.h"
#include "../utilitarios/cronometro.h"
#include <cmath>

  double calculaCustoEnergiaInstantaneo(const Bomba &bomba, double tarifa);

double Solucao::calculaEnergia(int &nInterrupcoes){
  double custoEnergia = 0, tarifa = 0;;

  nInterrupcoes = 0;

  for(int i = 0; i < inst.nHoras; i++){ //percorre o vetor em cada hora
    tarifa = inst.tarifaPorHora[i];
    for(int j = 0; j < inst.nBombas; j++){
      if(vetorSolucao.at((i + (j * inst.nHoras)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo(inst.bombas.at(j), tarifa);
      }
      if(i + 1 < inst.nHoras){
        if(vetorSolucao.at((i + (j * inst.nHoras))) != vetorSolucao.at((i + 1 + (j * inst.nHoras)))){
          //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
          nInterrupcoes++;
        }
      }
    }
  }

  return custoEnergia;

}

double Solucao::calculaCusto(int &nInterrupcoes){
  double energia = calculaEnergia(nInterrupcoes);
  return energia  / calculaLambda(nInterrupcoes);
}

double Solucao::calculaCusto(){
  int tempInterrupcoes = 0;
  double result = calculaCusto(tempInterrupcoes);
  return result;
}


double calculaCustoEnergiaInstantaneo(const Bomba &bomba, double tarifa){
  /*Custo de energia por minuto que uma bomba fica ligada*/
  double vazao = bomba.getFluxo() / 60.0; // m3/min para m3/s
  double potenciaW = PESO_ESPECIFICO_AGUA * vazao * bomba.getAlturaDeTopo() / bomba.getEficiencia();
  double potenciakW = potenciaW / 1000.0;
  return potenciakW * tarifa;

  // return bomba.getPotencia() * tarifa;
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

void Solucao::calculaNivel(TipoSolucao &vetorS, double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado){
  double excessoHora, faltaHora, variacaoDia;
  double totalExcedente = 0, totalFalta = 0, totalVariacao = 0;
  double nivel;
  for(Reservatorio &r : inst.reservatorios){
    nivel = r.getVolumeInicial();
    for(int i = 0; i < inst.nHoras; i++){
      nivel -= r.getConsumo(i);
      for(int bombaAcoplada : r.getBombasAcopladas()){
        nivel += inst.bombas.at(bombaAcoplada).getFluxo() * vetorS.at(i + (bombaAcoplada * inst.nHoras));
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

void Solucao::movimento(int i, int multiplicador){
  /*Movimento: O multiplicador será aplicado a um QUANT_MIN fixo e somado à posição i do vetor*/
  int p = QUANT_MIN; //Definindo p para o movimento (Nesse caso 5 minutos)
  p = p * multiplicador;
  int result = vetorSolucao[i] + p;
  /*
  if(result < 0 || result > 60)
    return false;
  */
  if(result < 0) result = 0;
  else if(result > 60) result = 60;
  
  vetorSolucao[i] = result;
  fAvaliacao += calculaDeltaMovimento(i, multiplicador, nInterrupcoes, custoEnergia);

}

void Solucao::movimentoLigarBomba(int i){
  movimento(i, 60);
}

void Solucao::movimentoDesligarBomba(int i){
  movimento(i, -60);
}

double Solucao::calculaDeltaMovimento(int i, int multiplicador, int &novo_numInterrupcoes, double &novo_custoEnergia){
  int bomba = i / inst.nHoras;
  int hora = i % inst.nHoras;
  int result = multiplicador * QUANT_MIN;

  result = (vetorSolucao.at(i) + result > 60) ?  ( 60 - vetorSolucao.at(i) ) : result;
  result = (vetorSolucao.at(i) + result < 0) ?  ( 0 - vetorSolucao.at(i) ) : result;

  double custoEnergiaInstantaneo = calculaCustoEnergiaInstantaneo(inst.bombas.at(bomba), inst.tarifaPorHora.at(hora));

  novo_custoEnergia = custoEnergia + (custoEnergiaInstantaneo * result);

  novo_numInterrupcoes = nInterrupcoes;
  if(hora > 0){
    if((bool)(vetorSolucao.at(i) + result) != (bool) vetorSolucao.at(i - 1)){
      //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
      novo_numInterrupcoes++;
    }
    else{
      novo_numInterrupcoes--;
    }
  }
  if(hora + 1 < inst.nHoras){
    if((bool)(vetorSolucao.at(i) + result) != (bool) vetorSolucao.at(i + 1)){
      //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
      novo_numInterrupcoes++;
    }
    else{
      novo_numInterrupcoes--;
    }
  }

  //calculo das penalizações
  double deltaExcedente = 0, deltaFalta = 0, deltaNivel = 0;
  TipoSolucao temp = vetorSolucao;
  temp.at(i) = temp.at(i) + result;
  calculaNivel(temp, deltaExcedente, deltaFalta, deltaNivel);

  double novaFA = novo_custoEnergia / calculaLambda(novo_numInterrupcoes) 
      + deltaExcedente * penalidade1 + deltaFalta * penalidade2 + deltaNivel * penalidade3;

  return novaFA - fAvaliacao;

}

double Solucao::calculaDeltaInversao(int i, int &novo_numInterrupcoes, double &novo_custoEnergia){
  int bomba = i / inst.nHoras;
  int hora = i % inst.nHoras;
  
  double custoEnergiaInstantaneo = calculaCustoEnergiaInstantaneo(inst.bombas.at(bomba), inst.tarifaPorHora.at(hora));

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
  if(hora + 1 < inst.nHoras){
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
  TipoSolucao temp = vetorSolucao;
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



bool Solucao::alteraVetorSolucao(TipoSolucao novaSolucao){
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
    for(int j = 0; j < s.inst.nHoras; j++){
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
