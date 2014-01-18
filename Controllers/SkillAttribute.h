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
		private:
			SkillAttribute() {};
			SkillAttribute(const SkillAttribute&) {};
	};
}
