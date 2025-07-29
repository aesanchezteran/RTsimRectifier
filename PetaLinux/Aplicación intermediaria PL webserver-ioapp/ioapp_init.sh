#!/bin/sh

PIPE=/tmp/ws_pipe
PY=/usr/bin/python3
WS_PY=/usr/bin/wsserver.py

case "$1" in
  start)
    [ -p "$PIPE" ] || mkfifo "$PIPE"
    chmod 666 "$PIPE"

    $PY $WS_PY &

    /usr/bin/ioapp &
    ;;
  stop)
    killall ioapp

    killall -q -w -9 python3
    ;;
  *)
    echo "Usage: $0 {start|stop}"
    exit 1
    ;;
esac

exit 0
