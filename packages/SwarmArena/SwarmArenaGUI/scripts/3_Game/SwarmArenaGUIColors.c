// SwarmArenaGUIColors - Common color definitions for arena UI

class SwarmArenaGUIColors
{
    // Team colors
    static const int TeamA_R = 100;
    static const int TeamA_G = 150;
    static const int TeamA_B = 255;
    
    static const int TeamB_R = 255;
    static const int TeamB_G = 100;
    static const int TeamB_B = 100;
    
    // Money colors
    static const int Money_R = 100;
    static const int Money_G = 255;
    static const int Money_B = 100;
    
    static const int MoneyLow_R = 255;
    static const int MoneyLow_G = 200;
    static const int MoneyLow_B = 100;
    
    // Status colors
    static const int Alive_R = 100;
    static const int Alive_G = 255;
    static const int Alive_B = 100;
    
    static const int Dead_R = 150;
    static const int Dead_G = 150;
    static const int Dead_B = 150;
    
    // Get team color as ARGB int
    static int GetTeamColor(bool isTeamA, int alpha = 255)
    {
        if (isTeamA)
        {
            return ARGB(alpha, TeamA_R, TeamA_G, TeamA_B);
        }
        return ARGB(alpha, TeamB_R, TeamB_G, TeamB_B);
    }
    
    // Get money color based on amount
    static int GetMoneyColor(int amount, int alpha = 255)
    {
        if (amount < 500)
        {
            return ARGB(alpha, MoneyLow_R, MoneyLow_G, MoneyLow_B);
        }
        return ARGB(alpha, Money_R, Money_G, Money_B);
    }
    
    // Get player status color
    static int GetStatusColor(bool isAlive, int alpha = 255)
    {
        if (isAlive)
        {
            return ARGB(alpha, Alive_R, Alive_G, Alive_B);
        }
        return ARGB(alpha, Dead_R, Dead_G, Dead_B);
    }
}
