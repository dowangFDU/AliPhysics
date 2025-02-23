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
#ifndef AliAnalysisTaskNetProtonCumulants_pp_H
#define AliAnalysisTaskNetProtonCumulants_pp_H

#include "AliAnalysisTaskSE.h"
#include "TString.h"
#include "TTree.h"
#include "AliEventCuts.h"

class TList;
class TTree;

class AliESDEvent;
//class AliAODEvent;
class AliESDtrackCuts;
class AliMCEvent;
class AliStack;
class AliVTrack;
class AliAODTrack;

class TH1D;
class TH2D;
class TH3D;
class TProfile;
class AliPIDResponse;
class AliPIDCombined;
class AliMultSelection;


class AliAnalysisTaskNetProtonCumulants_pp : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskNetProtonCumulants_pp();
  AliAnalysisTaskNetProtonCumulants_pp(const char *name);
  virtual ~AliAnalysisTaskNetProtonCumulants_pp();
  
  virtual void   UserCreateOutputObjects();
  virtual void   UserExec(Option_t *option);
  virtual void   Terminate(Option_t *);
  Bool_t GetEvent();
  Bool_t PassedTrackQualityCuts (AliAODTrack *track);
  Bool_t KaonSelector (AliVTrack *track, Double_t nSigmaCut); 
  Bool_t ProtonSelector (AliVTrack *track, Double_t nSigmaCut); 
  Bool_t PionSelector (AliVTrack *track, Double_t nSigmaCut); 
  Bool_t PassedPIDSelection (AliAODTrack *track, AliPID::EParticleType type);
  Bool_t PassedSingleParticlePileUpCuts(AliAODTrack *track);
  void GetMCEffCorrectionHist();
  Bool_t HasTrackPIDTPC(AliVTrack *track);
  Bool_t HasTrackPIDTOF(AliVTrack *track);
  Int_t IdentifyTrackBayesian(AliVTrack *track);

  void SetListForTrkCorr (TList *fList)
  {
 	this->fListTRKCorr = (TList*) fList->Clone();
  }

  void SetVzRangeMax(Double_t VzMax)
  {
    this->fVertexZMax = VzMax;
  }
   void SetPileupCutValue (Int_t cutval)
  {
    this->fPileupCutVal = cutval;
  }
  void SetCentralityEstimator (Int_t val)
  {
    this->fCentralityEstimator_flag = val;
  }
  void SetTrackFilterBit(Int_t FBno)
  {
    this->fFBNo = FBno;
  }
  void SetMaxChi2PerTPCClusterRange(Double_t chi2tpc)
  {
    this->fChi2TPC = chi2tpc;
  }
  void SetMaxChi2PerITSClusterRange(Double_t chi2its)
  {
    this->fChi2ITS = chi2its;
  }
  void SetPIDnSigmaCut(Double_t PIDnSigmaCut_pion, Double_t PIDnSigmaCut_kaon, Double_t PIDnSigmaCut_proton)
  {
    this->fPIDnSigmaPionCut = PIDnSigmaCut_pion;
    this->fPIDnSigmaKaonCut = PIDnSigmaCut_kaon;
    this->fPIDnSigmaProtonCut = PIDnSigmaCut_proton;
  }
  void SetMinNoTPCCrossedRows(Double_t tpccrossedrows)
  {
    this->fTPCcrossedrows = tpccrossedrows;
  }
  void SetEtaCut(Double_t eta_max)
  {
    this->fEtaMax = eta_max;
  
  }
  void SetTreeName(TString TreeName)
  {
    fTreeName = TreeName;
  }
  void SetSelectPiKaPrByBayesianPIDFlag (Int_t flagg)
  {
    this->fBayesianPID_flag = flagg;
  }
   void SetBayesPIDPionVal (Double_t bayesPidPi)
  {
    this->fPIDbayesPion = bayesPidPi;
  }
  void SetBayesPIDKaonVal (Double_t bayesPidKa)
  {
    this->fPIDbayesKaon = bayesPidKa;
  }
  void SetBayesPIDProtonVal (Double_t bayesPidPr)
  {
    this->fPIDbayesProton = bayesPidPr;
  }
  void SetRapidityCutFlag (Int_t flag)
  {
    this->fRapidityCutFlag = flag;
  }

  
 private:
  
  
  AliESDEvent *fESDevent;
  AliAODEvent *fAODevent;
  AliVEvent *fInputEvent;
  AliPIDResponse   *fPIDResponse;
  AliPIDCombined   *fPIDCombined;
  AliESDtrackCuts  *fESDtrackCuts;
  AliESDtrackCuts  *fESDtrackCuts_primary;
  AliAnalysisUtils *fUtils;
  AliEventCuts fAODeventCuts;
  TList *fOutputList;
  TList *fQAList;
  Double_t fMultLow;
  Double_t fMultHigh;
  TH1D *hNumberOfEvents;
  TH1D *hNumberOfKaonPlus;
  TH1D *hNumberOfKaonMinus;
  TH1D *hNumberOfPionPlus;
  TH1D *hNumberOfPionMinus;
  TH1D *hNumberOfProtonPlus;
  TH1D *hNumberOfProtonMinus;
  TH2D *f2Dhist_pt_vs_rapidity_proton;

  //ftreeEvent object variables
  TTree *fTreeEvent;
  Float_t fTreeVariableCentrality;
  Float_t fMultV0AplusC;
  Float_t fMultEta0p8_noProton;
  Float_t fMultEtaGap;
  Float_t fRefmult;
  Float_t fRefmult5;
  Float_t fRefmult8;
  Float_t fRefmult10;
  
  //reconstruced
  Float_t fNoKaonPlus_ptmax2;
  Float_t fNoKaonMinus_ptmax2;
  Float_t fNoKaonPlus_ptmax3;
  Float_t fNoKaonMinus_ptmax3;
  Float_t fNoPionPlus_ptmax2;
  Float_t fNoPionMinus_ptmax2;
  Float_t fNoPionPlus_ptmax3;
  Float_t fNoPionMinus_ptmax3;
  Float_t fNoProtonPlus_ptmax2;
  Float_t fNoProtonMinus_ptmax2;
  Float_t fNoProtonPlus_ptmax3;
  Float_t fNoProtonMinus_ptmax3;
  //CORRECTED
  Float_t fCorrectedNoKaonPlus_ptmax2;
  Float_t fCorrectedNoKaonMinus_ptmax2;
  Float_t fCorrectedNoKaonPlus_ptmax3;
  Float_t fCorrectedNoKaonMinus_ptmax3;
  Float_t fCorrectedNoPionPlus_ptmax2;
  Float_t fCorrectedNoPionMinus_ptmax2;
  Float_t fCorrectedNoPionPlus_ptmax3;
  Float_t fCorrectedNoPionMinus_ptmax3;
  Float_t fCorrectedNoProtonPlus_ptmax2;
  Float_t fCorrectedNoProtonMinus_ptmax2;
  Float_t fCorrectedNoProtonPlus_ptmax3;
  Float_t fCorrectedNoProtonMinus_ptmax3;
  //Eff.square factors
  Float_t fEffSqrFactrPionMinus_ptmax2;
  Float_t fEffSqrFactrPionPlus_ptmax2;
  Float_t fEffSqrFactrProtonMinus_ptmax2;
  Float_t fEffSqrFactrProtonPlus_ptmax2;
  Float_t fEffSqrFactrKaonMinus_ptmax2;
  Float_t fEffSqrFactrKaonPlus_ptmax2;
  Float_t fEffSqrFactrPionMinus_ptmax3;
  Float_t fEffSqrFactrPionPlus_ptmax3;
  Float_t fEffSqrFactrProtonMinus_ptmax3;
  Float_t fEffSqrFactrProtonPlus_ptmax3;
  Float_t fEffSqrFactrKaonMinus_ptmax3;
  Float_t fEffSqrFactrKaonPlus_ptmax3;

  Float_t fEffCubeFactrProtonMinus_ptmax2;
  Float_t fEffCubeFactrProtonPlus_ptmax2;
  Float_t fEffCubeFactrProtonMinus_ptmax3;
  Float_t fEffCubeFactrProtonPlus_ptmax3;
  Float_t fEffPower4FactrProtonMinus_ptmax2;
  Float_t fEffPower4FactrProtonPlus_ptmax2;
  Float_t fEffPower4FactrProtonMinus_ptmax3;
  Float_t fEffPower4FactrProtonPlus_ptmax3;
  Float_t fEffPower5FactrProtonMinus_ptmax2;
  Float_t fEffPower5FactrProtonPlus_ptmax2;
  Float_t fEffPower5FactrProtonMinus_ptmax3;
  Float_t fEffPower5FactrProtonPlus_ptmax3;
  Float_t fEffPower6FactrProtonMinus_ptmax2;
  Float_t fEffPower6FactrProtonPlus_ptmax2;
  Float_t fEffPower6FactrProtonMinus_ptmax3;
  Float_t fEffPower6FactrProtonPlus_ptmax3;

  //Efficiency list of histograms
  TList *fListTRKCorr; 
  TH1D *fHistMCEffKaonPlus;
  TH1D *fHistMCEffKaonMinus;
  TH1D *fHistMCEffPionPlus;
  TH1D *fHistMCEffPionMinus;
  TH1D *fHistMCEffProtonPlus;
  TH1D *fHistMCEffProtonMinus;
  TH1D *fEffPionPlus[9];
  TH1D *fEffKaonPlus[9];
  TH1D *fEffProtonPlus[9];
  TH1D *fEffPionMinus[9];
  TH1D *fEffKaonMinus[9];
  TH1D *fEffProtonMinus[9];
 
  Double_t fVertexZMax;
  Int_t fFBNo;
  Double_t fChi2TPC;
  Double_t fChi2ITS;
  Double_t fPIDnSigmaPionCut;
  Double_t fPIDnSigmaKaonCut;
  Double_t fPIDnSigmaProtonCut;
  Double_t fTPCcrossedrows;
  Double_t fEtaMax;
  TString fTreeName;

  //Pileup cut val
  Int_t fPileupCutVal;
  
  //Flag to select which centrality estimator
  Int_t fCentralityEstimator_flag;
  Int_t fBayesianPID_flag;
  Double_t fPIDbayesPion;
  Double_t fPIDbayesKaon;
  Double_t fPIDbayesProton;
  Int_t fRapidityCutFlag;
  
  
  AliAnalysisTaskNetProtonCumulants_pp(const AliAnalysisTaskNetProtonCumulants_pp&);
  AliAnalysisTaskNetProtonCumulants_pp& operator=(const AliAnalysisTaskNetProtonCumulants_pp&);  
  ClassDef(AliAnalysisTaskNetProtonCumulants_pp, 1);
};

#endif
