#!/bin/bash

echo "RANDOM SEED=$1"
echo "Nevents=$2"
root4star -b -q -l 'starsim.C( '"$2"', '"$1"' )'
