class ObjectBullet extends Object
{
    private bool m_IsBullet = false;
    private bool m_HasBeenFound = false;

    void SetHasBeenFound(bool found)
    {
        m_HasBeenFound = found;
    }
    void SetIsBullet(bool isBullet)
    {
        m_IsBullet = isBullet;
    }
    bool IsBullet() { return m_IsBullet; }
    bool HasBeenFound() { return m_HasBeenFound; }

    override void Delete()
    {
        if (IsBullet())
        {
            Object.Cast(GetChildren()).SetPosition(GetPosition());
        }
        super.Delete();
    }
}