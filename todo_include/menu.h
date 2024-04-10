// Wanton hosting of fudgepop's menu for my own purposes/modification

#ifndef PP_MENU_H
#define PP_MENU_H

#include "Containers/vector.h"
#include <Graphics/TextPrinter.h>
#include <CLibs/cstring.h>

#define RENDER_X_SPACING 80

// this is a completely arbitrary number I HIGHLY doubt anyone will need
// ever so it'll act as a default value
#define NUMERIC_DEFAULT -983746279

struct Menu;
struct Page;
struct SubpageOption;

struct OptionType {
    virtual void select() { isSelected = !isSelected; };
    virtual void deselect() { isSelected = false; };
    virtual void modify(float amount) { };
    virtual void render(TextPrinter* printer, char* buffer) = 0;
    virtual void down() = 0;
    virtual void up() = 0;
    virtual void setParentPage(Page* p) = 0;
    virtual bool isScalarOption() { return true; };
    virtual bool isFullySelected() { return isSelected; };
    void rawRender(TextPrinter* printer, const char* buffer);

    virtual ~OptionType() {};
    const char* name;
    Page* parent;
    SubpageOption* subParent = nullptr;
    bool isSelected = false;
    bool canModify = true;
    bool terminal = true;
};

struct StandardOption : public OptionType {
    virtual void up() { };
    virtual void down() { };
    virtual void setParentPage(Page* p) { this->parent = p; }
    virtual ~StandardOption() {}
};

struct Page {
    Page(Menu* myMenu) :
        menu(myMenu) {};
    void addOption(OptionType* option);
    void hide();
    void up();
    void down();
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);
    void saveHighlightRegion(TextPrinter* printer) {
        highlightedOptionTop = printer->message.yPos;
        highlightedOptionBottom = printer->message.yPos + printer->lineHeight;
    };
    virtual void show();
    virtual void select();
    virtual void deselect();
    virtual bool isFullySelected() { return options[currentOption]->isFullySelected(); };
    virtual const char* getTitle();
    virtual ~Page() {}

    vector<OptionType*> options;
    char currentOption = 0;
    bool isSelected = false;
    float highlightedOptionTop = 0.0;
    float highlightedOptionBottom = 0.0;
    Menu* menu;

    // TODO: Make this a title returning function.
    char title[256] = "generic page";
};

class Menu {
public:
    void nextPage();
    void prevPage();
    void addPage(Page* p);
    Page* getCurrentPage();
    void select();
    void deselect();
    void up();
    void down();
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);
    void unpause();
    void toggle();

    bool visible = false;
    bool paused = false;
    bool selected = false;
    vector<Page*> pages;

    u8 opacity = 0xFF;
    GXColor highlightedColor = 0xFFFFFFFF;
    GXColor selectedColor = 0x3333FFFF;
    GXColor readOnlyColor = 0xAAAAAAFF;
    GXColor defaultColor = 0xCCCCCCFF;
    GXColor bgColor = 0x555555CC;
    GXColor outlineColor = 0x000000FF;
    GXColor highlightBoxColor = 0x55FF55FF;

    float padding = 5;
protected:
    int currentPageIdx = -1;
    friend class Page;
};

template <typename T>
class IntOption : public StandardOption {
public:
    IntOption(const char* name, T& value) : value(value) {
        this->name = name;
        this->value = value;
    }
    IntOption(const char* name, T& value, T min, T max) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        hasBounds = true;
    }
    IntOption(const char* name, T& value, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->canModify = canModify;
    }
    IntOption(const char* name, T& value, T min, T max, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = canModify;
        hasBounds = true;
    }
    IntOption(const char* name, T& value, T min, T max, bool canModify, bool asHex) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = canModify;
        hasBounds = true;
        this->asHex = asHex;
    }
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

private:
    T& value;
    T max;
    T min;
    bool hasBounds = false;
    bool asHex = false;
};

class FloatOption : public StandardOption {
public:
    FloatOption(const char* name, float& value) : value(value) {
        this->name = name;
        this->value = value;
    }
    FloatOption(const char* name, float& value, float min, float max) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
    }
    FloatOption(const char* name, float& value, float changeMultiplier) : value(value) {
        this->name = name;
        this->value = value;
        this->changeMultiplier = changeMultiplier;
    }
    FloatOption(const char* name, float& value, float min, float max, float changeMultiplier) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->changeMultiplier = changeMultiplier;
    }
    FloatOption(const char* name, float& value, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->canModify = canModify;
    }
    FloatOption(const char* name, float& value, float min, float max, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = canModify;
    }
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

    ~FloatOption() {}

