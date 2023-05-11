enum eModifiersSuppression
{
    MDF_SUPPRESSION = 42
}
enum eModifierSyncIDsSuppression
{
    MODIFIER_SYNC_SUPPRESSION = 42
}

class SuppressionMdfr: ModifierBase
{
    const float STAMINA_RECOVERY_MULTIPLIER = 0;
	const float STAMINA_DEPLETION_MULTIPLIER = 1.4;
    const int 	TICK_FREQUENCY = 1;
    override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiersSuppression.MDF_SUPPRESSION;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= TICK_FREQUENCY;
		m_SyncID				= eModifierSyncIDsSuppression.MODIFIER_SYNC_SUPPRESSION;
	}
    override void OnTick(PlayerBase player, float deltaT)
	{
        // mfing nuffin, no need for this shizzle my nizzle.}
    }
    bool CheckSuppression(PlayerBase player)
    {
        return player.GetSuppressionLevel() >= 115;
    }
    override bool ActivateCondition(PlayerBase player)
    {
        return CheckSuppression(player);
    }
    override bool DeactivateCondition(PlayerBase player)
    {
        return !CheckSuppression(player);
    }
    override void OnActivate(PlayerBase player)
    {
        //Print("[Spaghetti Suppression] :: SuppressionMdfr.c :: " + player + "Suppression :: Activated.");
        player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypesSuppression.SUPPRESSION);
        player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypesSuppression.SUPPRESSION);
    }
    override void OnDeactivate(PlayerBase player)
    {
        //Print("[Spaghetti Suppression] :: SuppressionMdfr.c :: " + player + "Suppression :: Deactivated.");
        player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypesSuppression.SUPPRESSION);
        player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypesSuppression.SUPPRESSION);
    }
    override void OnReconnect(PlayerBase player)
	{

	}
}