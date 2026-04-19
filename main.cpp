#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <map>
using namespace  std;

class Entity {
    private:
    double posicion_x,posicion_y;
    int vida;
    int nivel;
    string nombre;
    vector<string> recursos;
    friend class CommandCenter ;
    public:
    Entity(double x,double y,int _nivel,int _energia,int _vida,vector<string> _re, string n):
    posicion_x(x),posicion_y(y),nivel(_nivel),vida(_vida), recursos(_re), nombre(n){}

    void mover(double dx, double dy) {
        posicion_x += dx;
        posicion_y += dy;
    }

    void curar(int curar) {
        vida += curar;
    }

    void danio(int danio) {
        vida -= danio;
    }

    void subirNivel()
    {
        nivel++;
    }

    void agregarRecurso(string recurso)
    {
        recursos.push_back(recurso);
    }

    void reset() {
        vida = 0;
        posicion_x = 0;
        posicion_y = 0;
        recursos.clear();
        nombre="";
        nivel=0;
    }

    void setNombre(string n) {
        nombre = n;
    }
    void mostrar_nombre() {
        cout << nombre << endl;
    }

    int getVida(){
        return vida;
    }

    int getNivel() {
        return nivel;
    }

    void estado()
    {
        cout<<"-----------------------------"<<endl;
        cout << "Nombre: " << nombre << endl;
        cout << "Vida: " << vida << endl;
        cout << "Nivel: " << nivel << endl;
        cout << "Posicion: " << posicion_x << " " << posicion_y << endl;
        cout << "Recursos: ";
        for (auto& r: recursos) {
            cout << r << " ";
        }
        cout << endl;
    }

};

using Command = function < void ( const list <string >&) >;

void subirNivel(Entity& e,const list<string>& arg)
{
    e.subirNivel();
    cout<<"Se subio de nivel"<<endl;
}

class RecursoFunctor
{
    Entity& entidad;
    int cantidad=0;
    public:
    RecursoFunctor(Entity& e,int c):entidad(e),cantidad(c){}
    void operator()(const list<string>& argumentos)
    {
        if (argumentos.empty()) return;
        entidad.agregarRecurso(argumentos.front());
        cantidad++;
        cout<<"Se agrego "<<cantidad<<" recurso"<<endl;
    }
};


class CommandCenter {
    private:
    Entity* entidad;
    map<string,Command> comandos;
    list<string> historial;
    map<string,list<pair<string, list<string>>>> macros;
    public:
    CommandCenter(Entity *enti) {
        entidad = enti;
    }

    void registrar(const string& nombre,Command comando) {
        comandos[nombre] = comando;
    }

    void eliminar_comando(const string& nombre) {
        if (comandos.find(nombre)!=comandos.end())
        {
            comandos.erase(nombre);
            cout<<"Se elimino el comando "<<nombre<<endl;
        }
    }

    void ejecutar(string nombre,list<string> argumento) {
        map<string,Command>::iterator it=comandos.find(nombre);
        if (it!=comandos.end())
        {
            int Vidaantes=entidad->getVida();
            it->second(argumento);
            int Vidadespues=entidad->getVida();

            string registro="Comando: "+ nombre + ", Estado antes: "+to_string(Vidaantes)+" Estado despues: "+to_string(Vidadespues);
            for (list<string>::iterator it=argumento.begin();it!=argumento.end();it++)
            {
                registro+=" "+*it;
            }

            registro+=" | Antes: "+to_string(Vidaantes)+" Despues: "+to_string(Vidadespues);
            historial.push_back(registro);
        }else
        {
            cout << "Error, el comando"<<nombre<<" no existe"<<endl;
        }
    }

    void registerMacro (const string & name,const list <pair<string ,list <string>>>& steps)
    {
        if (macros.find(name)!=macros.end())
        {
            cerr << "Macro "<<name<<" ya existe"<<endl;
            return;
        }
        macros[name]=steps;
        cout << "Macro "<<name<<" creada"<<endl;
    };
    void executeMacro ( const string & name )
    {
        auto it=macros.find(name);
        if (it==macros.end())
        {
            cerr << "Macro "<<name<<" no existe"<<endl;
            return;
        }

        list<pair<string, list<string>>>::iterator itStep;
        for (itStep=it->second.begin();itStep!=it->second.end();itStep++)
        {
            string nombre=itStep->first;
            list<string> argumentos=itStep->second;
            if (comandos.find(nombre)==comandos.end())
            {
                cout << "Comando "<<nombre<<" no existe"<<endl;
                break;
            }
            ejecutar(nombre,argumentos);
        }

    } ;


