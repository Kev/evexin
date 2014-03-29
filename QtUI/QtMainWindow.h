/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QMainWindow>

#include <Eve-Xin/Controllers/Character.h>

class QComboBox;
class QTreeView;

namespace EveXin {
	class DataController;
	class QtCharacterList;
	class QtCharacterPane;
	class QtSkillModel;
	class QtSkillPlannerWidget;
	class QtMainWindow : public QMainWindow {
		Q_OBJECT
		public:
			QtMainWindow(boost::shared_ptr<DataController>);
			~QtMainWindow();

		private slots:
			void handleEditAPIKeys();
			void handleCharacterSelected(Character::ref);
		private:
			void handleCharacterListUpdated();
			void handleCharacterDataChanged();
			Character::ref selectedCharacter();
		private:
			void createMenus();
		private:
			QtCharacterList* characterList_;
			QtCharacterPane* characterPane_;
			boost::shared_ptr<DataController> dataController_;
			QWidget* apiWindow_;
			QTreeView* trainingPane_;
			QTreeView* skillPane_;
			boost::shared_ptr<QtSkillModel> trainingModel_;
			boost::shared_ptr<QtSkillModel> skillModel_;
			QtSkillPlannerWidget* skillPlannerWidget_;
			Character::ref currentCharacter_;
	};

}
