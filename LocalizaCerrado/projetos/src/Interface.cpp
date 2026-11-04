#include "Interface.h"
#include "Grafo.h"

int DescobrirOpcaoUsuario(){
    string op;
    cin >> op;
    system("cls");
    while(op.compare("1") && op.compare("0")){
        puts("Opcao invalida. Digite uma opcao valida. (1-sim, 0-nao)");
        cin >> op;
        system("cls");
    }
    return (op[0] - '0');
}


int menu(string s,string f){
    puts("================================  MENU  ==================================");
    cout << "\nOrigem: " << s << endl << "Destino: " << f << endl << endl;
    puts("Selecione a opcao que melhor representa seu desejo.");
    puts("0 - Sair.");
    puts("1 - Calcular rota.");
    puts("2 - Alterar origem e destino.");
    puts("3 - Mudar atividade de alguma cidade.");
    puts("4 - Adicionar paradas.");
    puts("5 - Consultar cidades inativas.");
    puts("=========================================================================");
    string op;
    cin >> op;
    system("cls");
    return (op[0] - '0');
}


string OrigemUsuario(map<string,int> &mp){
    string s;
    getline(cin >> ws,s);
    system("cls");
    while(!mp.count(s)){
    puts("Cidade inexistente no atual escopo. Digite uma cidade existente.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
    puts("Digite a cidade da sua localizacao atual.");
    getline(cin >> ws,s);
    system("cls");
    }
    return s;
}


string DestinoUsuario(map<string,int> &mp){
    string f;
    getline(cin >> ws,f);
    system("cls");
    while(!mp.count(f)){
    puts("Cidade inexistente no atual escopo. Digite uma cidade existente.");
    puts("Digite a cidade do seu destino.");
    getline(cin >> ws,f);
    system("cls");
    }
    return f;
}


void ParadasUsuario(vector<int> &paradas, map<string,int> &mp,string s,string f){
    int op = 1;
    while(op && paradas.size() < 10){
        string p;
        puts("Digite a cidade a qual deseja fazer uma parada.");
        getline(cin >> ws,p);
        system("cls");
        while(!mp.contains(p) || !p.compare(s) || !p.compare(f) || count(paradas.begin(),paradas.end(),mp[p])){
            if(!mp.contains(p)) puts("Cidade Inexistente no atual escopo.\nDigite uma cidade existente.");
            else puts("Cidade ja adicionada. Digite uma cidade ainda nao adicionada.");
            getline(cin,p);
            system("cls");
        }
        paradas.push_back(mp[p]);
        puts("Cidade marcada como parada!\nExiste outra cidade que deseja adicionar como parada? (1-sim, 0-nao)");
        op = DescobrirOpcaoUsuario();
    }
    if(paradas.size() >= 10) puts("Voce atingiu o limite de paradas e nao eh possivel adicionar mais.");
    puts("Parada(s) adicionada(s) com sucesso!");
}
