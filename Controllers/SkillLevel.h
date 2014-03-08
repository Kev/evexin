/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <Eve-Xin/Controllers/SkillItem.h>

namespace EveXin {
	class Skill;
	class SkillLevel : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillLevel> ref;
			/* -1 means fully trained */
			SkillLevel(boost::shared_ptr<SkillItem> parent, boost::shared_ptr<Skill> skill, int level, int pointsTrained = -1, const boost::posix_time::ptime& startTime = boost::posix_time::not_a_date_time, const boost::posix_time::ptime& endTime = boost::posix_time::not_a_date_time);

			int getLevel() const;
			int getSkillPointsForLevel() const;
			int getSkillPointsTrained() const;
			int getRemainingSkillPoints() const;
			bool isComplete() const;
			boost::posix_time::ptime getStartTime() const;
			boost::posix_time::ptime getEndTime() const;

		private:
			int rawLevelPoints(int level) const;
		private:
			int level_;
			int skillPoints_;
			int skillPointsNeeded_;
			boost::posix_time::ptime startTime_;
			boost::posix_time::ptime endTime_;
	};
}
