# X2ServerRework

Porte dos servidores (e, mais adiante, do cliente) de **Elsword** para **Windows e Linux**, em 64 bits.

O código original só roda no Windows: MFC, DirectX 9 e MSVC de 32 bits. Aqui ele passa para C++20 e CMake, com
Clang nas duas plataformas, e é limpo no caminho. O **protocolo continua o do cliente original**: os servidores portados
funcionam com o `elsword.exe` de sempre, sem mudar nada nele nem nos assets dele.

## Estado (2026-10-31)

| Servidor | Função | Linux | Windows |
|---|---|---|---|
| EPublisherServer | login e cash | roda | compila |
| ChannelServer | primeiro contato do cliente: login, servidores e canais | roda | compila |
| LoginServer | contas, guildas, rankings | roda | compila |
| CenterServer | salas de campo e de dungeon | roda | compila |
| GameServer | personagens, vila, campos, dungeons, itens, quests, PvP | roda | compila |
| GlobalServer | missões globais, quest do dia, contagem de usuários | em porte | em porte |

Com o cliente original, já funcionam o login, a seleção de personagem, a vila, o campo e a dungeon.
No Windows tudo compila sem warnings e os testes passam, mas os servidores ainda não foram testados rodando lá.

## Compilar

- [Linux](docs/building-linux.md): pelo container Docker do projeto (recomendado) ou nativo, com os pacotes de cada distribuição.
- [Windows](docs/building-windows.md): Visual Studio 2026 com o clang-cl.

## Rodar

[Rodar os servidores](docs/running.md): o banco de dados (SQL Server em Docker), as configurações locais
(`docker/.env`), as pastas que o build monta, a ordem dos servidores e o cliente.

O cliente original e os backups do banco não fazem parte do repositório.

## Próximos passos

O GlobalServer, um terminal novo para os servidores e o **ServerManager**, uma ferramenta para orquestrar e monitorar os
servidores, local e por SSH. Depois vem o porte do cliente para SDL3 + SDL_GPU. Tudo isso está no [Roadmap](docs/projeto/roadmap.md).

## Documentação

O [wiki do projeto](docs/README.md) tem os detalhes: decisões, andamento, notas de cada módulo portado e a análise do
código original. Quem for mexer no código começa pelo [AGENTS.md](AGENTS.md), com as regras do repositório (estilo,
branches, o que não se muda).
