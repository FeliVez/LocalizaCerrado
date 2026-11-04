@echo off
cd /d "%~dp0"
where docker >nul 2>nul
if errorlevel 1 (
  echo Docker Desktop nao foi encontrado. Instale e abra o Docker Desktop para construir a API.
  pause
  exit /b 1
)
echo Construindo a imagem Linux da API RotasGoias...
docker build -f "%~dp0..\Dockerfile" -t rotasgoias-api "%~dp0.."
if errorlevel 1 (
  echo Falha ao construir a imagem Docker.
  pause
  exit /b 1
)
echo Imagem rotasgoias-api criada com sucesso.
pause