    void mostrar_historial() {
        cout<<"------------------------------"<<endl;
        cout<<"Historial de comandos"<<endl;
        list<string>::iterator it;
        for (it=historial.begin();it!=historial.end();it++)
        {
            cout<<*it<<endl;
        }
    }
    void mostrar_entidad() {
        entidad->mostrar_nombre();
    }
    void mostrar_vida() {
        cout << entidad->getVida() << endl;
    }
    void mostrar_nivel() {
        cout << entidad->getNivel() << endl;
    }
    void mostrar_posicion() {
        cout << entidad->posicion_x << " " << entidad->posicion_y << endl;
    }
    void mostrar_recursos() {
        for (auto& r: entidad->recursos) {
            cout << r << endl;
        }
    }
    void mostrar_nombre() {
        cout << entidad->nombre << endl;
    }

};

class HealCenter {
private:
    Entity& entidad;
    int contador;

public:
    HealCenter(Entity& e): entidad(e), contador(0) {}

    void operator()(const list<string>& argumentos) {
        if (argumentos.empty())
        {
            cout<<"El argumento que pases debe ser un numero "<<endl;
            return;
        } try
        {
            int valor = stoi(argumentos.front());
            entidad.curar(valor);
            contador++;
            cout << "Se curaron " << contador << " veces"<<endl;
        }catch (...)
        {
        cout<<"El argumento que pases debe ser un numero "<<endl;}
    }
};

class ContadorAcciones
{
    int total=0;
    public:
    void operator()(const list<string>& argumentos)
    {
        total++;
        cout<<"Se ejecuto "<<total<<" veces"<<endl;
    }
};

void saludo(Entity& e, const list<string>& arg)
{
    cout<<"Bienvenido al centro de comandos"<<endl;
}

void cambiar_nombre(Entity& e, const list<string>& arg)
{
    if (arg.empty()) return;
    e.setNombre(arg.front());
    cout<<"Se cambio el nombre"<<endl;
}


int main() {
    Entity Saiyajin(0, 0, 1, 100, 100, {}, "Son Goku");
    CommandCenter center(&Saiyajin);


    center.registrar("levelup", [](const list<string>& args) {});
    center.registrar("subir_nivel", [&](const list<string>& a){ subirNivel(Saiyajin, a); });
    center.registrar("saludo", [&](const list<string>& a){ saludo(Saiyajin, a); });
    center.registrar("rename", [&](const list<string>& a){ cambiar_nombre(Saiyajin, a); });


    center.registrar("move", [&](const list<string>& args) {
        if (args.size() < 2) return;
        try {
            auto it = args.begin();
            double dx = stod(*it++);
            double dy = stod(*it);
            Saiyajin.mover(dx, dy);
        } catch (...) { cout << "Error: Coordenadas invalidas." << endl; }
    });


    center.registrar("damage", [&](const list<string>& args) {
        if (args.empty()) return;
        try { Saiyajin.danio(stoi(args.front())); }
        catch (...) { cout << "Error: Valor de danio invalido." << endl; }
    });


    center.registrar("status", [&](const list<string>& args) {
        Saiyajin.estado();
    });

    center.registrar("loot", RecursoFunctor(Saiyajin, 0));
    center.registrar("heal", HealCenter(Saiyajin));
    center.registrar("contar", ContadorAcciones());


    center.registerMacro("inicio", {
        {"saludo", {}},
        {"status", {}},
        {"heal", {"20"}}
    });

    center.registerMacro("explorar", {
        {"move", {"15", "30"}},
        {"loot", {"kamehameha"}},
        {"contar", {}}
    });

    // Macro 3: Subida de rango
    center.registerMacro("evolucion", {
        {"rename", {"Goku"}},
        {"subir_nivel", {}},
        {"status", {}}
    });


    center.executeMacro("inicio");
    center.executeMacro("explorar");

    center.ejecutar("damage", {"15"});

    center.ejecutar(" semillas del ermitanio", {"100"});

    center.eliminar_comando("contar");

    center.executeMacro("evolucion");

    center.mostrar_historial();

    cout<<"Estado final del Saiyajin: "<<endl;
    Saiyajin.estado();

    return 0;
}