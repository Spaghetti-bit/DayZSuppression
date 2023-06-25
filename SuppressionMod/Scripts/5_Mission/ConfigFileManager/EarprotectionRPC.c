#ifdef SERVER
[CF_RegisterModule(SuppressionSettingsModule)]
#endif

class SuppressionSettingsModule : CF_ModuleWorld
{
    override void OnInit()
    {
        super.OnInit();

        EnableClientPrepare();
        InitConfiguration();
    }

    void InitConfiguration()
    {
        SuppressionConfigSettings config = SuppressionConfigSettings.GetInstance();

        if (FileExist(SUPPRESSION_SUPPRESSION_SETTINGS_CONFIG))
            JsonFileLoader<SuppressionConfigSettings>.JsonLoadFile(SUPPRESSION_SUPPRESSION_SETTINGS_CONFIG, config);

        JsonFileLoader<SuppressionConfigSettings>.JsonSaveFile(SUPPRESSION_SUPPRESSION_SETTINGS_CONFIG, config);
    }

    override void OnClientPrepare(Class sender, CF_EventArgs args)
    {
        super.OnClientPrepare(sender, args);

        auto cArgs = CF_EventPlayerPrepareArgs.Cast(args);
        SuppressionConfigSettings.SyncDataSend(cArgs.Identity);
    }
}