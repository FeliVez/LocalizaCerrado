@echo off
echo === Health ===
curl -i http://localhost:8080/api/health
echo.
echo === Cidades ===
curl -i http://localhost:8080/api/cities
echo.
echo === Rota Goiania para Brasilia ===
curl -i -X POST http://localhost:8080/api/route -H "Content-Type: application/json" -d "{\"origin\":\"Goiania\",\"destination\":\"Brasilia\",\"blockedCities\":[]}"
echo.
echo === JSON invalido (deve retornar 400) ===
curl -i -X POST http://localhost:8080/api/route -H "Content-Type: application/json" -d "{\"origin\":"
echo.
echo === Cidade bloqueada desconhecida (deve retornar 400) ===
curl -i -X POST http://localhost:8080/api/route -H "Content-Type: application/json" -d "{\"origin\":\"Goiania\",\"destination\":\"Brasilia\",\"blockedCities\":[\"Cidade Inexistente\"]}"
echo.
pause
