# RotasGoias — versão organizada

O código foi distribuído em quatro módulos, mantendo as implementações algorítmicas originais. As alterações de lógica foram evitadas; as mudanças na `main.cpp` e nas funções de carregamento/gravação centralizam o acesso aos arquivos.

## Estrutura

- `src/Interface.h/.cpp`: menu, entradas e coleta de paradas.
- `src/Grafo.h/.cpp`: leitura de cidades/conexões e ativação/bloqueio de cidades.
- `src/Usuarios.h/.cpp`: validação, cadastro, busca e persistência de usuários.
- `src/Roteamento.h/.cpp`: Dijkstra, custos, bitmask DP e reconstrução de caminhos.
- `src/Modelos.h`: tipos e constantes compartilhados.
- `data/`: arquivos de dados.

## Compilar (g++ com suporte a C++20)

Execute na pasta raiz do projeto:

```bash
g++ -std=c++20 -Wall -Wextra src/main.cpp src/Interface.cpp src/Grafo.cpp src/Usuarios.cpp src/Roteamento.cpp -o RotasGoias
```

Execute o programa a partir da pasta raiz, pois os caminhos de dados são relativos a ela.
