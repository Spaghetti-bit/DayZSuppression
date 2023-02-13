class SupFileManager
{
	static ref array<string>      HEADSET_EARPLUG_ITEM_DISPLAY_NAME_ARRAY;
	static array<string> ReadFileLines(string path)
	{
		
		string line_content;

		array<string> contents = {};

		// $profile:Suppression/EarProtectionItems.txt
		FileHandle file = OpenFile(path, FileMode.READ);
		//TODO: dfdf
		if (file == 0 )
			return contents;
		
		Print("[ Suppression Mod ] File Manager: " + path);
		while (FGets(file, line_content) > 0)
		{
			Print("[ Suppression Mod ] File Manager: " + line_content);
			contents.Insert(line_content);
		}
		CloseFile(file);
		return contents;
	}

	static array<string> GetEarprotectionArray()
    {
        return HEADSET_EARPLUG_ITEM_DISPLAY_NAME_ARRAY;
    }

	static void LoadConfig()
	{
		HEADSET_EARPLUG_ITEM_DISPLAY_NAME_ARRAY = SupFileManager.ReadFileLines(SUPPRESSION_EARPROTECTION_CONFIG);
	}
}