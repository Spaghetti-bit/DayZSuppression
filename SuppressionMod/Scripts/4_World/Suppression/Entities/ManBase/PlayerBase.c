modded class PlayerBase extends ManBase
{
    private float m_SuppressionLevel = 0;
    private float m_HearingLossLevel = 0;
	private float m_TinnitusLevel = 0;
    private bool m_IsSuppressed;
    private bool m_IsDeafened;
	private bool m_hasEarProtection;
	
    private EffectSound m_SoundEffect;

    SuppressionConfigSettings config = SuppressionConfigSettings.GetInstance();

    //Ear Protection Array
    ref TStringArray earProtectionItems;

    private EntityAI m_EarProtection;

    private const float SUPPRESSION_MIN = 0; // Generally speaking, keep this at 0.

    //bool TINNITUS_DISABLED = config.Suppression_Tinnitus_Disabled;
    //float SUPPRESSION_TINNITUS_START_LEVEL = config.Suppression_Tinnitus_Start;
    float SUPPRESSION_SOUND_START_LEVEL = config.Suppression_Muffle_Start;
    float SUPPRESSION_MAX = config.Suppression_Max_Value;
    float SUPPRESSION_DECAY_RATE = config.Suppression_Normal_Decay;
    float SUPPRESSION_DECAY_RATE_FAST = config.Suppression_Fast_Decay;
    float SUPPRESSION_DECAY_RATE_EAR = config.Suppresion_Protection_Decay;

    //Dont touch me
    private const int SUPPRESSION_TUNNEL_FACTOR_PPE = 4;

    private ref Param1<float> m_Param;

    override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);


        if (!GetGame().IsServer())
        {
            ProcessSuppression(pDt);
        }

        UpdateNetSyncVariableFloat("m_SuppressionLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
        UpdateNetSyncVariableFloat("m_HearingLossLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
    }

    override void Init()
    {
        super.Init();

        earProtectionItems = new TStringArray();

        RegisterNetSyncVariableFloat("m_SuppressionLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
        RegisterNetSyncVariableFloat("m_HearingLossLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);

        if (GetGame().IsServer())
            return;
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if (damageType == DamageType.FIRE_ARM)
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
            m_HearingLossLevel -= dT * SUPPRESSION_DECAY_RATE_EAR;
        }
        else if(m_IsSuppressed)
        {   
            m_SuppressionLevel -= dT * SUPPRESSION_DECAY_RATE;
            m_HearingLossLevel -= dT * SUPPRESSION_DECAY_RATE;
        }
        else
        {
            m_SuppressionLevel -= dT * SUPPRESSION_DECAY_RATE_FAST;
            m_HearingLossLevel -= dT * SUPPRESSION_DECAY_RATE_FAST;
        }
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);
        m_HearingLossLevel = Math.Clamp(m_HearingLossLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);

        m_IsSuppressed = MiscGameplayFunctions.IsValueInRange(m_SuppressionLevel, SUPPRESSION_SOUND_START_LEVEL, SUPPRESSION_MAX);
        m_IsDeafened = MiscGameplayFunctions.IsValueInRange(m_HearingLossLevel, SUPPRESSION_SOUND_START_LEVEL, SUPPRESSION_MAX);

        // PPE EFFECTS //
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
        ApplyHearingLoss(val);
        m_SuppressionLevel = Math.Clamp(m_SuppressionLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);

        UpdateNetSyncVariableFloat("m_SuppressionLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
    }

    void ApplyHearingLoss(int val)
    {
        m_HearingLossLevel += val;
        m_HearingLossLevel = Math.Clamp(m_HearingLossLevel, SUPPRESSION_MIN, SUPPRESSION_MAX);

        UpdateNetSyncVariableFloat("m_HearingLossLevel", SUPPRESSION_MIN, SUPPRESSION_MAX, 1);
    }

    bool CheckHearingProtection(EntityAI item)
    {
        if (!GetGame().GetPlayer())
        {
            return false;
        }
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

    float GetHearingLossLevel()
    {
        return m_HearingLossLevel;
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
        m_hasEarProtection = CheckHearingProtection(item);
	}
};

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
