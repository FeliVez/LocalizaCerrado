#ifndef ROTEAMENTO_H
#define ROTEAMENTO_H
#include "Modelos.h"

vector<float> dijkstra(vector<vector<tuple<float,float,int>>> &adj,vector<int> &V,vector<int> &pai,int s);
float CalcularAdicionalParada(vector<vector<tuple<float,float,int>>> &adj,vector<int> &V,map<string,int> &mp,map<int,string> &pm,string s,string f,string p);
vector<int> ConstruirCaminho(vector<int> &pai,map<string,int> &mp,map<int,string> &pm,string s,string f);
void ExibirResultado(vector<vector<tuple<float,float,int>>> &adj,vector<vector<float>> &MatrizPedagios,vector<int> &V,map<string,int> mp,map<int,string> pm,string s,string f,Usuario u);
void MatrizPontos(vector<vector<tuple<float,float,int>>> &adj,vector<int> &V,vector<vector<float>> &distPoints, vector<vector<int>> &pais, vector<int> &paradas);
vector<vector<float>> BitMask(vector<vector<float>> &distPoints,vector<vector<int>> &anterior, int K);
vector<int> ReconstruirCaminho(vector<int> &pai, map<string,int> &mp, int s, int f);
void ExibirResultadosParadas(vector<vector<int>> &anterior,vector<vector<int>> &pais,vector<vector<float>> &dp, vector<vector<float>> &MatrizPedagios, vector<int> &paradas,vector<vector<float>> &dist,map<string,int> &mp,map<int,string> &pm,int K, string s, string f, Usuario u);

#endif
