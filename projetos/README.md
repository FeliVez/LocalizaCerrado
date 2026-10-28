# RotasGoias — backend C++ e API HTTP

Este pacote mantém os algoritmos de roteamento do RotasGoias em C++ e disponibiliza endpoints HTTP/JSON para o frontend. Dijkstra continua sendo usado para a rota de menor distância; a rota com paradas usa Dijkstra entre pontos e DP com bitmask para decidir a ordem.

## Estrutura principal

- `src/Grafo.cpp/.h`: leitura de cidades e conexões.
- `src/Roteamento.cpp/.h`: Dijkstra, custos, DP com bitmask e reconstrução.
- `src/Usuarios.cpp/.h`: cadastro e leitura de usuários locais.
- `src/Api.cpp/.h`: API HTTP. A implementação web usa Boost.Beast e o parser JSON do Boost.PropertyTree.
- `data/cidades.txt` e `data/conexoes.txt`: dados necessários ao roteamento.
- `data/Usuarios.txt`: arquivo local opcional para autonomia por CPF; não coloque dados pessoais no Git nem em um ZIP público.
- `../Dockerfile`: build e execução do backend Linux em container.

## Executar a API localmente

É necessário ter Docker Desktop instalado e em execução.

1. Abra a pasta `projetos`.
2. Execute `compilar_api.bat` para construir a imagem Docker.
3. Execute `iniciar_api.bat` para iniciar o servidor em `http://localhost:8080`.
4. Em outra janela, execute `testar_api.bat` ou abra `http://localhost:8080/api/health`.

A compilação normal de terminal no Windows continua disponível sem Boost; a API web é compilada no ambiente Linux do Docker para evitar dependências adicionais no MinGW.

## Endpoints

- `GET /api/health`: estado do serviço.
- `GET /api/cities`: cidades com `id` e `name`.
- `GET /api/cities/inactive`: informa que bloqueios não são persistentes.
- `GET /api/docs`: resumo dos endpoints.
- `POST /api/route`: menor rota por distância, com cidades bloqueadas opcionais.
- `POST /api/route/stops`: visita até 10 paradas na melhor ordem, minimizando a distância total.

Consulte `API_INTEGRACAO_LOVABLE.md` e `api/openapi.yaml` para os contratos.

## Variáveis de ambiente

- `PORT`: porta HTTP; padrão `8080`.
- `ROTAS_CORS_ORIGIN`: origem exata autorizada do frontend. Para desenvolvimento local, o padrão é `http://localhost:5173`. Na hospedagem, configure o domínio HTTPS real do frontend no painel da plataforma.

A plataforma de hospedagem deve terminar HTTPS na frente do container. O servidor C++ escuta HTTP na porta configurada.

## Limites e segurança

- Corpo JSON limitado a 64 KiB.
- Timeout de leitura de 10 segundos.
- Até 32 conexões simultâneas em processamento.
- Limite em memória de 120 requisições por minuto por IP observado pelo container.
- JSON malformado, campos com estrutura inválida e cidades bloqueadas inexistentes são rejeitados.
- O arquivo `Usuarios.txt` não é copiado para a imagem Docker; a imagem inicia com ele vazio. Se usar perfis de autonomia, forneça armazenamento persistente privado na hospedagem.

Esses controles são uma base para um projeto de demonstração, não uma garantia de segurança empresarial. O limite por IP depende da configuração de proxy da plataforma; para tráfego público significativo, adicione rate limiting no gateway/WAF e monitoramento. Não use CPF como senha ou mecanismo de autenticação, e não exponha `Usuarios.txt`.

## Observações sobre custos

A API sempre calcula distância e pedágios com os dados locais. Combustível e custo total só são retornados quando o CPF corresponde a um usuário com autonomia válida em `data/Usuarios.txt`. Se houver autonomia válida no cadastro associado ao CPF, ela tem prioridade. Sem autonomia cadastrada, a API aceita `autonomy` como valor informado para aquela requisição, entre 0,1 e 100 km/L.

## Publicar

O arquivo `render.yaml` na raiz do projeto prepara um serviço Docker no Render. Consulte `README_DEPLOY.md` para os passos de publicação e configuração da URL pública no Lovable. A publicação ainda exige acesso às suas contas GitHub, Render e Lovable.
