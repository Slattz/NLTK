#include <3ds.h>
#include <string>
#include <vector>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "core/gui/AboutMenu.hpp"
#include "label.h"
#include "imagebutton.h"
#include "menus.h"

#define ABOUT_CREDITSCOUNT 5
#ifndef GIT_REV
    #define GIT_REV "NOGIT"
#endif

static const char* NameStrings[ABOUT_CREDITSCOUNT] = {
    "Slattz & Cuyler:", "Marc_Max:", "WemI0:", "Nanquitas:", "PabloMK7:"
};

static const char* DescStrings[ABOUT_CREDITSCOUNT] =
{
    "Creators of NLTK.",
    "For 'ACNL Save Editor' & Documentation.",
    "For updated NLSE, Documentation & Acre Images.",
    "For helping out a ton and for CTRFont.",
    "For tips on making the startup logo."
};

static const char *discordtext = "Need help with NLTK?\n\
Join the NLTK Discord Server:\nhttps://discord.gg/j2VUBBv\n";

static const char *twittertext = "Follow on Twitter for AC Hacking,\nNLTK stuff and random things:\n@Slattz3D\n@CuylerAC";

ImageButton* DiscordIcon;
ImageButton* TwitterIcon;
static Label* CreditText;
static Label* CommitText;
static std::vector<Label *> CreditsNameText;
static std::vector<Label *> CreditsDescText;
static bool g_Initialized = false; //We only need to initialize once as this screen is static

void Core::AboutMenu::Initialize(void) {
    for (u32 i = 0; i < ABOUT_CREDITSCOUNT; i++) {
        Label* Name = new Label({10, 55+(i*20)}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, std::string(NameStrings[i]));
        Name->SetTextSize(0.5f, 0.5f);
        CreditsNameText.push_back(Name);

        Label* Desc = new Label({110, 55+(i*20)}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, std::string(DescStrings[i]));
        Desc->SetTextSize(0.47f, 0.47f);
        CreditsDescText.push_back(Desc);
    }

    CreditText = new Label(175, 30, 0, 0, COLOR_TRANSPARENT, COLOR_WHITE, std::string("Credits:"));
    CreditText->SetTextSize(0.6f, 0.6f);
    CommitText = new Label(225, 3, 0, 0, COLOR_TRANSPARENT, COLOR_GREY, std::string("Commit: " GIT_REV));
    CommitText->SetTextSize(0.4f, 0.4f);

    DiscordIcon = new ImageButton(55, 180, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, DISCORD_ICON, About_ss);
    TwitterIcon = new ImageButton(215, 180, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, TWITTER_ICON, About_ss);

    g_Initialized = true;
}

void Core::Draw_AboutMenu(void) {
    draw_base_interface();
    CreditText->Draw();

    for (const auto names : CreditsNameText) {
        names->Draw();
    }

    for (const auto desc : CreditsDescText) {
        desc->Draw();
    }

    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon
    CommitText->Draw(); //Git Commit
    DiscordIcon->Draw(); //Discord Icon
    TwitterIcon->Draw(); //Twitter Icon
    /* L:55, M:135, R:215 */

    if (DiscordIcon->IsActive())
        MsgDisp(bottom, discordtext, MsgTypeNone, COLOR_BROWN, COLOR_GREY, 0.6f, 60.f, 100.f, 200.f, 70.f);

    else if (TwitterIcon->IsActive())
        MsgDisp(bottom, twittertext, MsgTypeNone, COLOR_BROWN, COLOR_GREY, 0.6f, 60.f, 100.f, 200.f, 70.f);

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void Core::Spawn_AboutMenu(void) {

    if (!g_Initialized)
        Core::AboutMenu::Initialize(); //Since this is a static gui, no need to always reinit

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Draw_AboutMenu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_B))
            break;
    }
}