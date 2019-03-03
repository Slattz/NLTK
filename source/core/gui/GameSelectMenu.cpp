#include <3ds.h>
#include <string>
#include <vector>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "common.h"
#include "core/gui/GameSelectMenu.hpp"
#include "label.h"
#include "imagebutton.h"
#include "utils.h"
#include "nfs.h"

enum class MediaType {
    None = -1,
    SD = 0,
    Cart = 1
};

enum class GameType {
    None = -1,
    Orig = 0,
    WA = 1
};

enum class RegionType {
    None = -1,
    JPN = 0,
    USA = 1,
    EUR = 2,
    KOR = 3,
};

extern NLTK_Titles_Info MediaInfo;

MediaType SelectedMedia = MediaType::None;
GameType SelectedGame = GameType::None;
RegionType SelectedRegion = RegionType::None;

static Label* ConfirmText;
static Label* SelectGameText;

static ImageButton* CartIcon;
static ImageButton* SDIcon;
static ImageButton* OrigIcon;
static ImageButton* WAIcon;
static ImageButton* JPNIcon;
static ImageButton* USAIcon;
static ImageButton* EURIcon;
static ImageButton* KORIcon;

static bool g_Initialized = false; //We only need to initialize once as this screen is static

void Core::GameSelectMenu::Initialize(void) {
    SelectedMedia = MediaType::None;
    SelectedGame = GameType::None;
    SelectedRegion = RegionType::None;

    ConfirmText = new Label(75, 190, 170, 30, COLOR_BROWN, COLOR_WHITE, std::string("Confirm"));
    ConfirmText->SetTextSize(0.7f, 0.7f);
    ConfirmText->myText.CenterInBounds(75.f, 190.f, 170.f, 30.f);

    SelectGameText = new Label(0, 0, 0, 0, COLOR_TRANSPARENT, COLOR_WHITE, std::string("Select your installed ACNL game:"));
    SelectGameText->SetTextSize(0.8f, 0.8f);
    SelectGameText->myText.CenterInBounds(0.f, 90.f, 400.f, 0.f);

    
    CartIcon = new ImageButton(101, 6, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, GAME_CART, GameSelect_ss);
    SDIcon = new ImageButton(169, 6, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, SD_CARD, GameSelect_ss);
    OrigIcon = new ImageButton(101, 66, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, ACNL_ICON, GameSelect_ss);
    WAIcon = new ImageButton(169, 66, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, ACNL_WA_ICON, GameSelect_ss);
    JPNIcon = new ImageButton(170, 130, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, JPN_FLAG, GameSelect_ss);
    USAIcon = new ImageButton(106, 127, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, USA_FLAG, GameSelect_ss);
    EURIcon = new ImageButton(42, 130, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, EUR_FLAG, GameSelect_ss);
    KORIcon = new ImageButton(234, 130, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, KOR_FLAG, GameSelect_ss);
    g_Initialized = true;
}

