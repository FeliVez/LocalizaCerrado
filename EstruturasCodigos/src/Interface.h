#ifndef INTERFACE_H
#define INTERFACE_H
#include "Modelos.h"

int DescobrirOpcaoUsuario();
int menu(string s,string f);
string OrigemUsuario(map<string,int> &mp);
string DestinoUsuario(map<string,int> &mp);
void ParadasUsuario(vector<int> &paradas, map<string,int> &mp, string s, string f);

#endif
