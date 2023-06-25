modded class MissionGameplay
{
    void MissionGameplay()
    {
        GetRPCManager().AddRPC("Suppression", "GetEarProtectionConfig", this, SingleplayerExecutionType.Client);
    };

    void GetEarProtectionConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
            return;

        Param1<TStringArray> data;

        if (!ctx.Read(data))
            return;

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if (player)
        {
            player.earProtectionItems = data.param1;
        }
    };
};

modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        //Set up Suppression Config Directory/File
        Print("[Suppression Config] :: MissionServer.c :: Suppression Config Directory/File");
        MakeDirectory(SUPPRESSION_ROOT_DIR);

        //Uniformed way of setting up the files without this becomeing chaotic.
        SetUpEarProtection();
        //SetUpSuppressionConfig();
    };

    void SetUpEarProtection()
    {
        if (!FileExist(SUPPRESSION_EARPROTECTION_CONFIG))
        {
            FileHandle file = OpenFile(SUPPRESSION_EARPROTECTION_CONFIG, FileMode.WRITE);
            if (file != 0)
            {
                FPrintln(file, "TankerHelmet");
                CloseFile(file);
            }
        }
    };

    // void SetUpSuppressionConfig()
    // {
    //     if (!FileExist(SUPPRESSION_SUPPRESSION_SETTINGS_CONFIG))
    //     {
    //         FileHandle file = OpenFile(SUPPRESSION_SUPPRESSION_SETTINGS_CONFIG, FileMode.WRITE);
    //         if (file != 0)
    //         {
    //             CloseFile(file);
    //         }
    //     }
    // };

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        //Print("[Suppression Config] :: MissionServer.c :: InvokeOnConnect");
        GetRPCManager().SendRPC("Suppression", "GetEarProtectionConfig", new Param1<TStringArray>(SupFileManager.ReadFileLines(SUPPRESSION_EARPROTECTION_CONFIG)), true, identity);
    };
};
