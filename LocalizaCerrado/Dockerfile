# Backend C++ com Boost.Beast (HTTP) e Boost.PropertyTree (parser JSON).
FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends g++ libc6-dev libboost-dev ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY projetos/ ./
RUN g++ -std=c++20 -O0 -pthread -DBOOST_ERROR_CODE_HEADER_ONLY -DROTAS_API_USE_BOOST \
    src/main.cpp src/Interface.cpp src/Grafo.cpp src/Usuarios.cpp src/Roteamento.cpp src/Api.cpp \
    -o /RotasGoias

FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends libstdc++6 libgcc-s1 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /RotasGoias ./RotasGoias
COPY projetos/data/cidades.txt ./data/cidades.txt
COPY projetos/data/conexoes.txt ./data/conexoes.txt
RUN touch ./data/Usuarios.txt && chmod 600 ./data/Usuarios.txt
ENV PORT=8080
ENV ROTAS_CORS_ORIGIN=http://localhost:5173
EXPOSE 8080
CMD ["/bin/sh", "-c", "exec ./RotasGoias --api ${PORT:-8080}"]
