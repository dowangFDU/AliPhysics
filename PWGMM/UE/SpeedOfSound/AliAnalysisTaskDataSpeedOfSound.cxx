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
 * provided "as is" without express or implied warranty.
 *                                                                        *
 * Authors: Omar Vazquez (omar.vazquez.rueda@cern.ch)                     *
 **************************************************************************/

/* This source code yields the histograms for the measurement of the
 * speed of sound using very central Pb-Pb collisions.
 */

class TTree;

class AliPPVsMultUtils;
class AliESDtrackCuts;

#include <AliAnalysisFilter.h>
#include <AliESDVertex.h>
#include <AliHeader.h>
#include <AliMultiplicity.h>
#include <TBits.h>
#include <TDirectory.h>
#include <TMath.h>
#include <TRandom.h>
#include <TTree.h>

#include <iostream>
#include <vector>

#include "AliAnalysisManager.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisTaskESDfilter.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisUtils.h"
#include "AliCentrality.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliESDUtils.h"
#include "AliESDVZERO.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"
// #include "AliEventCuts.h"
#include "AliGenCocktailEventHeader.h"
#include "AliGenEventHeader.h"
#include "AliInputEventHandler.h"
#include "AliLog.h"
#include "AliMCEvent.h"
#include "AliMCEventHandler.h"
// #include "AliMCParticle.h"
#include "AliMultEstimator.h"
#include "AliMultInput.h"
#include "AliMultSelection.h"
#include "AliMultVariable.h"
#include "AliMultiplicity.h"
#include "AliOADBContainer.h"
#include "AliOADBMultSelection.h"
#include "AliPPVsMultUtils.h"
#include "AliStack.h"
#include "AliVEvent.h"
#include "AliVTrack.h"
#include "AliVVertex.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THnSparse.h"
#include "TLegend.h"
#include "TList.h"
#include "TMath.h"
#include "TParticle.h"
#include "TProfile.h"
#include "TVector3.h"

using std::cout;
using std::endl;
static constexpr int uc_v0m_Nbins_fd{6};
static constexpr double uc_v0m_bins_high_fd[uc_v0m_Nbins_fd] = {1.0, 2.0, 3.0,
                                                                4.0, 5.0, 80.0};
static constexpr double uc_v0m_bins_low_fd[uc_v0m_Nbins_fd] = {0.0, 1.0, 2.0,
                                                               3.0, 4.0, 5.0};

static constexpr int uc_v0m_Nbins{5};
static constexpr double uc_v0m_bins[uc_v0m_Nbins + 1] = {0.0, 1.0, 2.0,
                                                         3.0, 4.0, 5.0};
static constexpr double uc_v0m_bins_high[uc_v0m_Nbins] = {1.0, 2.0, 3.0, 4.0,
                                                          5.0};
static constexpr double uc_v0m_bins_low[uc_v0m_Nbins] = {0.0, 1.0, 2.0, 3.0,
                                                         4.0};

#include "AliAnalysisTaskDataSpeedOfSound.h"

class AliAnalysisTaskDataSpeedOfSound;  // your analysis class

