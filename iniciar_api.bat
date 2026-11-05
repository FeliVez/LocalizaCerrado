@echo off
setlocal

cd /d "%~dp0"

if not exist "EstruturasCodigos\data\Usuarios.txt" (
  type nul > "EstruturasCodigos\data\Usuarios.txt"
)

where docker >nul 2>nul
if errorlevel 1 (
  echo Docker nao foi encontrado no PATH.
  echo Instale e abra o Docker Desktop.
  pause
  exit /b 1
)

docker image inspect rotasgoias-api >nul 2>nul
if errorlevel 1 (
  echo A imagem ainda nao existe. Execute compilar_api.bat primeiro.
  pause
  exit /b 1
)

echo Iniciando RotasGoias API em http://localhost:8080

docker run --rm --name rotasgoias-api-local -p 8080:8080 ^
  -e ROTAS_CORS_ORIGIN=http://localhost:5173 ^
  -v "%CD%\EstruturasCodigos\data\Usuarios.txt:/app/data/Usuarios.txt" ^
  rotasgoias-api