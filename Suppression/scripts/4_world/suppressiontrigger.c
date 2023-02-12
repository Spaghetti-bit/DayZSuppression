//! Trigger only accepting Object which IsMan()
class SuppressionTrigger : ManTrigger
{
	private PlayerBase m_ignoreObj;
	private CartridgeType m_CartridgeType;
	private float m_suppressionAmount;

	override void EOnInit(IEntity other, int extra)
    {
		// Extents, how big the trigger is.
        const vector mins = "-0.2 -0.05 -0.2";
        const vector maxs = ".2 .1 .2";
        SetExtents(mins, maxs);
		
		// Debug -- Client Side
		if ( !GetGame().IsServer())
		{
			//NotificationSystem.AddNotificationExtended(1, "Suppression System: Trigger", "Trigger created at @ [X, Y, Z]: " + triggerPosStr);
		}
		// Automatically deleted after 75ms.
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DeleteSafe, 75);
    }
	override void OnEnter( Object obj )
	{
		//Print("[ Suppression Mod ]: (DEBUG: Trigger) Object Entered!");
		super.OnEnter( obj );
		PlayerBase player;
		if( Class.CastTo( player, obj ))
		{
			if (player.GetIdentity() != null)
			{
				//Param2<PlayerBase, float> parPlayer = new Param2<PlayerBase, float>(player, m_suppressionAmount);
				// SUPPRESSION DEBUG:
				//Print("[ Suppression Mod ]: Trigger Suppression stored amount: " + parPlayer.param2);
				//GetRPCManager().SendRPC("Suppression", "RPC_SuppressionApply", parPlayer);

				{
					// in units (how much evade depletes stamina)
					//player.GetStaminaHandler().DepleteStamina( EStaminaModifiers.MELEE_LIGHT );
					//player.GetStaminaHandler().DepleteStamina( 0.1 );
					//Print("[ Suppression Mod ]: MissionServer.c | RPC_SuppressionApply : Suppression Amount: " + m_suppressionAmount);
					//Print("[ Suppression Mod ]: MissionServer.c | Applied Suppression: " + m_suppressionAmount);
					Param2<PlayerBase, float> params;
					params = new Param2<PlayerBase, float>(player, m_suppressionAmount);
					//player.SpawnShockEffect( 0.7 );
					//player.ApplySuppression( m_suppressionAmount );
					//player.ApplySuppression( 5 );
					//player.ApplySuppression( data.param2 );
					//player.GetCurrentCamera().SpawnCameraShake( 0.7 );
					GetRPCManager().SendRPC("Suppression", "RPC_SuppressionApply", params);
					//PPERequesterBank.GetRequester(PPERequester_ShockHitReaction).Start();
				}
			}
		}
		DeleteSafe();
	}

	void SetIgnoreEntity( PlayerBase obj )
	{
		m_ignoreObj = obj;
	}

	void SetSuppressionAmount( float amt )
	{
		m_suppressionAmount = amt;
		//Print("[ Suppression Mod ]: created with suppression value: " + m_suppressionAmount);
	}
};