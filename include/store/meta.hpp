/*
*   This file is part of NDS-Shop Project
*   Copyright (C) 2024-2025 NDS-Shop Team
*   
*   This program is a free open-source software that allows users
*	to browse and download digital products.  It is based on the
*	code of the Universal-Updater project from Universal-Team.
*   
*   It's distributed under the terms of the GNU General Public
*	License and it's completely free to use and modify.
*   
*   This program comes with no warranty, but we are constantly
*	working on improving its functionality and user experience.
*   
*   If you have any suggestions or find any bugs, please let us know!
*   
*   NDS-Shop Team reserves the right to update the license terms
*	at any time without prior notice.
*   Any changes to the code must be clearly marked as such to avoid confusion.
*/

#ifndef _NDS_SHOP_META_HPP
#define _NDS_SHOP_META_HPP

#include "json.hpp"
#include <string>
#include <vector>

enum favoriteMarks {
	STAR = 1 << 0,
	HEART = 1 << 1,
	DIAMOND = 1 << 2,
	CLUBS = 1 << 3,
	SPADE = 1 << 4
};

class Meta {
public:
	Meta();
	~Meta() { this->SaveCall(); };

	std::string GetUpdated(const std::string &unistoreName, const std::string &entry) const;
	int GetMarks(const std::string &unistoreName, const std::string &entry) const;
	bool UpdateAvailable(const std::string &unistoreName, const std::string &entry, const std::string &updated) const;
	std::vector<std::string> GetInstalled(const std::string &unistoreName, const std::string &entry) const;

	void SetUpdated(const std::string &unistoreName, const std::string &entry, const std::string &updated) {
		this->metadataJson[unistoreName][entry]["updated"] = updated;
	};

	void SetMarks(const std::string &unistoreName, const std::string &entry, int marks) {
		this->metadataJson[unistoreName][entry]["marks"] = marks;
	};

	/* TODO: Handle this better. */
	void SetInstalled(const std::string &unistoreName, const std::string &entry, const std::string &name) {
		const std::vector<std::string> installs = this->GetInstalled(unistoreName, entry);
		bool write = true;

		if (!installs.empty()) {
			write = !installs.empty();

			for (int i = 0; i < (int)installs.size(); i++) {
				if (installs[i] == name) {
					write = false;
					break;
				}
			}
		}

		if (write) this->metadataJson[unistoreName][entry]["installed"] += name;
	}

	/* Remove installed state from a download list entry. */
	void RemoveInstalled(const std::string &unistoreName, const std::string &entry, const std::string &name) {
		const std::vector<std::string> installs = this->GetInstalled(unistoreName, entry);
		if (installs.empty()) return;

		for (int i = 0; i < (int)installs.size(); i++) {
			if (installs[i] == name) {
				this->metadataJson[unistoreName][entry]["installed"].erase(i);
				break;
			}
		}

		if (this->metadataJson[unistoreName][entry]["installed"].empty() && this->metadataJson[unistoreName][entry].contains("updated")) this->metadataJson[unistoreName][entry].erase("updated");
	}

	void ImportMetadata();
	void SaveCall();
private:
	nlohmann::json metadataJson = nullptr;
};

#endif