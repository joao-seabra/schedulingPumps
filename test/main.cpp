//main para testes
#include <iostream>
#include <assert.h>

#include "../src/bomba.h"
#include "../src/instancia.h"
#include "../src/reservatorio.h"
#include "../src/solucao.h"
#include "../src/cronometro.h"
#include "../src/aleatorio.h"
#include "../src/util.h"

#include "../src/construcao.h"
#include "../src/simulatedAnnealing.h"

void testeLeituraDeArquivos();
void testeFuncaoDeAvaliacao();
double calculaDeltaInversaoTest(Instancia &inst, Solucao &s, int i);
double calculaCustoEnergiaInstantaneo_teste(const Bomba &bomba, double tarifa);
int calculaNInterrupcoesTest(Instancia &inst, Solucao &s);
void calculaNivelInversaoTeste(Instancia &inst, Solucao s, int i, double &excedente, double &falta);

int main(){

  testeLeituraDeArquivos();
  testeFuncaoDeAvaliacao();

  return 0;
}

void imprimeSolucao(const Instancia &inst, Solucao &s){
  std::cout << std::fixed << std::setprecision(2);
  std::cout << s << std::endl;
  std::cout << "\nCusto de s:" << s.calculaCusto() << std::endl;
  std::cout << "\nExcedente de s:" << s.getExcedente() << std::endl;
  std::cout << "\nPenalidade por excedente de s:" << s.getPenalidade1() << std::endl;
  std::cout << "\nFalta de s:" << s.getFalta() << std::endl;
  std::cout << "\nPenalidade por falta de s:" << s.getPenalidade2() << std::endl;

  if (validaSolucao(inst, s)) std::cout << "\nSolução válida\n";
  else std::cout << "\nSoulução Inválida\n";

}

void testeLeituraDeArquivos(){
  //Lê as informações dos arquivos e imprime o conteúdo da instância
  const std::string nomeArquivoTWLS("Instancia/TWLS_INFO.txt");
  const std::string nomeArquivoConsumo("Instancia/INFO_CONSUMO.txt");

  Instancia inst = leInstancia(nomeArquivoTWLS, nomeArquivoConsumo);

  std::vector<bool> solucao;
  solucao.assign(24 * inst.nBombas, false);
  double fo = 0;


  std::cout << "\nTeste leitura de arquivos: " << std::endl;

  std::cout << "Volume máximo do reservatorio: " << inst.reservatorio.getVolumeMax() << "\nVolume atual: " 
            << inst.reservatorio.getVolumeInicial() << "\nVolume mínimo: "
            << inst.reservatorio.getLimInferior() << "\nNúmero de bombas: "
            << inst.nBombas << std::endl;

  for(int i = 0; i < inst.nBombas; i++){
    std::cout << "Bomba No" << i+1 << ":\nPotencia: " 
              << inst.bombas.at(i).getPotencia() << "\nFluxo: "
              << inst.bombas.at(i).getFluxo() << "\nAltura de topo: "
              << inst.bombas.at(i).getAlturaDeTopo() << "\nEficiencia: "
              << inst.bombas.at(i).getEficiencia() << std::endl;
  }

  std::cout << "\nDados de consumo:\n";
  for(double d : inst.consumo){
    std::cout << " Dado: " << d;
  }

  std::cout<<std::endl;
}

double calculaLambdaTest(int nInterrupcoes){
  if (nInterrupcoes >= 36) return 0.1;
  return (double) 1 - 0.00036 * (nInterrupcoes * nInterrupcoes) - 0.0122 * nInterrupcoes;
}

void testeFuncaoDeAvaliacao(){
  const double PENALIDADE1 = 1;
  const double PENALIDADE2 = 2;
  semente(1000);

  const std::string nomeArquivoTWLS("Instancia/TWLS_INFO.txt");
  const std::string nomeArquivoConsumo("Instancia/INFO_CONSUMO.txt");

  Instancia inst = leInstancia(nomeArquivoTWLS, nomeArquivoConsumo);
  Solucao s(inst, PENALIDADE1, PENALIDADE2);

  std::cout << "\nInstância:\n" << inst << std::endl;
  std::cout << "\nSolução:\n";
  imprimeSolucao(inst, s);

  int nInterrupcoes = calculaNInterrupcoesTest(inst, s);
  assert(nInterrupcoes == s.getNInterrupcoes());

  for(int i = 1; i < s.getVetorSolucao().size(); i++){
    double delta1 = calculaDeltaInversaoTest(inst, s, i);
    double delta2 = s.calculaDeltaInversao(i);
    assert(delta1 - delta2 < 1e-6);
      
  }

  solucaoBangBang(inst, s);

  std::cout << "\nSolução:\n";
  imprimeSolucao(inst, s);

  int tam = s.getVetorSolucao().size();
  const int n = 100000;
  Cronometro cron1;
  for(int i = 0; i < n; i++){
    calculaDeltaInversaoTest(inst, s, inteiroAleatorio(0, tam - 1));
  }
  double tempo_cron1 = cron1.segundosDecorridos();

  Cronometro cron2;
  for(int i = 0; i < n; i++){
    s.calculaDeltaInversao(inteiroAleatorio(0, tam - 1));
  }
  double tempo_cron2 = cron2.segundosDecorridos();

  std::cout << "\nResultado da corrida:\nCron1: " << tempo_cron1 << "s\nCron2: " << tempo_cron2 <<"s\n";
  

}

