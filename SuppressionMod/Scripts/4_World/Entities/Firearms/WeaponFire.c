modded class WeaponFire extends WeaponStartAction
{
	override void OnEntry (WeaponEventBase e)
	{
		if ( GetGame().IsClient() )
		{
			PlayerBase player;
			PlayerBase.CastTo(player, e.m_player);
			if(player)
			{
				player.ApplyHearingLoss( 5 );
			}

			if (player.CanPlayerSuppress())
			{
				// Collect data from muzzle index (MI)
				int mi = m_weapon.GetCurrentMuzzle();
				AmmoData firedBulletData = Magazine.GetAmmoData( m_weapon.GetChamberAmmoTypeName( mi ));
				CartridgeType bulletCartType = firedBulletData.GetCartridgeType();
				
				vector barrelPosition = m_weapon.ModelToWorld(m_weapon.GetSelectionPositionLS( "konec hlavne" ));
				vector muzzlePosition = m_weapon.ModelToWorld(m_weapon.GetSelectionPositionLS( "usti hlavne" ));

				// !!!MAX DISTANCE SUPPRESSION CAN HAPPEN!!!
				float distance = 500;
				// !!!MAX DISTANCE SUPPRESSION CAN HAPPEN!!!


				vector end_point = muzzlePosition;
				vector begin_point = barrelPosition;

				vector aim_point = end_point - begin_point;
				aim_point = aim_point.Normalized() * distance;
				aim_point = aim_point + end_point;

				// Move beg_pos X unit(s) forward to avoid collision with gun.
				vector beg_Pos = end_point + (vector.Direction(barrelPosition, muzzlePosition ).Normalized() * 2.5);
				vector end_Pos = aim_point;
				//
				//GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(FindBullet, 10, false, muzzlePosition);
				vector hit_pos;
				vector hit_normal;
				vector hit_normalSCB;
				float hit_fraction; 
				float hit_fractionSCB; 
				Object hitObjectRCB;
				Object hitObjectSCB;
				PhxInteractionLayers layerMask = PhxInteractionLayers.CHARACTER;
				DayZPhysics.SphereCastBullet(beg_Pos, aim_point, 0.6, layerMask, player, hitObjectSCB, hit_pos, null, null);
				if (hitObjectSCB)
				{
					//Print("[Spaghetti Suppression] :: SphereCastBullet Hit OBJ: " + hitObjectSCB.GetType());
					PlayerBase hitPlayer = PlayerBase.Cast(hitObjectSCB);
					//if (hitPlayer && hitPlayer.GetIdentity() != null)
					if (CartridgeTypeToSuppression(bulletCartType) != ESuppressionStamina.SUPPPRESSION_PISTOL)
					{
						if (hitPlayer)
						{
							//if (bulletCartType != CartridgeType.Pistol)
							{
								player.SetPlayerCanSuppress(false);
								// Testing player can only suppress once every X / 1000 seconds OR X milliseconds
								GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(player.SetPlayerCanSuppress, 500, false, true);
								Param2<PlayerBase, ESuppressionStamina> params;
								// Player to apply PPE to and the amount to apply.
								ESuppressionStamina suppressionAmount = CartridgeTypeToSuppression(bulletCartType);
								params = new Param2<PlayerBase, ESuppressionStamina>(hitPlayer, suppressionAmount);
								//hitPlayer.ApplySuppression(suppressionAmount);
								GetRPCManager().SendRPC("Suppression", "RPC_SuppressionApply", params);
							}
						}
					}
				}
			}
		}
		super.OnEntry(e);
		/*TODO: Implement bullet based suppression.
		// Very intensive on the server sadly.
		//==================================================
						NOT IMPLEMENTED.
		//==================================================
		*/
		//GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(FindBullet, 50, false, CartridgeTypeToSuppression(bulletCartType), player);
	}
	ESuppressionStamina CartridgeTypeToSuppression(CartridgeType type)
	{
		// Values defined in enums. See estaminamodifiers.c
		ESuppressionStamina suppressionAmount = 0;
		switch (type)
		{
			case CartridgeType.None:
				return 0;
			case CartridgeType.Pistol:
				suppressionAmount = ESuppressionStamina.SUPPPRESSION_PISTOL;
				break;
			case CartridgeType.Intermediate:
				suppressionAmount = ESuppressionStamina.SUPPPRESSION_INTERMEDIATE;
				break;
			case CartridgeType.FullPower:
				suppressionAmount = ESuppressionStamina.SUPPPRESSION_FULLPOWER;
				break;
			case CartridgeType.Shell:
				suppressionAmount = ESuppressionStamina.SUPPPRESSION_SHELL;
				break;
			default:
				return 0;
		}
		return suppressionAmount;
	}
	/*
    //==================================================
                    NOT IMPLEMENTED.
    //==================================================
    */
	// void FindBullet(float suppressionAmountFromTrigger, PlayerBase player)
	// {
	// 	array<Object> objects = new array<Object>;
	// 	GetGame().GetObjectsAtPosition(m_weapon.GetWorldPosition(), 300, objects, NULL);
	// 	for (int i = 0; i < objects.Count(); ++i)
	// 	{
	// 		ObjectBullet checkObject = objects[i];
	// 		bool isBullet = (checkObject && BULLET_TYPES.Find(checkObject.ConfigGetString("model")) > -1 && !checkObject.HasBeenFound());
	// 		if (isBullet)
	// 		{
	// 			ObjectBullet bullet = objects[i];
	// 			if (bullet)
	// 			{
	// 				bullet.SetHasBeenFound(true);
	// 				bullet.SetIsBullet(true);
	// 				//Print("[Spaghetti Suppression] :: Weaponfire.c :: Bullet found.");
	// 				// Create trigger on bullet.
	// 				//TODO:
	// 				player.SetLastFiredBullet(bullet);
	// 				//Print("[Spaghetti Suppression] :: RPC Client Side PlayerBase (WeaponFire.c) :: Last Fired Bullet: " + player.GetLastFiredBullet());
	// 				CreateSuppressionTrigger(bullet, suppressionAmountFromTrigger);
	// 				//objects[i].AddChild(suppressionTrigger, -1);
	// 			}
	// 		}
	// 	}
	// }
	/*
    //==================================================
                    NOT IMPLEMENTED.
    //==================================================
    */
	// void CreateSuppressionTrigger(Object bullet, float suppressionAmountFromTrigger)
	// {
	// 	//ParticleSource p = ParticleManager.GetInstance().CreateParticle(ParticleList.ROADFLARE_BURNING_MAIN, bullet.GetPosition());
	// 	//Print("Model Path: " + bullet.ConfigGetString("model"));
	// 	//Print("Type: " + bullet.GetType());
	// 	//Print("=================================================");
	// 	// Server side stuff.
	// 	Param2<vector, float> params = new Param2<vector, float>(bullet.GetPosition(), suppressionAmountFromTrigger);
	// 	//Print("[Spaghetti Suppression] :: RPC Client Side :: Param1: " + params.param1);
	// 	//Print("=================================================");
	// 	// Send data to server.
	// 	GetRPCManager().SendRPC("Suppression", "CreateSuppressionTriggerEx", params);
	// }
	
}