

class StMaker;

void bfcRunner( int nevt, char* chain_opts, char* filename ){

	gROOT->LoadMacro("./bfc.C");
	bfc( -1, chain_opts, filename );

	gSystem->Load("FemtoDstFormat");
	gSystem->Load("McFemtoDstWriter");

	// needed since I use the StMuTrack
	gSystem->Load("StarClassLibrary");
	gSystem->Load("StStrangeMuDstMaker");
	gSystem->Load("StMuDSTMaker");

	// StMcEventMaker *mcEvent = new StMcEventMaker();
	// StAssociationMaker *association = new StAssociationMaker( );       // TPC association maker
	// association->SetDebug(0);

	McFemtoDstWriter *fmtWriter = new McFemtoDstWriter( "MtdMc");
	fmtWriter->SetDebug(0);
	fmtWriter->setOutputFileName( "FemtoDst.root" );

	
	// chain->AddAfter("MuDst", mcEvent );
	// chain->AddAfter("StMcEvent", association );
	// cout << "ADDING fmtWriter = " << chain->AddAfter("StMcEventMaker", fmtWriter ) << endl;
	cout << "ADDING fmtWriter = " << chain->AddAfter("StAssociationMaker", fmtWriter ) << endl;

	StMaker::lsMakers(chain);
	chain->PrintInfo();
	chain->Init();
	chain->EventLoop(nevt);
	chain->Finish();

	delete chain;
}
