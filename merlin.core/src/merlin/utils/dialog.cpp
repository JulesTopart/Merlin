#include "glpch.h"
#include "dialog.h"

#include <tinyfiledialogs.h>

namespace Merlin {

	std::string Dialog::saveFileDialog(FileType type) {
		int aNumOfFilterPatterns = 0;
		const char* aFilterPatterns[5];
		const char* aSingleFilterDescription;

		switch (type) {
		case FileType::IMAGE:
			aNumOfFilterPatterns = 2;
			aFilterPatterns[0] = "*.png";
			aFilterPatterns[1] = "*.jpg";
			aSingleFilterDescription = "Image files";
			break;
		case FileType::MESH:
			aNumOfFilterPatterns = 3;
			aFilterPatterns[0] = "*.obj";
			aFilterPatterns[1] = "*.stl";
			aFilterPatterns[2] = "*.3mf";
			aSingleFilterDescription = "Mesh files";
			break;
		case FileType::DATA:
			aNumOfFilterPatterns = 3;
			aFilterPatterns[0] = "*.csv";
			aFilterPatterns[1] = "*.json";
			aFilterPatterns[2] = "*.xml";
			aSingleFilterDescription = "Data files";
			break;
		case FileType::ALL:
		default:
			aNumOfFilterPatterns = 1;
			aFilterPatterns[0] = "*.*";
			aSingleFilterDescription = "All files";
			break;
		}

		const char* savePath = tinyfd_saveFileDialog("Save file", "./", aNumOfFilterPatterns, aFilterPatterns, aSingleFilterDescription);

		if (savePath) {
			return std::string(savePath);
		}
		else {
			return std::string();
		}
	}

	std::string Dialog::openFileDialog(FileType type) {
		int aNumOfFilterPatterns = 0;
		const char* aFilterPatterns[5];
		const char* aSingleFilterDescription;

		switch (type) {
		case FileType::IMAGE:
			aNumOfFilterPatterns = 2;
			aFilterPatterns[0] = "*.png";
			aFilterPatterns[1] = "*.jpg";
			aSingleFilterDescription = "Image files (.png, .jpg)";
			break;
		case FileType::MESH:
			aNumOfFilterPatterns = 3;
			aFilterPatterns[0] = "*.obj";
			aFilterPatterns[1] = "*.stl";
			aFilterPatterns[2] = "*.3mf";
			aSingleFilterDescription = "Mesh files (.obj, .stl, .3mf)";
			break;
		case FileType::DATA:
			aNumOfFilterPatterns = 3;
			aFilterPatterns[0] = "*.csv";
			aFilterPatterns[1] = "*.json";
			aFilterPatterns[2] = "*.xml";
			aSingleFilterDescription = "Data files (.csv, .json, .xml)";
			break;
		case FileType::ALL:
		default:
			aNumOfFilterPatterns = 1;
			aFilterPatterns[0] = "*.*";
			aSingleFilterDescription = "All files (.*)";
			break;
		}

		const char* openPath = tinyfd_openFileDialog("Open file", "./", aNumOfFilterPatterns, aFilterPatterns, aSingleFilterDescription, 0);

		if (openPath) {
			return std::string(openPath);
		}
		else {
			return std::string();
		}
	}

	std::string Dialog::selectFolderDialog() {
		const char* folderPath = tinyfd_selectFolderDialog("Select Folder", "./");

		if (folderPath) {
			return std::string(folderPath);
		}
		else {
			return std::string();
		}
	}

	int Dialog::alert(	const std::string& message,
						const std::string& title,
						IconType icon) {


		std::string iconStr = "";
		switch (icon) {
		case Dialog::IconType::INFO_ICON:
			iconStr = "info";
			break;
		case Dialog::IconType::WARNING_ICON:
			iconStr = "warning";
			break;
		case Dialog::IconType::ERROR_ICON:
			iconStr = "error";
			break;
		case Dialog::IconType::QUESTION_ICON:
			iconStr = "question";
			break;
		default:
			iconStr = "info";
			break;
		}

		return tinyfd_notifyPopup(title.c_str(), message.c_str(), iconStr.c_str());
	}

	int Dialog::messageBox(	const std::string& message, 
							const std::string& title, 
							MessageType type, IconType icon) {

		int defaultButton = 1;
		std::string iconStr = "";
		std::string typeStr = "";

		switch (type) {
		case Dialog::MessageType::OK:
			typeStr = "ok";
			break;
		case Dialog::MessageType::OK_CANCEL:
			typeStr = "okcancel";
			break;
		case Dialog::MessageType::YES_NO:
			typeStr = "yesno";
			break;
		case Dialog::MessageType::YES_NO_CANCEL:
			typeStr = "yesnocancel";
			break;
		default:
			typeStr = "ok";
			break;
		}

		switch (icon) {
		case Dialog::IconType::INFO_ICON:
			iconStr = "info";
			break;
		case Dialog::IconType::WARNING_ICON:
			iconStr = "warning";
			break;
		case Dialog::IconType::ERROR_ICON:
			iconStr = "error";
			break;
		case Dialog::IconType::QUESTION_ICON:
			iconStr = "question";
			break;
		default:
			iconStr = "info";
			break;
		}

		return tinyfd_messageBox(title.c_str(), 
			message.c_str(), 
			typeStr.c_str(), 
			iconStr.c_str(), 
			defaultButton);

	}

	std::string Dialog::inputBox(const std::string& message, const std::string& title, const std::string& defaultInput){
		const char* userInput = tinyfd_inputBox(title.c_str(), message.c_str(), defaultInput.c_str());

		if (userInput) {
			return std::string(userInput);
		}
		else {
			return std::string();
		}
	}
}