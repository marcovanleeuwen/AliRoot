//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUTPCNeighboursFinder.cxx
/// \author Sergey Gorbunov, Ivan Kisel, David Rohr

#include "GPUTPCHit.h"
#include "GPUTPCHitArea.h"
#include "GPUTPCNeighboursFinder.h"
#include "GPUTPCTracker.h"
#include "GPUCommonMath.h"
using namespace GPUCA_NAMESPACE::gpu;

template <>
GPUdii() void GPUTPCNeighboursFinder::Thread<0>(int /*nBlocks*/, int nThreads, int iBlock, int iThread, GPUsharedref() MEM_LOCAL(GPUSharedMemory) & s, processorType& tracker)
{
  //* find neighbours

#ifdef GPUCA_GPUCODE
  for (unsigned int i = iThread; i < sizeof(MEM_PLAIN(GPUTPCRow)) / sizeof(int); i += nThreads) {
    reinterpret_cast<GPUsharedref() int*>(&s.mRow)[i] = reinterpret_cast<GPUglobalref() int*>(&tracker.SliceDataRows()[iBlock])[i];
    if (iBlock >= 2 && iBlock < GPUCA_ROW_COUNT - 2) {
      reinterpret_cast<GPUsharedref() int*>(&s.mRowUp)[i] = reinterpret_cast<GPUglobalref() int*>(&tracker.SliceDataRows()[iBlock + 2])[i];
      reinterpret_cast<GPUsharedref() int*>(&s.mRowDown)[i] = reinterpret_cast<GPUglobalref() int*>(&tracker.SliceDataRows()[iBlock - 2])[i];
    }
  }
  GPUbarrier();
#endif
  if (iThread == 0) {
    s.mIRow = iBlock;
    if (s.mIRow < GPUCA_ROW_COUNT) {
#ifdef GPUCA_GPUCODE
      GPUsharedref() const MEM_LOCAL(GPUTPCRow)& row = s.mRow;
#else
      GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& row = tracker.Row(s.mIRow);
#endif
      s.mNHits = row.NHits();

      if ((s.mIRow >= 2) && (s.mIRow <= GPUCA_ROW_COUNT - 3)) {
        s.mIRowUp = s.mIRow + 2;
        s.mIRowDn = s.mIRow - 2;

        // references to the rows above and below

#ifdef GPUCA_GPUCODE
        GPUsharedref() const MEM_LOCAL(GPUTPCRow)& rowUp = s.mRowUp;
        GPUsharedref() const MEM_LOCAL(GPUTPCRow)& rowDn = s.mRowDown;
#else
        GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& rowUp = tracker.Row(s.mIRowUp);
        GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& rowDn = tracker.Row(s.mIRowDn);
#endif
        // the axis perpendicular to the rows
        const float xDn = rowDn.X();
        const float x = row.X();
        const float xUp = rowUp.X();

        // number of hits in rows above and below
        s.mUpNHits = tracker.Row(s.mIRowUp).NHits();
        s.mDnNHits = tracker.Row(s.mIRowDn).NHits();

        // distance of the rows (absolute and relative)
        s.mUpDx = xUp - x;
        s.mDnDx = xDn - x;
        s.mUpTx = xUp / x;
        s.mDnTx = xDn / x;
        // UpTx/DnTx is used to move the HitArea such that central events are preferred (i.e. vertices
        // coming from y = 0, z = 0).

        // s.mGridUp = tracker.Row( s.mIRowUp ).Grid();
        // s.mGridDn = tracker.Row( s.mIRowDn ).Grid();
      }
    }
  }
  GPUbarrier();

  if ((s.mIRow <= 1) || (s.mIRow >= GPUCA_ROW_COUNT - 2)) {
#ifdef GPUCA_GPUCODE
    GPUsharedref() const MEM_LOCAL(GPUTPCRow)& row = s.mRow;
#else
    GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& row = tracker.Row(s.mIRow);
#endif
    for (int ih = iThread; ih < s.mNHits; ih += nThreads) {
      tracker.SetHitLinkUpData(row, ih, CALINK_INVAL);
      tracker.SetHitLinkDownData(row, ih, CALINK_INVAL);
    }
    return;
  }

  float chi2Cut = 3.f * 3.f * 4 * (s.mUpDx * s.mUpDx + s.mDnDx * s.mDnDx);
// float chi2Cut = 3.*3.*(s.mUpDx*s.mUpDx + s.mDnDx*s.mDnDx ); //SG
#ifdef GPUCA_GPUCODE
  GPUsharedref() const MEM_LOCAL(GPUTPCRow)& row = s.mRow;
  GPUsharedref() const MEM_LOCAL(GPUTPCRow)& rowUp = s.mRowUp;
  GPUsharedref() const MEM_LOCAL(GPUTPCRow)& rowDn = s.mRowDown;
#else
  GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& row = tracker.Row(s.mIRow);
  GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& rowUp = tracker.Row(s.mIRowUp);
  GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& rowDn = tracker.Row(s.mIRowDn);
#endif
  const float y0 = row.Grid().YMin();
  const float z0 = row.Grid().ZMin();
  const float stepY = row.HstepY();
  const float stepZ = row.HstepZ();

  for (int ih = iThread; ih < s.mNHits; ih += nThreads) {
    int linkUp = -1;
    int linkDn = -1;

    if (s.mDnNHits > 0 && s.mUpNHits > 0) {
// coordinates of the hit in the current row
#if defined(GPUCA_TEXTURE_FETCH_NEIGHBORS)
      cahit2 tmpval = tex1Dfetch(gAliTexRefu2, ((char*)tracker.Data().HitData() - tracker.Data().GPUTextureBase()) / sizeof(cahit2) + row.HitNumberOffset() + ih);
      const float y = y0 + tmpval.x * stepY;
      const float z = z0 + tmpval.y * stepZ;
#else
      const float y = y0 + tracker.HitDataY(row, ih) * stepY;
      const float z = z0 + tracker.HitDataZ(row, ih) * stepZ;
#endif // GPUCA_TEXTURE_FETCH_NEIGHBORS

#if GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP < GPUCA_MAXN
      calink neighUp[GPUCA_MAXN - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP];
      float yzUp[GPUCA_MAXN - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP];
      float yzUp2[GPUCA_MAXN - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP];
#endif // GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP > 0

      int nNeighUp = 0;
      GPUTPCHitArea areaDn, areaUp;

      const float kAngularMultiplier = tracker.Param().rec.SearchWindowDZDR;
      const float kAreaSize = tracker.Param().rec.NeighboursSearchArea;
      areaUp.Init(rowUp, tracker.Data(), y * s.mUpTx, kAngularMultiplier != 0.f ? z : (z * s.mUpTx), kAreaSize, kAngularMultiplier != 0.f ? (s.mUpDx * kAngularMultiplier) : kAreaSize);
      areaDn.Init(rowDn, tracker.Data(), y * s.mDnTx, kAngularMultiplier != 0.f ? z : (z * s.mDnTx), kAreaSize, kAngularMultiplier != 0.f ? (-s.mDnDx * kAngularMultiplier) : kAreaSize);

      do {
        GPUTPCHit h;
        int i = areaUp.GetNext(tracker, rowUp, tracker.Data(), &h);
        if (i < 0) {
          break;
        }

#if GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP < GPUCA_MAXN
#if GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP == 0
        if (true) {
#else
        if ((unsigned int)nNeighUp >= GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP) {
#endif
          neighUp[nNeighUp - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP] = (calink)i;
          yzUp[nNeighUp - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP] = s.mDnDx * (h.Y() - y);
          yzUp2[nNeighUp - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP] = s.mDnDx * (h.Z() - z);
        } else
#endif
        {
#if GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP > 0
          s.mB[nNeighUp][iThread] = (calink)i;
          s.mA1[nNeighUp][iThread] = s.mDnDx * (h.Y() - y);
          s.mA2[nNeighUp][iThread] = s.mDnDx * (h.Z() - z);
#endif
        }
        if (++nNeighUp >= GPUCA_MAXN) {
          // GPUInfo("Neighbors buffer ran full...");
          break;
        }
      } while (1);

      if (nNeighUp > 0) {
        int bestDn = -1, bestUp = -1;
        float bestD = 1.e10f;

        int nNeighDn = 0;
        do {
          GPUTPCHit h;
          int i = areaDn.GetNext(tracker, rowDn, tracker.Data(), &h);
          if (i < 0) {
            break;
          }

          nNeighDn++;
          float2 yzdn = CAMath::MakeFloat2(s.mUpDx * (h.Y() - y), s.mUpDx * (h.Z() - z));

          for (int iUp = 0; iUp < nNeighUp; iUp++) {
#if GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP > 0 && GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP < GPUCA_MAXN
            float2 yzup = iUp >= GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP ? CAMath::MakeFloat2(yzUp[iUp - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP], yzUp2[iUp - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP]) : CAMath::MakeFloat2(s.mA1[iUp][iThread], s.mA2[iUp][iThread]);
#elif GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP == GPUCA_MAXN
            float2 yzup = CAMath::MakeFloat2(s.mA1[iUp][iThread], s.mA2[iUp][iThread]);
#else
            float2 yzup = CAMath::MakeFloat2(yzUp[iUp], yzUp2[iUp]);
#endif
            float dy = yzdn.x - yzup.x;
            float dz = yzdn.y - yzup.y;
            float d = dy * dy + dz * dz;
            if (d < bestD) {
              bestD = d;
              bestDn = i;
              bestUp = iUp;
            }
          }
        } while (1);

        if (bestD <= chi2Cut) {
#if GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP > 0 && GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP < GPUCA_MAXN
          linkUp = bestUp >= GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP ? neighUp[bestUp - GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP] : s.mB[bestUp][iThread];
#elif GPUCA_NEIGHBOURS_FINDER_MAX_NNEIGHUP == GPUCA_MAXN
          linkUp = s.mB[bestUp][iThread];
#else
          linkUp = neighUp[bestUp];
#endif
          linkDn = bestDn;
        }
      }
    }

    tracker.SetHitLinkUpData(row, ih, linkUp);
    tracker.SetHitLinkDownData(row, ih, linkDn);
  }
}