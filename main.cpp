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
    public:
    Entity(double x,double y,int _nivel,int _vida,vector<string> _re, string n):
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
        vida = 100;
        posicion_x = 0;
        posicion_y = 0;
        recursos.clear();
        nombre="reset";
        nivel=1;
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

    string getNombre() {
        return nombre;
    }

    pair<double, double> getPosicion()
    {
        return {posicion_x, posicion_y};
    }

    vector<string> getRecursos()
    {
        return recursos;
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
        if (argumentos.size()!=1)
        {
            cout<<"El argumento que pases requiere de un argumento"<<endl;
            return;
        }
        entidad.agregarRecurso(argumentos.front());
        cantidad++;
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
        if (comandos.erase(nombre))
        {
            cout << "Se elimino el comando "<<nombre<<endl;
        }
    }

    void ejecutar(string nombre,list<string> argumento) {
        auto it = comandos.find(nombre);

        if (it==comandos.end()) {
            cout << "Error: comando '"<<nombre<<"' no existe"<<endl;
            return;
        }

        int antes = entidad->getVida();

        it->second(argumento);

        int despues = entidad->getVida();

        string registro = "Comando: " + nombre;
        registro += " | Antes: " + to_string(antes);
        registro += " | Despues: " + to_string(despues);

        if (!argumento.empty()) {
            registro += " | Args:";
            for (auto& a: argumento) registro += " " + a;
        }

        historial.push_back(registro);
    }

    void registerMacro (const string & name,const list <pair<string ,list <string>>>& steps)
    {
        if (macros.count(name))
        {
            cout << "Macro "<<name<<" ya existe"<<endl;
            return;
        }
        macros[name]=steps;
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
            if (!comandos.count(itStep->first)) {
                cout<<"Error en macro: comando "<<itStep->first<<" no existe"<<endl;
                return;
            }

            ejecutar(itStep->first,itStep->second);
        }
        }
    void mostrar_historial() {
        cout<<"------------------------------"<<endl;
        cout<<"Historial de comandos"<<endl;
        for (auto&h: historial)
            cout<<h<<endl;
    }

} ;


class HealCenter {
private:
    Entity& entidad;
    int contador;

public:
    HealCenter(Entity& e): entidad(e), contador(0) {}

    void operator()(const list<string>& argumentos) {
        if (argumentos.size()!=1)
        {
            cout<<"El argumento que pases requiere de un argumento"<<endl;
            return;
        } try
        {
            int valor = stoi(argumentos.front());
            entidad.curar(valor);
            contador++;
            cout << "Se curaron " << contador << " veces"<<endl;
        }catch (...)
        {
        cout<<"El argumento que pases debe ser un numero "<<endl;
        }
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
    Entity Saiyajin(0, 0,1, 100, {}, "Son Goku");
    CommandCenter center(&Saiyajin);


    center.registrar("subir_nivel", [&](const list<string>& a){ subirNivel(Saiyajin, a); });
    center.registrar("saludo", [&](const list<string>& a){ saludo(Saiyajin, a); });



    center.registrar("move", [&](const list<string>& args) {
        if (args.size()!=2)
        {
            cout<<"El comando move requiere dos argumentos"<<endl;
            return;
        }
        try {
            auto it = args.begin();
            double dx = stod(*it++);
            double dy = stod(*it);
            Saiyajin.mover(dx, dy);
        } catch (...)
        {
            cout << "Error: Coordenadas invalidas." << endl;
        }
    });


    center.registrar("damage", [&](const list<string>& args) {
       if (args.size()!=1)
       {
           cout<<"El comando damage requiere un argumento"<<endl;
           return;
       }
        try { Saiyajin.danio(stoi(args.front())); }
        catch (...) { cout << "Error: Valor de danio invalido." << endl; }
    });


    center.registrar("status", [&](const list<string>& args) {
        Saiyajin.estado();
    });

    center.registrar("reset", [&](const list<string>& args)
    {
        if (!args.empty())
        {
            cout<<"El comando reset no recibe argumentos"<<endl;
            return;
        }
        Saiyajin.reset();
    });

    center.registrar("rename", [&](const list<string>& args)
    {
        if (args.size()!=1)
        {
            cout<<"El comando rename requiere 1 argumentos"<<endl;
            return;
        }
        Saiyajin.setNombre(args.front());
    });

    center.registrar("loot", RecursoFunctor(Saiyajin, 0));
    center.registrar("heal", HealCenter(Saiyajin));
    center.registrar("contar", ContadorAcciones());


    center.registerMacro("inicio", {
        {"status", {}},
        {"heal", {"20"}}
    });

    center.registerMacro("explorar", {
        {"move", {"10", "5"}},
        {"loot", {"ki"}},
        {"contar", {}}
    });

    center.registerMacro("full", {
        {"reset", {}},
        {"subir_nivel", {}},
        {"status", {}}
    });


    center.executeMacro("inicio");
    center.executeMacro("explorar");

    center.ejecutar("damage", {"15"});

    center.ejecutar("comando_invalido", {"100"});

    center.eliminar_comando("contar");

    center.mostrar_historial();

    cout<<"Estado final del Saiyajin: "<<endl;
    Saiyajin.estado();

    return 0;
}