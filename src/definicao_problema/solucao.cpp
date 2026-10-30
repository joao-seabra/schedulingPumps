#include "solucao.h"

#include <iomanip>

#include "instancia.h"
#include "../utilitarios/cronometro.h"
#include <cmath>

int Solucao::quant_min = 60;

#define INTERRUPCOES_LACUNA 2 //Quantas interrupções devem ser consideradas quando se tem uma lacuna no vetor Solucao

double calculaCustoEnergiaInstantaneo(const Bomba &bomba, double tarifa);

bool horaComLacuna(const TipoVetorSolucao &v, int i, int nHoras);
int interrupcoesAfetadas(const TipoVetorSolucao &v, int i, int nHoras);

bool horaComLacuna(const TipoVetorSolucao &v, int i, int nHoras){
  /*Confere a existência de uma lacuna*/
  // Hora parcial (0 < minutos < 60) com as duas horas vizinhas ligadas. Os minutos só encostam
  // em uma das vizinhas, então sobra uma lacuna.
  int hora = i % nHoras;
  if(hora == 0 || hora + 1 >= nHoras) 
    return false;
  if(v.at(i) == 0 || v.at(i) >= 60) 
    return false;
  if(v.at(i - 1) == 0 || v.at(i + 1) == 0) 
    return false;

  return true;
}

int interrupcoesAfetadas(const TipoVetorSolucao &v, int i, int nHoras){
  /*Calcula as interrupções que dependem da posição i*/
  int hora = i % nHoras;
  int total = 0;
  if(hora > 0){ //confere o horário anterior
    if((bool) v.at(i) != (bool) v.at(i - 1)){ //se x estiver ligada e y desligada
      total++;
    }
    if(horaComLacuna(v, i - 1, nHoras)){ //se i-1 tiver uma lacuna
      total += INTERRUPCOES_LACUNA; //somar 2 interrupções
    }
  }
  if(hora + 1 < nHoras){ //confere o próximo horário
    if((bool) v.at(i) != (bool) v.at(i + 1)){ //se x estiver ligada e y desligada
      total++;
    }
    if(horaComLacuna(v, i + 1, nHoras)){ //se i+1 tiver uma lacuna
      total += INTERRUPCOES_LACUNA; //somar 2 interrupções
    }
  }
  if(horaComLacuna(v, i, nHoras)){ //se o próprio i for uma lacuna
    total += INTERRUPCOES_LACUNA; //somar 2 interrupções
  }

  return total;
}

double Solucao::calculaEnergia(int &nInterrupcoes){
  double custoEnergia = 0, tarifa = 0;;

  nInterrupcoes = 0;

  for(int i = 0; i < inst.nHoras; i++){ //percorre o vetor em cada hora
    tarifa = inst.tarifaPorHora[i];
    for(int j = 0; j < inst.nBombas; j++){
      custoEnergia += calculaCustoEnergiaInstantaneo(inst.bombas.at(j), tarifa) * vetorSolucao.at((i + (j * inst.nHoras)));
      if(i + 1 < inst.nHoras){
        if((bool) vetorSolucao.at((i + (j * inst.nHoras))) != (bool) vetorSolucao.at((i + 1 + (j * inst.nHoras)))){
          //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
          nInterrupcoes++;
        }
      }
      if(horaComLacuna(vetorSolucao, i + (j * inst.nHoras), inst.nHoras)){
        nInterrupcoes += INTERRUPCOES_LACUNA;
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

void Solucao::calculaNivel(TipoVetorSolucao &vetorS, double &excedenteEncontrado, double &faltaEncontrada, double &variacaoDoNivelEncontrado){
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


void Solucao::movimento(int i, int multiplicador){
  /*Movimento: O multiplicador será aplicado a um QUANT_MIN fixo e somado à posição i do vetor*/
  int p = Solucao::getQUANT_MIN(); //Definindo p para o movimento (Nesse caso 5 minutos)
  p = p * multiplicador;
  int result = vetorSolucao[i] + p;
  /*
  if(result < 0 || result > 60)
    return false;
  */
  if(result < 0) result = 0;
  else if(result > 60) result = 60;
  
  fAvaliacao += calculaDeltaMovimento(i, multiplicador, nInterrupcoes, custoEnergia);
  vetorSolucao[i] = result;

}

void Solucao::movimentoLigarBomba(int i){
  movimento(i, 60);
}

void Solucao::movimentoDesligarBomba(int i){
  movimento(i, -60);
}

double Solucao::calculaDeltaMovimento(int i, int multiplicador){
  int temp1 = 0;
  double temp2 = 0;
  return calculaDeltaMovimento(i, multiplicador, temp1, temp2);
}

double Solucao::calculaDeltaMovimento(int i, int multiplicador, int &novo_numInterrupcoes, double &novo_custoEnergia){
  int bomba = i / inst.nHoras;
  int hora = i % inst.nHoras;
  int result = multiplicador * Solucao::getQUANT_MIN();
  result = (vetorSolucao.at(i) + result > 60) ?  ( 60 - vetorSolucao.at(i) ) : result;
  result = (vetorSolucao.at(i) + result < 0) ?  ( 0 - vetorSolucao.at(i) ) : result;
  
  TipoVetorSolucao temp; //Usado para calcular as penalizações por nível e o número de interrupções  
  temp = vetorSolucao;
  temp.at(i) = temp.at(i) + result;

  double custoEnergiaInstantaneo = calculaCustoEnergiaInstantaneo(inst.bombas.at(bomba), inst.tarifaPorHora.at(hora));
  novo_custoEnergia = custoEnergia + (custoEnergiaInstantaneo * result);
  
  
  //calculo do número de interrupções:
  int interrupcoesAntes = interrupcoesAfetadas(vetorSolucao, i, inst.nHoras); //interrupções ao redor de i no vetor original
  int interrupcoesDepois = interrupcoesAfetadas(temp, i, inst.nHoras); //interrupções ao redor de i no novo vetor
  novo_numInterrupcoes = nInterrupcoes - interrupcoesAntes + interrupcoesDepois;


  //calculo das penalizações
  double deltaExcedente = 0, deltaFalta = 0, deltaNivel = 0;
  calculaNivel(temp, deltaExcedente, deltaFalta, deltaNivel);

  double novaFA = novo_custoEnergia / calculaLambda(novo_numInterrupcoes) 
      + deltaExcedente * penalidade1 + deltaFalta * penalidade2 + deltaNivel * penalidade3;

  return novaFA - fAvaliacao;

}



bool Solucao::alteraVetorSolucao(TipoVetorSolucao novaSolucao){
  if(novaSolucao.size() != vetorSolucao.size()){
    return false;
  }

  vetorSolucao = novaSolucao;
  funcaoAvaliacao();

  return true;
}

void Solucao::zerarVetorSolucao(){
  vetorSolucao.assign(inst.nHoras * inst.nBombas, 0);
  funcaoAvaliacao();
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
    os << "Bomba b" << i << ":\n";
    for(int j = 0; j < s.inst.nHoras; j++){
      os << std::setw(2) << j << " ";
    }
    os << std::endl;
    os << std::left;
    for(int j = 0; j < s.inst.nHoras; j++){
      os << std::setw(2) << s.vetorSolucao.at((j + (i * s.inst.nHoras))) << " ";
    }
    os << std::endl << std::endl;
  }


  os << std::endl << "\nFunção objetivo: " << s.fAvaliacao << "\nNúmero de interrupções: " << s.nInterrupcoes;

  return os;
}
