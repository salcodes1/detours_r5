#include "core/stdafx.h"
#include "core/logdef.h"
#include "tier0/cvar.h"
#include "vpc/basefilesystem.h"
#include "gameui/IConsole.h"

//---------------------------------------------------------------------------------
// Purpose: prints the output of the filesystem based on the warning level
//---------------------------------------------------------------------------------
void HCBaseFileSystem_Warning(void* thisptr, FileWarningLevel_t level, const char* fmt, ...)
{
	if (fs_warning_level_native->GetInt() < (int)level)
	{
		return;
	}

	static char buf[1024] = {};

	static std::shared_ptr<spdlog::logger> iconsole = spdlog::get("game_console");
	static std::shared_ptr<spdlog::logger> wconsole = spdlog::get("win_console");
	static std::shared_ptr<spdlog::logger> fslogger = spdlog::get("filesystem_warn_logger");

	{/////////////////////////////
		va_list args{};
		va_start(args, fmt);

		vsnprintf(buf, sizeof(buf), fmt, args);

		buf[sizeof(buf) - 1] = 0;
		va_end(args);
	}/////////////////////////////

	fslogger->debug(buf);

	if (fs_show_warning_output->GetBool())
	{
		wconsole->debug(buf);
#ifndef DEDICATED
		g_spd_sys_w_oss.str("");
		g_spd_sys_w_oss.clear();

		iconsole->debug(buf);

		std::string s = g_spd_sys_w_oss.str();

		g_pIConsole->m_ivConLog.push_back(Strdup(s.c_str()));
#endif // !DEDICATED
	}
}

void CBaseFileSystem_Attach()
{
	DetourAttach((LPVOID*)&CBaseFileSystem_Warning, &HCBaseFileSystem_Warning);
}

void CBaseFileSystem_Detach()
{
	DetourDetach((LPVOID*)&CBaseFileSystem_Warning, &HCBaseFileSystem_Warning);
}
