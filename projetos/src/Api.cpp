#include <iostream>

#ifndef ROTAS_API_USE_BOOST
#include "Modelos.h"
#include "Api.h"
int IniciarAPI(int porta){
    std::cerr << "A API HTTP requer Boost.Beast. Use o Dockerfile para executar o backend; "
              << "a compilacao normal do terminal continua independente dessa biblioteca.\n";
    (void)porta;
    return 1;
}
#else
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Modelos.h"
#include "Api.h"
#include "Grafo.h"
#include "Roteamento.h"
#include "Usuarios.h"

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;


namespace {
const string PASTA_DADOS = "data/";

string EscaparJSON(const string &s){
    string r;
    for(unsigned char c : s){
        switch(c){
            case '"': r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\b': r += "\\b"; break;
            case '\f': r += "\\f"; break;
            case '\n': r += "\\n"; break;
            case '\r': r += "\\r"; break;
            case '\t': r += "\\t"; break;
            default: if(c < 0x20){
                char buf[7]; snprintf(buf,sizeof(buf),"\\u%04x",c); r += buf;
            } else r += static_cast<char>(c);
        }
    }
    return r;
}
string JSONString(const string &s){ return "\"" + EscaparJSON(s) + "\""; }
string Numero(float x){
    if(!isfinite(x)) return "null";
    ostringstream os; os << fixed << setprecision(2) << x; return os.str();
}
string ErroJSON(const string &msg){ return "{\"error\":" + JSONString(msg) + "}"; }

// JSON e interpretado por um parser que valida a sintaxe completa do documento.
boost::property_tree::ptree ParseJSON(const string &texto){
    boost::property_tree::ptree raiz;
    istringstream entrada(texto);
    boost::property_tree::read_json(entrada, raiz);
    return raiz;
}

string CampoString(const string &texto, const string &campo){
    try{
        auto raiz = ParseJSON(texto);
        auto valor = raiz.get_optional<string>(campo);
        return valor ? *valor : "";
    }catch(...){ return ""; }
}

vector<string> CampoArrayStrings(const string &texto, const string &campo){
    vector<string> resultado;
    try{
        auto raiz = ParseJSON(texto);
        auto campoAr = raiz.get_child_optional(campo);
        if(!campoAr) return resultado;
        for(const auto &item : *campoAr){
            if(!item.first.empty() || !item.second.empty()) return {};
            resultado.push_back(item.second.data());
        }
    }catch(...){ return {}; }
    return resultado;
}

float CampoNumero(const string &texto, const string &campo){
    try{
        auto raiz = ParseJSON(texto);
        auto valor = raiz.get_optional<string>(campo);
        if(!valor) return 0;
        size_t consumidos = 0;
        float numero = stof(*valor, &consumidos);
        while(consumidos < valor->size() && isspace(static_cast<unsigned char>((*valor)[consumidos]))) ++consumidos;
        if(consumidos != valor->size() || !isfinite(numero) || numero < 0.1f || numero > 100.0f) return 0;
        return numero;
    }catch(...){ return 0; }
}

bool ValidarJSONRequisicao(const string &texto, string &erro){
    try{
        auto raiz = ParseJSON(texto);
        for(const auto &item : raiz){
            if(item.first.empty()) { erro = "O corpo JSON deve ser um objeto."; return false; }
        }
        for(const char *campo : {"origin", "destination", "cpf", "autonomy"}){
            auto it = raiz.get_child_optional(campo);
            if(it && !it->empty()) { erro = string("O campo '") + campo + "' deve ser texto."; return false; }
        }
        for(const char *campo : {"blockedCities", "stops"}){
            auto it = raiz.get_child_optional(campo);
            if(it){
                for(const auto &item : *it){
                    if(!item.first.empty() || !item.second.empty()){
                        erro = string("O campo '") + campo + "' deve ser uma lista de textos."; return false;
                    }
                }
            }
        }
        if(raiz.get_child_optional("autonomy") && CampoNumero(texto, "autonomy") == 0){
            erro = "O campo 'autonomy' deve ser um numero entre 0,1 e 100 km/L."; return false;
        }
        return true;
    }catch(const std::exception &){ erro = "JSON invalido ou malformado."; return false; }
}

struct Dados {
    map<string,int> mp;
    map<int,string> pm;
    vector<vector<tuple<float,float,int>>> adj;
    vector<vector<float>> pedagios;
    vector<Usuario> usuarios;
    bool carregar(){
        adj.assign(MAX,{}); pedagios.assign(MAX,vector<float>(MAX,0));
        return CarregarCidades("data/cidades.txt",mp,pm) &&
               CarregarConexoes("data/conexoes.txt",adj,pedagios) &&
               CarregarUsuarios("data/Usuarios.txt",usuarios);
    }
};

string JSONCidades(const Dados &d){
    string r="{\"cities\":["; bool primeiro=true;
    for(const auto &par : d.pm){
        if(!primeiro) r+=','; primeiro=false;
        r+="{\"id\":"+to_string(par.first)+",\"name\":"+JSONString(par.second)+"}";
    }
    return r+"]}";
}

string RotaBasica(Dados &d,const string &body){
    string origem=CampoString(body,"origin"), destino=CampoString(body,"destination");
    if(origem.empty() || destino.empty()) return ErroJSON("Informe origin e destination como nomes de cidades.");
    if(!d.mp.count(origem)) return ErroJSON("Cidade de origem nao encontrada.");
    if(!d.mp.count(destino)) return ErroJSON("Cidade de destino nao encontrada.");
    vector<int> ativo(MAX,1),pai;
    for(const string &cidade : CampoArrayStrings(body,"blockedCities")){
        if(!d.mp.count(cidade)) return ErroJSON("Cidade bloqueada nao encontrada: " + cidade);
        if(cidade!=origem && cidade!=destino) ativo[d.mp[cidade]]=0;
    }
    vector<float> dist=dijkstra(d.adj,ativo,pai,d.mp[origem]);
    if(dist[d.mp[destino]]==INF) return "{\"found\":false,\"message\":\"Nao existe rota disponivel com essas restricoes.\"}";
    vector<int> caminho=ConstruirCaminho(pai,d.mp,d.pm,origem,destino);
    float custoPed=0;
    string cidades="[";
    for(size_t i=0;i<caminho.size();i++){
        if(i) cidades+=',';
        cidades+=JSONString(d.pm[caminho[i]]);
        if(i+1<caminho.size()) custoPed+=d.pedagios[caminho[i]][caminho[i+1]];
    }
    cidades+=']';
    float autonomia=0; string cpf=CampoString(body,"cpf");
    if(!cpf.empty()) for(const Usuario &u:d.usuarios) if(u.CPF==cpf){ autonomia=u.Autonomia; break; }
    if(autonomia<=0) autonomia=CampoNumero(body,"autonomy");
    float litros=autonomia>0 ? dist[d.mp[destino]]/autonomia : 0;
    float custoGas=litros*Gasolina;
    return "{\"found\":true,\"origin\":"+JSONString(origem)+",\"destination\":"+JSONString(destino)+
        ",\"distanceKm\":"+Numero(dist[d.mp[destino]])+",\"path\":"+cidades+
        ",\"tollCost\":"+Numero(custoPed)+",\"fuelPricePerLiter\":"+Numero(Gasolina)+
        ",\"autonomyKmPerLiter\":"+(autonomia>0?Numero(autonomia):"null")+
        ",\"fuelLiters\":"+(autonomia>0?Numero(litros):"null")+
        ",\"fuelCost\":"+(autonomia>0?Numero(custoGas):"null")+
        ",\"totalCost\":"+(autonomia>0?Numero(custoPed+custoGas):"null")+"}";
}

string RotaComParadas(Dados &d,const string &body){
    string origem=CampoString(body,"origin"), destino=CampoString(body,"destination");
    vector<string> nomesParadas=CampoArrayStrings(body,"stops");
    if(origem.empty() || destino.empty()) return ErroJSON("Informe origin e destination.");
    if(!d.mp.count(origem) || !d.mp.count(destino)) return ErroJSON("Origem ou destino nao encontrado.");
    if(nomesParadas.size()>10) return ErroJSON("O limite atual da API e de 10 paradas.");
    vector<int> ativo(MAX,1), pontos; set<int> unicas;
    for(const string &cidade : CampoArrayStrings(body,"blockedCities")){
        if(!d.mp.count(cidade)) return ErroJSON("Cidade bloqueada nao encontrada: " + cidade);
        if(cidade!=origem && cidade!=destino) ativo[d.mp[cidade]]=0;
    }
    pontos.push_back(d.mp[origem]);
    for(const string &cidade : nomesParadas){
        if(!d.mp.count(cidade)) return ErroJSON("Uma das paradas nao foi encontrada: "+cidade);
        int id=d.mp[cidade];
        if(cidade==origem || cidade==destino || !unicas.insert(id).second)
            return ErroJSON("As paradas devem ser unicas e diferentes da origem e do destino.");
        if(!ativo[id]) return ErroJSON("Uma parada solicitada esta bloqueada: "+cidade);
        pontos.push_back(id);
    }
    pontos.push_back(d.mp[destino]);
    int n=(int)nomesParadas.size();
    vector<vector<float>> distPontos(n+2,vector<float>(n+2,INF));
    vector<vector<int>> pais(n+2,vector<int>(MAX,-1));
    for(int i=0;i<n+1;i++){
        vector<int> pai;
        vector<float> dist=dijkstra(d.adj,ativo,pai,pontos[i]);
        pais[i]=pai;
        for(int j=0;j<n+2;j++) distPontos[i][j]=dist[pontos[j]];
    }
    vector<int> ordem;
    if(n==0){
        if(distPontos[0][1]==INF) return "{\"found\":false,\"message\":\"Nao existe rota disponivel.\"}";
        ordem={0,1};
    } else {
        int estados=1<<n, completo=estados-1;
        vector<vector<float>> dp(estados,vector<float>(n,INF));
        vector<vector<int>> anterior(estados,vector<int>(n,-1));
        for(int i=0;i<n;i++) if(distPontos[0][i+1]!=INF) dp[1<<i][i]=distPontos[0][i+1];
        for(int mask=1;mask<estados;mask++) for(int i=0;i<n;i++) if(dp[mask][i]!=INF){
            for(int j=0;j<n;j++) if(!(mask&(1<<j)) && distPontos[i+1][j+1]!=INF){
                int novo=mask|(1<<j); float custo=dp[mask][i]+distPontos[i+1][j+1];
                if(custo<dp[novo][j]){ dp[novo][j]=custo; anterior[novo][j]=i; }
            }
        }
        float melhor=INF; int ultimo=-1;
        for(int i=0;i<n;i++) if(dp[completo][i]!=INF && distPontos[i+1][n+1]!=INF){
            float custo=dp[completo][i]+distPontos[i+1][n+1];
            if(custo<melhor){ melhor=custo; ultimo=i; }
        }
        if(ultimo==-1) return "{\"found\":false,\"message\":\"Nao existe rota que conecte todas as paradas solicitadas.\"}";
        int mask=completo, atual=ultimo; vector<int> reversa;
        while(atual!=-1){ reversa.push_back(atual+1); int prox=anterior[mask][atual]; mask^=(1<<atual); atual=prox; }
        reverse(reversa.begin(),reversa.end()); ordem.push_back(0);
        for(int p:reversa) ordem.push_back(p);
        ordem.push_back(n+1);
    }
    vector<int> caminho;
    for(size_t i=0;i+1<ordem.size();i++){
        int a=ordem[i], b=ordem[i+1];
        vector<int> pai;
        vector<float> dist=dijkstra(d.adj,ativo,pai,pontos[a]);
        if(dist[pontos[b]]==INF) return "{\"found\":false,\"message\":\"Nao existe rota entre dois pontos da viagem.\"}";
        vector<int> trecho; int atual=pontos[b];
        while(atual!=pontos[a] && atual!=-1){ trecho.push_back(atual); atual=pai[atual]; }
        if(atual==-1) return ErroJSON("Falha ao reconstruir um trecho da rota.");
        trecho.push_back(pontos[a]); reverse(trecho.begin(),trecho.end());
        for(size_t j=0;j<trecho.size();j++) if(caminho.empty() || j>0) caminho.push_back(trecho[j]);
    }
    float distanciaTotal=0,custoPed=0;
    string cidades="[";
    for(size_t i=0;i<caminho.size();i++){
        if(i) cidades+=','; cidades+=JSONString(d.pm[caminho[i]]);
        if(i+1<caminho.size()){
            custoPed+=d.pedagios[caminho[i]][caminho[i+1]];
            for(const auto &aresta:d.adj[caminho[i]]) if(get<2>(aresta)==caminho[i+1]){ distanciaTotal+=get<0>(aresta); break; }
        }
    }
    cidades+=']';
    string ordemNomes="["; for(size_t i=0;i<ordem.size();i++){ if(i) ordemNomes+=','; ordemNomes+=JSONString(d.pm[pontos[ordem[i]]]); } ordemNomes+=']';
    float autonomia=0; string cpf=CampoString(body,"cpf");
    if(!cpf.empty()) for(const Usuario &u:d.usuarios) if(u.CPF==cpf){ autonomia=u.Autonomia; break; }
    if(autonomia<=0) autonomia=CampoNumero(body,"autonomy");
    float litros=autonomia>0?distanciaTotal/autonomia:0, custoGas=litros*Gasolina;
    return "{\"found\":true,\"origin\":"+JSONString(origem)+",\"destination\":"+JSONString(destino)+
        ",\"distanceKm\":"+Numero(distanciaTotal)+",\"path\":"+cidades+",\"stopOrder\":"+ordemNomes+
        ",\"tollCost\":"+Numero(custoPed)+",\"fuelPricePerLiter\":"+Numero(Gasolina)+
        ",\"autonomyKmPerLiter\":"+(autonomia>0?Numero(autonomia):"null")+
        ",\"fuelLiters\":"+(autonomia>0?Numero(litros):"null")+
        ",\"fuelCost\":"+(autonomia>0?Numero(custoGas):"null")+
        ",\"totalCost\":"+(autonomia>0?Numero(custoGas+custoPed):"null")+"}";
}

string OrigemPermitida(){
    const char *valor = std::getenv("ROTAS_CORS_ORIGIN");
    return valor && *valor ? string(valor) : string("http://localhost:5173");
}

struct ControleSeguranca {
    atomic<int> conexoesAtivas{0};
    mutex trava;
    unordered_map<string, pair<chrono::steady_clock::time_point, int>> requisicoes;

