@echo off
setlocal

cd /d "%~dp0"

where docker >nul 2>nul
if errorlevel 1 (
  echo Docker nao foi encontrado no PATH.
  echo Instale e abra o Docker Desktop e tente novamente.
  pause
  exit /b 1
)

echo Construindo a imagem Linux da API RotasGoias...

docker build -f "%~dp0Dockerfile" -t rotasgoias-api "%~dp0"

if errorlevel 1 (
  echo Falha ao construir a imagem Docker.
  pause
  exit /b 1
)

echo Imagem rotasgoias-api criada com sucesso.
pause