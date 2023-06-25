modded class MissionGameplay extends MissionBase
{
    void MissionGameplay()
    {
        // Register server->client RPCs
        GetRPCManager().AddRPC("Suppression", "RPC_SendPPEStartToClient", this, SingeplayerExecutionType.Client);
        //GetRPCManager().AddRPC("Suppression", "RPC_Client_SetSuppression", this, SingeplayerExecutionType.Client);
        /*
        //==================================================
                        NOT IMPLEMENTED.
        //==================================================
        */
        //GetRPCManager().AddRPC("Suppression", "RPC_AttachSuppressionTriggerToBullet", this, SingeplayerExecutionType.Client);
    }
    // Client Side ( Server -> Client )
    // void RPC_Client_SetSuppression(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
    // {
    //     if(type != CallType.Client)
	// 		return;

    //     Param2<PlayerBase, float> data;
	// 	if (!ctx.Read(data)) 
	// 		return;
        
    //     PlayerBase player;
    //     if( Class.CastTo( player, data.param1 ))
	// 	{
    //         player.SetSuppressionLevel( data.param2 + player.GetSuppressionLevel());
    //         //Print("[ Suppression Mod ] :: Debug :: RPC Client Side :: Suppression Level: " + player.GetSuppressionLevel());
    //     }
    // }
    void RPC_SendPPEStartToClient(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
    {
        if(type != CallType.Client)
			return;

        Param1<PlayerBase> data;
		if (!ctx.Read(data)) 
			return;
        
        PlayerBase player;
        if (data.param1)
        {
            if( Class.CastTo( player, data.param1 ))
            {
                //player.SetSuppressionLevel( data.param2 + player.GetSuppressionLevel());
                //Print("[ Suppression Mod ] :: Debug :: RPC Client Side :: Suppression Level: " + player.GetSuppressionLevel());
                if (player.GetIdentity())
                {
                    player.SpawnShockEffect( 0.7 );
                    player.GetCurrentCamera().SpawnCameraShake( 0.7 );
                }
            }
        }
    }
};