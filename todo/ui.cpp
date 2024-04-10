#include "./ui.h"

#include "./common.h"
#include "./linkedlist.h"
#include "./popup.h"
#include "./menu.h"
#include "./playerdata.h"

#include <Brawl/FT/ftManager.h>
#include "fighterNames.h"

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
    auto& ctrlInfoPage = *(new Page(this));
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

    auto fighters = FIGHTER_MANAGER->getEntryCount();
    OSReport("Detected %d fighters.\n", fighters);
    for (u32 i = 0; i < fighters; i++) {
        auto& newPage = *(new Page(this));
        auto& player = allPlayerData[i];
        auto ftName = fighterName(player.charId);
        OSReport("Adding page for P%d: %s @ 0x%x\n", player.playerNumber, ftName, (void*)&player);
        snprintf(newPage.title, 256, "P%d = %s", i+1, fighterName(player.charId));
        newPage.addOption(new BoolOption("Popup: On-Shield Advantage", player.showOnShieldAdvantage));
        newPage.addOption(new BoolOption("Popup: On-Hit Advantage", player.showOnHitAdvantage));
        newPage.addOption(new BoolOption("Show Fighter State", player.showFighterState));
        newPage.addOption(new SpacerOption());

        auto& playerInfoSubpage = *(new SubpageOption("Fighter Info", true));
        playerInfoSubpage.addOption(new PlayerDataShortHexObserver("Action ID", &player, &PlayerData::action));
        playerInfoSubpage.addOption(new PlayerDataStrObserver("Action", &player, &PlayerData::actionStr));
        playerInfoSubpage.addOption(new PlayerDataShortHexObserver("Subaction ID", &player, &PlayerData::subaction));
        playerInfoSubpage.addOption(new PlayerDataStrObserver("Subaction Name", &player, &PlayerData::subactionStr));
        playerInfoSubpage.addOption(new PlayerDataFlagObserver("RA Bits", &player, &PlayerData::raLowBits));
        newPage.addOption(&playerInfoSubpage);
        addPage(&newPage);

    }

    // TODO: Reimplement scrolling. This is w/e for now.
    auto& displayOptsPage = *(new Page(this));
    snprintf(displayOptsPage.title, 256, "Display Options");
    displayOptsPage.addOption(new IntOption<u8>("Opacity", opacity, 65, 255, true, true));
    displayOptsPage.addOption(new FloatOption("Title Font Scaling", titleFontScaleMultiplier, 0.0F, 10.0F, 0.01F));
    displayOptsPage.addOption(new FloatOption("Font Scaling", fontScaleMultiplier, 0.0F, 10.0F, 0.01F));
    displayOptsPage.addOption(new IntOption<int>("Line Height", lineHeightMultiplier, 0, 50, true, false));
    displayOptsPage.addOption(new FloatOption("Font base X", baseFontScale.x, 0.0F, 10.0F, 0.01F));
    displayOptsPage.addOption(new FloatOption("Font base Y", baseFontScale.y, 0.0F, 10.0F, 0.01F));

    auto& menuSizeSubPage = *(new SubpageOption("Menu Position", true));
    menuSizeSubPage.addOption(new IntOption<int>("Menu X", pos.x));
    menuSizeSubPage.addOption(new IntOption<int>("Menu Y", pos.y));
    menuSizeSubPage.addOption(new IntOption<int>("Menu W", size.x));
    menuSizeSubPage.addOption(new IntOption<int>("Menu H", size.y));
    displayOptsPage.addOption(&menuSizeSubPage);

    auto& bgColorSubP = *(new SubpageOption("Menu BG", true));
    bgColorSubP.addOption(new IntOption<u8>("R", bgColor.red, 0, 255, true, true));
    bgColorSubP.addOption(new IntOption<u8>("G", bgColor.blue, 0, 255, true, true));
    bgColorSubP.addOption(new IntOption<u8>("B", bgColor.green, 0, 255, true, true));
    bgColorSubP.addOption(new IntOption<u8>("A", bgColor.alpha, 0, 255, true, true));
    displayOptsPage.addOption(&bgColorSubP);

    auto& outlineColorSubP = *(new SubpageOption("Menu Outline", true));
    outlineColorSubP.addOption(new IntOption<u8>("R", outlineColor.red, 0, 255, true, true));
    outlineColorSubP.addOption(new IntOption<u8>("G", outlineColor.blue, 0, 255, true, true));
    outlineColorSubP.addOption(new IntOption<u8>("B", outlineColor.green, 0, 255, true, true));
    outlineColorSubP.addOption(new IntOption<u8>("A", outlineColor.alpha, 0, 255, true, true));
    displayOptsPage.addOption(&outlineColorSubP);

    auto& highlightBoxSubP = *(new SubpageOption("Highlight Box", true));
    highlightBoxSubP.addOption(new IntOption<u8>("R", highlightBoxColor.red, 0, 255, true, true));
    highlightBoxSubP.addOption(new IntOption<u8>("G", highlightBoxColor.blue, 0, 255, true, true));
    highlightBoxSubP.addOption(new IntOption<u8>("B", highlightBoxColor.green, 0, 255, true, true));
    highlightBoxSubP.addOption(new IntOption<u8>("A", highlightBoxColor.alpha, 0, 255, true, true));
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
    auto& menu = *this;
    PADButtons btn;

    u16 mask = 0b0001111101111111; // These button fields are unknown.
    btn.bits = (
        PREVIOUS_PADS[0].button.bits 
        | PREVIOUS_PADS[1].button.bits 
        | PREVIOUS_PADS[2].button.bits 
        | PREVIOUS_PADS[3].button.bits
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
    if ((btn.bits & mask) == 0) {
        #ifdef PP_MENU_INPUT_DEBUG
        OSReport("Bailed out by bitmask: %d\n", btn.bits);
        #endif
        goto end; // shortcut
    }

    if (!btn.X && ((frameCounter - lastInputFrame) < PP_MENU_INPUT_SPEED)) {
        #ifdef PP_MENU_INPUT_DEBUG
        OSReport("Bailed out by debounce: %d\n", btn.bits);
        #endif
        goto end; // debounce inputs.
    } 

    lastInputFrame = frameCounter;


    if (btn.L && btn.R && btn.UpDPad) {
        menu.toggle();
        goto end;
    }

    if (!menu.isActive()) {
        #ifdef PP_MENU_INPUT_DEBUG
        OSReport("Bailed out by !isActive: %d\n", btn.bits);
        #endif
        goto end; // reduce nesting.
    }

    if (btn.A) { menu.select();   goto end; }
    if (btn.B) { menu.deselect(); goto end; }

    if (btn.UpDPad)    menu.up();
    else if (btn.DownDPad)  menu.down();
    else if (btn.RightDPad) menu.modify(btn.Y ? 5 : 1);
    else if (btn.LeftDPad)  menu.modify(btn.Y ? -5 : -1);
    else if (btn.L && !LLastFrame)  menu.prevPage();
    else if (btn.R && !RLastFrame)  menu.nextPage();
    else if (btn.Start) menu.unpause();
    //}
    end:
        LLastFrame = btn.L == true;
        RLastFrame = btn.R == true;
        return;
}

