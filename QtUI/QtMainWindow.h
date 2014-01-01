/*
 * Copyright (c) 2013 Kevin Smith
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
			void handleCharacterSelected(int index);
			private:
			void handleCharacterListUpdated();
			void handleCharacterDataUpdated(const std::string& id);
			Character::ref selectedCharacter();
		private:
			void createMenus();
		private:
			QComboBox* characterComboBox_;
			QtCharacterPane* characterPane_;
			boost::shared_ptr<DataController> dataController_;
			QWidget* apiWindow_;
			QTreeView* skillPane_;
			boost::shared_ptr<QtSkillModel> skillModel_;
			QtSkillPlannerWidget* skillPlannerWidget_;
	};

}
