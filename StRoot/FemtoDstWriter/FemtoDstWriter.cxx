
#include "FemtoDstFormat/BranchWriter.h"
#include "FemtoDstWriter.h"

#include "StThreeVectorF.hh"

#include "StEvent.h"
#include "StTrack.h"
#include "StGlobalTrack.h"
#include "StTrackGeometry.h"
#include "StDcaGeometry.h"
#include "StMtdPidTraits.h"
#include "StBTofPidTraits.h"
#include "StPhysicalHelixD.hh"
#include "StThreeVectorD.hh"
#include "StEvent/StTrackNode.h"
#include "StEvent/StGlobalTrack.h"
#include "StEvent/StRunInfo.h"
#include "StEvent/StEventInfo.h"
#include "StEvent/StPrimaryVertex.h"
#include "StEvent/StBTofHeader.h"


#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuMtdHit.h"
#include "StMuDSTMaker/COMMON/StMuMtdPidTraits.h"



#include "StarClassLibrary/StParticleDefinition.hh"

// STL
#include <vector>
#include <map>
#include <algorithm>

ClassImp(FemtoDstWriter)


FemtoDstWriter::FemtoDstWriter( const Char_t *name ) : StMaker( name )
{
	this->_outputFilename = "FemtoDst.root";

	SetDebug(1);
}

FemtoDstWriter::~FemtoDstWriter()
{

}

Int_t FemtoDstWriter::Init()
{

	LOG_INFO << "INITIALIZE" <<endm;
	this->_rootFile = new TFile( this->_outputFilename.c_str(), "RECREATE" );
	this->_rootFile->cd();
	this->_tree = new TTree( "FemtoDst", "FemtoDst with MC info" );

	// Create Event Branch
	_few.createBranch( this->_tree, "Event" );
	_ftw.createBranch( this->_tree, "Tracks" );
	_fmtdw.createBranch( this->_tree, "MtdPidTraits" );
	_fbtofw.createBranch( this->_tree, "BTofPidTraits" );
	this->_fhw.createBranch( this->_tree, "Helices" );


	return kStOK;
}

Int_t FemtoDstWriter::Finish()
{
	LOG_INFO << "FINISH" <<endm;
	this->_rootFile->Write();
	this->_rootFile->Close();
	return kStOK;
}

