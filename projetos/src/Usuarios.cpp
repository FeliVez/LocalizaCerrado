#include "Usuarios.h"
#include "Interface.h"

bool ValidacaoCPF(string cpf){
    int i=0,b1=0,b2=0,fat=1;
    while(i!=12){
        if(cpf[i] != '.' && cpf[i] != '-'){
            b1+=(cpf[i] - '0')*fat;
            b2+=(cpf[i] - '0')*(fat-1);
            fat++;
        }
        i++;
    }
    b1%=11;
    if(b1==10) b1=0;
    b2+=(b1*9);
    b2%=11;
    if(b2==10) b2=0;
    if(((cpf[12] - '0') == b1) && ((cpf[13] - '0') == b2)) return true;
    return false;
}
string ValidarCPF(){
    string cpf;
    cin >> cpf;
    system("cls");
    string s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'!@#$%¨&*()_+=`´[{}]~^,<>:;?/|";
    bool sts = true;
    for(int i =0;i<s.size() && sts;i++) if(!cpf.compare("") || cpf.find(s[i])!=string::npos) sts=false;
    while(cpf.size()!=14 || !sts){
            puts("CPF Invalido! Digite seu CPF no formato:\nXXX.XXX.XXX-XX\nSomente algarismos, '.' ou '-'.");
            cin >> cpf;
            system("cls");
            sts=true;
            for(int i =0;i<s.size() && sts;i++) if(!cpf.compare("") || cpf.find(s[i])!=string::npos) sts=false;
        }
        while(!ValidacaoCPF(cpf)){
            puts("CPF invalido! Digite seu CPF no formato:\nXXX.XXX.XXX-XX\nSomente algarismos, '.' ou '-'.\nCadastre um CPF valido para prosseguir.");
            cin >> cpf;
            system("cls");
        }
        return cpf;
}


bool ValidacaoNOME(string nome){
    string s = "0123456789!@#$%¨&*()_+{}:><?/;|[]'',.-=";
    for(int i=0;i<s.size();i++) if(nome.find(s[i])!=string::npos) return false;
    return true;
}
string ValidarNome(){
    string nome;
    getline(cin >> ws,nome);
    system("cls");
        while(!nome.compare("") || !ValidacaoNOME(nome)){
            puts("Nome de usuario invalido. Digite um numero valido sem caracteres especias ou algarismos.");
            getline(cin >> ws,nome);
            system("cls");
        }
    return nome;
}


float ValidarAutonomia(){
    float Autonomia;
    cin >> Autonomia;
    system("cls");
        while(Autonomia <= 0){
            puts("Autonomia invalida. O valor deve ser positivo nao nulo. Informe uma autonomia valida.");
            cin >> Autonomia;
            system("cls");
        }
    return Autonomia;
}


bool Compara(const Usuario &a,const Usuario &b){
    return a.CPF < b.CPF;
}


int BuscaBinariaCPF(vector<Usuario> &V,int i,int j,string cpf){
    if(i > j) return -1;
    if(i==j){
        if(!V[i].CPF.compare(cpf)) return i;
        return -1;
    }
    int M = (i+j) >> 1;
    if(cpf > V[M].CPF) return BuscaBinariaCPF(V,M+1,j,cpf);
    return BuscaBinariaCPF(V,i,M,cpf);
}


bool CadastrarUsuario(Usuario u,vector<Usuario> &usuarios){
    int op = DescobrirOpcaoUsuario();
        if(op){
            puts("Certo! Iremos prosseguir no cadastro de seus dados.");
            puts("Digite seu nome a seguir:");
            u.Nome = ValidarNome();
            puts("Digite a autonomia do seu veiculo, ou seja, quantos quilometros faz com 1L de gasolina.\nCaso seu carro seja eletrico, informe quantos quilometros ele anda com a bateria carregada.");
            u.Autonomia = ValidarAutonomia();
            usuarios.push_back(u);
            sort(usuarios.begin(),usuarios.end(),Compara);
            int chave = BuscaBinariaCPF(usuarios,0,usuarios.size()-1,u.CPF);
            cout << "Cadastro finalizado com sucesso!\nSeja bem vindo(a) pela primeira vez " << usuarios[chave].Nome << "!\nSera um prazer ter voce conosco!\nAgora iremos prosseguir com o programa." << endl;
            return true;    
        }
            puts("Sem cadastro nao podemos prosseguir. Programa sera encerrado.");
            return false;
}

bool CarregarUsuarios(const string &caminho, vector<Usuario> &usuarios){
    ifstream arquivo(caminho);
    if(!arquivo.is_open()){
        puts("Erro ao abrir arquivo.");
        return false;
    }
    Usuario u;
    while(arquivo >> u.CPF && getline(arquivo >> ws,u.Nome) && arquivo >> u.Autonomia) usuarios.push_back(u);
    return true;
}

bool GravarUsuarios(const string &caminho, vector<Usuario> &usuarios){
    ofstream arquivo(caminho);
    if(!arquivo.is_open()){
        puts("Erro ao abrir arquivo.");
        return false;
    }
    for(auto v : usuarios) arquivo << v.CPF << endl << v.Nome << endl << v.Autonomia << endl;
    return true;
}
