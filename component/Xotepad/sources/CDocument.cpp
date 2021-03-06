

#include "CDocument.hpp"
#include "WindowsUtils.hpp"

#include <iostream>

static int convertToken(const int lexer, const int token) {
    switch (lexer) {
    case SCLEX_CPP:
        switch (token) {
        case CLIKE_COMMENT: return SCE_C_COMMENT;
        case CLIKE_COMMENTLINE: return SCE_C_COMMENTLINE;
        case CLIKE_COMMENTDOC: return SCE_C_COMMENTLINEDOC;
        case CLIKE_NUMBER: return SCE_C_NUMBER;
        case CLIKE_WORD: return SCE_C_WORD;
        case CLIKE_STRING: return SCE_C_STRING;
        case CLIKE_CHARACTER: return SCE_C_CHARACTER;
        case CLIKE_UUID: return SCE_C_UUID;
        case CLIKE_PREPROCESSOR: return SCE_C_PREPROCESSOR;
        case CLIKE_OPERATOR: return SCE_C_OPERATOR;
        case CLIKE_IDENTIFIER: return SCE_C_IDENTIFIER;
        case CLIKE_STRINGEOL: return SCE_C_STRINGEOL;
        case CLIKE_VERBATIM: return SCE_C_VERBATIM;
        case CLIKE_REGEX: return SCE_C_REGEX;
        case CLIKE_COMMENTLINEDOC: return SCE_C_COMMENTLINEDOC;
        case CLIKE_WORD2: return SCE_C_WORD2;
        case CLIKE_COMMENTDOCKEYWORD: return SCE_C_COMMENTDOCKEYWORD;
        case CLIKE_COMMENTDOCKEYWORDERROR: return SCE_C_COMMENTDOCKEYWORDERROR;
        case CLIKE_GLOBALCLASS: return SCE_C_GLOBALCLASS;
        case CLIKE_STRINGRAW: return SCE_C_STRINGRAW;
        case CLIKE_TRIPLEVERBATIM: return SCE_C_TRIPLEVERBATIM;
        case CLIKE_HASHQUOTEDSTRING: return SCE_C_HASHQUOTEDSTRING;
        case CLIKE_PREPROCESSORCOMMENT: return SCE_C_PREPROCESSORCOMMENT;
        case CLIKE_PREPROCESSORCOMMENTDOC: return SCE_C_PREPROCESSORCOMMENTDOC;
        case CLIKE_USERLITERAL: return SCE_C_USERLITERAL;
        case CLIKE_TASKMARKER: return SCE_C_TASKMARKER;
        case CLIKE_ESCAPESEQUENCE: return SCE_C_ESCAPESEQUENCE;
        }

    case SCLEX_CMAKE:
        switch (token) {
        case CMAKE_DEFAULT: return SCE_CMAKE_DEFAULT;
        case CMAKE_COMMENT: return SCE_CMAKE_COMMENT;
        case CMAKE_STRINGDQ: return SCE_CMAKE_STRINGDQ;
        case CMAKE_STRINGLQ: return SCE_CMAKE_STRINGLQ;
        case CMAKE_STRINGRQ: return SCE_CMAKE_STRINGRQ;
        case CMAKE_COMMANDS: return SCE_CMAKE_COMMANDS;
        case CMAKE_PARAMETERS: return SCE_CMAKE_PARAMETERS;
        case CMAKE_VARIABLE: return SCE_CMAKE_VARIABLE;
        case CMAKE_USERDEFINED: return SCE_CMAKE_USERDEFINED;
        case CMAKE_WHILEDEF: return SCE_CMAKE_WHILEDEF;
        case CMAKE_FOREACHDEF: return SCE_CMAKE_FOREACHDEF;
        case CMAKE_IFDEFINEDEF: return SCE_CMAKE_IFDEFINEDEF;
        case CMAKE_MACRODEF: return SCE_CMAKE_MACRODEF;
        case CMAKE_STRINGVAR: return SCE_CMAKE_STRINGVAR;
        case CMAKE_NUMBER: return SCE_CMAKE_NUMBER;
        }
    }

    return 0;
}


static std::string join_string(const std::vector<std::string> &elements, char delimiter) {
    std::string result;

    for (std::size_t i=0; i<elements.size() - 1; i++) {
        result += elements[i] + delimiter;
    }

    result += elements[elements.size() - 1];

    return result;
}


void CDocument::setContent(const std::string &content) {
    this->SetText(content.c_str());
}


std::string CDocument::getContent() const {
    return this->GetText();
}


void CDocument::clearContent() {
    this->SetWindowText(L"");
}


void CDocument::setSelection(const TextSelection &selection) {
    this->SetSel(selection.start, selection.end, FALSE);
}


