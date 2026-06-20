#include "AppStyle.h"

QString AppStyle::styleSheet()
{
    return R"(
        QWidget {
            font-family: "Microsoft YaHei", "Segoe UI", Arial;
            font-size: 14px;
            color: #1f2933;
            background: #f4f7f5;
        }

        QMainWindow, QDialog {
            background: #f4f7f5;
        }

        QLabel#PageTitle {
            font-size: 22px;
            font-weight: 700;
            color: #173f2e;
            background: transparent;
        }

        QLabel#PageSubTitle {
            color: #63736b;
            background: transparent;
        }

        QLabel#BrandTitle {
            font-size: 19px;
            font-weight: 700;
            color: white;
            background: transparent;
        }

        QLabel#BrandSubTitle {
            color: #c8d8d0;
            background: transparent;
            font-size: 12px;
        }

        QGroupBox {
            background: white;
            border: 1px solid #dce5df;
            border-radius: 8px;
            margin-top: 14px;
            padding: 14px;
            font-weight: 600;
            color: #244b3a;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            background: transparent;
        }

        QLineEdit, QSpinBox, QComboBox, QDateEdit, QTextEdit {
            min-height: 30px;
            border: 1px solid #cfd9d2;
            border-radius: 6px;
            padding: 4px 8px;
            background: white;
            selection-background-color: #2e7d57;
        }

        QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QDateEdit:focus, QTextEdit:focus {
            border: 1px solid #2e7d57;
        }

        QPushButton {
            min-height: 30px;
            border: 1px solid #c7d4cc;
            border-radius: 6px;
            padding: 6px 14px;
            background: #ffffff;
            color: #244b3a;
        }

        QPushButton:hover {
            background: #edf6f0;
            border-color: #8fb7a0;
        }

        QPushButton:pressed {
            background: #dbece1;
        }

        QPushButton#PrimaryButton {
            background: #2e7d57;
            border-color: #2e7d57;
            color: white;
            font-weight: 600;
        }

        QPushButton#PrimaryButton:hover {
            background: #256b49;
        }

        QPushButton#DangerButton {
            background: #b94a48;
            border-color: #b94a48;
            color: white;
            font-weight: 600;
        }

        QPushButton#DangerButton:hover {
            background: #9f3d3b;
        }

        QTableWidget {
            background: white;
            border: 1px solid #dce5df;
            border-radius: 8px;
            gridline-color: #e7eee9;
            alternate-background-color: #f8fbf9;
            selection-background-color: #cfe6d8;
            selection-color: #123225;
        }

        QHeaderView::section {
            background: #e8f0eb;
            color: #244b3a;
            border: none;
            border-right: 1px solid #d8e2dc;
            padding: 8px;
            font-weight: 600;
        }

        QWidget#NavPane {
            background: #173f2e;
        }

        QPushButton#NavButton {
            color: #d9e8df;
            text-align: left;
            padding: 12px 18px;
            border: none;
            border-radius: 0;
            background: transparent;
            font-size: 15px;
            min-height: 34px;
        }

        QPushButton#NavButton:hover {
            background: #23543f;
            color: white;
        }

        QPushButton#NavButton:checked {
            background: #2e7d57;
            color: white;
            font-weight: 700;
            border-left: 4px solid #b8e0c6;
        }

        QWidget#TopBar {
            background: white;
            border-bottom: 1px solid #dce5df;
        }

        QFrame#StatCard {
            background: white;
            border: 1px solid #dce5df;
            border-radius: 8px;
        }

        QLabel#StatValue {
            font-size: 28px;
            font-weight: 800;
            color: #173f2e;
            background: transparent;
        }

        QLabel#StatName {
            color: #63736b;
            background: transparent;
            font-weight: 600;
        }
    )";
}
