////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.10
//https://mikero.bytex.digital/Downloads
//'now' is Fri Jun 23 00:00:10 2023 : 'file' last modified on Mon Apr 10 14:21:50 2023
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class SpaghettiSuppression
	{
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Characters"};
		units[] = {};
		weapons[] = {};
	};
};
class CfgMods
{
	class SpaghettiSuppression
	{
		dir = "Suppression";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Suppression module";
		author = "Jupiter, Spaghetto, NullWorld";
		authorID = "";
		version = "62023469420.696969";
		extra = 0;
		dependencies[] = {"Game","World","Mission"};
		type = "mod";
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"SpaghettiSuppression/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"SpaghettiSuppression/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"SpaghettiSuppression/Scripts/5_Mission"};
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