Int_t FemtoDstWriter::Make()
{
	LOG_DEBUG << "FemtoDstWriter::Make()" << endm;
	
	// Check that the DS are present
	StMuDstMaker *muDstMaker = (StMuDstMaker*) GetMaker( "MuDst" );
	if (nullptr == this->_StMuEvent) {
		LOG_INFO << "No StMuDstMaker Found, cannot proceed" << endm;
		return kStWarn;
	}
	this->_StMuDst = muDstMaker->muDst();
	if (nullptr == this->_StMuDst) {
		LOG_INFO << "No StMuDst Found, cannot proceed" << endm;
		return kStWarn;
	}

	this->_StMuEvent = this->_StMuDst->event();
	if (nullptr == this->_StMuEvent) {
		LOG_INFO << "No StMuEvent Found, cannot proceed" << endm;
		return kStWarn;
	}
	
	

	bool passTrigger = this->_StMuEvent->triggerIdCollection().nominal().isTrigger( 490001 ); //VPDMB-5-ssd
	passTrigger |= this->_StMuEvent->triggerIdCollection().nominal().isTrigger( 490006 ); // VPDMB-5-nossd
	passTrigger |= this->_StMuEvent->triggerIdCollection().nominal().isTrigger( 490904 ); // VPDMB-30

	if ( false == passTrigger ){
		LOG_INFO << "REJECT TRIGGER" << endm;
		LOG_INFO << "=============" << endm;
		for ( size_t i = 0; i < this->_StMuEvent->triggerIdCollection().nominal().triggerIds().size(); i++ ){
			LOG_INFO  << "trigger : " << this->_StMuEvent->triggerIdCollection().nominal().triggerIds()[i] << endm;
		}
		return kStOK;
	}

	// Fill Event Info
	this->_fmtEvent.mRunId   = this->_StMuEvent->runId();
	this->_fmtEvent.mEventId = this->_StMuEvent->eventId();

	if ( nullptr == this->_StMuDst->primaryVertex() ){
		LOG_INFO << "No Primary Vertex Found, skipping event" << endm;
		return kStWarn;
	}
	this->_pvPosition = this->_StMuDst->primaryVertex()->position();
	this->_fmtEvent.vertex( this->_pvPosition.x(), this->_pvPosition.y(), this->_pvPosition.z() );
	
	if ( nullptr != this->_StMuDst->btofHeader ){
		this->_fmtEvent.mWeight = this->_StMuDst->btofHeader()->vpdVz();
		LOG_DEBUG << "vpdVz = " << this->_StMuDst->btofHeader()->vpdVz() << endm;
	}
	_few.set( this->_fmtEvent );



	// event cuts
	if ( fabs( this->_fmtEvent.mPrimaryVertex_mX3 ) > 100 || fabs( this->_fmtEvent.mPrimaryVertex_mX3 - this->_fmtEvent.mWeight ) >= 6 ){
		// LOG_INFO << "Vz = " << this->_fmtEvent.mPrimaryVertex_mX3 << endm;
		// LOG_INFO << "VPD - Vz = " << this->_fmtEvent.mPrimaryVertex_mX3 - this->_fmtEvent.mWeight << endm;
		return kStOK;
	}

	

	

	// RESET containers
	this->_ftw.reset();
	this->_fhw.reset();
	this->_fmtdw.reset();
	this->_fbtofw.reset();

	size_t nPrimary = this->_StMuDst->numberOfPrimaryTracks();

	for (int iNode = 0; iNode < nPrimary; iNode++ ){
		
		StMuTrack*	tPrimary 	= (StMuTrack*)this->_StMuDst->primaryTracks(iNode);
		if ( !tPrimary ) continue;

		StMuTrack*	tGlobal 	= (StMuTrack*)tPrimary->globalTrack();

		// analyzeTrack( iNode, nPrimaryGood );
		
		this->_fmtTrack.reset();
		addMtdPidTraits( tPrimary );
		addBTofPidTraits( tPrimary );
		// addTrackHelix( track );
		addTrack( tPrimary );
	}

	this->_tree->Fill();

	return kStOK;

}

void FemtoDstWriter::addTrack( StMuTrack *muTrack )
{

	if ( nullptr == muTrack ){
		LOG_INFO << "WARN, null StMuTrack" << endm;
		return;
	}
	// LOG_INFO << "nHitsPoss = " << globalTrack->numberOfPossiblePoints() << " == " << muTrack->nHitsPoss() << endm << endl;
	// LOG_INFO << "nHitsFit = " << globalTrack->fitTraits().numberOfFitPoints() << " == " << muTrack->nHitsFit() << endm << endl;
	// LOG_INFO << "dEdx = " << muTrack->dEdx() << endm;

	this->_fmtTrack.mNHitsMax = muTrack->nHitsPoss();
	this->_fmtTrack.mNHitsFit = muTrack->nHitsFit() * muTrack->charge();
	this->_fmtTrack.mNHitsDedx = muTrack->nHitsDedx();

	this->_fmtTrack.dEdx( muTrack->dEdx() * 1.e6 );
	this->_fmtTrack.nSigmaElectron( muTrack->nSigmaElectron() );
	this->_fmtTrack.nSigmaPion( muTrack->nSigmaPion() );
	this->_fmtTrack.nSigmaKaon( muTrack->nSigmaKaon() );
	this->_fmtTrack.nSigmaProton( muTrack->nSigmaProton() );

	StThreeVectorF p = muTrack->p();
	this->_fmtTrack.mPt  = p.perp();
	this->_fmtTrack.mEta = p.pseudoRapidity();
	this->_fmtTrack.mPhi = p.phi();

	this->_fmtTrack.mId = muTrack->id() - 1; // id starts at 1
	
	this->_fmtTrack.gDCA( muTrack->dcaGlobal().mag() );

	this->_ftw.add( this->_fmtTrack );
}


