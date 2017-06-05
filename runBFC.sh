#!/bin/bash

echo "Running BFC on $1"
# root4star -b -q -l 'bfc.C( 5, "DbV20160418 y2015b fzin mtdSim tpcX AgML tpcDB TpcHitMover Idst Tree ITTF UseXgeom BAna VFMinuit l3onl emcDY2 fpd trgd ZDCvtx  analysis pxlHit istHit btof mtd mtdCalib BEmcChkStat CorrX OSpaceZ2 OGridLeak3D tpcrs TpxRaw  VFMCE TpxClu big MakeEvent evout GeantOut IdTruth", "'$1'" )'

root4star -b -q -l 'bfcRunner.C( 5, "DbV20160418 y2014a fzin mtdSim tpcX AgML tpcDB TpcHitMover Idst Tree ITTF UseXgeom BAna VFMinuit l3onl emcDY2 fpd trgd ZDCvtx  analysis pxlHit istHit btof mtd mtdCalib BEmcChkStat CorrX OSpaceZ2 OGridLeak3D tpcrs TpxRaw  VFMCE TpxClu big MakeEvent evout GeantOut IdTruth McEvent McAss", "'$1'" )'
