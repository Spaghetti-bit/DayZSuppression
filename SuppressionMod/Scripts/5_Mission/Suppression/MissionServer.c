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
		/*
		if (GetRPCManager().AddRPC("Suppression", "RPC_ShootSCBFromPlayer", this, SingleplayerExecutionType.Both))
        {
            Print("[ Suppression Mod ]: SphereBulletCast System: Started!");
        }
		*/

		GetRPCManager().AddRPC("Suppression", "RPC_SuppressionApply", this, SingleplayerExecutionType.Server);
    }

	// (Server -> Client)
    void CreateSuppressionTriggerEx(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        //Print("[ Suppression Mod ]: CreateSuppressionTrigger Called!");
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
			if(suppressionTrigger)
			{
				//suppressionTrigger.SetSuppressionAmount(suppressionAmountFromTrigger);
				//suppressionTrigger.SetIgnoreEntity(sender.GetPlayer());
				//Print("[ Suppression Mod ]: MissionServer.c : Trigger created.");
			}
			else
			{
				Print("[ Suppression Mod ]: Trigger Failed to create.");
			}
		}
		
	}

	// Not functioning due to DayZ engine limitations with Raycasting.
	void RPC_ShootSCBFromPlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		//Print("[ Suppression Mod ]: Attempting to apply suppression.");
		if(type != CallType.Server)
			return;
		// Beg, End, Suppression Amount
		Param3<vector, vector, float> data;
    	//if the data is not retrieved we return to avoid issue
		if (!ctx.Read(data))
			return;
		// No sender... Sus.
		if(sender == null)
			return;
		PhxInteractionLayers layerMask = PhxInteractionLayers.CHARACTER  /*| PhxInteractionLayers.TERRAIN | PhxInteractionLayers.BUILDING | PhxInteractionLayers.ROADWAY */;
		vector beg_Pos = data.param1;
		// Move beg_pos 1 unit forward to avoid collision with gun.
		beg_Pos = beg_Pos + vector.Forward;
		vector end_Pos = data.param2;
		float suppressionAmt = data.param3;
		vector hit_pos;
		Object hitObjectSCB;
		DayZPhysics.SphereCastBullet(beg_Pos, end_Pos, 0.6, layerMask, null, hitObjectSCB, hit_pos, null, null);
		if (hit_pos != vector.Zero)
		{
			if(hitObjectSCB)
			{
				Print("[ Suppression Mod ]: MissionServer.c | Hit Object SCB : " + hitObjectSCB.GetType());
			}
			//Param2<vector, float> pos; // The value to be sent through RPC

			// Data update.
			//float ammoTypeToSuppression = CartridgeTypeToSuppression(bulletCartType);
			//pos = new Param2<vector, float>(hit_pos, ammoTypeToSuppression);
			Param2<vector, float> params = new Param2<vector, float>(hit_pos, suppressionAmt);
			m_suppressionAmount = suppressionAmt;
			
			//Print("[ Suppression Mod ]: MissionServer.c | RPC_SCBFromPlayer | Suppression Amount : " + suppressionAmt);
			Param2<PlayerBase, float> paramsPlayerTest = new Param2<PlayerBase, float>(sender.GetPlayer(), suppressionAmt);
			GetRPCManager().SendRPC("Suppression", "RPC_SuppressionApply", paramsPlayerTest);
			//CreateSuppressionTriggerEx(type, params, sender, target);
			//GetRPCManager().SendRPC("Suppression", "CreateSuppressionTriggerEx", params);
			//Sending RPC (Client -> Server)
			//GetRPCManager().SendRPC("Suppression", "CreateSuppressionTriggerEx", pos);
		}
	}

	// (Server -> Client)
	void RPC_SuppressionApply(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		//Print("[ Suppression Mod ]: Attempting to apply suppression.");
		if(type != CallType.Server)
			return;

		Param2<PlayerBase, float> data;
    	//if the data is not retrieved we return to avoid issue
		if (!ctx.Read(data))
			return;

		// if(sender == null)
		// 	return;
		
		PlayerBase player;
		float suppressionAmount = data.param2;
		//Print("[ Suppression Mod ]: SuppressionAPPLY: Suppression AMT: " + suppressionAmount);
		if( PlayerBase.CastTo( player, data.param1 ))
		{
			// in units (how much evade depletes stamina)
			//player.GetStaminaHandler().DepleteStamina( EStaminaModifiers.MELEE_LIGHT );
			//player.GetStaminaHandler().DepleteStamina( 0.1 );
			//Print("[ Suppression Mod ]: MissionServer.c | RPC_SuppressionApply : Suppression Amount: " + m_suppressionAmount);
			player.GetStaminaHandler().DepleteStamina( m_suppressionAmount / 10 );
			//Print("[ Suppression Mod ]: MissionServer.c | Applied Suppression: " + m_suppressionAmount);
			Param2<PlayerBase, float> params;
			// Player to apply PPE to and the amount to apply.
			params = new Param2<PlayerBase, float>(player, m_suppressionAmount);
			// PPE EFFECTS AND SUPPRESSION ADDITIONS SHOULD BE APPLIED ON THE CLIENT IN MISSIONGAMEPLAY.c
			
			/* Client side effects.
			player.SpawnShockEffect( 0.7 );
			player.ApplySuppression( data.param2 );
			//player.ApplySuppression( 5 );
			//player.ApplySuppression( data.param2 );
			player.GetCurrentCamera().SpawnCameraShake( 0.7 );
			*/
			GetRPCManager().SendRPC("Suppression", "RPC_SendPPEStartToClient", params);
			//PPERequesterBank.GetRequester(PPERequester_ShockHitReaction).Start();
		}
	}
};