#pragma once

namespace Merlin {
	class Dialog{
	public :


		enum class MessageType {
			OK,
			OK_CANCEL,
			YES_NO,
			YES_NO_CANCEL
		};

		enum class IconType {
			INFO_ICON,
			WARNING_ICON,
			ERROR_ICON,
			QUESTION_ICON
		};

		enum class FileType {
			IMAGE,
			MESH,
			DATA,
			ALL
		};

		static std::string saveFileDialog(FileType type);
		static std::string openFileDialog(FileType type);
		static std::string selectFolderDialog();
		static int alert(const std::string& message, const std::string& title = "Info", IconType icon = IconType::INFO_ICON);
		static int messageBox(const std::string& message, const std::string& title = "Info", MessageType type = MessageType::OK, IconType icon = IconType::INFO_ICON);
		static std::string inputBox(const std::string& message, const std::string& title = "Input", const std::string& defaultInput = "");


	};
}
