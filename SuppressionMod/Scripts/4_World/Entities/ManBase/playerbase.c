modded class PlayerBase extends ManBase
{
    private Object          m_LastFiredBullet;

    private float           m_SuppressionLevel = 0;
    private float           m_PrevSuppressionLevel = 0;
    private bool            m_PlayerCanSuppress = true;
    private float           m_hearingLossLevel = 0;
    private bool            m_IsSuppressed;
    private bool            m_IsDeafened;
    private bool            m_hasEarProtection;
    private EffectSound     m_SoundEffect;
    // Constants
    // ENABLE / DISABLE TINNITUS
    //TODO: Config for changing
    private const bool      TINNITUS_DISABLED = true;
    // Headset / Earplugs :)
    // Define display names here :)
    ref TStringArray earProtectionItems;

    SuppressionConfigSettings config = SuppressionConfigSettings.GetInstance();

    private EntityAI m_EarProtection;
    //

    private const float     SUPPRESSION_MIN = 0; // Generally speaking, keep this at 0. Unless you want people to have a 'suppression tolerance'
    private const float     SUPPRESSION_TINNITUS_START_LEVEL = 175; // Value at which tinnitus starts to play. If it is enabled.
    // private const float     SUPPRESSION_SOUND_START_LEVEL = 140; // Value at which volume is muffled from being suppressed.
    // private const float     SUPPRESSION_MAX = 200; // Default: 100
    // private const float     SUPPRESSION_DECAY_RATE = 10; //  10| per second. Used when the user has IsSuppressed = true.
    // private const float     SUPPRESSION_DECAY_RATE_FAST = 15; // 15| per second. Fast is used when the player has less suppressionLevel.
    // private const float     SUPPRESSION_DECAY_RATE_EAR = 50; // 50| per second. Ear is used when the player has ear protection regardless of their suppression level.
    private const int       SUPPRESSION_TUNNEL_FACTOR_PPE = 4; // I don't suggest changing this from 4.

    float SUPPRESSION_SOUND_START_LEVEL = config.Suppression_Muffle_Start;
    float SUPPRESSION_MAX = config.Suppression_Max_Value;
    float SUPPRESSION_DECAY_RATE = config.Suppression_Normal_Decay;
    float SUPPRESSION_DECAY_RATE_FAST = config.Suppression_Fast_Decay;
    float SUPPRESSION_DECAY_RATE_EAR = config.Suppresion_Protection_Decay;

    // PPE stuff, changing this to anything won't have an effect. It just holds data later down the line.
    private ref Param1<float> 			m_Param;

    void PlayerBase()
    {
        RegisterNetSyncVariableFloat("m_SuppressionLevel");
        RegisterNetSyncVariableFloat("m_hearingLossLevel");
    }
    override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)	
	{
		// lower implement 
		super.CommandHandler(pDt,pCurrentCommandID,pCurrentCommandFinished);
        ProcessSuppression(pDt);
        if ( GetGame().IsClient() )
        {
            ProcessClientSideSuppression(pDt);
        }
    }

    override void Init()
    {
        super.Init();
        earProtectionItems = new TStringArray();
        if(GetGame().IsServer())
            return;
        
        if (!TINNITUS_DISABLED)
        {
            m_SoundEffect = SEffectManager.CreateSound("Tinnitus_Soundset", GetPosition());
        }
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if( damageType == DamageType.FIRE_ARM )
		{
            float dmg = damageResult.GetDamage(dmgZone, "Health");
			GetStaminaHandler().DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
		}
    }
    void ProcessSuppression(float dT)
    {

        // Headset / Earplug check
        if(m_hasEarProtection)
        {            
            m_SuppressionLevel -= dT * SUPPRESSION_DECAY_RATE_EAR;
            m_hearingLossLevel -= dT * SUPPRESSION_DECAY_RATE_EAR;
        }
        else if(m_IsSuppressed)
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

        m_IsSuppressed = MiscGameplayFunctions.IsValueInRange(m_SuppressionLevel, SUPPRESSION_SOUND_START_LEVEL, SUPPRESSION_MAX);
        m_IsDeafened = MiscGameplayFunctions.IsValueInRange(m_hearingLossLevel, SUPPRESSION_SOUND_START_LEVEL, SUPPRESSION_MAX);
    }
    void ProcessClientSideSuppression(float dT)
    {
        // PPE EFFECTS // CLIENT SIDE
        m_Param = new Param1<float>(0);
        float maxTunnelVision = 130;
        m_Param.param1 = (Math.Min(maxTunnelVision, m_SuppressionLevel) / SUPPRESSION_TUNNEL_FACTOR_PPE) * dT;
        PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects).Start(m_Param);
        
        if (m_PrevSuppressionLevel < m_SuppressionLevel)
        {
            SpawnShockEffect( 0.7 );
            GetCurrentCamera().SpawnCameraShake( 0.7 );
        }
        // Deafened? Apply attenuation.
        if (m_IsDeafened && !m_hasEarProtection)
        {
            SetMasterAttenuation("SuppressionAttenuation");
        } 
        // Cancel attenuation
        else if (!m_IsDeafened || m_hasEarProtection)
        {
            SetMasterAttenuation("");
        }

        m_PrevSuppressionLevel = m_SuppressionLevel;
    }

    void ApplySuppression(int val)
    {
        m_SuppressionLevel += val;
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        SetSynchDirty();
        //Print("[Spaghetti Suppression] :: Suppression Level: " + m_SuppressionLevel);
        ApplyHearingLoss(val);
    }

    void ApplyHearingLoss(int val)
    {
        m_hearingLossLevel += val;
        m_hearingLossLevel = Math.Clamp(m_hearingLossLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        SetSynchDirty();
    }

    

    float GetSuppressionLevel()
    {
        return m_SuppressionLevel;
    }
    void SetSuppressionLevel(float val)
    {
        
        m_SuppressionLevel = val;
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        ApplyHearingLoss(val);

        //Print("[Spaghetti Suppression] :: Suppression Level: " + m_SuppressionLevel);
        SetSynchDirty();
    }

    float GetHearingLossLevel()
    {
        return m_hearingLossLevel;
    }
    bool IsDeafened()
    {
        return m_IsDeafened;
    }
    bool IsEarProtected()
    {
        return m_hasEarProtection;
    }

    bool IsSuppressed()
    {
        return m_IsSuppressed;
    }
    
    bool CanPlayerSuppress()
    {
        return m_PlayerCanSuppress;
    }
    void SetPlayerCanSuppress(bool suppress)
    {
        m_PlayerCanSuppress = suppress;
        //#if SPAGHETTIDEBUG
        //Print("[ Spaghetti Suppression ] :: PlayerBase :: CanPlayerSuppress? " + m_PlayerCanSuppress);
        //#endif
    }
    //earProtectionItems
    string GetEarProtectionItemName()
    {
        if (m_EarProtection)
        {
            return m_EarProtection.GetType();
        }
        return "_NO_EAR_PROTECTION_";
    }
    TStringArray GetEarprotectionArray()
    {
        return earProtectionItems;
    }
    override void EEItemAttached(EntityAI item, string slot_name)
	{
        super.EEItemAttached(item, slot_name);

        if (!IsEarProtected())
        {
            m_hasEarProtection = CheckHearingProtection(item);
        }
    }
    override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
        m_hasEarProtection = CheckAllItemsHearingProtection();
	}
    // HEARING PROTECTION IMPLEMENTATION BOOLEAN //
    bool CheckHearingProtection(EntityAI item)
    {
        bool hearingProtection = false;
        for (int j = 0; j < earProtectionItems.Count(); j++)
        {
            Print("[ Suppression Mod ] : PlayerBase.c : Checking: " + earProtectionItems[j] + " with " + item.GetType());
            hearingProtection = earProtectionItems[j] == item.GetType();
            if (hearingProtection)
            {
                m_EarProtection = item;
                
                return hearingProtection;
            }
        }
        return hearingProtection;
    }
    bool CheckAllItemsHearingProtection()
    {
        bool hearingProtection = false;
        ItemBase item;
        foreach(string inventorySlotStr : INVENTORY_SLOTS_ARRAY)
        {
            ItemBase.CastTo(item, FindAttachmentBySlotName(inventorySlotStr));
            if (item)
            {
                hearingProtection = CheckHearingProtection(item);
                if (hearingProtection)
                {
                    return hearingProtection;
                }
            }
        }
        if (!hearingProtection)
        {
            m_EarProtection = null;
        }
        return hearingProtection;
    }
}