void CDocument::selectAll() {
    this->SetSel(0, -1, FALSE);
}


void CDocument::clearSelection() {
    this->SetSel(-1, -1, FALSE);
}


TextSelection CDocument::getSelection() const {
    TextSelection selection = {0, 0};

    this->GetSel(selection.start, selection.end);
    
    return selection;
}


void CDocument::undo() {
    this->Undo();
}


void CDocument::redo() {
    this->Redo();
}


void CDocument::cut() {
    this->Cut();
}


void CDocument::copy() {
    this->Copy();
}


void CDocument::paste() {
    this->Paste();
}


void CDocument::setFont(const Font &font) {
    /*
    HFONT fontHandle = CreateFont(
        font.size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH, widen(font.family).c_str()
    );

    if (fontHandle) {
        if (this->fontHandle) {
            DeleteObject(fontHandle);
        }
    }

    this->fontHandle = fontHandle;
    */

    std::cout << "HOladsa" << std::endl;
}


Font CDocument::getFont() const {
    // TODO: Font
    return {"", 12};
}


void CDocument::applyLexer(const LexerConfiguration &value) {
    const std::string joined_keywords = join_string(value.keywords, ' ');

    switch (value.lexer) {
    case Lexer::Clike: 
        this->SendCommand(SCI_STYLECLEARALL);
        this->SendCommand(SCI_SETLEXER, SCLEX_CPP);
        this->SendCommand(SCI_SETKEYWORDS, 0, (LPARAM)(joined_keywords.c_str()));
        
        for (const auto style : value.tokenStyle) {
            this->SendCommand(
                SCI_STYLESETFORE, 
                convertToken(SCLEX_CPP, style.tokenCode),
                RGB(style.color.r, style.color.g, style.color.b)
            );
        }

        // TODO: Refactor to bold
        this->SendCommand(SCI_STYLESETBOLD, SCE_C_WORD, 1);
        this->SendCommand(SCI_STYLESETBOLD, SCE_C_WORD2, 1);

        this->SendCommand(SCI_SETTABWIDTH, 4);
        this->SendCommand(SCI_SETUSETABS, 0);

        break;

    case Lexer::CMake:
        this->SendCommand(SCI_STYLECLEARALL);
        this->SendCommand(SCI_SETLEXER, SCLEX_CMAKE);
        this->SendCommand(SCI_SETKEYWORDS, 0, (LPARAM)(joined_keywords.c_str()));
        
        for (const auto style : value.tokenStyle) {
            this->SendCommand(
                SCI_STYLESETFORE, 
                convertToken(SCLEX_CMAKE, style.tokenCode),
                RGB(style.color.r, style.color.g, style.color.b)
            );
        }

        this->SendCommand(SCI_SETTABWIDTH, 4);
        this->SendCommand(SCI_SETUSETABS, 0);

        break;

    default:
        this->SendCommand(SCI_STYLECLEARALL);
        this->SendCommand(SCI_SETLEXER, SCLEX_NULL);

        break;
    }
}


LRESULT CDocument::SendCommand(UINT Msg, WPARAM wParam, LPARAM lParam) {
    return ::SendMessageW(m_hWnd, Msg, wParam, lParam);
}


LRESULT CDocument::SendCommand(UINT Msg, WPARAM wParam, LPARAM lParam) const {
    return ::SendMessageW(m_hWnd, Msg, wParam, lParam);
}


void CDocument::SetStyle(int style, COLORREF fore, COLORREF back, int size, const char *face) {
    this->SendCommand(SCI_STYLESETFORE, style, fore);
    this->SendCommand(SCI_STYLESETBACK, style, back);

    if (size >= 1) {
        this->SendCommand(SCI_STYLESETSIZE, style, size);
    }
        
    if (face) {
        this->SendCommand(SCI_STYLESETFONT, style, (LPARAM)face);
    }
}

    
void CDocument::SetStyle(const ScintillaStyle &style) {
    const char *face = nullptr;

    if (style.face != "") {
        face = style.face.c_str();
    }

    this->SetStyle(style.style, style.fore, style.back, style.size, face);
}