double FemtoDstWriter::calculateDCA(StGlobalTrack *globalTrack, StThreeVectorF vtxPos) const
{
	if( nullptr == globalTrack ) 
		return 999;
	
	StDcaGeometry	*dcaGeom = globalTrack->dcaGeometry();
	
	if( nullptr == dcaGeom ) 
		return 999;
	
	StPhysicalHelixD dcaHelix = dcaGeom->helix();
	return dcaHelix.distance(vtxPos,kFALSE);
}

void FemtoDstWriter::addMtdPidTraits( StMuTrack *muTrack )
{
	const StMuTrack *global = muTrack->globalTrack();
	StMuMtdPidTraits mtdPid = global->mtdPidTraits();
	const StMuMtdHit *hit = global->mtdHit();

	if ( nullptr == hit ){
		return;
	}

	this->_fmtMtdPid.reset();
	this->_fmtMtdPid.mDeltaY            = mtdPid.deltaY();
	this->_fmtMtdPid.mDeltaZ            = mtdPid.deltaZ();
	this->_fmtMtdPid.mMatchFlag         = mtdPid.matchFlag();
	this->_fmtMtdPid.mDeltaTimeOfFlight = mtdPid.timeOfFlight() - mtdPid.expTimeOfFlight();
	this->_fmtMtdPid.mMtdHitChan        = (hit->backleg() - 1) * 60 + (hit->module() - 1) * 12 + hit->cell();
	this->_fmtMtdPid.mIdTruth           = hit->idTruth() - 1;	// minus one to index at 0

	this->_fmtTrack.mMtdPidTraitsIndex = this->_fmtdw.N();

	this->_fmtdw.add( this->_fmtMtdPid );
}


void FemtoDstWriter::addBTofPidTraits( StMuTrack *muTrack )
{
	

	// StTrack *track = node->track( primary );
	// if ( nullptr == track ) 
	// 	return;
	// StPtrVecTrackPidTraits traits = track->pidTraits(kTofId);
	
	// if ( traits.size() <= 0 )
	// 	return;

	// StBTofPidTraits *btofPid = dynamic_cast<StBTofPidTraits*>(traits[0]);
	// if ( nullptr == btofPid ){
	// 	LOG_INFO << "WARN, null BTofPidTraits" << endm;
	// 	return;
	// }

	// if ( Debug() ) {
	// 	LOG_INFO << "MC BTofHit? " << btofPid->tofHit()->idTruth() << endm;
	//  	LOG_INFO << "MC BTofHit? QA = " << btofPid->tofHit()->qaTruth() << endm;
	
	// 	if ( btofPid->tofHit()->associatedTrack() ){
	// 		LOG_INFO << "MC BTofHit Track " << btofPid->tofHit()->associatedTrack()->key() << endm;
	// 		LOG_INFO << "this track keey = " << track->key() << endm;
	// 	}
	// }

	// StBTofHit *hit = btofPid->tofHit();

	// this->_fmtBTofPid.reset();
	// this->_fmtBTofPid.yLocal (btofPid->yLocal() );
	// this->_fmtBTofPid.zLocal( btofPid->zLocal() );
	// this->_fmtBTofPid.matchFlag( btofPid->matchFlag() );
	// this->_fmtBTofPid.mIdTruth  =  btofPid->tofHit()->idTruth() - 1; // minus 1 to index at 0
	
	// double b = btofPid->beta();
	// if ( b < 0 )
	// 	b = 0;
	// this->_fmtBTofPid.beta( b );


	// this->_fmtTrack.mBTofPidTraitsIndex = this->_fbtofw.N();

	// this->_fbtofw.add( this->_fmtBTofPid );

}
