modded class PlayerBase extends ManBase
{
    // DEBUG, Set to 100.
    // otherwise, set to 0.
    private float           m_SuppressionLevel = 0;
    private float           m_hearingLossLevel = 0;
    private bool            m_IsSuppressed;
    private bool            m_IsDeafened;
    private EffectSound     m_SoundEffect;
    // Constants
    // ENABLE / DISABLE TINNITUS
    private const bool      TINNITUS_DISABLED = true;
    // Headset / Earplugs :)
    // Define display names here :)
    ref TStringArray earProtectionItems;
        //private ref const array<string>      HEADSET_EARPLUG_ITEM_DISPLAY_NAME_ARRAY = SupFileManager.GetEarprotectionArray()

    // Add new items by putting a comma after the last entry and surrounding it in "'s. Example: "gssh_headset"
    private ref const array<string>     INVENTORY_SLOTS_TO_CHECK = { "Head", "Headgear", "Eyewear", "Shoulder", "Back"};
    private EntityAI m_EarProtection;
    //

    private const float     SUPPRESSION_MIN = 0; // Generally speaking, keep this at 0.
    private const float     SUPPRESSION_TINNITUS_START_LEVEL = 175; // Value at which tinnitus starts to play. If it is enabled.
    private const float     SUPPRESSION_SOUND_START_LEVEL = 140; // Value at which volume is muffled from being suppressed.
    private const float     SUPPRESSION_MAX = 200; // Default: 100
    private const float     SUPPRESSION_DECAY_RATE = 10; //  10| per second. Used when the user has IsSuppressed = true.
    private const float     SUPPRESSION_DECAY_RATE_FAST = 15; // 15| per second. Fast is used when the player has less suppressionLevel.
    private const int       SUPPRESSION_TUNNEL_FACTOR_PPE = 4; // I don't suggest changing this from 4.

    // PPE stuff, changing this to anything won't have an effect. It just holds data later down the line.
    private ref Param1<float> 			m_Param;

    override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)	
	{
		// lower implement 
		super.CommandHandler(pDt,pCurrentCommandID,pCurrentCommandFinished);
        if (!GetGame().IsServer())
        {
            // Suppression decrements should be handled purely on the client and updates the server afterwards.
            //Client only beyond this point. :)
            ProcessSuppression(pDt);
        }
        // Desync issues testing.
        UpdateNetSyncVariableFloat("m_SuppressionLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
        UpdateNetSyncVariableFloat("m_hearingLossLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
    }

    override void Init()
    {
        super.Init();
        //Initialize our TStringArray for RPC information.
        earProtectionItems = new TStringArray();

        RegisterNetSyncVariableFloat("m_SuppressionLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
        RegisterNetSyncVariableFloat("m_hearingLossLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
        if(GetGame().IsServer())
            return;
        
        if (!TINNITUS_DISABLED)
        {
            m_SoundEffect = SEffectManager.CreateSound("Tinnitus_Soundset", GetPosition());
            //m_SoundEffect = SEffectManager.CreateSound("CSGOTerroristsCallouts_Soundset", GetPosition());
            // Joke ^
        }
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if( damageType == DamageType.FIRE_ARM /*&& ammo == "FlashGrenade_Ammo"*/ )
		{
            float dmg = damageResult.GetDamage(dmgZone, "Health");
            // Gun -> Magazine -> AmmoData -> Damage value (Implemented in suppressiontrigger.c)
			GetStaminaHandler().DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
			//GetStaminaHandler().DepleteStamina(damageResult.GetDamage(dmgZone, "Health") / 10);
		}
    }
    void ProcessSuppression(float dT)
    {
        if(m_IsSuppressed)
        {
            m_SuppressionLevel -= dT * SUPPRESSION_DECAY_RATE;
            m_hearingLossLevel -= dT * SUPPRESSION_DECAY_RATE;
        }
        else
        {
            m_SuppressionLevel -= dT * SUPPRESSION_DECAY_RATE_FAST;
            m_hearingLossLevel -= dT * SUPPRESSION_DECAY_RATE_FAST;
        }
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        m_hearingLossLevel = Math.Clamp(m_hearingLossLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        //Print("[ Suppression Mod ] HEARINGLOSS_LEVEL : " + m_hearingLossLevel);
        //Print("[ Suppression Mod ] SUPPRESSION_LEVEL : " + m_SuppressionLevel);

        
        //Print("[ Suppression Mod ]: DEBUG: AFTER DECAY & CLAMP || m_SuppressionLevel: " + m_SuppressionLevel);
        
        // Headset / Earplug check
        bool m_HasEarProtection = CheckHearingProtection();
        //Print("[ Suppression Mod ] Hearing Protection? " + m_HasEarProtection);
        m_IsSuppressed = MiscGameplayFunctions.IsValueInRange(m_SuppressionLevel, SUPPRESSION_SOUND_START_LEVEL, SUPPRESSION_MAX);
        m_IsDeafened = MiscGameplayFunctions.IsValueInRange(m_hearingLossLevel, SUPPRESSION_SOUND_START_LEVEL, SUPPRESSION_MAX);

        if (m_HasEarProtection)
        {
            m_SuppressionLevel = 0;
            m_hearingLossLevel = 0;
            m_IsDeafened = false;
            m_IsSuppressed = false;
        }

        // PPE EFFECTS //
        m_Param = new Param1<float>(0);
        float maxTunnelVision = 130;
        m_Param.param1 = (Math.Min(maxTunnelVision, m_SuppressionLevel) / SUPPRESSION_TUNNEL_FACTOR_PPE) * dT;
        PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects).Start(m_Param);
        
        // Deafened? Apply attenuation.
        if (m_IsDeafened && !m_HasEarProtection)
        {
            SetMasterAttenuation("SuppressionAttenuation");
        } 
        // Cancel attenuation
        else if (!m_IsDeafened || m_HasEarProtection)
        {
            SetMasterAttenuation("");
        }
    }

    void ApplySuppression(int val)
    {
        //Print("[ Suppression Mod ]: DEBUG: " + val);

        m_SuppressionLevel += val;
        ApplyHearingLoss(val);
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        //Print("[ Suppression Mod ]: PlayerBase.c || m_SuppressionLevel: " + m_SuppressionLevel);

        // Desync issues testing.
        UpdateNetSyncVariableFloat("m_SuppressionLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
    }

    void ApplyHearingLoss(int val)
    {
        m_hearingLossLevel += val;
        m_hearingLossLevel = Math.Clamp(m_hearingLossLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);

        // Desync issues testing.
        UpdateNetSyncVariableFloat("m_hearingLossLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
    }

    // HEARING PROTECTION IMPLEMENTATION BOOLEAN //
    bool CheckHearingProtection()
    {
        if (!GetGame().GetPlayer())
        {
            return false;
        }
        bool hearingProtection = false;
        int inventoryCount = DefaultCharacterCreationMethods.GetAttachmentSlotsArray().Count();
        //Print("[ Suppression Mod ] : PlayerBase.c : SlotsArrayCount: " + inventoryCount);
        for (int i = 0; i < inventoryCount; i++)
        {
            // :eyes: 
            // found the ear pro
            // Having ear pro cancels out muffle :)
            //TODO: Print("[SUPPRESSION MOD] : " + HEADSET_EARPLUG_ITEM_DISPLAY_NAME_ARRAY[i]);
            /* 
            Get the client
            Get the client's player
            Get their inventory
            Use the static DefaultCharacterCreationMethods array of defined slots
            Get the index we're on.
            Return the item pertaining to that index.
            */
            EntityAI item = GetGame().GetPlayer().GetInventory().FindAttachment(DefaultCharacterCreationMethods.GetAttachmentSlotsArray().Get(i));
            if (!item)
            {
                // If item doesn't exist on slot, skip to next slot.
                continue;
            }
            // Grabbing name of item for easier checking. Possible through IDs but less understandable in code.
            string nameOfItemOnSlot = item.GetType();

            //Print("[ Suppression Mod ] : PlayerBase.c : Item: " + nameOfItemOnSlot);
            
            // Iterate through our defined list of ear protection items.
            for (int j = 0; j < earProtectionItems.Count(); j++)
            {
                //Print("[ Suppression Mod ] : PlayerBase.c : Checking: " + earProtectionItems[j]);
                hearingProtection = earProtectionItems[j] == nameOfItemOnSlot;
                if (hearingProtection)
                {
                    m_EarProtection = item;
                    // Player has ear protection, no need to continue iterating.
                    // Return boolean.
                    // Adding a lot of items to HEADSET_EARPLUG list compounds the runtime. Minimal but still it adds computation time.
                    return hearingProtection;
                }
            }
        }
        return hearingProtection;
    }

    float GetSuppressionLevel()
    {
        return m_SuppressionLevel;
    }

    float GetHearingLossLevel()
    {
        return m_hearingLossLevel;
    }

    string GetEarProtectionItemName()
    {
        return m_EarProtection.GetType();
    }

}