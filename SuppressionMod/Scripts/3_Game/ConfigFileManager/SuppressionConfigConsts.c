static const string SUPPRESSION_ROOT_DIR = "$profile:Suppression/";
static const string SUPPRESSION_EARPROTECTION_CONFIG = SUPPRESSION_ROOT_DIR + "EarProtectionItems.txt";
static const string SUPPRESSION_SUPPRESSION_SETTINGS_CONFIG = SUPPRESSION_ROOT_DIR + "SuppressionSettings.json";

//!--------------------

enum SuppressionScriptRPC
{
	INVALID = -234234,
	Update
}

//!--------------------------------------------

modded class DayZGame
{
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);
		switch (rpc_type)
		{
			case SuppressionScriptRPC.Update:
				SM_RPC_UpdateSettings(ctx);
			break;
		}
	}

	protected void SM_RPC_UpdateSettings(ParamsReadContext ctx)
	{
		if (!IsMissionClient())
			return;
		
		SuppressionConfigSettings config;
		if (!ctx.Read(config))
		{
			Print("[SuppressionMod] :: OnRPC - Failed")
			return;
		}
		SuppressionConfigSettings.GetInstance().Copy(config);
	}
}

