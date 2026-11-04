#include "Grafo.h"
#include "Interface.h"

bool CarregarCidades(const string &caminho, map<string,int> &mp, map<int,string> &pm){
    ifstream arquivo(caminho);
    if(!arquivo.is_open()){
        puts("Erro ao abrir arquivo.");
        return false;
    }
    int id;
    string cidade;
    while(arquivo >> id && getline(arquivo >> ws,cidade)){
        mp[cidade] = id;
        pm[id] = cidade;
    }
    return !mp.empty();
}

bool CarregarConexoes(const string &caminho, vector<vector<tuple<float,float,int>>> &adj, vector<vector<float>> &MatrizPedagios){
    ifstream arquivo(caminho);
    if(!arquivo.is_open()){
        puts("Erro ao abrir arquivo.");
        return false;
    }
    Aresta a;
    while(arquivo >> a.a >> a.b >> a.w >> a.p){
        adj[a.a].push_back({a.w,a.p,a.b});
        adj[a.b].push_back({a.w,a.p,a.a});
        MatrizPedagios[a.a][a.b] = MatrizPedagios[a.b][a.a] = a.p;
    }
    return true;
}

vector<int> Atividade(vector<int> &V,int op,map<string,int> &mp,string s,string f){
    while(op){
        puts("Digite o nome da cidade que deseja mudar atividade.");
        string cidade;
        getline(cin >> ws,cidade);
        system("cls");
        while(!mp.count(cidade)){
            puts("Cidade inexistente no atual escopo. Digite uma cidade existente.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
            getline(cin >> ws,cidade);
            system("cls");
        }
        while((!cidade.compare(s) || !cidade.compare(f))){
            puts("Se voce bloquear a cidade de origem ou destino, nao ha como chegar.\nDigite outra cidade.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
            getline(cin >> ws,cidade);
            system("cls");
        }
        map<string,int> lb;
        if(V[mp[cidade]]){
            V[mp[cidade]] = 0;
            puts("Cidade bloqueada com sucesso!");
        } else{
            V[mp[cidade]] = 1;
            puts("Cidade desbloqueada com sucesso.");
        }
        puts("Existem outras cidades que deseja mudar os status? (1-sim, 0-nao)");
        op = DescobrirOpcaoUsuario();
    }
    return V;
}
