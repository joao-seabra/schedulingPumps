#ifndef BOMBA_H
#define BOMBA_H

#include <iostream>
#include <iomanip>

class Reservatorio;

class Bomba{
  private:
    double potencia; //KW
    double fluxo; //Fluxo nominal (m3/h)
    double alturaDeTopo; //Head height (mca)
    double eficiencia;
    Reservatorio *res;
  public:
    Bomba(double _potencia, double _fluxo, double _alturaDeTopo, double _eficiencia, Reservatorio *_res):
      potencia(_potencia), fluxo(_fluxo), alturaDeTopo(_alturaDeTopo), eficiencia(_eficiencia), res(_res){}
  
    virtual ~Bomba(){}

    // Getters
    double getPotencia() const { return potencia; }
    double getFluxo() const { return fluxo; }
    double getAlturaDeTopo() const { return alturaDeTopo; }
    double getEficiencia() const { return eficiencia; }

    // Setters
    void setPotencia(double p) { potencia = p; }
    void setFluxo(double f) { fluxo = f; }
    void setAlturaDeTopo(double a) { alturaDeTopo = a; }
    void setEficiencia(double e) { eficiencia = e; }

    friend std::ostream& operator<<(std::ostream& os, const Bomba &b){
      os << std::fixed << std::setprecision(2);
      os << "Potência: " << b.potencia << "kW\n";
      os << "Fluxo: " << b.fluxo << "m3/h\n";
      os << "Altura de topo: " << b.alturaDeTopo << "mca\n";
      os << "Eficiência: " << b.eficiencia << "\n";

      return os;
    }

};

#endif