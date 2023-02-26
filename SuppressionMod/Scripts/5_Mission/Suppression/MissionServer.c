modded class MissionGameplay extends MissionBase
{
    void MissionGameplay()
    {
        // Register server->client RPCs
        GetRPCManager().AddRPC("Suppression", "RPC_SendPPEStartToClient", this, SingeplayerExecutionType.Client);
    }
    // Client Side ( Server -> Client )
    void RPC_SendPPEStartToClient(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
    {
        if (type != CallType.Client)
            return;

        Param2<PlayerBase, float> data;
        if (!ctx.Read(data))
            return;

        PlayerBase player;

        if (Class.CastTo(player, data.param1))
        {
            player.SpawnShockEffect(0.7);
            player.ApplySuppression(data.param2);
            player.GetCurrentCamera().SpawnCameraShake(0.7);
        }
    }
};