ClassImp(AliAnalysisTaskDataSpeedOfSound)  // classimp: necessary for root

    AliAnalysisTaskDataSpeedOfSound::AliAnalysisTaskDataSpeedOfSound()
    : AliAnalysisTaskSE(),
      fESD(0),
      fEventCuts(0x0),
      fMCStack(0),
      fMC(0),
      fUseMC(kFALSE),
      fIsTPConly(kTRUE),
      fTrigger(AliVEvent::kCentral),
      fTrackFilter(0x0),
      fTrackFilterwoDCA(0x0),
      fOutputList(0),
      fEtaCut(0.8),
      fPtMin(0.15),
      fV0Mmin(0.0),
      fV0Mmax(100.0),
      ftrackmult08(0),
      fv0mpercentile(0),
      fdcaxy(-999),
      fdcaz(-999),
      fMultSelection(0x0),
      // fMultSelectionbefvtx(0x0),
      hRefMult(0),
      hNchUCvsV0M(0),
      hV0Mmult(0),
      hTrackletvsV0M(0),
      hPtvsNch05(0),
      hTrueVtxZ(0),
      hTrueNchvsV0M_UC(0),
      hRecNchvsV0M_UC(0),
      // hTrueNchvsTruePt(0),
      hFullNchResponse(0),
      hNchResponse(0),
      hPtTruePrivsV0M(0),
      // hPtTrueSecvsV0M(0),
      // hPtTrueAllvsV0M(0),
      hPtRecPrivsV0M(0),
      hRecNchvsRecPt05(0),
      hTrueNchvsTruePt05(0),
      hTrueNch(0),
      hTrueNchHM(0),
      hTrueNchWithTrigger(0),
      hTrueNchHMWithTrigger(0),
      hTrueNchWithEventCuts(0),
      hTrueNchHMWithEventCuts(0),
      hTrueNchWithVtxSel(0),
      hTrueNchHMWithVtxSel(0) {
  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    hPtvsNch[i] = 0;
    hRecNchvsRecPt[i] = 0;
    hTrueNchvsTruePt[i] = 0;
  }
  for (int i = 0; i < uc_v0m_Nbins_fd; ++i) {
    hDCAxyPri[i] = 0;
    hDCAxyWeDe[i] = 0;
    hDCAxyMaIn[i] = 0;
    hDCAxyData[i] = 0;
  }
}
//_____________________________________________________________________________
AliAnalysisTaskDataSpeedOfSound::AliAnalysisTaskDataSpeedOfSound(
    const char* name)
    : AliAnalysisTaskSE(name),
      fESD(0),
      fEventCuts(0x0),
      fMCStack(0),
      fMC(0),
      fUseMC(kFALSE),
      fIsTPConly(kTRUE),
      fTrigger(AliVEvent::kCentral),
      fTrackFilter(0x0),
      fTrackFilterwoDCA(0x0),
      fOutputList(0),
      fEtaCut(0.8),
      fPtMin(0.15),
      fV0Mmin(0.0),
      fV0Mmax(100.0),
      ftrackmult08(0),
      fv0mpercentile(0),
      fdcaxy(-999),
      fdcaz(-999),
      fMultSelection(0x0),
      // fMultSelectionbefvtx(0x0),
      hRefMult(0),
      hNchUCvsV0M(0),
      hV0Mmult(0),
      hTrackletvsV0M(0),
      hPtvsNch05(0),
      hTrueVtxZ(0),
      hTrueNchvsV0M_UC(0),
      hRecNchvsV0M_UC(0),
      // hTrueNchvsTruePt(0),
      hFullNchResponse(0),
      hNchResponse(0),
      hPtTruePrivsV0M(0),
      // hPtTrueSecvsV0M(0),
      // hPtTrueAllvsV0M(0),
      hPtRecPrivsV0M(0),
      hRecNchvsRecPt05(0),
      hTrueNchvsTruePt05(0),
      hTrueNch(0),
      hTrueNchHM(0),
      hTrueNchWithTrigger(0),
      hTrueNchHMWithTrigger(0),
      hTrueNchWithEventCuts(0),
      hTrueNchHMWithEventCuts(0),
      hTrueNchWithVtxSel(0),
      hTrueNchHMWithVtxSel(0) {
  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    hPtvsNch[i] = 0;
    hRecNchvsRecPt[i] = 0;
    hTrueNchvsTruePt[i] = 0;
  }
  for (int i = 0; i < uc_v0m_Nbins_fd; ++i) {
    hDCAxyPri[i] = 0;
    hDCAxyWeDe[i] = 0;
    hDCAxyMaIn[i] = 0;
    hDCAxyData[i] = 0;
  }
  DefineInput(0, TChain::Class());  // define the input of the analysis: in this
                                    // case you take a 'chain' of events
  // this chain is created by the analysis manager, so no need to worry about
  // it, does its work automatically
  DefineOutput(1, TList::Class());  // define the ouptut of the analysis: in
                                    // this case it's a list of histograms
}
//_____________________________________________________________________________
AliAnalysisTaskDataSpeedOfSound::~AliAnalysisTaskDataSpeedOfSound() {
  // destructor
  if (fOutputList) {
    delete fOutputList;  // at the end of your task, it is deleted from memory
                         // by calling this function
    fOutputList = 0x0;
  }
}
//_____________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::UserCreateOutputObjects() {
  if (!fTrackFilter) {
    if (fIsTPConly)  // Default option
    {
      fTrackFilter = new AliAnalysisFilter("trackFilterTPConly");
      AliESDtrackCuts* fCuts2 = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts();
      fCuts2->SetRequireTPCRefit(kTRUE);
      fCuts2->SetRequireITSRefit(kTRUE);
      fCuts2->SetEtaRange(-0.8, 0.8);
      fTrackFilter->AddCuts(fCuts2);
    } else {  // Same as in the Nch vs. mult in pp, p-Pb and Pb-Pb
      std::cout << "Selecting non-TPConly cuts" << std::endl;
      fTrackFilter = new AliAnalysisFilter("trackFilter2015");
      AliESDtrackCuts* fCuts2_1 = new AliESDtrackCuts();
      fCuts2_1->SetMaxFractionSharedTPCClusters(0.4);                //
      fCuts2_1->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);  //
      fCuts2_1->SetCutGeoNcrNcl(3., 130., 1.5, 0.85, 0.7);           //
      fCuts2_1->SetMaxChi2PerClusterTPC(4);                          //
      fCuts2_1->SetAcceptKinkDaughters(kFALSE);                      //
      fCuts2_1->SetRequireTPCRefit(kTRUE);                           //
      fCuts2_1->SetRequireITSRefit(kTRUE);                           //
      fCuts2_1->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
                                         AliESDtrackCuts::kAny);    //
      fCuts2_1->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");  //
      fCuts2_1->SetMaxChi2TPCConstrainedGlobal(36);                 //
      fCuts2_1->SetMaxDCAToVertexZ(2);                              //
      fCuts2_1->SetDCAToVertex2D(kFALSE);                           //
      fCuts2_1->SetRequireSigmaToVertex(kFALSE);                    //
      fCuts2_1->SetMaxChi2PerClusterITS(36);                        //
      fCuts2_1->SetEtaRange(-0.8, 0.8);
      fTrackFilter->AddCuts(fCuts2_1);
    }
  }

  // track cuts to find contamination via DCA distribution
  if (!fTrackFilterwoDCA) {
    fTrackFilterwoDCA = new AliAnalysisFilter("trackFilter2015");
    AliESDtrackCuts* fCuts3 = new AliESDtrackCuts();
    fCuts3->SetMaxFractionSharedTPCClusters(0.4);                //
    fCuts3->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);  //
    fCuts3->SetCutGeoNcrNcl(3., 130., 1.5, 0.85, 0.7);           //
    fCuts3->SetMaxChi2PerClusterTPC(4);                          //
    fCuts3->SetAcceptKinkDaughters(kFALSE);                      //
    fCuts3->SetRequireTPCRefit(kTRUE);                           //
    fCuts3->SetRequireITSRefit(kTRUE);                           //
    fCuts3->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
                                     AliESDtrackCuts::kAny);  //
    // fCuts3->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");//
    // fCuts3->SetMaxChi2TPCConstrainedGlobal(36);//
    fCuts3->SetMaxDCAToVertexZ(2);            //
    fCuts3->SetDCAToVertex2D(kFALSE);         //
    fCuts3->SetRequireSigmaToVertex(kFALSE);  //
    fCuts3->SetMaxChi2PerClusterITS(36);      //
    fCuts3->SetEtaRange(-0.8, 0.8);
    fTrackFilterwoDCA->AddCuts(fCuts3);
  }

  // create output objects
  OpenFile(1);
  fOutputList = new TList();

  fOutputList->SetOwner(kTRUE);

  constexpr int pt_Nbins{33};
  constexpr double pt_bins[pt_Nbins + 1] = {
      0.0, 0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45, 0.5,  0.6, 0.7, 0.8,
      0.9, 1.0,  1.25, 1.5,  2.0,  2.5,  3.0,  3.5,  4.0,  4.5, 5.0, 6.0,
      7.0, 8.0,  9.0,  10.0, 12.0, 14.0, 16.0, 18.0, 20.0, 50.0};

  constexpr int v0m_Nbins{8};
  constexpr double v0m_bins[v0m_Nbins + 1] = {0.0,  5.0,  10.0, 20.0, 30.0,
                                              40.0, 50.0, 60.0, 80.0};

  constexpr int nch_Nbins{168};
  double nch_bins[nch_Nbins + 1] = {0};
  for (int i = 0; i <= nch_Nbins; ++i) {
    nch_bins[i] = 30.0 * i;
  }

  constexpr int dcaxy_Nbins{300};
  double dcaxy_bins[dcaxy_Nbins + 1] = {0};
  for (int i = 0; i <= dcaxy_Nbins; ++i) {
    dcaxy_bins[i] = -3.0 + (0.02 * i);
  }

  const char* uc_v0m_bins_name[uc_v0m_Nbins] = {"0_1", "1_2", "2_3", "3_4",
                                                "4_5"};

  const char* uc_v0m_bins_fd_name[uc_v0m_Nbins_fd] = {"0_1", "1_2", "2_3",
                                                      "3_4", "4_5", "5_80"};

  hRefMult =
      new TH1F("hTrackletMult", ";#it{N}_{ch}^{tracklet} (|#eta|<0.8);Entries",
               nch_Nbins, nch_bins);
  fOutputList->Add(hRefMult);

  hV0Mmult =
      new TH1F("hV0Mmult_UC", ";V0M (%);Entries", uc_v0m_Nbins, uc_v0m_bins);
  fOutputList->Add(hV0Mmult);

  hNchUCvsV0M = new TH2F("hNchvsV0M_UC", ";#it{N}_{ch} (|#eta|<0.8); V0M (%)",
                         nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);
  fOutputList->Add(hNchUCvsV0M);

  hPtvsNch05 = new TH2F(
      "hPtvsNch_0_5", "; #it{p}_{T} GeV/#it{c}; #it{N}_{ch}^{rec} (|#eta|<0.8)",
      pt_Nbins, pt_bins, nch_Nbins, nch_bins);

  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    hPtvsNch[i] =
        new TH2F(Form("hPtvsNch_%s", uc_v0m_bins_name[i]),
                 "; #it{p}_{T} GeV/#it{c}; #it{N}_{ch}^{rec} (|#eta|<0.8)",
                 pt_Nbins, pt_bins, nch_Nbins, nch_bins);
    fOutputList->Add(hPtvsNch[i]);
  }
  fOutputList->Add(hPtvsNch05);

  hTrackletvsV0M =
      new TH2F("hTrackletvsV0M", ";#it{N}_{ch}^{tracklet} (|#eta|<0.8);V0M (%)",
               8001, -0.5, 8000.5, uc_v0m_Nbins, uc_v0m_bins);
  fOutputList->Add(hTrackletvsV0M);

  hTrueVtxZ =
      new TH1F("hTrueVtxZ", ";z-vertex position;Entries", 200, -10.0, 10.0);

  hNchResponse = new TH2F(
      "hNchResponse", ";#it{N}_{ch}^{rec} (|#eta|<0.8); #it{N}_{ch}^{true};",
      nch_Nbins, nch_bins, nch_Nbins, nch_bins);

  hFullNchResponse =
      new TH2F("hFullNchResponse",
               ";#it{N}_{ch}^{rec} (|#eta|<0.8); #it{N}_{ch}^{true};",
               nch_Nbins, nch_bins, nch_Nbins, nch_bins);

  hTrueNchvsV0M_UC =
      new TH2F("hTrueNchvsV0M_UC", "; #it{N}_{ch}^{rec} (|#eta|<0.8); V0M (%)",
               nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);

  hRecNchvsV0M_UC =
      new TH2F("hRecNchvsV0M_UC", "; #it{N}_{ch}^{rec} (|#eta|<0.8); V0M (%)",
               nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);

  hTrueNch = new TH1F("hTrueNch", ";#it{N}_{ch}^{true} (|#eta|<0.8); Entries",
                      nch_Nbins, nch_bins);
  hTrueNchHM =
      new TH2F("hTrueNchHM", ";#it{N}_{ch}^{true} (|#eta|<0.8); V0M (%)",
               nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);
  hTrueNchWithTrigger = new TH1F("hTrueNchWithTrigger",
                                 ";#it{N}_{ch}^{true} (|#eta|<0.8); Entries",
                                 nch_Nbins, nch_bins);
  hTrueNchHMWithTrigger = new TH2F(
      "hTrueNchHMWithTrigger", ";#it{N}_{ch}^{true} (|#eta|<0.8); V0M (%)",
      nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);
  hTrueNchWithEventCuts = new TH1F("hTrueNchWithEventCuts",
                                   ";#it{N}_{ch}^{true} (|#eta|<0.8); Entries",
                                   nch_Nbins, nch_bins);
  hTrueNchHMWithEventCuts = new TH2F(
      "hTrueNchHMWithEventCuts", ";#it{N}_{ch}^{true} (|#eta|<0.8); V0M (%)",
      nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);
  hTrueNchWithVtxSel = new TH1F("hTrueNchWithVtxSel",
                                ";#it{N}_{ch}^{true} (|#eta|<0.8); Entries",
                                nch_Nbins, nch_bins);
  hTrueNchHMWithVtxSel = new TH2F(
      "hTrueNchHMWithVtxSel", ";#it{N}_{ch}^{true} (|#eta|<0.8); V0M (%)",
      nch_Nbins, nch_bins, uc_v0m_Nbins, uc_v0m_bins);

  hRecNchvsRecPt05 =
      new TH2F("hRecPtvsRecNch_UC_0_5",
               "; #it{p}_{T} GeV/#it{c}; #it{N}_{ch}^{rec} (|#eta|<0.8)",
               pt_Nbins, pt_bins, nch_Nbins, nch_bins);

  hTrueNchvsTruePt05 =
      new TH2F("hTruePtvsTrueNch_UC_%s",
               "; #it{p}_{T} GeV/#it{c}; #it{N}_{ch}^{rec} (|#eta|<0.8)",
               pt_Nbins, pt_bins, nch_Nbins, nch_bins);

  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    hRecNchvsRecPt[i] =
        new TH2F(Form("hRecPtvsRecNch_UC_%s", uc_v0m_bins_name[i]),
                 "; #it{p}_{T} GeV/#it{c}; #it{N}_{ch}^{rec} (|#eta|<0.8)",
                 pt_Nbins, pt_bins, nch_Nbins, nch_bins);

    hTrueNchvsTruePt[i] =
        new TH2F(Form("hTruePtvsTrueNch_UC_%s", uc_v0m_bins_name[i]),
                 "; #it{p}_{T} GeV/#it{c}; #it{N}_{ch}^{rec} (|#eta|<0.8)",
                 pt_Nbins, pt_bins, nch_Nbins, nch_bins);
  }

  hPtTruePrivsV0M =
      new TH2F("hPtTruePrivsV0M", ";#it{p}_{T} GeV/#it{c}; V0M (%)", pt_Nbins,
               pt_bins, v0m_Nbins, v0m_bins);

  hPtRecPrivsV0M = new TH2F("hPtRecPrivsV0M", ";#it{p}_{T} GeV/#it{c}; V0M (%)",
                            pt_Nbins, pt_bins, v0m_Nbins, v0m_bins);

  for (int i = 0; i < uc_v0m_Nbins_fd; ++i) {
    hDCAxyPri[i] = new TH2F(Form("hDCAxyPri_%s", uc_v0m_bins_fd_name[i]),
                            "; #it{p}_{T} GeV/#it{c}; DCA_{xy} (cm)", pt_Nbins,
                            pt_bins, dcaxy_Nbins, dcaxy_bins);
    hDCAxyWeDe[i] = new TH2F(Form("hDCAxyWeDe_%s", uc_v0m_bins_fd_name[i]),
                             "; #it{p}_{T} GeV/#it{c}; DCA_{xy} (cm)", pt_Nbins,
                             pt_bins, dcaxy_Nbins, dcaxy_bins);
    hDCAxyMaIn[i] = new TH2F(Form("hDCAxyMaIn_%s", uc_v0m_bins_fd_name[i]),
                             "; #it{p}_{T} GeV/#it{c}; DCA_{xy} (cm)", pt_Nbins,
                             pt_bins, dcaxy_Nbins, dcaxy_bins);
    hDCAxyData[i] = new TH2F(Form("hDCAxyData_%s", uc_v0m_bins_fd_name[i]),
                             "; #it{p}_{T} GeV/#it{c}; DCA_{xy} (cm)", pt_Nbins,
                             pt_bins, dcaxy_Nbins, dcaxy_bins);
  }

  if (fUseMC) {
    fOutputList->Add(hTrueVtxZ);
    fOutputList->Add(hNchResponse);
    fOutputList->Add(hPtTruePrivsV0M);
    // fOutputList->Add(hPtTrueSecvsV0M);
    // fOutputList->Add(hPtTrueAllvsV0M);
    fOutputList->Add(hPtRecPrivsV0M);
    fOutputList->Add(hFullNchResponse);
    fOutputList->Add(hTrueNchvsV0M_UC);
    fOutputList->Add(hRecNchvsV0M_UC);

    fOutputList->Add(hTrueNch);
    fOutputList->Add(hTrueNchHM);
    fOutputList->Add(hTrueNchWithTrigger);
    fOutputList->Add(hTrueNchHMWithTrigger);
    fOutputList->Add(hTrueNchWithEventCuts);
    fOutputList->Add(hTrueNchHMWithEventCuts);
    fOutputList->Add(hTrueNchWithVtxSel);
    fOutputList->Add(hTrueNchHMWithVtxSel);
    for (int i = 0; i < uc_v0m_Nbins; ++i) {
      fOutputList->Add(hRecNchvsRecPt[i]);
      fOutputList->Add(hTrueNchvsTruePt[i]);
    }
    fOutputList->Add(hRecNchvsRecPt05);
    fOutputList->Add(hTrueNchvsTruePt05);

    for (int i = 0; i < uc_v0m_Nbins_fd; ++i) {
      fOutputList->Add(hDCAxyPri[i]);
      fOutputList->Add(hDCAxyWeDe[i]);
      fOutputList->Add(hDCAxyMaIn[i]);
    }
  }

  for (int i = 0; i < uc_v0m_Nbins_fd; ++i) {
    fOutputList->Add(hDCAxyData[i]);
  }

  fEventCuts.AddQAplotsToList(fOutputList);
  PostData(1, fOutputList);  // postdata will notify the analysis manager of
                             // changes / updates to the
}
//_____________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::UserExec(Option_t*) {
  AliVEvent* event = InputEvent();
  if (!event) {
    Error("UserExec", "Could not retrieve event");
    return;
  }

  fESD = dynamic_cast<AliESDEvent*>(event);

  if (!fESD) {
    Printf("%s:%d ESDEvent not found in Input Manager", (char*)__FILE__,
           __LINE__);
    this->Dump();
    return;
  }

  if (fUseMC) {
    //      E S D
    fMC = dynamic_cast<AliMCEvent*>(MCEvent());
    if (!fMC) {
      Printf("%s:%d MCEvent not found in Input Manager", (char*)__FILE__,
             __LINE__);
      this->Dump();
      return;
    }
    fMCStack = fMC->Stack();
  }

  double random_number = -1.0;
  gRandom->SetSeed(0);
  random_number = gRandom->Uniform(0.0, 1.0);
  // std::cout << "random_number = " << random_number << std::endl;

  bool isHMevent{false};
  ftrackmult08 = -999.0;
  fv0mpercentile = -999.0;

  fMultSelection = (AliMultSelection*)fESD->FindListObject("MultSelection");
  fv0mpercentile = fMultSelection->GetMultiplicityPercentile("V0M");
  ftrackmult08 = AliESDtrackCuts::GetReferenceMultiplicity(
      fESD, AliESDtrackCuts::kTrackletsITSTPC, 0.8);

  if (fv0mpercentile >= fV0Mmin && fv0mpercentile < fV0Mmax) {
    isHMevent = true;
  }

  bool isGoodVtxPosMC{false};
  int true_nch{0};
  int rec_nch{0};
  std::vector<float> vec_true_pt{};
  std::vector<float> vec_rec_pt{};

  if (fUseMC) {
    AliHeader* headerMC = fMC->Header();
    AliGenEventHeader* genHeader = headerMC->GenEventHeader();
    TArrayF vtxMC(3);  // primary vertex  MC
    vtxMC[0] = 9999;
    vtxMC[1] = 9999;
    vtxMC[2] = 9999;  // initialize with dummy
    if (genHeader) {
      genHeader->PrimaryVertex(vtxMC);
    }
    if (TMath::Abs(vtxMC[2]) <= 10.0) {
      isGoodVtxPosMC = true;
    }
    // cout << "Vtx_z = " << TMath::Abs(vtxMC[2]) << '\n';
    float vtx_z{999};
    vtx_z = vtxMC[2];
    if (!isGoodVtxPosMC) {
      return;
    }

    hTrueVtxZ->Fill(vtx_z);
    AnalyzeMCevent(true_nch, vec_true_pt);
    if (true_nch <= 0) {
      return;
    }
    // Before trigger selection
    // if random_number < 0.5 --> Multiplicity Distributions
    // if random_number >= 0.5 --> Detector Response & Corrections
    if (random_number < 0.5) {
      if (fv0mpercentile >= fV0Mmin && fv0mpercentile < fV0Mmax) {
        TrueMultiplicityDistributions(true_nch, vec_true_pt);
      }
    }

    hTrueNch->Fill(true_nch);
    if (isHMevent) {
      hTrueNchHM->Fill(true_nch, fv0mpercentile);
    }
  }

  // Trigger selection
  // bool isINT7selected = fSelectMask & AliVEvent::kINT7;
  // if (!isINT7selected) {
  //   return;
  // }

  // std::cout << "fTrigger= " << fTrigger << '\n';
  //! Trigger selection
  bool isEventTriggered{false};
  UInt_t fSelectMask = fInputHandler->IsEventSelected();
  isEventTriggered = fSelectMask & fTrigger;
  if (!isEventTriggered) {
    return;
  }

  if (fUseMC) {
    hTrueNchWithTrigger->Fill(true_nch);
    if (isHMevent) {
      hTrueNchHMWithTrigger->Fill(true_nch, fv0mpercentile);
    }
  }

  // Good events
  if (!fEventCuts.AcceptEvent(event)) {
    PostData(1, fOutputList);
    return;
  }

  if (fUseMC) {
    hTrueNchWithEventCuts->Fill(true_nch);
    if (isHMevent) {
      hTrueNchHMWithEventCuts->Fill(true_nch, fv0mpercentile);
    }
  }

  // Good vertex
  bool hasRecVertex = false;
  hasRecVertex = HasRecVertex();
  if (!hasRecVertex) {
    return;
  }

  if (fUseMC) {
    hTrueNchWithVtxSel->Fill(true_nch);
    if (isHMevent) {
      hTrueNchHMWithVtxSel->Fill(true_nch, fv0mpercentile);
    }
  }

  AnalyzeRecEvent(rec_nch, vec_rec_pt);

  if (fv0mpercentile >= fV0Mmin && fv0mpercentile < 80.0) {
    hRefMult->Fill(ftrackmult08);
  }

  hTrackletvsV0M->Fill(ftrackmult08, fv0mpercentile);

  //! if random_number < 0.5 --> Multiplicity Distributions
  //! if random_number >= 0.5 --> Detector Response & Corrections
  if (fUseMC && isGoodVtxPosMC) {
    if (random_number >= 0.5) {
      hFullNchResponse->Fill(rec_nch, true_nch);
      TrackingEfficiency();
    }
  }

  //! DCAxy templates
  DCAxyDistributions();

  // Only analyze ultra central events
  if (!(fv0mpercentile >= fV0Mmin && fv0mpercentile < fV0Mmax)) {
    return;
  }

  // Data Multiplicity distributions
  hV0Mmult->Fill(fv0mpercentile);
  MultiplicityDistributions(rec_nch, vec_rec_pt);
  if (fUseMC && isGoodVtxPosMC) {
    if (random_number < 0.5) {
      RecMultiplicityDistributions(rec_nch, vec_rec_pt);
    } else {
      DetectorResponse(true_nch, rec_nch);
    }
  }

  PostData(1, fOutputList);
}
//______________________________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::Terminate(Option_t*) {}