float PpunchMenu::lineHeight() {
    return this->baseFontScale.y * fontScaleMultiplier * this->lineHeightMultiplier;
}


// #define MELEE_STD_FONT (FontData*) 0x80497d2c; // crash
// #define MELEE_HIRANGA_FONT (FontData*) 0x80497d54; // crash
#define END_FONT (FontData*) 0x80497da4; // Works, Blocky, no punctuation
// Works: . / ? " ~ ! (@=pencil) % & + - = ' : ; , 
// Doesnt: () # $ ^ * _ ` [] {} \ <> |
// #define MELEE_MONO_FONT (FontData*) 0x80497dcc; // crash (needs monospace maybe? (no, doesn't work))
// resant font (I use)
// #define FOX_FONT (FontData*) 0x80497df4; // crash
// #define ALERT_FONT (FontData*) 0x80497e1c; // crash
#define USA_MAIN_MENU_FONT (FontData*) 0x80497e6c; // Works, no punctuation
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
    Message& printerMsgObj = printer.message;
    printerMsgObj.xPos = pos.x + padding;
    printerMsgObj.yPos = pos.y + padding;
    printerMsgObj.zPos = 0;
    printer.start2D();

    printer.startBoundingBox();

    /* Print title */
    auto& currentPage = *getCurrentPage();
    snprintf(buffer, maxLen, "Page %d / %d: %s", currentPageIdx+1, pages.size(), currentPage.getTitle());
    printerMsgObj.fontScaleY = titleBaseFontScale.y * titleFontScaleMultiplier;
    printerMsgObj.fontScaleX = titleBaseFontScale.x * titleFontScaleMultiplier;
    printer.lineHeight = this->titleBaseFontScale.y * titleFontScaleMultiplier * this->lineHeightMultiplier;
    printer.message.edgeWidth = 1;
    printer.message.edgeColor = 0x000000FF;
    printer.message.font = END_FONT;
    printer.message.drawFlag.alignment = 2;
    printer.setTextColor(applyAlpha(defaultColor, opacity));
    printer.printLine(buffer);
    
    /* Print body */
    printer.message.font = MELEE_FONT;
    printerMsgObj.fontScaleY = baseFontScale.y * fontScaleMultiplier;
    printerMsgObj.fontScaleX = baseFontScale.x * fontScaleMultiplier;
    printer.lineHeight = lineHeight();
    printerMsgObj.yPos += padding;
    currentPage.render(&printer, buffer);

    /* Draw graphics */
    drawBg(printer);
    drawHighlightBox();
    drawOutline(printer);

    /* Reset */
    printer.setup();
}

