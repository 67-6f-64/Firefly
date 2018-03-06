#pragma once

namespace firefly
{
	namespace opcode
	{
		namespace in
		{
			enum // socket
			{
				LP_CheckPasswordResult			= 0x0000,
				LP_WorldInformation,
				
				LP_SetPhysicalWorldID			= 0x0005,
				LP_SelectWorldResult,
				LP_SelectCharacterResult,
				LP_AccountInfoResult,

				LP_AliveReq						= 0x0012,
				LP_SecurityPacket				= 0x0016,
				LP_HeaderEncryption				= 0x0027,
			};
		}

		namespace out
		{
			enum // socket
			{
				CP_SecurityPacket				= 0x0066,
				CP_PermissionRequest,

				CP_CheckLoginAuthInfo			= 0x0069,
				CP_SelectWorld,
				CP_CheckSPWRequest,
				CP_ClientLodingTimeLog,

				CP_MigrateIn					= 0x006E,
				CP_SelectCharacter,

				CP_WorldInfoRequest				= 0x0072,
				CP_WorldInfoForShiningRequest,

				CP_AliveAck						= 0x0093,
			};
		}
	}
}