double calculaDeltaInversaoTest(Instancia &inst, Solucao &s, int i){
  double delta = 0;
  int interrupcoes = s.getNInterrupcoes();
  int bomba = i / 24;
  int hora = i % 24;

  double custoEnergiaInstantaneo = (hora < 18 || hora >= 21) ? 
  calculaCustoEnergiaInstantaneo_teste(inst.bombas.at(bomba), inst.tarifaNormal) : calculaCustoEnergiaInstantaneo_teste(inst.bombas.at(bomba), inst.tarifaPico);

  if(!s.getVetorSolucao().at(i)){
    delta += custoEnergiaInstantaneo;
  }
  else{
    delta -= custoEnergiaInstantaneo;
  }

  if(hora > 0){
    if(!s.getVetorSolucao().at(i) != s.getVetorSolucao().at(i - 1)){
      //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
      interrupcoes++;
    }
    else{
      interrupcoes--;
    }
  }
  if(hora < 23){
    if(!s.getVetorSolucao().at(i) != s.getVetorSolucao().at(i + 1)){
      //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
      interrupcoes++;
    }
    else{
      interrupcoes--;
    }
  }

  
  delta = delta/calculaLambdaTest(interrupcoes); //delta já possui o valor do custo nesse momento


  //calculo das penalizações
  double excedente = 0, falta = 0;
  // s.calculaNivel(excedente, falta);
  double nivel = inst.reservatorio.getVolumeInicial();
  double excessoHora, faltaHora;

  int pos = 0;
  for(int k = 0; k < 24; k++){
    nivel -= inst.consumo.at(k);
    for(int j = 0; j < inst.nBombas; j++){
      pos = k + (j * 24);
      if(pos != i){
        if(s.getVetorSolucao().at(pos))
          nivel += inst.bombas.at(j).getFluxo();
      }
      else{
        if(!s.getVetorSolucao().at(pos))
          nivel += inst.bombas.at(j).getFluxo();
      }
    }

    //calcula quao abaixo do nível o reservatorio esta naquele instante
    faltaHora = inst.reservatorio.getLimInferior() - nivel;
    if(faltaHora > 0){
      falta += faltaHora;
    }

    //calcula quao acima do nível o reservatorio esta naquele instante
    excessoHora = nivel - inst.reservatorio.getVolumeMax();
    // excessoHora = nivel - inst.reservatorio.getLimSuperior();
    if(excessoHora > 0){
      excedente += excessoHora;
    }

  }

  delta += s.getPenalidade1() * excedente;
  delta += s.getPenalidade2() * falta;


  return delta - s.getFAvaliacao();
}



double calculaCustoEnergiaInstantaneo_teste(const Bomba &bomba, double tarifa){
  double vazao = bomba.getFluxo() / 3600.0; // m3/h para m3/s
  double potenciaW = PESO_ESPECIFICO_AGUA * vazao * bomba.getAlturaDeTopo() / bomba.getEficiencia();
  double potenciakW = potenciaW / 1000.0;
  return potenciakW * tarifa;
}

double calculaCustoEnergiaTest(Instancia &inst, Solucao &s){
  double custoEnergia = 0;

  for(int i = 0; i < 18; i++){ //percorre o vetor em cada hora de não pico
    for(int j = 0; j < inst.nBombas; j++){
      if(s.getVetorSolucao().at((i + (j * 24)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo_teste(inst.bombas.at(j), inst.tarifaNormal);
      }
    }
  }

  for(int i = 18; i < 21; i++){ //percorre o vetor em cada hora de pico
    for(int j = 0; j < inst.nBombas; j++){
      if(s.getVetorSolucao().at((i + (j * 24)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo_teste(inst.bombas.at(j), inst.tarifaPico);
      }
    }
  }

  for(int i = 21; i < 24; i++){
    for(int j = 0; j < inst.nBombas; j++){
      if(s.getVetorSolucao().at((i + (j * 24)))){ //se a bomba j estiver ativa na hora i
        custoEnergia += calculaCustoEnergiaInstantaneo_teste(inst.bombas.at(j), inst.tarifaNormal);
      }      
    }
  }

  return custoEnergia;
}

int calculaNInterrupcoesTest(Instancia &inst, Solucao &s){
  int nInterrupcoes = 0;

  for(int i = 0; i < 23; i++){ 
    for(int j = 0; j < inst.nBombas; j++){
      if(s.getVetorSolucao().at((i + (j * 24))) != s.getVetorSolucao().at((i + 1 + (j * 24)))){
        //cada vez que uma bomba é ligada ou desligada, devemos penalizar pelo maior custo de manutenção
        nInterrupcoes++;
      }
    }
  }


  return nInterrupcoes;
}
