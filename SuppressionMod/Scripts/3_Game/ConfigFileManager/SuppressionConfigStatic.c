class SupFileManager
{
	static array<string> ReadFileLines(string path)
	{
		FileHandle file;
		string line;

		array<string> contents = {};

		file = OpenFile(path, FileMode.READ);
		while (FGets(file, line)> 0)
		{
			line.Trim();
			if (line != string.Empty)
			{
				contents.Insert(line);
				line = string.Empty;
			}
		}
		CloseFile(file);
		return contents;
	}
}