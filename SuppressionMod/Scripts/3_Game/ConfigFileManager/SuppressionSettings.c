class SuppressionConfigSettings
{

	//bool Suppression_Tinnitus_Disabled          = true;

	float Suppression_Normal_Decay	            = 10;
	float Suppression_Fast_Decay                = 15;
	float Suppresion_Protection_Decay		    = 40;
	
	float Suppression_Max_Value	                = 200;

	float Suppression_Muffle_Start              = 140;
	//float Suppression_Tinnitus_Start            = 175;


	void Copy(SuppressionConfigSettings config)
	{
		if (!config) return;

		//Suppression_Tinnitus_Disabled           = config.Suppression_Tinnitus_Disabled;
		Suppression_Normal_Decay	            = config.Suppression_Normal_Decay;
		Suppression_Fast_Decay                  = config.Suppression_Fast_Decay;
		Suppression_Max_Value	                = config.Suppression_Max_Value;
		Suppresion_Protection_Decay		        = config.Suppresion_Protection_Decay;
		Suppression_Muffle_Start                = config.Suppression_Muffle_Start;
		//Suppression_Tinnitus_Start              = config.Suppression_Tinnitus_Start;
	}


	static void SyncDataSend(PlayerIdentity identity)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(s_Instance);
		rpc.Send(null, SuppressionScriptRPC.Update, true, identity);
	}

	private static ref SuppressionConfigSettings s_Instance = new SuppressionConfigSettings();
	static SuppressionConfigSettings GetInstance()
	{
		if (!s_Instance)
			s_Instance = new SuppressionConfigSettings();

		return s_Instance;
	}
};