// ⠄⠄⠄⠄⠄⠰⣿⣿⣿⣿⣶⠶⠤⠄⠄⣶⣶⣤⣀⣀⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄
// ⠄⠄⠄⠄⠄⠄⠈⠻⣿⣿⣰⣿⣿⣿⣶⣬⠝⠻⢯⣥⣶⣶⠄⣀⡀⠄⠄⠄⠄⠄
// ⠄⠄⠄⠄⠄⠄⠄⠄⠈⢿⣿⣿⣿⡟⣫⠶⣚⣓⣒⠌⢛⡵⢋⣵⠶⢦⡀⠄⠄⠄
// ⠄⠄⠄⠄⢠⢒⣉⣑⠢⣾⣿⣿⣿⡟⢩⣾⣿⡿⠛⢿⣮⠱⣿⣿⠠⡄⠹⡄⠠⠤
// ⠄⠄⠄⠄⢻⣯⣶⣿⣿⣦⣙⠿⠿⠿⠆⠋⠉⠄⠄⠄⢠⡄⠄⠉⠄⠁⠄⠄⠄⠄
// ⠄⢀⣤⣾⣿⣿⣏⣭⣭⣿⣿⡆⠄⠄⠄⠄⠄⠄⠄⠄⠼⣅⠄⠄⠄⠄⠄⠄⠄⣠
// ⠺⣿⣿⣿⢟⣵⡿⢿⣷⣮⡻⢿⣷⣦⣤⣤⣤⣤⣤⣵⣿⣿⣿⣮⣻⡿⠶⣚⠔⠄
// ⠄⠈⠻⣿⢸⣿⡕⣷⣬⡻⢿⣷⣬⣛⠿⠿⠿⠿⠿⠿⠿⠿⣛⣫⣥⠶⠛⠁⠄⠄
// ⠄⠄⠄⠈⠁⠻⢿⣮⡉⠉⠓⠶⣮⣭⣛⣛⣛⣛⣛⠛⠛⠉⠉⠄⣰⡿⠁⠄⠄⠄
// ⠄⠄⠄⠄⠄⠄⠄⠈⠉⠓⠒⠠⣄⣉⣙⣛⣛⣋⣁⣠⠤⠴⠖⢟⡁⠄⠄⠄⠄⠄
// ⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⣨⣭⣭⠵⠶⠒⠉⠄⠄⠄⠈⠑⠢⡄⠄⠄
// ⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⢰⣷⣶⠶⣫⣼⣦⡄⠄⠄⠄⠄⡴⠋⠄⠄⠄
// ⠄⠄⠄⠄⠄⠄⠄YOU FOUND ME HACKER MAN⠄⠄⠄⠄⠄⠄