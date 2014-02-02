/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace EveXin {
	class SkillAttribute {
		public:
			enum Attribute {Intelligence, Memory, Charisma, Perception, Willpower, Unknown};

			static Attribute stringToAttribute(const std::string& string) {
				if (string == "intelligence") return Intelligence;
				if (string == "memory") return Memory;
				if (string == "charisma") return Charisma;
				if (string == "perception") return Perception;
				if (string == "willpower") return Willpower;
				return Unknown;
			}
			static std::string toString(Attribute attribute) {
				switch (attribute) {
					case Intelligence: return "Intelligence";
					case Memory: return "Memory";
					case Charisma: return "Charisma";
					case Perception: return "Perception";
					case Willpower: return "Willpower";
					case Unknown: return "ERROR";
				}
			}
			static std::vector<Attribute> allAttributes() {
				std::vector<Attribute> result;
				result.push_back(Charisma);
				result.push_back(Intelligence);
				result.push_back(Memory);
				result.push_back(Perception);
				result.push_back(Willpower);
				return result;
			}
		private:
			SkillAttribute() {}
			SkillAttribute(const SkillAttribute&) {}
	};
}
