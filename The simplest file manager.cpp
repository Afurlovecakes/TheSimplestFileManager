#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <regex>
#include <limits>

namespace fs = std::filesystem;

class File {
public:
    File(const std::string& name) : name(name) {}
    std::string getName() const { return name; }
    virtual void displayInfo() const {
        std::cout << "File: " << name << std::endl;
    }
    virtual uintmax_t getSize() const {
        return fs::file_size(name);
    }
private:
    std::string name;
};

class Folder : public File {
public:
    Folder(const std::string& name) : File(name) {}
    void addFile(File* file) {
        files.push_back(file);
    }
    void displayInfo() const override {
        std::cout << "Folder: " << getName() << std::endl;
        for (const auto& file : files) {
            file->displayInfo();
        }
    }
    uintmax_t getSize() const override {
        uintmax_t size = 0;
        for (const auto& file : files) {
            size += file->getSize();
        }
        return size;
    }
private:
    std::vector<File*> files;
};

class FileManager {
public:
    void createFile(const std::string& name) {
        std::ofstream file(name);
        file.close();
        std::cout << "File created: " << name << std::endl;
    }
    void createFolder(const std::string& name) {
        fs::create_directory(name);
        std::cout << "Folder created: " << name << std::endl;
    }
    void deleteItem(const std::string& name) {
        if (fs::exists(name)) {
            fs::remove_all(name);
            std::cout << "Item deleted: " << name << std::endl;
        }
        else {
            std::cout << "Item not found: " << name << std::endl;
        }
    }
    void renameItem(const std::string& oldName, const std::string& newName) {
        if (fs::exists(oldName)) {
            fs::rename(oldName, newName);
            std::cout << "Item renamed from " << oldName << " to " << newName << std::endl;
        }
        else {
            std::cout << "Item not found: " << oldName << std::endl;
        }
    }
    void copyItem(const std::string& source, const std::string& destination) {
        if (fs::exists(source)) {
            fs::path destPath = destination;
            if (fs::is_directory(destination)) {
                destPath /= fs::path(source).filename();
            }
            fs::copy(source, destPath, fs::copy_options::recursive);
            std::cout << "Item copied from " << source << " to " << destPath << std::endl;
        }
        else {
            std::cout << "Source not found: " << source << std::endl;
        }
    }
    void moveItem(const std::string& source, const std::string& destination) {
        if (fs::exists(source)) {
            std::string target = destination;
            if (fs::is_directory(destination)) {
                target = destination + "/" + fs::path(source).filename().string();
            }
            fs::rename(source, target);
            std::cout << "Item moved from " << source << " to " << target << std::endl;
        }
        else {
            std::cout << "Source not found: " << source << std::endl;
        }
    }
    uintmax_t calcSize(const std::string& name) {
        if (fs::exists(name)) {
            if (fs::is_directory(name)) {
                return getDirSize(name);
            }
            else {
                return fs::file_size(name);
            }
        }
        else {
            std::cout << "Item not found: " << name << std::endl;
            return 0;
        }
    }
    void search(const std::string& pattern, const std::string& path) {
        try {
            if (fs::exists(path) && fs::is_directory(path)) {
                std::regex re(pattern);
                std::cout << "Searching for '" << pattern << "' in path: " << path << std::endl;
                for (const auto& entry : fs::recursive_directory_iterator(path)) {
                    std::cout << "Checking: " << entry.path().filename().string() << std::endl;
                    try {
                        if (std::regex_search(entry.path().filename().string(), re)) {
                            std::cout << "Match found: " << entry.path().string() << std::endl;
                        }
                    }
                    catch (const std::regex_error& e) {
                        std::cout << "Regex error: " << e.what() << std::endl;
                        break;
                    }
                }
            }
            else {
                std::cout << "Invalid path: " << path << std::endl;
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cout << "Filesystem error: " << e.what() << std::endl;
        }
        catch (const std::regex_error& e) {
            std::cout << "Regex error: " << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "An unknown error occurred." << std::endl;
        }
    }
    void displayContents() const {
        std::cout << "File Manager Contents: " << std::endl;
        for (const auto& entry : fs::directory_iterator(".")) {
            std::cout << entry.path().string() << std::endl;
        }
    }
private:
    uintmax_t getDirSize(const std::string& path) const {
        uintmax_t size = 0;
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (fs::is_regular_file(entry.status())) {
                size += fs::file_size(entry);
            }
        }
        return size;
    }
};

void displayMenu() {
    std::cout << "1. Display Contents" << std::endl;
    std::cout << "2. Create File" << std::endl;
    std::cout << "3. Create Folder" << std::endl;
    std::cout << "4. Delete File/Folder" << std::endl;
    std::cout << "5. Rename File/Folder" << std::endl;
    std::cout << "6. Copy File/Folder" << std::endl;
    std::cout << "7. Move File/Folder" << std::endl;
    std::cout << "8. Calculate Size" << std::endl;
    std::cout << "9. Search by Mask" << std::endl;
    std::cout << "0. Exit" << std::endl;
}

std::string getInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int main() {
    FileManager fm;
    int choice;
    std::string name, newName, source, destination, pattern, path;

    do {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистить буфер ввода после выбора чтения
        switch (choice) {
        case 1:
            fm.displayContents();
            break;
        case 2:
            name = getInput("Enter file name: ");
            fm.createFile(name);
            break;
        case 3:
            name = getInput("Enter folder name: ");
            fm.createFolder(name);
            break;
        case 4:
            name = getInput("Enter name of file/folder to delete: ");
            fm.deleteItem(name);
            break;
        case 5:
            name = getInput("Enter current name: ");
            newName = getInput("Enter new name: ");
            fm.renameItem(name, newName);
            break;
        case 6:
            source = getInput("Enter source name: ");
            destination = getInput("Enter destination name: ");
            fm.copyItem(source, destination);
            break;
        case 7:
            source = getInput("Enter source name: ");
            destination = getInput("Enter destination name: ");
            fm.moveItem(source, destination);
            break;
        case 8:
            name = getInput("Enter name to calculate size: ");
            std::cout << "Size: " << fm.calcSize(name) << " bytes" << std::endl;
            break;
        case 9:
            pattern = getInput("Enter search pattern: ");
            path = getInput("Enter path to search in: ");
            fm.search(pattern, path);
            break;
        case 0:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    } while (choice != 0);

    return 0;
}
