modded class PlayerBase extends ManBase
{
    private Object          m_LastFiredBullet;

    private float           m_SuppressionLevel = 0;
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

    private const float     SUPPRESSION_MIN = 0; // Generally speaking, keep this at 0.
    private const float     SUPPRESSION_TINNITUS_START_LEVEL = 175; // Value at which tinnitus starts to play. If it is enabled.

    float SUPPRESSION_SOUND_START_LEVEL = config.Suppression_Muffle_Start;
    float SUPPRESSION_MAX = config.Suppression_Max_Value;
    float SUPPRESSION_DECAY_RATE = config.Suppression_Normal_Decay;
    float SUPPRESSION_DECAY_RATE_FAST = config.Suppression_Fast_Decay;
    float SUPPRESSION_DECAY_RATE_EAR = config.Suppresion_Protection_Decay;


    private const int       SUPPRESSION_TUNNEL_FACTOR_PPE = 4; // I don't suggest changing this from 4.

    // PPE stuff, changing this to anything won't have an effect. It just holds data later down the line.
    private ref Param1<float> 			m_Param;

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
        //Initialize our TStringArray for RPC information.
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
        //Print("[Spaghetti Suppression] :: SuppressionLevel :: " + m_SuppressionLevel);
        // if (m_HasEarProtection)
        // {
        //     m_SuppressionLevel = 0;
        //     m_hearingLossLevel = 0;
        //     m_IsDeafened = false;
        //     m_IsSuppressed = false;
        // }
    }
    void ProcessClientSideSuppression(float dT)
    {
        // PPE EFFECTS // CLIENT SIDE
        m_Param = new Param1<float>(0);
        float maxTunnelVision = 130;
        m_Param.param1 = (Math.Min(maxTunnelVision, m_SuppressionLevel) / SUPPRESSION_TUNNEL_FACTOR_PPE) * dT;
        PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects).Start(m_Param);
        
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
    }

    void ApplySuppression(int val)
    {
        m_SuppressionLevel += val;
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        SetSynchDirty();
        ApplyHearingLoss(val);
    }

    void ApplyHearingLoss(int val)
    {
        m_hearingLossLevel += val;
        m_hearingLossLevel = Math.Clamp(m_hearingLossLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        SetSynchDirty();
    }

    // HEARING PROTECTION IMPLEMENTATION BOOLEAN //
    bool CheckHearingProtection(EntityAI item)
    {
        bool hearingProtection = false;
        for (int j = 0; j < earProtectionItems.Count(); j++)
        {
            //Print("[ Suppression Mod ] : PlayerBase.c : Checking: " + earProtectionItems[j]);
            hearingProtection = earProtectionItems[j] == item.GetType();
            if (hearingProtection)
            {
                m_EarProtection = item;
                
                return hearingProtection;
            }
        }
        return hearingProtection;
    }

    float GetSuppressionLevel()
    {
        return m_SuppressionLevel;
    }
    void SetSuppressionLevel(float val)
    {
        if (GetGame().IsClient())
        {
            m_SuppressionLevel = val;
            m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
            ApplyHearingLoss(val);
        }
        else
        {
            m_SuppressionLevel = val;
            m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
            ApplyHearingLoss(val);
            Param2<PlayerBase, float> params = new Param2<PlayerBase, float>(this, val);
            GetRPCManager().SendRPC("Suppression", "RPC_Client_SetSuppression", params);
        }
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

    /*
    //==================================================
                    NOT IMPLEMENTED.
    //==================================================
    */
    // Spaghetti Code suppression
    ObjectBullet GetLastFiredBullet()
    {
        return m_LastFiredBullet;
    }
    void SetLastFiredBullet(ObjectBullet bullet)
    {
        m_LastFiredBullet = bullet;
        SetSynchDirty();
    }
    string GetEarProtectionItemName()
    {
        return m_EarProtection.GetType();
    }
    override void EEItemAttached(EntityAI item, string slot_name)
	{
        super.EEItemAttached(item, slot_name);
        m_hasEarProtection = CheckHearingProtection(item);
    }
    override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
        if (CheckHearingProtection(item))
        {
            m_hasEarProtection = false;
        }
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