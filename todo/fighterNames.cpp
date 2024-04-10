#include <Brawl/FT/Fighter.h>

#include "fighterNames.h"

// TODO: Maybe some of these exist as fixed strings already in the brawl disc.
// F.e. several characters exist around 0x8059dfd8
const char* fighterName(u32 kind) {
    switch (kind) {
    case (CHAR_ID::Mario): return "Mario";
    case (CHAR_ID::Donkey): return "DK";
    case (CHAR_ID::Link): return "Link";
    case (CHAR_ID::Samus): return "Samus";
    case (CHAR_ID::Yoshi): return "Yoshi";
    case (CHAR_ID::Kirby): return "Kirby";
    case (CHAR_ID::Fox): return "Fox";
    case (CHAR_ID::Pikachu): return "Pikachu";
    case (CHAR_ID::Luigi): return "Luigi";
    case (CHAR_ID::Captain): return "Cpt Falcon";
    case (CHAR_ID::Ness): return "Ness";
    case (CHAR_ID::Koopa): return "Bowser";
    case (CHAR_ID::Peach): return "Peach";
    case (CHAR_ID::Zelda): return "Zelda";
    case (CHAR_ID::Sheik): return "Sheik";
    case (CHAR_ID::Popo): return "Ice Climbers";
    case (CHAR_ID::Nana): return "Ice Climbers";
    case (CHAR_ID::Marth): return "Marth";
    case (CHAR_ID::Gamewatch): return "G&W";
    case (CHAR_ID::Falco): return "Falco";
    case (CHAR_ID::Ganon): return "Ganon";
    case (CHAR_ID::Wario): return "Wario";
    case (CHAR_ID::Metaknight): return "Meta Knight";
    case (CHAR_ID::Pit): return "Pit";
    case (CHAR_ID::Szerosuit): return "ZSS";
    case (CHAR_ID::Pikmin): return "Olimar";
    case (CHAR_ID::Lucas): return "Lucas";
    case (CHAR_ID::Diddy): return "Diddy Kong";
    case (CHAR_ID::Poketrainer): return "Pokemon Trainer";
    case (CHAR_ID::Pokelizardon): return "Charizard";
    case (CHAR_ID::Pokezenigame): return "Squirtle";
    case (CHAR_ID::Pokefushigisou): return "Ivysaur";
    case (CHAR_ID::Dedede): return "Dedede";
    case (CHAR_ID::Lucario): return "Lucario";
    case (CHAR_ID::Ike): return "Ike";
    case (CHAR_ID::Robot): return "ROB";
    case (CHAR_ID::Pramai): return "Pramai";
    case (CHAR_ID::Purin): return "Jigglypuff";
    case (CHAR_ID::Mewtwo): return "Mewtwo";
    case (CHAR_ID::Roy): return "Roy";
    case (CHAR_ID::DrMario): return "Doctor Mario";
    case (CHAR_ID::Toonlink): return "Toon Link";
    case (CHAR_ID::Knuckles): return "Knuckles";
    case (CHAR_ID::Toonsheik): return "Toon Sheik";
    case (CHAR_ID::Wolf): return "Wolf";
    case (CHAR_ID::Dixie): return "Knuckles";
    case (CHAR_ID::Snake): return "Snake";
    case (CHAR_ID::Sonic): return "Sonic";
    case (CHAR_ID::Gkoopa): return "Giga Bowser";
    case (CHAR_ID::Warioman): return "Warioman";
    case (CHAR_ID::Zakoboy): return "Zakoboy";
    case (CHAR_ID::Zakogirl): return "Zakogirl";
    case (CHAR_ID::Zakochild): return "Zakochild";
    case (CHAR_ID::Zakoball): return "Zakoball";
    case (CHAR_ID::Mariod): return "MarioD";
    default:
        return "UNKNOWN";
    };
}