modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        // TODO: Remove print when not testing.
        Print(" [ Suppression Mod ] : File System dir: " + SUPPRESSION_ROOT_DIR);
        MakeDirectory(SUPPRESSION_ROOT_DIR);

        if (!FileExist(SUPPRESSION_EARPROTECTION_CONFIG))
        {
            FileHandle file = OpenFile(SUPPRESSION_EARPROTECTION_CONFIG, FileMode.WRITE)
            if (file != 0)
            {
                FPrintln(file, "TankerHelmet");  
                CloseFile(file);
            }
        }
        //SupFileManager.LoadConfig();
    }
}
