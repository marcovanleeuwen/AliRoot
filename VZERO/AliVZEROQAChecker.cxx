/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/


/*
  Checks the quality assurance. Under construction. 
  By comparing with reference data

*/

// --- ROOT system ---
#include <TClass.h>
#include <TH1F.h> 
#include <TH1I.h> 
#include <TIterator.h> 
#include <TKey.h> 
#include <TFile.h> 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQA.h"
#include "AliQAChecker.h"
#include "AliVZEROQAChecker.h"
//#include "AliCDBEntry.h"
//#include "AliCDBManager.h"

ClassImp(AliVZEROQAChecker)

//__________________________________________________________________
const Double_t AliVZEROQAChecker::Check(AliQA::ALITASK_t index, TObjArray * list) 
{

// Main check function: Depending on the TASK, different checks will be applied
// Check for empty histograms 

//   AliDebug(1,Form("AliVZEROChecker"));
//   AliCDBEntry *QARefRec = AliCDBManager::Instance()->Get("VZERO/QARef/RAW");
//   if( !QARefRec){
//     AliInfo("QA reference data NOT retrieved for QA check...");
//     return 1.;
//   }

//   Check that histos are filled - (FATAL) set if empty
     if(CheckEntries(list) == 0.0){
        return CheckEntries(list);
     }

//   Check for one disk missing (FATAL) or one ring missing (ERROR) in ESDs     
     if(index == AliQA::kESD) {
        return CheckEsds(list);
     } 
     
     return 1.0; 
}
//_________________________________________________________________
Double_t AliVZEROQAChecker::CheckEntries(TObjArray * list) const
{
  
  //  check on the QA histograms on the input list: list
//  list->Print();

  Double_t test = 0.0 ;
  Int_t   count = 0 ; 

  if (list->GetEntries() == 0){  
      test = 1.0; 
      AliInfo(Form("There are NO ENTRIES to be checked..."));
  }
  else {
      TIter next(list) ; 
      TH1 * hdata ;
      count = 0 ; 
      while ( (hdata = dynamic_cast<TH1 *>(next())) ) {
        if (hdata) { 	   
	   Double_t rv = 0.0;
	   if(hdata->GetEntries()>0) rv=1.0;
//	   AliInfo(Form("%s -> %f", hdata->GetName(), rv)); 
	   count++ ;        // number of histos
	   test += rv ;     // number of histos filled
        }
        else{
	   AliError(Form("Data type cannot be processed"));
        }      
      }
      if (count != 0) { 
          if (test==0.0) {
	      AliWarning(Form("Histograms are BOOKED for this specific task, but they are all EMPTY"));
          }
          else {
	      test /= count; 
	  }
      }
  }
  return test ; 
}  

//_________________________________________________________________
Double_t AliVZEROQAChecker::CheckEsds(TObjArray * list) const
{
  
//  check the ESDs for missing disk or ring

//  printf(" Number of entries in ESD list = %d\n", list->GetEntries()); 
//  list->Print();

  Double_t test     = 1.0;     // initialisation to OK
  Int_t    histo_nb =   0; 
  Double_t Mult_V0A = 0.0;
  Double_t Mult_V0C = 0.0;
  Double_t V0A_BB_Ring[4], V0C_BB_Ring[4];
  Double_t V0A_BG_Ring[4], V0C_BG_Ring[4];
  for (Int_t i=0; i<4; i++) { 
       V0A_BB_Ring[i]= V0C_BB_Ring[i]= 0.0;
       V0A_BG_Ring[i]= V0C_BG_Ring[i]= 0.0;
  }  
  TIter next(list) ; 
  TH1 * hdata ;
  
  while ( (hdata = dynamic_cast<TH1 *>(next())) ) {
          if (hdata) {
	      histo_nb++;
	      if (histo_nb == 3) {	         
		  Mult_V0A  = hdata->GetMean();
//		  printf(" Histo ESD number %d; Mean Mult on V0A = %f\n",histo_nb, Mult_V0A);
	      }
	      if (histo_nb == 4) {	         
		  Mult_V0C  = hdata->GetMean();
//		  printf(" Histo ESD number %d; Mean Mult on V0C = %f\n",histo_nb, Mult_V0C);
	      }
	      if (histo_nb == 6) {
	          for (Int_t i=0; i<4; i++) {         
		       V0A_BB_Ring[i]  = hdata->Integral((i*8)+1, (i*8) +8);
//	               printf(" Histo ESD number %d; Ring = %d; BB A %f\n",histo_nb, i, V0A_BB_Ring[i]);		  
		  }	      
	      }
	      if (histo_nb == 7) {
	          for (Int_t i=0; i<4; i++) {         
		       V0A_BG_Ring[i]  = hdata->Integral((i*8)+1, (i*8) +8);
//		       printf(" Histo ESD number %d; Ring = %d; BG A %f\n",histo_nb, i, V0A_BG_Ring[i]);
	          }	      
	      }	      
	      if (histo_nb == 8) {
	          for (Int_t i=0; i<4; i++) {    
		       V0C_BB_Ring[i]  = hdata->Integral((i*8)+1, (i*8) +8);
//		       printf(" Histo ESD number %d; Ring = %d; BB C %f\n",histo_nb, i, V0C_BB_Ring[i]);
	          }
	      }
	      if (histo_nb == 9) {
	          for (Int_t i=0; i<4; i++) {    
		       V0C_BG_Ring[i]  = hdata->Integral((i*8)+1, (i*8) +8);
//		       printf(" Histo ESD number %d; Ring = %d; BG C %f\n",histo_nb, i, V0C_BG_Ring[i]);
	          }
	      }
          } 
  }
  
  if(Mult_V0A == 0.0 || Mult_V0C == 0.0) {
     AliWarning(Form("One of the two disks is missing !") );
     test = 0.0; // bit FATAL set
  }
  if( V0A_BB_Ring[0]+V0A_BG_Ring[0] == 0.0 || 
      V0A_BB_Ring[1]+V0A_BG_Ring[1] == 0.0 || 
      V0A_BB_Ring[2]+V0A_BG_Ring[2] == 0.0 || 
      V0A_BB_Ring[3]+V0A_BG_Ring[3] == 0.0 || 
      V0C_BB_Ring[0]+V0C_BG_Ring[0] == 0.0 || 
      V0C_BB_Ring[1]+V0C_BG_Ring[1] == 0.0 || 
      V0C_BB_Ring[2]+V0C_BG_Ring[2] == 0.0 || 
      V0C_BB_Ring[3]+V0C_BG_Ring[3] == 0.0  ){    
      AliWarning(Form("One ring is missing !") );
      test = 0.1;   // bit ERROR set
  }

  return test ; 
} 
 
//______________________________________________________________________________
void AliVZEROQAChecker::SetQA(AliQA::ALITASK_t index, const Double_t value) const
{
// sets the QA word according to return value of the Check

  AliQA * qa = AliQA::Instance(index);
  
  qa->UnSet(AliQA::kFATAL);
  qa->UnSet(AliQA::kWARNING);
  qa->UnSet(AliQA::kERROR);
  qa->UnSet(AliQA::kINFO);
   	
  if ( value <= 0.0 ) qa->Set(AliQA::kFATAL) ; 
  else if ( value > 0.0 && value <= 0.2 ) qa->Set(AliQA::kERROR) ; 
  else if ( value > 0.2 && value <= 0.5 ) qa->Set(AliQA::kWARNING) ;
  else if ( value > 0.5 && value < 1.0 ) qa->Set(AliQA::kINFO) ; 		
}
  
