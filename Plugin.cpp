// This file added in headers queue
// File: "Sources.h"
#include "resource.h"
#include "Plugin.h"

namespace GOTHIC_ENGINE {
    const string PLUGIN_NAME = "UnlockAll";
    string unlockHotkey;
    bool skipDoors = false;
    bool isKeyPressed = false;

    void Msg(string msg) 
    {
        ogame->GetTextView()->Printwin(msg);
    }

    void CallFuncOnPlayer(int id)
    {
        parser->SetInstance("SELF", player);
        bool currentFunc = parser->IsStackFull();

        if (id != Invalid) {
            parser->CallFunc(id);
        }
    }

    void PlayDontKnow() 
    {
        oCMsgConversation* animation = new oCMsgConversation(oCMsgConversation::EV_PLAYANI_NOOVERLAY, "T_DONTKNOW");
        animation->number = 0;
        player->GetEM(false)->OnMessage(animation, player);
    }

    bool HasLockPicking() 
    {
        zCPar_Symbol* symbol = parser->GetSymbol("NPC_TALENT_PICKLOCK");
        int symbolValue;
        symbol->GetValue(symbolValue, 0);
        int symbolSkill = player->GetTalentSkill(symbolValue);
        int noLockPickingActionId = parser->GetIndex("PLAYER_MOB_MISSING_LOCKPICK");

        if (symbolSkill <= 0) {

            #if ENGINE == Engine_G1
                PlayDontKnow();
            #else
                CallFuncOnPlayer(noLockPickingActionId);
            #endif

            return false;
        }

        return true;
    }

    void UnlockVob() 
    {
        zCVob* focusVob = player->GetFocusVob();

        if (!focusVob)
            return;

        oCMobLockable* vob = focusVob->CastTo<oCMobLockable>();
        if (!vob)
            return;

        oCMobContainer* chest = focusVob->CastTo<oCMobContainer>();
        oCMobDoor* door = focusVob->CastTo<oCMobDoor>();

        if (skipDoors && door) {
            return;
        }

        if (!door && !chest)
            return;

        if (!vob->locked) {
            return;
        }

        if (vob->locked && vob->pickLockStr.IsEmpty() && !vob->keyInstance.IsEmpty()) {
            int missingKeyActionId = parser->GetIndex("PLAYER_MOB_MISSING_KEY");

            #if ENGINE == Engine_G1
                PlayDontKnow();
            #else
                CallFuncOnPlayer(missingKeyActionId);
            #endif

            return;
        }

        if (!HasLockPicking()) {
            return;
        }

        zSTRING sound = zSTRING("PICKLOCK_SUCCESS");

        oCMsgConversation* msg;
        #if ENGINE == Engine_G1
                msg = new oCMsgConversation(oCMsgConversation::EV_PLAYSOUND, sound);
        #else
                msg = new oCMsgConversation(oCMsgConversation::EV_SNDPLAY, sound);
        #endif

        msg->f_yes = true;
        msg->target = vob;
        player->GetEM(false)->OnMessage(msg, player);

        vob->locked = false;

        return;
    }

    void Game_Entry() {
    }

    void Game_Init() 
    {
        InitKeyMap();
        #if ENGINE == Engine_G1
            unlockHotkey = zoptions->ReadString(PLUGIN_NAME, "UnlockHotkey", "KEY_V").ToChar();
        #else
            unlockHotkey = zoptions->ReadString(PLUGIN_NAME, "UnlockHotkey", "MOUSE_BUTTONRIGHT").ToChar();
        #endif
        unlockHotkey = zoptions->ReadString(PLUGIN_NAME, "UnlockHotkey", "MOUSE_BUTTONRIGHT").ToChar();
        skipDoors = zoptions->ReadBool(PLUGIN_NAME, "SkipDoors", false);
    }

    void Game_Exit() {
    }

    void Game_PreLoop() {
    }

    void Game_Loop() 
    {
        if (IsHotkeyTriggered(unlockHotkey)) {
            if (!isKeyPressed) {
                UnlockVob();
                isKeyPressed = true;
            }
        }
        else {
            isKeyPressed = false;
        }
    }

    void Game_PostLoop() {
    }

    void Game_MenuLoop() {
    }

    // Information about current saving or loading world
    TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

    void Game_SaveBegin() {
    }

    void Game_SaveEnd() {
    }

    void LoadBegin() {
    }

    void LoadEnd() {
    }

    void Game_LoadBegin_NewGame() {
        LoadBegin();
    }

    void Game_LoadEnd_NewGame() {
        LoadEnd();
    }

    void Game_LoadBegin_SaveGame() {
        LoadBegin();
    }

    void Game_LoadEnd_SaveGame() {
        LoadEnd();
    }

    void Game_LoadBegin_ChangeLevel() {
        LoadBegin();
    }

    void Game_LoadEnd_ChangeLevel() {
        LoadEnd();
    }

    void Game_LoadBegin_Trigger() {
    }

    void Game_LoadEnd_Trigger() {
    }

    void Game_Pause() {
    }

    void Game_Unpause() {
    }

    void Game_DefineExternals() {
    }

    void Game_ApplyOptions() {
    }

    /*
    Functions call order on Game initialization:
      - Game_Entry           * Gothic entry point
      - Game_DefineExternals * Define external script functions
      - Game_Init            * After DAT files init

    Functions call order on Change level:
      - Game_LoadBegin_Trigger     * Entry in trigger
      - Game_LoadEnd_Trigger       *
      - Game_Loop                  * Frame call window
      - Game_LoadBegin_ChangeLevel * Load begin
      - Game_SaveBegin             * Save previous level information
      - Game_SaveEnd               *
      - Game_LoadEnd_ChangeLevel   *

    Functions call order on Save game:
      - Game_Pause     * Open menu
      - Game_Unpause   * Click on save
      - Game_Loop      * Frame call window
      - Game_SaveBegin * Save begin
      - Game_SaveEnd   *

    Functions call order on Load game:
      - Game_Pause              * Open menu
      - Game_Unpause            * Click on load
      - Game_LoadBegin_SaveGame * Load begin
      - Game_LoadEnd_SaveGame   *
    */

#define AppDefault True
    CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
        Enabled(AppDefault) Game_Entry,
        Enabled(AppDefault) Game_Init,
        Enabled(AppDefault) Game_Exit,
        Enabled(AppDefault) Game_PreLoop,
        Enabled(AppDefault) Game_Loop,
        Enabled(AppDefault) Game_PostLoop,
        Enabled(AppDefault) Game_MenuLoop,
        Enabled(AppDefault) Game_SaveBegin,
        Enabled(AppDefault) Game_SaveEnd,
        Enabled(AppDefault) Game_LoadBegin_NewGame,
        Enabled(AppDefault) Game_LoadEnd_NewGame,
        Enabled(AppDefault) Game_LoadBegin_SaveGame,
        Enabled(AppDefault) Game_LoadEnd_SaveGame,
        Enabled(AppDefault) Game_LoadBegin_ChangeLevel,
        Enabled(AppDefault) Game_LoadEnd_ChangeLevel,
        Enabled(AppDefault) Game_LoadBegin_Trigger,
        Enabled(AppDefault) Game_LoadEnd_Trigger,
        Enabled(AppDefault) Game_Pause,
        Enabled(AppDefault) Game_Unpause,
        Enabled(AppDefault) Game_DefineExternals,
        Enabled(AppDefault) Game_ApplyOptions
    );
}