#!/usr/bin/env sh

set -eu

BASE_URL="${ROTAS_API_URL:-http://localhost:8080}"

printf '\n== Health ==\n'
curl --fail-with-body -sS "$BASE_URL/api/health"

printf '\n\n== Cidades (primeiros bytes) ==\n'
curl --fail-with-body -sS "$BASE_URL/api/cities" | head -c 300

printf '\n\n== Rota Goiania -> Brasilia ==\n'
curl --fail-with-body -sS -X POST "$BASE_URL/api/route" \
  -H 'Content-Type: application/json' \
  --data '{"origin":"Goiania","destination":"Brasilia","blockedCities":[]}'

printf '\n\n== Rota com parada em Anapolis ==\n'
curl --fail-with-body -sS -X POST "$BASE_URL/api/route/stops" \
  -H 'Content-Type: application/json' \
  --data '{"origin":"Goiania","destination":"Brasilia","stops":["Anapolis"],"blockedCities":[]}'

printf '\n'