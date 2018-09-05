#!/bin/sh
set -e

DIR=$(cd "$(dirname "$0")" && pwd)
DBFILE="$DIR/boat.db"

print_result() {
if $1; then
printf 'Success\n'
else
printf 'Failed\n'
fi
}

if [ -f "$DBFILE" ]; then
rm -v "$DBFILE"
fi

printf 'Creating database in %s\n' "$DBFILE"
if sqlite3 "$DBFILE" <"$DIR/createtables.sql"; then
print_result true
else
print_result false
fi

printf "Server connection HTTPsync configuration\\n"
printf 'Enter server address (URL): '
read -r SRVADDR
printf 'Enter boat ID: '
read -r BOATID
printf 'Enter boat password: '
read -r BOATPWD

printf 'Storing configuration into %s\n' "$DBFILE"
if sqlite3 "$DBFILE" "INSERT INTO config_httpsync(id, loop_time, remove_logs, push_only_latest_logs, boat_id, boat_pwd, srv_addr) VALUES('1', '0.5', '0', '1', '$BOATID', '$BOATPWD', '$SRVADDR')"
then
print_result true
else
print_result false
fi