void Core::Draw_GameSelectMenu(void) {
    draw_base_interface();
    C2D_SceneBegin(bottom);

    // Media Type
    if (SelectedMedia == MediaType::Cart) { //Game Card
        C2D_DrawRectSolid(98, 3, 0, 54, 54, COLOR_GREY); //Grey Outline
        DrawSprite(GameSelect_ss, GAME_CART, 101, 6); //Game Card is selected
        DrawSprite(GameSelect_ss, SD_CARD, 169, 6, GreyFilter); //Grey filter over SD card
    }
    else if (SelectedMedia == MediaType::SD) { //SD Card
        C2D_DrawRectSolid(166, 3, 0, 54, 54, COLOR_GREY); //Grey Outline
        DrawSprite(GameSelect_ss, SD_CARD, 169, 6); //SD Card is selected
        DrawSprite(GameSelect_ss, GAME_CART, 101, 6, GreyFilter); //Grey Filter over Game Cart
    }
    else { //No Media selected yet
        if (MediaInfo.GameCartInfo.HasACNLData) {
            DrawSprite(GameSelect_ss, GAME_CART, 101, 6);
        }
        else {
            DrawSprite(GameSelect_ss, GAME_CART, 101, 6, GreyFilter);
        }

        if (MediaInfo.SDCardInfo.HasACNLData) {
            DrawSprite(GameSelect_ss, SD_CARD, 169, 6);
        }
        else {
            DrawSprite(GameSelect_ss, SD_CARD, 169, 6, GreyFilter);
        }
    }

    NLTK_Media_Installed mediaInstalled = SelectedMedia == MediaType::SD
        ? MediaInfo.SDCardInfo : MediaInfo.GameCartInfo;

    /* Grey Outlines */
    if (SelectedGame == GameType::Orig) //Orig ACNL
        C2D_DrawRectSolid(98, 63, 0, 54, 54, COLOR_GREY);

    else if (SelectedGame == GameType::WA) //WA ACNL
        C2D_DrawRectSolid(166, 63, 0, 54, 54, COLOR_GREY);

    if (SelectedRegion == RegionType::JPN) //JPN
        C2D_DrawRectSolid(167, 127, 0, 50, 36, COLOR_GREY);

    else if (SelectedRegion == RegionType::USA) //USA
        C2D_DrawRectSolid(103, 127, 0, 50, 33, COLOR_GREY);

    else if (SelectedRegion == RegionType::EUR) //EUR
        C2D_DrawRectSolid(39, 127, 0, 50, 36, COLOR_GREY);

    else if (SelectedRegion == RegionType::KOR) //KOR
        C2D_DrawRectSolid(231, 127, 0, 50, 36, COLOR_GREY);

    CartIcon->Draw();
    SDIcon->Draw();
    OrigIcon->Draw();
    WAIcon->Draw();
    JPNIcon->Draw();
    USAIcon->Draw();
    EURIcon->Draw();
    KORIcon->Draw();
    ConfirmText->Draw(); //Confirm Button Text

    if (SelectedMedia == MediaType::None || !mediaInstalled.InstalledTitles.ORIG_installed) //Grey out Orig ACNL Icon if no ORIG ACNL game is found
    C2D_DrawRectSolid(101, 66, 0, 48, 48, COLOR_GREY_FILTER);

    if (SelectedMedia == MediaType::None || !mediaInstalled.InstalledTitles.WA_installed) //Grey out WA ACNL Icon if no WA ACNL game is found
    C2D_DrawRectSolid(169, 66, 0, 48, 48, COLOR_GREY_FILTER);

    if (SelectedGame == GameType::Orig)
    {
        if (!mediaInstalled.InstalledTitles.ORIG_JPN_installed) //Grey out JPN Flag if no orig JPN game is found
            C2D_DrawRectSolid(170, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_USA_installed) //Grey out USA Flag if no orig USA game is found
            C2D_DrawRectSolid(106, 130, 0, 44, 27, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_EUR_installed) //Grey out EUR Flag if no orig EUR game is found
            C2D_DrawRectSolid(42, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_KOR_installed) //Grey out KOR Flag if no orig KOR game is found
            C2D_DrawRectSolid(234, 130, 0, 44, 30, COLOR_GREY_FILTER);
    }

    else if (SelectedGame == GameType::WA)
    {
        if (!mediaInstalled.InstalledTitles.WA_JPN_installed) //Grey out JPN Flag if no WA JPN game is found
            C2D_DrawRectSolid(170, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_USA_installed) //Grey out USA Flag if no WA USA game is found
            C2D_DrawRectSolid(106, 130, 0, 44, 27, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_EUR_installed) //Grey out EUR Flag if no WA EUR game is found
            C2D_DrawRectSolid(42, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_KOR_installed) //Grey out KOR Flag if no WA KOR game is found
            C2D_DrawRectSolid(234, 130, 0, 44, 30, COLOR_GREY_FILTER);
    }

    else
    {
        C2D_DrawRectSolid(170, 130, 0, 44, 30, COLOR_GREY_FILTER); //JPN
        C2D_DrawRectSolid(106, 130, 0, 44, 27, COLOR_GREY_FILTER); //USA
        C2D_DrawRectSolid(42, 130, 0, 44, 30, COLOR_GREY_FILTER); //EUR
        C2D_DrawRectSolid(234, 130, 0, 44, 30, COLOR_GREY_FILTER); //KOR
    }

    C2D_SceneBegin(top);
    SelectGameText->Draw();
    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

u64 Core::Spawn_GameSelectMenu(FS_MediaType &mediaType) {
    checkInstalledTitles();

    if (MediaInfo.GameCartInfo.HasACNLData == false && MediaInfo.SDCardInfo.HasACNLData == false)
    {
        MsgDisp(top, "Error:\nACNL could not be found!");
        return 0;
    }

    if (g_Initialized == false) Core::GameSelectMenu::Initialize();

    while (aptMainLoop())
    {
        if (SelectedMedia == MediaType::Cart && !IsGameCartInserted()) {
            DisplayCardError();
        }

        if (SelectedMedia == MediaType::None || SelectedGame == GameType::None || SelectedRegion == RegionType::None)
            ConfirmText->BackgroundColor = COLOR_BROWN; //Confirm Button

        else
            ConfirmText->BackgroundColor =  COLOR_LIGHT_BROWN; //Confirm Button (Can Confirm)

        Core::Draw_GameSelectMenu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_START))
            break;

        NLTK_Media_Installed mediaInstalled = SelectedMedia == MediaType::SD
            ? MediaInfo.SDCardInfo : MediaInfo.GameCartInfo;

        if (CartIcon->IsActive() && MediaInfo.GameCartInfo.HasACNLData)
        {
            SelectedMedia = MediaType::Cart;
            SelectedGame = GameType::None;
            SelectedRegion = RegionType::None;
        }
        else if (SDIcon->IsActive() && MediaInfo.SDCardInfo.HasACNLData)
        {
            SelectedMedia = MediaType::SD;
            SelectedGame = GameType::None;
            SelectedRegion = RegionType::None;
        }
        else if (SelectedMedia != MediaType::None && OrigIcon->IsActive() && mediaInstalled.InstalledTitles.ORIG_installed)
        {
            SelectedGame = GameType::Orig;    //Orig ACNL
            SelectedRegion = RegionType::None; //Reset region, user mightn't have same regions installed for WA ACNL
        }
        else if (SelectedMedia != MediaType::None && WAIcon->IsActive() && mediaInstalled.InstalledTitles.WA_installed)
        {
            SelectedGame = GameType::WA;    //WA ACNL
            SelectedRegion = RegionType::None; //Reset region, user may not have same regions installed for orig ACNL
        }

        else if (JPNIcon->IsActive())
        {
            if (SelectedGame == GameType::Orig && mediaInstalled.InstalledTitles.ORIG_JPN_installed) //if orig ACNL
                SelectedRegion = RegionType::JPN;                                                     //JPN

            else if (SelectedGame == GameType::WA && mediaInstalled.InstalledTitles.WA_JPN_installed) //if WA ACNL
                SelectedRegion = RegionType::JPN;                                                        //JPN
        }

        else if (USAIcon->IsActive())
        {
            if (SelectedGame == GameType::Orig && mediaInstalled.InstalledTitles.ORIG_USA_installed) //if orig ACNL
                SelectedRegion = RegionType::USA;                                                         //USA

            else if (SelectedGame == GameType::WA && mediaInstalled.InstalledTitles.WA_USA_installed) //if WA ACNL
                SelectedRegion = RegionType::USA;                                                          //USA
        }
        else if (EURIcon->IsActive())
        {
            if (SelectedGame == GameType::Orig && mediaInstalled.InstalledTitles.ORIG_EUR_installed) //if orig ACNL
                SelectedRegion = RegionType::EUR;                                                         //EUR

            else if (SelectedGame == GameType::WA && mediaInstalled.InstalledTitles.WA_EUR_installed) //if WA ACNL
                SelectedRegion = RegionType::EUR;                                                          //EUR
        }
        else if (KORIcon->IsActive())
        {
            if (SelectedGame == GameType::Orig && mediaInstalled.InstalledTitles.ORIG_KOR_installed) //if orig ACNL
                SelectedRegion = RegionType::KOR;                                                         //KOR

            else if (SelectedGame == GameType::WA && mediaInstalled.InstalledTitles.WA_KOR_installed) //if WA ACNL
                SelectedRegion = RegionType::KOR;                                                          //KOR
        }

        else if (ConfirmText->IsActive()) //Confirm Button
        {
            if (SelectedMedia == MediaType::None)
                MsgDisp(top, "Please select a media type!");

            else if (SelectedGame == GameType::None)
                MsgDisp(top, "Please select a game!");

            else if (SelectedRegion == RegionType::None)
                MsgDisp(top, "Please select a region!");

            else
            {
                // Set Media Type
                mediaType = static_cast<FS_MediaType>(static_cast<u8>(SelectedMedia) + 1);

                if (SelectedGame == GameType::Orig)
                    return JPN_TID + (static_cast<u8>(SelectedRegion) * 0x100); //Form orig TID

                else if (SelectedGame == GameType::WA)
                    return JPN_WA_TID + (static_cast<u8>(SelectedRegion) * 0x100); //Form WA TID

                else
                {
                    MsgDisp(top, Format("Error:\nSelected Media Type is: %d\nSelected Game is: %d\nSelected Region is: %d",
                            static_cast<u8>(SelectedMedia), static_cast<u8>(SelectedGame), static_cast<u8>(SelectedRegion)));
                    return 0;
                }
            }
        }
    }
    return 0;
}