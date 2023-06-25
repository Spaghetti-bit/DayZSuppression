modded class MissionServer extends MissionBase
{
	private float m_suppressionAmount;
	//private SuppressionTrigger m_SuppressionTrigger;

    void ~MissionServer()
    {
    }
    void MissionServer()
    {
        if (GetRPCManager().AddRPC("Suppression", "RPC_SuppressionApply", this, SingleplayerExecutionType.Server)) 
        {
            Print("[ Suppression Mod ]: Suppression Trigger System: Started!");
        }
    }
	// (Server -> Client)
	void RPC_SuppressionApply(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		//TODO:
		//Print("[Spaghetti Suppression] :: Debug :: RPC_SuppressionApply called from Server.");
		if(type != CallType.Server)
			return;
		Param2<PlayerBase, ESuppressionStamina> data;
    	//if the data is not retrieved we return to avoid issue
		if (!ctx.Read(data))
			return;

		PlayerBase player;
		ESuppressionStamina suppressionAmount = data.param2;
		//Print("[ Spaghetti Suppression ] :: Debug :: RPC Server Side :: Suppression Amount: " + suppressionAmount);
		if( PlayerBase.CastTo( player, data.param1 ))
		{
			// in units (how much depletes stamina)
			//=====================================
			//				STAMINA
			//=====================================
			player.GetStaminaHandler().SetStamina(player.GetStaminaHandler().GetStamina() - suppressionAmount);
			//=====================================
			//			SUPPRESSION LEVEL
			//=====================================
			player.ApplySuppression(suppressionAmount);

			//Param1<PlayerBase> params;
			// Player to apply PPE to and the amount to apply.
			//params = new Param1<PlayerBase>(player);
			//Server tracking of suppression for modifier
				//Param2<PlayerBase, float> params2;
			// Player to apply PPE to and the amount to apply.
				//params2 = new Param2<PlayerBase, float>(player, suppressionAmount);
			//Print("[ Suppression Mod ] :: Debug :: RPC Server Side :: Suppression Level: " + player.GetSuppressionLevel());
		
			//Client Side
				//GetRPCManager().SendRPC("Suppression", "RPC_Client_SetSuppression", params2);
			//GetRPCManager().SendRPC("Suppression", "RPC_SendPPEStartToClient", params);
		}
	}
};