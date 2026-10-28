#ifndef GRAFO_H
#define GRAFO_H
#include "Modelos.h"

typedef struct{
    int a,b;
    float w,p;
}Aresta;

bool CarregarCidades(const string &caminho, map<string,int> &mp, map<int,string> &pm);
bool CarregarConexoes(const string &caminho, vector<vector<tuple<float,float,int>>> &adj, vector<vector<float>> &MatrizPedagios);
vector<int> Atividade(vector<int> &V,int op,map<string,int> &mp,string s,string f);

#endif

