#include <ft/ft_manager.h>

#include "pp/input/pad.h"
#include "pp/ui.h"
#include "pp/common.h"
#include "pp/collections/linkedlist.h"
#include "pp/popup.h"
#include "pp/menu.h"
#include "pp/playerdata.h"
#include "pp/fighter_names.h"

namespace ProjectPunch {

using namespace Input;

extern u32 frameCounter;

/* EXTERN DEFS */
PpunchMenu& punchMenu = *(new PpunchMenu());
linkedlist<Popup> playerPopups[PP_MAX_PLAYERS] = {
    linkedlist<Popup>(),
    linkedlist<Popup>(),
    linkedlist<Popup>(),
    linkedlist<Popup>()
};
/************/

void PpunchMenu::init() {
    /* Controls Info Page */
    OSReport("Initializing Project Punch menu.\n");
    Page& ctrlInfoPage = *(new Page(this));
    snprintf(ctrlInfoPage.title, 256, "Menu Controls");
    ctrlInfoPage.addOption(new LabelOption("L+R+DpadUp", "Open and close the menu."));
    ctrlInfoPage.addOption(new LabelOption("L/R", "Prev/Next Page"));
    ctrlInfoPage.addOption(new LabelOption("A/B",  "Select / Cancel Selection"));
    ctrlInfoPage.addOption(new LabelOption("DPad U/D", "Select options"));
    ctrlInfoPage.addOption(new LabelOption("DPad L/R", "Adjust selected value"));
    ctrlInfoPage.addOption(new LabelOption("Start", "Resume gameplay with menu open"));
    ctrlInfoPage.addOption(new LabelOption("X (hold)", "No input delay (turbo)"));
    ctrlInfoPage.addOption(new LabelOption("Y (hold)", "Adjust values by 5x the default amount"));
    ctrlInfoPage.addOption(new LabelOption("L/R (hold on startup)", "Don't open this menu automatically"));
    addPage(&ctrlInfoPage);

    u32 fighters = g_ftManager->getEntryCount();
    OSReport("Detected %d fighters.\n", fighters);
    for (u32 i = 0; i < fighters; i++) {
        Page& newPage = *(new Page(this));
        PlayerData& player = allPlayerData[i];
        const char* ftName = fighterName(player.charId);
        OSReport("Adding page for P%d: %s @ 0x%x\n", player.playerNumber, ftName, (void*)&player);
        snprintf(newPage.title, 256, "P%d = %s", i+1, fighterName(player.charId));
        newPage.addOption(new BoolOption("Popup: On-Shield Advantage", player.showOnShieldAdvantage));
        newPage.addOption(new BoolOption("Popup: On-Hit Advantage", player.showOnHitAdvantage));
        newPage.addOption(new BoolOption("Show Fighter State", player.showFighterState));
        newPage.addOption(new SpacerOption());

        SubpageOption& playerInfoSubpage = *(new SubpageOption("Fighter Info", true));
        playerInfoSubpage.addOption(new PlayerDataShortHexObserver("Action ID", &player, &PlayerData::action));
        playerInfoSubpage.addOption(new PlayerDataStrObserver("Action", &player, &PlayerData::actionStr));
        playerInfoSubpage.addOption(new PlayerDataShortHexObserver("Subaction ID", &player, &PlayerData::subaction));
        playerInfoSubpage.addOption(new PlayerDataStrObserver("Subaction Name", &player, &PlayerData::subactionStr));
        playerInfoSubpage.addOption(new PlayerDataFlagObserver("RA Bits", &player, &PlayerData::raLowBits));
        newPage.addOption(&playerInfoSubpage);
        addPage(&newPage);

    }

    // TODO: Reimplement scrolling. This is w/e for now.
    Page& displayOptsPage = *(new Page(this));
    snprintf(displayOptsPage.title, 256, "Display Options");
    displayOptsPage.addOption(new IntOption<u8>("Opacity", opacity, 65, 255, true, true));
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


    currentPageIdx = 0;
    initialized = true;
}

void PpunchMenu::cleanup() {
    currentPageIdx = 0;
    pages.clear();
    initialized = false;
    paused = false;
    visible = false;
}

void PpunchMenu::handleInput() {
    PpunchMenu& menu = *this;

    u16 mask = 0b0001111101111111; // These button fields are unknown.
    PadButtons buttons = (
        g_padStatus[0].btns.bits
        | g_padStatus[1].btns.bits 
        | g_padStatus[2].btns.bits 
        | g_padStatus[3].btns.bits
    ) & mask; 

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

        if ((buttons.bits & mask) == 0) {
#ifdef PP_MENU_INPUT_DEBUG
            OSReport("Bailed out by bitmask: %d\n", btn.bits);
#endif
            break; // shortcut
        }

        if (!buttons.X && ((frameCounter - lastInputFrame) < PP_MENU_INPUT_SPEED)) {
#ifdef PP_MENU_INPUT_DEBUG
            OSReport("Bailed out by debounce: %d\n", btn.bits);
#endif
            break; // debounce inputs.
        }

        lastInputFrame = frameCounter;


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

        if (buttons.UpDPad)    menu.up();
        else if (buttons.DownDPad)  menu.down();
        else if (buttons.RightDPad) menu.modify(buttons.Y ? 5 : 1);
        else if (buttons.LeftDPad)  menu.modify(buttons.Y ? -5 : -1);
        else if (buttons.L && !LLastFrame)  menu.prevPage();
        else if (buttons.R && !RLastFrame)  menu.nextPage();
        else if (buttons.Start) menu.unpause();
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
    printer.setup();
    printer.renderPre = true;
    ms::CharWriter& charWriter = *printer.charWriter;
    charWriter.SetCursor(pos.x + padding, pos.y + padding, 0);
    printer.startBoundingBox();

    /* Print title */
    Page& currentPage = *getCurrentPage();
    snprintf(buffer, maxLen, "Page %d / %d: %s", currentPageIdx+1, pages.size(), currentPage.getTitle());
    charWriter.SetScale(
        titleBaseFontScale.x * titleFontScaleMultiplier,
        titleBaseFontScale.y * titleFontScaleMultiplier
    );

    printer.lineHeight = this->titleBaseFontScale.y * titleFontScaleMultiplier * this->lineHeightMultiplier;
    printer.charWriter->m_edgeWidth = 1;
    printer.charWriter->m_edgeColor = Color(0x000000FF).utColor;
    printer.charWriter->m_font = END_FONT;
    printer.setTextColor(applyAlpha(defaultColor, opacity));
    printer.printLine(buffer);
    
    /* Print body */
    charWriter.m_font = MELEE_FONT;
    charWriter.SetScale(
        baseFontScale.x * fontScaleMultiplier,
        baseFontScale.y * fontScaleMultiplier
    );
    printer.lineHeight = lineHeight();
    charWriter.m_yPos += padding;

    currentPage.render(&printer, buffer);

    /* Draw graphics */
    drawBg(printer);
    drawHighlightBox();
    drawOutline(printer);

    /* Reset */
    printer.setup();
}

void PpunchMenu::drawBg(TextPrinter& printer) {
    ms::CharWriter& charWriter = *printer.charWriter;
    // void Te    
    renderables.items.preFrame.push(new Graphics::Rect(
        0,
        1,
        applyAlpha(bgColor, opacity).gxColor,
        (float)pos.y,
        //message.yPos + printer.lineHeight + padding,
        (float)pos.y + size.y,
        (float)pos.x,
        (float)(pos.x + size.x),
        true
    ));
}

void PpunchMenu::drawOutline(TextPrinter& printer) {
    ms::CharWriter& message = *printer.charWriter;

    renderables.items.preFrame.push(new RectOutline(
            0,
            1,
            applyAlpha(outlineColor, opacity).gxColor,
            (float)(pos.y),
//            message.yPos + printer.lineHeight + padding + 1,
            (float)pos.y + size.y,
            (float)(pos.x),
            (float)(pos.x + size.x),
            outlineWidth * 6,
            true
    ));

    renderables.items.preFrame.push(new RectOutline(
            0,
            1,
            applyAlpha(0xFFFFFFFF, opacity).gxColor,
            (float)(pos.y),
//            message.yPos + printer.lineHeight + padding + 1,
            (float)pos.y + size.y,
            (float)(pos.x),
            (float)(pos.x + size.x),
            12,
            true
    ));
}

void PpunchMenu::drawHighlightBox() {
    Page& currentPage = *pages[currentPageIdx];
    if (!(currentPage.highlightedOptionBottom == 0 || currentPage.highlightedOptionTop == 0)) { 
        // OSReport("Drawing highlight top: %0.01f bot: %0.01f\n", currentPage.highlightedOptionTop, currentPage.highlightedOptionBottom);

        renderables.items.preFrame.push(new Rect (
            0,
            1,
            applyAlpha(highlightBoxColor, opacity).gxColor,
            currentPage.highlightedOptionTop,
            currentPage.highlightedOptionBottom,
            (float)pos.x,
            (float)(pos.x + size.x),
            true
        ));

        currentPage.highlightedOptionBottom = 0;
        currentPage.highlightedOptionTop = 0;
    }
}

} // namespace