#pragma once
#include "pp/collections/vector.h"
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {
    
class SubpageOption : public OptionType {
public:
    SubpageOption(const char* name, bool collapsible, int height = 10, int depth = 0): currentOption(_index) {
        this->name = name;
        this->height = height;
        this->depth = depth;
        this->collapsible = collapsible;
        this->_index = -1;
        this->indent = 16;
        this->height = height;
        this->depth = depth;
        this->collapsed = collapsible;
    }

    virtual ~SubpageOption() {
        for (int i = 0; i < options.size(); i++) {
            delete reinterpret_cast<OptionType*>(options[i]);
        }
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

    Collections::vector options;
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
    };
};

}