#ifndef FEMTO_DST_WRITER_H
#define FEMTO_DST_WRITER_H

#include <string>

// StRoot
#include "StMaker.h"
#include "StThreeVectorF.hh"
#include "StThreeVectorD.hh"


// ROOT
#include "TTree.h"
#include "TFile.h"
// Project
#include "FemtoDstFormat/FemtoEvent.h"
#include "FemtoDstFormat/FemtoTrack.h"
#include "FemtoDstFormat/FemtoTrackHelix.h"
#include "FemtoDstFormat/FemtoMtdPidTraits.h"
#include "FemtoDstFormat/FemtoBTofPidTraits.h"
#include "FemtoDstFormat/BranchWriter.h"
#include "FemtoDstFormat/TClonesArrayWriter.h"


class StEvent;
class StMuEvent;
class StTrack;
class StTrackNode;
class StGlobalTrack;
class StMuDst;
class StMuTrack;


class FemtoDstWriter : public StMaker 
{
public:
	FemtoDstWriter( const Char_t *name = "FemtoDstWriter" );
	~FemtoDstWriter();
	
	Int_t Init();
	Int_t Make();
	Int_t Finish();

	void setOutputFileName( std::string fn){
		this->_outputFilename = fn;
	}

protected:

	std::string _outputFilename;

	void addTrack( StMuTrack * track );
	// void addTrackHelix( StMuTrack * track );
	// void addMcTrack( StMcTrack * mcTrack, StTrack *rcTrack );
	// void addMcVertex( StMcVertex * mcVertex );

	void addMtdPidTraits( StMuTrack * track );
	void addBTofPidTraits( StMuTrack * track );

	TTree *_tree;
	TFile *_rootFile;

	FemtoEvent         _fmtEvent;
	FemtoTrack         _fmtTrack;
	FemtoTrackHelix    _fmtHelix;
	FemtoMtdPidTraits  _fmtMtdPid;
	FemtoBTofPidTraits _fmtBTofPid;

// The Branch Writers
#ifndef __CINT__	// gets confused by std::shared_ptr<>
	BranchWriter<FemtoEvent> _few;
	TClonesArrayWriter<FemtoTrack> _ftw;
	TClonesArrayWriter<FemtoMtdPidTraits> _fmtdw;
	TClonesArrayWriter<FemtoBTofPidTraits> _fbtofw;
	TClonesArrayWriter<FemtoTrackHelix> _fhw;

	StThreeVectorD     _pvPosition;
#endif


	StEvent            *_StEvent;
	StMuEvent          *_StMuEvent;
	StMuDst            *_StMuDst;

	double calculateDCA(StGlobalTrack *globalTrack, StThreeVectorF vtxPos) const;

	/***************************************************/
	// SHIM, in std starting with c++17
	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper) {
		return std::max(lower, std::min(n, upper));
	}
	/***************************************************/


	ClassDef(FemtoDstWriter, 1)
};


#endif