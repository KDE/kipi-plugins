#ifndef SETUPCAMERA_H
#define SETUPCAMERA_H

#include <kdialogbase.h>

class QListView;
class QListViewItem;
class QPushButton;

class SetupCamera : public KDialogBase {
	Q_OBJECT

	public:

		SetupCamera(QWidget* parent = 0, const char* name = 0);
		~SetupCamera();
		void applySettings();

	private:
		QListView*    listView_;
		QPushButton* addButton_;
		QPushButton* removeButton_;
		QPushButton* editButton_;
		QPushButton* autoDetectButton_;

		private slots:
			void slotSelectionChanged();
		void slotAddCamera();
		void slotRemoveCamera();
		void slotEditCamera();
		void slotAutoDetectCamera();
		void slotAddedCamera(const QString& title, const QString& model, const QString& port,  const QString& path);
		void slotEditedCamera(const QString& title, const QString& model, const QString& port,  const QString& path);
		void slotOkClicked();
};

#endif 

