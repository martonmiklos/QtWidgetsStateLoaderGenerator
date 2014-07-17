QtWidgetsStateLoaderGenerator
=============================

The QtWidgetsStateLoaderGenerator is a tool for autogenerating methods for saving and loading the states from the ui files of a widgets in Qt widget based application through the QSettings class.

Project scope:
- The tool will parse the ui files for looking up for the widgets and will provide a user interface to change the following options for each widget:
    - Section: the given widget will be saved to the given section
        - Deeper sections will be able to created by separating the sections with '/'
    - Key name: by default it will be the widget's name, but could be changed by the user.
                The program will ensure that in a section there should not be two widgets with the same name.
    - Skip save/load: the given widget will be excluded from the selected opertation

- The tool should have an XML based file format for saving/loading files.
- These "project files" should be updated by reparsing the same ui file.
    - Upon a new UI file import the following cases should be handled:
        - Widgets not presented in the UI file should be removed from project automatically
        - New widgets should be automatically added to the project with the default settings
    - A command line interface will be provided to being able to integrate it to a build process.

- The following options will be provided before the generation:
    - Output file name: a cpp file where the methods will be generated
    - Class name: by default it will be the name of the class tag of the ui file.
    - Save and load method names (loadUI and saveUI by default)

- Usage of the generated code
    - Define the save and load methods in your class declaration.
    - Include the generated cpp file to your project.
    - Use the generated methods in your constructor and destructor.

- Currently supported widgets:
    QCheckBox
    QRadioButton
    QPushButton
    QLineEdit
    QPlainTextEdit <sup>*</sup>
    QTextEdit <sup>*</sup>
    QSpinBox
    QDoubleSpinBox
    QSlider
    QDial
    QDateEdit
    QDateTimeEdit
    QTimeEdit
    QComboBox

*: QPlainText saved as plain text, use QTextEdit to save rich text
