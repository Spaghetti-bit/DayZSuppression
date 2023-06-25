enum EStaminaMultiplierTypesSuppression
{
    SUPPRESSION = 42
}

modded class StaminaHandler
{
    // Unit is in %
    const float STAMINA_DRAIN_SUPPRESSION_PISTOL = 5;
    const float STAMINA_DRAIN_SUPPRESSION_INTERMEDIATE = 8;
    const float STAMINA_DRAIN_SUPPRESSION_FULLPOWER = 15;
    const float STAMINA_DRAIN_SUPPRESSION_SHELL = 20;

    override void Init()
    {
        super.Init();
        //----------------- depletion --------------------
        m_RegisteredDepletionModifiers.Insert(EStaminaMultiplierTypesSuppression.SUPPRESSION, SuppressionMdfr.STAMINA_DEPLETION_MULTIPLIER);
        
        //----------------- recovery --------------------
        m_RegisteredRecoveryModifiers.Insert(EStaminaMultiplierTypesSuppression.SUPPRESSION, SuppressionMdfr.STAMINA_RECOVERY_MULTIPLIER);

    }
    override void RegisterStaminaConsumers()
	{
        super.RegisterStaminaConsumers();
        m_StaminaConsumers.RegisterConsumer(ESuppressionStaminaConsumers.SUPPPRESSION_PISTOL, STAMINA_DRAIN_SUPPRESSION_PISTOL);
        m_StaminaConsumers.RegisterConsumer(ESuppressionStaminaConsumers.SUPPPRESSION_INTERMEDIATE, STAMINA_DRAIN_SUPPRESSION_INTERMEDIATE);
        m_StaminaConsumers.RegisterConsumer(ESuppressionStaminaConsumers.SUPPPRESSION_FULLPOWER, STAMINA_DRAIN_SUPPRESSION_FULLPOWER);
        m_StaminaConsumers.RegisterConsumer(ESuppressionStaminaConsumers.SUPPPRESSION_SHELL, STAMINA_DRAIN_SUPPRESSION_SHELL);
    }
    override void RegisterStaminaModifiers()
	{
        super.RegisterStaminaModifiers();
        m_StaminaModifiers.RegisterFixed(ESuppressionStaminaModifiers.SUPPPRESSION_PISTOL, STAMINA_DRAIN_SUPPRESSION_PISTOL);
        m_StaminaModifiers.RegisterFixed(ESuppressionStaminaModifiers.SUPPPRESSION_INTERMEDIATE, STAMINA_DRAIN_SUPPRESSION_INTERMEDIATE);
        m_StaminaModifiers.RegisterFixed(ESuppressionStaminaModifiers.SUPPPRESSION_FULLPOWER, STAMINA_DRAIN_SUPPRESSION_FULLPOWER);
        m_StaminaModifiers.RegisterFixed(ESuppressionStaminaModifiers.SUPPPRESSION_SHELL, STAMINA_DRAIN_SUPPRESSION_SHELL);
    }
}