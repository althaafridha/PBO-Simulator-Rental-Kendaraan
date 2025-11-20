#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
using namespace std;

class Logger {
    ofstream file;
public:
    Logger(const string &name){ file.open(name, ios::app); }
    ~Logger(){ if(file.is_open()) file.close(); }
    void log(const string &msg){ if(file.is_open()) file << msg << ""; }
};

class VehicleException : public exception {
protected:
    string msg;
public:
    VehicleException(string m): msg(m){}
    const char* what() const noexcept override { return msg.c_str(); }
};

class VehicleNotAvailable : public VehicleException{
public:
    VehicleNotAvailable(string id): VehicleException("Vehicle not available: " + id){}
};

class BatteryLowException : public VehicleException{
public:
    BatteryLowException(string id): VehicleException("Battery low on EV: " + id){}
};

class OverloadException : public VehicleException{
public:
    OverloadException(string id): VehicleException("Overload truck: " + id){}
};

class InvalidReturnException : public VehicleException{
public:
    InvalidReturnException(string msg): VehicleException(msg){}
};

class Vehicle{
protected:
    string id, model;
    double dailyRate;
    bool isRented = false;
public:
    Vehicle(string id_, string model_, double r): id(id_), model(model_), dailyRate(r){}
    virtual ~Vehicle(){}
    string getId() const { return id; }
    bool rented() const { return isRented; }
    void setRented(bool b){ isRented = b; }

    virtual double rentCost(int days) const = 0;
    virtual void start(){}
    virtual unique_ptr<Vehicle> clone() const = 0;

    virtual void info(){ cout << id << " - " << model << " rate: " << dailyRate; }
};

class Car : public Vehicle{
    int capacity;
public:
    Car(string id,string m,double r,int c): Vehicle(id,m,r), capacity(c){}
    double rentCost(int d) const override { return dailyRate * d; }
    unique_ptr<Vehicle> clone() const override { return make_unique<Car>(*this); }
    void info() override { Vehicle::info(); cout << " cap: " << capacity; }
};

class Truck : public Vehicle{
    double maxLoad;
public:
    Truck(string id,string m,double r,double ml): Vehicle(id,m,r), maxLoad(ml){}
    double rentCost(int d) const override { return dailyRate * d; }
    double rentCost(int d,double load) const { return dailyRate*d + load*0.05; }
    double getMaxLoad() const { return maxLoad; }
    unique_ptr<Vehicle> clone() const override { return make_unique<Truck>(*this); }
    void info() override { Vehicle::info(); cout << " maxLoad: " << maxLoad; }
};

class ElectricCar : public Vehicle{
    double cap, charge;
public:
    ElectricCar(string id,string m,double r,double c,double ch): Vehicle(id,m,r), cap(c), charge(ch){}
    double rentCost(int d) const override { return dailyRate*d + (charge < cap*0.2 ? 20:0); }
    void start() override {
        if(charge < cap*0.1) throw BatteryLowException(id);
    }
    void chargeEV(double kwh){ charge = min(cap, charge+kwh); }
    unique_ptr<Vehicle> clone() const override { return make_unique<ElectricCar>(*this); }
    void info() override { Vehicle::info(); cout << " charge: "<<charge<<"/"<<cap; }
};

class RentalManager{
    vector<unique_ptr<Vehicle>> fleet;
    Logger &log;
public:
    RentalManager(Logger &lg): log(lg){}

    void addVehicle(const Vehicle &v){ fleet.push_back(v.clone()); }

    Vehicle* find(string id){ for(auto &v: fleet) if(v->getId()==id) return v.get(); return nullptr; }

    void rent(string vid,int days,double load = -1){
        Vehicle* v = find(vid);
        if(!v) throw VehicleException("Vehicle not found");
        if(v->rented()) throw VehicleNotAvailable(vid);

        if(auto t = dynamic_cast<Truck*>(v)){
            if(load >= 0 && load > t->getMaxLoad()){
                log.log("Overload: " + vid);
                throw OverloadException(vid);
            }
        }

        if(auto ev = dynamic_cast<ElectricCar*>(v)){
            ev->start();
        }

        double cost = (dynamic_cast<Truck*>(v) && load>=0) ?
            dynamic_cast<Truck*>(v)->rentCost(days,load): v->rentCost(days);

        cout << "Biaya sewa: " << cost << "";
        v->setRented(true);
        log.log("Rent success: " + vid);
    }

