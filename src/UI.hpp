#ifndef UI_HPP
#define UI_HPP
#include "App.hpp"
#include "FileManager.hpp"
#include "Terminal++.hpp"

class UI {
    App& app;
    // int tWidth{};
    // int tHeight{};

    // todo: change
    static Color getColor(const fs::directory_entry& entry) {
        if (entry.is_directory())
            return Color::Blue;
        if (FileManager::isExecutable(entry.path()))
            return Color::Red;
        if (entry.is_regular_file())
            return Color::Green;
        return Color::White;
    }

    static std::string getIcon(const fs::directory_entry& entry) {
        if (entry.is_directory())
            return " ";
        if (FileManager::isExecutable(entry.path()))
            return "  ";
        if (entry.path().filename().extension().string() == ".txt")
            return " ";
        if (entry.is_regular_file())
            return " ";
        return "? ";
    }

    static void printEntry(const fs::directory_entry& entry, const bool& highlight = false) {
        const Color color = getColor(entry);
        Terminal t;
        t.setTextColor(color).setStyle(Style::Bold);

        if (highlight)
            t.setBackgroundColor(color).setTextColor(Color::Black).setStyle(Style::Bold);

        std::string entryStr = getIcon(entry) + FileManager::getName(entry).string();

        if (static_cast<int>(entryStr.size()) >= App::getSelectionWidth())     // limiting the number of characters
            entryStr = entryStr.substr(0, App::getSelectionWidth() - 5) + "~"; // TODO: remove magic number

        t.print(" ", entryStr, std::setw(App::getSelectionWidth() - static_cast<int>(entryStr.size())), " ");
        Terminal(Color::Reset).println();
    }

public:
    UI()
        : app(App::getInstance()) {
        Terminal::setTitle("BFilex");
        Terminal::clearScreen();
        Terminal::hideCursor();
    }

    ~UI() {
        Terminal::showCursor();
    }

    void renderTopBar() const {
        Terminal::moveTo(1, 1); // todo: consider removing if not needed

        std::string topBar = "$ " + (fs::current_path() / app.getEntries()[app.fileIndex].path().filename()).string();
        const auto lastSlashIndex = topBar.find_last_of('/');

        if (auto [tWidth,tHeight] = Terminal::size(); topBar.size() > tWidth)
            topBar = topBar.substr(0, tWidth - 1) + "~";

        Terminal(Color::Blue)
                .setStyle(Style::Bold).print(
                    topBar.substr(0, std::min(lastSlashIndex + 1, topBar.size()))
                );

        if (lastSlashIndex < static_cast<int>(topBar.length()))
            Terminal().println(topBar.substr(lastSlashIndex + 1));
    }

    void renderEntries() const {
        Terminal::moveTo(1, 2); // todo: consider removing if not needed
        for (int i = 0; i < app.getEntries().size(); ++i)
            printEntry(app.getEntries()[i], i == app.fileIndex);
    }

    void renderFooter() const {
        const auto lastWriteTime = FileManager::getLastWriteTime(app.getEntries()[app.fileIndex]);

        Terminal::moveTo(1, Terminal::size().height);
        Terminal().print(
            app.getEntries()[app.fileIndex].is_directory() ? "d" : ".",      // print file type
            FileManager::getPermissionsStr(app.getEntries()[app.fileIndex]), // print permissions
            "  ",
            std::put_time(std::localtime(&lastWriteTime), "%a %b %e %r %Y") // Print formatted time
        );

        const std::string directoryNumber =
                " " + std::to_string(app.fileIndex + 1) +
                "/" + std::to_string(static_cast<int>(app.getEntries().size()));

        auto [tWidth,tHeight] = Terminal::size();

        Terminal::moveTo(tWidth - static_cast<int>(directoryNumber.length()) + 1, tHeight);
        Terminal().print(directoryNumber);
    }

    void renderApp() const {
        Terminal::clearScreen();
        renderTopBar();
        renderEntries();
        renderFooter();
        Terminal::flush();
    }

    // void setDimensions(const int& width, const int& height) {
    //     app.setSelectionWidth(width / 2);
    // }
};

#endif //UI_HPP
