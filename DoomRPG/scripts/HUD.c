#include "Defs.h"

#include "HUD.h"
#include "ItemData.h"
#include "Map.h"
#include "Mission.h"
#include "RPG.h"
#include "Shield.h"
#include "Skills.h"
#include "Stats.h"
#include "Stims.h"
#include "Utils.h"

str StatusEffects[SE_MAX] =
{
    "\CcBlind",
    "\CfConfusion",
    "\CqPoison",
    "\CdCorrosion",
    "\CiFatigue",
    "\CtVirus",
    "\CnSilence",
    "\CgCurse",
    "\CvEMP",
    "\CdRadiation"
};

str StatusNumerals[6] =
{
    "",
    "I",
    "II",
    "III",
    "IV",
    "V"
};

NamedScript Type_ENTER void StatusEffectHUD()
{
    int const StatusColors[SE_MAX] =
    {
        CR_GRAY,
        CR_GOLD,
        CR_DARKGREEN,
        CR_GREEN,
        CR_ORANGE,
        CR_PURPLE,
        CR_LIGHTBLUE,
        CR_RED,
        CR_CYAN,
        CR_GREEN
    };
    
    Start: NOP; // [KS] C doesn't allow declarations after labels, so we need this.
    
    fixed X = GetActivatorCVar("drpg_stateffect_x");
    fixed Y = GetActivatorCVar("drpg_stateffect_y");
    
    for (int i = 0; i < SE_MAX; i++)
    {
        str Icon;
        str Fill;
        int TimerPercent;
        
        switch (i)
        {
        case SE_BLIND:      Icon = "SE_Blnd";   Fill = "BarSBlnd";    break;
        case SE_CONFUSION:  Icon = "SE_Conf";   Fill = "BarSConf";    break;
        case SE_POISON:     Icon = "SE_Pois";   Fill = "BarSPois";    break;
        case SE_CORROSION:  Icon = "SE_Corr";   Fill = "BarSCorr";    break;
        case SE_FATIGUE:    Icon = "SE_Fati";   Fill = "BarSFati";    break;
        case SE_VIRUS:      Icon = "SE_Viru";   Fill = "BarSViru";    break;
        case SE_SILENCE:    Icon = "SE_Sile";   Fill = "BarSSile";    break;
        case SE_CURSE:      Icon = "SE_Curs";   Fill = "BarSCurs";    break;
        case SE_EMP:        Icon = "SE_EMP";    Fill = "BARSEmp";     break;
        case SE_RADIATION:  Icon = "SE_Radi";   Fill = "BARSRadi";    break;
        }
        
        if (Player.StatusType[i] || GetActivatorCVar("drpg_hud_preview"))
        {
            TimerPercent = (int)(((fixed)Player.StatusTimer[i] / (fixed)Player.StatusTimerMax[i]) * 100.0);
            if (TimerPercent > 100)
                TimerPercent = 100;
            
            // Preview
            if (GetActivatorCVar("drpg_hud_preview"))
                TimerPercent = 100;
            
            SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
            
            // Name / Intensity
            SetFont("SMALLFONT");
            HudMessage("%s %s\n", StatusEffects[i], StatusNumerals[Player.StatusIntensity[i]], HUDMSG_PLAIN | HUDMSG_ALPHA, 0, CR_WHITE, X + 50.0, Y, 0.05, 0.75);
            
            // Time
            HudMessage("%s\n", FormatTime(Player.StatusTimer[i]), HUDMSG_PLAIN | HUDMSG_ALPHA, 0, StatusColors[i], X + 104.1, Y, 0.05, 0.75);
            
            // Bar
            SetHudClipRect((int)X, (int)(Y - 6), TimerPercent, (int)(Y + 6));
            PrintSpriteAlpha(Fill, 0, X + 0.1, Y, 0.05, 0.75);
            SetHudClipRect(0, 0, 0, 0);
            
            // Increment Y
            Y += 16.0;
        }
    }
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void CreditHUD()
{
    str const CreditSprites[6] =
    {
        "CREDA0",
        "CREDB0",
        "CREDC0",
        "CREDD0",
        "CREDE0",
        "CREDF0"
    };
    
    // Interpolators
    InterpData Credits;
    Credits.Value = CheckInventory("DRPGCredits");
    Credits.OldValue = Credits.Value;
    Credits.StartValue = Credits.Value;
    Credits.DisplayValue = Credits.Value;
    Credits.TimerMaxCap = 2;
    InterpData Modules;
    Modules.Value = CheckInventory("DRPGModule");
    Modules.OldValue = Modules.Value;
    Modules.StartValue = Modules.Value;
    Modules.DisplayValue = Modules.Value;
    Modules.TimerMaxCap = 2;
    
    // Collection
    int CreditsCollected = 0;
    int CreditsCollectionTimer = 0;
    int ModulesCollected = 0;
    int ModulesCollectionTimer = 0;
    
    // Misc
    int CreditColor;
    
    Start:
    
    // If we're on the title map, terminate
    if (InTitle) return;
    
    Credits.Value = CheckInventory("DRPGCredits");
    Modules.Value = CheckInventory("DRPGModule");
    CreditColor = (Timer() / (35 * 60)) % 6;
    fixed X = GetActivatorCVar("drpg_credits_x");
    fixed Y = GetActivatorCVar("drpg_credits_y");
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    // Interpolation
    Credits = Interpolate(Credits);
    Modules = Interpolate(Modules);
    
    // Update the collection values
    if (Credits.Value != Credits.OldValue)
    {
        CreditsCollected += (Credits.Value - Credits.OldValue);
        CreditsCollectionTimer = 35 * 6;
    }
    if (Modules.Value != Modules.OldValue)
    {
        ModulesCollected += (Modules.Value - Modules.OldValue);
        ModulesCollectionTimer = 35 * 6;
    }
    
    PrintSprite(CreditSprites[CreditColor], 0, X, Y + 12.0, 0.05);
    SetFont("BIGFONT");
    HudMessage("%d\n", Credits.DisplayValue, HUDMSG_PLAIN, 0, CR_GOLD, X + 16.1, Y, 0.05);
    if (CreditsCollected != 0)
        HudMessage("%c%d\n", (CreditsCollected > 0 ? '+' : '@'), CreditsCollected, HUDMSG_FADEOUT, PAY_ID, (CreditsCollected > 0 ? CR_WHITE : CR_RED), X + 16.1, Y + 12.0, 0.05, 2.0, 1.0);
    if (GetActivatorCVar("drpg_module_show") || GetActivatorCVar("drpg_hud_preview"))
    {
        PrintSprite("UMODA0", 0, X - 4.0, Y + 56.0, 0.05);
        SetFont("BIGFONT");
        HudMessage("%d\n", Modules.DisplayValue, HUDMSG_PLAIN, 0, CR_GREEN, X + 16.1, Y + 24.0, 0.05);
        if (ModulesCollected != 0)
            HudMessage("%c%d\n", (ModulesCollected > 0 ? '+' : '@'), ModulesCollected, HUDMSG_FADEOUT, PAY_ID + 1, (ModulesCollected > 0 ? CR_DARKGREEN : CR_DARKRED), X + 16.1, Y + 36.0, 0.05, 2.0, 1.0);
    }
    
    // Collection timer handling
    if (CreditsCollectionTimer > 0)
        CreditsCollectionTimer--;
    else
        CreditsCollected = 0;
    if (ModulesCollectionTimer > 0)
        ModulesCollectionTimer--;
    else
        ModulesCollected = 0;
    
    Credits.OldValue = CheckInventory("DRPGCredits");
    Modules.OldValue = CheckInventory("DRPGModule");
    Delay(1);
    
    goto Start;
}

NamedScript Type_ENTER void ComboHUD()
{
    int TimerPercent;
    
    // Interpolators
    InterpData Combo;
    Combo.TimerMaxCap = 1;
    InterpData XP;
    XP.TimerMaxCap = 1;
    InterpData Rank;
    Rank.TimerMaxCap = 1;
    InterpData Bonus;
    Bonus.TimerMaxCap = 1;
    
    Start:
    
    // If we're on the title map, terminate
    if (InTitle) return;
    
    fixed X = GetActivatorCVar("drpg_combo_x");
    fixed Y = GetActivatorCVar("drpg_combo_y");
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    Combo.Value = Player.Combo;
    XP.Value = Player.XPGained;
    Rank.Value = Player.RankGained;
    Bonus.Value = Player.BonusGained;
    
    // Interpolation
    Combo = Interpolate(Combo);
    XP = Interpolate(XP);
    Rank = Interpolate(Rank);
    Bonus = Interpolate(Bonus);
    
    if (Player.Combo > 0 && Player.ComboTimer <= COMBO_MAX)
        TimerPercent = (int)(((fixed)Player.ComboTimer / (fixed)COMBO_MAX) * 100.0);
    else if (GetActivatorCVar("drpg_hud_preview"))
        TimerPercent = 100;
    else
        TimerPercent = 0;
    if (TimerPercent > 100)
        TimerPercent = 100;
    
    // Bar
    if (TimerPercent > 0)
    {
        DrawBar("CBar1", X, Y, (TimerPercent > 50 ? 50 : TimerPercent), true);
        DrawBar("CBar2", X + 50, Y, TimerPercent - 50, true);
    }
    
    // Combo Info
    SetFont("BIGFONT");
    if (Combo.DisplayValue > 0 || GetActivatorCVar("drpg_hud_preview"))
        HudMessage("%d\n", Combo.DisplayValue, HUDMSG_PLAIN, 0, CR_PURPLE, X + 0.1, Y + 10.0, 0.05);
    if (XP.DisplayValue != 0 || GetActivatorCVar("drpg_hud_preview"))
        HudMessage("%ld\n", XP.DisplayValue, HUDMSG_PLAIN, 0, (XP.DisplayValue >= 0 ? CR_WHITE : CR_GREY), X + 0.1, Y + 22.0, 0.05);
    if (Rank.DisplayValue != 0 || GetActivatorCVar("drpg_hud_preview"))
        HudMessage("%ld\n", Rank.DisplayValue, HUDMSG_PLAIN, 0, (Rank.DisplayValue >= 0 ? CR_YELLOW : CR_DARKBROWN), X + 0.1, Y + 34.0, 0.05);
    if (Bonus.DisplayValue != 0 || GetActivatorCVar("drpg_hud_preview"))
        HudMessage("%ld\n", Bonus.DisplayValue, HUDMSG_PLAIN, 0, (Bonus.DisplayValue >= 0 ? CR_GREEN : CR_DARKGREEN), X + 0.1, Y + 46.0, 0.05);
    
    Combo.OldValue = Player.Combo;
    XP.OldValue = Player.XPGained;
    Rank.OldValue = Player.RankGained;
    Bonus.OldValue = Player.BonusGained;
    Delay(1);
    
    goto Start;
}

NamedScript Type_ENTER void SkillHUD()
{
    Start: NOP;
    
    fixed X = GetActivatorCVar("drpg_skill_x");
    fixed Y = GetActivatorCVar("drpg_skill_y");
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    if (Timer() > 4 && !(GetActivatorCVar("drpg_menu_hideskills") && (Player.InMenu || Player.InShop || Player.OutpostMenu > 0)))
    {
        // Current Skill
        if (Player.SkillCategory[Player.SkillSelected] != -1 && Player.SkillIndex[Player.SkillSelected] != -1)
        {
            SkillPtr CurrentSkill = &Skills[Player.SkillCategory[Player.SkillSelected]][Player.SkillIndex[Player.SkillSelected]];
            SkillLevelInfo *SkillLevel = &Player.SkillLevel[Player.SkillCategory[Player.SkillSelected]][Player.SkillIndex[Player.SkillSelected]];
            
            int Cost = ScaleEPCost(CurrentSkill->Cost * SkillLevel->CurrentLevel);
            int Color = CR_LIGHTBLUE;
            
            if (Player.EP < Cost)
                Color = CR_RED;
            
            // Cost
            SetFont("SMALLFONT");
            HudMessage("%d\n", Cost, HUDMSG_PLAIN, 0, Color, X, Y - 16.0, 0.05);
            
            // Level
            HudMessage("%d/%d\n", SkillLevel->CurrentLevel, SkillLevel->Level, HUDMSG_PLAIN, 0, CR_GREEN, X, Y + 16.0, 0.05);
            
            // Icon
            PrintSprite(CurrentSkill->Icon, 0, X, Y, 0.05);
        }
    }
    
    Delay(1);
    goto Start;
}

NamedScript void StimHUD()
{
    Start: NOP;
    
    fixed X = GetActivatorCVar("drpg_stim_x");
    fixed Y = GetActivatorCVar("drpg_stim_y");
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    // Stat Boosts
    if (Player.Stim.Timer > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        int TimerPercent = (int)(((fixed)Player.Stim.Timer / (fixed)Player.Stim.TimerMax) * 100.0);
        if (TimerPercent > 100)
            TimerPercent = 100;
        
        // Preview
        if (GetActivatorCVar("drpg_hud_preview"))
            TimerPercent = 100;
        
        // Time Bar
        DrawBar("Stim10", X, Y, TimerPercent, true);
        
        // Time
        SetFont("BIGFONT");
        HudMessage("%s\n", FormatTime(Player.Stim.Timer), HUDMSG_PLAIN, 0, CR_WHITE, X + 110.1, Y, 0.05);
        
        // Icons
        for (int i = 0; i < StimStatsEnd; i++)
            if (Player.Stim.ActiveBonus[i] || GetActivatorCVar("drpg_hud_preview"))
            {
                SetFont("BIGFONT");
                HudMessage("+%d\n", Player.Stim.Last[i], HUDMSG_PLAIN, 0, CompoundColors[i], X + 8.0 + ((i % 4) * 36.0), Y + 20.0 + ((i / 4) * 24.0), 0.05);
                PrintSpritePulse(StrParam("STAT%dS\n", i + 1), 0, X + 20.0 + ((i % 4) * 36.0), Y + 40.0 + ((i / 4) * 24.0), 0.75, 32.0, 0.25);
            }
    }
        
    // Powerups
    for (int i = StimPowerupStart; i < StimPowerupEnd; i++)
        if ((Player.Stim.ActiveBonus[i] && Player.Stim.PowerupTimer[i] > 0) || GetActivatorCVar("drpg_hud_preview"))
        {
            int TimerPercent = (int)(((fixed)Player.Stim.PowerupTimer[i] / (fixed)Player.Stim.PowerupTimerMax[i]) * 100.0);
            if (TimerPercent > 100)
                TimerPercent = 100;
            
            // Preview
            if (GetActivatorCVar("drpg_hud_preview"))
                TimerPercent = 100;
            
            SetFont("SMALLFONT");
            HudMessage("%s\n", FormatTime(Player.Stim.PowerupTimer[i]), HUDMSG_PLAIN, 0, CompoundColors[i], X + 112.1, Y + 64.0, 0.05);
            DrawBar(StrParam("Stim%d\n", i + 1), X, Y + 64.0, TimerPercent, true);
            
            Y += 8.0;
        }
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void MissionHUD()
{
    Start: NOP;
    
    int OldAmount = Player.Mission.Current;
    fixed X = GetActivatorCVar("drpg_mission_x");
    fixed Y = GetActivatorCVar("drpg_mission_y");
    
    Delay(1);
    
    if (Player.Mission.Active && Player.Mission.Current != OldAmount || GetActivatorCVar("drpg_hud_preview"))
    {
        SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
        SetFont("BIGFONT");
        
        if (GetActivatorCVar("drpg_hud_preview")) // Preview
        {
            HudMessage("0 / 0\n", HUDMSG_FADEOUT, MISSION_ID, CR_GREEN, X + 0.1, Y, 2.0, 1.0);
            PrintSpriteFade("PISTA0", MISSION_ID + 1, X + 11.0 - 40.0 + 0.4, Y + 15.0 + 0.4, 2.0, 1.0);
        }
        else
        {
            switch (Player.Mission.Type)
            {
            case MT_COLLECT:
                HudMessage("%d / %d\n", Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_GREEN, X + 0.1, Y, 2.0, 1.0);
                PrintSpriteFade(Player.Mission.Item->Sprite.Name, MISSION_ID + 1, X + Player.Mission.Item->Sprite.XOff - 40.0 + 0.4, Y + Player.Mission.Item->Sprite.YOff + 0.4, 2.0, 1.0);
                break;
            case MT_KILL:
                HudMessage("\Cg%s\n\C-%d / %d\n", Player.Mission.Monster->Name, Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_BRICK, X + 0.4, Y, 2.0, 1.0);
                break;
            case MT_KILLAURAS:
                HudMessage("Auras: %d / %d\n", Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_BRICK, X, Y, 2.0, 1.0);
                break;
            case MT_REINFORCEMENTS:
                HudMessage("Reinforcements: %d / %d\n", Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_BRICK, X, Y, 2.0, 1.0);
                break;
            case MT_SECRETS:
                HudMessage("Secrets: %d / %d\n", Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_YELLOW, X, Y, 2.0, 1.0);
                break;
            case MT_ITEMS:
                HudMessage("Items: %d / %d\n", Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_LIGHTBLUE, X, Y, 2.0, 1.0);
                break;
            case MT_COMBO:
                HudMessage("Combo: %d / %d\n", Player.Mission.Current, Player.Mission.Amount, HUDMSG_FADEOUT, MISSION_ID, CR_PURPLE, X, Y, 2.0, 1.0);
                break;
            }
        }
    }
    
    goto Start;
}

NamedScript Type_ENTER void AuraTimerHUD()
{
    Start: NOP;
    
    fixed Offset = 0;
    
    while (PlayerHasAura(PlayerNumber()) || GetActivatorCVar("drpg_hud_preview"))
    {
        fixed X = GetActivatorCVar("drpg_auratimer_x");
        fixed Y = GetActivatorCVar("drpg_auratimer_y");
        int AuraCount = 0;
        int AuraAdd = 0;
        int Radius = 16;
        
        SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
        
        // Calculate number of active Auras
        for (int i = 0; i < AURA_MAX; i++)
            if (Player.Aura.Type[i].Active)
                AuraCount++;
        
        // Preview
        if (GetActivatorCVar("drpg_hud_preview"))
            AuraCount = AURA_MAX;
        
        // Timer
        if (Player.Aura.Time > 0 || GetActivatorCVar("drpg_hud_preview"))
        {
            SetFont("BIGFONT");
            HudMessage("%s\n", FormatTime(Player.Aura.Time), HUDMSG_PLAIN, 0, CR_WHITE, X, Y, 0.05);
        }
        
        // Main Icon
        if (Player.Aura.Time > 0 && PlayerHasShadowAura(PlayerNumber()))
            PrintSpritePulse("AuraBlac", 0, X, Y + 32.0, 0.75, 64.0, 0.25);
        else if (Player.Aura.Time > 0)
            PrintSpritePulse("Aura", 0, X, Y, 0.75, 64.0, 0.25);
        
        // Team Aura Icon
        if (Player.Aura.Team)
            PrintSpritePulse("SAURK0", 0, X + 18.0, Y + 24.0, 0.75, 64.0, 0.25);
        
        // Orbiting Icons
        for (int i = 0; i < AURA_MAX; i++)
            if (Player.Aura.Type[i].Active || GetActivatorCVar("drpg_hud_preview"))
            {
                fixed Angle = -0.25 + ((1.0 / AuraCount) * AuraAdd++) + Offset;
                fixed XOff = X + (Radius * Cos(Angle));
                fixed YOff = Y + (Radius * Sin(Angle)) + 32.0;
                PrintSpriteAlpha(AuraIcons[i], 0, (int)XOff, (int)YOff, 0.05, 0.75);
            }
        
        // Offset
        Offset += 0.0025;
        
        Delay(1);
    }
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void PowerupHUD()
{
    Start: NOP;
    
    fixed BaseX = GetActivatorCVar("drpg_powerup_x");
    fixed BaseY = GetActivatorCVar("drpg_powerup_y");
    fixed X = BaseX;
    fixed Y = BaseY;
    int GridCount = 0;
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    // Invulnerability
    int InvulnTime = GetActorPowerupTics(0, "PowerInvulnerable");
    if (InvulnTime > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        SetFont("SMALLFONT");
        HudMessage("%s\n", FormatTime(InvulnTime), HUDMSG_PLAIN, 0, CR_GREEN, X, Y , 0.05);
        PrintSpritePulse("PINVA0", 0, X + 10.0, Y + 22.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Invisibility
    int InvisTime = GetActorPowerupTics(0, "PowerInvisibility");
    int ShadowTime = GetActorPowerupTics(0, "PowerShadow");
    int GhostTime = GetActorPowerupTics(0, "PowerGhost");
    if (InvisTime > 0 || ShadowTime > 0 || GhostTime > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        int ActualInvisTime;
        if (InvisTime > 0)
            ActualInvisTime = InvisTime;
        else if (ShadowTime > 0)
            ActualInvisTime = ShadowTime;
        else if (GhostTime > 0)
            ActualInvisTime = GhostTime;
        
        SetFont("SMALLFONT");
        HudMessage("%s\n", FormatTime(ActualInvisTime), HUDMSG_PLAIN, 0, CR_LIGHTBLUE, X, Y, 0.05);
        PrintSpritePulse("PINSA0", 0, X + 10.0, Y + 38.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Time Freeze
    int FreezeTime = GetActorPowerupTics(0, "PowerTimeFreezer");
    if (FreezeTime > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        SetFont("SMALLFONT");
        HudMessage("%s\n", FormatTime(FreezeTime), HUDMSG_PLAIN, 0, CR_GREEN, X, Y, 0.05);
        PrintSpritePulse("TIMEA0", 0, X + 17.0, Y + 44.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Regeneration Sphere
    if (Player.RegenBoostTimer > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        SetFont("SMALLFONT");
        HudMessage("%s\n", FormatTime(Player.RegenBoostTimer), HUDMSG_PLAIN, 0, CR_PURPLE, X, Y, 0.05);
        PrintSpritePulse("REGNA0", 0, X + 12.0, Y + 37.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Light Amp
    int LightAmpPowerupTime = GetActorPowerupTics(0, "PowerLightAmp");
    int LightAmpTime = LightAmpPowerupTime;
    if (LightAmpTime > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        SetFont("SMALLFONT");
        HudMessage("%s\n", FormatTime(LightAmpTime), HUDMSG_PLAIN, 0, CR_WHITE, X, Y, 0.05);
        PrintSpritePulse("PVISA0", 0, X + 13.0, Y + 8.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Computer Map/Scanner
    if (CheckInventory("PowerScanner") || GetActivatorCVar("drpg_hud_preview"))
    {
        PrintSpritePulse("PMAPA0", 0, X + 14.0, Y + 22.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Berserk
    if (CheckInventory("PowerStrength") || GetActivatorCVar("drpg_hud_preview"))
    {
        PrintSpritePulse("PSTRA0", 0, X + 12.0, Y + 13.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Wings
    if (CheckInventory("PowerFlight") || GetActivatorCVar("drpg_hud_preview"))
    {
        PrintSpritePulse("WINGA0", 0, X + 13.0, Y + 35.0, 0.75, 32.0, 0.25);
        X += 36.0;
        GridCount++;
        if ((GridCount % 4) == 0)
        {
            X = BaseX;
            Y += 32.0;
        }
    }
    
    // Iron Feet
    bool HaveIronFeet = false;
    int IronFeetPowerTime = GetActorPowerupTics(0, "PowerIronFeet");
    int IronFeetTime = IronFeetPowerTime;
    if (IronFeetTime > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        if (GridCount > 0)
        {
            X = BaseX - 36.0;
            Y = BaseY;
        }
        SetFont("SMALLFONT");
        HudMessage("%s\n", FormatTime(IronFeetTime), HUDMSG_PLAIN, 0, CR_WHITE, X, Y + 16.0, 0.05);
        PrintSpritePulse("SUITA0", 0, X + 10.0, Y + 66.0, 0.75, 32.0, 0.25);
        HaveIronFeet = true;
    }
    
    // Continues
    int Continues = CheckInventory("DRPGContinue");
    if (Continues > 0 || GetActivatorCVar("drpg_hud_preview"))
    {
        if (HaveIronFeet)
            X -= 40.0;
        else if (GridCount > 0)
            X = BaseX - 40.0;
        SetFont("SMALLFONT");
        HudMessage("%d\n", Continues, HUDMSG_PLAIN, 0, CR_GOLD, X, Y + 16.0  , 0.05);
        PrintSpritePulse("CONTA0", 0, X + 17.0, Y + 63.0, 0.75, 32.0, 0.25);
    }
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void EventHUD()
{
    str const KeySprites[MAX_NUKE_KEYS] =
    {
        "NKEYA0",
        "NKEYB0",
        "NKEYC0",
        "NKEYD0",
        "NKEYE0",
        "NKEYF0",
        "NKEYG0",
        "NKEYH0",
        "NKEYI0"
    };
    
    Start: NOP;
    
    fixed X = GetActivatorCVar("drpg_event_x");
    fixed Y = GetActivatorCVar("drpg_event_y");
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    // Thermonuclear Bomb
    if (CurrentLevel->Event == MAPEVENT_NUCLEARBOMB && CurrentLevel->BombTime > 0 && !CurrentLevel->EventCompleted)
    {
        SetFont("BIGFONT");
        HudMessage("\CgT - %s\n", FormatTime(CurrentLevel->BombTime), HUDMSG_PLAIN, NUKE_ID, CR_RED, X + 0.1, Y + 0.1, 0.05);
        
        int KeyOffset = 0;
        for (int i = 0; i < MAX_NUKE_KEYS; i++)
        {
            // Skip drawing this key if it's disabled
            if (!CurrentLevel->BombKeyActive[i]) continue;
            
            // Timer
            SetFont("BIGFONT");
            HudMessage("%s\n", FormatTime(CurrentLevel->BombKeyTimer[i]), HUDMSG_PLAIN, 0, (CurrentLevel->BombKeyDisarming[i] ? CR_RED : CR_WHITE), X + (KeyOffset % 3 * 48.0), Y + 48.0 + (KeyOffset / 3 * 48.0), 0.05);
            
            // Key
            for (int j = 0; j < MAX_PLAYERS; j++)
                if (CheckActorInventory(Players(j).TID, StrParam("DRPGNukeKey%d\n", i + 1)))
                    PrintSprite(StrParam("%s\n", KeySprites[i]), 0, X - 12.0 + (KeyOffset % 3 * 48.0), Y + 52.0 + (KeyOffset / 3 * 48.0), 0.05);
            
            // Icon
            PrintSprite(StrParam("E_NKEY%d\n", i + 1), 0, X + (KeyOffset % 3 * 48.0), Y + 34.0 + (KeyOffset / 3 * 48.0), 0.05);
            
            KeyOffset++;
        }
    }
    
    // Low Power
    if (CurrentLevel->Event == MAPEVENT_LOWPOWER && !CurrentLevel->PowerGeneratorActive)
    {
        // Draw if someone has a working power cell
        for (int i = 0; i < MAX_PLAYERS; i++)
            if (CheckActorInventory(Players(i).TID, "DRPGGeneratorCell") > 0)
                PrintSprite("GCELA0", 0, X + 8.1, Y + 33.1, 0.05);
    }
    
    // Environmental Hazard
    if (CurrentLevel->Event == MAPEVENT_TOXICHAZARD && !CurrentLevel->EventCompleted)
    {
        str GeneratorStatus = "\CgStopped\C-";
        if (CurrentLevel->GeneratorFuel > 0)
            GeneratorStatus = "\CdRunning\C-";
        SetFont("BIGFONT");
        HudMessage("\CdHazard Level %d\n", CurrentLevel->HazardLevel, HUDMSG_PLAIN, 0, CR_RED, X + 0.1, Y + 0.1, 0.05);
        
        PrintSprite("RADMA0", 0, X + 33.1, Y + 56.1, 0.05);
        SetFont("SMALLFONT");
        HudMessage("\CqToxicity: \Cd%d%%\n\CcGenerator status: %s \Ck(%s left)\n\CqSpare fuel tanks: \Cd%d\n", ((CurrentLevel->HazardLevel - 1) * 100) + CurrentLevel->RadLeft, GeneratorStatus, FormatTime(CurrentLevel->GeneratorFuel), CheckInventory("DRPGNeutralizerFuel"), HUDMSG_PLAIN, 0, CR_RED, X + 71.1, Y + 16.1, 0.05);
    }
    
    // Hell Unleashed
    if ((CurrentLevel->Event == MAPEVENT_HELLUNLEASHED && CurrentLevel->HellUnleashedActive >= 2) || GetActivatorCVar("drpg_hud_preview"))
    {
        PrintSpritePulse("MonLevel", 0, X + 0.1, Y, 0.75, 32.0, 0.25);
        PrintSpritePulse("STAT8S", 0, X + 8.1, Y + 38.0, 0.75, 32.0, 0.25);
        
        SetFont("BIGFONT");
        HudMessage("+%k\n", CurrentLevel->LevelAdd, HUDMSG_PLAIN, 0, CR_WHITE, X + 24.1, Y, 0.05); // TODO: BOLD
        HudMessage("+%k%%\n", CurrentLevel->RareAdd, HUDMSG_PLAIN, 0, CR_GOLD, X + 24.1, Y + 18.0, 0.05); // TODO: BOLD
    }
    
    // Doomsday
    if (CurrentLevel->Event == MAPEVENT_DOOMSDAY)
    {
        SetFont("BIGFONT");
        HudMessage("\CgDEATH IN %s\n", FormatTime(CurrentLevel->DoomTime), HUDMSG_PLAIN, NUKE_ID, CR_RED, X + 0.1, Y + 0.1, 0.05);
    }
    
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void CoopViewHUD()
{
    Start:
    
    while (Player.PlayerView != PlayerNumber())
    {
        fixed X = GetActivatorCVar("drpg_coopview_x");
        fixed Y = GetActivatorCVar("drpg_coopview_y");
        
        SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
        
        if (!Player.InMenu && !Player.InShop && !Player.OutpostMenu && !Player.InMinigame)
        {
            SetFont("BIGFONT");
            HudMessage("%N\n", PlayerNumber() + 1, HUDMSG_PLAIN, 0, CR_WHITE, X + 0.1, Y + 8.0, 0.05);
            PrintSprite(StrParam("P%dSVIEW\n", Player.PlayerView + 1), 0, X + 0.1, Y + 0.1, 0.05);
        }
        
        Delay(1);
    }
}

NamedScript void MultiplayerHUD()
{
    Start:
    
    while ((InMultiplayer && GetActivatorCVar("drpg_multiplayer_hud")) || GetActivatorCVar("drpg_hud_preview"))
    {
        fixed X = GetActivatorCVar("drpg_multiplayer_x");
        fixed Y = GetActivatorCVar("drpg_multiplayer_y");
        
        SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
        
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            // Skip this player if they aren't in-game
            if (!PlayerInGame(i)) continue;
            
            int HealthPercent = (int)(((fixed)Players(i).ActualHealth / (fixed)Players(i).HealthMax) * 100.0);
            int ShieldPercent = (int)(((fixed)Players(i).Shield.Charge / (fixed)Players(i).Shield.Capacity) * 100.0);
            fixed Alpha = 1.0;
            
            // Health Critical
            if (HealthPercent <= 10)
            {
                Alpha = 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25);
                PrintSpritePulse("HLTHCRIT", 0, X - 8.0, Y + 5.0, 0.75, 32.0, 0.25);
            }
            
            SetFont("SMALLFONT");
            
            if (Players(i).Shield.Active)
            {
                HudMessage("%N\C- (\Cv%d/%d\C-)\n", i + 1, Players(i).Shield.Charge, Players(i).Shield.Capacity, HUDMSG_PLAIN | HUDMSG_ALPHA, 0, CR_WHITE, X + 0.1, Y, 0.05, Alpha);
                DrawBar("MPFill2", X, Y + 8.0, ShieldPercent, true);
            }
            else
                HudMessage("%N\C- (\Ca%d/%d\C-)\n", i + 1, Players(i).ActualHealth, Players(i).HealthMax, HUDMSG_PLAIN | HUDMSG_ALPHA, 0, CR_WHITE, X + 0.1, Y, 0.05, Alpha);
            
            DrawBar("MPFill", X, Y + 8.0, HealthPercent, true);
            
            Y += 16.0;
        }
        
        Delay(1);
    }
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void TurretHUD()
{
    int const AmmoColors[5] =
    {
        CR_BRICK,
        CR_ORANGE,
        CR_GREY,
        CR_LIGHTBLUE,
        CR_GREEN
    };
    
    InterpData Health;
    Health.TimerMaxCap = 2;
    
    Start:
    
    while (Player.Turret.Active || Player.Turret.Maintenance || GetActivatorCVar("drpg_hud_preview"))
    {
        fixed X = GetActivatorCVar("drpg_turret_x");
        fixed Y = GetActivatorCVar("drpg_turret_y");
        int Ammo[TW_MAX] =
        {
            Player.Turret.BulletAmmo,
            Player.Turret.ShellAmmo,
            Player.Turret.RocketAmmo,
            Player.Turret.PlasmaAmmo,
            Player.Turret.RailAmmo
        };
        str AmmoIcon = "";
        
        SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
        
        Health.Value = Player.Turret.Health;
        
        // Interpolation
        Health = Interpolate(Health);
        
        // Determine Ammo Icon
        switch (Player.Turret.Weapon)
        {
            case TW_BULLET:                     AmmoIcon = "CLIPA0";    break;
            case TW_PELLET:                     AmmoIcon = "SHELA0";    break;
            case TW_ROCKET:                     AmmoIcon = "ROCKA0";    break;
            case TW_PLASMA: case TW_RAILGUN:    AmmoIcon = "CELLA0";    break;
        }
        
        if (Player.Turret.Maintenance)
        {
            // Icon
            PrintSprite("TMaint", 0, X + -16.1, Y - 4.0 + (int)(Sin((fixed)Timer() / 128.0) * 8.0), 0.05);
            
            // Timers
            SetFont("BIGFONT");
            HudMessage("%s\n", FormatTime(Player.Turret.ChargeTimer * 35), HUDMSG_PLAIN, 0, CR_YELLOW, X + 24.1, Y - 16.0, 0.05);
            HudMessage("%s\n", FormatTime(Player.Turret.RepairTimer * 35), HUDMSG_PLAIN, 0, (Player.Turret.PaidForRepair ? CR_BRICK : CR_RED), X + 24.1, Y, 0.05);
            HudMessage("%s\n", FormatTime(Player.Turret.RefitTimer * 35), HUDMSG_PLAIN, 0, CR_LIGHTBLUE, X + 24.1, Y + 16.0, 0.05);
        }
        else if (!Player.Turret.Maintenance || GetActivatorCVar("drpg_hud_preview"))
        {
            // Icons
            PrintSprite("PTURA3A7", 0, X + 0.1, Y + (int)(Sin((fixed)Timer() / 64.0) * 8.0), 0.05);
            if (Player.Turret.Weapon > 0)
                PrintSprite(AmmoIcon, 0, X + 0.1, Y + 24.0, 0.05);
            
            // Health, Battery, Ammo
            SetFont("BIGFONT");
            HudMessage("%d\n", Health.DisplayValue, HUDMSG_PLAIN, 0, CR_RED, X + 24.1, Y - 16.0, 0.05);
            HudMessage("%s\n", FormatTime(Player.Turret.Battery * 35), HUDMSG_PLAIN, 0, CR_YELLOW, X + 24.1, Y, 0.05);
            if (Player.Turret.Weapon != TW_NONE)
                HudMessage("%d\n", Ammo[Player.Turret.Weapon - 1], HUDMSG_PLAIN, 0, AmmoColors[Player.Turret.Weapon - 1], X + 24.1, Y + 16.0, 0.05);
        }
        
        Health.OldValue = Player.Turret.Health;
        Delay(1);
    }
    
    Delay(1);
    goto Start;
}

NamedScript Type_ENTER void StatHUD()
{
    int const StatColors[STAT_MAX] =
    {
        CR_RED,         // Strength
        CR_GREEN,       // Defense
        CR_BRICK,       // Vitality
        CR_LIGHTBLUE,   // Energy
        CR_PURPLE,      // Regeneration
        CR_ORANGE,      // Agility
        CR_BLUE,        // Capacity
        CR_GOLD         // Luck
    };
    
    Start:
    
    // If we're on the title map, terminate
    if (InTitle) return;
    
    int *Stats[STAT_MAX] =
    {
        &Player.Strength,
        &Player.Defense,
        &Player.Vitality,
        &Player.Energy,
        &Player.Regeneration,
        &Player.Agility,
        &Player.Capacity,
        &Player.Luck
    };
    
    int PrevStats[STAT_MAX];
    bool Change = false;
    fixed X = GetActivatorCVar("drpg_stats_x");
    fixed Y = GetActivatorCVar("drpg_stats_y");
    
    SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
    
    // Determine if there was a change in your stats
    for (int i = 0; i < STAT_MAX; i++)
        if (*Stats[i] != PrevStats[i])
        {
            Change = true;
            break;
        }
    
    if (Change || GetActivatorCVar("drpg_stats_alwaysshow") || GetActivatorCVar("drpg_hud_preview"))
        for (int i = 0; i < STAT_MAX; i++)
        {
            SetFont("BIGFONT");
            HudMessage("%d\n", *Stats[i], HUDMSG_FADEOUT, STAT_ID + i, StatColors[i], X + 9.0 + ((i % 4) * 36.0), Y + ((i / 4) * 24.0), 2.0, 3.0);
            PrintSpriteFade(StrParam("STAT%dS\n", i + 1), STAT_ID + STAT_MAX + i, X + 20.0 + ((i % 4) * 36.0), Y + 20.0 + ((i / 4) * 24.0), 2.0, 3.0);
        }
    
    // Store stats for comparison next tic
    for (int i = 0; i < STAT_MAX; i++)
        PrevStats[i] = *Stats[i];
    
    Delay(1);
    goto Start;
}

NamedScript void DamageHUD(int Amount, bool Critical)
{
    // Return if the CVAR is disabled
    if (!GetActivatorCVar("drpg_damagenumbers_hud")) return;
    
    int Color = CR_WHITE;
    str Text = "";
    fixed Time = 1.0;
    
    // Size
    if (Amount == 1)
        SetFont("SMALLFONT");
    else
        SetFont("BIGFONT");
    
    // Color
    switch (Player.DamageType)
    {
    case DT_NORMAL:                     Color = CR_WHITE;       break;
    case DT_TOXIC: case DT_RADIATION:   Color = CR_GREEN;       break;
    case DT_MELEE:                      Color = CR_GREY;        break;
    case DT_FIRE:                       Color = CR_RED;         break;
    case DT_PLASMA: case DT_LIGHTNING:  Color = CR_LIGHTBLUE;   break;
    }
    
    if (Amount > 0 && Amount < SHIELD_HEALTH - Player.HealthMax - Amount)
    {
        // Fatal Blow
        if (!Player.Shield.Active && Player.ActualHealth - Amount <= 0)
        {
            SetFont("BIGFONT");
            Color = CR_DARKRED;
            Text = StrParam("%s\CrFATAL\n\n", Text);
            Time += 4.0;
        }
        
        // Critical Hit
        if (Critical)
        {
            SetFont("BIGFONT");
            Text = StrParam("%s\CgCRITICAL\n\n", Text);
            Time += 1.0;
        }
        
        // Status Effect
        if (Player.StatusTypeHUD >= 0)
        {
            SetFont("BIGFONT");
            Text = StrParam("%s%s %s\n\n", Text, StatusEffects[Player.StatusTypeHUD], StatusNumerals[Player.StatusIntensity[Player.StatusTypeHUD]]);
            Time += 1.0;
            Player.StatusTypeHUD = -1;
        }
        
        // Loss
        Text = StrParam("%s%d\n", Text, Amount);
        
        // Display
        HudMessage("%s\n", Text, HUDMSG_FADEOUT, 0, Color, 1.4 + RandomFixed(0.0, 0.4), 0.6 + RandomFixed(0.0, 0.3), 0.5, Time);
    }
}

NamedScript Type_ENTER void DRLAHUD()
{
    str const RaritySuffix[6] =
    {
        " \Ct[Exotic]\C-",
        " \Ci[Superior]\C-",
        " \Cv[Assembled]\C-",
        " \Cd[Unique]\C-",
        " \Cg[Demonic]\C-",
        " \Cf[Legendary]\C-"
        // " \Ct[Pony]\C-"
    };
    
    if (CompatMode != COMPAT_DRLA) return;
    
    Start:
    
    // If we're on the title map, terminate
    if (InTitle) return;
    
    fixed Offset;
    
    while (true)
    {
        fixed X = GetActivatorCVar("drpg_drla_x");
        fixed Y = GetActivatorCVar("drpg_drla_y");
        bool IsTechnician = (PlayerClass(PlayerNumber()) == 2);
        int Weapons = CheckInventory("RLWeaponLimit");
        int Armors = CheckInventory("RLArmorInInventory");
        int ModPacks = (IsTechnician ? CheckInventory("RLScavengerModLimit") : CheckInventory("RLModLimit"));
        int Skulls = CheckInventory("RLSkullLimit");
        int Devices = CheckInventory("RLPhaseDeviceLimit");
        
        SetHudSize(GetActivatorCVar("drpg_hud_width"), GetActivatorCVar("drpg_hud_height"), false);
         
        // Counters
        if (!(GetCVar("drpg_debug_drla") & DDM_NOLIMITS))
        {
            fixed XOff = X - 54.0; // +27.0 offset
            
            if (Devices > 0)
            {
                SetFont("BIGFONT");
                if (Devices >= DRLA_DEVICE_MAX)
                    HudMessage("%d\n", Devices, HUDMSG_ALPHA, 0, CR_GOLD, XOff, Y + 12.0, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25));
                else
                    HudMessage("%d\n", Devices, HUDMSG_PLAIN, 0, CR_WHITE, XOff, Y + 12.0, 0.05);
                PrintSprite("PHS1I0", 0, XOff - 7.0, Y, 0.05);
                XOff -= 27.0;
            }
            if (Skulls > 0)
            {
                SetFont("BIGFONT");
                if (Skulls >= DRLA_SKULL_MAX)
                    HudMessage("%d\n", Skulls, HUDMSG_ALPHA, 0, CR_GOLD, XOff, Y + 12.0, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25));
                else
                    HudMessage("%d\n", Skulls, HUDMSG_PLAIN, 0, CR_WHITE, XOff, Y + 12.0, 0.05);
                PrintSprite("ISKLC0", 0, XOff + 11.0, Y + 16.0, 0.05);
                XOff -= 27.0;
            }
            if (ModPacks > 0)
            {
                SetFont("BIGFONT");
                if ((!IsTechnician && ModPacks >= 4) || (IsTechnician && ModPacks >= 8))
                    HudMessage("%d\n", ModPacks, HUDMSG_ALPHA, 0, CR_GOLD, XOff, Y + 12.0, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25));
                else
                    HudMessage("%d\n", ModPacks, HUDMSG_PLAIN, 0, CR_WHITE, XOff, Y + 12.0, 0.05);
                PrintSprite("GMODICON", 0, XOff + 1.0, Y, 0.05);
                XOff -= 27.0;
            }
            if (Armors > 0)
            {
                SetFont("BIGFONT");
                if (Armors >= DRLA_ARMOR_MAX)
                    HudMessage("%d\n", Armors, HUDMSG_ALPHA, 0, CR_GOLD, XOff, Y + 12.0, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25));
                else
                    HudMessage("%d\n", Armors, HUDMSG_PLAIN, 0, CR_WHITE, XOff, Y + 12.0, 0.05);
                PrintSprite("HARMOR", 0, XOff + 1.0, Y + 4.0, 0.05);
                XOff -= 27.0;
            }
            if (Weapons > 0)
            {
                SetFont("BIGFONT");
                if (Weapons >= 6)
                    HudMessage("%d\n", Weapons, HUDMSG_ALPHA, 0, CR_GOLD, XOff, Y + 12.0, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25));
                else
                    HudMessage("%d\n", Weapons, HUDMSG_PLAIN, 0, CR_WHITE, XOff, Y + 12.0, 0.05);
                PrintSprite("PISGX0", 0, XOff + 11.0, Y + 12.0, 0.05);
            }
        }
        
        for (int i = 0; i < ItemMax[0]; i++)
        {
            ItemInfoPtr ItemPtr = &ItemData[0][i];
            
            if (CheckWeapon(ItemPtr->Actor))
            {
                bool Duel = false;
                str Name = StrParam("%s\n", ItemPtr->Name);
                str Color = "";
                int TotalMax = 0;
                int Total[2] = { CheckInventory(StrParam("%sModLimit\n", ItemPtr->Actor)), 0 };
                int Power[2] = { CheckInventory(StrParam("%sPowerMod\n", ItemPtr->Actor)), 0 };
                int Bulk[2] = { CheckInventory(StrParam("%sBulkMod\n", ItemPtr->Actor)), 0 };
                int Agility[2] = { CheckInventory(StrParam("%sAgilityMod\n", ItemPtr->Actor)), 0 };
                int Tech[2] = { CheckInventory(StrParam("%sTechnicalMod\n", ItemPtr->Actor)), 0 };
                int Sniper[2] = { CheckInventory(StrParam("%sSniperMod\n", ItemPtr->Actor)), 0 };
                int Firestorm[2] = { CheckInventory(StrParam("%sFirestormMod\n", ItemPtr->Actor)), 0 };
                int Nano[2] = { CheckInventory(StrParam("%sNanoMod\n", ItemPtr->Actor)), 0 };
                int DemonArtifacts[2] = { CheckInventory(StrParam("%sDemonArtifacts\n", ItemPtr->Actor)), 0 };
                
                // Determine total modpacks and the color char to use
                if (CheckInventory("RLStandardWeaponToken"))
                {
                    Color = "\Cj";
                    TotalMax = 4;
                }
                else if (CheckInventory("RLExoticWeaponToken"))
                {
                    Name = StrLeft(Name, StrLen(Name) - StrLen(RaritySuffix[0]));
                    Color = "\Ct";
                    TotalMax = 4;
                }
                else if (CheckInventory("RLSuperiorWeaponToken"))
                {
                    Name = StrLeft(Name, StrLen(Name) - StrLen(RaritySuffix[1]));
                    Color = "\Ci";
                    TotalMax = 2;
                }
                else if (CheckInventory("RLAssembledWeaponToken"))
                {
                    Name = StrLeft(Name, StrLen(Name) - StrLen(RaritySuffix[2]));
                    Color = "\Cv";
                    TotalMax = 2;
                }
                else if (CheckInventory("RLUniqueWeaponToken"))
                {
                    Name = StrLeft(Name, StrLen(Name) - StrLen(RaritySuffix[3]));
                    Color = "\Cd";
                    TotalMax = 1;
                }
                else if (CheckInventory("RLDemonicWeaponToken"))
                {
                    Name = StrLeft(Name, StrLen(Name) - StrLen(RaritySuffix[4]));
                    Color = "\Cg";
                    TotalMax = 1;
                }
                else if (CheckInventory("RLLegendaryWeaponToken"))
                {
                    Name = StrLeft(Name, StrLen(Name) - StrLen(RaritySuffix[5]));
                    Color = "\Cf";
                    TotalMax = 1;
                }
                else break; // Kinda lolhax
                
                // Synthfire/Duel-wielded weapons
                if (CheckWeapon("RLAntiFreakJackal") && CheckInventory("RLAntiFreakJackalDemonArtifacts")) // Jackal/Casull
                {
                    Duel = true;
                    Name = "\CdAnti-Freak Jackal & Hellsing ARMS Casull";
                    Total[1] = CheckInventory("RLHellsingARMSCasullModLimit");
                    Power[1] = CheckInventory("RLHellsingARMSCasullPowerMod");
                    Bulk[1] = CheckInventory("RLHellsingARMSCasullBulkMod");
                    Agility[1] = CheckInventory("RLHellsingARMSCasullAgilityMod");
                    Tech[1] = CheckInventory("RLHellsingARMSCasullTechnicalMod");
                    Sniper[1] = CheckInventory("RLHellsingARMSCasullSniperMod");
                    Firestorm[1] = CheckInventory("RLHellsingARMSCasullFirestormMod");
                    Nano[1] = CheckInventory("RLHellsingARMSCasullNanoMod");
                }
                else if (CheckWeapon("RLMarathonShotguns") && CheckInventory("RLMarathonShotgunsDemonArtifacts")) // Twin WSTE-M5's
                {
                    Duel = true;
                    Name = "\CdTwin WSTE-M5 Shotguns";
                }
                else if (CheckWeapon("RLUzi") && CheckInventory("RLUziDemonArtifacts")) // Duel Uzis
                    Duel = true;
                
                // Name
                SetFont("RLFONT");
                HudMessage("%s%s\n", Color, Name, HUDMSG_PLAIN, 0, CR_WHITE, X, Y - 20.0, 0.05);
                
                // Duke 2 Rifle special icon fancyness
                if (CheckWeapon("RLDuke2Rifle"))
                {
                    str const DukeModIcons[5] =
                    {
                        "DN2PAICO",
                        "DN2PBICO",
                        "DN2PCICO",
                        "DN2PDICO",
                        "DN2PEICO"
                    };
                    
                    int Mods[5] =
                    {
                        1, // You always have the default shot
                        Sniper[0],
                        Firestorm[0],
                        Nano[0],
                        (GetActorPowerupTics(0, "PowerRLDuke2RifleRapidFire") > 0)
                    };
                    
                    int Count = 0;
                    int ModAdd = 0;
                    int Radius = 24;
                    
                    // Count the mods
                    for (int i = 0; i < 5; i++)
                        if (Mods[i])
                            Count++;
                    
                    // Draw the mods
                    for (int i = 0; i < 5; i++)
                        if (Mods[i])
                        {
                            fixed Angle = -0.25 + ((1.0 / Count) * ModAdd++) + Offset;
                            fixed XOff = X + (Radius * Cos(Angle));
                            fixed YOff = Y + (Radius * Sin(Angle));
                            PrintSprite(DukeModIcons[i], 0, (int)XOff + ItemPtr->Sprite.XOff - 12.0, (int)YOff + ItemPtr->Sprite.YOff + 8.0, 0.05);
                        }
                    
                    // Offset
                    Offset += 0.005;
                }
                else // Everything else
                {
                    // Demonic Weapons special artifact icons and animation
                    if (CheckInventory("RLDemonicWeaponToken"))
                    {
                        int Count = DemonArtifacts[0] + DemonArtifacts[1];
                        int Add = 0;
                        int Radius = 24 + (int)(Sin((fixed)Offset / 64.0) * 4.0);
                        
                        // Draw the mods
                        for (int i = 0; i < Count; i++)
                        {
                            fixed Angle = -0.25 + ((1.0 / Count) * Add++) + Offset;
                            fixed XOff = X + (Radius * Cos(Angle));
                            fixed YOff = Y + (Radius * Sin(Angle));
                            PrintSpritePulse("DMNAA0", 0, (int)XOff + 16.0, (int)YOff + 44.0, 0.5, 256.0, 0.25);
                        }
                        
                        // Offset
                        Offset += 0.005;
                    }
                    
                    // Mod Packs
                    SetFont("SMALLFONT");
                    /* TODO: Turn this into a loop
                     * Maybe I'll bring these back if I can come up with a decent way to get the actual total mods the weapon can ahve
                       otherwise right now it's just too inaccurate to be useful
                    if (Total[0] > 0 && Total[0] >= TotalMax)
                        HudMessage("%d\n", Total[0], HUDMSG_ALPHA, 0, CR_GOLD, X - 50.0, Y, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25))
                    else if (Total[0] > 0)
                        HudMessage("%d\n", Total[0], HUDMSG_PLAIN, 0, CR_WHITE, X - 50.0, Y, 0.05);
                    if (Total[1] > 0 && Total[0] >= TotalMax)
                        HudMessage("%d\n", Total[1], HUDMSG_ALPHA, 0, CR_GOLD, X - 50.0, Y + 12.0, 0.05, 0.75 + (Sin((fixed)Timer() / 32.0) * 0.25))
                    else if (Total[1] > 0)
                        HudMessage("%d\n", Total[1], HUDMSG_PLAIN, 0, CR_WHITE, X - 50.0, Y + 12.0, 0.05);
                    */
                    if (Power[0] > 0)
                        HudMessage("%d\n", Power[0], HUDMSG_PLAIN, 0, CR_RED, X - 30.0, Y + 20.0, 0.05);
                    if (Bulk[0] > 0)
                        HudMessage("%d\n", Bulk[0], HUDMSG_PLAIN, 0, CR_BLUE, X - 20.0, Y + 20.0, 0.05);
                    if (Agility[0] > 0)
                        HudMessage("%d\n", Agility[0], HUDMSG_PLAIN, 0, CR_GREEN, X - 10.0, Y + 20.0, 0.05);
                    if (Tech[0] > 0)
                        HudMessage("%d\n", Tech[0], HUDMSG_PLAIN, 0, CR_YELLOW, X, Y + 20.0, 0.05);
                    if (Sniper[0] > 0)
                        HudMessage("%d\n", Sniper[0], HUDMSG_PLAIN, 0, CR_PURPLE, X + 10.0, Y + 20.0, 0.05);
                    if (Firestorm[0] > 0)
                        HudMessage("%d\n", Firestorm[0], HUDMSG_PLAIN, 0, CR_ORANGE, X + 20.0, Y + 20.0, 0.05);
                    if (Nano[0] > 0)
                        HudMessage("%d\n", Nano[0], HUDMSG_PLAIN, 0, CR_WHITE, X + 30.0, Y + 20.0, 0.05);
                    if (DemonArtifacts[0] > 0)
                        HudMessage("%d\n", DemonArtifacts[0], HUDMSG_PLAIN, 0, CR_DARKRED, X + 40.0, Y + 20.0, 0.05);
                    if (Power[1] > 0)
                        HudMessage("%d\n", Power[1], HUDMSG_PLAIN, 0, CR_RED, X - 30.0, Y + 32.0, 0.05);
                    if (Bulk[1] > 0)
                        HudMessage("%d\n", Bulk[1], HUDMSG_PLAIN, 0, CR_BLUE, X - 20.0, Y + 32.0, 0.05);
                    if (Agility[1] > 0)
                        HudMessage("%d\n", Agility[1], HUDMSG_PLAIN, 0, CR_GREEN, X - 10.0, Y + 32.0, 0.05);
                    if (Tech[1] > 0)
                        HudMessage("%d\n", Tech[1], HUDMSG_PLAIN, 0, CR_YELLOW, X, Y + 32.0, 0.05);
                    if (Sniper[1] > 0)
                        HudMessage("%d\n", Sniper[1], HUDMSG_PLAIN, 0, CR_PURPLE, X + 10.0, Y + 32.0, 0.05);
                    if (Firestorm[1] > 0)
                        HudMessage("%d\n", Firestorm[1], HUDMSG_PLAIN, 0, CR_ORANGE, X + 20.0, Y + 32.0, 0.05);
                    if (Nano[1] > 0)
                        HudMessage("%d\n", Nano[1], HUDMSG_PLAIN, 0, CR_WHITE, X + 30.0, Y + 32.0, 0.05);
                    if (DemonArtifacts[1] > 0)
                        HudMessage("%d\n", DemonArtifacts[1], HUDMSG_PLAIN, 0, CR_DARKRED, X + 40.0, Y + 32.0, 0.05);
                }
                
                // Drop Icon
                if (CheckInventory("RLWeaponDrop") || CheckInventory("RLScavengerDrop"))
                    PrintSpritePulse("DROPICON", 0, X - 30.0, Y + 12.0, 0.75, 32.0, 0.25);
                
                // Icon
                if (Duel) // Duel-wielding
                {
                    if (CheckWeapon("RLAntiFreakJackal") && CheckInventory("RLAntiFreakJackalDemonArtifacts")) // Jackal/Casull
                    {
                        ItemInfoPtr JackelItem = &ItemData[0][42]; // Should probably come up with a better way to reference these?
                        ItemInfoPtr CasullItem = &ItemData[0][43];
                        
                        PrintSprite(JackelItem->Sprite.Name, 0, X + JackelItem->Sprite.XOff, Y + JackelItem->Sprite.YOff + (int)(Sin((fixed)Timer() / 128.0) * 4.0), 0.05);
                        PrintSprite(CasullItem->Sprite.Name, 0, X + CasullItem->Sprite.XOff + 32.0, Y + CasullItem->Sprite.YOff + (int)(Cos((fixed)Timer() / 128.0) * 4.0), 0.05);
                    }
                    else if (CheckWeapon("RLUzi") && CheckInventory("RLUziDemonArtifacts") ||
                             CheckWeapon("RLMarathonShotguns") && CheckInventory("RLMarathonShotgunsDemonArtifacts")) // Everything else that is just duel wielding
                    {
                        PrintSprite(ItemPtr->Sprite.Name, 0, X + ItemPtr->Sprite.XOff, Y + ItemPtr->Sprite.YOff + (int)(Sin((fixed)Timer() / 128.0) * 4.0), 0.05);
                        PrintSprite(ItemPtr->Sprite.Name, 0, X + ItemPtr->Sprite.XOff + 32.0, Y + ItemPtr->Sprite.YOff + (int)(Cos((fixed)Timer() / 128.0) * 4.0), 0.05);
                    }
                }
                else
                {
                    if (CheckInventory("RLUsePowerMod") || CheckInventory("RLUseBulkMod") ||
                        CheckInventory("RLUseAgilityMod") || CheckInventory("RLUseTechnicalMod") ||
                        CheckInventory("RLUseFirestormMod") || CheckInventory("RLUseSniperMod") ||
                        CheckInventory("RLUseNanoMod") || CheckInventory("RLUseDemonArtifact"))
                        PrintSpritePulse(ItemPtr->Sprite.Name, 0, X + ItemPtr->Sprite.XOff, Y + ItemPtr->Sprite.YOff, 0.75, 32.0, 0.25);
                    else
                        PrintSprite(ItemPtr->Sprite.Name, 0, X + ItemPtr->Sprite.XOff, Y + ItemPtr->Sprite.YOff, 0.05);
                }
                
                break;
            }
        }
        
        Delay(1);
    }
}