    void returnVehicle(string vid,int actual,bool damage){
        Vehicle* v = find(vid);
        if(!v) throw VehicleException("Vehicle not found");
        if(!v->rented()) throw InvalidReturnException("Vehicle not rented");

        int reserved = 3; 
        int late = max(0, actual - reserved);
        double penalty = late * (v->rentCost(1)*0.5);

        if(damage){
            double dmg = v->rentCost(1)*5;
            if(dmg > 1000){ log.log("Severe damage: "+vid); throw InvalidReturnException("Severe damage!"); }
            penalty += dmg;
        }

        cout << "Total penalty: " << penalty << "";
        v->setRented(false);
        log.log("Return success: " + vid);
    }

    void list(){ for(auto &v: fleet){ v->info(); cout << ""; } }
};

int main(){
    Logger log("log.txt");
    RentalManager rm(log);

    int menu;
    while(true){
        cout << "===== MENU RENTAL KENDARAAN =====" << endl;
        cout << "1. Tambah kendaraan" << endl;
        cout << "2. Lihat daftar kendaraan" << endl;
        cout << "3. Sewa kendaraan" << endl;
        cout << "4. Kembalikan kendaraan" << endl;
        cout << "5. Charge baterai EV" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilih menu: ";
        cin >> menu;

        if(menu == 0) break;

        try{
            if(menu == 1){
                int jenis;
                cout << "1. Car" << endl << "2. Truck" << endl << "3. ElectricCar"<< endl << "Pilih jenis kendaraan: ";
                cin >> jenis;

                string id, model;
                double rate;
                cout << "Masukkan ID: "; cin >> id;
                cout << "Masukkan Model: "; cin >> model;
                cout << "Tarif per hari: "; cin >> rate;

                if(jenis == 1){
                    int cap;
                    cout << "Kapasitas penumpang: "; cin >> cap;
                    rm.addVehicle(Car(id, model, rate, cap));
                }
                else if(jenis == 2){
                    double maxL;
                    cout << "Max load (kg): "; cin >> maxL;
                    rm.addVehicle(Truck(id, model, rate, maxL));
                }
                else if(jenis == 3){
                    double cap, ch;
                    cout << "Kapasitas baterai (kWh): "; cin >> cap;
                    cout << "Isi baterai sekarang (kWh): "; cin >> ch;
                    rm.addVehicle(ElectricCar(id, model, rate, cap, ch));
                }
                cout << "Kendaraan berhasil ditambahkan!" << endl;
            }
            else if(menu == 2){
                rm.list();
            } 
            else if(menu == 3){
                string id; int hari; double muatan;
                cout << "ID kendaraan: "; cin >> id;
                cout << "Lama sewa (hari): "; cin >> hari;
                cout << "Masukkan muatan (Truck), atau -1 jika bukan Truck: "; cin >> muatan;
                rm.rent(id, hari, muatan);
            }
            else if(menu == 4){
                string id; int actual; int dmg;
                cout << "ID kendaraan: "; cin >> id;
                cout << "Jumlah hari aktual: "; cin >> actual;
                cout << "Apakah rusak? (1. ya, 0. tidak): "; cin >> dmg;
                rm.returnVehicle(id, actual, dmg);
            }
            else if(menu == 5){
                string id; double kwh;
                cout << "ID kendaraan: "; cin >> id;
                cout << "Jumlah kWh untuk charge: "; cin >> kwh;

                Vehicle* v = rm.find(id);
                if(auto ev = dynamic_cast<ElectricCar*>(v)){
                    ev->chargeEV(kwh);
                    cout << "Berhasil mengisi baterai!";
                } else {
                    cout << "Kendaraan ini bukan ElectricCar!";
                }
            }
        }
        catch(exception &e){ cout << "Terjadi error: " << e.what() << ""; }
    }
}
