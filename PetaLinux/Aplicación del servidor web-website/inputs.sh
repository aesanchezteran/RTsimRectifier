#!/bin/sh
echo "Content-type: text/plain"
echo ""

# Extraccion de datos desde el HTML
ctrl=$(echo "$QUERY_STRING" | sed -n 's/.*ctrl=\([^&]*\).*/\1/p')
alpha=$(echo "$QUERY_STRING" | sed -n 's/.*alpha=\([^&]*\).*/\1/p')
CONF=$(echo "$QUERY_STRING" | sed -n 's/.*CONF=\([^&]*\).*/\1/p')
R=$(echo "$QUERY_STRING" | sed -n 's/.*R=\([^&]*\).*/\1/p')
L=$(echo "$QUERY_STRING" | sed -n 's/.*L=\([^&]*\).*/\1/p')
E=$(echo "$QUERY_STRING" | sed -n 's/.*E=\([^&]*\).*/\1/p')
VM=$(echo "$QUERY_STRING" | sed -n 's/.*VM=\([^&]*\).*/\1/p')

# Valores default
ctrl=${ctrl:-0}
alpha=${alpha:-0}
CONF=${CONF:-0}
R=${R:-0}
L=${L:-0}
E=${E:-0}
VM=${VM:-0}

#Escrituta de valores en txt
echo "$ctrl $CONF $alpha $R $L $E $VM" > /tmp/inputs.txt

#Confirmacion
echo "Datos enviados con éxito"
