modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        //Set up Suppression Config Directory/File
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
    }
}
