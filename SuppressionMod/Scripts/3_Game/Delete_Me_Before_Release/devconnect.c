// Script File
modded class DayZGame
{
    void DevConnect(string address, int port)
    {
        m_ConnectAddress = address;
        m_ConnectPort = port;

        Connect();
    }
}