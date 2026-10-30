#!/usr/bin/env bash
# Restores every _database/<name>.bak as database <name> in the x2-mssql container (replacing it).
#   docker/mssql/restore.sh [file.bak...]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
NAME=x2-mssql
PASSWORD="${X2_DB_PASSWORD:-X2dev!Passw0rd}"
DIR=/var/opt/mssql/backup

sqlcmd() {
    docker exec "$NAME" /opt/mssql-tools18/bin/sqlcmd -C -S localhost -U sa -P "$PASSWORD" -b -h -1 -W "$@"
}

for _ in $(seq 60); do
    sqlcmd -Q "SELECT 1" >/dev/null 2>&1 && break
    sleep 1
done

if [ $# -eq 0 ]; then set -- "$ROOT"/_database/*.bak; fi

# copied in, not mounted: the server runs as uid 10001, which may not be able to read the files
docker exec -u 0 "$NAME" mkdir -p "$DIR"
for bak in "$@"; do
    db="$(basename "$bak" .bak)"
    docker cp -q "$bak" "$NAME:$DIR/$db.bak"
    docker exec -u 0 "$NAME" chown mssql "$DIR/$db.bak"

    # each logical file of the backup goes to the container's data folder
    moves=""
    while IFS='|' read -r logical type; do
        [ -n "$logical" ] || continue
        ext=mdf; [ "$type" = "L" ] && ext=ldf
        moves+=", MOVE N'$logical' TO N'/var/opt/mssql/data/${db}_${logical}.$ext'"
    done < <(sqlcmd -s '|' -Q "SET NOCOUNT ON; DECLARE @t TABLE (LogicalName nvarchar(128), PhysicalName nvarchar(260), Type char(1),
                FileGroupName nvarchar(128), Size numeric(20,0), MaxSize numeric(20,0), FileId bigint, CreateLSN numeric(25,0),
                DropLSN numeric(25,0), UniqueId uniqueidentifier, ReadOnlyLSN numeric(25,0), ReadWriteLSN numeric(25,0),
                BackupSizeInBytes bigint, SourceBlockSize int, FileGroupId int, LogGroupGUID uniqueidentifier,
                DifferentialBaseLSN numeric(25,0), DifferentialBaseGUID uniqueidentifier, IsReadOnly bit, IsPresent bit,
                TDEThumbprint varbinary(32), SnapshotUrl nvarchar(360));
                INSERT @t EXEC('RESTORE FILELISTONLY FROM DISK = N''$DIR/$db.bak''');
                SELECT LogicalName, Type FROM @t;")

    echo "restoring $db"
    sqlcmd -Q "RESTORE DATABASE [$db] FROM DISK = N'$DIR/$db.bak' WITH REPLACE$moves" >/dev/null
done

# The servers find themselves in Statistics.LServerList by address (SetLocalIP in the configs), and the Channel
# Server sends the game server addresses of Statistics.LChannelList to the client. The backup has the
# production addresses: point both to X2_SERVER_IP (only data changes, the schema stays as it is).
for bak in "$@"; do
    if [ "$(basename "$bak" .bak)" = Statistics ]; then
        ip="${X2_SERVER_IP:-127.0.0.1}"
        echo "LServerList -> $ip"
        sqlcmd -d Statistics -Q "UPDATE dbo.LServerList SET PublicIP = N'$ip', PrivateIP = N'$ip', IP = N'$ip';
                                 UPDATE dbo.LChannelList SET IP = N'$ip'" >/dev/null
    fi
done
