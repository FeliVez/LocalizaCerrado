#ifndef USUARIOS_H
#define USUARIOS_H
#include "Modelos.h"

bool ValidacaoCPF(string cpf);
string ValidarCPF();
bool ValidacaoNOME(string nome);
string ValidarNome();
float ValidarAutonomia();
int BuscaBinariaCPF(vector<Usuario> &V,int i,int j,string cpf);
bool Compara(const Usuario &a,const Usuario &b);
bool CadastrarUsuario(Usuario u,vector<Usuario> &usuarios);
bool CarregarUsuarios(const string &caminho, vector<Usuario> &usuarios);
bool GravarUsuarios(const string &caminho, vector<Usuario> &usuarios);

#endif