void AliAnalysisTaskDataSpeedOfSound::MultiplicityDistributions(
    const int& rec_nch, const std::vector<float>& vec_rec_pt) const {
  unsigned long n_rec_nch = (unsigned long)rec_nch;
  if (n_rec_nch != vec_rec_pt.size()) {
    cout << "Different rec_nch and elements in vec_rec_pt"
         << "nch= " << rec_nch << "nch from vector= " << vec_rec_pt.size()
         << '\n';
  }

  int index{-1};
  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    if (fv0mpercentile >= uc_v0m_bins_low[i] &&
        fv0mpercentile < uc_v0m_bins_high[i]) {
      index = i;
      break;
    }
  }

  if (index < 0) {
    return;
  }

  for (auto pt : vec_rec_pt) {
    hPtvsNch[index]->Fill(pt, rec_nch);
    hPtvsNch05->Fill(pt, rec_nch);
  }

  hNchUCvsV0M->Fill(rec_nch, fv0mpercentile);
  // hITSclustersvsNch->Fill(track->GetITSNcls(), multrec);
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::AnalyzeRecEvent(
    int& rec_nch, std::vector<float>& vec_rec_pt) const {
  rec_nch = 0;
  vec_rec_pt.clear();
  const int n_tracks{fESD->GetNumberOfTracks()};
  for (int i = 0; i < n_tracks; ++i) {
    AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
    if (!track) {
      continue;
    }
    if (!fTrackFilter->IsSelected(track)) {
      continue;
    }
    if (track->Pt() < fPtMin) {
      continue;
    }
    if (TMath::Abs(track->Eta()) > fEtaCut) {
      continue;
    }
    rec_nch++;
    vec_rec_pt.push_back(track->Pt());
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::DCAxyDistributions() const {
  int index{-1};
  for (int i = 0; i < uc_v0m_Nbins_fd; ++i) {
    if (fv0mpercentile >= uc_v0m_bins_low_fd[i] &&
        fv0mpercentile < uc_v0m_bins_high_fd[i]) {
      index = i;
      break;
    }
  }

  if (index < 0) {
    return;
  }

  const int n_tracks{fESD->GetNumberOfTracks()};
  if (fUseMC) {
    for (int i = 0; i < n_tracks; ++i) {
      AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
      if (!track) {
        continue;
      }
      if (!fTrackFilterwoDCA->IsSelected(track)) {
        continue;
      }
      if (track->Pt() < fPtMin) {
        continue;
      }
      if (TMath::Abs(track->Eta()) > fEtaCut) {
        continue;
      }

      float dcaxy = -999;
      float dcaz = -999;
      track->GetImpactParameters(dcaxy, dcaz);

      int label = -1;
      label = TMath::Abs(track->GetLabel());
      // TParticle* particle = fMC->GetTrack(label)->Particle();
      // if (!particle) {
      //   continue;
      // }
      if (fMC->IsPhysicalPrimary(label)) {
        hDCAxyPri[index]->Fill(track->Pt(), dcaxy);
      } else if (fMC->IsSecondaryFromWeakDecay(label)) {
        hDCAxyWeDe[index]->Fill(track->Pt(), dcaxy);
      } else if (fMC->IsSecondaryFromMaterial(label)) {
        hDCAxyMaIn[index]->Fill(track->Pt(), dcaxy);
      } else {
        continue;
      }
    }
  }
  if (!fUseMC) {
    for (int i = 0; i < n_tracks; ++i) {
      AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
      if (!track) {
        continue;
      }
      if (!fTrackFilterwoDCA->IsSelected(track)) {
        continue;
      }
      if (track->Pt() < fPtMin) {
        continue;
      }
      if (TMath::Abs(track->Eta()) > fEtaCut) {
        continue;
      }

      float dcaxy = -999;
      float dcaz = -999;
      track->GetImpactParameters(dcaxy, dcaz);

      hDCAxyData[index]->Fill(track->Pt(), dcaxy);
    }
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::TrackingEfficiency() const {
  const int n_tracks{fESD->GetNumberOfTracks()};
  if (fUseMC) {
    for (int i = 0; i < n_tracks; ++i) {
      AliESDtrack* track = static_cast<AliESDtrack*>(fESD->GetTrack(i));
      if (!track) {
        continue;
      }
      if (!fTrackFilter->IsSelected(track)) {
        continue;
      }
      if (track->Pt() < fPtMin) {
        continue;
      }
      if (TMath::Abs(track->Eta()) > fEtaCut) {
        continue;
      }

      int label = -1;
      label = TMath::Abs(track->GetLabel());
      // TParticle* particle = fMC->GetTrack(label)->Particle();
      // if (!particle) {
      //   continue;
      // }
      if (fMC->IsPhysicalPrimary(label)) {
        hPtRecPrivsV0M->Fill(track->Pt(), fv0mpercentile);
      } else {
        continue;
      }
    }

    const int n_particles{fMC->GetNumberOfTracks()};
    for (int i = 0; i < n_particles; ++i) {
      AliMCParticle* particle = (AliMCParticle*)fMC->GetTrack(i);
      if (!particle) {
        continue;
      }
      if (AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(i, fMC)) {
        continue;
      }
      if (TMath::Abs(particle->Eta()) > fEtaCut) {
        continue;
      }
      if (particle->Pt() < fPtMin) {
        continue;
      }
      if (particle->Charge() == 0.0) {
        continue;
      }
      if (fMC->IsPhysicalPrimary(i)) {
        hPtTruePrivsV0M->Fill(particle->Pt(), fv0mpercentile);
      } else {
        continue;
      }
    }
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::AnalyzeMCevent(
    int& true_nch, std::vector<float>& vec_true_pt) const {
  true_nch = 0;
  vec_true_pt.clear();
  const int n_particles{fMC->GetNumberOfTracks()};
  for (int i = 0; i < n_particles; ++i) {
    AliMCParticle* particle = (AliMCParticle*)fMC->GetTrack(i);
    if (!particle) {
      continue;
    }
    if (AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(i, fMC)) {
      continue;
    }
    if (TMath::Abs(particle->Eta()) > fEtaCut) {
      continue;
    }
    if (particle->Pt() < fPtMin) {
      continue;
    }
    if (particle->Charge() == 0.0) {
      continue;
    }
    if (fMC->IsPhysicalPrimary(i)) {
      true_nch++;
      vec_true_pt.push_back(particle->Pt());
    }
  }
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::DetectorResponse(
    const int& true_nch, const int& rec_nch) const {
  hNchResponse->Fill(rec_nch, true_nch);
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::RecMultiplicityDistributions(
    const int& rec_nch, const std::vector<float>& vec_rec_pt) const {
  unsigned long n_rec_nch = (unsigned long)rec_nch;
  if (n_rec_nch != vec_rec_pt.size()) {
    cout << "Different rec_nch and elements in vec_rec_pt"
         << "nch= " << rec_nch << "nch from vector= " << vec_rec_pt.size()
         << '\n';
  }

  int index{-1};
  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    if (fv0mpercentile >= uc_v0m_bins_low[i] &&
        fv0mpercentile < uc_v0m_bins_high[i]) {
      index = i;
      break;
    }
  }

  if (index < 0) {
    return;
  }

  for (auto pt : vec_rec_pt) {
    hRecNchvsRecPt[index]->Fill(pt, rec_nch);
    hRecNchvsRecPt05->Fill(pt, rec_nch);
  }

  hRecNchvsV0M_UC->Fill(rec_nch, fv0mpercentile);
}
//____________________________________________________________
void AliAnalysisTaskDataSpeedOfSound::TrueMultiplicityDistributions(
    const int& true_nch, const std::vector<float>& vec_true_pt) const {
  unsigned long n_true_nch = (unsigned long)true_nch;
  if (n_true_nch != vec_true_pt.size()) {
    cout << "Different true_nch and elements in vec_true_pt"
         << "nch= " << true_nch << "nch from vector= " << vec_true_pt.size()
         << '\n';
  }

  int index{-1};
  for (int i = 0; i < uc_v0m_Nbins; ++i) {
    if (fv0mpercentile >= uc_v0m_bins_low[i] &&
        fv0mpercentile < uc_v0m_bins_high[i]) {
      index = i;
      break;
    }
  }

  if (index < 0) {
    return;
  }

  for (auto pt : vec_true_pt) {
    hTrueNchvsTruePt[index]->Fill(pt, true_nch);
    hTrueNchvsTruePt05->Fill(pt, true_nch);
  }

  hTrueNchvsV0M_UC->Fill(true_nch, fv0mpercentile);
}
//____________________________________________________________
Bool_t AliAnalysisTaskDataSpeedOfSound::HasRecVertex() {
  float fMaxDeltaSpdTrackAbsolute = 0.5f;
  float fMaxDeltaSpdTrackNsigmaSPD = 1.e14f;
  float fMaxDeltaSpdTrackNsigmaTrack = 1.e14;
  float fMaxResolutionSPDvertex = 0.25f;
  float fMaxDispersionSPDvertex = 1.e14f;

  Bool_t fRequireTrackVertex = true;
  unsigned long fFlag;
  fFlag = BIT(AliEventCuts::kNoCuts);

  const AliVVertex* vtTrc = fESD->GetPrimaryVertex();
  bool isTrackV = true;
  if (vtTrc->IsFromVertexer3D() || vtTrc->IsFromVertexerZ()) isTrackV = false;
  const AliVVertex* vtSPD = fESD->GetPrimaryVertexSPD();

  if (vtSPD->GetNContributors() > 0) fFlag |= BIT(AliEventCuts::kVertexSPD);

  if (vtTrc->GetNContributors() > 1 && isTrackV)
    fFlag |= BIT(AliEventCuts::kVertexTracks);

  if (((fFlag & BIT(AliEventCuts::kVertexTracks)) || !fRequireTrackVertex) &&
      (fFlag & BIT(AliEventCuts::kVertexSPD)))
    fFlag |= BIT(AliEventCuts::kVertex);

  const AliVVertex*& vtx =
      bool(fFlag & BIT(AliEventCuts::kVertexTracks)) ? vtTrc : vtSPD;
  AliVVertex* fPrimaryVertex = const_cast<AliVVertex*>(vtx);
  if (!fPrimaryVertex) return kFALSE;

  /// Vertex quality cuts
  double covTrc[6], covSPD[6];
  vtTrc->GetCovarianceMatrix(covTrc);
  vtSPD->GetCovarianceMatrix(covSPD);
  double dz = bool(fFlag & AliEventCuts::kVertexSPD) &&
                      bool(fFlag & AliEventCuts::kVertexTracks)
                  ? vtTrc->GetZ() - vtSPD->GetZ()
                  : 0.;  /// If one of the two vertices is not available this
                         /// cut is always passed.
  double errTot = TMath::Sqrt(covTrc[5] + covSPD[5]);
  double errTrc =
      bool(fFlag & AliEventCuts::kVertexTracks) ? TMath::Sqrt(covTrc[5]) : 1.;
  double nsigTot = TMath::Abs(dz) / errTot, nsigTrc = TMath::Abs(dz) / errTrc;
  /// vertex dispersion for run1, only for ESD, AOD code to be added here
  const AliESDVertex* vtSPDESD = dynamic_cast<const AliESDVertex*>(vtSPD);
  double vtSPDdispersion = vtSPDESD ? vtSPDESD->GetDispersion() : 0;
  if ((TMath::Abs(dz) <= fMaxDeltaSpdTrackAbsolute &&
       nsigTot <= fMaxDeltaSpdTrackNsigmaSPD &&
       nsigTrc <=
           fMaxDeltaSpdTrackNsigmaTrack) &&  // discrepancy track-SPD vertex
      (!vtSPD->IsFromVertexerZ() ||
       TMath::Sqrt(covSPD[5]) <= fMaxResolutionSPDvertex) &&
      (!vtSPD->IsFromVertexerZ() ||
       vtSPDdispersion <= fMaxDispersionSPDvertex)  /// vertex dispersion cut
                                                    /// for run1, only for ESD
      )  // quality cut on vertexer SPD z
    fFlag |= BIT(AliEventCuts::kVertexQuality);

  Bool_t hasVtx = (TESTBIT(fFlag, AliEventCuts::kVertex)) &&
                  (TESTBIT(fFlag, AliEventCuts::kVertexQuality));

  return hasVtx;
}