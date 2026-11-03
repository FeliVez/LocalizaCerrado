# LocalizaCerrado — contrato de integração com Lovable

O frontend deve chamar esta API. Os algoritmos de roteamento permanecem no backend C++.

## Executar localmente

Use Docker Desktop:

1. Na pasta `projetos`, execute `compilar_api.bat`.
2. Execute `iniciar_api.bat`.
3. Teste `http://localhost:8080/api/health`.

O padrão CORS local autoriza `http://localhost:5173`. Se o frontend de desenvolvimento usar outra origem, defina `ROTAS_CORS_ORIGIN` antes de iniciar o container. Para produção, configure a origem HTTPS exata do domínio do site no serviço de hospedagem.

## Endpoints

### `GET /api/health`

Resposta:
```json
{"status":"ok","service":"RotasGoias API","version":"2.0.0"}
```

### `GET /api/cities`

Resposta com a lista usada para preencher os seletores:
```json
{"cities":[{"id":1,"name":"Anapolis"},{"id":2,"name":"Goiania"}]}
```
Os nomes acima são ilustrativos; a API retorna os dados carregados de `data/cidades.txt`.

### `POST /api/route`

Cabeçalho obrigatório: `Content-Type: application/json`.

Requisição:
```json
{
  "origin": "Goiania",
  "destination": "Brasilia",
  "blockedCities": [],
  "cpf": "",
  "autonomy": 9.5
}
```

- `origin` e `destination`: nomes exatos fornecidos por `/api/cities`.
- `blockedCities`: lista opcional de nomes exatos a evitar. Cidade desconhecida gera HTTP 400. Origem e destino não são bloqueados.
- `cpf`: opcional; se estiver cadastrado em `data/Usuarios.txt`, a autonomia daquele perfil é usada. CPF não é autenticação.
- `autonomy`: opcional, autonomia informada para aquela rota em km/L, de 0,1 a 100. Se o CPF corresponder a um usuário com autonomia válida, a autonomia cadastrada tem prioridade; caso contrário, usa-se o valor informado. Isso permite calcular combustível enquanto as telas de cadastro/consulta ainda não estão integradas.

Resposta de sucesso (valores ilustrativos):
```json
{
  "found": true,
  "origin": "Goiania",
  "destination": "Brasilia",
  "distanceKm": 215.40,
  "path": ["Goiania", "...", "Brasilia"],
  "tollCost": 0.00,
  "fuelPricePerLiter": 6.43,
  "autonomyKmPerLiter": null,
  "fuelLiters": null,
  "fuelCost": null,
  "totalCost": null
}
```

Se não existir caminho, a API retorna `found: false` e `message`. Se não houver autonomia cadastrada, os campos de combustível e custo total são `null`.

### `POST /api/route/stops`

Requisição:
```json
{
  "origin": "Goiania",
  "destination": "Brasilia",
  "stops": ["Anapolis", "Abadiania"],
  "blockedCities": [],
  "cpf": "",
  "autonomy": 9.5
}
```

`stops` aceita até 10 cidades únicas, diferentes da origem e do destino. A resposta usa os campos da rota normal e inclui `stopOrder`, que contém origem, paradas na ordem otimizada e destino. A distância total é a soma dos trechos escolhidos.

### `GET /api/cities/inactive`

Retorna uma lista vazia e explica que os bloqueios são temporários por requisição. O frontend deve manter a seleção de bloqueios na tela e enviar `blockedCities` a cada cálculo.

### Códigos de erro

- `400`: JSON inválido, campos inválidos, cidade desconhecida ou parada inválida.
- `404`: endpoint não encontrado.
- `413`: corpo maior que 64 KiB.
- `415`: requisição POST sem `Content-Type: application/json`.
- `429`: limite de requisições excedido.
- `500`: erro interno durante o cálculo.

As respostas de erro têm o formato `{"error":"mensagem"}`.

## Exemplo de chamada TypeScript

```ts
const response = await fetch(`${import.meta.env.VITE_ROTAS_API_URL}/api/route`, {
  method: "POST",
  headers: { "Content-Type": "application/json" },
  body: JSON.stringify({
    origin: origemSelecionada,
    destination: destinoSelecionado,
    blockedCities: cidadesBloqueadas,
    cpf: cpfFormatado || undefined,
    autonomy: autonomiaSelecionada || undefined
  })
});

const resultado = await response.json();
if (!response.ok || resultado.error) {
  throw new Error(resultado.error ?? "Falha ao calcular a rota");
}
```

A URL pública deve ser configurada como variável de ambiente no frontend, por exemplo `VITE_ROTAS_API_URL`. Não coloque segredos em variáveis `VITE_*`, pois elas são incorporadas ao código público do navegador.

## Hospedagem

O `Dockerfile` na raiz do ZIP constrói o backend Linux. Publique a imagem em um serviço de container (por exemplo, Render ou Railway), configure `PORT` se a plataforma exigir e defina `ROTAS_CORS_ORIGIN` para a origem HTTPS exata do frontend. Configure armazenamento persistente privado se o arquivo de usuários for utilizado. O arquivo de usuários é intencionalmente omitido da imagem.

O backend deve estar publicamente acessível via HTTPS por meio da plataforma antes de o frontend publicado conseguir chamá-lo. `localhost` só funciona no computador/ambiente que executa o serviço.

## Instruções ao Lovable

- Use `GET /api/cities` para os seletores e os dois endpoints POST para cálculos.
- Não replique Dijkstra nem a DP com bitmask no frontend.
- Use a URL-base por variável de ambiente, sem fixar URL de produção no código.
- Trate estados de carregamento, erros HTTP, `found: false` e custos nulos.
- Não mostre cidades de demonstração como se viessem da API real.
- Não implemente cadastro/consulta de usuários nesta etapa. A prioridade atual é o cálculo de rotas.
