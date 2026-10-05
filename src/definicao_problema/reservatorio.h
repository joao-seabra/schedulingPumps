#ifndef RESERVATORIO_H
#define RESERVATORIO_H

#include <iostream>
#include <iomanip>

class Reservatorio{
  private:
    double volume_max;
    double lim_inferior;
    double lim_superior;
    double volume_inicial;
    std::vector<int> bombasAcopladas;

    std::vector<double> consumo;
  public:
    Reservatorio(double _volume_max, double _volume_atual):
      volume_inicial(_volume_atual){
        volume_max = _volume_max;

        lim_superior = (0.7) * _volume_max; //limite superior 7/10 do maximo
        lim_inferior = (1.0/3.0) * _volume_max; //limite inferior 1/3 do maximo
      }
    virtual ~Reservatorio(){}

    // Getters
    double getVolumeMax() const { return volume_max; }
    double getLimInferior() const { return lim_inferior; }
    double getLimSuperior() const { return lim_superior; }
    double getVolumeInicial() const { return volume_inicial; }
    const std::vector<int> &getBombasAcopladas() const { return bombasAcopladas; }
    const std::vector<double> &getVetorConsumo() const { return consumo; }
    double getConsumo(int hora) const { return consumo.at(hora); }

    // Setters
    void setVolumeMax(double v) { volume_max = v; }
    void setLimInferior(double v) { lim_inferior = v; }
    void setLimSuperior(double v) { lim_superior = v; }
    void setVolumeInicial(double v) { volume_inicial = v; }
    void setConsumo(const std::vector<double>& c) { consumo = c; }

    void acoplarBomba(int bomba) { bombasAcopladas.push_back(bomba); }
    void adicionarConsumo(double c) { consumo.push_back(c); }


    friend std::ostream& operator<<(std::ostream& os, const Reservatorio &r){
      os << std::fixed << std::setprecision(2);
      os << "Volume Max: " << r.volume_max << "m3\n";
      os << "Volume min: " << r.lim_inferior << "m3\n";
      os << "Volume inicial: " << r.volume_inicial << "m3\n";

      os << "Consumo:\n";
      os << std::left << std::setprecision(1);
      for(int j = 0; j < 24; j++){
        os << std::setw(4) << j << " ";
      }
      os << std::endl;
      os << std::left;
      for(double d : r.consumo){
        os << std::setw(4) << d << " ";
      }

      return os;
    }

};

#endif