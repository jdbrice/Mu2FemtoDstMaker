#!/bin/bash

echo "RANDOM SEED=$1"
root4star -b -q -l 'starsim.C( 5, '"$1"' )'
