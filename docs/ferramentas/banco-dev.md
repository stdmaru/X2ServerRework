[Início](../README.md) › [Ferramentas](README.md) › Banco de desenvolvimento

# Banco de desenvolvimento (SQL Server em Docker)

SQL Server 2022 local para testar o servidor e o `KODBC` ([Banco de dados (ODBC)](../porte/server-common-db.md)), com os
bancos do jogo restaurados dos backups em `_database/` (fora do git).

## Bancos (2026-10-30)

| Backup | Banco | Conteúdo |
|---|---|---|
| `Account.bak` | `Account` | contas (`MUser`: 5 usuários), 31 tabelas, 103 procedures |
| `Game01.bak` | `Game01` | personagens, itens, quests (`GUnit`, `GItem`...), 145 tabelas, 445 procedures |
| `ES_BILLING.bak` | `ES_BILLING` | loja/cobrança (`EB_Product`: 8.193 produtos), 40 tabelas, 62 procedures |
| `Statistics.bak` | `Statistics` | logs e estatísticas, 168 tabelas, 333 procedures |

Collation `Latin1_General_CI_AS`, nível de compatibilidade 100 (SQL Server 2008). Nos `.dsn` do servidor original os bancos têm
outros nomes (`DATABASE=Elsword-Account`...): ao rodar um servidor contra este banco, ajuste o `DATABASE` dos `.dsn`.

## Uso

```bash
docker compose -f docker/docker-compose.yml up -d --wait     # sobe (cria o volume na primeira vez)
docker/mssql/restore.sh                                       # restaura todos os _database/*.bak (substitui os bancos)
docker compose -f docker/docker-compose.yml stop              # para; os dados ficam no volume x2-mssql-data
docker compose -f docker/docker-compose.yml down -v           # apaga o container e o volume
```

- Escuta só em `127.0.0.1:1433` (`X2_DB_PORT` muda a porta). Usuário `sa`; a senha padrão de desenvolvimento está no
  `docker/docker-compose.yml` e muda com `X2_DB_PASSWORD` (antes do primeiro `up`, e também no `restore.sh`).
- O `restore.sh` copia os `.bak` para dentro do container (o SQL Server roda como uid 10001, que pode não conseguir ler os arquivos montados)
  e restaura cada um com `MOVE` dos arquivos lógicos. Aceita arquivos específicos: `docker/mssql/restore.sh _database/Game01.bak`.
- `sqlcmd` dentro do container: `docker exec -it x2-mssql /opt/mssql-tools18/bin/sqlcmd -C -S localhost -U sa -P '<senha>'`.

## Connection strings

FreeTDS (pacote `freetds`; driver em `/usr/lib/libtdsodbc.so`, sem registrar no `odbcinst.ini`):

```
DRIVER=/usr/lib/libtdsodbc.so;SERVER=127.0.0.1,1433;UID=sa;PWD=<senha>;TDS_Version=7.4;ClientCharset=UTF-8
```

ODBC Driver 18 (`msodbcsql18`, AUR; não testado em 2026-10-30):

```
DRIVER={ODBC Driver 18 for SQL Server};SERVER=127.0.0.1,1433;UID=sa;PWD=<senha>;Encrypt=yes;TrustServerCertificate=yes
```

## Testes do `KODBC` contra o banco

A suíte `db` de `x2_server_common_tests` roda quando o CMake recebe a connection string (sem `DATABASE`; os testes escolhem o banco):

```bash
cmake --preset linux-debug -DX2_TEST_ODBC="DRIVER=/usr/lib/libtdsodbc.so;SERVER=127.0.0.1,1433;UID=sa;PWD=<senha>;TDS_Version=7.4;ClientCharset=UTF-8"
ctest --preset linux-debug -R server_common_db
```

Os testes só leem dados (e criam uma procedure temporária `#x2_test_sp`, que some com a conexão). Precisam dos backups restaurados:
um deles lê os personagens do usuário 1 do `Game01`.
