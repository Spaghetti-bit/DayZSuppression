// modded class IngameHud
// {
// 	protected ref BuildingDbgUI m_BuildingDebug;

// 	void IngameHud()
// 	{
// 		m_BuildingDebug = new BuildingDbgUI();
// 	}

// 	override void Update(float timeslice)
// 	{
// 		super.Update(timeslice);

// 		if (m_BuildingDebug)
// 			m_BuildingDebug.Update(timeslice);
// 	}
// }

// class BuildingDbgUI
// {
// 	private PlayerBase m_Player;
    
// 	private ActionManagerClient m_ActionManager;

// 	void Update(float timeslice)
// 	{
// 		if (m_Player && !m_Player.IsAlive())
// 		{
// 			m_Player = null;
// 			m_ActionManager = null;
// 		}

// 		if (!m_Player)
// 			PlayerBase.CastTo(m_Player, GetGame().GetPlayer());
// 			ShowDebugInfo(m_Player);
// 	}

// 	void ShowDebugInfo(PlayerBase player)
// 	{
// 		DbgUI.Begin("Suppression Stats: ", 200, 0);
// 		//DbgUI.Text(String("Ear Protection : " +  player.CheckHearingProtection()));
// 		//DbgUI.Text(String("Ear Protection Item Name: " +  player.GetEarProtectionItemName()));
// 		DbgUI.Text(String("Suppression Level : " + player.GetSuppressionLevel()));
// 		DbgUI.Text(String("Hearing Loss Level : " + player.GetHearingLossLevel()));
// 		DbgUI.Text(String("Hearing Protection Item : " + player.GetEarProtectionItemName()));
// 		DbgUI.Text(String("Hearing Protection (T/F): " + player.IsEarProtected()));
// 		//TODO: Config testing.
// 		DbgUI.Text(String("==============================="));
// 		DbgUI.Text("Loaded File : " + SUPPRESSION_EARPROTECTION_CONFIG);
// 		DbgUI.Text("Ear Protection Array 1: " + player.GetEarprotectionArray()[0]);
// 		DbgUI.Text("Ear Protection Array 2: " + player.GetEarprotectionArray()[1]);
// 		DbgUI.Text("Ear Protection Array 3: " + player.GetEarprotectionArray()[2]);
// 		DbgUI.Text(String("==============================="));
		
// 		//foreach (string s : SupFileManager.GetEarprotectionArray())
// 		//{
// 		//	DbgUI.Text(String(s));
// 		//}
// 		DbgUI.End();
// 	}
// }