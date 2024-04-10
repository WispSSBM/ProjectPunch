#include <ft/fighter.h>

#include "pp/fighter_names.h"

namespace ProjectPunch {
// TODO: Maybe some of these exist as fixed strings already in the brawl disc.
// F.e. several characters exist around 0x8059dfd8
const char* fighterName(ftKind kind) {
    switch (kind) {
    case (Fighter_Mario): return "Mario";
    case (Fighter_Donkey): return "DK";
    case (Fighter_Link): return "Link";
    case (Fighter_Samus): return "Samus";
    case (Fighter_Yoshi): return "Yoshi";
    case (Fighter_Kirby): return "Kirby";
    case (Fighter_Fox): return "Fox";
    case (Fighter_Pikachu): return "Pikachu";
    case (Fighter_Luigi): return "Luigi";
    case (Fighter_Captain): return "Cpt Falcon";
    case (Fighter_Ness): return "Ness";
    case (Fighter_Koopa): return "Bowser";
    case (Fighter_Peach): return "Peach";
    case (Fighter_Zelda): return "Zelda";
    case (Fighter_Sheik): return "Sheik";
    case (Fighter_Popo): return "Ice Climbers";
    case (Fighter_Nana): return "Ice Climbers";
    case (Fighter_Marth): return "Marth";
    case (Fighter_GameWatch): return "G&W";
    case (Fighter_Falco): return "Falco";
    case (Fighter_Ganon): return "Ganon";
    case (Fighter_Wario): return "Wario";
    case (Fighter_MetaKnight): return "Meta Knight";
    case (Fighter_Pit): return "Pit";
    case (Fighter_ZeroSuitSamus): return "ZSS";
    case (Fighter_Olimar): return "Olimar";
    case (Fighter_Lucas): return "Lucas";
    case (Fighter_Diddy): return "Diddy Kong";
    case (Fighter_PokemonTrainer): return "Pokemon Trainer";
    case (Fighter_PokeLizardon): return "Charizard";
    case (Fighter_PokeZenigame): return "Squirtle";
    case (Fighter_PokeFushigisou): return "Ivysaur";
    case (Fighter_Dedede): return "Dedede";
    case (Fighter_Lucario): return "Lucario";
    case (Fighter_Ike): return "Ike";
    case (Fighter_Robot): return "ROB";
    case (Fighter_PraMai): return "Pramai";
    case (Fighter_Purin): return "Jigglypuff";
    case (Fighter_Mewtwo): return "Mewtwo";
    case (Fighter_Roy): return "Roy";
    case (Fighter_DrMario): return "Doctor Mario";
    case (Fighter_ToonLink): return "Toon Link";
    case (Fighter_Dixie): return "Knuckles";
    case (Fighter_ToonSheik): return "Toon Sheik";
    case (Fighter_Wolf): return "Wolf";
    case (Fighter_Snake): return "Snake";
    case (Fighter_Sonic): return "Sonic";
    case (Fighter_GigaBowser): return "Giga Bowser";
    case (Fighter_WarioMan): return "Warioman";
    case (Fighter_Zako_Boy): return "Zakoboy";
    case (Fighter_Zako_Girl): return "Zakogirl";
    case (Fighter_Zako_Child): return "Zakochild";
    case (Fighter_Zako_Ball): return "Zakoball";
    case (Fighter_MarioD): return "MarioD";
    default:
        return "UNKNOWN";
    };
}
}