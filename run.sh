#!/bin/bash

jobID=$1
echo "JobID=$jobID"

# Generate a random seed for the job
# # crypto quality random number - should also work in parallel since thread safe
trandom="$(od -vAn -N4 -tu4 < /dev/urandom | tr -d '[:space:]')"

./runStarSim.sh $trandom

mv pythia6.starsim.root starsim_${jobID}_${trandom}.GenTree.root
mv pythia6.starsim.fzd starsim_${jobID}_${trandom}.GEANT.fzd

./runBFC.sh starsim_${jobID}_${trandom}.GEANT.fzd

mv FemtoDst.root starsim_${jobID}_${trandom}.FemtoDst.root