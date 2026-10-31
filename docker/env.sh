# Sourced by docker/build.sh and docker/mssql/restore.sh: exports the KEY=VALUE lines of docker/.env (not versioned)
# that are not already in the environment, so a variable set in the shell wins over the file, as in docker compose.
# Only plain lines: no quotes, no `export`, `#` comments.
x2_load_env() {
    local file="$1/docker/.env" key value
    [ -f "$file" ] || return 0
    while IFS='=' read -r key value || [ -n "$key" ]; do
        [[ $key =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]] || continue
        [ -n "${!key+x}" ] || export "$key=${value%$'\r'}"
    done < "$file"
}
