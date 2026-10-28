#include "Roteamento.h"

vector<float> dijkstra(vector<vector<tuple<float,float,int>>> &adj,vector<int> &V,vector<int> &pai,int s){
    vector<float> dist(MAX,INF);
    dist[s] = 0;
    pai.assign(MAX,-1);
    priority_queue<pair<float,int>> q;
    q.push({0,s});
    while(!q.empty()){
        float d = -(q.top().first);
        int u = q.top().second;
        q.pop();
        if(d <= dist[u]){
            for(auto v : adj[u]){
                if(!V[get<2>(v)]) continue;
                float w = d + get<0>(v);
                    if(w < dist[get<2>(v)]){
                        dist[get<2>(v)] = w;
                        pai[get<2>(v)] = u;
                        q.push({-w,get<2>(v)});
                    }
                }
            }
        }
    return dist;
    }

vector<int> ConstruirCaminho(vector<int> &pai,map<string,int> &mp,map<int,string> &pm,string s,string f){
    vector<int> path;
        for(int i=mp[f];pai[i]!=-1;i=pai[i]) path.push_back(i);
        path.push_back(mp[s]);
        reverse(path.begin(),path.end());
        return path;
}

void ExibirResultado(vector<vector<tuple<float,float,int>>> &adj,vector<vector<float>> &MatrizPedagios,vector<int> &V,map<string,int> mp,map<int,string> pm,string s,string f,Usuario u){
    float custoPed = 0.0f;
        vector<int> pai(MAX,-1);
        vector<float> dist = dijkstra(adj,V,pai,mp[s]);
        if(dist[mp[f]] == INF) puts("Nao ha como chegar ao destino.");
        else{
            puts("==================    ROTAS   ====================\n");
            cout << "O caminho mais otimizado no momento eh de: " << dist[mp[f]] << " quilometros." << endl;
            puts("E durante o caminho voce ira passar por:");
            vector<int> path = ConstruirCaminho(pai,mp,pm,s,f);
            for(int i = 0; i < path.size(); i++){
                cout << pm[path[i]];
                if(i != path.size() - 1){
                    cout << " -> ";
                    custoPed += MatrizPedagios[path[i]][path[i+1]];
                }
                else cout << endl;
            }
            float custoGas = ((dist[mp[f]])/u.Autonomia) * Gasolina;
            printf("O custo de pedagios equivale a: R$ %.2f reais.\nO custo de gasolina equivale a: R$ %.2f reais.\nO custo total dessa viagem eh de cerca de: R$ %.2f reais.\n",custoPed,custoGas,custoGas + custoPed);
            puts("\n====================================================\n");
            }
    }


void MatrizPontos(vector<vector<tuple<float,float,int>>> &adj,vector<int> &V,vector<vector<float>> &distPoints, vector<vector<int>> &pais, vector<int> &paradas){
    int K = paradas.size();
    distPoints.resize(K,vector<float>(K));
    pais.resize(K,vector<int>(MAX,-1));
    for(int i = 0; i < K; i++){
        vector<float> menordist = dijkstra(adj,V,pais[i],paradas[i]);
        for(int j = 0; j < K; j++) distPoints[i][j] = menordist[paradas[j]];
    }
}

vector<vector<float>> BitMask(vector<vector<float>> &distPoints,vector<vector<int>> &anterior, int K){
    K-=2;
    vector<vector<float>> dp((1 << K), vector<float>(K, INF));
    anterior.assign((1 << K), vector<int>(K, -1));
    int fullmask = (1 << K) - 1;
    for(int j = 0; j < K; j++){
        if(distPoints[0][j+1] != INF)
            dp[1 << j][j] = distPoints[0][j+1];
    }
    for(int mask = 1; mask < (1 << K); mask++){
        for(int i = 0; i < K; i++){
            if(dp[mask][i] != INF){
                int NaoVisitados = fullmask ^ mask;
                while(NaoVisitados){
                    int j = __builtin_ctzll(NaoVisitados);
                    int newmask = mask | (1 << j);
                    if(distPoints[i+1][j+1] != INF){
                        float w = dp[mask][i] + distPoints[i+1][j+1];
                        if(w < dp[newmask][j]){
                            dp[newmask][j] = w;
                            anterior[newmask][j] = i;
                        }
                    }
                    NaoVisitados &= ~(1 << j);
                }
            }
        }
    }
    return dp;
}

vector<int> ReconstruirCaminho(vector<int> &pai, map<string,int> &mp, int s, int f){
    vector<int> path;
    for(int i = f; pai[i] != -1; i = pai[i]){
        path.push_back(i);
        if(i == s) break;
    }
        path.push_back(s);
        reverse(path.begin(),path.end());
        return path;
}

void ExibirResultadosParadas(vector<vector<int>> &anterior,vector<vector<int>> &pais,vector<vector<float>> &dp, vector<vector<float>> &MatrizPedagios, vector<int> &paradas,vector<vector<float>> &dist,map<string,int> &mp,map<int,string> &pm,int K, string s, string f,Usuario u){
    K -= 2;
    int fullmask = (1 << K) - 1, ultimo = -1;
    float ans = (float) INF;
    for(int i = 0; i < K; i++){
        if(dp[fullmask][i] != INF && dist[K+1][i+1] != INF){
            float w = dp[fullmask][i] + dist[K+1][i+1];
            if(w < ans){
                ans = w;
                ultimo = i;
            }
        }
    }
    puts("\n================ ROTAS ====================\n");
    if(ans == INF){
        puts("Nao tem como realizar esse trecho.");
        return;
    }
    cout << "A distancia mais otimizada no com tais paradas eh de " << ans << " quilometros." << endl;
    vector<int> ordem;
    int mask = fullmask, atual = ultimo;
    while(atual != -1){
        ordem.push_back(atual+1);
        int proximo = anterior[mask][atual];
        mask ^= (1 << (atual));
        atual = proximo;
    }
    ordem.push_back(0);
    reverse(ordem.begin(),ordem.end());
    ordem.push_back(K+1);
    vector<int> path;
    for(int i = 0; i < ordem.size() - 1; i++){
        vector<int> trecho = ReconstruirCaminho(pais[ordem[i]],mp,paradas[ordem[i]],paradas[ordem[i+1]]);
        if(trecho.empty()){
            puts("Nao foi possivel realizar esse trecho.");
            return;
        }
        for(int j = 0; j < trecho.size(); j++){
            if(path.empty() || j) path.push_back(trecho[j]);
        }
    }
    puts("A rota mais otimizada com tais paradas passa por:");
    float custoPed = 0.0f;
    for(int i = 0; i < path.size(); i++){
        cout << pm[path[i]];
        if(i != path.size() - 1){
            cout << " -> ";
            custoPed += MatrizPedagios[path[i]][path[i+1]];
        }
        else cout << endl;
    }
    printf("O custo de combustivel eh de: R$ %.2f reais.\nO custo de pedagios eh de: R$ %.2f reais.\nO custo total eh de: %.2f reais.\n",(ans/u.Autonomia) * Gasolina,custoPed,((ans/u.Autonomia) * Gasolina) + custoPed);
    puts("\n==========================================\n");
}
