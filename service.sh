#!/bin/sh

script=$(readlink -f "$0")
basedir=$(dirname "$script")

action=$1
shift

PIDFILE=$basedir/server.pid
LOGFILE=$basedir/server.log

EXEC="$basedir/server.out"

case "$action" in
    start)
        /sbin/start-stop-daemon --chdir "$basedir" --pidfile "$PIDFILE" --start --background --make-pidfile --oknodo --exec "$EXEC" -- "$@"
        ;;
    stop)
        /sbin/start-stop-daemon --pidfile "$PIDFILE" --stop --signal TERM --oknodo --exec "$EXEC"
        ;;
    restart)
        "$script" stop
        "$script" start
        ;;
    nodaemon)
        "$EXEC" "$@"
        ;;
    *)
        echo "Unknown action: $action"
        ;;
esac
