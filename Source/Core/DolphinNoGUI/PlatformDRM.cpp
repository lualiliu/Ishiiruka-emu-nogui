// Copyright 2020 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <unistd.h>

#include "DolphinNoGUI/Platform.h"

#include "Common/MsgHandler.h"
#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/State.h"

#include "Core/HW/GCPad.h"
#include "InputCommon/GCPadStatus.h"
//#include <fmt/format.h>
#include "Core/Config/GraphicsSettings.h"
#include "VideoCommon/VideoConfig.h"

#include <climits>
#include <cstdio>

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "VideoCommon/RenderBase.h"

namespace
{
class PlatformDRM : public Platform
{
public:
  ~PlatformDRM() override;

  bool Init() override;
  void SetTitle(const std::string& string) override;
  void MainLoop() override;

  WindowSystemInfo GetWindowSystemInfo() const override;
};

PlatformDRM::~PlatformDRM() = default;

bool PlatformDRM::Init()
{
  return true;
}

void PlatformDRM::SetTitle(const std::string& string)
{
  std::fprintf(stdout, "%s\n", string.c_str());
}

void PlatformDRM::MainLoop()
{
  while (IsRunning())
  {
    static int hotkey = 0;
    static int slot = 0;
    static int stereo = 0;

    UpdateRunningFlag();
    Core::HostDispatchJobs();

    if(Pad::IsInitialized()) {
      GCPadStatus x = Pad::GetStatus(0);

      if( (x.button & PAD_TRIGGER_R) == PAD_TRIGGER_R) { // hotkey pressed
	if(hotkey == 1) {
	  hotkey = 2;
	}
      } else {
	hotkey = 1; // assure hotkey is released between actions
      }

      if(hotkey == 2) { // hotkey pressed
	if( (x.button & PAD_BUTTON_START) == PAD_BUTTON_START) {
	  RequestShutdown();
	  hotkey = 0;
	}

	if( (x.button & PAD_BUTTON_Y) == PAD_BUTTON_Y) {
	  State::Load(slot);
	  hotkey = 0;
	}
	if( (x.button & PAD_BUTTON_B) == PAD_BUTTON_B) {
	  State::Save(slot);
	  hotkey = 0;
	}
	if( (x.button & PAD_BUTTON_DOWN) == PAD_BUTTON_DOWN) {
	  if(slot > 0) slot--;
	  //Core::DisplayMessage(fmt::format("Slot {} selected", slot), 4000);
	  hotkey = 0;
	}
	if( (x.button & PAD_BUTTON_UP) == PAD_BUTTON_UP) {
	  if(slot < 10) slot++;
	  //Core::DisplayMessage(fmt::format("Slot {} selected", slot), 4000);
	  hotkey = 0;
	}
	if( (x.button & PAD_TRIGGER_L) == PAD_TRIGGER_L) {
	  Core::SaveScreenShot();
	  hotkey = 0;
	}
      }

    }
    //

    // TODO: Is this sleep appropriate?
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

WindowSystemInfo PlatformDRM::GetWindowSystemInfo() const
{
  WindowSystemInfo wsi;
  wsi.type = WindowSystemType::DRM;
  wsi.display_connection = nullptr;  // EGL_DEFAULT_DISPLAY
  wsi.render_window = nullptr;
  wsi.render_surface = nullptr;
  return wsi;
}
}  // namespace

std::unique_ptr<Platform> Platform::CreateDRMPlatform()
{
  return std::make_unique<PlatformDRM>();
}
