#include "Modelos.h"
#include "Interface.h"
#include "Grafo.h"
#include "Usuarios.h"
#include "Roteamento.h"
#include "Api.h"

signed main(signed argc, char* argv[]){
    if(argc >= 2 && string(argv[1]) == "--api"){
        int porta = 8080;
        if(argc >= 3){
            try{ porta = stoi(argv[2]); }catch(...){ cerr << "Porta invalida.\n"; return 1; }
        }
        return IniciarAPI(porta);
    }
    ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0);
    puts("Deseja iniciar o programa? (1-sim, 0-nao)");
    int op = DescobrirOpcaoUsuario();
        if(op){
            vector<Usuario> usuarios;
            if(!CarregarUsuarios("data/Usuarios.txt",usuarios)){
                puts("Erro ao abrir arquivo. O programa sera encerrado.");
                return 0;
            }
            map<string,int> mp;
            map<int,string> pm;
            if(!CarregarCidades("data/cidades.txt",mp,pm)){
                puts("Erro ao abrir arquivo. O programa sera encerrado.");
                return 0;
            }
            vector<vector<tuple<float,float,int>>> adj(MAX);
            vector<vector<float>> MatrizPedagios(MAX,vector<float>(MAX));
            if(!CarregarConexoes("data/conexoes.txt",adj,MatrizPedagios)){
                puts("Erro ao abrir o arquivo. O programa sera encerrado.");
                return 0;
            }
            vector<int> V(MAX,1);
            puts("Digite seu CPF no formato:\nXXX.XXX.XXX-XX");
            Usuario u;
            u.CPF = ValidarCPF();
            puts("===============\nCPF validado!\n===============\n");
                if(usuarios.empty()){
                    puts("Nao ha usuarios cadastrados no momento. Gostaria de se cadastrar? (1-sim,0-nao)");
                    if(!CadastrarUsuario(u,usuarios)) return 0;
                }else{
                    int chave = BuscaBinariaCPF(usuarios,0,usuarios.size()-1,u.CPF);
                        if(chave!=-1){
                            u = usuarios[chave];
                            cout << "CPF encontrado com sucesso! Bem vindo(a) de volta " << usuarios[chave].Nome << '!' << endl;
                    puts("Agora iremos prosseguir com o programa!");
                        } else{
                            puts("Seu CPF ainda nao foi cadastrado no sistema.\nGostaria de realizar seu cadastro no sistema? (1-sim,0-nao)");
                if(!CadastrarUsuario(u,usuarios)) return 0;
                        }   
                }
            puts("Digite a cidade da sua localizacao atual.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
            string s = OrigemUsuario(mp);
            puts("Digite a cidade do seu destino.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
            string f = DestinoUsuario(mp),p;
            bool sts = false;
            vector<int> paradas;
            vector<vector<float>> distPoints,dp;
            vector<vector<int>> anterior,pais; 
                while(op){
                    op = menu(s,f);
                        switch(op){
                        case 0:
                            if(!GravarUsuarios("data/Usuarios.txt",usuarios)) puts("Erro ao abrir arquivo.");
                            puts("Programa encerrado. Obrigado pela visita!");
                            return 0;
                        case 1:
                            ExibirResultado(adj,MatrizPedagios,V,mp,pm,s,f,u);
                            break;
                        case 2:
                            puts("A seguir, informe seus novos interesses.");
                            puts("Digite a cidade da sua localizacao atual.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
                            s = OrigemUsuario(mp);
                            puts("Digite a cidade do seu destino.\nCada cidade comeca com maiusculas e em cidades com nome compostos, cada nome comeca com maiusculas.\nNao coloque caracteres especias como acentos.");
                            f = DestinoUsuario(mp);
                            puts("Localidades alteradas com sucesso!");
                            break;
                        case 3:
                            V = Atividade(V,op,mp,s,f);
                            break;
                        case 4:
                            paradas.push_back(mp[s]);
                            ParadasUsuario(paradas,mp,s,f);
                            paradas.push_back(mp[f]);
                            MatrizPontos(adj,V,distPoints,pais,paradas);
                            dp = BitMask(distPoints,anterior,paradas.size());
                            ExibirResultadosParadas(anterior,pais,dp,MatrizPedagios,paradas,distPoints,mp,pm,paradas.size(),s,f,u);
                            paradas.clear();
                            break;
                        case 5:
                            for(int i = 1; i < V.size(); i++){
                                if(!V[i]){
                                    sts = true;
                                    cout << pm[i] << endl;
                                }
                            }
                            if(!sts) puts("Voce ainda nao bloqueou nenhuma cidade.");
                            else puts("Essas sao as atuais cidade bloqueadas.");
                            sts = false;
                            break;
                        default:
                            puts("Opcao invalida.");
                            break;
                        }
                puts("Voce deseja continuar no programa? (1-sim,0-nao)");
                op = DescobrirOpcaoUsuario();
            }
            if(!GravarUsuarios("data/Usuarios.txt",usuarios)){
                puts("Erro ao abrir arquivo. O programa sera encerrado.");
                return 0;
            }
        }
    puts("programa encerrado, obigado pela visita!");
    return 0;
}
