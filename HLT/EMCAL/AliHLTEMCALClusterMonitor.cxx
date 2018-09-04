/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors: Francesco Blanco                                      *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          * 
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/**
 * @file   AliHLTEMCALClusterMonitor.cxx
 * @author Francesco Blanco
 * @date 
 * @brief  Online Monitoring Histogram maker for EMCAL  
 */
  

#include "AliHLTEMCALClusterMonitor.h"
#include "AliHLTCaloSharedMemoryInterfacev2.h"
#include "TMath.h"

ClassImp(AliHLTEMCALClusterMonitor);

AliHLTEMCALClusterMonitor::AliHLTEMCALClusterMonitor():
  fClusterReaderPtr(0),
  hList(0),
	hClusterEneEMCAL(0),
	hClusterEneDCAL(0),
	hClusterEneVsTime(0),
	hClusterCells(0),
	hClusterEneVsCells(0),
	hClusterEtaVsPhi(0),
	hClusterM02(0),
	hClusterM20(0)

{
  // See header file for documentation

  fClusterReaderPtr = new AliHLTCaloClusterReader();

  // Booking histograms
  hList = new TObjArray;

	hClusterEneEMCAL = new TH1F("hClusterEneEMCAL", "ClusterEnergy (GeV)", 200, 0, 100);
	hList->Add(hClusterEneEMCAL);
	hClusterEneDCAL = new TH1F("hClusterEneDCAL", "ClusterEnergy (GeV)", 200, 0, 100);
	hList->Add(hClusterEneDCAL);

	hClusterEneVsTime = new TH2F("hClusterEneVsTime", "ClusterEnergy vs Time", 200, 0, 100, 40, -0.5, 39.5);
	hList->Add(hClusterEneVsTime);
	
	hClusterCells = new TH1I("hClusterCells", "# of cells per cluster", 50, -0.5, 49.5);
	hList->Add(hClusterCells);

	hClusterEneVsCells = new TH2F("hClusterEneCells", "# of cells per cluster vs cluster energy", 200, 0, 100, 50, -0.5, 49.5);
	hList->Add(hClusterEneVsCells);

  hClusterM02 = new TH1F("hClusterM02", "Cluster M02", 200, 0, 10);
	hList->Add(hClusterM02);
  hClusterM20 = new TH1F("hClusterM20", "Cluster M20", 200, 0, 10);
	hList->Add(hClusterM20);

	hClusterEtaVsPhi = new TH2F("hClusterEtaVsPhi", "Cluster position in #eta#phi", 100, -0.7, 0.7, 100, 0, TMath::Pi()*2);
	hClusterEtaVsPhi->GetXaxis()->SetTitle("#eta");
	hClusterEtaVsPhi->GetYaxis()->SetTitle("#phi");
	hList->Add(hClusterEtaVsPhi);
	
	
}

AliHLTEMCALClusterMonitor::~AliHLTEMCALClusterMonitor() 
{
  //See header file for documentation
}

// Pointer to histograms objects
TObjArray* AliHLTEMCALClusterMonitor::GetHistograms()
{
  return hList;
}


Int_t AliHLTEMCALClusterMonitor::MakeHisto(AliHLTCaloClusterDataStruct *caloClusterStructPtr, Int_t nClusters)
{
	float clusterEne, clusterTime, clusterEta, clusterPhi, clusterX, clusterY, clusterZ;
  int nCells;
  
  // stuff to handle clusters here
  fClusterReaderPtr->SetMemoryNew(caloClusterStructPtr, nClusters);
  
  // Define second cluster reader/ptr to iterate over two clusters at the same time
  AliHLTCaloClusterReader* clusterReaderPtr2 = new AliHLTCaloClusterReader();
  AliHLTCaloClusterDataStruct* caloClusterStructPtr2 = caloClusterStructPtr;
  clusterReaderPtr2->SetMemoryNew(caloClusterStructPtr2, nClusters);


  while((caloClusterStructPtr = fClusterReaderPtr->NextCluster()) != 0) {
    clusterX = caloClusterStructPtr->fGlobalPos[0];
    clusterY = caloClusterStructPtr->fGlobalPos[1];
    clusterZ = caloClusterStructPtr->fGlobalPos[2];

    nCells = caloClusterStructPtr->fNCells;
    clusterEne = caloClusterStructPtr->fEnergy;
    clusterTime = caloClusterStructPtr->fTOF;
    hClusterEneVsTime->Fill(clusterEne, clusterTime);
    hClusterCells->Fill(nCells);
    hClusterEneVsCells->Fill(clusterEne, nCells);
    float r = TMath::Sqrt(clusterX*clusterX + clusterY*clusterY + clusterZ*clusterZ);
    clusterEta = 0.5*TMath::Log( (r+clusterZ)/(r-clusterZ) );
    clusterPhi = TMath::ATan2(clusterY, clusterX);
    if(clusterPhi < 4)
      hClusterEneEMCAL->Fill(clusterEne);
    else
      hClusterEneDCAL->Fill(clusterEne);

    hClusterEtaVsPhi->Fill(clusterEta, clusterPhi);
    hClusterM02->Fill(caloClusterStructPtr->fM02);
    hClusterM20->Fill(caloClusterStructPtr->fM20);

    // Invariant mass plots
    while((caloClusterStructPtr2 = clusterReaderPtr2->NextCluster()) != 0) {
      //TODO: Fill invariant mass plots
    }

  }
	  

return 0; 
}
