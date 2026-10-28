# Publicar a API RotasGoias no Render

O arquivo `render.yaml` contém um Blueprint para construir o backend a partir do `Dockerfile`.

1. Crie um repositório privado no GitHub para este projeto e envie o conteúdo do ZIP, sem dados pessoais.
2. No Render, crie um novo Blueprint e conecte esse repositório.
3. Revise o serviço `rotasgoias-api` e faça o deploy.
4. Quando o deploy terminar, abra `https://SEU-SERVICO.onrender.com/api/health`. Deve aparecer JSON com `"status":"ok"`.
5. Copie a URL HTTPS do serviço e configure-a no frontend do Lovable como `VITE_ROTAS_API_URL` (sem adicionar `/api` ao final).
6. Após publicar o site, troque `ROTAS_CORS_ORIGIN` de `*` para o domínio HTTPS exato do site Lovable e faça novo deploy.

**Limitações:** não consigo entrar na sua conta GitHub, Render ou Lovable nem realizar cliques/publicações por você a partir desta conversa. O pacote deixa os arquivos prontos; a criação do repositório e a autorização do serviço exigem sua conta. O plano gratuito do Render pode suspender o serviço após inatividade e o primeiro pedido pode demorar para responder.

O arquivo de usuários é excluído do build Docker e está vazio no ZIP. Não o preencha com CPFs reais em um repositório ou imagem pública. Cadastro e consulta de CPF permanecem fora desta etapa.
