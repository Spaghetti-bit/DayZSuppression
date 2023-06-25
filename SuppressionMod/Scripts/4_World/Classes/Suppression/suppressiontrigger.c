// class SuppressionTrigger : ManTrigger
// {
// 	private PlayerBase m_ignoreObj;
// 	private CartridgeType m_CartridgeType;
// 	private float m_suppressionAmount;

// 	override void EOnInit(IEntity other, int extra)
//     {
// 		// Extents, how big the trigger is.
//         //const vector mins = "-0.2 -0.05 -0.2";
//         //const vector maxs = ".2 .1 .2";
// 		//Print("[Spaghetti Suppression] :: Trigger Created.");
// 		const vector mins = "-0.75 -0.75 -0.75";
//         const vector maxs = "0.75 0.75 0.75";
//         SetExtents(mins, maxs);
		
// 		// Automatically deleted after 2000ms.
// 		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DeleteSafe, 10000);
//     }
// 	override void OnEnter( Object obj )
// 	{
		
// 		super.OnEnter( obj );
// 		PlayerBase player;
// 		if( Class.CastTo( player, obj ))
// 		{
// 			//if (player.GetIdentity() != null)
// 			{
// 				Param2<PlayerBase, ESuppressionStamina> params;
// 				params = new Param2<PlayerBase, ESuppressionStamina>(player, 200);
// 				//Print("[Spaghetti Suppression Triggers] :: Debug :: Suppression Applied: " + 200);
// 				GetRPCManager().SendRPC("Suppression", "RPC_SuppressionApply", params);
// 			}
// 		}
// 		//DeleteSafe();
// 	}
// 	void SetIgnoreEntity( PlayerBase obj )
// 	{
// 		m_ignoreObj = obj;
// 	}



// 	void SetSuppressionAmount( float amt )
// 	{
// 		m_suppressionAmount = amt;
// 		SetSynchDirty();
// 	}
// };
// class SpaghettiTrigger : Trigger
// {
// 	const vector mins = "-2 -2 -2";
// 	const vector maxs = "2 2 2";
// 	override void EOnInit(IEntity other, int extra)
//     {
// 		SetExtents(mins, maxs);
// 	}
// 	override void EOnEnter(IEntity other, int extra)
// 	{
// 		EntityAI entity = EntityAI.Cast(other);
// 		if (entity)
// 		{
// 			//Print("[Spaghetti Suppression Triggers] :: Object entered: " + entity.GetType());
// 		}
// 	}
// }