#ifndef MC_FEMTO_DST_WRITER_H
#define MC_FEMTO_DST_WRITER_H

#include <string>

// StRoot
#include "StMaker.h"
#include "StAssociationMaker/StAssociationMaker.h"

// ROOT
#include "TTree.h"
#include "TFile.h"
// Project
#include "FemtoDstFormat/FemtoEvent.h"
#include "FemtoDstFormat/FemtoTrack.h"
#include "FemtoDstFormat/FemtoTrackHelix.h"
#include "FemtoDstFormat/FemtoMcTrack.h"
#include "FemtoDstFormat/FemtoMcVertex.h"
#include "FemtoDstFormat/FemtoMtdPidTraits.h"
#include "FemtoDstFormat/FemtoBTofPidTraits.h"
#include "FemtoDstFormat/BranchWriter.h"
#include "FemtoDstFormat/TClonesArrayWriter.h"


class StEvent;
class StTrack;
class StTrackNode;
class StMcEvent;
class StAssociationMaker;
class StMcTrack;

class McFemtoDstWriter : public StMaker 
{
public:
	McFemtoDstWriter( const Char_t *name = "McFemtoDstWriter" );
	~McFemtoDstWriter();
	
	Int_t Init();
	Int_t Make();
	Int_t Finish();

	void setOutputFileName( std::string fn){
		this->_outputFilename = fn;
	}

protected:

	std::string _outputFilename;

	void addTrack( StTrackNode * track );
	void addTrackHelix( StTrackNode * track );
	void addMcTrack( StMcTrack * mcTrack, StTrack *rcTrack );
	void addMcVertex( StMcVertex * mcVertex );

	void addMtdPidTraits( StTrackNode * track );
	void addBTofPidTraits( StTrackNode * track );

	TTree *_tree;
	TFile *_rootFile;

	FemtoEvent         _fmtEvent;
	FemtoTrack         _fmtTrack;
	FemtoTrackHelix    _fmtHelix;
	FemtoMcTrack       _fmtMcTrack;
	FemtoMcVertex      _fmtMcVertex;
	FemtoMtdPidTraits  _fmtMtdPid;
	FemtoBTofPidTraits _fmtBTofPid;

// The Branch Writers
#ifndef __CINT__	// gets confused by std::shared_ptr<>
	BranchWriter<FemtoEvent> _few;
	TClonesArrayWriter<FemtoTrack> _ftw;
	TClonesArrayWriter<FemtoMcTrack> _fmcw;
	TClonesArrayWriter<FemtoMcVertex> _fmcvertw;
	TClonesArrayWriter<FemtoMtdPidTraits> _fmtdw;
	TClonesArrayWriter<FemtoBTofPidTraits> _fbtofw;
	TClonesArrayWriter<FemtoTrackHelix> _fhw;

	StThreeVectorF      _pvPosition;
#endif


	StEvent            *_StEvent;
	StMcEvent          *_StMcEvent;
	StAssociationMaker *_StAssociationMaker;



	rcTrackMapType     *_rcTrackMap = nullptr;
	mcTrackMapType     *_mcTrackMap = nullptr;

	int getMatchedTrackIndex( StMcTrack *mcTrack );

	double calculateDCA(StGlobalTrack *globalTrack, StThreeVectorF vtxPos) const;

	/***************************************************/
	// SHIM, in std starting with c++17
	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper) {
		return std::max(lower, std::min(n, upper));
	}
	/***************************************************/


	ClassDef(McFemtoDstWriter, 1)
};


#endif