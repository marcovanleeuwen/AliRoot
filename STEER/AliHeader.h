#ifndef ALIHEADER_H
#define ALIHEADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TObject.h>
#include <TArrayI.h>
class AliStack;
class AliGenEventHeader;
 
class AliHeader : public TObject {
public:
  AliHeader();
  AliHeader(Int_t run, Int_t event);
  virtual ~AliHeader() {}

  virtual void Reset(Int_t run, Int_t event);

  virtual  void  SetRun(Int_t run) {fRun = run;}
  virtual  Int_t GetRun() const {return fRun;}
  
  virtual  void  SetNprimary(Int_t nprimary) {fNprimary = nprimary;}
  virtual  Int_t GetNprimary()   const {return fNprimary;}
  virtual  Int_t GetNsecondary() const {return fNtrack-fNprimary;}
  
  virtual  void  SetNvertex(Int_t vertex) {fNvertex = vertex;}
  virtual  Int_t GetNvertex() const {return fNvertex;}
  
  virtual  void  SetNtrack(Int_t ntrack) {fNtrack = ntrack;}
  virtual  Int_t GetNtrack() const {return fNtrack;}
  
  virtual  void  SetEvent(Int_t event) {fEvent = event;}
  virtual  Int_t GetEvent() const {return fEvent;}
  virtual  AliStack* Stack() const;
  virtual  void SetStack(AliStack* stack);

  virtual  void SetGenEventHeader(AliGenEventHeader* header);
  virtual  AliGenEventHeader*  GenEventHeader() const;

  virtual void Print(const char *opt=0);
  
protected:
  Int_t         fRun;         //Run number
  Int_t         fNvertex;     //Number of vertices
  Int_t         fNprimary;    //Number of primary tracks
  Int_t         fNtrack;      //Number of tracks
  Int_t         fEvent;       //Event number
  AliStack     *fStack;       //Pointer to stack
  AliGenEventHeader* fGenHeader;    //Event Header for Generator  
  
  ClassDef(AliHeader,1) //Alice event header    
};

#endif
