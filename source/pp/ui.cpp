#include "pp/ui.h"

#include <ft/ft_manager.h>
#include <gf/gf_draw.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
#include <GX/GXPixel.h>

#include "pp/collections/linkedlist.h"
#include "pp/common.h"
#include "pp/input/pad.h"
#include "pp/menu/menu.h"
#include "pp/menu/options/bool_option.h"
#include "pp/menu/options/int_option.h"
#include "pp/menu/options/choice_option.h"
#include "pp/menu/options/float_option.h"
#include "pp/menu/options/label_option.h"
#include "pp/menu/options/spacer_option.h"
#include "pp/menu/options/subpage_option.h"
#include "pp/menu/options/playerdata_observer.h"

#include "pp/playerdata.h"
#include "pp/popup.h"

namespace PP {

using namespace Input;

/* EXTERN DEFS */
PpunchMenu& punchMenu = *(new PpunchMenu());
/************/

void ui_cpp_static_dtor() {
    delete &punchMenu;
}

void PpunchMenu::init() {
    /* Controls Info Page */
    DEBUG_MENU("Initializing Project Punch menu.\n");
    Page& ctrlInfoPage = *(new Page(this));
    snprintf(ctrlInfoPage.title, 256, "Menu Controls");
    ctrlInfoPage.addOption(new LabelOption("L+R+DpadUp", "Open and close the menu."));
    ctrlInfoPage.addOption(new LabelOption("L/R", "Prev/Next Page"));
    ctrlInfoPage.addOption(new LabelOption("A/B",  "Select / Cancel"));
    ctrlInfoPage.addOption(new LabelOption("DPad/Stick U/D", "Up/Down"));
    ctrlInfoPage.addOption(new LabelOption("DPad/Stick", "Adjust selected value"));
    ctrlInfoPage.addOption(new LabelOption("Start", "Exit the Menu"));
    ctrlInfoPage.addOption(new LabelOption("X (hold)", "Turbo adjust values"));
    ctrlInfoPage.addOption(new LabelOption("Y (hold)", "Adjust values by 5x"));
    ctrlInfoPage.addOption(new LabelOption("L/R (hold on startup)", "Disable menu autoopen."));
    addPage(&ctrlInfoPage);

    Page* globalSettingsPage = new Page(this);
    snprintf(globalSettingsPage->title, 256, "Global Settings");
    globalSettingsPage->addOption(new IntOption<int>("Max On-Screen Displays", GlobalSettings::maxOsdLimit, 1, 10, true, false));
    globalSettingsPage->addOption(new IntOption<int>("Max Ledgedash Viz Frames", GlobalSettings::maxLedgedashVizFrames, 30, 64, true, false));
    globalSettingsPage->addOption(new BoolOption("Enable Frame Advance", GlobalSettings::enableFrameAdvance, true));
    globalSettingsPage->addOption(new BoolOption("Invisible Tech Reaction Trainer", GlobalSettings::enableInvisibleTechs, true));
    globalSettingsPage->addOption(new ChoiceOption(
        "Frame Advance Button", 
        frameAdvanceButtonOptions, 
        *((int*)&GlobalSettings::frameAdvanceButton),
        PP_FAB_OPT_COUNT
    ));
    globalSettingsPage->addOption(new IntOption<int>("Frame Advance Hold Delay (Frames)", GlobalSettings::frameAdvanceRepeatDelayFrames, 5, 60, true, false));
    globalSettingsPage->addOption(new IntOption<int>("Actionable OoShield Minimum", GlobalSettings::shieldActionabilityTolerance, 0, 5, true, false));
    this->addPage(globalSettingsPage);
    

    u32 fighters = g_ftManager->getEntryCount();
    DEBUG_MENU("Detected %d fighters.\n", fighters);
    for (u32 i = 0; i < fighters; i++) {
        Page& newPage = *(new Page(this));
        PlayerData& player = allPlayerData[i];
        const char* ftName = player.fighterName;
        DEBUG_MENU("Adding page for P%d: %s @ 0x%x\n", player.playerNumber, ftName, (void*)&player);
        snprintf(newPage.title, 256, "P%d = %s", i+1, player.fighterName);
        newPage.addOption(new BoolOption("On-Shield Adv OSD", player.showOnShieldAdvantage));
        newPage.addOption(new BoolOption("On-Hit Adv OSD", player.showOnHitAdvantage));
        newPage.addOption(new BoolOption("Ledgedash Visualization", player.enableLedgeTechFrameDisplay));
        newPage.addOption(new BoolOption("GALINT OSD", player.enableLedgeTechGalintPopup));
        newPage.addOption(new BoolOption("Frames-on-ledge OSD", player.enableLedgeTechFramesOnLedgePopup));
        newPage.addOption(new BoolOption("Ledgedash Angle OSD", player.enableLedgeTechAirdodgeAngle));
        newPage.addOption(new BoolOption("Wait Overlay", player.enableWaitOverlay));
        newPage.addOption(new BoolOption("Dash Overlay", player.enableDashOverlay));
        newPage.addOption(new BoolOption("IASA Overlay", player.enableIasaOverlay));
        newPage.addOption(new BoolOption("Show Fighter State", player.showFighterState));
        newPage.addOption(new SpacerOption());

        /*
        SubpageOption& playerInfoSubpage = *(new SubpageOption("Fighter Info", true));
        newPage.addOption(&playerInfoSubpage);

        playerInfoSubpage.addOption(new PlayerDataShortHexObserver("Action ID", &player, &PlayerData::action));
        playerInfoSubpage.addOption(new PlayerDataStrObserver("Action", &player, &PlayerData::actionStr));
        playerInfoSubpage.addOption(new PlayerDataShortHexObserver("Subaction ID", &player, &PlayerData::subaction));
        playerInfoSubpage.addOption(new PlayerDataStrObserver("Subaction Name", &player, &PlayerData::subactionStr));
        playerInfoSubpage.addOption(new PlayerDataFlagObserver("RA Bits", &player, &PlayerData::raLowBits));
        */

        addPage(&newPage);

    }

    #ifdef PP_MENU_DISPLAY_DEBUG
    // TODO: Reimplement scrolling. This is w/e for now.
    Page& displayOptsPage = *(new Page(this));
    snprintf(displayOptsPage.title, 256, "Display Options");
    displayOptsPage.addOption(new IntOption<u8>("Opacity", opacity, 0, 255, true, true));
    displayOptsPage.addOption(new FloatOption("Title Font Scaling", titleFontScaleMultiplier, 0.0F, 10.0F, 0.01F));
    displayOptsPage.addOption(new FloatOption("Font Scaling", fontScaleMultiplier, 0.0F, 10.0F, 0.01F));
    displayOptsPage.addOption(new IntOption<int>("Line Height", lineHeightMultiplier, 0, 50, true, false));
    displayOptsPage.addOption(new FloatOption("Font base X", baseFontScale.x, 0.0F, 10.0F, 0.01F));
    displayOptsPage.addOption(new FloatOption("Font base Y", baseFontScale.y, 0.0F, 10.0F, 0.01F));

    SubpageOption& menuSizeSubPage = *(new SubpageOption("Menu Position", true));
    menuSizeSubPage.addOption(new IntOption<int>("Menu X", pos.x));
    menuSizeSubPage.addOption(new IntOption<int>("Menu Y", pos.y));
    menuSizeSubPage.addOption(new IntOption<int>("Menu W", size.x));
    menuSizeSubPage.addOption(new IntOption<int>("Menu H", size.y));
    displayOptsPage.addOption(&menuSizeSubPage);

    SubpageOption& bgColorSubP = *(new SubpageOption("Menu BG", true));
    bgColorSubP.addOption(new IntOption<u8>("R", bgColor.r, 0, 255, true, true));
    bgColorSubP.addOption(new IntOption<u8>("G", bgColor.b, 0, 255, true, true));
    bgColorSubP.addOption(new IntOption<u8>("B", bgColor.g, 0, 255, true, true));
    bgColorSubP.addOption(new IntOption<u8>("A", bgColor.a, 0, 255, true, true));
    displayOptsPage.addOption(&bgColorSubP);

    SubpageOption& outlineColorSubP = *(new SubpageOption("Menu Outline", true));
    outlineColorSubP.addOption(new IntOption<u8>("R", outlineColor.r, 0, 255, true, true));
    outlineColorSubP.addOption(new IntOption<u8>("G", outlineColor.b, 0, 255, true, true));
    outlineColorSubP.addOption(new IntOption<u8>("B", outlineColor.g, 0, 255, true, true));
    outlineColorSubP.addOption(new IntOption<u8>("A", outlineColor.a, 0, 255, true, true));
    displayOptsPage.addOption(&outlineColorSubP);

    SubpageOption& highlightBoxSubP = *(new SubpageOption("Highlight Box", true));
    highlightBoxSubP.addOption(new IntOption<u8>("R", highlightBoxColor.r, 0, 255, true, true));
    highlightBoxSubP.addOption(new IntOption<u8>("G", highlightBoxColor.b, 0, 255, true, true));
    highlightBoxSubP.addOption(new IntOption<u8>("B", highlightBoxColor.g, 0, 255, true, true));
    highlightBoxSubP.addOption(new IntOption<u8>("A", highlightBoxColor.a, 0, 255, true, true));
    displayOptsPage.addOption(&highlightBoxSubP);
    addPage(&displayOptsPage);
    #endif

    /* If it's a VS Mode, only show if L/R are held. In training mode, 
     * auto-open the menu UNLESS L/R are held. 
     */
    gfPadStatus pad;
    g_gfPadSystem.getSysPadStatus(GF_PAD_SYSTEM_GET_ALL_PADS, &pad);
    DEBUG_INIT("Pad buttons on menu init: 0x%08x\n", pad.m_buttonsCurrentFrame.bits);
    if ((getScene() == VS) == (pad.m_buttonsCurrentFrame.m_l == true || pad.m_buttonsCurrentFrame.m_r == true)) {
        punchMenu.toggle();
    }

    currentPageIdx = 0;
    initialized = true;
}

void PpunchMenu::cleanup() {
    currentPageIdx = 0;
    for (int i = 0; i < pages.size(); i++) {
        delete reinterpret_cast<Page*>(pages[i]);
    }
    pages.clear();
    initialized = false;
    paused = false;
    visible = false;
}

#define ANALOG_DEADZONE 30
void PpunchMenu::handleInput() {
    PpunchMenu& menu = *this;

    u16 mask = 0b0001111101111111; // These button fields are unknown.

    PadButtons buttons = 0;
    s8 stickX = 0;
    s8 stickY = 0;
    for (int i = 0; i < 4; i++) {
        if (stickX == 0 && g_padStatus[i].stickX > ANALOG_DEADZONE || g_padStatus[i].stickX < -ANALOG_DEADZONE) {
            stickX = g_padStatus[i].stickX;
        }

        if (stickY == 0 && g_padStatus[i].stickY > ANALOG_DEADZONE || g_padStatus[i].stickY < -ANALOG_DEADZONE) {
            stickY = g_padStatus[i].stickY;
        }

        buttons.bits |= g_padStatus[i].btns.bits;
    }
    buttons.bits &= mask;



    // The following additions to the mask are things that we want
    // to not trigger if they are held down. 
    // This masks out the Y button bit... this is fine because 
    // the Y button is just a hold modifier so we don't actually
    // want it trigger our debounce. Same with held L and R.
    mask &= 0b1111011111111111;
    if (LLastFrame) {
        mask &= 0b1111111110111111;
    }
    if (RLastFrame) {
        mask &= 0b1111111111011111;
    }


    do {
        if ((buttons.bits & mask) == 0 && stickX == 0 && stickY == 0) {
            #ifdef PP_MENU_INPUT_DEBUG
            OSReport("Bailed out by bitmask: %d\n", btn.bits);
            #endif
            break; // shortcut
        }

        if (!buttons.X && ((menuFrameCounter - lastInputFrame) < PP_MENU_INPUT_SPEED)) {
            #ifdef PP_MENU_INPUT_DEBUG
            OSReport("Bailed out by debounce: %d\n", btn.bits);
            #endif
            break; // debounce inputs.
        }

        lastInputFrame = menuFrameCounter;

        if (buttons.L && buttons.R && buttons.UpDPad) {
            menu.toggle();
            break;
        }

        if (!menu.isActive()) {
            #ifdef PP_MENU_INPUT_DEBUG
            OSReport("Bailed out by !isActive: %d\n", buttons.bits);
            #endif
            break; // reduce nesting.
        }

        if (buttons.A) { menu.select();   break; }
        if (buttons.B) { menu.deselect(); break; }
        if (buttons.Start) { menu.toggle(); break; }

        if      (buttons.UpDPad    || stickY > 0) menu.up();
        else if (buttons.DownDPad  || stickY < 0) menu.down();
        else if (buttons.RightDPad || stickX > 0) menu.modify(buttons.Y ? 5 : 1);
        else if (buttons.LeftDPad  || stickX < 0) menu.modify(buttons.Y ? -5 : -1);
        else if (buttons.L && !LLastFrame)  menu.prevPage();
        else if (buttons.R && !RLastFrame)  menu.nextPage();
        //}
    } while (false);

    LLastFrame = buttons.L == true;
    RLastFrame = buttons.R == true;
    return;
}

float PpunchMenu::lineHeight() {
    return this->baseFontScale.y * fontScaleMultiplier * this->lineHeightMultiplier;
}


// #define MELEE_STD_FONT (FontData*) 0x80497d2c; // crash
// #define MELEE_HIRANGA_FONT (FontData*) 0x80497d54; // crash
#define END_FONT (void*) 0x80497da4; // Works, Blocky, no punctuation
// Works: . / ? " ~ ! (@=pencil) % & + - = ' : ; , 
// Doesnt: () # $ ^ * _ ` [] {} \ <> |
// #define MELEE_MONO_FONT (FontData*) 0x80497dcc; // crash (needs monospace maybe? (no, doesn't work))
// resant font (I use)
// #define FOX_FONT (FontData*) 0x80497df4; // crash
// #define ALERT_FONT (FontData*) 0x80497e1c; // crash
#define USA_MAIN_MENU_FONT (void*) 0x80497e6c; // Works, no punctuation
// #define UNKNOWN_FONT (FontData*) 0x80497d7c; // crash
void PpunchMenu::render(TextPrinter& printer, char* buffer, u32 maxLen) {
    if (!visible) { return; }
    if (pages.size() == 0) {
        OSReport("Tried to render a menu with no pages.\n");
        return;
    }

    /* Setup */
    printer.renderPre = true;
    ms::CharWriter& charWriter = *printer.charWriter;

    /* Print title */
    Page& currentPage = *getCurrentPage();
    snprintf(buffer, maxLen, "Page %d / %d: %s", currentPageIdx+1, pages.size(), currentPage.getTitle());
    printer.charWriter->m_font = END_FONT;
    printer.opacity = opacity;
    printer.setTextColor(defaultColor);
    printer.setTextBorder(0x000000FF, 1);
    printer.setScale(titleBaseFontScale, titleFontScaleMultiplier, lineHeightMultiplier);
    printer.setPosition(pos.x + padding, pos.y + padding);
    printer.setMinWidth(20);
    printer.begin();
    printer.printLine(buffer);
    
    /* Print body */
    charWriter.m_font = MELEE_FONT;
    printer.setScale(baseFontScale, fontScaleMultiplier, lineHeightMultiplier);

    charWriter.SetCursorY(charWriter.GetCursorY() + padding);
    currentPage.render(&printer, buffer);

    /* Draw graphics */
    drawBg(printer);
    drawHighlightBox();
    drawOutline(printer);
}

void PpunchMenu::drawBg(TextPrinter& printer) {
    ms::CharWriter& charWriter = *printer.charWriter;
    // void Te    
    renderables.items.preFrame.push(static_cast<Drawable*>(new Graphics::Rect(
        0,
        1,
        bgColor.withAlpha(opacity).gxColor,
        (float)pos.y,
        //message.yPos + printer.lineHeight + padding,
        (float)pos.y + size.y,
        (float)pos.x,
        (float)(pos.x + size.x),
        true
    )));
}

void PpunchMenu::drawOutline(TextPrinter& printer) {
    ms::CharWriter& message = *printer.charWriter;

    renderables.items.preFrame.push(static_cast<Drawable*>(new RectOutline(
            0,
            1,
            outlineColor.withAlpha(opacity).gxColor,
            (float)(pos.y),
//            message.yPos + printer.lineHeight + padding + 1,
            (float)pos.y + size.y,
            (float)(pos.x),
            (float)(pos.x + size.x),
            outlineWidth * 6,
            true
    )));

    renderables.items.preFrame.push(static_cast<Drawable*>(new RectOutline(
            0,
            1,
            ((Color)0xFFFFFFFF).withAlpha(opacity).gxColor,
            (float)(pos.y),
//            message.yPos + printer.lineHeight + padding + 1,
            (float)pos.y + size.y,
            (float)(pos.x),
            (float)(pos.x + size.x),
            12,
            true
    )));
}

void PpunchMenu::drawHighlightBox() {
    Page& currentPage = *(reinterpret_cast<Page*>(pages[currentPageIdx]));
    if (!(currentPage.highlightedOptionBottom == 0 || currentPage.highlightedOptionTop == 0)) { 
        // OSReport("Drawing highlight top: %0.01f bot: %0.01f\n", currentPage.highlightedOptionTop, currentPage.highlightedOptionBottom);

        renderables.items.preFrame.push(static_cast<Drawable*>(new Rect (
            0,
            1,
            highlightBoxColor.withAlpha(opacity).gxColor,
            currentPage.highlightedOptionTop,
            currentPage.highlightedOptionBottom,
            (float)pos.x,
            (float)(pos.x + size.x),
            true
        )));

        currentPage.highlightedOptionBottom = 0;
        currentPage.highlightedOptionTop = 0;
    }
}

#define P1_2P_WIDESCREEN_COORDS Coord2D(200, 350)
#define P2_2P_WIDESCREEN_COORDS Coord2D(355, 350)
#define P1_2P_COORDS Coord2D(180, 350)
#define P2_2P_COORDS Coord2D(390, 350)
#define P1_4P_COORDS Coord2D(50, 350)
#define P2_4P_COORDS Coord2D(200, 350)
#define P3_4P_COORDS Coord2D(350, 350)
#define P4_4P_COORDS Coord2D(500, 350)
Coord2D getHpPopupBoxCoords(int playerNum) {
    // playerNum is 1-indexed like the variant in the playerData struct.

    SCENE_TYPE scene = getScene();
    char totalPlayers;

    // I don't know why training mode has the player numbers
    // backwards. :(
    if (scene == TRAINING_MODE_MMS) {
        totalPlayers = 4;
        switch(playerNum){
        case 0: return P1_4P_COORDS;
        case 1: return P2_4P_COORDS;
        case 2: return P3_4P_COORDS;
        case 3: return P4_4P_COORDS;
        default: return Coord2D(0, 0);
        }
    }

    totalPlayers = g_ftManager->getEntryCount();

    if (totalPlayers == 2) {
        if (g_GameGlobal->m_record->m_menuData.m_isWidescreen) {
            switch(playerNum) {
                case 0: return P1_2P_WIDESCREEN_COORDS;
                case 1: return P2_2P_WIDESCREEN_COORDS;
                default: return Coord2D(0, 0);
            }
        } else {
            switch(playerNum) {
                case 0: return P1_2P_COORDS;
                case 1: return P2_2P_COORDS;
                default: return Coord2D(0, 0);
            }
        }
    }
    if (totalPlayers == 4) {
        switch(playerNum) {
            case 0: return P1_4P_COORDS;
            case 1: return P2_4P_COORDS;
            case 2: return P3_4P_COORDS;
            case 3: return P4_4P_COORDS;
            default: return Coord2D(0, 0);
        }
    }

    // TODO: Other numbers of players.

    return Coord2D(0, 0);
}


void drawAllPopups() {
    for(int i = 0; i < PP_MAX_PLAYERS; i++) {
        const PlayerData& playerData = allPlayerData[i];

        if (playerPopups[i].length > 0 ) {
            LinkedlistIterator<Popup> itr = LinkedlistIterator<Popup>(playerPopups[i]);
            Popup* popup;
            Coord2D coords = getHpPopupBoxCoords(allPlayerData[i].playerNumber);

            while ((popup = itr.next()) != NULL) {
                if (popup->expired()) {
                    itr.deleteHere();
                    delete popup;
                } else {
                    popup->coords = coords;
                    // OSReport("Set popup coords to %d,%d\n", coords.x, coords.y);
                    popup->draw(printer);

                    coords.y -= PP_POPUP_VERTICAL_OFFSET;
                }
            }
        }
    }
}

void addPopup(int playerNum, Popup& popup) {
    playerPopups[playerNum].append(popup);
}

} // namespace