modded class MissionServer extends MissionBase
{
	private float m_suppressionAmount;

    void ~MissionServer()
    {
    }
    void MissionServer()
    {
        if (GetRPCManager().AddRPC("Suppression", "CreateSuppressionTriggerEx", this, SingleplayerExecutionType.Both))
        {
            Print("[ Suppression Mod ]: Suppression Trigger System: Started!");
        }
		GetRPCManager().AddRPC("Suppression", "RPC_SuppressionApply", this, SingleplayerExecutionType.Server);
    }

	// (Server -> Client)
    void CreateSuppressionTriggerEx(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
    	//if this function is trigger anywhere but on server, we return without continuing.
		if(type != CallType.Server)
			return;

		Param2<vector, float> data;
    	//if the data is not retrieved we return to avoid issue
		if (!ctx.Read(data)) 
			return;

		if(sender == null)
			return;

        vector clientResponse = data.param1;
		float suppressionAmountFromTrigger = data.param2;
		m_suppressionAmount = data.param2;
		if ( GetGame().IsServer() )
		{
			SuppressionTrigger suppressionTrigger = GetGame().CreateObject("SuppressionTrigger", clientResponse);
		}
		
	}

	// (Server -> Client)
	void RPC_SuppressionApply(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type != CallType.Server)
			return;

		Param2<PlayerBase, float> data;
    	//if the data is not retrieved we return to avoid issue
		if (!ctx.Read(data))
			return;

		PlayerBase player;
		float suppressionAmount = data.param2;
		//Print("[ Suppression Mod ]: SuppressionAPPLY: Suppression AMT: " + suppressionAmount);
		if( PlayerBase.CastTo( player, data.param1 ))
		{
			// in units (how much depletes stamina)
			player.GetStaminaHandler().DepleteStamina( m_suppressionAmount / 10 );
			
			Param2<PlayerBase, float> params;
			// Player to apply PPE to and the amount to apply.
			params = new Param2<PlayerBase, float>(player, m_suppressionAmount);
			GetRPCManager().SendRPC("Suppression", "RPC_SendPPEStartToClient", params);
		}
	}
};