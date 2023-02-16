modded class WeaponFire extends WeaponStartAction
{
	override void OnEntry (WeaponEventBase e)
	{
		if (e)
		{
			m_dtAccumulator = 0;

			int mi = m_weapon.GetCurrentMuzzle();
			if (TryFireWeapon(m_weapon, mi))
			{
				DayZPlayerImplement p;
				if (Class.CastTo(p, e.m_player))
					p.GetAimingModel().SetRecoil(m_weapon);
				m_weapon.OnFire(mi);
				vector barrelPosition = m_weapon.ModelToWorld(m_weapon.GetSelectionPositionLS( "konec hlavne" ));
				vector muzzlePosition = m_weapon.ModelToWorld(m_weapon.GetSelectionPositionLS( "usti hlavne" ));

				// !!!MAX DISTANCE SUPPRESSION CAN HAPPEN!!!
				float distance = 1100;
				// !!!MAX DISTANCE SUPPRESSION CAN HAPPEN!!!


				vector end_point = muzzlePosition;
				vector begin_point = barrelPosition;
				vector aim_point = end_point - begin_point;
				aim_point = aim_point.Normalized() * distance;
				aim_point = aim_point + end_point;

				vector beg_Pos = end_point + (vector.Direction(barrelPosition, muzzlePosition ).Normalized() * 2);
				
				// Move beg_pos 1 unit forward to avoid collision with gun.
				vector end_Pos = aim_point;

				vector hit_pos;
				Object hitObjectSCB;
				PhxInteractionLayers layerMask = PhxInteractionLayers.CHARACTER;


				//Client side shooting for RPC.
				if ( !GetGame().IsServer())
				{
					
					AmmoData firedBulletData = Magazine.GetAmmoData( m_weapon.GetChamberAmmoTypeName( mi ));
					CartridgeType bulletCartType = firedBulletData.GetCartridgeType();
					
					PlayerBase player;
					PlayerBase.CastTo(player, e.m_player);
					if(player)
					{
						player.ApplyHearingLoss( 3 );
					}
					// SphereCast Method
					DayZPhysics.SphereCastBullet(beg_Pos, aim_point, 0.6, layerMask, null, hitObjectSCB, hit_pos, null, null);

					if (hit_pos != vector.Zero)
					{
						Param2<vector, float> params; // The value to be sent through RPC

						// Data update.
						float ammoTypeToSuppression = CartridgeTypeToSuppression(bulletCartType);
						params = new Param2<vector, float>(hit_pos, ammoTypeToSuppression);
						//Sending RPC (Client -> Server)
						GetRPCManager().SendRPC("Suppression", "CreateSuppressionTriggerEx", params);
					}
					
				}
			}
	}
	super.OnEntry(e);
	}

	float CartridgeTypeToSuppression(CartridgeType type)
	{
		float suppressionAmount = 0;
		switch (type)
		{
			case CartridgeType.None:
				return 0;
			case CartridgeType.Pistol:
				suppressionAmount = 3;
				break;
			case CartridgeType.Intermediate:
				suppressionAmount = 5;
				break;
			case CartridgeType.FullPower:
				suppressionAmount = 20;
				break;
			case CartridgeType.Shell:
				suppressionAmount = 15;
				break;
			default:
				return 0;
		}
		return suppressionAmount;
	}
}