/*
void CScintilla::OnInitialUpdate() {
    this->SetAStyle(STYLE_DEFAULT, black, white, 10, "Courier New");
    this->SendEditor(SCI_STYLECLEARALL);

    this->SendEditor(SCI_SETLEXER, SCLEX_CPP);
    this->SendEditor(SCI_SETSTYLEBITS, 7);
    this->SendEditor(SCI_SETKEYWORDS, 0, (LPARAM)(cpp_keywords));
        
    this->SetAStyle(SCE_C_DEFAULT, black, white, 10, "Courier New");

    std::map<int, COLORREF> colors = {
        {SCE_C_COMMENT, RGB(0x00, 0x80, 0x00)}, 
        {SCE_C_COMMENTLINE, RGB(0x00, 0x80, 0x00)}, 
        {SCE_C_COMMENTDOC, RGB(0x00, 0x80, 0x00)}, 
        {SCE_C_NUMBER, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_WORD, RGB(0x00, 0x00, 0xFF)}, 
        {SCE_C_STRING, RGB(0x80, 0x00, 0x00)}, 
        {SCE_C_CHARACTER, RGB(0x80, 0x00, 0x00)}, 
        {SCE_C_UUID, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_PREPROCESSOR, RGB(0xA0, 0x00, 0xFF)}, 
        {SCE_C_OPERATOR, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_IDENTIFIER, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_STRINGEOL, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_VERBATIM, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_REGEX, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_COMMENTLINEDOC, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_WORD2, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_COMMENTDOCKEYWORD, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_COMMENTDOCKEYWORDERROR, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_GLOBALCLASS, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_STRINGRAW, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_TRIPLEVERBATIM, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_HASHQUOTEDSTRING, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_PREPROCESSORCOMMENT, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_PREPROCESSORCOMMENTDOC, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_USERLITERAL, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_TASKMARKER, RGB(0x00, 0x00, 0x00)}, 
        {SCE_C_ESCAPESEQUENCE, RGB(0x00, 0x00, 0x00)}
    };

    for (auto styleIt=colors.begin(); styleIt!=colors.end(); styleIt++) {
        this->SetAStyle(styleIt->first, styleIt->second);
    }

    this->SendEditor(SCI_STYLESETBOLD, SCE_C_WORD, 1);
    this->SendEditor(SCI_STYLESETBOLD, SCE_C_WORD2, 1);
}
*/

void CDocument::SetText(const std::string &text) {
    this->SendCommand(SCI_SETTEXT, 0, (LPARAM)text.c_str());
}


std::string CDocument::GetText() const {
    std::string content;
    const int length = (int)this->SendCommand(SCI_GETTEXTLENGTH) + 1;
    content.resize(length);
    this->SendCommand(SCI_GETTEXT, length, (LPARAM)content.c_str());

    return content;
}
    

void CDocument::SetSavePoint() {
    this->SendCommand(SCI_SETSAVEPOINT);
}


void CDocument::EmptyUndoBuffer() {
    this->SendCommand(SCI_EMPTYUNDOBUFFER);
}


void CDocument::ClearAll() { 
    this->SendCommand(SCI_CLEARALL);
}


void CDocument::SetFont(const std::string &name, const int size) {

}


void CDocument::SetTabWidth(const int spaces) {
    this->SendCommand(SCI_SETUSETABS, 0);
    this->SendCommand(SCI_SETTABWIDTH, spaces);
}


void CDocument::Undo() {
    this->SendCommand(SCI_UNDO);
}
    

void CDocument::Redo() {
    this->SendCommand(SCI_REDO);
}


void CDocument::Cut() {
    this->SendCommand(SCI_CUT);
}

    
void CDocument::Copy() {
    this->SendCommand(SCI_COPY);
}


void CDocument::Paste() {
    this->SendCommand(SCI_PASTE);
}


void CDocument::SetSel(int start, int end, BOOL _) {
    this->SendCommand(SCI_SETSEL, start, end);
}


void CDocument::GetSel(int &start, int &end) const {
    start = (int)this->SendCommand(SCI_GETSELECTIONSTART);
    end = (int)this->SendCommand(SCI_GETSELECTIONEND);
}


void CDocument::SetMarginType(const int margin, const int type) {
    this->SendCommand(SCI_SETMARGINTYPEN, margin, type);
}

    
void CDocument::SetMarginWidth(const int margin, const int pixelWidth) {
    this->SendCommand(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}


int CDocument::TextWidth(const int style, const std::string &text) {
    return (int) this->SendCommand(SCI_TEXTWIDTH, style, (LPARAM)text.c_str());
}


void CDocument::ApplyConfig(const ScintillaConfig &config) {
    this->SendCommand(SCI_SETLEXER, config.lexer);
    this->SendCommand(SCI_SETKEYWORDS, 0, (LPARAM)(config.keywords.c_str()));
    this->SendCommand(SCI_SETUSETABS, config.useTabs?1:0);
    this->SendCommand(SCI_SETTABWIDTH, config.tabWidth);

    for (const auto &style : config.styles) {
        this->SetStyle(style);
    }

    for (const auto &pair : config.colors) {
        this->SetStyle(pair.first, pair.second);
    }
}
