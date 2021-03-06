
#include "MainWindowPresenter.hpp"

#include "MainWindow.hpp"
#include "FileService.hpp"
#include "WindowsUtils.hpp"
#include "LexerConfigurationService.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/optional/optional.hpp>


void MainWindowPresenter::attachView(MainWindow *view) {
    this->view = view;
    this->view->getDocument()->setFont(Font{"Consolas", 14});

    documentDirty = false;
    this->updateTitle();
}


void MainWindowPresenter::handleContentModified() {
    documentDirty = true;
    this->updateTitle();
}


AfterCheckAction MainWindowPresenter::checkDocumentChanges() {
    if (documentDirty) {
        auto result = view->showMessageBoxModal(
            appTitle.c_str(), 
            "Current file was modified. Do you want to save it?", 
            DialogButtons::YesNoCancel, 
            DialogIcon::Question
        );

        if (result == DialogResult::Cancel) {
            return AfterCheckAction::Stop;
        }

        if (result == DialogResult::Yes) {
            this->handleFileSave();
        }
    }

    return AfterCheckAction::Continue;
}


void MainWindowPresenter::handleFileNew() {
    if (auto action = this->checkDocumentChanges(); action == AfterCheckAction::Stop) {
        return;
    }

    this->newFile();
}


DialogUserOutcome MainWindowPresenter::handleFileSave() {
    if (documentFileName) {
        this->saveFile(*documentFileName, view->getDocument()->getModel()->getContent());

        return DialogUserOutcome::Accept;
    }

    return this->handleFileSaveAs();
}


std::vector<FileFilter> enumerateFilters() {
    return {
        FileFilter{"c++", "C/C++ Files", {"*.c", "*.cpp", "*.cxx", "*.c++", "*.cc", "*.h", "*.hpp", "*.hxx", "*.h++", "*.hh"}},
        FileFilter{"gl", "OpenGL Shader Files", {"*.glsl", "*.vert", "*.frag"}},
        FileFilter{"cl", "OpenCL C Files", {"*.cl"}},
        FileFilter{"cmake", "CMake Files", {"CMakeLists.txt", "*.cmake", "CMakeCache.txt"}},
        FileFilter{"", "All Files", {"*.*"}}
    };
}


boost::optional<FileFilter> matchFileFilter(const std::vector<FileFilter> &filters, const std::string &fileName) {
    for (const FileFilter &filter : filters) {
        for (const std::string &wildcard : filter.wildcards) {
            if (wildcard_match(wildcard.c_str(), fileName.c_str())) {
                return filter;
            }
        }
    }

    return {};
}


Lexer fileType2Lexer(const std::string &fileType) {
    if (fileType == "c++" || fileType == "gl" || fileType == "cl") {
        return Lexer::Clike;
    }

    if (fileType == "cmake") {
        return Lexer::CMake;
    }
    
    return Lexer::Text;
}


DialogUserOutcome MainWindowPresenter::handleFileSaveAs() {
    const std::vector<FileFilter> filters = enumerateFilters();

    if (auto result = view->showFilePickModal(FileDialog::Save, appTitle + " - Save File ...", filters); result) {
        const std::string fileName = *result;
        const std::string content = view->getDocument()->getModel()->getContent();

        this->saveFile(fileName, content);

        return DialogUserOutcome::Accept;
    }

    return DialogUserOutcome::Cancel;
}


void MainWindowPresenter::newFile() {
    view->getDocument()->getModel()->clearContent();

    documentFileName.reset();
    documentDirty = false;
    documentCount ++;

    this->updateTitle();
}


void MainWindowPresenter::handleFileOpen() {
    if (auto action = this->checkDocumentChanges(); action == AfterCheckAction::Stop) {
        return;
    }

    const std::vector<FileFilter> filters = enumerateFilters();

    if (auto fileName = view->showFilePickModal(FileDialog::Open, appTitle + " - Open File ...", filters)) {
        this->loadFile(*fileName);
    }
}


std::string MainWindowPresenter::getDocumentTitle() const {
    std::string title;

    if (documentFileName) {
        title = *documentFileName;
    } else {
        title = untitledTitle + " " + std::to_string(documentCount);
    }

    return title + (documentDirty ? "*" : "");
}


std::string MainWindowPresenter::computeTitle(const std::string &documentTitle) const {
    return appTitle + " - " + documentTitle;
}


void MainWindowPresenter::updateTitle() {
    view->setTitle(computeTitle(getDocumentTitle()).c_str());
}


void MainWindowPresenter::loadFile(const std::string &fileName) {
    const FileService fileService;
    const std::string content = fileService.loadFile(fileName.c_str());

    const auto fileFilter = matchFileFilter(enumerateFilters(), boost::filesystem::path(fileName).filename().string());
    const LexerConfigurationService lexerService;
    const auto lexerConfig = lexerService.getConfiguration(fileFilter->id);

    view->getDocument()->applyLexer(lexerConfig);
    view->getDocument()->getModel()->setContent(content);

    // FIXME: On Windows, setContent triggers a Notification for modified file.
    // so we set the internal variables here
    this->documentFileName = fileName;
    this->documentDirty = false;
    
    this->updateTitle();
}


void MainWindowPresenter::saveFile(const std::string &fileName, const std::string &content) {
    FileService fileService;
    fileService.saveFile(fileName.c_str(), content.c_str());

    this->documentFileName = fileName;
    this->documentDirty = false;
    
    this->updateTitle();
}


void MainWindowPresenter::handleFileExit() {
    this->handleCloseRequested();
}


void MainWindowPresenter::handleCloseRequested() {
    if (auto action = this->checkDocumentChanges(); action == AfterCheckAction::Stop) {
        return;
    }

    view->terminateApp();
}


void MainWindowPresenter::handleEditUndo() {
    view->getDocument()->getModel()->undo();
}


void MainWindowPresenter::handleEditRedo() {
    view->getDocument()->getModel()->redo();
}


void MainWindowPresenter::handleEditCut() {
    view->getDocument()->getModel()->cut();
}


void MainWindowPresenter::handleEditCopy() {
    view->getDocument()->getModel()->copy();
}


void MainWindowPresenter::handleEditPaste() {
    view->getDocument()->getModel()->paste();
}


void MainWindowPresenter::handleEditSelectAll() {
    view->getDocument()->selectAll();
}


void MainWindowPresenter::handleEditFindReplace() {
    findReplacePresenter.attachView(view->getFindReplace(), view->getDocument());
    findReplacePresenter.handleEditFindReplace();
}


void MainWindowPresenter::handleHelpAbout() {

}
