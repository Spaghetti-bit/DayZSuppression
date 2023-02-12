#define _ARMA_

class CfgPatches
{
    class Suppression
    {
        requiredAddons[] = {"DZ_Data","DZ_Scripts", "DZ_Characters"};
    };
};

class CfgMods
{
    class Suppression
    {    
        
        dir = "Suppression";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Suppression module";
        author = "Jupiter, Spaghetto";
        authorID = "";
        version = "12.0";
        extra = 0;
        dependencies[] = {"GameLib","Game","World","Mission"};                
        type = "mod";
        
        class defs
        {
            class gameLibScriptModule
            {
                value="";
                files[]={"Suppression/Scripts/2_GameLib"};
            };            
            class gameScriptModule
            {
                value = "";
                files[] = {"Suppression/Scripts/3_Game"};
            };

            class worldScriptModule
            {
                value = "";
                files[] = {"Suppression/Scripts/4_World"};
            };

            class missionScriptModule
            {
                value = "";
                files[] = {"Suppression/Scripts/5_Mission"};
            };
        };
    };
};
class CfgSoundEffects
{
	class AttenuationsEffects
	{
		class SuppressionAttenuation
		{
			class Equalizer0
			{
				center[] = {196,2000,8367,10000};
				bandwidth[] = {2,2,0.1,2};
				gain[] = {1.4125376,0.12589253,1.9952624,0.12589253};
			};
			class Equalizer1
			{
				center[] = {60,1000,2000,10000};
				bandwidth[] = {2,1,0.1,2};
				gain[] = {1,0.3548134,0.25118864,0.12589253};
			};
			class Echo
			{
				WetDryMix = 0.25;
				Feedback = 0.2;
				Delay = 50;
			};
		};
	};
};