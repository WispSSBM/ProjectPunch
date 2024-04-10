#ifndef PP_UI
#define PP_UI

#include "menu.h"
#include "common.h"
#include "linkedlist.h"
#include "popup.h"
#include "playerdata.h"

#include <Wii/PAD/PADButtons.h>
#include <Wii/PAD/PADStatus.h>

#define PP_MENU_INPUT_SPEED 10
class PpunchMenu : public Menu {
    public:
        PpunchMenu() {
            pos = {100, 25};
            size = {440, 350};

            padding = 25;
            outlineWidth = 4;
            lineHeightMultiplier = 25;

            baseFontScale = {0.5, 0.7};
            titleBaseFontScale = {0.5, 1.0};
            fontScaleMultiplier = 1.15;
            titleFontScaleMultiplier = 1.0;

            bgColor = 0x303030DD;  // dark grey
            highlightedColor = 0xEEE8AAFF; //ylw
            selectedColor = 0x75A5E2FF; // blu
            defaultColor = 0xFFFFFFFF; 
            outlineColor = 0x505050FF; // light grey
            highlightBoxColor = 0x000000FF;
        };

        Coord2D pos;
        Coord2D size;
        Coord2DF baseFontScale;
        Coord2DF titleBaseFontScale;

        float fontScaleMultiplier;
        float titleFontScaleMultiplier;
        int lineHeightMultiplier;
        u8 outlineWidth;

        bool fixedHeight = true;
        u32 initialized: 1 = false;

        void init();
        void cleanup();
        void handleInput();
        void render(TextPrinter& printer, char* buffer, u32 maxLen);
        void drawBg(TextPrinter& printer);
        void drawOutline(TextPrinter& printer);
        void drawHighlightBox();
        float lineHeight();

        inline bool isActive() { return this->visible && this->paused; };
    private:
        u32 lastInputFrame;
        bool LLastFrame = false;
        bool RLastFrame = false;
};

#pragma region observers
#define BO_METH_PTR(T, V) T const* instance, V (T::*getter) () const
template <typename T, typename V>
class BaseObserver: public StandardOption {
    public:
        BaseObserver(const char* name, BO_METH_PTR(T, V)) : instance(instance), getter(getter){
            this->name = name;
            canModify = false;
        };
        virtual ~BaseObserver() {};
        virtual void modify(float amount) {};
        virtual void select() {};
        virtual void deselect() {};
        virtual void render(TextPrinter* printer, char* buffer) {};
    protected:
        V (T::*getter)() const;
        T const* instance;

};

class PlayerDataFlagObserver : public BaseObserver<PlayerData, u32> {
public:
    PlayerDataFlagObserver(const char* name, BO_METH_PTR(PlayerData, u32)) : BaseObserver(name, instance, getter) {};
    virtual void render(TextPrinter* printer, char* buffer) {
        auto byte = (*(this->instance).*getter)();
        char binBuf[50];
        char* bufPos = binBuf;
        char val;

        for (char i = 0; i < 32; i++) {
            val = (byte >> i) & 0x1;
            *bufPos++ = (val == 0 ? '0' : '1');
            if (i % 8 == 7) {
                *bufPos++ = ' ';
            }
        }
        *bufPos = '\0'; 

        sprintf(buffer, "%s: %s", name, binBuf);
        printer->printLine(buffer);
    };
};

class PlayerDataHexObserver : public BaseObserver<PlayerData, u32> {
public:
    PlayerDataHexObserver(const char* name, BO_METH_PTR(PlayerData, u32)) : BaseObserver(name, instance, getter) {};
    virtual void render(TextPrinter* printer, char* buffer) {
        auto value = (*(this->instance).*getter)();
        sprintf(buffer, "%s: 0x%08x", name, value);
        printer->printLine(buffer);
    };
};

class PlayerDataShortHexObserver : public BaseObserver<PlayerData, u16> {
public:
    PlayerDataShortHexObserver(const char* name, BO_METH_PTR(PlayerData, u16)) : BaseObserver(name, instance, getter) {};
    virtual void render(TextPrinter* printer, char* buffer) {
        auto value = (*(this->instance).*getter)();
        sprintf(buffer, "%s: 0x%04x", name, value);
        printer->printLine(buffer);
    };
};

class PlayerDataByteHexObserver : public BaseObserver<PlayerData, char> {
public:
    PlayerDataByteHexObserver(const char* name, BO_METH_PTR(PlayerData, char)) : BaseObserver(name, instance, getter) {};
    virtual void render(TextPrinter* printer, char* buffer) {
        auto value = (*(this->instance).*getter)();
        sprintf(buffer, "%s: 0x%02x", name, value);
        printer->printLine(buffer);
    };
};

class PlayerDataStrObserver: public BaseObserver<PlayerData, const char*> {
public:
    PlayerDataStrObserver(const char* name, BO_METH_PTR(PlayerData, const char*)) : BaseObserver(name, instance, getter) {};
    virtual void render(TextPrinter* printer, char* buffer) {
        auto value = (*(this->instance).*getter)();
        sprintf(buffer, "%s: %s", name, value);
        printer->printLine(buffer);
    };
};
#pragma endregion

extern linkedlist<Popup> playerPopups[PP_MAX_PLAYERS];
extern PpunchMenu& punchMenu;

#endif