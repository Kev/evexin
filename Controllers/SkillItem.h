/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace EveXin {
	class SkillItem {
		public:
			typedef boost::shared_ptr<SkillItem> ref;
			SkillItem() {}
			virtual ~SkillItem() {}
	};
}
