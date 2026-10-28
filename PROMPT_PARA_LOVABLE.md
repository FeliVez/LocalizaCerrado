# Prompt de integração para o Lovable

Estou enviando uma versão revisada do backend C++ RotasGoias. Leia `projetos/API_INTEGRACAO_LOVABLE.md` e `projetos/api/openapi.yaml` antes de modificar o frontend.

O backend oferece `GET /api/health`, `GET /api/cities`, `POST /api/route` e `POST /api/route/stops`. Dijkstra e a programação dinâmica com bitmask continuam exclusivamente no C++. O frontend não deve reimplementar os algoritmos.

Preserve o design e as telas existentes. Mantenha a URL-base da API configurável por variável de ambiente `VITE_ROTAS_API_URL`; não fixe `localhost` nem invente uma URL pública. Não mostre dados de demonstração como se fossem dados reais quando a API estiver indisponível. Trate erros HTTP 400, 404, 413, 415, 429 e 500, `found: false`, custos nulos e falhas de conexão.

Nesta etapa, priorize cálculo de rotas, cidades bloqueadas, pedágios, estimativa de combustível e paradas obrigatórias. Não implemente agora telas de cadastro/consulta de CPF. Como as telas de perfil ainda não estão integradas, permita informar uma autonomia opcional (km/L) nas telas de cálculo e envie o campo numérico `autonomy`. Se houver autonomia válida cadastrada para o CPF, o backend dá prioridade ao valor cadastrado; caso contrário, usa a autonomia enviada para aquela rota. Não armazene nem trate esse campo como segredo. Nunca exponha `Usuarios.txt` nem coloque segredos em variáveis `VITE_*`.

O backend precisa ser publicado separadamente como serviço de container. A URL pública HTTPS será fornecida depois da hospedagem; não declare a integração de produção concluída antes de uma chamada real ao backend publicado. Informe os passos de configuração da variável `VITE_ROTAS_API_URL` e do CORS `ROTAS_CORS_ORIGIN`.