void PpunchMenu::drawBg(TextPrinter& printer) {
    auto& message = printer.message;
    // void Te    
    renderables.items.preFrame.push(new Rect{
            0,
            1,
            applyAlpha(bgColor, opacity),
            (float)pos.y,
            //message.yPos + printer.lineHeight + padding,
            (float)pos.y + size.y,
            (float)pos.x,
            (float)(pos.x + size.x),
            true
    });
}

void PpunchMenu::drawOutline(TextPrinter& printer) {
    auto& message = printer.message;

    renderables.items.preFrame.push(new RectOutline{
            0,
            1,
            applyAlpha(outlineColor, opacity),
            (float)(pos.y),
//            message.yPos + printer.lineHeight + padding + 1,
            (float)pos.y + size.y,
            (float)(pos.x),
            (float)(pos.x + size.x),
            outlineWidth * 6,
            true
    });

    renderables.items.preFrame.push(new RectOutline{
            0,
            1,
            applyAlpha(0xFFFFFFFF, opacity),
            (float)(pos.y),
//            message.yPos + printer.lineHeight + padding + 1,
            (float)pos.y + size.y,
            (float)(pos.x),
            (float)(pos.x + size.x),
            12,
            true
    });
}

void PpunchMenu::drawHighlightBox() {
    auto& currentPage = *pages[currentPageIdx];
    if (!(currentPage.highlightedOptionBottom == 0 || currentPage.highlightedOptionTop == 0)) { 
        // OSReport("Drawing highlight top: %0.01f bot: %0.01f\n", currentPage.highlightedOptionTop, currentPage.highlightedOptionBottom);

        renderables.items.preFrame.push(new Rect {
            0,
            1,
            applyAlpha(highlightBoxColor, opacity),
            currentPage.highlightedOptionTop,
            currentPage.highlightedOptionBottom,
            (float)pos.x,
            (float)(pos.x + size.x),
            true
        });

        currentPage.highlightedOptionBottom = 0;
        currentPage.highlightedOptionTop = 0;
    }
}