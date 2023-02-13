modded class WeaponFire extends WeaponStartAction
{
	override void OnEntry (WeaponEventBase e)
	{
		if (e)
		{
			m_dtAccumulator = 0;
	
			//if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponFire bang!"); }
			//m_weapon.Fire();
			int mi = m_weapon.GetCurrentMuzzle();
			if (TryFireWeapon(m_weapon, mi))
			{
				
				

				DayZPlayerImplement p;
				if (Class.CastTo(p, e.m_player))
					p.GetAimingModel().SetRecoil(m_weapon);
				m_weapon.OnFire(mi);
				// SphereCastBullet(vector begPos, vector endPos, float radius, PhxInteractionLayers layerMask, Object ignoreObj, out Object hitObject, out vector hitPosition, out vector hitNormal, out float hitFraction);
				/*
					begPos = weapon muzzle 
					endPos = 1000m from muzzle
					PhxInteraction Layers mask
				*/
				vector barrelPosition = m_weapon.ModelToWorld(m_weapon.GetSelectionPositionLS( "konec hlavne" ));
				vector muzzlePosition = m_weapon.ModelToWorld(m_weapon.GetSelectionPositionLS( "usti hlavne" ));

				// !!!MAX DISTANCE SUPPRESSION CAN HAPPEN!!!
				float distance = 800;
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
				vector hit_normal;
				vector hit_normalSCB;
				// vector hit_dir;
				// int contactComponent;
				float hit_fraction; 
				float hit_fractionSCB; 
				Object hitObjectRCB;
				Object hitObjectSCB;
				PhxInteractionLayers layerMask = PhxInteractionLayers.CHARACTER  /*| PhxInteractionLayers.TERRAIN | PhxInteractionLayers.BUILDING | PhxInteractionLayers.ROADWAY */;
				// //set<Object> results;

				vector 		from 			=  end_point;
				//GetGame().GetCurrentCameraPosition();
				vector 		to 				= from + vector.Direction(barrelPosition, muzzlePosition ).Normalized() * distance;
				//from + (GetGame().GetCurrentCameraDirection() * distance);
				vector 		contact_pos;
				vector 		contact_dir;
				int 		contactComponent;
				set<Object> results;
				array<Object> nearObjects;
				array<Object> vicinityObjects;

				
				//Client side shooting for RPC.
				if ( !GetGame().IsServer())
				{
					
					AmmoData firedBulletData = Magazine.GetAmmoData( m_weapon.GetChamberAmmoTypeName( mi ));
					CartridgeType bulletCartType = firedBulletData.GetCartridgeType();
					//Print("[ Suppression Mod ]: WeaponFire.c : Cartridge Type: " + typename.EnumToString(CartridgeType, bulletCartType));
					PlayerBase player;
					PlayerBase.CastTo(player, e.m_player);
					if(player)
					{
						player.ApplyHearingLoss( 3 );
						//float ammoTypeToSuppression = CartridgeTypeToSuppression(bulletCartType);
						//Param3<vector, vector, float> params = new Param3<vector, vector, float>(end_point, aim_point, ammoTypeToSuppression); // The value to be sent through RPC
						//GetRPCManager().SendRPC("Suppression", "RPC_ShootSCBFromPlayer", params);
					}
					//DayZPhysics.RayCastBullet(from, to, layerMask, e.m_player, hitObjectRCB, contact_pos, hit_normal, hit_fraction);
					// SphereCast Method
					DayZPhysics.SphereCastBullet(beg_Pos, aim_point, 0.6, layerMask, null, hitObjectSCB, hit_pos, null, null);
					//Particle particle1 = Particle.PlayInWorld(ParticleList.DEBUG_DOT, end_point); // Muzzle
					//Particle particle2 = Particle.PlayInWorld(ParticleList.DEBUG_DOT, beg_Pos); // Suppression Start Point
					if (hit_pos != vector.Zero)
					{
						Param2<vector, float> params; // The value to be sent through RPC

						// Data update.
						float ammoTypeToSuppression = CartridgeTypeToSuppression(bulletCartType);
						params = new Param2<vector, float>(hit_pos, ammoTypeToSuppression);
						GetRPCManager().SendRPC("Suppression", "CreateSuppressionTriggerEx", params);
						// if (hitObjectSCB)
						// {
						// 	Print("[ Suppression Mod ]: WeaponFire.c : Hit Object: " + hitObjectSCB.GetType());
						// }
						
						//Particle particle3 = Particle.PlayInWorld(ParticleList.DEBUG_DOT, hit_pos); // Point of hit.
						//Sending RPC (Client -> Server)
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