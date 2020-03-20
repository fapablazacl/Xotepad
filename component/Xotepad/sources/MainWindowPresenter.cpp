
#include "MainWindowPresenter.hpp"
#include "MainWindow.hpp"
#include "FileService.hpp"


void MainWindowPresenter::attachView(MainWindow *view) {
    this->view = view;

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
        this->saveFile(*documentFileName, view->getContent());

        return DialogUserOutcome::Accept;
    }

    return this->handleFileSaveAs();
}


DialogUserOutcome MainWindowPresenter::handleFileSaveAs() {
    if (auto result = view->showFilePickModal(FileDialog::Save, appTitle + " - Save File ..."); result) {
        const std::string fileName = *result;
        const std::string content = view->getContent();

        this->saveFile(fileName, content);

        return DialogUserOutcome::Accept;
    }

    return DialogUserOutcome::Cancel;
}


void MainWindowPresenter::newFile() {
    view->clearContent();

    documentFileName.reset();
    documentDirty = false;
    documentCount ++;

    this->updateTitle();
}


void MainWindowPresenter::handleFileOpen() {
    if (auto action = this->checkDocumentChanges(); action == AfterCheckAction::Stop) {
        return;
    }

    if (auto fileName = view->showFilePickModal(FileDialog::Open, appTitle + " - Open File ...")) {
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

    this->documentFileName = fileName;
    this->documentDirty = false;
    
    view->setContent(content);
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
    view->undo();
}


void MainWindowPresenter::handleEditRedo() {
    view->redo();
}


void MainWindowPresenter::handleEditCut() {
    view->cut();
}


void MainWindowPresenter::handleEditCopy() {
    view->copy();
}


void MainWindowPresenter::handleEditPaste() {
    view->paste();
}


void MainWindowPresenter::handleEditSelectAll() {
    view->selectAll();
}


void MainWindowPresenter::handleEditFindReplace() {

}


void MainWindowPresenter::handleHelpAbout() {

}