private:
    float& value;
    float max = NUMERIC_DEFAULT;
    float min = NUMERIC_DEFAULT;
    float changeMultiplier = 1;
};

class BoolOption : public StandardOption {
public:
    BoolOption(const char* name, bool& value) : value(value) {
        this->name = name;
    }
    BoolOption(const char* name, bool& value, bool canModify) : value(value) {
        this->name = name;
        this->canModify = canModify;
    }

    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

private:
    bool& value;
};

enum HexSize {
    CHAR = 1,
    SHORT = 2,
    INT = 3
};

class HexObserver : public StandardOption {
public:
    HexObserver(const char* name, u32* val, HexSize size) {
        this->name = name;
        this->canModify = false;
        this->size = size;
        this->value = value;
    }
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

private:
    const u32* value;
    HexSize size;
};

class ControlOption : public StandardOption {
public:
    ControlOption(const char* name, bool& value) : value(value) {
        this->name = name;
    }

    void modify(float amount);
    void select();
    void deselect();
    void render(TextPrinter* printer, char* buffer);

private:
    bool& value;
};

class StringOption : public StandardOption {
public:
    StringOption(const char* name, char* value) : value(value) {
        this->name = name;
        this->canModify = false;
    }

    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

private:
    char* value;
};

class LabelOption: public StandardOption {
    public:
        LabelOption(const char* name, const char* value): value(value) {
            this->name = name;
            this->canModify = false;
        }

        void modify(float amount) {};
        void render(TextPrinter* printer, char* buffer);
    private:
        const char* value;
};

class NamedIndexOption : public StandardOption {
public:
    NamedIndexOption(const char* name, const char** nameArray, int& index, int arrayLength) : index(index), nameArray(nameArray) {
        this->name = name;

        this->canModify = false;
        this->arrayLength = arrayLength;
    }

    void modify(float amount);
    void select();
    void deselect();
    void render(TextPrinter* printer, char* buffer);

private:
    const char** nameArray;
    int& index;
    int arrayLength;
};

class SubpageOption : public OptionType {
public:
    SubpageOption(const char* name) {
        this->name = name;
    }
    SubpageOption(const char* name, int height, int depth) {
        this->name = name;
        this->height = height;
        this->depth = depth;
    }
    SubpageOption(const char* name, bool collapsible) {
        this->name = name;
        this->collapsible = collapsible;
    }
    SubpageOption(const char* name, int height, int depth, bool collapsible) {
        this->name = name;
        this->height = height;
        this->depth = depth;
        this->collapsible = collapsible;
    }

    virtual void modify(float amount);
    virtual void select();
    virtual void deselect();
    virtual void render(TextPrinter* printer, char* buffer);
    virtual void down();
    virtual void up();
    virtual void setParentPage(Page* p);
    virtual bool isScalarOption() { return false; };
    virtual bool isFullySelected();

    bool hasCurrentOption() { 
      return !((currentOption < 0) || (currentOption >= (int)options.size())); 
    };
    OptionType& currentOptionRef() {return *options[currentOption]; }

    void addOption(OptionType* option);
    void clearOptions();
    void removeOptions();
    int getOptionCount();

    virtual ~SubpageOption() {
        options.clear();
    }

    vector<OptionType*> options = vector<OptionType*>();
    int& currentOption = this->_index;
    int _index = -1;
    char indent = 16;
    char scrollIdx = 0;
    int height = 10;
    char depth = 1;
    u32 modifiableChildren = 0;
    bool hasSelection = false;
    bool collapsible = false;
    bool collapsed = true;
    bool terminal = false;
};

class SpacerOption : public StandardOption {
public:
    SpacerOption() { canModify = false; }
    void render(TextPrinter* printer, char* buffer) {
        printer->printLine("");
    };
};

#pragma region templated
template <typename T>
void IntOption<T>::modify(float amount) {
    value += (T)amount;
    if (hasBounds && value > max) value = min;
    else if (hasBounds && value < min) value = max;
}

template <typename T>
void IntOption<T>::render(TextPrinter* printer, char* buffer) {
    if (asHex) {
      _sprintf(buffer, "%s: 0x%02X", name, value);
    } else {
      _sprintf(buffer, "%s: %02d", name, value);
    }
    printer->printLine(buffer);
}
#pragma endregion
#endif