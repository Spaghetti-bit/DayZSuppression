class SuppressionTrigger : ManTrigger
{
	private PlayerBase m_ignoreObj;
	private CartridgeType m_CartridgeType;
	private float m_suppressionAmount;

	override void EOnInit(IEntity other, int extra)
	{
		// Extents, how big the trigger is.
		const vector mins = "-0.2 -0.05 -0.2";
		const vector maxs = ".2 .1 .2";
		SetExtents(mins, maxs);

		// Automatically deleted after 100ms.
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, 100);
	}
	override void OnEnter(Object obj)
	{
		super.OnEnter(obj);
		PlayerBase player;
		if (Class.CastTo(player, obj))
		{
			if (player.GetIdentity() != null)
			{
				Param2<PlayerBase, float> params;
				params = new Param2<PlayerBase, float>(player, m_suppressionAmount);
				GetRPCManager().SendRPC("Suppression", "RPC_SuppressionApply", params);
			}
		}
		DeleteSafe();
	}

	void SetIgnoreEntity(PlayerBase obj)
	{
		m_ignoreObj = obj;
	}

	void SetSuppressionAmount(float amt)
	{
		m_suppressionAmount = amt;
	}
};
