modded class MissionGameplay
{
    void MissionGameplay()
    {
        GetRPCManager().AddRPC("Suppression", "GetConfigResponse", this, SingleplayerExecutionType.Client);
    }

    void GetConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type != CallType.Client)
            return;

        Param1<TStringArray> data;

        if (!ctx.Read(data))
            return;

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if(player)
        {
            player.earProtectionItems = data.param1;
        }
    }
}

modded class MissionServer
{
    override void InvokeOnConnect (PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player,identity);
        GetRPCManager().SendRPC("Suppression", "GetConfigResponse", new Param1<TStringArray>(SupFileManager.ReadFileLines(SUPPRESSION_EARPROTECTION_CONFIG)), true, identity);
    }
}