    bool Permitir(const string &ip){
        lock_guard<mutex> lock(trava);
        auto agora = chrono::steady_clock::now();
        if(requisicoes.size() > 10000){
            for(auto it = requisicoes.begin(); it != requisicoes.end(); ){
                if(agora - it->second.first >= chrono::minutes(2)) it = requisicoes.erase(it);
                else ++it;
            }
        }
        auto &registro = requisicoes[ip];
        if(registro.second == 0 || agora - registro.first >= chrono::minutes(1)){
            registro.first = agora; registro.second = 1; return true;
        }
        if(registro.second >= 120) return false;
        ++registro.second; return true;
    }
};

http::response<http::string_body> Resposta(http::status status, const string &body,
        unsigned version, bool keepAlive, const string &origin){
    http::response<http::string_body> resposta{status, version};
    resposta.set(http::field::server, "RotasGoias-API/2.0");
    resposta.set(http::field::content_type, "application/json; charset=utf-8");
    resposta.set(http::field::access_control_allow_origin, origin);
    resposta.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
    resposta.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    resposta.set(http::field::vary, "Origin");
    resposta.keep_alive(keepAlive);
    resposta.body() = body;
    resposta.prepare_payload();
    return resposta;
}

void TratarCliente(tcp::socket socket, Dados &d, ControleSeguranca &controle){
    beast::error_code ec;
    auto endpoint = socket.remote_endpoint(ec);
    string ip = ec ? "unknown" : endpoint.address().to_string();
    int anterior = controle.conexoesAtivas.fetch_add(1);
    if(anterior >= 32){
        controle.conexoesAtivas.fetch_sub(1);
        beast::error_code ignorar; socket.shutdown(tcp::socket::shutdown_both, ignorar); socket.close(ignorar); return;
    }
    struct Guarda { ControleSeguranca &c; ~Guarda(){ c.conexoesAtivas.fetch_sub(1); } } guarda{controle};

    beast::tcp_stream stream(std::move(socket));
    stream.expires_after(chrono::seconds(10));
    beast::flat_buffer buffer;
    http::request_parser<http::string_body> parser;
    parser.body_limit(64 * 1024);
    http::read(stream, buffer, parser, ec);
    if(ec){
        if(ec == http::error::body_limit){
            auto resposta = Resposta(http::status::payload_too_large, ErroJSON("Corpo da requisicao excede 64 KB."), 11, false, OrigemPermitida());
            http::write(stream, resposta, ec);
        }
        beast::error_code ignorar; stream.socket().shutdown(tcp::socket::shutdown_both, ignorar); stream.socket().close(ignorar); return;
    }
    auto req = parser.release();
    string origin;
    auto originHeader = req.find(http::field::origin);
    string cors = OrigemPermitida();
    if(originHeader != req.end()) origin = string(originHeader->value());
    string corsResposta = cors == "*" ? "*" : (origin == cors ? cors : "null");
    http::response<http::string_body> resposta;

    if(req.method() == http::verb::options){
        resposta = Resposta(http::status::no_content, "", req.version(), req.keep_alive(), corsResposta);
    } else if(!controle.Permitir(ip)){
        resposta = Resposta(http::status::too_many_requests, ErroJSON("Limite de requisicoes excedido. Tente novamente em um minuto."), req.version(), req.keep_alive(), corsResposta);
    } else {
        string caminho = string(req.target());
        size_t q = caminho.find('?'); if(q != string::npos) caminho.resize(q);
        string resultado; http::status status = http::status::ok;
        if(req.method() == http::verb::get && caminho == "/api/health"){
            resultado = "{\"status\":\"ok\",\"service\":\"RotasGoias API\",\"version\":\"2.0.0\"}";
        } else if(req.method() == http::verb::get && caminho == "/api/cities"){
            resultado = JSONCidades(d);
        } else if(req.method() == http::verb::get && caminho == "/api/cities/inactive"){
            resultado = "{\"cities\":[],\"message\":\"O bloqueio e enviado por requisicao em blockedCities; nao existe bloqueio persistente no servidor.\"}";
        } else if(req.method() == http::verb::get && caminho == "/api/docs"){
            resultado = "{\"baseUrl\":\"configurada pelo ambiente\",\"endpoints\":[{\"method\":\"GET\",\"path\":\"/api/health\"},{\"method\":\"GET\",\"path\":\"/api/cities\"},{\"method\":\"POST\",\"path\":\"/api/route\"},{\"method\":\"POST\",\"path\":\"/api/route/stops\"}]}";
        } else if(req.method() == http::verb::post && (caminho == "/api/route" || caminho == "/api/route/stops")){
            auto tipo = req.find(http::field::content_type);
            if(tipo == req.end() || string(tipo->value()).find("application/json") == string::npos){
                status = http::status::unsupported_media_type; resultado = ErroJSON("Envie Content-Type: application/json.");
            } else {
                string erro;
                if(!ValidarJSONRequisicao(req.body(), erro)){ status = http::status::bad_request; resultado = ErroJSON(erro); }
                else {
                    try{
                        resultado = caminho == "/api/route" ? RotaBasica(d, req.body()) : RotaComParadas(d, req.body());
                        if(resultado.find("\"error\"") != string::npos) status = http::status::bad_request;
                    }catch(const std::exception &){
                        status = http::status::internal_server_error;
                        resultado = ErroJSON("Erro interno ao calcular a rota.");
                    }catch(...){
                        status = http::status::internal_server_error;
                        resultado = ErroJSON("Erro interno ao calcular a rota.");
                    }
                }
            }
        } else {
            status = http::status::not_found; resultado = ErroJSON("Endpoint nao encontrado. Consulte GET /api/docs.");
        }
        resposta = Resposta(status, resultado, req.version(), req.keep_alive(), corsResposta);
    }
    http::write(stream, resposta, ec);
    beast::error_code ignorar; stream.socket().shutdown(tcp::socket::shutdown_both, ignorar); stream.socket().close(ignorar);
}
}

int IniciarAPI(int porta){
    try{
        net::io_context contexto{1};
        tcp::acceptor acceptor{contexto, {net::ip::make_address("0.0.0.0"), static_cast<unsigned short>(porta)}};
        Dados dados;
        if(!dados.carregar()){
            cerr << "Nao foi possivel carregar os arquivos em data/. Execute a API com a pasta de dados configurada.\n";
            return 1;
        }
        ControleSeguranca controle;
        cout << "RotasGoias API ativa na porta " << porta << "\n";
        cout << "CORS permitido: " << OrigemPermitida() << "\n";
        cout << "Endpoints: /api/health, /api/cities, /api/route, /api/route/stops\n";
        while(true){
            tcp::socket socket{contexto};
            acceptor.accept(socket);
            std::thread(TratarCliente, std::move(socket), std::ref(dados), std::ref(controle)).detach();
        }
    }catch(const std::exception &e){
        cerr << "Falha ao iniciar a API: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

#endif // ROTAS_API_USE_BOOST
