#ifndef ALIITSUHIT_H
#define ALIITSUHIT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////////////////////////////
//
// At the moment the same functionality/data-members as parent AliITShit 
// except the geometry transformation uses AliITSgeomTGeoUp 
//
////////////////////////////////////////////////////////////////////////

#include "TLorentzVector.h"

#include "AliITSMFTHit.h" 
#include "AliRun.h"

class AliITSUHit : public AliITSMFTHit {

 public:
  //
  AliITSUHit();
  AliITSUHit(Int_t shunt, Int_t track, Int_t *vol, Float_t *hits);
  AliITSUHit(Int_t shunt,Int_t track,Int_t *vol,Float_t edep,Float_t tof,TLorentzVector &x,TLorentzVector &x0,TLorentzVector &p);
  AliITSUHit(const AliITSUHit &h);
  AliITSUHit& operator=(const AliITSUHit &h);
  virtual ~AliITSUHit() {}

  void SetModule(Int_t mod){fModule=mod;};
  void SetShunt(Int_t shunt);
  void SetPosition(TLorentzVector &x){fX=x.X();fY=x.Y();fZ=x.Z();}
  void SetStartPosition(TLorentzVector &x){fx0=x.X();fy0=x.Y();fz0=x.Z();}
  void SetTime(Float_t t){fTof = t;}
  void SetStartTime(Float_t t){ft0 = t;}
  void SetStatus(Int_t stat){fStatus = stat;}
  void SetStartStatus(Int_t stat){fStatus0 = stat;}
  void SetEdep(Float_t de){fDestep = de;}
  void SetMomentum(TLorentzVector &p){fPx=p.Px();fPy=p.Py();fPz=p.Pz();}

  void SetChip(Int_t chip) {SetModule(chip);}
  Int_t GetChip()          {return GetModule();}

  Int_t GetLayer() const;
  Int_t GetStave() const;
  Int_t GetHalfStave() const;
  Int_t GetModule() const;  
  Int_t GetChipInModule() const;
  void  GetChipID(Int_t &layer,Int_t &stave,Int_t &sstave, Int_t &mod, Int_t &det) const;
  void  GetPositionL(Float_t &x,Float_t &y,Float_t &z,Float_t &tof);
  void  GetPositionL(Float_t &x,Float_t &y,Float_t &z) {Float_t tf;GetPositionL(x,y,z,tf);}
  void  GetPositionL(Double_t &x,Double_t &y,Double_t &z,Double_t &t) {Float_t xf,yf,zf,tf;GetPositionL(xf,yf,zf,tf);x=xf,y=yf;z=zf;t=tf;}
  void  GetPositionL(Double_t &x,Double_t &y,Double_t &z) {Float_t xf,yf,zf,tf;GetPositionL(xf,yf,zf,tf);x=xf,y=yf;z=zf;}
  void  GetPositionL0(Double_t &x,Double_t &y,Double_t &z,Double_t &t);
  //
  void Print(Option_t *option="") const;
  
  void GetPositionG(Float_t &x,Float_t &y,Float_t &z)const {
        // returns the position in the Global frame
     x=fX;y=fY;z=fZ;return;}
  void GetPositionG(Double_t &x,Double_t &y,Double_t &z)const {
        // returns the position in the Global frame
     x=fX;y=fY;z=fZ;return;}
  void GetPositionG0(Double_t &x,Double_t &y,Double_t &z,Double_t &tof)const {
        // returns the initial position in the Global frame and the
        // time of flight
     x=fx0;y=fy0;z=fz0,tof=fTof;return;}
 
  Bool_t StatusEntering() const {// checks if the particle is "entering"
        if((fStatus&0x0002)==0) return kFALSE;else return kTRUE;}
  Float_t GetIonization() const {return fDestep;}//returns the Destep

 protected:
//  Int_t GetModule(){return fModule;};
/*
   Int_t     fStatus; // Track Status
   Int_t     fModule; // Module number 
   Float_t   fPx;     // PX of particle at the point of the hit
   Float_t   fPy;     // PY of particle at the point of the hit
   Float_t   fPz;     // PZ of particle at the point of the hit
   Float_t   fDestep; // Energy deposited in the current step
   Float_t   fTof;    // Time of flight at the point of the hit
   Int_t     fStatus0;// Track Status of Starting point
   Float_t   fx0;     // Starting point of this step
   Float_t   fy0;     // Starting point of this step
   Float_t   fz0;     // Starting point of this step
   Float_t   ft0;     // Starting point of this step
*/
  ClassDef(AliITSUHit,3)  //Hits object
	 
}; 

#endif
