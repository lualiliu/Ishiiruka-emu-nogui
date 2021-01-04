// Copyright 2009 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "Common/CommonTypes.h"
//#include "Common/GL/GLInterfaceBase.h"
#include "Common/Logging/Log.h"
#include "Core/HW/Memmap.h"
#include "VideoBackends/Software/EfbCopy.h"
#include "VideoBackends/Software/EfbInterface.h"
#include "VideoBackends/Software/TextureEncoder.h"

#include "VideoCommon/BPMemory.h"
#include "VideoCommon/Fifo.h"

static const float s_gammaLUT[] =
{
    1.0f,
    1.7f,
    2.2f,
    1.0f
};

namespace EfbCopy
{

static void CopyToRam()
{
  u8 *dest_ptr = Memory::GetPointer(bpmem.copyTexDest << 5);

  TextureEncoder::Encode(dest_ptr);
}

void ClearEfb()
{
  u32 clearColor = (bpmem.clearcolorAR & 0xff) << 24 | bpmem.clearcolorGB << 8 | (bpmem.clearcolorAR & 0xff00) >> 8;

  int left = bpmem.copyTexSrcXY.x;
  int top = bpmem.copyTexSrcXY.y;
  int right = left + bpmem.copyTexSrcWH.x;
  int bottom = top + bpmem.copyTexSrcWH.y;

  for (u16 y = top; y <= bottom; y++)
  {
    for (u16 x = left; x <= right; x++)
    {
      EfbInterface::SetColor(x, y, (u8*)(&clearColor));
      EfbInterface::SetDepth(x, y, bpmem.clearZValue);
    }
  }
}

void CopyEfb()
{
  EFBRectangle rc;
  rc.left = (int)bpmem.copyTexSrcXY.x;
  rc.top = (int)bpmem.copyTexSrcXY.y;

  // flipper represents the widths internally as last pixel minus starting pixel, so
  // these are zero indexed.
  rc.right = rc.left + (int)bpmem.copyTexSrcWH.x + 1;
  rc.bottom = rc.top + (int)bpmem.copyTexSrcWH.y + 1;
  
  CopyToRam(); // FIXME: should use the rectangle we have already created above
  
}
}
