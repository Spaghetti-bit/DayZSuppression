modded class MissionServer extends MissionBase
{
	private float m_suppressionAmount;
	private SuppressionTrigger m_SuppressionTrigger;

    void ~MissionServer()
    {
    }
    void MissionServer()
    {
        if (GetRPCManager().AddRPC("Suppression", "RPC_SuppressionApply", this, SingleplayerExecutionType.Server)) 
        {
            Print("[ Suppression Mod ]: Suppression Trigger System: Started!");
        }
		
		/*
		//==================================================
						NOT IMPLEMENTED.
		//==================================================
		*/
		//GetRPCManager().AddRPC("Suppression", "CreateSuppressionTriggerEx", this, SingleplayerExecutionType.Server);
		//GetRPCManager().AddRPC("Suppression", "RPC_UpdateSuppressionTrigger", this, SingleplayerExecutionType.Server)
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
		if( PlayerBase.CastTo( player, data.param1 ))
		{
			// in units (how much depletes stamina)
			player.GetStaminaHandler().SetStamina(player.GetStaminaHandler().GetStamina() - suppressionAmount);
			//Print("[ Suppression Mod ] :: Debug :: RPC Server Side :: Suppression Amount: " + suppressionAmount);
			
			Param2<PlayerBase, ESuppressionStamina> params;
			// Player to apply PPE to and the amount to apply.
			params = new Param2<PlayerBase, ESuppressionStamina>(player, suppressionAmount);
			//Server tracking of suppression for modifier
			player.SetSuppressionLevel(player.GetSuppressionLevel() + suppressionAmount);
			//Print("[ Suppression Mod ] :: Debug :: RPC Server Side :: Suppression Level: " + player.GetSuppressionLevel());
			
			//Client Side
			GetRPCManager().SendRPC("Suppression", "RPC_SendPPEStartToClient", params);
		}
	}
	/*
    //==================================================
                    NOT IMPLEMENTED.
    //==================================================
    */
    void CreateSuppressionTriggerEx(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        //Print("[ Suppression Mod ]: CreateSuppressionTrigger Called!");
		//Print("[Spaghetti Suppression] :: RPC Server Side :: CreateSuppressionTrigger.");
    	//if this function is triggered anywhere but on server, we return without continuing.
		if(type != CallType.Server)
			return;

		Param2<vector, float> data;
    	//if the data is not retrieved we return to avoid issue
		if (!ctx.Read(data)) 
			return;

		if(sender == null)
			return;

		//Print("[Spaghetti Suppression] :: RPC Server Side :: Param1: " + data.param1);
		//Print("[Spaghetti Suppression] :: RPC Server Side :: Param2: " + data.param2);
		vector position = data.param1;
		float suppressionAmountFromTrigger = data.param2;

		if ( GetGame().IsServer() )
		{
				SuppressionTrigger suppressionTrigger = GetGame().CreateObjectEx("SuppressionTrigger", position, ECE_NONE);
				if(suppressionTrigger)
				{
					//Print("[Spaghetti Suppression] :: RPC Server Side :: Bullet Based Trigger created.");
					suppressionTrigger.SetSuppressionAmount(suppressionAmountFromTrigger);
					m_suppressionAmount = suppressionAmountFromTrigger;
					//Client Side
					Param1<SuppressionTrigger> params = new Param1<SuppressionTrigger>(suppressionTrigger);
					m_SuppressionTrigger = suppressionTrigger;
					GetRPCManager().SendRPC("Suppression", "RPC_AttachSuppressionTriggerToBullet", params);
					//bullet.AddChild(suppressionTrigger, -1);
				}
				else
				{
					Print("[ Suppression Mod ] :: RPC Server Side :: Trigger Failed to create.");
				}
		}
	}
	/*
    //==================================================
                    NOT IMPLEMENTED.
    //==================================================
    */
	void RPC_UpdateSuppressionTrigger(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<SuppressionTrigger> data;
		if (!ctx.Read(data)) 
			return;
		m_SuppressionTrigger = SuppressionTrigger.Cast(data.param1);
	}
};