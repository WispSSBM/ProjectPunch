#pragma once
// Wanton hosting of fudgepop's menu for my own purposes/modification

#include "pp/collections/vector.h"
#include "pp/graphics/text_printer.h"
#include <cstring>


namespace ProjectPunch {

using namespace ProjectPunch::Graphics;
using namespace ProjectPunch::Collections;

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

    OptionType() {
        name = "";
        parent = NULL;
        subParent = NULL;
        isSelected = false;
        canModify = true;
        terminal = true;
    }

    virtual ~OptionType() {};
    const char* name;
    Page* parent;
    SubpageOption* subParent;
    bool isSelected;
    bool canModify;
    bool terminal;
};

struct StandardOption : public OptionType {
    virtual void up() { };
    virtual void down() { };
    virtual void setParentPage(Page* p) { 
        OSReport("StandardOption setParentPage() this = 0x%0X\n", this);
        this->parent = p; 
    }
    virtual ~StandardOption() {}
};

struct Page {
    Page(Menu* myMenu) :
        menu(myMenu), options(*new vector()) {
            currentOption = 0;
            isSelected = false;
            highlightedOptionBottom = 0.f;
            highlightedOptionTop = 0.f;
            strcpy("generic page", title);
        };
    void addOption(OptionType* option);
    void hide();
    void up();
    void down();
    void modify(float amount);
    virtual void render(TextPrinter* printer, char* buffer);
    void saveHighlightRegion(TextPrinter* printer) {
        highlightedOptionTop = printer->charWriter->GetCursorY();
        highlightedOptionBottom = printer->charWriter->GetCursorY() + printer->lineHeight;
    };
    OptionType* getCurrentOption();
    virtual void show();
    virtual void select();
    virtual void deselect();
    virtual bool isFullySelected() { return reinterpret_cast<OptionType*>(options[currentOption])->isFullySelected(); };
    virtual const char* getTitle();
    Page(): options(*new vector()) {
        currentOption = 0;
        isSelected = false;
        highlightedOptionBottom = 0.f;
        highlightedOptionTop = 0.f;
        strcpy("generic page", title);
    }
    virtual ~Page() {}

    vector& options;
    char currentOption;
    bool isSelected;
    float highlightedOptionTop;
    float highlightedOptionBottom;
    Menu* menu;

    // TODO: Make this a title returning function.
    char title[256];
};

class Menu {
public:
    Menu() :
        highlightedColor(0xFFFFFFFF),
        selectedColor(0x3333FFFF),
        readOnlyColor(0xAAAAAAFF),
        defaultColor(0xCCCCCCFF),
        bgColor(0x555555CC),
        outlineColor(0x000000FF),
        highlightBoxColor(0x55FF55FF)
    {
        opacity = 0xFF;
        padding = 5.f;
        visible = false;
        paused = false;
        selected = false;
        currentPageIdx = -1;
    };

    void nextPage();
    void prevPage();
    void addPage(Page* p);
    Page* getCurrentPage();
    virtual void select();
    virtual void deselect();
    virtual void up();
    virtual void down();
    virtual void modify(float amount);
    virtual void render(TextPrinter* printer, char* buffer);
    virtual void unpause();
    virtual void toggle();

    bool visible;
    bool paused;
    bool selected;
    vector pages;

    u8 opacity;
    Color highlightedColor;
    Color selectedColor;
    Color readOnlyColor;
    Color defaultColor;
    Color bgColor;
    Color outlineColor;
    Color highlightBoxColor;

    float padding;
protected:
    int currentPageIdx;
    friend class Page;
};

template <typename T>
class IntOption : public StandardOption {
public:
    IntOption(const char* name, T& value) : value(value) {
        this->name = name;
        this->value = value;
        asHex = false;
        hasBounds = false;
    }
    IntOption(const char* name, T& value, T min, T max) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        asHex = false;
        hasBounds = true;
    }
    IntOption(const char* name, T& value, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->canModify = canModify;
        asHex = false;
        hasBounds = false;
    }
    IntOption(const char* name, T& value, T min, T max, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = canModify;
        asHex = false;
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
    bool hasBounds;
    bool asHex;
};

class FloatOption : public StandardOption {
public:
    FloatOption(const char* name, float& value) : value(value) {
        this->name = name;
        this->value = value;
        float max = NUMERIC_DEFAULT;
        float min = NUMERIC_DEFAULT;
        changeMultiplier = 1;
    }
    FloatOption(const char* name, float& value, float min, float max) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        changeMultiplier = 1;
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
        min = NUMERIC_DEFAULT;
        max = NUMERIC_DEFAULT;
        changeMultiplier = 1;
    }
    FloatOption(const char* name, float& value, float min, float max, bool canModify) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = canModify;
        changeMultiplier = 1;
    }
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

    ~FloatOption() {}

private:
    float& value;
    float max;
    float min;
    float changeMultiplier;
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
    SubpageOption(const char* name): currentOption(_index), options(*new vector()) {
        setDefaults();
        this->name = name;
    }
    SubpageOption(const char* name, int height, int depth): currentOption(_index), options(*new vector()) {
        setDefaults();
        this->name = name;
        this->height = height;
        this->depth = depth;
    }
    SubpageOption(const char* name, bool collapsible): currentOption(_index), options(*new vector()) {
        setDefaults();
        this->name = name;
        this->collapsible = collapsible;
    }
    SubpageOption(const char* name, int height, int depth, bool collapsible): currentOption(_index), options(*new vector()) {
        setDefaults();
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
    OptionType& currentOptionRef() {return *reinterpret_cast<OptionType*>(options[currentOption]); }

    void addOption(OptionType* option);
    void clearOptions();
    void removeOptions();
    int getOptionCount();

    virtual ~SubpageOption() {
        for (int i = 0; i < options.size(); i++) {
            delete reinterpret_cast<OptionType*>(options[i]);
        }

        options.clear();
        delete &options;
    }

    vector& options;
    int& currentOption;
    int _index;
    char indent;
    char scrollIdx;
    int height;
    char depth;
    u32 modifiableChildren;
    bool hasSelection;
    bool collapsible;
    bool collapsed;
    bool terminal;
private:
    void setDefaults() {
        _index = -1;
        indent = 16;
        scrollIdx = 0;
        height = 10;
        depth = 1;
        modifiableChildren = 0;
        hasSelection = false;
        collapsible = false;
        collapsed = true;
        terminal = false;
    };
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
      sprintf(buffer, "%s: 0x%02X", name, value);
    } else {
      sprintf(buffer, "%s: %02d", name, value);
    }
    printer->printLine(buffer);
}
#pragma endregion

} // namespace