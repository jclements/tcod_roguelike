#ifndef MAIN_HPP
#define MAIN_HPP

#include "libtcod.hpp"
class Actor;	// because Destructible, Attacker, Ai have Actor* pointers,
							// but Actor also has pointers to them
#include "Persistent.hpp"
#include "Destructible.hpp"	// Actor* pointers are OK now because we have declared
#include "Attacker.hpp"			// at least the name of the class
#include "Ai.hpp"
#include "Pickable.hpp"
#include "Container.hpp"
#include "Actor.hpp"		// pointers to previous classes are fine, as well
#include "Map.hpp"
#include "Engine.hpp"
#include "Gui.hpp"

#endif
