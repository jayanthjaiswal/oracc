#!/bin/sh
if [ "$ORACC_MODE" = "single" ]; then
    exit 0
fi
x=`id -Gn | grep oracc`
if [ "$x" != "" ]; then
    exit 0
fi
echo agg: user `whoami` is not permitted to run aggregation scripts. Stop.
